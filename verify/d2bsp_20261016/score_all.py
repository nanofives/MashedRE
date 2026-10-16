"""d2bsp scorer (PREREG_d2bsp.md section 3): D2 (score_d2.py, bounds d81a8df6), recovery H1 (a8_launch.py),
D3 (b) (score_regimes.py) and collateral (coll.py). Read-only on the captures; writes SCORE_<what>[_<suffix>].txt.
usage: py -3.12 score_all.py d2 <suffix> <D2 arm> [...]       (arm = D2_W_1 -> D2_W_1.motion_diag.log)
       py -3.12 score_all.py b <suffix> <B stack> [...]       (stack = BASE -> B_BASE_K{0,1,26}.step.csv)
       py -3.12 score_all.py coll <suffix> <H arm> [...]      (arm = H_W -> H_W.step.csv + H_W-2.step.csv)
H1: >= 200 of the 400 post-trough frames >= 100 and median >= 900, per run."""
import os, re, subprocess, sys
W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
OUT = os.environ.get('D2B_OUT', os.path.join(W, r'verify\d2bsp_20261016'))
M = r'C:\Users\maria\Desktop\Proyectos\Mashed'
ORIG = os.path.join(M, r'verify\d2_reopen_20260929\orig_solo3.msd')
PY = sys.executable


def sh(*a):
    p = subprocess.run([PY, *a], capture_output=True, text=True)
    return p.stdout + p.stderr


what, suffix, arms = sys.argv[1], sys.argv[2], sys.argv[3:]
lines = []
if what == 'd2':
    for a in arms:
        log = os.path.join(OUT, a + '.motion_diag.log')
        s = sh(os.path.join(W, r'verify\d2settle_20261016\score_d2.py'), log).strip()
        la = sh(os.path.join(W, r're\tools\statediff\a8_launch.py'), '--orig', ORIG, '--port', log)
        port = la.split('  PORT', 1)[-1].split('best-fit', 1)[0] if '  PORT' in la else ''
        rec = ['PORT' + ' '.join(port.split())]
        m = re.search(r'>=100:\s+(\d+)\s+\(.*?median\s+([\d.]+)', port, re.S)
        h1 = 'NA'
        if m:
            k, med = int(m.group(1)), float(m.group(2))
            h1 = f'{k}/400 median {med:.1f} -> {"PASS" if k >= 200 and med >= 900 else "FAIL"}'
        lines.append(f'{a}: {s.split(": ", 1)[-1]}')
        lines.append(f'    H1 {h1}')
        lines += ['    ' + l.strip() for l in rec[:6]]
elif what == 'b':
    env = os.path.join(W, r're\tools\b_envelopes.json')
    lab = os.path.join(W, r'verify\d3_regimes_20261013\ORIG_LABELS.txt')
    csvs = [os.path.join(OUT, f'B_{s}_K{k}.step.csv') for s in arms for k in (0, 1, 26)]
    lines.append(sh(os.path.join(W, r'verify\d3_regimes_20261013\score_regimes.py'), env, lab, *csvs))
elif what == 'coll':
    lines.append(sh(os.path.join(OUT, 'coll.py'), *arms))
txt = '\n'.join(lines)
print(txt)
open(os.path.join(OUT, f'SCORE_{what.upper()}_{suffix}.txt'), 'w').write(txt + '\n')
