#!/usr/bin/env python3
"""
bulk_attest_frontend_oversize.py — Promote the 2 oversize frontend C1 RVAs
deferred by the s1 worker in the bulk batches (300-line cap).

Evidence: full Ghidra decomp inspection + analysis plate per RVA at
re/analysis/frontend_c1_to_c2_followup_main/FUN_<rva>.md.
"""

import os
import re
import sys

DT = "20260525-1740"
TAG = f"frontend-oversize-{DT}"

PROMOTE_RVAS = {
    "0040acd0": "save-game state-machine dispatcher (6 ops x sub-state ladder); plate at re/analysis/frontend_c1_to_c2_followup_main/FUN_0040acd0.md",
    "0040eee0": "round-end car-elimination + score distribution (3-branch game-type dispatch); plate at re/analysis/frontend_c1_to_c2_followup_main/FUN_0040eee0.md",
}


def process_line(line):
    m = re.match(r'^([0-9a-f]{8}),', line)
    if not m:
        return line, False
    rva = m.group(1)
    if rva not in PROMOTE_RVAS:
        return line, False
    if ',C1,' not in line:
        return line, False
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
    return body + ending, True


def main():
    csv_path = os.path.join(
        os.path.dirname(os.path.dirname(os.path.abspath(__file__))),
        "hooks.csv",
    )
    with open(csv_path, "r", encoding="utf-8", newline="") as f:
        lines = f.readlines()
    result, promoted = [], []
    for line in lines:
        nl, was = process_line(line)
        result.append(nl)
        if was:
            promoted.append(re.match(r'^([0-9a-f]{8}),', line).group(1))
    missing = set(PROMOTE_RVAS) - set(promoted)
    if missing:
        print(f"WARNING: missing: {sorted(missing)}")
    with open(csv_path, "w", encoding="utf-8", newline="") as f:
        f.writelines(result)
    print(f"=== bulk_attest_frontend_oversize results ===")
    print(f"Promoted: {len(promoted)} / target {len(PROMOTE_RVAS)}")
    for r in sorted(promoted):
        print(f"  {r}")
    return len(promoted)


if __name__ == "__main__":
    sys.exit(0 if main() == len(PROMOTE_RVAS) else 1)
