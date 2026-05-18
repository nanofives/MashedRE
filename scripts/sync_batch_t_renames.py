#!/usr/bin/env python3
"""Sync the 29 RW renames the ghidra-sweep applied to master Ghidra into
hooks.csv. Each row's `name` column is updated in-place from FUN_XXXXXXXX
to the canonical Rw*/Rp* symbol. All other columns are preserved.

The rename data is hard-coded from the post-sweep master Ghidra query
(function_list with Rp/Rw prefixes, filtered by RVA range to batch_t).
"""

from __future__ import annotations

import csv
import re
import shutil
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
HOOKS = ROOT / "hooks.csv"

RENAMES: dict[str, str] = {
    # s3 — RpWorld family (7)
    "004e4810": "RpWorldAddLight",
    "004e4860": "RpWorldRemoveLight",
    "004e5300": "RpWorldInstance",
    "004e5700": "RpWorldDestroy",
    "004e5850": "RpWorldCreate",
    "004e5c30": "RpWorldForAllAtomics",
    "004e5c70": "RpWorldForAllWorldSectors",
    # s3 — RpLight family (4)
    "004e4b90": "RpLightSetConeAngle",
    "004e4c30": "RpLightStreamRead",
    "004e4dd0": "RpLightCreate",
    "004e4ec0": "RpLightDestroy",
    # s3 — RpAtomic family (5)
    "004e6100": "RpAtomicGetWorldBoundingSphere",
    "004e6470": "RpAtomicDestroy",
    "004e67b0": "RpAtomicCreate",
    "004e7060": "RpAtomicStreamRead",
    "004e7d40": "RpAtomicRegisterPlugin",
    # s3 — RpClump family (5)
    "004e66d0": "RpClumpForAllAtomics",
    "004e6760": "RpClumpForAllLights",
    "004e6e00": "RpClumpDestroy",
    "004e7420": "RpClumpStreamRead",
    "004e7d70": "RpClumpRegisterPlugin",
    # s3 — RwFrame family (4)
    "004e4450": "RwFrameAddChild",
    "004e45b0": "RwFrameRemoveChild",
    "004e4f70": "RwFrameDestroy",
    "004e5020": "RwFrameDestroyHierarchy",
    # s3 — RwEngine (1)
    "004e5660": "RwEngineForAllPlugins",
    # s6 — RwV3d family (3)
    "005cb000": "RwV3dTransformPoints",
    "005cb07f": "RwV3dTransformVectors",
    "005cb0ef": "RwV3dTransformPoint",
}


def main() -> int:
    assert len(RENAMES) == 29, f"expected 29 renames, got {len(RENAMES)}"
    print(f"Syncing {len(RENAMES)} renames into hooks.csv...")

    # Read all rows
    rows: list[list[str]] = []
    with open(HOOKS, encoding="utf-8-sig", newline="") as f:
        rdr = csv.reader(f)
        for r in rdr:
            rows.append(r)

    hex8 = re.compile(r"^([0-9a-fA-F]{8})$")
    updated = 0
    seen_rvas: set[str] = set()
    for r in rows:
        if not r:
            continue
        m = hex8.match(r[0].strip())
        if not m:
            continue
        rva = m.group(1).lower()
        if rva in RENAMES:
            new_name = RENAMES[rva]
            if len(r) >= 2 and r[1] != new_name:
                old_name = r[1]
                r[1] = new_name
                updated += 1
                seen_rvas.add(rva)
                print(f"  0x{rva}: {old_name} -> {new_name}")

    missing = set(RENAMES.keys()) - seen_rvas
    if missing:
        print(f"\nWARNING — {len(missing)} renames have no hooks.csv row:")
        for rva in sorted(missing):
            print(f"  0x{rva}  (would have been: {RENAMES[rva]})")

    # Backup + write
    shutil.copy2(HOOKS, str(HOOKS) + ".bak.sync_renames")
    with open(HOOKS, "w", encoding="utf-8", newline="") as f:
        w = csv.writer(f, lineterminator="\n")
        for r in rows:
            w.writerow(r)
    print(f"\nUpdated {updated} rows in {HOOKS.relative_to(ROOT)}")
    print(f"Backup at {(HOOKS.name + '.bak.sync_renames')}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
