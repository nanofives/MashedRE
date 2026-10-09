"""D-11073 leg 2 scorer (PREREG_LEG2.md §5). Read-only. usage: py -3.12 score_leg2.py L2
G-STATIC     log: track 30, count 7, types 0,1,1,1,2,2,3, e0 (1.5,9.7,34.5), a4 12, 5/180/-15, draw 4 (310+4), fc0 0
G-RULE       trace: the site-(a) distance (target shifted by min(L,4)/3, position BEFORE the call's move) is
             recomputed in float32 from row r-1's logged position and target; at the clear row it is < 0.02f and
             on every earlier phase-3 row >= 0.02f. Site (b) is ruled out by census: no exe writer of
             0x007f1042 + k*0x4c (BSS 0), recorded in RESULT_LEG2.md.
G-FRAME      log 'phase 3 released after N frames', N = 764 +-2 (PREREG §4)
G-TRANSCRIBE leg2_model.py on the arm's own trace vs the logged entry-0 (x,z), every phase-3 row, |diff| <= 1e-3
G-CURVE      raw distance (trace dist_raw at p3f = k) vs the original's at f = 60..540, within 10 %
G-AFTER      exactly one release line; FUN_00414180 lines = rounds; exits read 0x3feeeee2 (score_leg1b_gates.py)
"""
import csv, math, re, sys, importlib.util
import numpy as np
f = np.float32
arm = sys.argv[1]
log = open(arm + '.log', encoding='utf-8', errors='replace').read()
ok_all = True
def gate(name, ok, msg):
    global ok_all
    ok_all &= bool(ok); print('%-13s %s  %s' % (name, 'PASS' if ok else 'FAIL', msg))

m = re.search(r"FUN_00442600 track=(\d+) count=(\d+) types=([\d,]+) e0=\(([^)]*)\) a4=(\S+) ac/b0/b4=(\S+) draws_before=(\d+) fc0=(-?\d+)", log)
gate('G-STATIC', m and m[1] == '30' and m[2] == '7' and m[3] == '0,1,1,1,2,2,3' and m[4] == '1.5,9.7,34.5'
     and m[5] == '12' and m[6] == '5/180/-15' and int(m[7]) - 310 == 4 and m[8] == '0', m.group(0) if m else 'no line')

rows = [r for r in csv.DictReader(open(arm + '.trace.csv'))]
p3 = []
for r in rows:
    if r['ba8c'] != '3': break
    p3.append(r)
clear = next(i for i, r in enumerate(p3) if r['flag'] == '0') if any(r['flag'] == '0' for r in p3) else None
# row i's step reads position from row i-1 (logged after call i-1) and target as copied by call i-1 (row i-1's tx);
# the trace row i-1 carries both. Recompute with that pairing:
def site_a(i):
    if i == 0: return None
    prv = p3[i - 1]
    px, pz, tx, tz = f(prv['e0x']), f(prv['e0z']), f(prv['tx']), f(prv['tz'])
    dx, dz = f(px - tx), f(pz - tz); L = f(np.sqrt(f(dx * dx + dz * dz)))
    if L > f(4): L = f(4)
    L3 = f(L * f(0.3333333432674408)); sx, sz = f(tx + L3), f(L3 + tz)
    dx, dz = f(px - sx), f(pz - sz)
    return float(f(np.sqrt(f(dx * dx + dz * dz))))
if clear is None:
    gate('G-RULE', False, 'flag never cleared in phase 3')
else:
    dc = site_a(clear); earlier = [site_a(i) for i in range(1, clear)]
    gate('G-RULE', dc < 0.02 and all(d >= 0.02 for d in earlier),
         'clear at p3f %s, site-(a) d = %.6f; min earlier d = %.6f (%d rows)' % (p3[clear]['p3f'], dc, min(earlier), len(earlier)))

mm = re.findall(r"phase 3 released after (\d+) frames", log)
gate('G-FRAME', len(mm) == 1 and abs(int(mm[0]) - 764) <= 2, 'released after %s (predicted 764 +-2)' % mm)

spec = importlib.util.spec_from_file_location('m', 'leg2_model.py'); mod = importlib.util.module_from_spec(spec)
exec(open('leg2_model.py').read().split("if __name__ == '__main__':")[0], mod.__dict__)
out, rel = mod.run_port(arm + '.trace.csv')
worst = 0.0
for (p3f, mx, mz, _, _), r in zip(out, p3):
    worst = max(worst, abs(mx - float(r['e0x'])), abs(mz - float(r['e0z'])))
gate('G-TRANSCRIBE', worst <= 1e-3, 'max |model - port| over %d rows = %.3g (model clear p3f %s)' % (len(out), worst, rel))

o = [r for r in csv.DictReader(open('o_cadence_nopress.csv')) if r['substate'] == '3']
t0 = float(o[1]['t'])
od = {round((float(r['t']) - t0) * 60): math.hypot(float(r['e0_x']) - float(r['tgt_fe4']), float(r['e0_z']) - float(r['tgt_fec'])) for r in o[1:]}
byf = {int(r['p3f']): float(r['dist_raw']) for r in p3}
res = [(k, byf[k], od[k], byf[k] / od[k]) for k in range(60, 541, 60)]
gate('G-CURVE', all(abs(x[3] - 1) <= 0.10 for x in res),
     ' '.join('f%d:%.3f' % (k, q) for k, _, _, q in res))

nrel = len(mm); n414 = len(re.findall(r"FUN_00414180 draws_before", log)); nexit = re.findall(r"d588_read=\S+ \((0x[0-9a-f]+)\)", log)
gate('G-AFTER', nrel == 1 and n414 == len(nexit) and all(x == '0x3feeeee2' for x in nexit),
     'release lines %d, rounds %d, exits %s (phase-5 detail: score_leg1b_gates.py %s)' % (nrel, n414, nexit, arm))
print('ALL', 'PASS' if ok_all else 'FAIL (see rows)')
