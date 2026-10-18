"""Per window call k: own_x/own_z of each AI car, original aistep against port step dump, and the
car a <-> car b planar distance on each side. own_x/own_z is the position the AI step read (the
port's is stale on FUN_00414a70 return-2 calls, memory ret2-calls-have-no-lookahead-and-stale-own-pos).
usage: pos13.py <orig.aistep.csv> <port.step.csv> [--k0 0 --k1 80 --pair 1,3 --every 1]"""
import csv, sys, math, collections


def load(p):
    rows = collections.defaultdict(list)
    for r in csv.DictReader(open(p, newline='')):
        rows[int(r['v'])].append(r)
    win = {}
    for v, rs in rows.items():
        i0 = next((i for i, x in enumerate(rs) if int(x['c4']) != 0), None)
        win[v] = rs[i0:] if i0 is not None else []
    return win


def arg(name, d):
    a = sys.argv
    return a[a.index(name) + 1] if name in a else d


def main():
    o, p = load(sys.argv[1]), load(sys.argv[2])
    k0, k1, ev = int(arg('--k0', 0)), int(arg('--k1', 80)), int(arg('--every', 1))
    a, b = map(int, arg('--pair', '1,3').split(','))
    print('k   ' + '  '.join('d%d(x,z)          ' % v for v in (1, 2, 3)) + ' dist%d%d o / p   ov%d o/p ov%d o/p' % (a, b, a, b))
    for k in range(k0, k1, ev):
        try:
            ro = {v: o[v][k] for v in (1, 2, 3)}
            rp = {v: p[v][k] for v in (1, 2, 3)}
        except IndexError:
            break
        xy = lambda r: (float(r['own_x']), float(r['own_z']))
        parts = []
        for v in (1, 2, 3):
            (ox, oz), (px, pz) = xy(ro[v]), xy(rp[v])
            parts.append('%+8.4f,%+8.4f' % (px - ox, pz - oz))
        do = math.dist(xy(ro[a]), xy(ro[b]))
        dp = math.dist(xy(rp[a]), xy(rp[b]))
        print('%-3d %s  %7.4f / %7.4f  %4s/%4s %4s/%4s' % (k, '  '.join(parts), do, dp, ro[a]['ai_override'], rp[a]['ai_override'],
                                                       ro[b]['ai_override'], rp[b]['ai_override']))


main()
