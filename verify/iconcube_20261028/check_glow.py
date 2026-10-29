r"""D5-ICONCUBE-GLOW arm A4: recompute every flushed glow particle from the pool snapshots.

    py -3.12 verify\iconcube_20261028\check_glow.py <H_GLOWD.puglow.tsv> [--max-print N]

Input: the MASHED_DBG_PUGLOW dump (Gameplay/PickupGlow.cpp): PRE / POST pool rows around each FUN_00459000 call,
then the batch's particles (P rows) and count (N row). The expected particle list is the static formula of
PREREG_D5-ICONCUBE-GLOW.md section 3 (disassembly of FUN_00459000, original/MASHED.exe.unpatched), evaluated in
float32 operation by operation (the tick runs at PC_24: each x87 add / sub / mul / div rounds to 24 bits, the
float32 result). The x87 fsin of the state-2 size is compared within 1 ulp, every other field bit-exact.
Prints one line per check and a verdict line.
"""
import argparse
import math
import struct
import sys
from collections import defaultdict

import numpy as np

F = np.float32
COL = [0xffffffff] * 7 + [0xfff1c12d, 0xffffffff, 0xff00e0ff, 0xff0d8a51, 0xffac78f1, 0xff241cec, 0xffffffff,
                          0xffffffff, 0xffffffff, 0xff1d94f7, 0xffab3578, 0xff84ff84, 0xffff0000, 0xffffffff,
                          0xffffffff, 0x00000000]                 # DAT_005f9838[0..21] + 0x005f9890
C540 = F(1.25)              # [0x005ce540] / [0x005ce544] 0x3fa00000
C548 = F(1.0)               # [0x005ce548] 0x3f800000
C2F4 = F(struct.unpack('<f', struct.pack('<I', 0x40490fdb))[0])   # [0x005ce2f4]
C7A8 = F(struct.unpack('<f', struct.pack('<I', 0x3c8efa35))[0])   # [0x005cd7a8]
ONE = F(1.0)                # qword [0x005cd0c8] 1.0, used at PC_24
ZERO = F(0.0)               # [0x005d757c]


def f(h):
    return F(struct.unpack('<f', struct.pack('<I', int(h, 16)))[0])


def bits(x):
    return struct.unpack('<I', struct.pack('<f', float(F(x))))[0]


def ulp_dist(a, b):
    def o(u):
        return u if u < 0x80000000 else 0x80000000 - u
    return abs(o(a) - o(b))


def expected(pre, post):
    """pre/post: {i: row}; row = (state, type, a04, r18, t1c, a[3], p[3], fr[3]) as float32 / int."""
    out = []
    n = len(pre)
    for i in range(n - 1, -1, -1):          # 0x0045901f..0x00459033: entry n-1 first
        st, ty, a04, r18, t1c, a, p, fr = pre[i]
        if ty == 0x15:                       # 0x00459036..0x0045903b
            st = 0
        if st == 1:                          # 0x0045912b..
            dx = F(a[0] - p[0]); dy = F(a[1] - p[1]); dz = F(a[2] - p[2])
            d2 = F(F(F(dy * dy) + F(dx * dx)) + F(dz * dz))
            if not (d2 > ZERO):              # 0x0045915f..0x0045916a: <= 0 or unordered -> the constants
                sx = sy = C540
            else:
                t = F(ONE - d2)              # 0x0045916c fsubr qword
                m = ONE if t >= ONE or math.isnan(t) else t
                if m < ZERO:
                    s = ZERO
                else:
                    s = t if ONE > t else ONE
                sx = F(C540 * s); sy = F(C540 * s)
            q = post[i]
            rot = F(q[2] * C7A8)             # 0x0045920d..0x00459210 (POST: FUN_00458a40 ran at 0x004591e8)
            out.append(dict(i=i, st=1, pos=[bits(v) for v in q[7]], size=(bits(sx), bits(sy)), sz_tol=0,
                            col=COL[ty] if 0 <= ty < len(COL) else None, rot=bits(rot)))
        elif st == 2:                        # 0x00459057..
            if not (t1c > ZERO):             # 0x00459059..0x00459064 -> the reset branch, no particle
                continue
            x = F(r18 - t1c)                 # 0x0045906a..0x0045906d
            if not (x < C548):               # 0x00459079..0x00459084
                continue
            y = F(F(x / C548) * C2F4)        # 0x0045908a, 0x00459098
            s = F(math.sin(float(y)))        # 0x0045909e fsin (PC_24), within 1 ulp
            out.append(dict(i=i, st=2, pos=[bits(v) for v in p], size=(bits(F(C540 * s)), bits(F(C540 * s))),
                            sz_tol=1, col=COL[ty] if 0 <= ty < len(COL) else None, rot=0))
    return out[:25]                          # Rfx::Submit caps at the batch capacity 0x19


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('dump')
    ap.add_argument('--max-print', type=int, default=10)
    a = ap.parse_args()
    pre, post = defaultdict(dict), defaultdict(dict)
    parts, count = defaultdict(list), {}
    for ln in open(a.dump):
        if ln.startswith('#') or not ln.strip():
            continue
        c = ln.rstrip('\n').split('\t')
        if c[0] in ('PRE', 'POST'):
            tick, i = int(c[1]), int(c[2])
            row = (int(c[3]), int(c[4]), f(c[5]), f(c[6]), f(c[7]), [f(c[8]), f(c[9]), f(c[10])],
                   [f(c[11]), f(c[12]), f(c[13])], [f(c[14]), f(c[15]), f(c[16])])
            (pre if c[0] == 'PRE' else post)[tick][i] = row
        elif c[0] == 'P':
            parts[int(c[1])].append([int(x, 16) for x in c[3:10]])
        elif c[0] == 'N':
            count[int(c[1])] = int(c[2])
    ticks = sorted(count)
    bad_n = bad_p = 0
    n_st = defaultdict(int)
    printed = 0
    exact_sz2 = 0
    for t in ticks:
        exp = expected(pre[t], post[t])
        got = parts[t]
        if len(exp) != count[t] or len(got) != count[t]:
            bad_n += 1
            if printed < a.max_print:
                print(f'COUNT tick {t}: expected {len(exp)} flushed {count[t]} rows {len(got)}'); printed += 1
            continue
        for e, g in zip(exp, got):
            n_st[e['st']] += 1
            ok = (g[0:3] == e['pos'] and g[5] == e['col'] and g[6] == e['rot'] and
                  ulp_dist(g[3], e['size'][0]) <= e['sz_tol'] and ulp_dist(g[4], e['size'][1]) <= e['sz_tol'])
            if e['st'] == 2 and ok and g[3] == e['size'][0] and g[4] == e['size'][1]:
                exact_sz2 += 1
            if not ok:
                bad_p += 1
                if printed < a.max_print:
                    print(f'PART tick {t} entry {e["i"]} state {e["st"]}: expected pos {[hex(x) for x in e["pos"]]} '
                          f'size {[hex(x) for x in e["size"]]} col {e["col"] and hex(e["col"])} rot {hex(e["rot"])} | '
                          f'got {[hex(x) for x in g]}'); printed += 1
    total = sum(n_st.values())
    print(f'ticks {len(ticks)}  particles {total} (state 1: {n_st[1]}, state 2: {n_st[2]}, '
          f'state-2 sizes bit-exact {exact_sz2})  count mismatches {bad_n}  particle mismatches {bad_p}')
    ok = len(ticks) > 0 and total > 0 and bad_n == 0 and bad_p == 0
    print('A4', 'PASS' if ok else 'FAIL', '(state 1 only: no state-2 particle seen)' if ok and n_st[2] == 0 else '')
    return 0 if ok else 1


if __name__ == '__main__':
    sys.exit(main())
