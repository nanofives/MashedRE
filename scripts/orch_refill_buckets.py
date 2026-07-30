#!/usr/bin/env python3
"""Build read-fleet candidate buckets for the orchestrator ledger.

Source pool: re/analysis/plans/state_eligible_CLEAN.txt (state-lane eligible
RVAs, tab-separated: rva, size_bytes, subsystem).

Filters applied here (each one is a documented project fact, not a guess):
  - keep only rows still at confidence C2 in hooks.csv (nothing already C3/C4);
  - drop the three library bands that are tagged, not ported:
      libpng/zlib      0x00516000..0x00529fff   (memory: library-skip bands)
      MSVC CRT         0x005c0000..0x005c8000   (memory: library-skip bands)
      qhull/RW-Physics 0x0057c5b0..0x005a5820   (memory: qhull_rwphysics_island)
    The D3DX9 PSGP band (0x004ec000..0x004fc9e0) is deliberately NOT dropped —
    0x004f8660/0x004f8690 were promoted out of it on orch-iter4.

Ordering: within each subsystem, smallest body first (cheapest to brief+author).

Writes re/orchestrator/candidate_buckets.json. Does NOT touch state.json —
add the ledger items with orch.ps1 add so orch.ps1 stays the only writer.
"""
import csv
import json
import pathlib
import sys
from collections import defaultdict

ROOT = pathlib.Path(__file__).resolve().parents[1]
POOL = ROOT / "re/analysis/plans/state_eligible_CLEAN.txt"
HOOKS = ROOT / "hooks.csv"
OUT = ROOT / "re/orchestrator/candidate_buckets.json"

LIBRARY_BANDS = [
    (0x00516000, 0x00529FFF, "libpng/zlib"),
    (0x005C0000, 0x005C8000, "msvc-crt"),
    (0x0057C5B0, 0x005A5820, "qhull/rwphysics"),
]

BUCKET_SIZE = 12
# subsystem -> how many buckets to cut. Weighted by pool depth.
PLAN = [
    ("render", 3), ("audio", 2), ("gameplay", 1), ("particle", 1),
    ("boot", 1), ("util", 1), ("vehicle", 1), ("hud", 1), ("frontend", 1),
]


def in_library_band(va):
    return next((n for lo, hi, n in LIBRARY_BANDS if lo <= va <= hi), None)


def load_confidence():
    lvl = {}
    with HOOKS.open(newline="", encoding="utf-8", errors="replace") as f:
        for row in csv.DictReader(f):
            rva = (row.get("rva") or "").strip()
            if not rva or rva.startswith("#"):
                continue
            try:
                va = int(rva, 16)
            except ValueError:
                continue
            if va < 0x400000:  # 21 util rows store file offsets, not VAs
                va += 0x400000
            lvl[va] = (row.get("confidence") or "").strip()
    return lvl


def main():
    lvl = load_confidence()
    by_sub = defaultdict(list)
    skipped_band = 0
    for line in POOL.read_text(encoding="utf-8").splitlines():
        parts = line.split("\t")
        if len(parts) < 3:
            continue
        va = int(parts[0], 16)
        size, sub = int(parts[1]), parts[2].strip()
        if lvl.get(va) != "C2":
            continue
        if in_library_band(va):
            skipped_band += 1
            continue
        by_sub[sub].append({"rva": "0x%08x" % va, "size": size})

    for rows in by_sub.values():
        rows.sort(key=lambda r: (r["size"], r["rva"]))

    buckets = []
    for sub, n in PLAN:
        pool = by_sub.get(sub, [])
        for i in range(n):
            chunk = pool[i * BUCKET_SIZE:(i + 1) * BUCKET_SIZE]
            if not chunk:
                break
            buckets.append({
                "id": "state_%s_b%d" % (sub, i + 1),
                "subsystem": sub,
                "count": len(chunk),
                "size_range": [chunk[0]["size"], chunk[-1]["size"]],
                "rvas": [r["rva"] for r in chunk],
            })

    OUT.write_text(json.dumps({
        "source": "re/analysis/plans/state_eligible_CLEAN.txt",
        "filters": "hooks.csv confidence==C2; library bands dropped",
        "pool_c2_after_filters": sum(len(v) for v in by_sub.values()),
        "skipped_library_band": skipped_band,
        "bucket_size": BUCKET_SIZE,
        "buckets": buckets,
    }, indent=2) + "\n", encoding="utf-8")

    print("pool after filters: %d  (dropped %d library-band)"
          % (sum(len(v) for v in by_sub.values()), skipped_band))
    for b in buckets:
        print("  %-22s %2d rvas  size %d..%d"
              % (b["id"], b["count"], b["size_range"][0], b["size_range"][1]))
    print("wrote %s" % OUT.relative_to(ROOT))
    return 0


if __name__ == "__main__":
    sys.exit(main())
