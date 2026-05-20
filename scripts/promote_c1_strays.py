#!/usr/bin/env python3
"""Promote 7 stray C0 rows to C1 + apply subsystem/note refinements.

After bucket_c1_strays investigation (2026-05-20). 8 C0 rows existed in
hooks.csv from past batches that filed stub-rows without authoring per-RVA
.md plates. The investigation found that 7 are real game-code functions
deserving C1 (decomp evidence in re/analysis/bucket_c1_strays/), and 1
(0x005aa1e0) is a listing-level-only inline callback with no function
body — stays at C0 with a refined note.

Also applies two subsystem reclassifications + three note refinements
discovered during the investigation.
"""

from __future__ import annotations

import csv
import re
import shutil
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
HOOKS = ROOT / "hooks.csv"

# RVAs to promote C0 -> C1, with optional subsystem reclassification + note refinement.
# Format: rva -> (new_subsystem_or_None_to_keep, new_note)
PROMOTIONS = {
    "00473870": (
        None,
        "7-param textured-quad draw; 11 frontend callers; builds 4-vertex "
        "TRIANGLEFAN in DAT_00898a20 via DAT_007d3ff8 vtable",
    ),
    "00458630": (
        None,
        "14-case powerup-type-to-sprite-name switch; delegates to FUN_004c5c00",
    ),
    "004736c0": (
        None,
        "9-param per-corner-colored quad (sibling of 0x00473870; same vertex "
        "buffer + renderstate sequence); previous note 'line/border renderer' "
        "REFINED to 'per-corner colored quad'",
    ),
    "00474e60": (
        None,
        "10-byte float->float10 multiply leaf; sole caller FUN_00434720",
    ),
    "004391b0": (
        None,
        "4-case sprite-name resolver; returns handle from FUN_0040bb90 "
        "('Star' literal in cases 1/3); previous note 'sprite draw' REFINED "
        "to 'sprite-name resolver'",
    ),
    "004927c0": (
        None,
        "12-state race-progression FSM; inline-init pair-index table "
        "{0,2,1,3,0,3,1,2,0,1,0,1}; 8 gate-pair globals at 0x007f10dc/"
        "0x007f1044; 624-byte terminal clear at 0x007f0a40",
    ),
    "004c4dc0": (
        "util",  # SUBSYSTEM RECLASSIFICATION: vehicle -> util (it's RwMatrixInvert)
        "RwMatrixInvert (identity fast-path + orthonormal 3x3-transpose fast-"
        "path + FUN_004c4eb0 general fallback); 23 callers span entire code-"
        "base; previous subsystem 'vehicle' REFINED to 'util' (general-purpose "
        "RenderWare math primitive, not vehicle-specific)",
    ),
}

# RVAs to keep at C0 with an updated note.
STAY_C0 = {
    "005aa1e0": (
        None,
        "Listing-level-only inline callback (LAB_, no function envelope); "
        "26-byte inline strcmp-style equality comparator passed as function "
        "pointer to FUN_005aa0c0; CANNOT promote to C1 — no Ghidra function "
        "body; CONFIRMED 2026-05-20 by bucket_c1_strays investigation",
    ),
}

BUCKET_FILE_PATH = "re/analysis/bucket_c1_strays"


def main() -> int:
    backup = HOOKS.with_suffix(".csv.bak.c1_strays")
    shutil.copy2(HOOKS, backup)
    print(f"Backup at {backup.relative_to(ROOT)}")

    rows: list[list[str]] = []
    with open(HOOKS, encoding="utf-8-sig", newline="") as f:
        rdr = csv.reader(f)
        for row in rdr:
            rows.append(row)

    hex8 = re.compile(r"^(?:0x)?([0-9a-fA-F]{8})$")
    n_promoted = 0
    n_stay_c0 = 0
    n_subsystem_changed = 0

    for row in rows:
        if not row:
            continue
        m = hex8.match(row[0].strip())
        if not m:
            continue
        rva = m.group(1).lower()

        if rva in PROMOTIONS:
            new_sub, new_note = PROMOTIONS[rva]
            # row schema: rva, name, subsystem, confidence, status, file, scenario, frida_diff, notes
            while len(row) < 9:
                row.append("")
            old_sub = row[2].strip()
            row[3] = "C1"  # confidence
            row[4] = "mapped"  # status
            row[5] = f"{BUCKET_FILE_PATH}/0x{rva}.md"  # file
            row[8] = new_note  # notes
            if new_sub is not None and new_sub != old_sub:
                row[2] = new_sub
                n_subsystem_changed += 1
                print(f"  0x{rva} subsystem: {old_sub} -> {new_sub}")
            n_promoted += 1
            print(f"  0x{rva} promoted C0 -> C1")
            continue

        if rva in STAY_C0:
            _new_sub, new_note = STAY_C0[rva]
            while len(row) < 9:
                row.append("")
            row[5] = f"{BUCKET_FILE_PATH}/0x{rva}.md"  # file
            row[8] = new_note
            n_stay_c0 += 1
            print(f"  0x{rva} stays C0 (listing-level-only) — note refined")
            continue

    with open(HOOKS, "w", encoding="utf-8", newline="") as f:
        w = csv.writer(f, lineterminator="\n")
        for row in rows:
            w.writerow(row)

    print()
    print(f"Promoted C0 -> C1:        {n_promoted}")
    print(f"Subsystem reclassified:   {n_subsystem_changed}")
    print(f"Stay at C0 (note refined):{n_stay_c0}")
    print(f"Wrote {HOOKS.relative_to(ROOT)}")
    return 0


if __name__ == "__main__":
    import sys
    sys.exit(main())
