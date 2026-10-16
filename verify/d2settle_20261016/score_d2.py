"""D2 three-metric score (PREREG_d2settle.md section 4), bounds d81a8df6 unchanged.
Recipe pinned in this lane: a8_slip_axis.py + a8_momentum.py --orig-steer-min 33.0 --port-steer-min 0.9, NO --max-lines,
against verify/d2_reopen_20260929/orig_solo3.msd (reproduces attempt 20's t1 exactly: 0.1943 / 0.2524 / 1852.66 n=1367).
usage: py -3.12 score_d2.py <motion_diag.log> [...]"""
import re, subprocess, sys, os
M = r'C:\Users\maria\Desktop\Proyectos\Mashed'
W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
ORIG = os.path.join(M, r'verify\d2_reopen_20260929\orig_solo3.msd')
TOOLS = os.path.join(W, r're\tools\statediff')
BOUNDS = {'slip1500': (0.18855, 0.19635), 'slip2000': (0.24488, 0.25487), 'drvmed': (1904.70, 1982.44)}


def run(script, *args):
    return subprocess.run([sys.executable, os.path.join(TOOLS, script), '--orig', ORIG, *args],
                          capture_output=True, text=True).stdout


for log in sys.argv[1:]:
    s = run('a8_slip_axis.py', '--port', log)
    port = s.split('=== PORT', 1)[1] if '=== PORT' in s else ''
    m1 = re.search(r'1500-2000: n=(\d+)\s+slip vs fwd ([\d.]+)', port)
    m2 = re.search(r'2000-2600: n=(\d+)\s+slip vs fwd ([\d.]+)', port)
    mo = run('a8_momentum.py', '--port', log, '--orig-steer-min', '33.0', '--port-steer-min', '0.9')
    meds = re.findall(r'driving-only horiz median ([\d.]+)\s+n=(\d+)', mo)
    vals = {'slip1500': float(m1.group(2)) if m1 else None, 'slip2000': float(m2.group(2)) if m2 else None,
            'drvmed': float(meds[1][0]) if len(meds) > 1 else None}
    ns = (m1.group(1) if m1 else '-', m2.group(1) if m2 else '-', meds[1][1] if len(meds) > 1 else '-')
    verdict = ' '.join(f'{k}={v:.4f}{"" if v is None else (" in" if BOUNDS[k][0] <= v <= BOUNDS[k][1] else " OUT")}'
                       if v is not None else f'{k}=NA' for k, v in vals.items())
    print(f'{os.path.basename(os.path.dirname(log)) or log} {os.path.basename(log)}: {verdict}  n={ns}')
