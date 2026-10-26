"""u9775: for original captures, align on first moving frame and report, around the split, the frame index per call,
whether each record equals the reference's record at k, k-1, k+1 (dynamic block 0x928..0x9f0), and the frame gaps.
usage: splitprobe.py <ref.msd> <other.msd> [lo] [hi]"""
import sys, struct, os
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 're', 'tools', 'statediff'))
import msd_fields as m
f = lambda r, o: struct.unpack_from('<f', r, o)[0]
u = lambda r, o: struct.unpack_from('<I', r, o)[0]
def mv(L): return next(i for i, (fi, r) in enumerate(L) if f(r, 0x9e4) > 0.01)
SKIP = {0x954, 0x984, 0x994, 0x9a4, 0x9d4, 0x9d8, 0x9dc}
offs = [o for o in range(0x928, 0x9f0, 4) if o not in SKIP]
def nd(a, b): return sum(1 for o in offs if u(a, o) != u(b, o))
A = list(m.parse(sys.argv[1])); B = list(m.parse(sys.argv[2])); a0, b0 = mv(A), mv(B)
lo = int(sys.argv[3]) if len(sys.argv) > 3 else 295; hi = int(sys.argv[4]) if len(sys.argv) > 4 else 330
print('records A %d (moving @%d)  B %d (moving @%d)' % (len(A), a0, len(B), b0))
for k in range(lo, hi):
    fa, ra = A[a0 + k]; fb, rb = B[b0 + k]
    fpa = A[a0 + k - 1][0]; fpb = B[b0 + k - 1][0]
    print('k=%3d  A fr %5d (gap %d)  B fr %5d (gap %d)  diff k:%3d  B vs A[k-1]:%3d  B vs A[k+1]:%3d  full-rec diff words %d'
          % (k, fa, fa - fpa, fb, fb - fpb, nd(ra, rb), nd(A[a0 + k - 1][1], rb), nd(A[a0 + k + 1][1], rb),
             sum(1 for o in range(0, min(len(ra), len(rb)) - 3, 4) if u(ra, o) != u(rb, o))))
