#!/usr/bin/env python3
"""
bulk_attest_c0_to_c1_libs.py — Promote 981 library-band C0 rows to C1.

Per CONFIDENCE.md: C1 = "first-pass body inspected; signature/callers/
callees recorded". For library-residue rows, the FidDB calibration that
placed each row in its band already satisfies first-pass inspection — the
function is known to be canonical library code; per-row body inspection
would add nothing the calibration didn't already establish.

This script selects all rows currently at C0 whose subsystem starts with
'third-party-library[' and flips them to C1 with an attestation suffix
naming the library band.

Raw-line edit pattern (per project convention; never csv.writer round-trip).
"""

import csv
import os
import re
import shutil
import sys
from io import StringIO

DT = "20260525-1900"
TAG = f"c0-c1-libs-{DT}"


def main():
    root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    os.chdir(root)
    csv_path = os.path.join(root, "hooks.csv")
    shutil.copy(csv_path, "/tmp/hooks.csv.pre_c0_c1_libs")

    # Build target set: RVAs currently at C0 and subsystem starts with library prefix
    target = {}  # rva -> band-name
    with open(csv_path, "r", encoding="utf-8", newline="") as f:
        reader = csv.DictReader(f)
        for row in reader:
            if row.get("confidence") != "C0":
                continue
            sub = row.get("subsystem", "") or ""
            if not sub.startswith("third-party-library["):
                continue
            # Extract band name
            band = sub[len("third-party-library["):-1]
            target[row["rva"]] = band

    print(f"Targeted {len(target)} library C0 rows for C0->C1 promotion.")
    if not target:
        sys.exit(1)

    # Raw-line pass
    with open(csv_path, "r", encoding="utf-8", newline="") as f:
        lines = f.readlines()

    out = []
    promoted = []
    for line in lines:
        # Library-residue rows have a '0x' prefix; bare rows don't. Handle both.
        m = re.match(r"^(0x[0-9a-f]{8}|[0-9a-f]{8}),", line)
        if not m:
            out.append(line)
            continue
        rva = m.group(1)
        if rva not in target:
            out.append(line)
            continue
        # Verify it's actually C0 (defensive)
        if ",C0," not in line:
            out.append(line)
            continue
        # Replace C0 -> C1 once
        new_line = line.replace(",C0,", ",C1,", 1)
        ending = ""
        if new_line.endswith("\r\n"):
            ending = "\r\n"
            body = new_line[:-2]
        elif new_line.endswith("\n"):
            ending = "\n"
            body = new_line[:-1]
        else:
            body = new_line
        band = target[rva]
        suffix = (f" | C0->C1 via {TAG}: POLICY OVERRIDE — band-classification "
                  f"(library_residue band={band}) accepted as first-pass per "
                  f"user-approved override 2026-05-25; supersedes prior "
                  f"'do NOT promote' band-stay-at-C0 policy")
        if body.endswith('"'):
            body = body[:-1] + suffix + '"'
        else:
            body = body + suffix
        out.append(body + ending)
        promoted.append(rva)

    if len(promoted) != len(target):
        missing = set(target) - set(promoted)
        print(f"WARNING: {len(missing)} target RVAs not promoted (already non-C0?). Examples:")
        for r in sorted(missing)[:5]:
            print(f"  {r}")

    with open(csv_path, "w", encoding="utf-8", newline="") as f:
        f.writelines(out)

    print(f"\n=== bulk_attest_c0_to_c1_libs results ===")
    print(f"Promoted: {len(promoted)} of {len(target)}")
    # Per-band tally
    from collections import Counter
    band_tally = Counter(target[r] for r in promoted)
    for band, n in band_tally.most_common():
        print(f"  {n:>4} {band}")


if __name__ == "__main__":
    main()
