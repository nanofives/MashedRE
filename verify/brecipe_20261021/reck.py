"""u9779: original MSD vs port MSD1 record dump on one k axis: k = the aistep row index from the first substate-6 row of the
original (car 3's aistep), and the port record-dump frame index. Both records are the state after frame k's physics (port dump:
end of the race frame; original: verified with MASHED_ELIM_TRACE, original record k = the port camera row k+1 = port record k).
Prints the first k where +0x958 / +0x960 / +0x9b0 / +0x9b8 differ, and the rows k0..k1. usage: reck.py <orig.msd> <port.rec.msd> [k0 k1]"""
import sys, csv, struct, os
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 're', 'tools', 'statediff'))
import msd_fields as m
f = lambda r, o: struct.unpack_from('<f', r, o)[0]
A = list(csv.DictReader(open(sys.argv[1] + '.aistep.csv')))
f0 = int(next(r for r in A if r['v'] == '3' and r['substate'] == '6')['frame'])
O = {fi - f0: rec for fi, rec in m.parse(sys.argv[1])}
P = {fi: rec for fi, rec in m.parse(sys.argv[2])}
OFF = (0x958, 0x960, 0x9b0, 0x9b8)
first = None
for k in sorted(P):
    if k not in O: continue
    if any(O[k][o:o + 4] != P[k][o:o + 4] for o in OFF): first = k; break
print('first difference k', first, 'deltas', None if first is None else [round(f(P[first], o) - f(O[first], o), 6) for o in OFF])
if len(sys.argv) > 3:
    for k in range(int(sys.argv[3]), int(sys.argv[4]) + 1):
        if k in O and k in P:
            print(k, 'O', [round(f(O[k], o), 4) for o in OFF], 'P', [round(f(P[k], o), 4) for o in OFF])
