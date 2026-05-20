#!/usr/bin/env python3
"""frida-sweep-20260520-1620: apply integration-diff results to hooks.csv.

5 RVAs verified GREEN by integration diff -> promote C2 -> C3.
1 RVA falsely claimed C3 by s4 worker (no Frida access in-session due to
the wiped original/ — see feedback_never_touch_original_dir.md) -> demote
C3 -> C2 with RED-evidence note.
2 RVAs stay C2 with RED note appended.

Idempotent.
"""

from __future__ import annotations

import csv
import re
import shutil
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
HOOKS = ROOT / "hooks.csv"

# RVAs verified GREEN by the integration diff -> promote C2 -> C3.
PROMOTE_C2_TO_C3 = {
    "0042b960": (
        "Frontend/MenuButtonDetect.cpp",
        "C3 via frida-sweep-20260520-1620 integration diff GREEN (10/10); "
        "CarSlotInit1P; queued by c3-batch-k-s1 (pre-existing reimpl)",
    ),
    "0040bb50": (
        "Frontend/SpriteCluster_k2.cpp",
        "C3 via frida-sweep-20260520-1620 integration diff GREEN (10/10); "
        "SpriteLookupC; c3-batch-k-s2",
    ),
    "00408a50": (
        "Frontend/Leaves_k3.cpp",
        "C3 via frida-sweep-20260520-1620 integration diff GREEN (10/10); "
        "PerCarRaceProgressGet; c3-batch-k-s3",
    ),
    "0040e340": (
        "Util/UtilLeaves_k3.cpp",
        "C3 via frida-sweep-20260520-1620 integration diff GREEN (10/10); "
        "GetLiveCarCount; c3-batch-k-s3",
    ),
    "0040e370": (
        "Util/UtilLeaves_k3.cpp",
        "C3 via frida-sweep-20260520-1620 integration diff GREEN (10/10); "
        "IsCarSlotActive; c3-batch-k-s3",
    ),
}

# False-claimed C3 (s4 worker promoted in-session while original/ was wiped;
# integration diff confirms RED with None=None pattern -> harness gap, not
# a verified promotion).
DEMOTE_C3_TO_C2 = {
    "00552750": (
        "mashedmod/src/mashed_re/HUD/TextCluster_k4.cpp",
        "DEMOTED C3->C2 by frida-sweep-20260520-1620; s4 worker claimed "
        "GREEN in-session but had no Frida access (original/ was wiped at "
        "the time); integration diff RED 10/10 with orig=None/reimpl=None "
        "pattern (likely arg_type='none' + crash_equal_ok=True interaction). "
        "Reimpl source kept in TextCluster_k4.cpp for future fix",
    ),
}

# Stayed C2 with RED integration diff -> annotate but don't promote.
ANNOTATE_RED_C2 = {
    "00430b90": (
        "ProgressBarSetA c3-batch-k-s2 RED in frida-sweep-20260520-1620 "
        "(KeyError: 'lut_root_delta' missing from hooks_registry entry; "
        "schema fix needed before re-attempt)",
    ),
    "004c5c00": (
        "LinkedListStringSearch c3-batch-k-s2 RED in frida-sweep-20260520-1620 "
        "(orig=None/reimpl=None on all 10 vectors; 2-pointer signature may "
        "need different arg_type than what was registered)",
    ),
}


def main() -> int:
    backup = HOOKS.with_suffix(".csv.bak.frida_sweep_k")
    shutil.copy2(HOOKS, backup)
    print(f"Backup at {backup.relative_to(ROOT)}")

    hex8 = re.compile(r"^(?:0x)?([0-9a-fA-F]{8})$")
    rows: list[list[str]] = []
    with open(HOOKS, encoding="utf-8-sig", newline="") as f:
        for row in csv.reader(f):
            rows.append(row)

    n_promoted = 0
    n_demoted = 0
    n_annotated = 0

    for row in rows:
        if not row:
            continue
        m = hex8.match(row[0].strip())
        if not m:
            continue
        rva = m.group(1).lower()

        while len(row) < 9:
            row.append("")

        if rva in PROMOTE_C2_TO_C3:
            file_path, note = PROMOTE_C2_TO_C3[rva]
            row[3] = "C3"
            row[5] = f"mashedmod/src/mashed_re/{file_path}"
            row[8] = note
            n_promoted += 1
            print(f"  0x{rva}: PROMOTE C2->C3 ({file_path})")

        elif rva in DEMOTE_C3_TO_C2:
            file_path, note = DEMOTE_C3_TO_C2[rva]
            row[3] = "C2"
            row[5] = file_path
            row[8] = note
            n_demoted += 1
            print(f"  0x{rva}: DEMOTE C3->C2 (RED in integration diff)")

        elif rva in ANNOTATE_RED_C2:
            note = ANNOTATE_RED_C2[rva]
            row[8] = note
            n_annotated += 1
            print(f"  0x{rva}: ANNOTATE C2 with RED note")

    with open(HOOKS, "w", encoding="utf-8", newline="") as f:
        w = csv.writer(f, lineterminator="\n")
        for row in rows:
            w.writerow(row)

    print()
    print(f"Promoted C2->C3:           {n_promoted}")
    print(f"Demoted C3->C2 (false claim): {n_demoted}")
    print(f"Annotated C2 with RED note: {n_annotated}")
    print(f"Wrote {HOOKS.relative_to(ROOT)}")
    return 0


if __name__ == "__main__":
    import sys
    sys.exit(main())
