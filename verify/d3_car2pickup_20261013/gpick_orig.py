#!/usr/bin/env python3
"""G-PICK, original side: collect-arm rows of FUN_00458920 (<x>.cb.tsv PK rows) as driving ticks.
Tick 0 = the first FUN_00442a60 row with DAT_0063ba8c == 6 (<x>.rd.tsv); clocks advance 50 per tick.
Usage: py -3.12 gpick_orig.py <x.msd>  (reads <x.msd>.cb.tsv and <x.msd>.rd.tsv)"""
import csv, sys, struct, math
csv.field_size_limit(1 << 30)
base = sys.argv[1]
rd = list(csv.DictReader(open(base + '.rd.tsv', newline=''), delimiter='\t'))
clk6 = next(int(r['clk']) for r in rd if r['ba8c'] == '6')
print('first ba8c==6 clk', clk6)
for line in open(base + '.cb.tsv'):
    p = line.rstrip('\n').split('\t')
    if p[0] != 'PK':
        continue
    frame, a, b, type_b = int(p[2]), p[3], p[4], p[6]
    pool, entry_in, slot_b, a8_out, wit = p[16], p[17], p[19], p[22], p[24]
    eb = bytes.fromhex(slot_b)
    car = struct.unpack_from('<fff', eb, 0)
    rad = struct.unpack_from('<f', eb, 12)[0]
    ea = bytes.fromhex(p[18])
    pk = struct.unpack_from('<fff', ea, 0)
    prad = struct.unpack_from('<f', ea, 12)[0]
    print('frame %d tick %.1f car_type %s pool %s pickup(%.6f,%.6f,%.6f r%.4f) car(%.6f,%.6f,%.6f r%.6f) d=%.6f sum=%.6f a8_out=%s %s' % (
        frame, (frame - clk6) / 50.0, type_b, pool, *pk, prad, *car, rad, math.dist(pk, car), prad + rad, a8_out, wit))
