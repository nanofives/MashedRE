"""u9775: pairwise first-split call between original captures (d2first.py dynamic-block word set), aligned on each side's first moving frame; plus each capture's
body-matrix pad word +0x954 and host. usage: splitmatrix.py <msd> [<msd> ...]"""
import sys, struct, os, json
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 're', 'tools', 'statediff'))
import msd_fields as m
f = lambda r, o: struct.unpack_from('<f', r, o)[0]
u = lambda r, o: struct.unpack_from('<I', r, o)[0]
SKIP = {0x954, 0x9a4} | {0x1c4 + w * 0xc4 + 4 * i for w in range(4) for i in range(12)}   # pads + wheel-history ring
caps = []
for p in sys.argv[1:]:
    L = [r for _, r in m.parse(p)]; a0 = next(i for i, r in enumerate(L) if f(r, 0x9e4) > 0.01)
    host = '?'
    try: host = json.load(open(p + '.provenance.json')).get('host', '?')
    except Exception: pass
    caps.append((os.path.basename(p).replace('.msd', ''), L[a0:], host))
OFFS = [o for a, b in ((0x928, 0x9f0), (0xb00, 0xb30)) for o in range(a, b, 4)
        if o not in {0x954, 0x984, 0x994, 0x9a4, 0x9d4, 0x9d8, 0x9dc}]   # d2first.py's set (dynamic block)
def split(A, B, lim=1500):
    for k in range(min(len(A), len(B), lim)):
        if any(u(A[k], o) != u(B[k], o) for o in OFFS): return k
    return None
names = [c[0] for c in caps]
print('capture host pad954 moving_calls')
for nm, L, h in caps: print('  %-10s %-11s %08x %d' % (nm, h, u(L[0], 0x954), len(L)))
print('first split k (row vs col):')
print(' ' * 11 + ''.join('%11s' % n for n in names))
for i, (ni, Li, _) in enumerate(caps):
    print('%-11s' % ni + ''.join('%11s' % ('-' if i == j else str(split(Li, Lj))) for j, (nj, Lj, _) in enumerate(caps)))
