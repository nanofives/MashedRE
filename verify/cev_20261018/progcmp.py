"""Per-car progress (0x008a96e8 + v*0x30c) at FUN_00414a70 entry: original wobj_ins rows (scenario_launch --wobj-probe)
aligned to the aistep window by per-car call order, against the port step dump's prog column (car v's own
progress at its step) -- and, for the original, every car's progress as seen by car v's call.
usage: progcmp.py <orig.msd> <port.step.csv> <car> [<from> <to>]
<orig.msd> = path prefix carrying .aistep.csv and .wobj_ins.csv."""
import csv, sys, collections


def by_v(path):
    d = collections.defaultdict(list)
    for r in csv.DictReader(open(path, newline='')):
        d[int(r['v'])].append(r)
    return d


o_msd, p_step, v = sys.argv[1], sys.argv[2], int(sys.argv[3])
k0, k1 = (int(sys.argv[4]), int(sys.argv[5])) if len(sys.argv) > 5 else (0, 80)
ai = by_v(o_msd + '.aistep.csv')
wi = by_v(o_msd + '.wobj_ins.csv')
pp = by_v(p_step)
oa = ai[v]
called = [i for i, r in enumerate(oa) if r['ret14a70'] not in ('-1', '')]
print('orig car %d: aistep rows %d, rows with ret14a70 recorded %d, wobj_ins rows %d' % (v, len(oa), len(called), len(wi[v])))
i0 = next(i for i, r in enumerate(oa) if int(r['c4']) != 0)
# FUN_00414a70 call j <-> the j-th aistep row whose ret14a70 is recorded
mapi = {ai_i: j for j, ai_i in enumerate(called)}
pa = pp[v]
j0 = next(i for i, r in enumerate(pa) if int(r['c4']) != 0)
print(' k  ret o/p | o prog1 prog2 prog3 (as read by car %d) | p prog(own)  | o own - p own' % v)
for k in range(k0, k1):
    r = oa[i0 + k]
    j = mapi.get(i0 + k)
    w = wi[v][j] if j is not None and j < len(wi[v]) else None
    po = pa[j0 + k] if j0 + k < len(pa) else None
    ostr = ' '.join('%.5f' % float(w['prog%d' % c]) for c in (1, 2, 3)) if w else '-'
    own_o = float(w['prog%d' % v]) if w else None
    own_p = float(po['prog']) if po and po.get('prog') not in (None, '') else None
    d = '%+.5f' % (own_o - own_p) if own_o is not None and own_p is not None else '-'
    print('%3d  %2s/%2s | %s | %s | %s' % (k, r['ret14a70'], po['ret14a70'] if po else '-', ostr,
                                        '%.5f' % own_p if own_p is not None else '-', d))
