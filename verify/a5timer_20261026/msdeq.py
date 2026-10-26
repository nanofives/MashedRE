"""a5timer G-ON-REC: record-for-record equality of two MSD dumps over their common length.
usage: msdeq.py <a.msd> <b.msd>   prints n_a, n_b, common, the first differing record (or none) and its first 8 differing dword offsets"""
import os, struct, sys
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 're', 'tools', 'statediff'))
import msd_fields as m

A = list(m.parse(sys.argv[1])); B = list(m.parse(sys.argv[2]))
n = min(len(A), len(B)); first = None
for i in range(n):
    if A[i][1] != B[i][1]:
        first = i; break
print('n_a=%d n_b=%d common=%d first_diff=%s' % (len(A), len(B), n, first))
if first is not None:
    ra, rb = A[first][1], B[first][1]
    offs = [o for o in range(0, min(len(ra), len(rb)) - 3, 4) if ra[o:o + 4] != rb[o:o + 4]]
    print('  frame a=%s b=%s ndiff=%d offs=%s' % (A[first][0], B[first][0], len(offs), ' '.join('0x%x' % o for o in offs[:8])))
    for o in offs[:8]:
        print('  +0x%03x a=%08x (%g) b=%08x (%g)' % (o, struct.unpack_from('<I', ra, o)[0], struct.unpack_from('<f', ra, o)[0],
                                                    struct.unpack_from('<I', rb, o)[0], struct.unpack_from('<f', rb, o)[0]))
