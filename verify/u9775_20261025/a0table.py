"""u9775: per original capture, the MSD frame index of the first moving call (a0) and the frame of the first call where
it leaves a reference capture (d2first.py word set). usage: a0table.py <ref.msd> <msd> [<msd> ...]"""
import sys, struct, os
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 're', 'tools', 'statediff'))
import msd_fields as m
f = lambda r, o: struct.unpack_from('<f', r, o)[0]
u = lambda r, o: struct.unpack_from('<I', r, o)[0]
OFFS = [o for a, b in ((0x928, 0x9f0), (0xb00, 0xb30)) for o in range(a, b, 4) if o not in {0x954, 0x984, 0x994, 0x9a4, 0x9d4, 0x9d8, 0x9dc} | ({0xb2c} if os.environ.get("U9775_SKIP_B2C") else set())]
def load(p):
    L = list(m.parse(p)); a0 = next(i for i, (fi, r) in enumerate(L) if f(r, 0x9e4) > 0.01); return L, a0
R, r0 = load(sys.argv[1])
print('%-12s %6s %6s %8s %8s' % ('capture', 'a0', 'a0fr', 'split_k', 'split_fr'))
for p in sys.argv[1:]:
    L, a0 = load(p); sk = None
    for k in range(min(len(L) - a0, len(R) - r0)):
        if any(u(L[a0 + k][1], o) != u(R[r0 + k][1], o) for o in OFFS): sk = k; break
    print('%-12s %6d %6d %8s %8s' % (os.path.basename(p).replace('.msd', ''), a0, L[a0][0], sk, (L[a0 + sk][0] if sk is not None else '-')))
