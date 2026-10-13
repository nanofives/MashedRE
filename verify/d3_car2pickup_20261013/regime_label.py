#!/usr/bin/env python3
"""Regime label per car, the ROADMAP §D3 (b) rule (re/tools/ai_ctrl_window.py window_regime): the FIRST non-zero
flag_a368 inside the car's 220-call window (from its first call with c4 != 0): none -> A, 1 -> B, 2 -> C.
Works on an original <x>.aistep.csv or an exe <arm>.step.csv. Usage: py -3.12 regime_label.py <csv> [...]"""
import csv, sys
for path in sys.argv[1:]:
    per = {}
    for r in csv.DictReader(open(path, newline='')):
        per.setdefault(int(r['v']), []).append(r)
    lab = {}
    for v, L in sorted(per.items()):
        i0 = next((i for i, r in enumerate(L) if int(r['c4']) != 0), None)
        if i0 is None:
            lab[v] = '-'; continue
        f = next((int(r['flag_a368']) for r in L[i0:i0 + 220] if int(r['flag_a368']) != 0), 0)
        lab[v] = {0: 'A', 1: 'B', 2: 'C'}.get(f, '?%d' % f)
    s = set(lab.values())
    print('%s: %s -> %s' % (path, lab, s.pop() if len(s) == 1 else 'MIXED'))
