"""D-11073 leg 0 item 3: poll the STANDALONE's values at the original addresses that
FUN_0040dbd0 / FUN_004103a0 read. READ-ONLY. Spawns mashed_re.exe, adopts ONLY that PID,
waits for its self-exit (MASHED_DET_FRAMES), kills only that PID on timeout."""
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
def i32(h, a):
    b = rd(h, a); return None if b is None else struct.unpack("<i", b)[0]
def f32(h, a):
    b = rd(h, a); return None if b is None else struct.unpack("<f", b)[0]
I = {"substate_ba8c": 0x0063ba8c, "submode_e9fc": 0x0067e9fc, "team_ea64": 0x0067ea64, "rule_0fd0": 0x007f0fd0,
     "g_91bc": 0x008991bc, "ovr_ed6c": 0x0067ed6c, "cd_29b8": 0x005f29b8, "framedt_1008": 0x007f1008,
     "c_1018": 0x007f1018, "clk_0ff4": 0x007f0ff4, "tick_0ff8": 0x007f0ff8, "plr_ba78": 0x0063ba78,
     "slotptr": 0x005f2770, "n_d584": 0x0063d584, "trk_4158": 0x00644158, "g_898c": 0x0089898c,
     "n_1ca0": 0x00801ca0, "flag_fe0": 0x00897fe0, "n994": 0x00898994}
F = {"t_d588": 0x0063d588, "dt_100c": 0x007f100c}
out = sys.argv[1]; frames = sys.argv[2] if len(sys.argv) > 2 else "1500"
env = dict(os.environ)
for k in list(env):
    if k.startswith("MASHED_"): del env[k]
env.update({"MASHED_MUTE": "1", "MASHED_TRACK_VIEW": "Training", "MASHED_CAR": "1", "MASHED_ROUND": "1",
            "MASHED_ROUND_RULE": "4", "MASHED_WIN_POS": "primary-bl", "MASHED_DETERMINISTIC": "1",
            "MASHED_DET_FRAMES": frames})
print("[env]", {k: v for k, v in env.items() if k.startswith("MASHED_")})
p = subprocess.Popen([EXE], cwd=ROOT, env=env)
print("[spawn] own pid", p.pid)
h = k32.OpenProcess(0x0010 | 0x0400, False, p.pid)
rows, t0 = [], time.time()
try:
    while p.poll() is None and time.time() - t0 < 600:
        r = {"t": round(time.time() - t0, 3)}
        for k, a in I.items(): r[k] = i32(h, a)
        for k, a in F.items(): r[k] = f32(h, a)
        base = r["slotptr"]
        tb = i32(h, base) if base else None
        r["slot_tbl"] = tb
        for v in range(4):
            r["slot%d" % v] = i32(h, tb + 0x34 + v * 4) if tb else None   # *(*(0x5f2770)+0x34+v*4)? see note
            r["slotb%d" % v] = i32(h, base + 0x34 + v * 4) if base else None  # PTR_PTR_005f2770 + 0x34 + v*4
            r["alive%d" % v] = i32(h, 0x008815a4 + v * 0xd04)
            r["c2194_%d" % v] = i32(h, 0x00882194 + v * 0xd04)
            r["c2198_%d" % v] = i32(h, 0x00882198 + v * 0xd04)
        rows.append(r); time.sleep(1 / 60)
finally:
    if p.poll() is None:
        print("[timeout] killing OWN pid", p.pid); p.kill()
    p.wait(); print("[exit]", p.returncode)
with open(out, "w", newline="") as f:
    w = csv.DictWriter(f, fieldnames=list(rows[0])); w.writeheader(); w.writerows(rows)
print("[out]", out, len(rows))
