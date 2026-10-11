# Read-only global sampler (ReadProcessMemory, no hooks, no writes).
# usage: peek.py addrs.json out.json (--pid N | --spawn-sa) [--secs S] [--period P]
#   addrs.json: list of [addr_hex, stride_hex_or_0]
# Samples every P s: frame tick 0x007f0ff4, phase 0x00771968, and 4 dwords per address
# (cars 0..3 at stride, or 4 consecutive dwords when stride is 0).
import ctypes, json, os, subprocess, sys, time
from ctypes import wintypes
k32 = ctypes.windll.kernel32
k32.OpenProcess.restype = wintypes.HANDLE
k32.ReadProcessMemory.argtypes = [wintypes.HANDLE, ctypes.c_void_p, ctypes.c_void_p, ctypes.c_size_t, ctypes.POINTER(ctypes.c_size_t)]
ROOT = r"C:\Users\maria\Desktop\Proyectos\Mashed"
a = sys.argv[1:]
addrs = json.load(open(a[0])); outp = a[1]
secs = float(a[a.index("--secs") + 1]) if "--secs" in a else 60
period = float(a[a.index("--period") + 1]) if "--period" in a else 0.5
proc = None
if "--spawn-sa" in a:
    env = dict(os.environ)
    for k in list(env):
        if k.startswith("MASHED_"): del env[k]
    env.update(MASHED_MUTE="1", MASHED_TRACK_VIEW="Training", MASHED_CAR="1", MASHED_ROUND="1", MASHED_ROUND_RULE="4",
               MASHED_WIN_POS="primary-bl", MASHED_DETERMINISTIC="1", MASHED_DET_FRAMES="14400")
    proc = subprocess.Popen([os.path.join(ROOT, "mashedmod", "build", "mashed_re.exe")], cwd=ROOT, env=env)
    pid = proc.pid
else:
    pid = int(a[a.index("--pid") + 1])
print("pid", pid, flush=True)
h = None
for _ in range(100):
    h = k32.OpenProcess(0x0410, False, pid)
    if h: break
    time.sleep(0.1)
def rd(addr, n):
    buf = ctypes.create_string_buffer(n); got = ctypes.c_size_t(0)
    ok = k32.ReadProcessMemory(h, ctypes.c_void_p(addr), buf, n, ctypes.byref(got))
    return buf.raw if ok and got.value == n else None
def u32(addr):
    b = rd(addr, 4)
    return None if b is None else int.from_bytes(b, "little")
samples = []
t0 = time.time()
while time.time() - t0 < secs:
    if proc is not None and proc.poll() is not None: break
    if proc is None:
        ec = wintypes.DWORD(); k32.GetExitCodeProcess(h, ctypes.byref(ec))
        if ec.value != 259: break
    s = {"t": round(time.time() - t0, 2), "tick": u32(0x007f0ff4), "phase": u32(0x00771968), "v": {}}
    for ah, st in addrs:
        ad = int(ah, 16); stv = int(st, 16) if isinstance(st, str) else st
        vals = [u32(ad + (i * stv if stv else i * 4)) for i in range(4)]
        s["v"][ah] = vals
    samples.append(s)
    time.sleep(period)
if proc is not None:
    try: proc.kill()
    except Exception: pass
json.dump({"pid": pid, "samples": samples}, open(outp, "w"))
print("samples", len(samples))
