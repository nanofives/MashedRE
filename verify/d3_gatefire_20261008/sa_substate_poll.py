"""D-11073 leg 1a G-STEP / G-STEP-CTL (PREREG_LEG1A.md section 4): poll the STANDALONE's
DAT_0063d584 / DAT_0063d588 / _DAT_007f100c at their original addresses. READ-ONLY.
Spawns mashed_re.exe, adopts ONLY that PID, waits for its self-exit (MASHED_DET_FRAMES),
kills only that PID on timeout.

usage: py -3.12 sa_substate_poll.py <out.csv> <frames> [KNOB=VAL ...]
d584 and d588 are adjacent, so they are read in ONE 8-byte ReadProcessMemory (still not
guaranteed atomic against the game thread, hence the n-1 allowance in the scorer)."""
import ctypes, struct, subprocess, sys, time, os, csv
from ctypes import wintypes
ROOT = r"C:\Users\maria\Desktop\Proyectos\Mashed"
EXE = ROOT + r"\mashedmod\build\mashed_re.exe"
k32 = ctypes.WinDLL("kernel32", use_last_error=True)
k32.OpenProcess.restype = wintypes.HANDLE
k32.ReadProcessMemory.argtypes = [wintypes.HANDLE, ctypes.c_void_p, ctypes.c_void_p, ctypes.c_size_t, ctypes.POINTER(ctypes.c_size_t)]
def rd(h, a, n=4):
    b = (ctypes.c_char * n)(); g = ctypes.c_size_t(0)
    return bytes(b) if k32.ReadProcessMemory(h, ctypes.c_void_p(a), b, n, ctypes.byref(g)) and g.value == n else None
out = sys.argv[1]; frames = sys.argv[2]; knobs = sys.argv[3:]
env = dict(os.environ)
for k in list(env):
    if k.startswith("MASHED_"): del env[k]
env.update({"MASHED_MUTE": "1", "MASHED_TRACK_VIEW": "Training", "MASHED_CAR": "1", "MASHED_ROUND": "1",
            "MASHED_ROUND_RULE": "4", "MASHED_WIN_POS": "primary-bl", "MASHED_DETERMINISTIC": "1",
            "MASHED_DET_FRAMES": frames})
for kv in knobs:
    k, _, v = kv.partition("="); env[k] = v or "1"
print("[env]", {k: v for k, v in env.items() if k.startswith("MASHED_")})
log = os.path.join(ROOT, "mashed_re.log")
if os.path.exists(log): os.remove(log)
p = subprocess.Popen([EXE], cwd=ROOT, env=env)
print("[spawn] own pid", p.pid)
h = k32.OpenProcess(0x0010 | 0x0400, False, p.pid)
rows, t0 = [], time.time()
try:
    while p.poll() is None and time.time() - t0 < 600:
        b = rd(h, 0x0063d584, 8); s = rd(h, 0x007f100c, 4); c = rd(h, 0x007f0ff4, 4); m = rd(h, 0x0063ba8c, 4)
        if b and s and c:
            n, t = struct.unpack("<if", b)
            rows.append({"t": round(time.time() - t0, 3), "n_d584": n, "t_d588_bits": "%08x" % struct.unpack("<I", b[4:])[0],
                         "t_d588": t, "dt_100c_bits": "%08x" % struct.unpack("<I", s)[0],
                         "clk_0ff4": struct.unpack("<i", c)[0], "ba8c": struct.unpack("<i", m)[0] if m else None})
        time.sleep(1 / 60)
finally:
    if p.poll() is None:
        print("[timeout] killing OWN pid", p.pid); p.kill()
    p.wait(); print("[exit]", p.returncode)
with open(out, "w", newline="") as f:
    w = csv.DictWriter(f, fieldnames=list(rows[0])); w.writeheader(); w.writerows(rows)
print("[out]", out, len(rows))
if os.path.exists(log):
    sub = [l.rstrip() for l in open(log, errors="replace") if "[substate]" in l]
    print("[log] [substate] lines:", len(sub), sub[:1])
