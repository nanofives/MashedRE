"""One car, window call k: own_x/own_z (the lookahead's only position inputs, AiStandalone.cpp LookReplayOnce) and speed,
original aistep vs port step dump at full precision; marks the calls where the look target differs.
usage: posdrift.py <orig.aistep.csv> <port.step.csv> <car> [<from> <to>]"""
import csv, sys, math


def win(p, v):
    rs = [r for r in csv.DictReader(open(p, newline='')) if r.get('v', '').strip() == str(v)]
    i0 = next(i for i, r in enumerate(rs) if int(r['c4']) != 0)
    return rs[i0:i0 + 220]


o, p, v = win(sys.argv[1], int(sys.argv[3])), win(sys.argv[2], int(sys.argv[3])), int(sys.argv[3])
k0, k1 = (int(sys.argv[4]), int(sys.argv[5])) if len(sys.argv) > 5 else (0, 30)
print('  k      dx          dz        |d|       dspeed     look o/p z            c0 o/p')
for k in range(k0, min(k1, len(o), len(p))):
    a, b = o[k], p[k]
    dx = float(b['own_x']) - float(a['own_x']); dz = float(b['own_z']) - float(a['own_z'])
    ds = float(b['rec_9e4']) - float(a['rec_9e4'])
    lk = '' if a['look_z'] == b['look_z'] else '*'
    print('%3d %+.3e %+.3e %.3e %+9.4f  %8.4f/%8.4f %s  %s/%s' % (k, dx, dz, math.hypot(dx, dz), ds, float(a['look_z']),
                                                             float(b['look_z']), lk, a['c0'], b['c0']))
