"""run_nav.py <exe> <name> [--timeout 300] : the standalone's scripted menu walk (MASHED_NAV_DEMO=1) on one exe image, backbuffer BMPs into
verify/dualcopy4_20261016/nav/<name>/ (MASHED_VERIFY_OUT), then a sha8 per BMP and for the whole set. Spawns and stops ONLY its own pid.
The demo writes edge events into the exe's own input buffers (no OS input), but it bypasses the focus gate, so a key held in another
window can leak in (memory nav-demo-bypasses-focus-gate): a mismatch must be re-run before it is believed.

    py -3.12 run_nav.py images/base.exe base ; py -3.12 run_nav.py images/b.exe new ; py -3.12 nav_cmp.py base new
"""
import hashlib, os, subprocess, sys, time
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = Path(r"C:\Users\maria\Desktop\Proyectos\Mashed")
exe, name = sys.argv[1], sys.argv[2]
timeout = int(sys.argv[sys.argv.index("--timeout") + 1]) if "--timeout" in sys.argv else 300
out = HERE / "nav" / name
out.mkdir(parents=True, exist_ok=True)
for f in out.glob("*"):
    if f.is_file():
        f.unlink()
env = dict(os.environ)
for k in ("MASHED_GOTO", "MASHED_NAV_KEYS", "MASHED_RACE_DEMO", "MASHED_RESULT_DEMO", "MASHED_DBG_DRAWSTREAM", "MASHED_PLAYERTRACE"):
    env.pop(k, None)
env.update({"MASHED_ROOT": str(ROOT), "MASHED_MUTE": "1", "MASHED_NAV_DEMO": "1", "MASHED_VERIFY_OUT": str(out),
            "MASHED_WIN_POS": "primary-bl", "MASHED_SETTINGS_DEFAULTS": "1"})
t0 = time.time()
p = subprocess.Popen([str(Path(exe).resolve())], cwd=str(ROOT), env=env)
print(f"[{name}] pid={p.pid}", flush=True)
try:
    rc = p.wait(timeout)
    done = True
except subprocess.TimeoutExpired:
    print(f"[{name}] TIMEOUT {timeout}s - stopping pid {p.pid} ONLY", flush=True)
    p.kill()
    rc = p.wait(15)
    done = False
bmps = sorted(out.rglob("walk_*.bmp"))
print(f"[{name}] exit={rc} self_exited={done} {time.time()-t0:.0f}s  walk bmps={len(bmps)}", flush=True)
allh = hashlib.sha256()
for b in bmps:
    h = hashlib.sha256(b.read_bytes()).hexdigest()[:8]
    allh.update(b.name.encode() + h.encode())
    print(f"  {b.name} {h}", flush=True)
print(f"[{name}] set sha8={allh.hexdigest()[:8]}", flush=True)
