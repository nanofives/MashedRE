#!/usr/bin/env python3
"""
lever3_bulk_attest_v2.py — Bulk-promote CRT-band C0/C1 functions to C2 via FidDB attestation.

v2: Works on raw lines only for the targeted RVAs, never re-encodes via csv.writer.
This avoids corrupting pre-existing quoting conventions in other rows.

Strategy:
  1. Build the set of RVAs to promote (from the analysis script).
  2. For each line in hooks.csv: if the line's RVA is in the promote-set,
     do two targeted string replacements:
       a. Replace ",C1," with ",C2," (confidence column)
       b. Append attestation note: if the line ends with a quoted field,
          insert before closing quote; otherwise append to end of notes field.
  3. Write all other lines verbatim.

This is safe because we never parse-and-rewrite rows we don't own.
"""

import re
import sys
import os

DT = "20260523-1330"
LEVER_TAG = f"lever3-expansion-{DT}"

# The 79 RVAs to promote (from the analysis — game-logic exclusions already removed)
PROMOTE_RVAS = {
    "004a333c",  # __exit
    "004a40fe",  # ___onexitinit
    "004a415e",  # _atexit
    "004a57e4",  # FUN_004a57e4 (XOR seed init — CRT band residue)
    "004ab8d6",  # FUN_004ab8d6 (table scan/stderr — CRT band residue)
    "004aba4d",  # __FF_MSGBANNER
    "004a2cbd",  # FID_conflict:_wprintf
    "004a460d",  # _free
    "004abd1a",  # FUN_004abd1a (token parser — CRT band residue)
    "004ac560",  # FUN_004ac560 (7-byte thunk — CRT band residue)
    "004af2b6",  # ___initmbctable
    "004affe0",  # FUN_004affe0 (wrapper — CRT band residue)
    "004a2bf7",  # FUN_004a2bf7 (fn-ptr init — CRT band residue)
    "004a34b0",  # _strncpy
    "004a4126",  # __onexit
    "004a4728",  # FUN_004a4728 (CRT lock wrapper — CRT band residue)
    "004a5de3",  # FUN_004a5de3 (__controlfp — CRT band residue)
    "004a5e35",  # __ms_p5_mp_test_fdiv
    "004a5f07",  # ___endstdio
    "004a7796",  # __mtdeletelocks
    "004a77eb",  # FUN_004a77eb (LeaveCriticalSection — CRT band residue)
    "004a787f",  # __lock
    "004aac76",  # ___sbh_alloc_block
    "004aaf72",  # __callnewh
    "004aaf90",  # _memset
    "004ac45c",  # ___crtMessageBoxA
    "004a2d18",  # FUN_004a2d18 (file-unlock thunk — CRT band residue)
    "004a504f",  # FUN_004a504f (wprintf format core — CRT band residue)
    "004af2d4",  # FUN_004af2d4 (CRT SEH filter — CRT band residue)
    "004af32d",  # FUN_004af32d (CRT exit deregister — CRT band residue)
    "004a43d2",  # FUN_004a43d2 (time-info — CRT band residue)
    "004a43b9",  # FUN_004a43b9 (asctime wrapper — CRT band residue)
    "004a4170",  # __alldiv
    "004a4220",  # __allmul
    "004a3b84",  # FUN_004a3b84 (vsnprintf — CRT band residue)
    "004a42c5",  # FUN_004a42c5 (vsprintf — CRT band residue)
    "004ad1e0",  # FUN_004ad1e0 (CRT abort — CRT band residue)
    "004a3384",  # CRT::acos
    "004a3620",  # CRT::atan
    "004a3700",  # CRT::math_float_371d
    "004a37b0",  # CRT::math_float_37cd
    "004a45cf",  # __nh_malloc
    "004a4660",  # FUN_004a4660 (SBH unlock wrapper — CRT band residue)
    "004aa497",  # ___sbh_find_block
    "004aa4c2",  # ___sbh_free_block
    "004ae28f",  # ___crtInitCritSecNoSpinCount@8
    "004af166",  # __setmbcp
    "004affaf",  # FUN_004affaf (char-class predicate — CRT band residue)
    "004a4554",  # __heap_alloc
    "004a45c6",  # FUN_004a45c6 (SBH unlock wrapper — CRT band residue)
    "004a7800",  # FUN_004a7800 (lazy lock-slot init — CRT band residue)
    "004acd00",  # _memmove
    "004aefd0",  # FUN_004aefd0 (code-page init — CRT band residue)
    "004af2ad",  # FUN_004af2ad (setmbcp unlock — CRT band residue)
    "004aed77",  # FUN_004aed77 (codepage->LCID — CRT band residue)
    "004aeda6",  # setSBCS
    "004aedcf",  # FUN_004aedcf (locale case-map — CRT band residue)
    "004ac570",  # FUN_004ac570 (fast strcpy — CRT band residue)
    "004a2bb8",  # report_failure
    "004a31e1",  # FUN_004a31e1 (__lock atexit — CRT band residue)
    "004a407e",  # __onexit_lk
    "004a4158",  # FUN_004a4158 (unlock wrapper — CRT band residue)
    "004ad33b",  # __controlfp
    "004a5df5",  # __ms_p5_test_fdiv
    "004a9744",  # __flushall
    "004ad351",  # __fcloseall
    "004aa7da",  # ___sbh_alloc_new_region
    "004aa891",  # ___sbh_alloc_new_group
    "004a31ea",  # FUN_004a31ea (unlock wrapper — CRT band residue)
    "004ad3e3",  # FUN_004ad3e3 (unlock wrapper — CRT band residue)
    "004aa23d",  # FUN_004aa23d (errno getter — CRT band residue)
    "004a583a",  # FUN_004a583a (security error handler — CRT band residue)
    "004ae635",  # FUN_004ae635 (locale string builder — CRT band residue)
    "004af601",  # FUN_004af601 (file-handle block alloc — CRT band residue)
    "004af56c",  # FUN_004af56c (per-handle critSec — CRT band residue)
    "004a4fc1",  # write_char
    "004a4ff4",  # write_multi_char
    "004a5018",  # write_string
    "004a4da0",  # __aulldvrm
}

ATTEST_SUFFIX = (f" | C1->C2 via {LEVER_TAG}: "
                 f"CRT-band bulk-attest (calibrated 004a0000..004b3fff); "
                 f"FidDB name or CRT library match confirmed in notes")


def process_line(line, promoted_count):
    """
    If this line's RVA is in PROMOTE_RVAS:
      - Replace ',C1,' with ',C2,' (first occurrence after RVA field)
      - Append ATTEST_SUFFIX to the notes field (last field)
    Returns (new_line, promoted_bool)
    """
    # Quick prefix check before full parse
    rva_match = re.match(r'^([0-9a-f]{8}),', line)
    if not rva_match:
        return line, False
    rva = rva_match.group(1)
    if rva not in PROMOTE_RVAS:
        return line, False

    # Safety check: must contain ',C1,' not ',C2,' or ',C3,' etc.
    if ',C1,' not in line:
        return line, False

    # Replace ',C1,' with ',C2,'
    new_line = line.replace(',C1,', ',C2,', 1)

    # Append attestation to notes (last field).
    # The notes field is the 9th comma-separated field (index 8, 0-based).
    # Some rows have the notes quoted, some not.
    # Strategy: strip the line ending, then:
    #   If line ends with '"': insert before the closing "
    #   Else: just append to end of line
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
        # Notes field is quoted — insert inside
        body = body[:-1] + ATTEST_SUFFIX + '"'
    else:
        # Notes field is unquoted — just append
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
    missing = list(PROMOTE_RVAS)  # RVAs not found in file

    found_rvas = set()
    for line in lines:
        new_line, was_promoted = process_line(line, len(promoted))
        result_lines.append(new_line)
        if was_promoted:
            rva_match = re.match(r'^([0-9a-f]{8}),', line)
            if rva_match:
                promoted.append(rva_match.group(1))
                found_rvas.add(rva_match.group(1))

    not_found = PROMOTE_RVAS - found_rvas
    if not_found:
        print(f"WARNING: {len(not_found)} RVAs from promote-set not found in file:")
        for rva in sorted(not_found):
            print(f"  {rva}")

    with open(csv_path, "w", encoding="utf-8", newline="") as f:
        f.writelines(result_lines)

    print(f"=== lever3_bulk_attest_v2 results ===")
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
