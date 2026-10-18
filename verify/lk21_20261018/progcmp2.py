"""Own progress (0x008a96e8 + v*0x30c) at each car's FUN_00414a70 call, window call k, original against port.
Original: aistep window rows (first c4 != 0) joined to wobj_ins rows on (v, clk_0ff4 == wobj frame column).
Port: step-dump window rows joined to MASHED_TGT3_TRACE rows of the same car by order (counts checked).
usage: progcmp2.py <orig.msd> <port arm prefix> [<from> <to>]   (prefix: arms/B_ST_K1 -> .step.csv + .tgt3.csv)"""
import csv, sys, collections


def by_v(path):
    d = collections.defaultdict(list)
    for r in csv.DictReader(open(path, newline='')):
        d[int(r['v'])].append(r)
    return d


o_msd, pre = sys.argv[1], sys.argv[2]
k0, k1 = (int(sys.argv[3]), int(sys.argv[4])) if len(sys.argv) > 4 else (30, 60)
oa, ow = by_v(o_msd + '.aistep.csv'), by_v(o_msd + '.wobj_ins.csv')
ps, pt = by_v(pre + '.step.csv'), by_v(pre + '.tgt3.csv')
oprog, pprog, oret, pret = {}, {}, {}, {}
for v in (1, 2, 3):
    wk = {}
    for r in ow[v]:
        wk.setdefault(int(r['frame']), r)
    rows = oa[v]
    i0 = next(i for i, r in enumerate(rows) if int(r['c4']) != 0)
    for k in range(k0, k1):
        r = rows[i0 + k]
        w = wk.get(int(r['clk_0ff4']))
        oprog[v, k] = float(w['prog%d' % v]) if w else None
        oret[v, k] = r['ret14a70']
    srows, trows = ps[v], pt[v]
    if len(srows) != len(trows):
        print('port car %d: step rows %d != tgt3 rows %d (order join refused for this car)' % (v, len(srows), len(trows)))
        continue
    j0 = next(i for i, r in enumerate(srows) if int(r['c4']) != 0)
    for k in range(k0, k1):
        pprog[v, k] = float(trows[j0 + k]['prog'])
        pret[v, k] = trows[j0 + k]['ret']
print(' k  | ret o/p  car1   car2   car3 | o prog1   prog2    prog3  | p prog1   prog2    prog3  | o 3-2     p 3-2')
for k in range(k0, k1):
    f = lambda d, v: ('%8.5f' % d[v, k]) if d.get((v, k)) is not None else '       -'
    rr = '  '.join('%s/%s' % (oret[v, k], pret.get((v, k), '-')) for v in (1, 2, 3))
    def gap(d):
        a, b = d.get((3, k)), d.get((2, k))
        if a is None or b is None:
            return '      -'
        g = a - b
        g = g - 30 if g > 15 else g + 30 if g < -15 else g
        return '%+.5f' % g
    print('%3d | %s | %s %s %s | %s %s %s | %s %s' % (k, rr, f(oprog, 1), f(oprog, 2), f(oprog, 3), f(pprog, 1), f(pprog, 2),
                                                      f(pprog, 3), gap(oprog), gap(pprog)))
