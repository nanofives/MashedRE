"""PREREG_pontoon.md predictions P1..P5 on an OP capture.  py -3.12 analyze_op.py <tag>"""
import struct
import sys

import numpy as np

import pn_rows as P

f = P.f
F32 = np.float32


def m4(m):
    a = np.array([f(x) for x in m], dtype=np.float64).reshape(4, 4)
    a[:, 3] = [0, 0, 0, 1]
    return a


def main():
    tag = sys.argv[1]
    rs = P.load(f'{tag}.msd.pontoon.csv')
    out = []
    pr = out.append
    pr(f'# {tag}: {len(rs)} rows, n {rs[0]["n"]}..{rs[-1]["n"]}')
    # P1
    units = sorted(set(r['units'] for r in rs))
    box = rs[0]['E']['box']
    pr(f'P1 units {units}; anim4c non-zero on all rows: {all(r["E"]["anim4c"] for r in rs)}; '
       f'box {["%08x" % b for b in box]}')
    # P2 timing per cycle
    trans = [(r['n'], r['E']['state'], r['L']['state'], r) for r in rs if r['E']['state'] != r['L']['state']]
    for n, a, b, r in trans:
        pr(f'P2 transition n={n} {a}->{b} ff50={r["L"]["ff50"]} c8={f(r["L"]["c8"]):.6f} a4={f(r["L"]["a4"]):.6f}')
    s1 = [r for r in rs if r['E']['state'] == 1]
    pr(f'P2 calls with state_in 1: {len(s1)}')
    bad_a4 = 0
    for r in s1:
        a_in, a_out = F32(f(r['E']['a4'])), F32(f(r['L']['a4']))
        exp = F32(a_in - F32(F32(r['units']) * F32(f(0x39ae9681))))
        if exp != a_out:
            bad_a4 += 1
    pr(f'P2 state-1 a4 step mismatches (float32 model): {bad_a4} of {len(s1)}')
    s2 = [r for r in rs if r['E']['state'] == 2]
    d = F32(F32(50) * F32(f(0x3a2e9681)))
    bad_c8 = sum(1 for r in s2 if F32(d + F32(f(r['E']['c8']))) != F32(f(r['L']['c8'])))
    pr(f'P2 state-2 calls {len(s2)}, c8 step mismatches (float32 model, d={d!r}): {bad_c8}')
    for n, a, b, r in trans:
        if a == 2 and b == 3:
            pr(f'P2 2->3 row: all words 0 = {r["L"]["ff50"] == [0, 0, 0, 0]}, c8 > 2.0 = {f(r["L"]["c8"]) > 2.0}')
        if a == 3 and b == 0:
            pr(f'P2 3->0 row: c8 after = {f(r["L"]["c8"])}; c8 before = {f(r["E"]["c8"]):.6f}')
    # P3 carry (approximate check in float64; the exe replay is the bit-exact check)
    carried = 0; maxerr = 0.0; still = 0; still_bad = 0
    for r in rs:
        M54 = m4(r['L']['m54']); M94 = m4(r['L']['m94'])
        inv = np.linalg.inv(M54)
        for v in range(4):
            e = r['Ecars'][v]['mat']; l = r['Lcars'][v]['mat']
            if r['L']['ff50'][v] == 1:
                exp = m4(e) @ inv @ M94
                err = np.abs(exp[:, :3] - m4(l)[:, :3]).max()
                maxerr = max(maxerr, err); carried += 1
            elif r['L']['b4'][v] == 0 and r['E']['b4'][v] == 0:
                still += 1
                if [e[k] for k in range(16) if k not in (7, 11, 15)] != [l[k] for k in range(16) if k not in (7, 11, 15)]:
                    still_bad += 1
    pr(f'P3 carried car-rows {carried}, max |leave - enter*inv(M54)*M94| = {maxerr:.3g}; '
       f'not-in-box rows {still}, changed by the call: {still_bad}')
    # P4 exit rows
    for r in rs:
        for v in range(4):
            if r['E']['b4'][v] == 1 and r['L']['b4'][v] == 0:
                ce, cl = r['Ecars'][v], r['Lcars'][v]
                pr(f'P4 exit n={r["n"]} car {v}: rwp {ce["rwp"]}->{cl["rwp"]} timer {cl["rwp_t"]} '
                   f'vel_in {[round(f(x), 3) for x in ce["vel"][:3]]} vel_out {[f(x) for x in cl["vel"]]} '
                   f'mov {[round(f(x), 4) for x in r["L"]["mov"]]}')
    # P5 car y on the platform
    ys = [(r['n'], r['L']['state'], f(r['Lcars'][v]['mat'][13]), f(r['L']['m94'][13]))
          for r in rs for v in range(4) if r['L']['ff50'][v] == 1]
    if ys:
        st2 = [y for y in ys if y[1] in (2, 3)]
        if st2:
            arr = np.array([y[2] for y in st2])
            pr(f'P5 car y while aboard in states 2/3: n={len(st2)} min {arr.min():.4f} max {arr.max():.4f} '
               f'median {np.median(arr):.4f}; platform node y {st2[0][3]:.4f}')
        st0 = [y for y in ys if y[1] in (0, 1)]
        if st0:
            arr = np.array([y[2] for y in st0])
            pr(f'P5 car y while aboard in states 0/1: n={len(st0)} median {np.median(arr):.4f}')
    # marker
    mk = set((c['mk'], c['mk_on']) for r in rs for c in r['Lcars'])
    pr('marker (colour, on) at leave: ' + ', '.join(f'({a:08x},{b})' for a, b in sorted(mk)))
    # tower / deck rest vs MTS file
    pr('tower_rest ' + ' '.join('%08x' % x for x in rs[0]['tower_rest']))
    pr('deck_rest  ' + ' '.join('%08x' % x for x in rs[0]['deck_rest']))
    text = '\n'.join(out)
    print(text)
    open(f'{tag}_predictions.txt', 'w').write(text + '\n')


if __name__ == '__main__':
    main()
