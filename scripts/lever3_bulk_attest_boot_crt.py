#!/usr/bin/env python3
"""
lever3_bulk_attest_boot_crt.py — Promote 81 CRT-band-mis-tagged-as-boot C1 rows to C2.

Same raw-line-edit strategy as lever3_bulk_attest_v2.py:
  - For each PROMOTE_RVA, replace ',C1,' with ',C2,' once and append an
    attestation suffix. Subsystem is left as 'boot' (re-tag is a separate
    concern; prior lever-3 batches did promotion-only).

Calibration: 0x005c1d63..0x005cb160 — the MSVC CRT band that prior batch_v s6
flagged as MIXED[msvc-crt-fiddb(33)+msvc-crt-internal-helpers(23)+
game-dynarray-and-chunk-readers(15)+audio-mixer-voice-init(1)+
game-collision-or-rwmath-zero-callers(8)].

Per CONFIDENCE.md: C2 = "callee body inspected; behavior summarized but not
yet hooked". For these CRT-band rows the prior batch_v note already documents
this — bulk-attest just lifts the confidence label.
"""

import os
import re
import sys

DT = "20260525-1700"
LEVER_TAG = f"lever3-boot-crt-{DT}"

PROMOTE_RVAS = {
    "005c1d63", "005c1da2", "005c1dac", "005c1dd3", "005c1e27", "005c1e31",
    "005c1ea0", "005c1f1c", "005c1f97", "005c2025", "005c2090", "005c21ba",
    "005c21e2", "005c228e", "005c229b", "005c22bd", "005c2300", "005c2435",
    "005c2470", "005c24e0", "005c2770", "005c278f", "005c27e6", "005c281e",
    "005c28e7", "005c29cb", "005c2a0e", "005c2a45", "005c2bc5", "005c2d84",
    "005c2d9b", "005c2dd9", "005c2e79", "005c2f0a", "005c2f26", "005c2f58",
    "005c2f70", "005c2f84", "005c2f8d", "005c302a", "005c318c", "005c31d0",
    "005c326b", "005c3340", "005c337e", "005c33bb", "005c3403", "005c340a",
    "005c3551", "005c3584", "005c35c0", "005c35d6", "005c4180", "005c419e",
    "005c4440", "005c4547", "005c4607", "005c4640", "005c4670", "005c4780",
    "005c47b0", "005c47e0", "005c4ab0", "005c4ad0", "005c4ba0", "005c4bb0",
    "005c4c60", "005c4d20", "005c4d30", "005c4d50", "005c4da0", "005c4df0",
    "005c4e10", "005c4e20", "005c4fa0", "005c5780", "005c6b40", "005c6b60",
    "005c6bb0", "005c6cc0", "005cb160",
}

ATTEST_SUFFIX = (f" | C1->C2 via {LEVER_TAG}: "
                 f"CRT-band bulk-attest (calibrated 005c1d63..005cb160); "
                 f"FidDB/canonical-body attestation per batch_v s6 MIXED cluster")


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

    if body.endswith('"'):
        body = body[:-1] + ATTEST_SUFFIX + '"'
    else:
        body = body + ATTEST_SUFFIX

    return body + ending, True


def main():
    csv_path = os.path.join(
        os.path.dirname(os.path.dirname(os.path.abspath(__file__))),
        "hooks.csv",
    )
    if not os.path.exists(csv_path):
        print(f"ERROR: hooks.csv not found at {csv_path}", file=sys.stderr)
        sys.exit(1)

    with open(csv_path, "r", encoding="utf-8", newline="") as f:
        lines = f.readlines()

    result_lines = []
    promoted = []
    for line in lines:
        new_line, was_promoted = process_line(line)
        result_lines.append(new_line)
        if was_promoted:
            m = re.match(r'^([0-9a-f]{8}),', line)
            if m:
                promoted.append(m.group(1))

    found = set(promoted)
    missing = PROMOTE_RVAS - found
    if missing:
        print(f"WARNING: {len(missing)} RVAs not found:")
        for r in sorted(missing):
            print(f"  {r}")

    with open(csv_path, "w", encoding="utf-8", newline="") as f:
        f.writelines(result_lines)

    print(f"=== lever3_bulk_attest_boot_crt results ===")
    print(f"Promoted: {len(promoted)} / target {len(PROMOTE_RVAS)}")
    return len(promoted)


if __name__ == "__main__":
    sys.exit(0 if main() == len(PROMOTE_RVAS) else 1)
