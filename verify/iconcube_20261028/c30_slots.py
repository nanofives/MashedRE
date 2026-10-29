"""Decode the registry slots in an original FUN_00414c30 probe capture (*.c30.tsv).

Usage: py -3.12 c30_slots.py <file.c30.tsv> [--all]
Prints, per row near the phase change (ba8c 3 -> 6), the slot type words (slot +0x10) and the
type-7 slots' position (slot +0x00/+0x04/+0x08) and radius (slot +0x0c).
"""
import struct
import sys


def main():
    fn = sys.argv[1]
    show_all = '--all' in sys.argv
    with open(fn) as f:
        rows = [l.rstrip('\n').split('\t') for l in f]
    ix = {k: i for i, k in enumerate(rows[0])}
    seen6 = 0
    prev_ba = None
    for r in rows[1:]:
        ba = r[ix['ba8c']]
        if ba == '6':
            seen6 += 1
        near = (ba != prev_ba) or (ba == '6' and seen6 <= 9) or show_all
        prev_ba = ba
        if not near:
            continue
        sl = r[ix['slots']].split(';') if r[ix['slots']] else []
        types, sevens = [], []
        for k, s in enumerate(sl):
            w = struct.unpack('<8I', bytes.fromhex(s))
            fl = struct.unpack('<8f', bytes.fromhex(s))
            types.append('%x' % w[4])
            if w[4] == 7:
                sevens.append('%d:(%.4f,%.4f,%.4f) r=%.3f sub=%08x' % (k, fl[0], fl[1], fl[2], fl[3], w[5]))
        print(r[ix['n']], 'frame', r[ix['frame']], 'v', r[ix['v']], 'ba', ba, 'd8', r[ix['d8']],
              'held', r[ix['held']], 'ret', r[ix['ret']], 'types', ' '.join(types))
        for s in sevens:
            print('    type7', s)


if __name__ == '__main__':
    main()
