#!/usr/bin/env python3
"""
lever3_bulk_attest_v2.py â€” Bulk-promote CRT-band C0/C1 functions to C2 via FidDB attestation.

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

DT = "20260524-2200"
LEVER_TAG = f"lever3-lua50-{DT}"

# The 79 RVAs to promote (from the analysis â€” game-logic exclusions already removed)
PROMOTE_RVAS = {
    "004bdcc0",  # lparser_fornum
    "004bdd50",  # lparser_new_localvar
    "004bdda0",  # lparser_registerlocalvar
    "004bddf0",  # lparser_new_localvarliteral
    "004bde20",  # lparser_forbody
    "004bdea0",  # lparser_adjustlocalvars
    "004bdee0",  # lparser_forlist
    "004bdf80",  # lparser_funcargs_or_methodcall
    "004be070",  # lparser_test_then_block
    "004be0a0",  # lparser_localstat
    "004be110",  # lparser_adjust_assign
    "004be170",  # lparser_funcname_or_callstat
    "004be1b0",  # lparser_funcname_dotcolon
    "004be220",  # lparser_exprstat
    "004be290",  # lparser_assignment
    "004be360",  # lparser_retstat
    "004be3b0",  # lparser_breakstat
    "004be410",  # lparser_body
    "004be4b0",  # lparser_pushclosure
    "004be540",  # lparser_parlist
    "004be5d0",  # lparser_parlist_end
    "004be640",  # lundump_LoadFunction
    "004be6c0",  # lundump_getsource
    "004be6d0",  # lundump_LoadChunk
    "004be6f0",  # lundump_LoadFunction_body
    "004be780",  # lundump_LoadByte
    "004be7d0",  # lundump_error_eof
    "004be7f0",  # lundump_LoadInt
    "004be820",  # lundump_LoadVar
    "004be880",  # lundump_LoadBlock
    "004be8b0",  # lundump_LoadString
    "004be8d4",  # lundump_LoadString_tail
    "004be900",  # lundump_LoadCode
    "004be980",  # lundump_LoadVector
    "004bea00",  # lundump_LoadLines
    "004beaa0",  # lundump_LoadLocals
    "004beaf0",  # lundump_LoadConstants
    "004bebe0",  # lundump_LoadHeader
    "004bed50",  # lundump_LoadDouble
    "004bed80",  # lundump_LoadSignature
    "004bede0",  # lundump_TESTSIZE
    "004bee20",  # lfunc_newupval
    "004bee60",  # lfunc_newproto
    "004beec0",  # lfunc_setcodesize
    "004beef0",  # lfunc_sizeproto
    "004bef20",  # lfunc_freeproto
    "004befa0",  # lfunc_freeclosure
    "004befd0",  # lfunc_getlocalname
    "004bf020",  # lcode_syntaxerror_curtoken
    "004bf040",  # lcode_jump
    "004bf080",  # lcode_getlabel
    "004bf0b0",  # lcode_checkstack
    "004bf0f0",  # lcode_storestring_or_storek
    "004bf110",  # lcode_storeNumber
    "004bf180",  # lcode_numberK
    "004bf200",  # lcode_setreg_or_loadnil
    "004bf230",  # lcode_hasjumps
    "004bf260",  # lcode_getprevinstr
    "004bf280",  # lcode_setreturns
    "004bf2d0",  # lcode_storevar
    "004bf320",  # lcode_patchlist
    "004bf360",  # lcode_patchlistaux
    "004bf3f0",  # lcode_patchtarget
    "004bf460",  # lcode_getjump
    "004bf490",  # lcode_patchtohere
    "004bf4e0",  # lcode_goiftrue
    "004bf510",  # lcode_codecond
    "004bf5c0",  # lcode_dischargevars_short
    "004bf600",  # lcode_dischargevars
    "004bf670",  # lcode_invertcond
    "004bf6e0",  # lcode_codearith
    "004bf820",  # lcode_need_value
    "004bf870",  # lcode_codeABx_label
    "004bf890",  # lcode_prefix
    "004bf920",  # lcode_infix
    "004bf970",  # lcode_goiffalse
    "004bf9a0",  # lcode_posfix
    "004bfa40",  # lcode_codeABC_nil
    "004bfa60",  # lcode_codeABx
    "004bfa80",  # lcode_codeABC_core
    "004b6fc0",  # FUN_004b6fc0
    "004b6fd0",  # FUN_004b6fd0
    "004b6ff0",  # FUN_004b6ff0
    "004b7040",  # FUN_004b7040
    "004b7090",  # FUN_004b7090
    "004b70d0",  # FUN_004b70d0
    "004b7110",  # FUN_004b7110
    "004b7180",  # FUN_004b7180
    "004b7210",  # FUN_004b7210
    "004b7280",  # FUN_004b7280
    "004b7530",  # FUN_004b7530
    "004b75d0",  # FUN_004b75d0
    "004b7620",  # FUN_004b7620
    "004b7660",  # FUN_004b7660
    "004b76a0",  # FUN_004b76a0
    "004b76e0",  # FUN_004b76e0
    "004b7740",  # FUN_004b7740
    "004b78a0",  # FUN_004b78a0
    "004b78e0",  # FUN_004b78e0
    "004b79c0",  # FUN_004b79c0
    "004b7b00",  # FUN_004b7b00
    "004b7b30",  # FUN_004b7b30
    "004b7ba0",  # FUN_004b7ba0
    "004b7c70",  # FUN_004b7c70
    "004b7ca0",  # FUN_004b7ca0
    "004b7cc0",  # FUN_004b7cc0
    "004b7d20",  # FUN_004b7d20
    "004b7d60",  # FUN_004b7d60
    "004b7e30",  # FUN_004b7e30
    "004b7f30",  # FUN_004b7f30
    "004b7f70",  # FUN_004b7f70
    "004b80a0",  # FUN_004b80a0
    "004b8190",  # FUN_004b8190
    "004b82a0",  # FUN_004b82a0
}

ATTEST_SUFFIX = (f" | C1->C2 via {LEVER_TAG}: "
                 f"lua-5.0 band bulk-attest (calibrated 004b6fc0..004bfa80, ~36KB); "
                 f"FidDB lua name (lparser/lcode/lundump/lfunc) or band-residue match")


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
        # Notes field is quoted â€” insert inside
        body = body[:-1] + ATTEST_SUFFIX + '"'
    else:
        # Notes field is unquoted â€” just append
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
