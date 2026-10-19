"""D2 three-metric scorer, d2rebase (2026-10-19). Supersedes verify/d2settle_20261016/score_d2.py for new lanes; that file is kept
unchanged for provenance. Reductions = re/analysis/D2_REOPEN_2026-09-29.md section 1 ("the reducers do not change" in 13.3 / 16.7):
  a8_slip_axis.py  --orig <orig> --port <log> --max-lines 1080            -> 'slip vs fwd', bands 1500-2000 / 2000-2600
  a8_momentum.py   --orig <orig> --port <log> --max-lines 1080 --orig-steer-min 33.0 --port-steer-min 0.9 -> driving-only median
The original side is the whole capture (--max-lines applies to the port log only).
usage: py -3.12 score_d2_v2.py port [--legacy] <motion_diag.log> [...]   (--legacy: no --max-lines, = score_d2.py's reduction)
       py -3.12 score_d2_v2.py orig <capture.msd> [...]                  (the original path, same reducers)
BOUNDS: the d2rebase band (BAND_PROVENANCE below). H1 unchanged (a8_launch.py, >= 200 of 400 post-trough frames >= 100, median >= 900)."""
import re, subprocess, sys, os
M = r'C:\Users\maria\Desktop\Proyectos\Mashed'
TOOLS = os.path.join(M, r're\tools\statediff')
REF = os.path.join(M, r'verify\d2_reopen_20260929\orig_solo3.msd')   # only feeds the reducers' ORIGINAL block in port mode
BOUNDS = {}   # filled from band.json next to this file
BAND_PROVENANCE = 'verify/d2rebase_20261019/band.json'


def load_bounds():
    import json
    p = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'band.json')
    if os.path.exists(p):
        b = json.load(open(p))
        for k in ('slip1500', 'slip2000', 'drvmed'):
            BOUNDS[k] = tuple(b[k]['band'])


def run(script, *args):
    return subprocess.run([sys.executable, os.path.join(TOOLS, script), *args], capture_output=True, text=True).stdout


def block(s, tag):
    # the section after the '=== TAG ... ===' header line, up to the next line starting with '==='
    i = s.find('=== ' + tag)
    if i < 0:
        return ''
    j = s.find(chr(10), i)
    k = s.find(chr(10) + '===', j)
    return s[j:k if k >= 0 else len(s)]


def metrics(orig, port, maxl):
    ml = [] if maxl is None else ['--max-lines', str(maxl)]
    s = run('a8_slip_axis.py', '--orig', orig, '--port', port, *ml)
    mo = run('a8_momentum.py', '--orig', orig, '--port', port, *ml, '--orig-steer-min', '33.0', '--port-steer-min', '0.9')
    out = {}
    for side, tag, idx in (('orig', 'ORIGINAL', 0), ('port', 'PORT', 1)):
        b = block(s, tag)
        m1 = re.search(r'1500-2000: n=(\d+)\s+slip vs fwd ([\d.]+)', b)
        m2 = re.search(r'2000-2600: n=(\d+)\s+slip vs fwd ([\d.]+)', b)
        meds = re.findall(r'driving-only horiz median ([\d.]+)\s+n=(\d+)', mo)
        out[side] = {'slip1500': float(m1.group(2)) if m1 else None, 'slip2000': float(m2.group(2)) if m2 else None,
                     'drvmed': float(meds[idx][0]) if len(meds) > idx else None,
                     'n': (m1.group(1) if m1 else '-', m2.group(1) if m2 else '-', meds[idx][1] if len(meds) > idx else '-')}
    return out


def h1(orig, port, side):
    la = run('a8_launch.py', '--orig', orig, '--port', port)
    blk = la.split('  ' + side, 1)[-1] if ('  ' + side) in la else ''
    m = re.search(r'>=100:\s+(\d+)\s+\(.*?median\s+([\d.]+)', blk, re.S)
    return f'{m.group(1)}/400 median {float(m.group(2)):.1f}' if m else 'NA'


def verdict(v):
    if not BOUNDS:
        return ' '.join(f'{k}={v[k]:.4f}' if v[k] is not None else f'{k}=NA' for k in ('slip1500', 'slip2000', 'drvmed'))
    return ' '.join((f'{k}={v[k]:.4f} ' + ('in' if BOUNDS[k][0] <= v[k] <= BOUNDS[k][1] else 'OUT')) if v[k] is not None
                    else f'{k}=NA' for k in ('slip1500', 'slip2000', 'drvmed'))


if __name__ == '__main__':
    load_bounds()
    mode, args = sys.argv[1], sys.argv[2:]
    legacy = '--legacy' in args
    args = [a for a in args if a != '--legacy']
    if mode == 'port':
        for log in args:
            r = metrics(REF, log, None if legacy else 1080)['port']
            print(f'{log}: {verdict(r)} n={r["n"]} H1 {h1(REF, log, "PORT")}{"  [legacy reduction]" if legacy else ""}')
    elif mode == 'orig':
        # the port block needs a log; any motion_diag.log works, the ORIGINAL block does not depend on it
        dummy = os.environ.get('D2V2_DUMMY_PORT', os.path.join(M, r'verify\d2_reopen_20260929\solo_post1\motion_diag.log'))
        for msd in args:
            r = metrics(msd, dummy, 1080)['orig']
            r2 = metrics(msd, dummy, None)['orig']
            same = 'max-lines-independent' if r == r2 else 'DIFFERS WITH --max-lines'
            print(f'{msd}: {verdict(r)} n={r["n"]} H1 {h1(msd, dummy, "ORIG")} ({same})')
