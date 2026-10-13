#!/usr/bin/env python3
"""Report-only: the consumer calls (FUN_00414c30, <x>.c30.tsv) of car v that return 2, with their driving-call index
(from the car's first ba8c==6 call), frame, held word and refdist; plus the index of the car's first held != 0 call.
Usage: py -3.12 ret2_rows.py <c30.tsv> <v>"""
import sys
path, v = sys.argv[1], sys.argv[2]
rows = [l.rstrip('\n').split('\t') for l in open(path) if l.strip()]
ix = {k: i for i, k in enumerate(rows[0])}
L = [r for r in rows[1:] if r[ix['v']] == v and r[ix['ba8c']] == '6']
first = next((i for i, r in enumerate(L) if r[ix['held']] != '0'), None)
print('car %s: %d driving calls, first held != 0 at call %s (frame %s)' % (v, len(L), first, L[first][ix['frame']] if first is not None else '-'))
for i, r in enumerate(L):
    if r[ix['ret']] == '2':
        print('  call %d frame %s held %s refdist %s' % (i, r[ix['frame']], r[ix['held']], r[ix['refdist']]))
