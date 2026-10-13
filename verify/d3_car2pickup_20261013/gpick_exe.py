#!/usr/bin/env python3
"""G-PICK / G-FIX, exe side: car 2's closest pass to pool 4's position over registry frames 0..60 (<arm>.registry.csv)
and the first PK rows of the exe's FUN_00458920 trace (<arm>.cb.tsv, collect / bump arms).
Usage: py -3.12 gpick_exe.py <arm> [<arm> ...]   (arm = path prefix, e.g. verify/d3_car2pickup_20261013/R_FIX)"""
import csv, math, os, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import traj as T

SUM = 0.5 + 0.533388913  # pickup radius + car radius (registry columns)
for arm in sys.argv[1:]:
    rows = T.exe_rows(arm + '.registry.csv')
    ds = [(fr, math.dist(c[2], T.PK)) for fr, c in rows if 2 in c and fr <= 60]
    fmin, dmin = min(ds, key=lambda x: x[1])
    first_in = next((fr for fr, d in ds if d < SUM), None)
    pk = []
    if os.path.exists(arm + '.cb.tsv'):
        for line in open(arm + '.cb.tsv'):
            p = line.rstrip('\n').split('\t')
            if p and p[0] == 'PK':
                pk.append('frame %s car %s pool %s a8 %s->%s %s' % (p[2], p[6], p[16], p[9], p[22], p[24]))
    print('%s: car-2 closest pass to pool 4 = %.6f at registry frame %d; first frame inside %.6f: %s' % (
        os.path.basename(arm), dmin, fmin, SUM, first_in))
    for s in pk[:3]:
        print('   ', s)
