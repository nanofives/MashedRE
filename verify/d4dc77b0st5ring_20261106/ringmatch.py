"""D4-DC-77b0-ST5-RING: match a captured FUN_00534870 ring state (--ring-probe rows) against the port ring.

Port ring = FUN_00534990 verbatim (seed 0x9a319039 at 0x005349e2, LCG 0x41c64e6d/0x3039 at 0x005349fb..0x00534a07,
p = base+0xc, q = base at 0x005349bd.., 0x136 discards at 0x00534a22..). k = draws after the open's 310 discards.
Usage: py -3.12 ringmatch.py <ring.csv> [maxk]
"""
import csv, sys
M = 0xffffffff
def fresh():
    r = [0x9a319039]
    for i in range(1, 31): r.append((r[-1] * 0x41c64e6d + 0x3039) & M)
    return r
class Ring:
    def __init__(s):
        s.r = fresh(); s.p = 3; s.q = 0
        for _ in range(0x136): s.next()
    def next(s):
        r = s.r
        r[s.p] = (r[s.p] + r[s.q]) & M; u = r[s.p] >> 1; s.p += 1
        if s.p >= 31: s.p = 0; s.q += 1; return u
        s.q += 1
        if s.q >= 31: s.q = 0
        return u
def cd(u):
    import numpy as np
    f = np.float32
    return int(f(4.0) * f(u & 0x7fffffff) * f(4.656612873e-10) + f(1.0))
def main():
    rows = list(csv.DictReader(open(sys.argv[1])))
    maxk = int(sys.argv[2]) if len(sys.argv) > 2 else 2000000
    want = {}
    for i, row in enumerate(rows):
        want[i] = (int(row['p']), int(row['q']), tuple(int(row['r%d' % j], 16) for j in range(31)))
    R = Ring(); found = {}
    for k in range(maxk + 1):
        st = (R.p, R.q, tuple(R.r))
        for i, w in want.items():
            if i not in found and w == st: found[i] = k
        if len(found) == len(want): break
        R.next()
    for i, row in enumerate(rows):
        print(f"{row['tag']:5s} ba8c={row['ba8c']} ret={row['ret'] or '-':8s} p={row['p']:>2s} q={row['q']:>2s} -> k={found.get(i, 'NOT FOUND <= %d' % maxk)}")
    # cd draws at the first E row, for reference
    for i, row in enumerate(rows):
        if row['tag'] == 'E' and i in found:
            R = Ring()
            for _ in range(found[i]): R.next()
            print('cd draws from E row:', [cd(R.next()) for _ in range(4)])
            break
if __name__ == '__main__':
    main()
