#!/usr/bin/env python3
"""
resolve_merge_followup.py — Resolve git conflicts for the frontend-followup sweep.

Strategy:
  - hooks.csv: for each conflict chunk, take the row version with `,C2,`
    where both sides have the same RVA (per-RVA union). For RVAs only on
    one side, take that side's row.
  - UNCERTAINTIES.md, re/SCRIBE_QUEUE.md, re/analysis/CHANGELOG.md:
    pure append-union — take both sides' additions in HEAD-then-other order.

Works on files with `<<<<<<<`, `=======`, `>>>>>>>` markers in place.
"""

import re
import sys
import os


def parse_conflicts(lines):
    """Yield (pre_lines, head_lines, other_lines, post_lines) tuples."""
    chunks = []
    i = 0
    pre = []
    while i < len(lines):
        if lines[i].startswith('<<<<<<<'):
            chunks.append(pre)
            head = []
            i += 1
            while i < len(lines) and not lines[i].startswith('======='):
                head.append(lines[i])
                i += 1
            i += 1  # skip =======
            other = []
            while i < len(lines) and not lines[i].startswith('>>>>>>>'):
                other.append(lines[i])
                i += 1
            i += 1  # skip >>>>>>>
            chunks.append({'head': head, 'other': other})
            pre = []
        else:
            pre.append(lines[i])
            i += 1
    chunks.append(pre)
    return chunks


def hooks_csv_per_rva_union(head, other):
    """For each RVA appearing in either side, take the version with C2 if any."""
    def parse(side):
        out = {}
        for line in side:
            m = re.match(r'^([0-9a-f]{8}),', line)
            if m:
                out[m.group(1)] = line
        return out
    h, o = parse(head), parse(other)
    rvas = sorted(set(h) | set(o), key=lambda r: int(r, 16))
    merged = []
    for rva in rvas:
        hl = h.get(rva)
        ol = o.get(rva)
        if hl and ol:
            # Pick the side with ,C2,
            if ',C2,' in hl and ',C1,' in ol:
                merged.append(hl)
            elif ',C2,' in ol and ',C1,' in hl:
                merged.append(ol)
            elif hl == ol:
                merged.append(hl)
            else:
                # Both C2 (from independent batches) — take the longer / both annotations.
                # Use HEAD as base, append other's suffix if it has one we don't.
                if ' | batch-frontend-followup-' in ol and ' | batch-frontend-followup-' not in hl:
                    merged.append(ol.rstrip('\r\n') + '\n')
                else:
                    merged.append(hl)
        elif hl:
            merged.append(hl)
        else:
            merged.append(ol)
    return merged


def append_union(head, other):
    """Concatenate head then other (preserve order)."""
    return list(head) + list(other)


def resolve_file(path, mode):
    """mode: 'csv_rva_union' or 'append_union'"""
    with open(path, 'r', encoding='utf-8', newline='') as f:
        lines = f.readlines()
    if not any(l.startswith('<<<<<<<') for l in lines):
        print(f'  {path}: no conflicts, skipping')
        return False
    chunks = parse_conflicts(lines)
    out = []
    n_conflicts = 0
    for c in chunks:
        if isinstance(c, dict):
            n_conflicts += 1
            if mode == 'csv_rva_union':
                out.extend(hooks_csv_per_rva_union(c['head'], c['other']))
            else:
                out.extend(append_union(c['head'], c['other']))
        else:
            out.extend(c)
    with open(path, 'w', encoding='utf-8', newline='') as f:
        f.writelines(out)
    print(f'  {path}: {n_conflicts} chunk(s) resolved via {mode}')
    return True


def main():
    root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    os.chdir(root)
    resolve_file('hooks.csv', 'csv_rva_union')
    resolve_file('UNCERTAINTIES.md', 'append_union')
    resolve_file('re/SCRIBE_QUEUE.md', 'append_union')
    resolve_file('re/analysis/CHANGELOG.md', 'append_union')


if __name__ == '__main__':
    main()
