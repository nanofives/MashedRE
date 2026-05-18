#!/usr/bin/env python3
"""Harvest batch_t plate frontmatter -> append C1 rows to hooks.csv.

Run AFTER ghidra-sweep has drained the SCRIBE_QUEUE rows. Each plate in
re/analysis/bucket_*/ becomes one hooks.csv row with confidence=C1,
status=mapped, scenario=batch-t-s<N>, session_date=2026-05-18.

Subsystem assignment is bucket-level (one token per bucket, matching the
existing hooks.csv vocabulary). The `notes` field carries the multi-cluster
detail. RVAs already present in hooks.csv are skipped and reported as
drift cases.

Output:
- Appends to hooks.csv (one row per new plated RVA)
- Prints summary: added / skipped (drift) / not-found
"""

from __future__ import annotations

import csv
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
HOOKS = ROOT / "hooks.csv"

BUCKET_INFO: dict[str, tuple[str, str, str]] = {
    # bucket -> (session_id, subsystem token, note)
    "bucket_00405400": ("batch-t-s1", "gameplay",
                        "batch_t s1 MULTI[copters-helicopter-npcs,projectile-smoke,ai-pathfinding,led-loader,score-rank,hud-vfx]"),
    "bucket_0048ad50": ("batch-t-s2", "particle",
                        "batch_t s2 MULTI[particles-effects,directshow-ibasefilter]"),
    "bucket_004e1ce0": ("batch-t-s3", "render",
                        "batch_t s3 RW scenegraph CORE (RpWorld/RpClump/RpAtomic/RpLight/RwFrame); RW 3.7.0.2 version anchor; 26 RW renames applied in master Ghidra"),
    "bucket_00583f10": ("batch-t-s5", "util",
                        "batch_t s5 third-party qhull 2002.1 library-residue; library-tag drain candidate"),
    "bucket_005c7070": ("batch-t-s6", "audio",
                        "batch_t s6 MULTI[audio-dll-mixer,rw-v3d-vector,msvc-seh-funclets,crt-fiddb]; 3 RW V3dTransform renames at 0x005cb000/07f/0ef"),
}


def parse_plate(path: Path) -> dict | None:
    """Return frontmatter dict, or None if not a valid plate."""
    try:
        txt = path.read_text(encoding="utf-8")
    except Exception:
        return None
    if not txt.startswith("---"):
        return None
    end = txt.find("\n---", 3)
    if end == -1:
        return None
    fm = txt[3:end].strip()
    data = {}
    for line in fm.splitlines():
        line = line.rstrip()
        if not line or line.startswith("-") or line.startswith(" "):
            continue
        if ":" in line:
            k, _, v = line.partition(":")
            data[k.strip()] = v.strip()
    return data


def load_existing_rvas() -> set[str]:
    seen: set[str] = set()
    hex8 = re.compile(r"^(?:0x)?([0-9a-fA-F]{8})$")
    with open(HOOKS, encoding="utf-8-sig") as f:
        rdr = csv.reader(f)
        for r in rdr:
            if not r:
                continue
            m = hex8.match(r[0].strip())
            if m:
                seen.add(m.group(1).lower())
    return seen


def main() -> int:
    existing = load_existing_rvas()
    print(f"Existing hooks.csv RVAs: {len(existing)}")

    added: list[list[str]] = []
    drift: list[tuple[str, str]] = []  # (rva, bucket)
    no_data: list[Path] = []

    for bucket, (session_id, subsystem, note) in BUCKET_INFO.items():
        bdir = ROOT / "re" / "analysis" / bucket
        if not bdir.exists():
            print(f"  bucket missing: {bdir}")
            continue
        plates = sorted(p for p in bdir.glob("*.md")
                        if not p.name.startswith("_"))
        for plate in plates:
            data = parse_plate(plate)
            if not data:
                no_data.append(plate)
                continue
            rva_raw = data.get("rva", "")
            m = re.search(r"([0-9a-fA-F]{8})", rva_raw)
            if not m:
                no_data.append(plate)
                continue
            rva = m.group(1).lower()
            if rva in existing:
                drift.append((rva, bucket))
                continue
            name = data.get("name_in_ghidra") or data.get("name") or f"FUN_{rva}"
            # Build hooks.csv row (9 cols: rva,name,subsystem,confidence,status,file,scenario,frida_diff,notes)
            row = [
                rva,
                name,
                subsystem,
                "C1",
                "mapped",
                f"re/analysis/{bucket}/{plate.name}",
                session_id,
                "",  # no frida_diff at C1
                note,
            ]
            added.append(row)

    print(f"\nPlates parsed:    {len(added) + len(drift) + len(no_data)}")
    print(f"  new rows:       {len(added)}")
    print(f"  drift skipped:  {len(drift)}")
    print(f"  no-frontmatter: {len(no_data)}")

    if drift:
        print(f"\nDrift cases (RVAs already in hooks.csv):")
        for rva, bucket in drift:
            print(f"  0x{rva} (in {bucket})")
    if no_data:
        print(f"\nNo-frontmatter files (sample):")
        for p in no_data[:5]:
            print(f"  {p.relative_to(ROOT)}")

    # Append
    if added:
        with open(HOOKS, "a", encoding="utf-8", newline="") as f:
            w = csv.writer(f, lineterminator="\n")
            for r in added:
                w.writerow(r)
        print(f"\nAppended {len(added)} rows to {HOOKS.relative_to(ROOT)}")
    else:
        print(f"\nNo rows appended.")

    return 0


if __name__ == "__main__":
    sys.exit(main())
