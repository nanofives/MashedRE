"""Per-step series of one car's window (d3b lane): steer = c1 - c0, accel c4, brake c5, rec_9e4, substate, ai_mode,
own_x/own_z, look_x/look_z, curv, the ret* columns. Window = the 220 calls from the first c4 != 0 (ai_ctrl_window rule).
usage: py -3.12 series.py <step.csv> <car> [<from> <to>] [--cols a,b,c]"""
import csv, sys
p, v = sys.argv[1], int(sys.argv[2])
args = [a for a in sys.argv[3:] if not a.startswith('--')]
lo, hi = (int(args[0]), int(args[1])) if len(args) >= 2 else (0, 220)
cols = None
for a in sys.argv[3:]:
    if a.startswith('--cols='):
        cols = a.split('=', 1)[1].split(',')
rows = [r for r in csv.DictReader(open(p, newline='')) if int(r['v']) == v]
i0 = next(i for i, r in enumerate(rows) if int(r['c4']) != 0)
w = rows[i0:i0 + 220]
cols = cols or ['frame', 'c0', 'c1', 'c4', 'c5', 'ai_mode', 'substate', 'rec_9e4', 'own_x', 'own_z', 'curv', 'ret14a70', 'ret14c30', 'ret148b0']
print('k  steer ' + ' '.join(cols))
for k in range(lo, min(hi, len(w))):
    r = w[k]
    vals = []
    for c in cols:
        x = r.get(c, '')
        try:
            f = float(x)
            x = ('%d' % f) if f == int(f) and abs(f) < 1e7 else ('%.4f' % f)
        except ValueError:
            pass
        vals.append(x)
    print('%3d %5d %s' % (k, int(r['c1']) - int(r['c0']), ' '.join(vals)))
