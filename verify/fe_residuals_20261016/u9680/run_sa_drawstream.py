"""Run a standalone exe at screens and dump the draw stream. Usage: run_sa_drawstream.py <exe> <gameroot> <prefix> <screens...>
Spawns only its own PIDs and kills only those. Logs/outputs go to the private gameroot copy (MASHED_ROOT)."""
import os, subprocess, sys, time
from pathlib import Path
exe, gr, prefix, screens = sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4:]
for sc in screens:
    out = Path(f"{prefix}_{sc}.json").resolve(); out.parent.mkdir(parents=True, exist_ok=True)
    if out.exists(): out.unlink()
    env = dict(os.environ, MASHED_ROOT=gr, MASHED_WIN_POS="primary-bl", MASHED_SETTINGS_DEFAULTS="1",
               MASHED_DBG_DRAWSTREAM="200:203", MASHED_DBG_DRAWSTREAM_OUT=str(out), MASHED_GOTO=sc)
    p = subprocess.Popen([exe], cwd=gr, env=env)
    print("S", sc, "pid", p.pid, flush=True)
    t0 = time.time()
    while not out.exists() and time.time() - t0 < 90 and p.poll() is None: time.sleep(0.5)
    time.sleep(3)
    if p.poll() is None: p.kill()
    print(" out", out.exists() and out.stat().st_size, flush=True)
