#!/usr/bin/env python3
"""Report-only: contact coasts per AI car, read from an AI step dump (original <x>.aistep.csv or exe <arm>.step.csv).
A coast = >= 8 consecutive substate-6 steps of one car with rec_9e4 (speed, +0x9e4) bit-equal and > 100, which is what
the +0xaec A4 skip (0x00471042) produces: A4 does not run, nothing writes +0x9b0, and 0x004710c8 re-stores |v|.
Prints per car: the coasts as (step index from the car's first substate-6 step, frame, speed), within the first N steps.
Usage: py -3.12 coast_events.py <csv> [N=400]"""
import csv, sys
path = sys.argv[1]
N = int(sys.argv[2]) if len(sys.argv) > 2 else 400
per = {}
for r in csv.DictReader(open(path, newline='')):
    if r.get('substate') != '6':
        continue
    per.setdefault(int(r['v']), []).append(r)
for v in sorted(per):
    L = per[v][:N]
    ev, k = [], 0
    while k < len(L):
        j = k
        while j + 1 < len(L) and L[j + 1]['rec_9e4'] == L[k]['rec_9e4']:
            j += 1
        if j - k + 1 >= 8 and float(L[k]['rec_9e4']) > 100:
            ev.append((k, L[k]['frame'], L[k]['rec_9e4'], j - k + 1))
        k = j + 1
    print('car %d: %d coasts in the first %d steps: %s' % (v, len(ev), len(L),
          ', '.join('step %d (frame %s, spd %.1f, x%d)' % (a, b, float(c), d) for a, b, c, d in ev)))
