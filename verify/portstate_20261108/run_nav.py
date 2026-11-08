"""run_nav.py <exe> <name> <root> [KEY=VAL ...] [--timeout 300]

Copy of verify/dualcopy4_20261016/run_nav.py for PORT-STATE-01: the standalone's scripted menu walk
(MASHED_NAV_DEMO=1) on one exe image with a private MASHED_ROOT and extra env (e.g. MASHED_PORTSTATE_FRONTEND=1).
Backbuffer BMPs go to D:/MashedWT/PORT-STATE-01_data/nav/<name>/ (MASHED_VERIFY_OUT), then one sha8 per BMP and one
for the set. The exe's mashed_re.log (cwd = root) is copied next to them as mashed_re.log. Spawns and stops ONLY its
own pid. The demo bypasses the focus gate (memory nav-demo-bypasses-focus-gate): re-run a mismatch before believing it.
"""
import hashlib, os, shutil, subprocess, sys, time
from pathlib import Path

args = [a for a in sys.argv[1:]]
timeout = 300
if '--timeout' in args:
    i = args.index('--timeout'); timeout = int(args[i + 1]); del args[i:i + 2]
exe, name, root = args[0], args[1], Path(args[2])
extra = dict(a.split('=', 1) for a in args[3:])
out = Path('D:/MashedWT/PORT-STATE-01_data/nav') / name
out.mkdir(parents=True, exist_ok=True)
for f in out.glob('*'):
    if f.is_file():
        f.unlink()
for lf in root.glob('*.log'):
    lf.unlink()
env = dict(os.environ)
for k in list(env):
    if k.startswith('MASHED_'):
        env.pop(k)
env.update({'MASHED_ROOT': str(root), 'MASHED_MUTE': '1', 'MASHED_VERIFY_OUT': str(out),
            'MASHED_WIN_POS': 'primary-bl', 'MASHED_SETTINGS_DEFAULTS': '1'})
if 'MASHED_NAV_KEYS' not in extra:      # MASHED_GOTO + MASHED_NAV_KEYS = the key-script route (logs NAV_KEYS, quits)
    env['MASHED_NAV_DEMO'] = '1'
env.update(extra)
t0 = time.time()
p = subprocess.Popen([str(Path(exe).resolve())], cwd=str(root), env=env)
print(f'[{name}] pid={p.pid} extra={extra}', flush=True)
try:
    rc = p.wait(timeout); done = True
except subprocess.TimeoutExpired:
    print(f'[{name}] TIMEOUT {timeout}s - stopping pid {p.pid} ONLY', flush=True)
    p.kill(); rc = p.wait(15); done = False
for lf in root.glob('*.log'):
    shutil.copy(lf, out / lf.name)
bmps = sorted(out.rglob('walk_*.bmp'))
print(f'[{name}] exit={rc} self_exited={done} {time.time()-t0:.0f}s  walk bmps={len(bmps)}', flush=True)
allh = hashlib.sha256()
for b in bmps:
    h = hashlib.sha256(b.read_bytes()).hexdigest()[:8]
    allh.update(b.name.encode() + h.encode())
    print(f'  {b.name} {h}', flush=True)
print(f'[{name}] set sha8={allh.hexdigest()[:8]}', flush=True)
