"""Standalone side of orig_scr_keys.py. Usage: sa_scr_keys.py <exe> <gameroot> <screen> <keys> <out.log>
Runs the standalone with MASHED_GOTO=<screen> MASHED_NAV_KEYS=<keys> in the private gameroot, copies the NAV_KEYS
lines of ./mashed_re.log to <out.log>. Kills only its own PID."""
import os, subprocess, sys, time
from pathlib import Path
exe, gr, scr, keys, out = sys.argv[1:6]
log = Path(gr) / 'mashed_re.log'
env = dict(os.environ, **({'MASHED_SAVE': os.environ['SA_SAVE']} if 'SA_SAVE' in os.environ else {}), MASHED_ROOT=gr, MASHED_GOTO=scr, MASHED_NAV_KEYS=keys, MASHED_WIN_POS='primary-bl',
           MASHED_SETTINGS_DEFAULTS='1')
p = subprocess.Popen([exe], cwd=gr, env=env)
n = len(keys.split(','))
t0 = time.time()
while time.time() - t0 < 120 and p.poll() is None:
    time.sleep(1.0)
    txt = log.read_text(errors='replace') if log.exists() else ''
    if txt.count('NAV_KEYS') > 0 and txt.count('tap=') and 'tap=-' in txt and time.time() - t0 > 20: pass
    if time.time() - t0 > 10 + 0.6 * n * 30 / 10: break
time.sleep(2)
if p.poll() is None: p.kill()
lines = [l for l in log.read_text(errors='replace').splitlines() if l.startswith(("NAV_KEYS","MODAL","REPEAT"))]
Path(out).write_text('\n'.join(lines))
print(len(lines), 'NAV_KEYS lines')
