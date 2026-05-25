#!/usr/bin/env python3
"""
bulk_attest_boot_mini.py — Promote 7 real-boot C1 RVAs to C2.

Companion to lever3_bulk_attest_boot_crt.py. The CRT lever-3 pass cleared
81 of the 90 boot C1 rows. This pass handles the remaining real-boot
game-code RVAs (sub-0x005c) that *aren't* CRT library residue.

Evidence summary (collected 2026-05-25 via Ghidra MCP on Mashed_pool0):

  Thunks — 5-byte JMP rel32, body trivially inspected:
    0x00471eb0  bytes e9 3b ff ff ff  JMP 0x00471df0  (thunk_FUN_00471df0)
    0x0047ba10  bytes e9 5b af 01 00  JMP 0x00496970  (thunk_FUN_00496970)
    0x004955c0  bytes e9 bb ff ff ff  JMP 0x00495580  (thunk_FUN_00495580)
    0x004b6550  bytes e9 ab 01 00 00  JMP 0x004b6700  (thunk_FUN_004b6700)

  Single-writers:
    0x00431b40  9 bytes  body: DAT_0067eaa8 = param_1; return

  Already-analyzed (Ghidra annotation predates this row):
    0x004955d0  [C1 2026-05-18] plate already present; ~95-line CD-ROM
                drive-scan via GetCurrentDirectoryA + GetLogicalDrives
                + GetDriveTypeA. Body inspected.
    0x00558240  [C2 2026-05-16] plate already present; alloc 2D tex
                (FUN_004c77c0) + create RT (FUN_004cdca0) + lock+fill loop.
                Already C2 in Ghidra — hooks.csv is just out of sync.

  Skipped (self-deferred):
    0x00496e40  row at hooks.csv line 1794 has "DEFERRED C2 (D-10770):
                7/7 callees unmapped". Pickup condition not yet met.

The duplicate row at 0x00496e40 (line 1788) is left untouched — dedupe is
a separate, broader hooks.csv hygiene issue (93 duplicate RVAs total) not
in scope for this task.
"""

import os
import re
import sys

DT = "20260525-1730"
TAG = f"boot-mini-{DT}"

PROMOTE_RVAS = {
    "00471eb0": "5-byte JMP rel32 thunk -> 0x00471df0 (Ghidra-verified bytes e93bffffff)",
    "0047ba10": "5-byte JMP rel32 thunk -> 0x00496970 (Ghidra-verified bytes e95baf0100)",
    "004955c0": "5-byte JMP rel32 thunk -> 0x00495580 (Ghidra-verified bytes e9bbffffff)",
    "004b6550": "5-byte JMP rel32 thunk -> 0x004b6700 (Ghidra-verified bytes e9ab010000)",
    "00431b40": "9-byte single-writer: DAT_0067eaa8 = param_1; return",
    "004955d0": "CD-ROM drive-scan via GetCurrentDirectoryA + GetLogicalDrives + GetDriveTypeA; existing C1 plate confirms body inspection",
    "00558240": "alloc 2D tex(FUN_004c77c0)+create RT(FUN_004cdca0)+lock+fill loop; Ghidra annotation already [C2 2026-05-16] (CSV catch-up only)",
}


def process_line(line):
    m = re.match(r'^([0-9a-f]{8}),', line)
    if not m:
        return line, False, None
    rva = m.group(1)
    if rva not in PROMOTE_RVAS:
        return line, False, None
    if ',C1,' not in line:
        return line, False, None

    new_line = line.replace(',C1,', ',C2,', 1)

    ending = ''
    if new_line.endswith('\r\n'):
        ending = '\r\n'
        body = new_line[:-2]
    elif new_line.endswith('\n'):
        ending = '\n'
        body = new_line[:-1]
    else:
        body = new_line

    suffix = f" | C1->C2 via {TAG}: {PROMOTE_RVAS[rva]}"
    if body.endswith('"'):
        body = body[:-1] + suffix + '"'
    else:
        body = body + suffix

    return body + ending, True, rva


def main():
    csv_path = os.path.join(
        os.path.dirname(os.path.dirname(os.path.abspath(__file__))),
        "hooks.csv",
    )
    with open(csv_path, "r", encoding="utf-8", newline="") as f:
        lines = f.readlines()

    result_lines = []
    promoted = []
    for line in lines:
        new_line, was_promoted, rva = process_line(line)
        result_lines.append(new_line)
        if was_promoted:
            promoted.append(rva)

    found = set(promoted)
    missing = set(PROMOTE_RVAS) - found
    if missing:
        print(f"WARNING: {len(missing)} RVAs not found:")
        for r in sorted(missing):
            print(f"  {r}")

    with open(csv_path, "w", encoding="utf-8", newline="") as f:
        f.writelines(result_lines)

    print(f"=== bulk_attest_boot_mini results ===")
    print(f"Promoted: {len(promoted)} / target {len(PROMOTE_RVAS)}")
    for r in sorted(promoted):
        print(f"  {r}")
    return len(promoted)


if __name__ == "__main__":
    sys.exit(0 if main() == len(PROMOTE_RVAS) else 1)
