#!/usr/bin/env python3
"""cap_splash.py - D5-ICON-ASSET splash captures (loading-disc icon), one arm per call.

  py -3.12 cap_splash.py <arm> <exe> <root> <frame> <outdir>

Spawns <exe> with MASHED_ROOT=<root> and MASHED_DBG_BBDUMP=<frame> (exe_main.cpp frontend frame counter), waits
for the backbuffer BMP (CAP_WAIT seconds, default 90), kills ONLY the spawned PID, and copies the loadicon lines of <root>/mashed_re.log next to
the capture. Run it under mashed_lock.py run.
"""
import os
import shutil
import subprocess
import sys
import time

arm, exe, root, frame, outdir = sys.argv[1:6]
os.makedirs(outdir, exist_ok=True)
bmp = os.path.join(outdir, "%s.bmp" % arm)
if os.path.exists(bmp):
    os.remove(bmp)
env = dict(os.environ)
env.update(MASHED_ROOT=root, MASHED_DBG_BBDUMP=frame, MASHED_DBG_BBDUMP_OUT=bmp)
p = subprocess.Popen([exe], cwd=os.path.dirname(exe), env=env)
print("[%s] pid %d frame %s" % (arm, p.pid, frame), flush=True)
t0 = time.time()
ok = False
while time.time() - t0 < float(os.environ.get("CAP_WAIT", "90")):
    if p.poll() is not None:
        print("[%s] exited early rc=%s" % (arm, p.returncode), flush=True)
        break
    if os.path.exists(bmp) and os.path.getsize(bmp) > 0:
        time.sleep(1.0)
        ok = True
        break
    time.sleep(0.25)
if p.poll() is None:
    p.kill()
    p.wait()
log = os.path.join(root, "mashed_re.log")
lines = []
if os.path.exists(log):
    with open(log, "r", errors="replace") as f:
        lines = [l.rstrip("\n") for l in f if "loadicon" in l]
with open(os.path.join(outdir, "%s.loadicon.txt" % arm), "w") as f:
    f.write("\n".join(lines) + "\n")
print("[%s] %s  %s" % (arm, "CAPTURED" if ok else "NO CAPTURE", " | ".join(lines)), flush=True)
sys.exit(0 if ok else 1)
