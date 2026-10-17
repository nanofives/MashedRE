"""Totals of a run13.py scores_diff.txt: frames, rows (the column after '|'), cited, RED, CITED lines.
usage: py -3.12 sum13.py <scores_diff.txt> [...]"""
import re, sys
for p in sys.argv[1:]:
    fr = rows = cited = red = ok = 0
    for line in open(p, encoding='utf-8', errors='replace'):
        m = re.search(r'\|\s+(\d+)\s+(\d+)\s+(\d+)\s+(\w+)\s*$', line)
        if m:
            fr += 1; rows += int(m.group(1)); cited += int(m.group(2)); red += int(m.group(3)); ok += m.group(4) == 'CITED'
    print('%s: frames %d rows %d cited %d RED %d CITED-frames %d' % (p, fr, rows, cited, red, ok))
