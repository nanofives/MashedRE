#!/usr/bin/env python3
"""Harvest batch_x plate frontmatter -> hooks.csv C1 rows.

Run AFTER ghidra-sweep-20260519-1404 has drained the 6 SCRIBE_QUEUE rows
into master Ghidra. Each plate becomes one hooks.csv row at C1 / mapped.

Buckets covered:
- batch-x-s1 bucket_00452ec0 (80 plates) -> gameplay (projectile-and-pickup-pools)
- batch-x-s2 bucket_004b4a80 (80 plates) -> TRIMODAL: per-plate classification
    * Lua VM markers (luaV_/luaA_/TValue/L->top/l_gt/etc.) -> third-party-library[lua-5.0]
    * 0x004b6b10 __stricmp FidDB-attested CRT thunk        -> third-party-library[msvc-crt]
    * everything else                                       -> render
- batch-x-s3 bucket_004c4270 (80 plates) -> render (RW Image/Texture/Raster + D3D9 + IM3D)
- batch-x-s4 bucket_00549580 (80 plates) -> render (RpPatch+RtFS+Rt2d+anim+image-loader)
- batch-x-s5 bucket_005a6f30 (80 plates) -> audio   (audio-channel-meta)
- batch-x-s6 bucket_005bba60 (80 plates) -> audio   (RwaDSRenderer DirectShow filter)

Output:
- Appends to hooks.csv (one row per new plated RVA)
- Prints summary: per-bucket added / drift-skipped / no-frontmatter
"""

from __future__ import annotations

import csv
import re
import shutil
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
HOOKS = ROOT / "hooks.csv"

# -----------------------------------------------------------------------------
# Plate buckets. For TRIMODAL s2, subsystem is "MIXED" (sentinel) and a
# classify_s2_plate() helper assigns per-plate.
# -----------------------------------------------------------------------------

PLATE_BUCKETS: dict[str, tuple[str, str, str]] = {
    "bucket_00452ec0": (
        "batch-x-s1",
        "gameplay",
        "batch_x s1 projectile-and-pickup-pools; per-slot reset/lookup-release "
        "helpers around small gameplay pools (depthcharge/MP-vehicle-adjacent)",
    ),
    "bucket_004b4a80": (
        "batch-x-s2",
        "MIXED",  # sentinel — handled per-plate
        "",       # per-plate note
    ),
    "bucket_004c4270": (
        "batch-x-s3",
        "render",
        "batch_x s3 RW Image/Texture/Raster + D3D9 + IM3D; many plates carry "
        "direct RW catalog names (RwImageCreate, RwCameraFrustumTestSphere, etc.)",
    ),
    "bucket_00549580": (
        "batch-x-s4",
        "render",
        "batch_x s4 MIXED RW-plugin code; RpPatch + RtFS + Rt2d + anim + "
        "image-loader; mostly RenderWare plugin glue",
    ),
    "bucket_005a6f30": (
        "batch-x-s5",
        "audio",
        "batch_x s5 audio-channel-meta-cluster; per-channel state + mixer "
        "callbacks around the central audio object at DAT_007dca34",
    ),
    "bucket_005bba60": (
        "batch-x-s6",
        "audio",
        "batch_x s6 RwaDSRenderer DirectShow filter; audio backend implemented "
        "via DirectShow filter graph (RW audio over DS)",
    ),
}

# Per-plate s2 classification: precompute Lua-marker set + the one CRT RVA.
LUA_VM_PATTERN = re.compile(
    r"luaV_|luaA_|luaH_|luaO_|luaS_|luaK_|luaD_|luaG_|"
    r"TValue|L->top|l_gt|lua_State|TString|lua_Number|GCObject|"
    r"Tobject|Lua VM|lua-5\.0|lua_pushvalue|StkId"
)

S2_CRT_RVA = "004b6b10"  # FidDB-attested __stricmp thunk

S2_LUA_NOTE = (
    "library_residue lua-5.0 (s2 batch_x; luaV_/luaA_/TValue/L->top/l_gt "
    "markers); static-linked third-party Lua 5.0 VM"
)
S2_CRT_NOTE = (
    "library_residue msvc-crt (s2 batch_x; __stricmp FidDB-attested thunk); "
    "static-linked MSVC CRT residue"
)
S2_RENDER_NOTE = (
    "batch_x s2 game viewport/raster/Im2D helpers (mixed bucket; non-Lua/non-CRT "
    "plates); matches batch_w s2 render style"
)


def classify_s2(plate_path: Path) -> tuple[str, str]:
    """Returns (subsystem, note) for a single s2 plate."""
    rva_match = re.match(r"0x([0-9a-fA-F]{8})\.md$", plate_path.name)
    if rva_match and rva_match.group(1).lower() == S2_CRT_RVA:
        return ("third-party-library[msvc-crt]", S2_CRT_NOTE)
    try:
        txt = plate_path.read_text(encoding="utf-8")
    except Exception:
        txt = ""
    if LUA_VM_PATTERN.search(txt):
        return ("third-party-library[lua-5.0]", S2_LUA_NOTE)
    return ("render", S2_RENDER_NOTE)


# -----------------------------------------------------------------------------
# Plate parser
# -----------------------------------------------------------------------------


def parse_plate(path: Path) -> dict | None:
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
    data: dict[str, str] = {}
    for line in fm.splitlines():
        line = line.rstrip()
        if not line or line.startswith("-") or line.startswith(" "):
            continue
        if ":" in line:
            k, _, v = line.partition(":")
            data[k.strip()] = v.strip()
    return data


def load_existing_rvas() -> tuple[set[str], dict[str, str]]:
    """Returns (set of RVA, map of RVA -> existing confidence)."""
    seen: set[str] = set()
    conf: dict[str, str] = {}
    hex8 = re.compile(r"^(?:0x)?([0-9a-fA-F]{8})$")
    with open(HOOKS, encoding="utf-8-sig") as f:
        rdr = csv.reader(f)
        for r in rdr:
            if not r:
                continue
            m = hex8.match(r[0].strip())
            if m:
                rva = m.group(1).lower()
                seen.add(rva)
                if len(r) >= 4:
                    conf[rva] = r[3].strip()
    return seen, conf


def main() -> int:
    existing, existing_conf = load_existing_rvas()
    print(f"hooks.csv: {len(existing)} unique mapped RVAs (pre-batch_x)")

    added: list[list[str]] = []
    per_bucket_added: dict[str, int] = {}
    s2_breakdown: dict[str, int] = {}  # subsystem -> count
    drift_higher: list[tuple[str, str, str]] = []
    drift_equal_or_lower: list[tuple[str, str, str]] = []
    no_data: list[Path] = []

    def emit(row: list[str], bucket_tag: str) -> None:
        added.append(row)
        per_bucket_added[bucket_tag] = per_bucket_added.get(bucket_tag, 0) + 1

    for bucket, (session_id, subsystem, default_note) in PLATE_BUCKETS.items():
        bdir = ROOT / "re" / "analysis" / bucket
        if not bdir.exists():
            print(f"  bucket missing: {bdir}")
            continue
        plates = sorted(
            p for p in bdir.glob("*.md")
            if not p.name.startswith("_") and p.name.lower() != "readme.md"
        )
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
                ec = existing_conf.get(rva, "")
                if ec in ("C2", "C3", "C4"):
                    drift_higher.append((rva, bucket, ec))
                    continue
                drift_equal_or_lower.append((rva, bucket, ec))
                continue

            # Determine subsystem + note for this plate.
            if subsystem == "MIXED":
                sub, note = classify_s2(plate)
                s2_breakdown[sub] = s2_breakdown.get(sub, 0) + 1
            else:
                sub, note = subsystem, default_note

            name = (
                data.get("proposed_name")
                or data.get("name_in_ghidra")
                or data.get("name")
                or f"FUN_{rva}"
            )

            file_col = f"re/analysis/{bucket}/{plate.name}"
            row = [
                rva, name, sub, "C1", "mapped",
                file_col, session_id, "", note,
            ]
            emit(row, bucket)

    # Dedupe newly-added against itself.
    seen_added: set[str] = set()
    deduped: list[list[str]] = []
    for r in added:
        if r[0] in seen_added:
            continue
        seen_added.add(r[0])
        deduped.append(r)

    print(f"\n=== Per-bucket added counts ===")
    for k in sorted(per_bucket_added):
        print(f"  {k}: {per_bucket_added[k]}")
    total = sum(per_bucket_added.values())
    print(f"  TOTAL: {total} (deduped-internal: {len(deduped)})")

    if s2_breakdown:
        print(f"\n=== s2 trimodal breakdown ===")
        for k in sorted(s2_breakdown):
            print(f"  {k}: {s2_breakdown[k]}")

    if drift_higher:
        print(f"\n=== Drift cases - already at C2+ ({len(drift_higher)}) ===")
        for rva, bucket, ec in drift_higher[:20]:
            print(f"  0x{rva} (in {bucket}, existing={ec}) -> SKIP")
        if len(drift_higher) > 20:
            print(f"  ... + {len(drift_higher) - 20} more")
    if drift_equal_or_lower:
        print(f"\n=== Drift cases - already at <=C1 ({len(drift_equal_or_lower)}) ===")
        for rva, bucket, ec in drift_equal_or_lower[:20]:
            print(f"  0x{rva} (in {bucket}, existing={ec or 'blank'}) -> SKIP (row exists)")
        if len(drift_equal_or_lower) > 20:
            print(f"  ... + {len(drift_equal_or_lower) - 20} more")
    if no_data:
        print(f"\n=== No-frontmatter / missing plates ({len(no_data)}) ===")
        for p in no_data[:5]:
            try:
                print(f"  {p.relative_to(ROOT)}")
            except ValueError:
                print(f"  {p}")

    if deduped:
        backup = HOOKS.with_suffix(".csv.bak.batch_x")
        shutil.copy2(HOOKS, backup)
        new_path = HOOKS.with_suffix(".csv.new")
        with open(HOOKS, encoding="utf-8-sig") as src, open(
            new_path, "w", encoding="utf-8", newline=""
        ) as dst:
            dst.write(src.read())
            w = csv.writer(dst, lineterminator="\n")
            for r in deduped:
                w.writerow(r)
        new_path.replace(HOOKS)
        print(f"\nAppended {len(deduped)} rows to {HOOKS.relative_to(ROOT)}")
        print(f"Backup at {backup.relative_to(ROOT)}")
    else:
        print("\nNo rows appended.")

    new_existing, _ = load_existing_rvas()
    pct_pre = 100.0 * len(existing) / 5800
    pct_post = 100.0 * len(new_existing) / 5800
    print(f"\nCoverage: {len(existing)} -> {len(new_existing)} unique RVAs of 5800")
    print(f"          {pct_pre:.1f}% -> {pct_post:.1f}% mapped")

    return 0


if __name__ == "__main__":
    sys.exit(main())
