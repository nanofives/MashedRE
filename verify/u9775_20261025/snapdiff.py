"""u9775: align each .data snapshot (scenario_launch --snap-probe, taken at entry of the physics dispatcher 0x00470c70)
to its record call k by matching the player record inside the image (0x008815a0) against the run's MSD capture, then
diff two runs' images at equal k. Prints, per k, the differing dword count and a classification of differing addresses:
'always' (differ in every common k), 'onset' (equal at the first common k, differ later, with the onset k).
usage: snapdiff.py <a.msd> <b.msd> [--show N] [--lo ADDR --hi ADDR]"""
import sys, struct, os, collections
import numpy as np
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 're', 'tools', 'statediff'))
import msd_fields as m
BASE, SZ, REC = 0x005ea000, 0x32a800, 0x008815a0
f = lambda r, o: struct.unpack_from('<f', r, o)[0]
OFFS = [o for a, b in ((0x928, 0x9f0), (0xb00, 0xb30)) for o in range(a, b, 4) if o not in {0x954, 0x984, 0x994, 0x9a4, 0x9d4, 0x9d8, 0x9dc}]
def load(msd):
    L = [r for _, r in m.parse(msd)]; a0 = next(i for i, r in enumerate(L) if f(r, 0x9e4) > 0.01)
    key = lambda r: b''.join(r[o:o + 4] for o in OFFS)
    idx = {}
    for k in range(0, len(L) - a0):
        idx.setdefault(key(L[a0 + k]), k)
    raw = open(msd + '.snap.bin', 'rb').read(); n = len(raw) // SZ
    out = {}
    for i in range(n):
        img = raw[i * SZ:(i + 1) * SZ]; r = img[REC - BASE: REC - BASE + 0xd04]
        k = idx.get(key(r))
        out[k] = np.frombuffer(img, dtype='<u4')
        print('  %s snap %d -> record k %s' % (os.path.basename(msd), i, k))
    return out
args = sys.argv[1:]
show = int(args[args.index('--show') + 1]) if '--show' in args else 40
A, B = load(args[0]), load(args[1])
ks = sorted(k for k in A if k is not None and k in B)
print('common k:', ks)
D = {k: np.nonzero(A[k] != B[k])[0] for k in ks}
for k in ks: print('k=%d (entry of the call producing record %d): %d dwords differ' % (k, k + 1, len(D[k])))
allk = set(D[ks[0]].tolist())
for k in ks[1:]: allk &= set(D[k].tolist())
onset = {}
for k in ks:
    for i in D[k].tolist():
        if i not in onset: onset[i] = k
first = ks[0]
on = sorted((k, i) for i, k in onset.items() if k > first)
print('differ at every common k: %d dwords; first-common-k diffs not in every k: %d; later onsets: %d'
      % (len(allk), len(set(D[first].tolist()) - allk), len(on)))
def va(i): return BASE + 4 * i
def fl(x): return struct.unpack('<f', struct.pack('<I', int(x)))[0]
print('-- onsets (k, va, a, b) first %d --' % show)
for k, i in on[:show]: print('  k=%d  0x%08x  %08x / %08x  (%.9g / %.9g)' % (k, va(i), A[k][i], B[k][i], fl(A[k][i]), fl(B[k][i])))
grp = collections.Counter((va(i) & ~0xfff) for i in allk)
print('-- always-differing dwords by 4 KB page (top 30) --')
for pg, c in grp.most_common(30): print('  0x%08x  %d' % (pg, c))
