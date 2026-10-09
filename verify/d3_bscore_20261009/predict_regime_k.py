"""Predict each MASHED_RING_SKIP K's (b) regime from the verbatim FUN_00534870 ring (PREREG_BREGIME_C.md).
Assumption under test: the number of ring draws before the band-0 roll does not depend on the draws'
values, so the roll reads draw index n0 + K (after RwRandomOpen's 310 discards).
Roll (AiStandalone.cpp:1705-1716, row 2 band 0): r1 = AiRand(0,100); r1 <= 20 -> B; else r2 = AiRand(0,100);
r2 <= 10 -> C; else A. AiRand law: (hi-lo) * float(u & 0x7fffffff) * 2^-31 + lo, float32."""
import numpy as np, sys
M = 0xffffffff
def ring(n):
    rg = [0x9a319039]
    for _ in range(1, 31): rg.append((rg[-1] * 0x41c64e6d + 0x3039) & M)
    p, q, out = 3, 0, []
    for i in range(0x136 + n):
        rg[p] = (rg[p] + rg[q]) & M; u = rg[p] >> 1; p += 1
        if p >= 31: p = 0; q += 1
        else:
            q += 1
            if q >= 31: q = 0
        if i >= 0x136: out.append(u)
    return out
f = np.float32
U = ring(4000)
R = [float(f(f(f(100) * f(u & 0x7fffffff)) * f(4.656612873e-10))) for u in U]
def regime(n):  # roll reads draw n (and n+1 if the first roll loses)
    if R[n] <= 20.0: return 'B'
    return 'C' if R[n + 1] <= 10.0 else 'A'
obs = {k: ('B' if k in (1, 4) else 'A') for k in range(12)}
cands = [n0 for n0 in range(0, 3000) if all(regime(n0 + k) == r for k, r in obs.items())]
print('n0 candidates reproducing K=0..11:', cands[:20], '(count %d)' % len(cands))
if cands:
    n0 = cands[0]
    pred = {k: regime(n0 + k) for k in range(12, 64)}
    print('n0 =', n0)
    print('predicted C phases in K=12..63:', [k for k, r in pred.items() if r == 'C'])
    print('predicted B phases in K=12..63:', [k for k, r in pred.items() if r == 'B'])
