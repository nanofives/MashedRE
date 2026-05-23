#!/usr/bin/env python3
"""
lever3_bulk_attest_d3dx9.py — Bulk-promote D3DX9 PSGP band C1 functions to C2.

Follows the same raw-line pattern as lever3_bulk_attest_v2.py to avoid
csv.writer round-trip corruption.

Evidence basis: calibrated PSGP band 0x004ec000..0x004fc9e0 (~67KB,
statically-linked Microsoft PSGP from D3DX9). Spatial location in the
calibrated band is the attestation equivalent of a FidDB match — every
function here is Microsoft PSGP library code, not game logic.
Dispatcher confirmed at 0x004fbe7a with 71-entry SSE/SSE2/3DNow! table.

This promotes the 105 functions already tracked at C1 in hooks.csv.
Functions in the band but not yet in hooks.csv are skipped (out of scope).
"""

import re
import sys
import os

DT = "20260523T120000"
LEVER_TAG = f"lever3-d3dx9-{DT}"

# 105 D3DX9 PSGP band RVAs confirmed C1 in hooks.csv (enumerated from Ghidra master,
# filtered to 0x004ec000..0x004fc9e0, cross-referenced against hooks.csv C1 rows)
PROMOTE_RVAS = {
    "004ec130",  # FUN_004ec130 — PSGP band
    "004ec720",  # FUN_004ec720 — PSGP band
    "004ec730",  # FUN_004ec730 — PSGP band
    "004ec740",  # FUN_004ec740 — PSGP band
    "004ec750",  # FUN_004ec750 — PSGP band
    "004ec760",  # FUN_004ec760 — PSGP band
    "004ec79d",  # FUN_004ec79d — PSGP band
    "004ec839",  # FUN_004ec839 — PSGP band
    "004ec8eb",  # FUN_004ec8eb — PSGP band
    "004ec915",  # FUN_004ec915 — PSGP band
    "004ec9a3",  # FUN_004ec9a3 — PSGP band
    "004ec9cd",  # FUN_004ec9cd — PSGP band
    "004eca7c",  # FUN_004eca7c — PSGP band
    "004ecaab",  # FUN_004ecaab — PSGP band
    "004ecaee",  # FUN_004ecaee — PSGP band
    "004ecafb",  # thunk_FUN_004ecaee — PSGP band
    "004ecb12",  # FUN_004ecb12 — PSGP band
    "004ecb1f",  # thunk_FUN_004ecb12 — PSGP band
    "004ecb36",  # FUN_004ecb36 — PSGP band
    "004ecb43",  # thunk_FUN_004ecb36 — PSGP band
    "004ecb5a",  # FUN_004ecb5a — PSGP band
    "004ecb67",  # thunk_FUN_004ecb5a — PSGP band
    "004ecb97",  # FUN_004ecb97 — PSGP band
    "004ecc45",  # FUN_004ecc45 — PSGP band
    "004ecc6f",  # FUN_004ecc6f — PSGP band
    "004f022d",  # FUN_004f022d — PSGP band
    "004f0307",  # FUN_004f0307 — PSGP band
    "004f07a0",  # FUN_004f07a0 — PSGP band
    "004f07d0",  # FUN_004f07d0 — PSGP band
    "004f0800",  # FUN_004f0800 — PSGP band
    "004f0810",  # FUN_004f0810 — PSGP band
    "004f0900",  # FUN_004f0900 — PSGP band
    "004f0910",  # FUN_004f0910 — PSGP band
    "004f0940",  # FUN_004f0940 — PSGP band
    "004f0970",  # FUN_004f0970 — PSGP band
    "004f0ae0",  # FUN_004f0ae0 — PSGP band
    "004f0bd0",  # FUN_004f0bd0 — PSGP band
    "004f0c10",  # FUN_004f0c10 — PSGP band
    "004f0c50",  # FUN_004f0c50 — PSGP band
    "004f0d80",  # FUN_004f0d80 — PSGP band
    "004f0dc0",  # FUN_004f0dc0 — PSGP band
    "004f0f10",  # FUN_004f0f10 — PSGP band
    "004f10e0",  # FUN_004f10e0 — PSGP band
    "004f1130",  # FUN_004f1130 — PSGP band
    "004f1150",  # FUN_004f1150 — PSGP band
    "004f13d0",  # FUN_004f13d0 — PSGP band
    "004f1800",  # FUN_004f1800 — PSGP band
    "004f1870",  # FUN_004f1870 — PSGP band
    "004f1b00",  # FUN_004f1b00 — PSGP band
    "004f1f00",  # FUN_004f1f00 — PSGP band
    "004f1f90",  # FUN_004f1f90 — PSGP band
    "004f1fb0",  # FUN_004f1fb0 — PSGP band
    "004f2570",  # FUN_004f2570 — PSGP band
    "004f2e70",  # FUN_004f2e70 — PSGP band
    "004f2f90",  # FUN_004f2f90 — PSGP band
    "004f3030",  # FUN_004f3030 — PSGP band
    "004f3150",  # FUN_004f3150 — PSGP band
    "004f3440",  # FUN_004f3440 — PSGP band
    "004f36c0",  # FUN_004f36c0 — PSGP band
    "004f3b00",  # FUN_004f3b00 — PSGP band
    "004f3b60",  # FUN_004f3b60 — PSGP band
    "004f3bc0",  # FUN_004f3bc0 — PSGP band
    "004f3bd0",  # FUN_004f3bd0 — PSGP band
    "004f3be0",  # FUN_004f3be0 — PSGP band
    "004f3cb0",  # FUN_004f3cb0 — PSGP band
    "004f3ce0",  # FUN_004f3ce0 — PSGP band
    "004f3d40",  # FUN_004f3d40 — PSGP band
    "004f3e90",  # FUN_004f3e90 — PSGP band
    "004f4200",  # FUN_004f4200 — PSGP band
    "004f4270",  # FUN_004f4270 — PSGP band
    "004f42d0",  # FUN_004f42d0 — PSGP band
    "004f4340",  # FUN_004f4340 — PSGP band
    "004f43b0",  # FUN_004f43b0 — PSGP band
    "004f43f0",  # FUN_004f43f0 — PSGP band
    "004f4470",  # FUN_004f4470 — PSGP band
    "004f4510",  # FUN_004f4510 — PSGP band
    "004f46a0",  # FUN_004f46a0 — PSGP band
    "004f4900",  # FUN_004f4900 — PSGP band
    "004f5020",  # FUN_004f5020 — PSGP band
    "004f5030",  # FUN_004f5030 — PSGP band
    "004f57e0",  # FUN_004f57e0 — PSGP band
    "004f6870",  # FUN_004f6870 — PSGP band
    "004f71f0",  # FUN_004f71f0 — PSGP band
    "004f77f0",  # FUN_004f77f0 — PSGP band
    "004f7850",  # FUN_004f7850 — PSGP band
    "004f79f0",  # FUN_004f79f0 — PSGP band
    "004f7dc0",  # FUN_004f7dc0 — PSGP band
    "004f81b0",  # FUN_004f81b0 — PSGP band
    "004f8400",  # FUN_004f8400 — PSGP band
    "004f8490",  # FUN_004f8490 — PSGP band
    "004f8580",  # FUN_004f8580 — PSGP band
    "004f8640",  # FUN_004f8640 — PSGP band
    "004f8660",  # FUN_004f8660 — PSGP band
    "004f8670",  # FUN_004f8670 — PSGP band
    "004f8690",  # FUN_004f8690 — PSGP band
    "004f8730",  # FUN_004f8730 — PSGP band
    "004fa5f0",  # FUN_004fa5f0 — PSGP band
    "004fb050",  # FUN_004fb050 — PSGP band
    "004fb0e0",  # FUN_004fb0e0 — PSGP band
    "004fb130",  # FUN_004fb130 — PSGP band
    "004fb210",  # FUN_004fb210 — PSGP band
    "004fb2a0",  # FUN_004fb2a0 — PSGP band
    "004fb3e0",  # FUN_004fb3e0 — PSGP band
    "004fb4d0",  # FUN_004fb4d0 — PSGP band
    "004fb5a0",  # FUN_004fb5a0 — PSGP band
}

ATTEST_SUFFIX = (f" | C1->C2 via {LEVER_TAG}: "
                 f"D3DX9 PSGP band attestation (calibrated 004ec000..004fc9e0); "
                 f"statically-linked Microsoft PSGP; dispatcher 004fbe7a confirmed")


def process_line(line):
    """
    If this line's RVA is in PROMOTE_RVAS:
      - Replace ',C1,' with ',C2,' (first occurrence after RVA field)
      - Append ATTEST_SUFFIX to the notes field (last field)
    Returns (new_line, promoted_bool)
    """
    rva_match = re.match(r'^([0-9a-f]{8}),', line)
    if not rva_match:
        return line, False
    rva = rva_match.group(1)
    if rva not in PROMOTE_RVAS:
        return line, False

    # Safety check: only promote C1
    if ',C1,' not in line:
        return line, False

    # Replace ',C1,' with ',C2,'
    new_line = line.replace(',C1,', ',C2,', 1)

    # Append attestation suffix to notes (last field)
    ending = ''
    if new_line.endswith('\r\n'):
        ending = '\r\n'
        body = new_line[:-2]
    elif new_line.endswith('\n'):
        ending = '\n'
        body = new_line[:-1]
    else:
        body = new_line

    if body.endswith('"'):
        body = body[:-1] + ATTEST_SUFFIX + '"'
    else:
        body = body + ATTEST_SUFFIX

    return body + ending, True


def main():
    csv_path = os.path.join(
        os.path.dirname(os.path.dirname(os.path.abspath(__file__))),
        "hooks.csv"
    )
    if not os.path.exists(csv_path):
        print(f"ERROR: hooks.csv not found at {csv_path}", file=sys.stderr)
        sys.exit(1)

    with open(csv_path, "r", encoding="utf-8", newline="") as f:
        lines = f.readlines()

    result_lines = []
    promoted = []
    found_rvas = set()

    for line in lines:
        new_line, was_promoted = process_line(line)
        result_lines.append(new_line)
        if was_promoted:
            m = re.match(r'^([0-9a-f]{8}),', line)
            if m:
                promoted.append(m.group(1))
                found_rvas.add(m.group(1))

    not_found = PROMOTE_RVAS - found_rvas
    if not_found:
        print(f"WARNING: {len(not_found)} RVAs from promote-set not found in file:")
        for rva in sorted(not_found):
            print(f"  {rva}")

    with open(csv_path, "w", encoding="utf-8", newline="") as f:
        f.writelines(result_lines)

    print(f"=== lever3_bulk_attest_d3dx9 results ===")
    print(f"Promoted: {len(promoted)}")
    print(f"Not found in file: {len(not_found)}")
    print()
    print("Promoted RVAs:")
    for rva in sorted(promoted):
        print(f"  {rva}")

    return len(promoted)


if __name__ == "__main__":
    n = main()
    sys.exit(0 if n > 0 else 1)
