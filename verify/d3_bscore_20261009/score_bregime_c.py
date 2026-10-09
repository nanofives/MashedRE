"""Regime-C sweep scorer (PREREG_BREGIME_C.md). Read-only. usage: py -3.12 score_bregime_c.py"""
import glob, importlib.util, json, sys, subprocess
spec = importlib.util.spec_from_file_location('w', '../../re/tools/ai_ctrl_window.py')
w = importlib.util.module_from_spec(spec); sys.argv = ['x']; spec.loader.exec_module(w)
g = {}; exec(open('predict_regime_k.py').read().split("obs = {")[0], g)
M = list(w.TOLERANCE.keys())
lab, sig = {}, {}
for p in sorted(glob.glob('K*.step.csv')):
    k = int(p[1:-9]); regs = w.window_regime(p, 220); st = w.window_stats(p, 220)
    rs = {regs.get(v) for v in (1, 2, 3)}
    lab[k] = rs.pop() if len(rs) == 1 else '?'
    sig[k] = tuple(tuple(st[v].get(m) for m in M) for v in (1, 2, 3))
ks = sorted(lab)
print('labels:', ' '.join('%d%s' % (k, lab[k]) for k in ks))
for n0 in (0, 926, 2642):
    mism = [k for k in ks if g['regime'](n0 + k) != lab[k]]
    print('n0=%4d mismatches %d %s' % (n0, len(mism), mism[:12]))
cs = [k for k in ks if lab[k] == 'C']
print('C phases:', cs, '| C2', 'HOLDS' if cs else 'FAILS')
if cs:
    print('C3 (identical windows):', 'HOLDS' if len({sig[k] for k in cs}) == 1 else 'FAILS')
    env = json.load(open('../../re/tools/b_envelopes.json'))['envelopes']['C']
    st = w.window_stats('K%02d.step.csv' % cs[0], 220)
    for v in (1, 2, 3):
        f = ['%s=%s not in [%s,%s]' % (m, st[v][m], lo, hi) for m, (lo, hi) in env.items() if not (lo <= st[v][m] <= hi)]
        print('  K%02d car %d (b-regime C) %s' % (cs[0], v, 'PASS' if not f else 'FAIL: ' + '; '.join(f)))
for r in 'ABC':
    ss = {sig[k] for k in ks if lab[k] == r}
    print('regime %s: %d phases, %d distinct window sets' % (r, sum(1 for k in ks if lab[k] == r), len(ss)))
