"""Collision-FX re-measure run (default build). Usage: run_fx.py <exe> <gameroot> <tag> [extra ENV=VAL ...]
G-CTRL race recipe (run_h4.ps1 F_NEW: Training, car 1, round rule 4, deterministic 14400 frames) + MASHED_FX_DEBUG=1.
Output: <gameroot>/log/fx_debug.txt copied to verify/fe_residuals_20261016/fx/<tag>.fx_debug.txt. Own PID only."""
import os, subprocess, sys, shutil, time
from pathlib import Path
exe, gr, tag = sys.argv[1:4]
extra = dict(a.split('=', 1) for a in sys.argv[4:])
out = Path(__file__).resolve().parent / 'fx'; out.mkdir(exist_ok=True)
fx = Path(gr) / 'log' / 'fx_debug.txt'
if fx.exists(): fx.unlink()
env = dict(os.environ, MASHED_ROOT=gr, MASHED_MUTE='1', MASHED_TRACK_VIEW='Training', MASHED_CAR='1', MASHED_ROUND='1',
           MASHED_ROUND_RULE='4', MASHED_WIN_POS='primary-bl', MASHED_DETERMINISTIC='1', MASHED_DET_FRAMES='14400',
           MASHED_FX_DEBUG='1', **extra)
t0 = time.time()
p = subprocess.Popen([exe], cwd=gr, env=env)
print('pid', p.pid, flush=True)
try: p.wait(timeout=900)
except subprocess.TimeoutExpired: p.kill(); print('TIMEOUT killed', p.pid)
print('exit', p.returncode, 'secs', round(time.time() - t0))
if fx.exists(): shutil.copy(fx, out / f'{tag}.fx_debug.txt'); print('lines', sum(1 for _ in open(fx)))
else: print('NO fx_debug.txt')
