"""Side-by-side of one car's (b) window, original aistep row k vs port step-dump row k (window = the 220 calls from the
first c4 != 0, ai_ctrl_window rule). Prints the columns both files carry, and the first k where each differs.
usage: py -3.12 sbs.py <orig aistep.csv> <port step.csv> <car> [<from> <to>] [--cols=a,b] [--first]"""
import csv, sys

def win(p, v):
    rows = [r for r in csv.DictReader(open(p, newline='')) if int(r['v']) == v]
    i0 = next(i for i, r in enumerate(rows) if int(r['c4']) != 0)
    return rows[i0:i0 + 220], rows[:i0]

o_path, p_path, v = sys.argv[1], sys.argv[2], int(sys.argv[3])
args = [a for a in sys.argv[4:] if not a.startswith('--')]
lo, hi = (int(args[0]), int(args[1])) if len(args) >= 2 else (0, 220)
cols = ['c0', 'c1', 'c4', 'c5', 'ai_mode', 'substate', 'flag_a368', 'rec_9e4', 'own_x', 'own_z', 'look_x', 'look_z', 'curv']
for a in sys.argv[4:]:
    if a.startswith('--cols='):
        cols = a.split('=', 1)[1].split(',')
O, Opre = win(o_path, v)
P, Ppre = win(p_path, v)
print('pre-window calls: orig %d port %d' % (len(Opre), len(Ppre)))
if '--first' in sys.argv:
    for c in cols:
        k = next((k for k in range(min(len(O), len(P))) if O[k].get(c) != P[k].get(c)
                  and not _close(O[k].get(c), P[k].get(c))), None) if False else None
    def num(x):
        try: return float(x)
        except (TypeError, ValueError): return None
    for c in cols:
        first = None
        for k in range(min(len(O), len(P))):
            a, b = num(O[k].get(c)), num(P[k].get(c))
            if a is None or b is None:
                continue
            if abs(a - b) > 1e-3 * max(1.0, abs(a)):
                first = k; break
        print('%-10s first diff k=%s' % (c, first))
    sys.exit(0)
print('  k  st_o st_p | ' + ' | '.join('%s o/p' % c for c in cols))
for k in range(lo, min(hi, len(O), len(P))):
    o, p = O[k], P[k]
    so, sp = int(o['c1']) - int(o['c0']), int(p['c1']) - int(p['c0'])
    def f(x):
        try:
            y = float(x)
            return ('%d' % y) if y == int(y) and abs(y) < 1e7 else ('%.3f' % y)
        except (TypeError, ValueError):
            return str(x)
    print('%3d %5d %5d%s | ' % (k, so, sp, ' *' if so != sp else '  ') + ' | '.join('%s/%s' % (f(o.get(c)), f(p.get(c))) for c in cols))
