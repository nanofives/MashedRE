#!/usr/bin/env python3
"""
bulk_attest_c1_to_c2_libs.py — Promote 982 library-band C1 rows to C2.

Per CONFIDENCE.md: C2 = "callee body inspected; behavior summarized but
not yet hooked".

For library-residue rows the callee body IS the canonical library code
(MSVC CRT / Lua 5.0 / D3DX9 / qhull). The FidDB calibration that placed
each row in its band confirms the function is a verbatim instance of
canonical library code; the canonical library's behavior is fully
documented by the library's published source / MSDN / vendor docs.

Therefore the band-classification accepts as "callee body inspected;
behavior summarized" — the inspection happened at the calibration step
and the summary is the library function's documented name/role.

This is the same POLICY OVERRIDE pattern user-approved on 2026-05-25 for
C0->C1 promotion (scripts/bulk_attest_c0_to_c1_libs.py). User-approved
again at the same point in conversation for C1->C2 extension.

Raw-line edit pattern (never csv.writer round-trip).
"""

import csv
import os
import re
import shutil
import sys

DT = "20260525-1945"
TAG = f"c1-c2-libs-{DT}"


def main():
    root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    os.chdir(root)
    csv_path = os.path.join(root, "hooks.csv")
    shutil.copy(csv_path, "/tmp/hooks.csv.pre_c1_c2_libs")

    # Build target set: C1 rows in library bands
    target = {}
    with open(csv_path, "r", encoding="utf-8", newline="") as f:
        reader = csv.DictReader(f)
        for row in reader:
            if row.get("confidence") != "C1":
                continue
            sub = row.get("subsystem", "") or ""
            if not sub.startswith("third-party-library["):
                continue
            band = sub[len("third-party-library["):-1]
            target[row["rva"]] = band

    print(f"Targeted {len(target)} library C1 rows for C1->C2 promotion.")
    if not target:
        sys.exit(1)

    with open(csv_path, "r", encoding="utf-8", newline="") as f:
        lines = f.readlines()

    out = []
    promoted = []
    for line in lines:
        m = re.match(r"^(0x[0-9a-f]{8}|[0-9a-f]{8}),", line)
        if not m:
            out.append(line)
            continue
        rva = m.group(1)
        if rva not in target:
            out.append(line)
            continue
        if ",C1," not in line:
            out.append(line)
            continue
        new_line = line.replace(",C1,", ",C2,", 1)
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
        suffix = (f" | C1->C2 via {TAG}: POLICY OVERRIDE — band-classification "
                  f"(library_residue band={band}) accepted as C2 callee-body "
                  f"inspection + behavior-summary; canonical library's "
                  f"behavior is documented by published source/MSDN/vendor "
                  f"docs; user-approved override 2026-05-25")
        if body.endswith('"'):
            body = body[:-1] + suffix + '"'
        else:
            body = body + suffix
        out.append(body + ending)
        promoted.append(rva)

    if len(promoted) != len(target):
        missing = set(target) - set(promoted)
        print(f"WARNING: {len(missing)} target RVAs not promoted. Examples:")
        for r in sorted(missing)[:5]:
            print(f"  {r}")

    with open(csv_path, "w", encoding="utf-8", newline="") as f:
        f.writelines(out)

    from collections import Counter
    band_tally = Counter(target[r] for r in promoted)
    print(f"\n=== bulk_attest_c1_to_c2_libs results ===")
    print(f"Promoted: {len(promoted)} of {len(target)}")
    for band, n in band_tally.most_common():
        print(f"  {n:>4} {band}")


if __name__ == "__main__":
    main()
