"""D5-FLIP-RFLAME-SORT: R_FLAME reachability on a MASHED_PU_STEPDUMP arm.

    py -3.12 pu_reach.py <arm>.pustep.csv [...]

Counts, per slot, the rows where the slot held each power-up code (code_pre / code_post, PowerupSystem.cpp DumpRow)
and the rows where FIRE ran (fire_modes non-empty). R_FLAME is code 16 (Powerup/PowerupSystem.h kRFlame). A spark is
emitted only from RFlame_Fire / RFlame_Step on a slot holding 16, so zero R_FLAME rows means the sorter only ever
sees dead records on that arm.
"""
import csv
import sys
from collections import Counter

NAMES = {7: 'MORTAR', 9: 'GUN', 10: 'DRUM', 11: 'MISSILE', 12: 'PMINE', 16: 'R_FLAME', 17: 'SHOTGUN', 18: 'FLASH', 19: 'OIL'}

for p in sys.argv[1:]:
    held, fired = Counter(), Counter()
    rows = 0
    frames = set()
    with open(p, newline='') as f:
        for r in csv.DictReader(f):
            rows += 1
            frames.add(r['frame'])
            for c in {int(r['code_pre']), int(r['code_post'])} - {-1}:
                held[(int(r['slot']), c)] += 1
            if r['fire_modes']:
                fired[(int(r['slot']), int(r['code_pre']))] += 1
    print(f'{p}: rows={rows} frames={len(frames)}')
    for (s, c), n in sorted(held.items()):
        print(f'  slot {s} held {NAMES.get(c, c)} rows={n} fire_rows={fired.get((s, c), 0)}')
    rf = sum(n for (s, c), n in held.items() if c == 16)
    rff = sum(n for (s, c), n in fired.items() if c == 16)
    print(f'  R_FLAME held rows={rf} fire rows={rff}')
