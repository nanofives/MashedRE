#!/usr/bin/env python3
"""
dedupe_hooks_csv.py — Remove duplicate-RVA rows from hooks.csv.

Per-pattern rules (from the 2026-05-25 audit):

  identical (2 RVAs):
      Drop all rows but the first occurrence.

  diff_confidence (87 RVAs):
      Keep highest confidence (rank C4 > C3 > C2 > C1 > C0).
      Tie-break by longest notes (most-informative).

  diff_status (1 RVA = 0x00431b40):
      Keep status='mapped' over 'new' (mapped is the more-advanced label).
      Confidence is equal in this case so notes-length tie-break doesn't fire.

  diff_subsystem (1 RVA = 0x004963d0):
      Keep subsystem='input' over 'boot'. Hardcoded: 0x004963d0 is a
      5-byte JMP rel32 thunk to 0x00496370 which is in subsystem=input.
      Both 'boot' rows are wrong AND duplicates of each other.

  diff_other (2 RVAs):
      Keep longest notes (most-recent / most-annotated). The recent
      notes are typically additive (e.g., "| boot-mini-20260525 …"
      suffix) so longer == newer.

Raw-line CSV editor pattern (per project convention: NEVER round-trip
via csv.writer — corrupts existing quoting). Reads all lines, identifies
RVA via regex anchor, decides keep/drop per line.

Backup is made to /tmp/hooks.csv.pre_dedupe.
"""

import csv
import os
import re
import shutil
import sys
from collections import defaultdict
from io import StringIO

CONF_RANK = {'C0': 0, 'C1': 1, 'C2': 2, 'C3': 3, 'C4': 4}


def parse_rows(csv_text):
    """Return list of (line_index, line_str, parsed_dict) tuples.
    parsed_dict is None for lines we can't parse (header, blank, comments)."""
    out = []
    lines = csv_text.splitlines(keepends=True)
    # Parse header
    reader = csv.reader(StringIO(lines[0]))
    fields = next(reader)
    for i, line in enumerate(lines):
        if not line.strip() or line.startswith('#') or i == 0:
            out.append((i, line, None))
            continue
        try:
            row_reader = csv.reader(StringIO(line))
            vals = next(row_reader)
            if len(vals) < len(fields):
                vals += [''] * (len(fields) - len(vals))
            d = dict(zip(fields, vals))
            if not re.match(r'^[0-9a-f]{8}$', d.get('rva', '')):
                out.append((i, line, None))
                continue
            out.append((i, line, d))
        except Exception:
            out.append((i, line, None))
    return out, fields


def decide_keepers(rows_for_rva):
    """Given list of (line_index, line_str, parsed_dict) for one RVA, return
    the line_index of the single row to KEEP. Drop the others."""
    rva = rows_for_rva[0][2]['rva']
    if len(rows_for_rva) == 1:
        return rows_for_rva[0][0]

    # Hardcoded subsystem decision for 004963d0
    if rva == '004963d0':
        for i, line, d in rows_for_rva:
            if d['subsystem'] == 'input':
                return i

    # All identical -> first
    parsed = [d for _, _, d in rows_for_rva]
    if all(parsed[0] == p for p in parsed[1:]):
        return rows_for_rva[0][0]

    # Confidence-rank tie-break
    best_idx = rows_for_rva[0][0]
    best_rank = CONF_RANK.get(rows_for_rva[0][2]['confidence'], -1)
    best_notes_len = len(rows_for_rva[0][2].get('notes', '') or '')
    best_status_priority = 1 if rows_for_rva[0][2].get('status') == 'mapped' else 0

    for i, line, d in rows_for_rva[1:]:
        rank = CONF_RANK.get(d['confidence'], -1)
        notes_len = len(d.get('notes', '') or '')
        status_priority = 1 if d.get('status') == 'mapped' else 0
        # Confidence rank dominates; then status priority; then notes length
        if (rank, status_priority, notes_len) > (best_rank, best_status_priority, best_notes_len):
            best_idx = i
            best_rank = rank
            best_status_priority = status_priority
            best_notes_len = notes_len

    return best_idx


def main():
    root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    os.chdir(root)
    csv_path = os.path.join(root, 'hooks.csv')

    shutil.copy(csv_path, '/tmp/hooks.csv.pre_dedupe')
    print(f'Backup: /tmp/hooks.csv.pre_dedupe')

    with open(csv_path, 'r', encoding='utf-8', newline='') as f:
        csv_text = f.read()

    parsed, fields = parse_rows(csv_text)
    lines = csv_text.splitlines(keepends=True)

    # Group by RVA
    by_rva = defaultdict(list)
    for i, line, d in parsed:
        if d is not None:
            by_rva[d['rva']].append((i, line, d))

    # Decide which line index to keep per RVA with duplicates
    keep_indices = set(i for i, _, d in parsed if d is None)  # keep non-row lines
    drop_indices = set()
    n_dupes = 0
    n_drops = 0
    for rva, group in by_rva.items():
        if len(group) == 1:
            keep_indices.add(group[0][0])
            continue
        n_dupes += 1
        keeper = decide_keepers(group)
        keep_indices.add(keeper)
        for i, _, _ in group:
            if i != keeper:
                drop_indices.add(i)
                n_drops += 1

    new_lines = [lines[i] for i in sorted(keep_indices)]
    with open(csv_path, 'w', encoding='utf-8', newline='') as f:
        f.writelines(new_lines)

    print(f'\n=== dedupe results ===')
    print(f'Duplicate RVAs:       {n_dupes}')
    print(f'Rows dropped:         {n_drops}')
    print(f'Rows before: {len(lines)}')
    print(f'Rows after:  {len(new_lines)}')


if __name__ == '__main__':
    main()
