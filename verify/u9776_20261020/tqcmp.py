"""u9776: original --tri-probe rows vs the port's MASHED_WCS_TRILOG .tq.csv rows of one car, n-th alignment vs n-th,
field by field (hex words; ulp = integer distance of the float bits).
usage: tqcmp.py <orig.triprobe.csv> <port.tq.csv> <car> [n]"""
import sys, csv, struct
def f(h): return struct.unpack('<f', struct.pack('<I', int(h, 16)))[0]
def ulp(a, b):
    ia, ib = int(a, 16), int(b, 16)
    sa = ia if ia < 0x80000000 else 0x80000000 - ia; sb = ib if ib < 0x80000000 else 0x80000000 - ib
    return sb - sa
O = [r for r in csv.DictReader(open(sys.argv[1])) if r['car'] == sys.argv[3]]
P = []
for line in open(sys.argv[2]):
    t = line.strip().split(',')
    if t[0] == 'TQ' and t[1] == sys.argv[3]:
        P.append(dict(sub=t[2], angle=t[3], e0=t[4], axis=t[5], lac=t[6], m=t[7], ring_pre=t[8], ring_post=t[9]))
n = int(sys.argv[4]) if len(sys.argv) > 4 else min(len(O), len(P))
print(f'orig {len(O)} rows (frames {[r["frame"] for r in O]}), port {len(P)} rows (subs {[p["sub"] for p in P]})')
SKIP = {'m': {3, 7, 11, 15}, 'ring_pre': {3, 7, 11, 15}, 'ring_post': {3, 7, 11, 15}}   # flags / pad words
for i in range(min(n, len(O), len(P))):
    o, p = O[i], P[i]
    out = []
    for k in ('angle', 'e0', 'axis', 'lac', 'm', 'ring_pre', 'ring_post'):
        if k == 'lac':
            if o[k] != p[k]: out.append(f'lac {o[k]}/{p[k]}')
            continue
        a, b = o[k].split(';'), p[k].split(';')
        d = [(j, ulp(x, y), f(x)) for j, (x, y) in enumerate(zip(a, b)) if j not in SKIP.get(k, ()) and x != y]
        out.append(f'{k}: ' + ('=' if not d else ' '.join(f'[{j}]{u:+d}ulp({v:.6g})' for j, u, v in d)))
    print(f'#{i} frame {o["frame"]} sub {p["sub"]} | ' + ' | '.join(out))
