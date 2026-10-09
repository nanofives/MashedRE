"""(b) ring-phase sweep scorer (PREREG_BSCORE.md). Read-only. usage: py -3.12 score_bscore.py"""
import csv, glob, importlib.util, sys
spec = importlib.util.spec_from_file_location('w', '../../re/tools/ai_ctrl_window.py')
w = importlib.util.module_from_spec(spec); sys.argv = ['x']; spec.loader.exec_module(w)
M = ['c0_distinct', 'c1_distinct', 'steer_distinct', 'c0_median', 'c1_median', 'abs_steer_median',
     'accel_distinct', 'accel_median', 'brake_distinct', 'brake_median']
def flag_hist(path, n=220):
    rows = list(csv.DictReader(open(path, newline='')))
    out = {}
    for v in sorted({int(r['v']) for r in rows}):
        r = [x for x in rows if int(x['v']) == v]
        i0 = next((i for i, x in enumerate(r) if int(x['c4']) != 0), None)
        if i0 is None: continue
        win = r[i0:i0 + n]
        # compress the flag series into (value, run-length) pairs
        seq = []
        for x in win:
            f = x.get('flag_a368', '?')
            if seq and seq[-1][0] == f: seq[-1][1] += 1
            else: seq.append([f, 1])
        out[v] = tuple((a, b) for a, b in seq)
    return out
res = {}
for p in sorted(glob.glob('K*.step.csv')):
    k = int(p[1:3]); st = w.window_stats(p, 220); fh = flag_hist(p)
    res[k] = {v: (tuple(st[v].get(m) for m in M), fh.get(v), st[v]) for v in st if v > 0}
print('K   car  metrics(c0#,c1#,steer#,c0~,c1~,abs~,acc#,acc~,brk#,brk~)          368 history in window              pooled-envelope')
for k in sorted(res):
    for v, (mt, fh, s) in sorted(res[k].items()):
        fails = w.check(s) if not s.get('short') else ['SHORT']
        print('%2d  %d   %-58s %-36s %s' % (k, v, mt, fh, 'PASS' if not fails else 'FAIL %d' % len(fails)))
print()
for v in (1, 2, 3):
    outs = {res[k][v][0] for k in res if v in res[k]}
    passes = sum(1 for k in res if v in res[k] and not w.check(res[k][v][2]))
    print('car %d: %d distinct outcomes over %d K (B2: <=3); pooled-envelope pass %d/%d (B4)' % (v, len(outs), len(res), passes, len(res)))
# B3: same 368 history -> same metrics
bad = []
for v in (1, 2, 3):
    by = {}
    for k in res:
        if v not in res[k]: continue
        by.setdefault(res[k][v][1], set()).add(res[k][v][0])
    bad += [(v, h) for h, s in by.items() if len(s) > 1]
print('B3 (same 368 history -> same metrics):', 'HOLDS' if not bad else 'FAILS on %d (car, history) groups' % len(bad))
# B1
ctrl = w.window_stats('../d3_gatefire_20261008/L2_CTRL.step.csv', 220)
same = all(tuple(ctrl[v].get(m) for m in M) == res[0][v][0] for v in (1, 2, 3))
print('B1 (K=0 == L2_CTRL window):', 'HOLDS' if same else 'FAILS')
