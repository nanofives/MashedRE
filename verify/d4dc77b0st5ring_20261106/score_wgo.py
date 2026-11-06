"""W-GO scorer for D4-DC-77b0-ST5-RING (PREREG in this folder).

Reads port MSD1 record dumps (MASHED_RECDUMP, re/tools/statediff/msd_fields.py) and prints the (+0xbf4, +0xbf8) change
rows, then checks race frames 0..5 against the original's frames exit+1..exit+6 (ST5-C4ACCEL
SUMMARY_orig_cd_20261106.txt: car 1 877..882, car 3 837..842 = 900/1, 700/1, 500/1, 300/1, 100/1, 0/0).
Usage: py -3.12 score_wgo.py <arm.rec.msd> [...]   exit 0 = every file matches.
"""
import os
import struct
import sys

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 're', 'tools', 'statediff'))
import msd_fields as m  # noqa: E402

WANT = [(900, 1), (700, 1), (500, 1), (300, 1), (100, 1), (0, 0)]


def main(paths):
    bad = 0
    for p in paths:
        rows = [(fi, struct.unpack_from('<i', r, 0xbf4)[0], struct.unpack_from('<i', r, 0xbf8)[0]) for fi, r in m.parse(p)]
        print('==', os.path.basename(p))
        print('frame,+bf4,+bf8')
        last = None
        for fi, a, b in rows:
            if (a, b) != last:
                print('%d,%d,%d' % (fi, a, b)); last = (a, b)
            if fi > 12:
                break
        got = [(a, b) for fi, a, b in rows[:6]]
        ok = got == WANT and [fi for fi, _, _ in rows[:6]] == list(range(6))
        print('W-GO', 'MATCH' if ok else 'DIFF', got)
        bad += 0 if ok else 1
    return 1 if bad else 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
