"""run_seed.py <exe> <name> <root> [KEY=VAL ...] [--timeout 300] [--cfg keep|absent]

PORT-STATE-02 arms that are not G-CTRL / B / D2 / 13-track: one standalone boot on a private MASHED_ROOT, driven by the
key script (MASHED_NAV_KEYS, exe_main.cpp RunNavKeysStep: logs one NAV_KEYS line per frame step and quits when the script
ends). Prints and returns: exit code, whether it self-exited, the [portstate-seed] line, the B17-SUMMARY line, the
[videocfg] lines, the sha8 of the NAV_KEYS lines (tap/depth/screen/cursor/mode/flow per step), the last NAV_KEYS line,
and videocfg.bin's sha8 after the run.

--cfg absent  removes <root>/videocfg.bin before the run (first-run write path), keep (default) leaves it.
Logs go to D:/MashedWT/PORT-STATE-02_data/runs/<name>/. Spawns and stops ONLY its own pid.
"""
import hashlib, os, shutil, subprocess, sys, time
from pathlib import Path

args = sys.argv[1:]
timeout, cfg = 300, 'keep'
if '--timeout' in args:
    i = args.index('--timeout'); timeout = int(args[i + 1]); del args[i:i + 2]
if '--cfg' in args:
    i = args.index('--cfg'); cfg = args[i + 1]; del args[i:i + 2]
exe, name, root = args[0], args[1], Path(args[2])
extra = dict(a.split('=', 1) for a in args[3:])
out = Path('D:/MashedWT/PORT-STATE-02_data/runs') / name
if out.exists():
    shutil.rmtree(out)
out.mkdir(parents=True)
for lf in list(root.glob('*.log')) + list((root / 'log').glob('*.log')):
    lf.unlink()
if cfg == 'absent' and (root / 'videocfg.bin').exists():
    (root / 'videocfg.bin').unlink()
env = dict(os.environ)
for k in list(env):
    if k.startswith('MASHED_'):
        env.pop(k)
env.update({'MASHED_ROOT': str(root), 'MASHED_MUTE': '1', 'MASHED_VERIFY_OUT': str(out),
            'MASHED_WIN_POS': 'primary-bl', 'MASHED_SETTINGS_DEFAULTS': '1'})
env.update(extra)
t0 = time.time()
p = subprocess.Popen([str(Path(exe).resolve())], cwd=str(root), env=env)
print(f'[{name}] pid={p.pid} extra={extra} cfg={cfg}', flush=True)
try:
    rc = p.wait(timeout); done = True
except subprocess.TimeoutExpired:
    print(f'[{name}] TIMEOUT {timeout}s - stopping pid {p.pid} ONLY', flush=True)
    p.kill(); rc = p.wait(15); done = False
for lf in list(root.glob('*.log')) + list((root / 'log').glob('*.log')):
    shutil.copy(lf, out / (lf.parent.name + '_' + lf.name if lf.parent != root else lf.name))
log = (root / 'mashed_re.log').read_text(errors='replace').splitlines() if (root / 'mashed_re.log').exists() else []
vlog = (root / 'log' / 'mashed_re.log')
vlines = [l for l in (vlog.read_text(errors='replace').splitlines() if vlog.exists() else []) if l.startswith('[videocfg]')]
seed = [l for l in log if l.startswith('[portstate-seed]')]
summ = [l for l in log if 'B17-SUMMARY' in l]
nav = [l for l in log if l.startswith('NAV_KEYS')]
navh = hashlib.sha256('\n'.join(nav).encode()).hexdigest()[:8]
cf = root / 'videocfg.bin'
cfh = hashlib.sha256(cf.read_bytes()).hexdigest()[:8] if cf.exists() else 'absent'
if cf.exists():
    shutil.copy(cf, out / 'videocfg.bin')
print(f'[{name}] exit={rc} self_exited={done} {time.time() - t0:.0f}s', flush=True)
print(f'[{name}] seed: {seed[0] if seed else "(none)"}', flush=True)
print(f'[{name}] summary: {summ[0].strip() if summ else "(none)"}', flush=True)
for l in vlines:
    print(f'[{name}] {l}', flush=True)
print(f'[{name}] nav lines={len(nav)} sha8={navh} last={nav[-1] if nav else "-"}', flush=True)
print(f'[{name}] videocfg.bin sha8={cfh}', flush=True)
