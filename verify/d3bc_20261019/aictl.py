"""u9767: first car-v AI control / input difference, original aistep CSV vs port step CSV (rows from the first c4 != 0;
aistep row r = record call r - 1). usage: aictl.py <orig.aistep.csv> <port.step.csv> [v] [rows]"""
import csv, struct, sys
def f32(x):
    try: return struct.unpack('<f', struct.pack('<f', float(x)))[0]
    except ValueError: return None
def load(p, v):
    rows = [r for r in csv.DictReader(open(p, newline='')) if r['v'] == v]
    i0 = next(i for i, r in enumerate(rows) if int(r['c4']) != 0)
    return rows[i0:]
v = sys.argv[3] if len(sys.argv) > 3 else '1'; n = int(sys.argv[4]) if len(sys.argv) > 4 else 300
O, P = load(sys.argv[1], v), load(sys.argv[2], v)
ic = ['c0', 'c1', 'c3', 'c4', 'c5', 'ai_mode', 'ai_override', 'substate', 'ai_spline_idx', 'tgt_7ffc', 'flag_a368', 'march_idx0', 'march_n']
fc = ['curv', 'own_x', 'own_z', 'look_x', 'look_z', 'diff_a360', 'rec_9e4', 'hist_d8', 'c7', 'rec_b0c', 'step_1008']
fi = None
for k in range(1, min(len(O), len(P), n)):
    di = [c for c in ic if O[k][c] != '' and P[k][c] != '' and int(float(O[k][c])) != int(float(P[k][c]))]
    df = [c for c in fc if f32(O[k][c]) is not None and f32(P[k][c]) is not None and f32(O[k][c]) != f32(P[k][c])]
    if (di or df) and fi is None: fi = k
    if fi is not None and k < fi + 6:
        print(k, 'INT', ' '.join('%s o=%s p=%s' % (c, O[k][c], P[k][c]) for c in di), '| FLT',
              ' '.join('%s o=%.9g p=%.9g' % (c, f32(O[k][c]), f32(P[k][c])) for c in df[:5]))
print('first difference at aistep row', fi)
