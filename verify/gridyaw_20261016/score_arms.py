"""d2engage D2 scorer: per motion_diag log, (1) the D2 three metrics via verify/d2settle_20261016/score_d2.py
(bounds d81a8df6 unchanged), (2) launch + recovery H1 via re/tools/statediff/a8_launch.py (orig_solo3, R=886/1),
(3) part-4 knob-took (PREREG_d2engage.md section 3): order of the `settle slot=0` marker vs `launchrev release`,
and bf4/bf8 on series lines 0 (settle) and 1 (d=0).
usage: py -3.12 score_arms.py <arm> [<arm> ...]   (reads <arm>.motion_diag.log next to this file)"""
import os, re, subprocess, sys
D = os.path.dirname(os.path.abspath(__file__))
W = os.path.dirname(os.path.dirname(D))
M = r'C:\Users\maria\Desktop\Proyectos\Mashed'
ORIG = os.path.join(M, r'verify\d2_reopen_20260929\orig_solo3.msd')
SCORE = os.path.join(W, r'verify\d2settle_20261016\score_d2.py')
LAUNCH = os.path.join(W, r're\tools\statediff\a8_launch.py')

for arm in sys.argv[1:]:
    log = os.path.join(D, arm + '.motion_diag.log')
    print(f'== {arm}')
    if not os.path.exists(log):
        print('   MISSING', log); continue
    s = subprocess.run([sys.executable, SCORE, log], capture_output=True, text=True).stdout.strip()
    print('   D2:', s.split(': ', 1)[-1])
    la = subprocess.run([sys.executable, LAUNCH, '--orig', ORIG, '--port', log], capture_output=True, text=True).stdout
    port = la.split('PORT', 1)[1] if 'PORT' in la else ''
    m = re.search(r'post-trough n=\s*(\d+)\s+>=100:\s*(\d+)\s*\(\s*([\d.]+) %\)\s+median\s+([\d.]+)', port)
    e = re.search(r'engages at d=(\S+)', port)
    b = re.search(r'BEST L = (\S+)\s+at\s+([\d.]+) %', la)
    if m:
        n, ge, pct, med = int(m.group(1)), int(m.group(2)), float(m.group(3)), float(m.group(4))
        h1 = (ge * 2 >= n) and med >= 900.0
        print(f'   recovery: {ge}/{n} ({pct} %) median {med}  H1 {"PASS" if h1 else "FAIL"}'
              f' | +0xb14 engages d={e.group(1) if e else "?"} | best lag L={b.group(1) if b else "?"} at {b.group(2) if b else "?"} %')
    else:
        print('   recovery: unparsed')
    lines = open(log, errors='replace').read().splitlines()
    im = next((i for i, l in enumerate(lines) if l.startswith('settle slot=0')), None)
    ir = next((i for i, l in enumerate(lines) if l.startswith('launchrev release')), None)
    ser = [(i, l) for i, l in enumerate(lines) if 'sp=' in l and 'b14=' in l]
    def bf(l):
        x = re.search(r' bf4=(-?\d+) bf8=(-?\d+) st=\[([^\]]*)\]', l)
        return f'bf4={x.group(1)} bf8={x.group(2)} st=[{x.group(3)}]' if x else 'bf4=? (no token)'
    s0 = ser[0] if ser else (None, '')
    s1 = ser[1] if len(ser) > 1 else (None, '')
    print(f'   order: marker line {im}, release line {ir}, series[0] at line {s0[0]} ({bf(s0[1])}), '
          f'series[1] at line {s1[0]} ({bf(s1[1])})')
