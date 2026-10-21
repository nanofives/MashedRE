"""u9778: VehicleCarCarContact 0x00469df0 rows, original (--carcar-probe) vs port (MASHED_CARCAR_RECDUMP), aligned on k.
Original k = sdframe - car-3 aistep c4 onset frame; port k = race frame - the port step dump's car-3 onset (recpos0.py's alignment).
Rows are matched on (k, self, other, pass). Per matched row: the record offsets (dword) that differ at entry and at leave.
usage: ccdiff.py <orig.msd> <port.carcar.tsv> <port.step.csv> [kmin] [kmax] [maxoffs]"""
import csv, struct, sys


def onset(f):
    return int(next(r for r in csv.DictReader(open(f)) if r['v'] == '3' and int(r['c4']) != 0)['frame'])


def load(path, f0):
    out = []
    for ln in open(path):
        c = ln.rstrip('\n').split('\t')
        if not c[0].lstrip('-').isdigit(): continue
        rec = [bytes.fromhex(x) for x in (c[5], c[6], c[8], c[9])]
        out.append((int(c[0]) - f0, int(c[2]), int(c[3]), int(c[4]), int(c[7]), rec))
    return out


def dif(a, b, n):
    d = []
    for o in range(0, min(len(a), len(b)), 4):
        if a[o:o + 4] != b[o:o + 4]:
            fa, fb = struct.unpack_from('<f', a, o)[0], struct.unpack_from('<f', b, o)[0]
            d.append(f'+0x{o:03x} {a[o:o+4][::-1].hex()}/{b[o:o+4][::-1].hex()} ({fa:.7g}/{fb:.7g})')
    return f'{len(d)} differ: ' + '; '.join(d[:n]) if d else 'equal'


O = load(sys.argv[1] + '.carcar.tsv', onset(sys.argv[1] + '.aistep.csv'))
P = load(sys.argv[2], onset(sys.argv[3]))
kmin = int(sys.argv[4]) if len(sys.argv) > 4 else -10**9
kmax = int(sys.argv[5]) if len(sys.argv) > 5 else 10**9
n = int(sys.argv[6]) if len(sys.argv) > 6 else 12
Pk = {}
for r in P: Pk.setdefault(r[:4], []).append(r)
Ok = {}
for r in O: Ok.setdefault(r[:4], []).append(r)
print('orig rows', len(O), 'port rows', len(P))
for key in sorted(set(Ok) | set(Pk)):
    if not (kmin <= key[0] <= kmax): continue
    a, b = Ok.get(key, []), Pk.get(key, [])
    if not a or not b:
        print(f'k={key[0]} self={key[1]} oth={key[2]} pass={key[3]}: ONLY in {"orig" if a else "port"} (ret {(a or b)[0][4]})')
        continue
    for i, (x, y) in enumerate(zip(a, b)):
        print(f'k={key[0]} self={key[1]} oth={key[2]} pass={key[3]} #{i} ret {x[4]}/{y[4]}')
        for lab, j in (('self_in ', 0), ('other_in', 1), ('self_out', 2), ('oth_out ', 3)):
            print(f'   {lab} {dif(x[5][j], y[5][j], n)}')
