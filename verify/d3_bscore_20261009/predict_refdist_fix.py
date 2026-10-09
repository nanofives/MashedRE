"""Offline prediction (PREREG_REFDIST_FIX.md): branch 2's first firing per AI car in the B2 arm if the
FUN_00442a60 bridge picks the reference as the pair's HIGHER race_pct car (the original's bytes,
0x00442b3a..0x00442b97), instead of the rearmost. Inputs: B2K00.gates.csv (ptr_x/ptr_z = record XZ,
racepct_ec = race pct). Pair = most separated in XZ (FUN_0040e180 uses 3D; y differences are small,
[UNCERTAIN] for near-ties). refdist[i] = 0.8 * |XZ(ref) - XZ(i)|. Gate (exit 114): refdist[v] == 0 and
refdist[0] > 6.5. Car states assumed 0 (no dying cars in the window)."""
import csv, math, itertools, sys
g = list(csv.DictReader(open(sys.argv[1] if len(sys.argv) > 1 else 'B2K00.gates.csv')))
by = {}
for x in g: by.setdefault(int(x['frame']), {})[int(x['v'])] = x
first = {}
for f in sorted(by):
    d = by[f]
    if len(d) < 4: continue
    P = {i: (float(d[i]['ptr_x']), float(d[i]['ptr_z'])) for i in range(4)}
    R = {i: float(d[i]['racepct_ec']) for i in range(4)}
    a, b = max(itertools.permutations(range(4), 2), key=lambda p: math.dist(P[p[0]], P[p[1]]))
    pa, pb = R[a], R[b]
    if pa > 80 and pb < 20: pa -= 100
    elif pb > 80 and pa < 20: pb -= 100
    ref = a if pa > pb else b
    rd = {i: 0.8 * math.dist(P[ref], P[i]) for i in range(4)}
    for v in (1, 2, 3):
        if v not in first and ref == v and rd[0] > 6.5: first[v] = f
print('predicted first branch-2 firing frame per car (corrected bridge):', {v: first.get(v) for v in (1, 2, 3)})
