#!/usr/bin/env python3
"""
lever3_bulk_attest_libpng.py — Bulk-promote libpng/zlib band C1 functions to C2.

Follows the same raw-line pattern as lever3_bulk_attest_d3dx9.py to avoid
csv.writer round-trip corruption.

Evidence basis: calibrated libpng/zlib band 0x00513f90..0x0052caf0 (~116KB,
statically-linked libpng 1.x + zlib 1.x).  Spatial location in the
calibrated band is the attestation equivalent of a FidDB match — every
function here is libpng/zlib library code, not game logic.

Confirmed libpng strings at boundary functions:
  - 0x00516bb0: "Call to NULL write function"  (png_write_data)
  - 0x00517250: "Call to NULL error function"  (png_error)
  - 0x005150e0: "zlib_error"                  (zlib wrapper)
  - 0x0051e230: "Can't write tRNS..."          (png_write_tRNS)
  - 0x0051ee70: "Unrecognized equation type"   (png_write_sCAL)
  - 0x00522970: CRC32 table at DAT_005e3460    (zlib crc32)
  - 0x00522ce0: "unknown compression method",  (zlib inflate)
               "invalid window size",
               "incorrect header check"
  - 0x0052c7a0: "invalid literal/length code", (zlib inflate_fast)
               "invalid distance code"

Band lower boundary: 0x00513f90 (first libpng function in hooks.csv)
Band upper boundary: 0x0052caf0 (last confirmed zlib function body_end)
Game code confirmed at: 0x0052cb00 (BMP reader, calls FUN_004d8480 error handler)

This promotes the 152 functions already tracked at C1 in hooks.csv.
Functions in the band not yet in hooks.csv are skipped (out of scope).
"""

import re
import sys
import os

DT = "20260523T150000"
LEVER_TAG = f"lever3-libpng-{DT}"

# 152 libpng/zlib band RVAs confirmed C1 in hooks.csv
# (enumerated from Ghidra master, filtered to 0x00513f90..0x0052caf0,
# cross-referenced against hooks.csv C1 rows)
PROMOTE_RVAS = {
    "00516bb0",  # FUN_00516bb0 — png_write_data (confirmed: "Call to NULL write function")
    "00516bd0",  # FUN_00516bd0 — libpng band
    "00516bf0",  # FUN_00516bf0 — libpng band
    "00516c40",  # FUN_00516c40 — libpng band
    "00516cc0",  # FUN_00516cc0 — libpng band
    "00516d20",  # thunk_FUN_00520930 — libpng band thunk
    "00516d30",  # FUN_00516d30 — libpng band
    "00516d50",  # FUN_00516d50 — libpng band
    "00516da0",  # FUN_00516da0 — libpng band
    "00516de0",  # FUN_00516de0 — libpng band
    "005171a0",  # FUN_005171a0 — libpng band
    "00517200",  # FUN_00517200 — libpng band
    "00517250",  # FUN_00517250 — libpng band
    "00517270",  # FUN_00517270 — libpng band (confirmed: "Call to NULL error function")
    "005172b0",  # FUN_005172b0 — libpng band
    "005172f0",  # FUN_005172f0 — libpng band
    "00517380",  # FUN_00517380 — libpng band
    "005173d0",  # FUN_005173d0 — libpng band
    "005173f0",  # FUN_005173f0 — libpng band
    "005175d0",  # FUN_005175d0 — libpng band
    "00517940",  # FUN_00517940 — libpng band
    "00517da0",  # FUN_00517da0 — libpng band
    "00517e00",  # FUN_00517e00 — libpng band
    "00518080",  # FUN_00518080 — libpng band
    "00518140",  # FUN_00518140 — libpng band
    "005183a0",  # FUN_005183a0 — libpng band
    "005183c0",  # FUN_005183c0 — libpng band
    "005184b0",  # FUN_005184b0 — libpng band
    "005184f0",  # FUN_005184f0 — libpng band
    "00518570",  # FUN_00518570 — libpng band
    "00518580",  # FUN_00518580 — libpng band
    "00518590",  # FUN_00518590 — libpng band
    "005185a0",  # FUN_005185a0 — libpng band
    "00519270",  # FUN_00519270 — libpng band
    "005196f0",  # FUN_005196f0 — libpng band
    "00519820",  # FUN_00519820 — libpng band
    "005199f0",  # FUN_005199f0 — libpng band
    "00519af0",  # FUN_00519af0 — libpng band
    "00519bc0",  # FUN_00519bc0 — libpng band
    "00519e30",  # FUN_00519e30 — libpng band
    "00519f70",  # FUN_00519f70 — libpng band
    "0051a6c0",  # FUN_0051a6c0 — png_do_background (confirmed: palette/transparency ops)
    "0051b750",  # FUN_0051b750 — png_do_gamma (confirmed: gamma LUT ops)
    "0051bb40",  # FUN_0051bb40 — libpng band (palette expand)
    "0051bd70",  # FUN_0051bd70 — libpng band (transparency expand)
    "0051c100",  # FUN_0051c100 — libpng band
    "0051c220",  # FUN_0051c220 — libpng band
    "0051c9f0",  # FUN_0051c9f0 — libpng band
    "0051ca10",  # FUN_0051ca10 — libpng band
    "0051ca60",  # FUN_0051ca60 — libpng band
    "0051ca90",  # FUN_0051ca90 — libpng band
    "0051cb40",  # FUN_0051cb40 — libpng band
    "0051cb70",  # FUN_0051cb70 — libpng band
    "0051cbe0",  # FUN_0051cbe0 — libpng band
    "0051cfc0",  # FUN_0051cfc0 — libpng band
    "0051d0f0",  # FUN_0051d0f0 — libpng band
    "0051d1b0",  # FUN_0051d1b0 — libpng band
    "0051d240",  # FUN_0051d240 — libpng band
    "0051d340",  # FUN_0051d340 — libpng band
    "0051d400",  # FUN_0051d400 — libpng band
    "0051d560",  # FUN_0051d560 — libpng band
    "0051d800",  # FUN_0051d800 — libpng band
    "0051d900",  # FUN_0051d900 — libpng band
    "0051db30",  # FUN_0051db30 — libpng band
    "0051dca0",  # FUN_0051dca0 — libpng band
    "0051e230",  # FUN_0051e230 — png_write_tRNS (confirmed: "Can't write tRNS...")
    "0051e4a0",  # FUN_0051e4a0 — libpng band
    "0051e6f0",  # FUN_0051e6f0 — libpng band
    "0051e7f0",  # FUN_0051e7f0 — libpng band
    "0051e990",  # FUN_0051e990 — libpng band
    "0051ead0",  # FUN_0051ead0 — libpng band
    "0051ed60",  # FUN_0051ed60 — libpng band
    "0051ee70",  # FUN_0051ee70 — png_write_sCAL (confirmed: "Unrecognized equation type")
    "0051f0e0",  # FUN_0051f0e0 — libpng band
    "0051f240",  # FUN_0051f240 — libpng band
    "0051f350",  # FUN_0051f350 — libpng band
    "0051f4f0",  # FUN_0051f4f0 — libpng band
    "0051f810",  # FUN_0051f810 — libpng band
    "0051fa50",  # FUN_0051fa50 — png row filter selector (confirmed: Paeth filter)
    "00520640",  # FUN_00520640 — libpng band
    "005207c0",  # FUN_005207c0 — libpng band
    "00520850",  # FUN_00520850 — libpng band
    "00520870",  # FUN_00520870 — libpng band
    "005208c0",  # FUN_005208c0 — png_malloc (confirmed: callee of png_set_text_2)
    "00520930",  # FUN_00520930 — png_free (confirmed: callee of png_write_sCAL)
    "00520960",  # FUN_00520960 — libpng band
    "00520990",  # FUN_00520990 — libpng band
    "005209d0",  # FUN_005209d0 — libpng band
    "00520a00",  # FUN_00520a00 — libpng band
    "00520b60",  # FUN_00520b60 — libpng band
    "00520c70",  # FUN_00520c70 — libpng band
    "00520ef0",  # FUN_00520ef0 — png_do_swap (confirmed: RGBA channel swap)
    "00521000",  # FUN_00521000 — png_do_invert (confirmed: alpha inversion)
    "00521110",  # FUN_00521110 — libpng band
    "00521410",  # FUN_00521410 — libpng band
    "00521520",  # FUN_00521520 — zlib band
    "00521990",  # FUN_00521990 — zlib band
    "00522970",  # FUN_00522970 — zlib crc32 (confirmed: CRC32 LUT at DAT_005e3460)
    "00522ab0",  # FUN_00522ab0 — zlib band
    "00522b00",  # FUN_00522b00 — zlib band
    "00522b50",  # FUN_00522b50 — zlib band
    "00522cc0",  # FUN_00522cc0 — zlib band
    "00522ce0",  # FUN_00522ce0 — zlib inflate (confirmed: "unknown compression method", etc.)
    "00523110",  # FUN_00523110 — zlib band
    "00523140",  # FUN_00523140 — zlib band
    "00523170",  # FUN_00523170 — zlib band
    "005232c0",  # FUN_005232c0 — zlib band
    "00523360",  # FUN_00523360 — zlib band
    "00523640",  # FUN_00523640 — zlib band
    "005238c0",  # FUN_005238c0 — zlib band
    "00523cd0",  # FUN_00523cd0 — zlib band
    "00523e40",  # FUN_00523e40 — zlib band
    "005241f0",  # FUN_005241f0 — zlib band
    "005245b0",  # FUN_005245b0 — zlib band
    "00524f70",  # FUN_00524f70 — zlib band
    "005253c0",  # FUN_005253c0 — zlib band
    "00525870",  # FUN_00525870 — zlib band
    "00525bd0",  # FUN_00525bd0 — zlib band
    "00525fb0",  # FUN_00525fb0 — zlib band
    "00526310",  # FUN_00526310 — zlib band
    "00526630",  # FUN_00526630 — zlib band
    "00526910",  # FUN_00526910 — zlib band
    "00526bf0",  # FUN_00526bf0 — zlib band
    "005270e0",  # FUN_005270e0 — zlib band
    "00527560",  # FUN_00527560 — zlib band
    "005278a0",  # FUN_005278a0 — zlib band
    "00527a40",  # FUN_00527a40 — zlib band
    "00527c30",  # FUN_00527c30 — zlib band
    "00527ee0",  # FUN_00527ee0 — zlib band
    "00528200",  # FUN_00528200 — zlib band
    "00528630",  # FUN_00528630 — zlib band
    "00528810",  # FUN_00528810 — zlib band
    "00528ba0",  # FUN_00528ba0 — zlib band
    "00528e00",  # FUN_00528e00 — zlib band
    "00528e20",  # FUN_00528e20 — zlib band
    "00528e30",  # FUN_00528e30 — zlib band
    "00528ef0",  # FUN_00528ef0 — zlib band
    "00529050",  # FUN_00529050 — zlib band
    "0052a890",  # FUN_0052a890 — zlib band
    "0052a9c0",  # FUN_0052a9c0 — zlib band
    "0052aa40",  # FUN_0052aa40 — zlib band
    "0052ab00",  # FUN_0052ab00 — zlib band
    "0052b750",  # FUN_0052b750 — zlib band
    "0052b7e0",  # FUN_0052b7e0 — zlib band
    "0052b820",  # FUN_0052b820 — zlib band
    "0052bf20",  # FUN_0052bf20 — zlib band
    "0052bf40",  # FUN_0052bf40 — zlib band
    "0052bff0",  # FUN_0052bff0 — zlib band
    "0052c4a0",  # FUN_0052c4a0 — zlib band
    "0052c630",  # FUN_0052c630 — zlib band
    "0052c660",  # FUN_0052c660 — zlib band
    "0052c7a0",  # FUN_0052c7a0 — zlib inflate_fast (confirmed: "invalid literal/length code")
}

ATTEST_SUFFIX = (f" | C1->C2 via {LEVER_TAG}: "
                 f"libpng/zlib band attestation (calibrated 00513f90..0052caf0); "
                 f"statically-linked libpng 1.x + zlib 1.x; "
                 f"band confirmed by png_write_data@00516bb0, png_write_tRNS@0051e230, "
                 f"zlib_crc32@00522970, zlib_inflate@00522ce0, inflate_fast@0052c7a0")


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

    print(f"=== lever3_bulk_attest_libpng results ===")
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
