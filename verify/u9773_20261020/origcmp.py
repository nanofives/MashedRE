"""u9773: first record call where two ORIGINAL captures of the same car differ (each aligned on its own aistep c4 onset).
usage: origcmp.py <a.msd> <b.msd> <car>"""
import sys, csv, struct, os
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 're', 'tools', 'statediff'))
import msd_fields as m
u = lambda r, o: struct.unpack_from('<I', r, o)[0]
def recs(msd, v):
    rows = [r for r in csv.DictReader(open(msd + '.aistep.csv', newline='')) if r['v'] == v]
    i0 = next(i for i, r in enumerate(rows) if int(r['c4']) != 0); f0 = int(rows[i0]['frame'])
    return {fi - f0: rec for fi, rec in m.parse(msd)}
A, B = recs(sys.argv[1], sys.argv[3]), recs(sys.argv[2], sys.argv[3])
for k in range(0, 400):
    if k not in A or k not in B: print('equal through', k - 1, '(end of one capture)'); break
    d = [o for o in list(range(0x928, 0x9f0, 4)) if o not in (0x954, 0x964, 0x984, 0x994, 0x9a4) and u(A[k], o) != u(B[k], o)]
    if d: print('first differing call', k, 'offsets', ' '.join('%x' % o for o in d[:8])); break
else: print('equal through', k)
