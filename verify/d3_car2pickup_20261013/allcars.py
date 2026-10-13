import sys, math
sys.path.insert(0, __import__('os').path.dirname(__file__))
import traj as T

kind, path = sys.argv[1], sys.argv[2]
a, b = int(sys.argv[3]), int(sys.argv[4])
if kind == 'orig':
    rows = T.orig_rows(path)
    i6 = next(i for i, r in enumerate(rows) if r[2] == 6)
    seq = [(k, rows[i6 + k][3]) for k in range(a, b)]
else:
    rows = T.exe_rows(path)
    seq = [(fr, c) for fr, c in rows if a <= fr < b]
for k, cars in seq:
    cs = [cars[i] if (isinstance(cars, list) or i in cars) else None for i in range(4)]
    s = ' | '.join('%d:%.4f,%.4f,%.4f' % (i, p[0], p[1], p[2]) if p else '%d:-' % i for i, p in enumerate(cs))
    d02 = math.dist(cs[0][::2], cs[2][::2]) if cs[0] and cs[2] else float('nan')
    d12 = math.dist(cs[1][::2], cs[2][::2]) if cs[1] and cs[2] else float('nan')
    print(k, s, 'xz02=%.4f xz12=%.4f' % (d02, d12))
