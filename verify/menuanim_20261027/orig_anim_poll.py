# Original-side menu slide timing without injection: launch MASHED.exe, poll the record table
# (0x00898ac0, 30 x 0x34) and phase (0x0067eca4) with ReadProcessMemory, log perf_counter ms
# whenever the table changes. Catches the boot-time menu slide-in. Usage: orig_anim_poll.py <out.json>
import ctypes, json, os, subprocess, sys, time
from ctypes import wintypes as W
from pathlib import Path
ORIG = Path(os.environ["MASHED_ROOT"]) / "original"
k = ctypes.WinDLL("kernel32", use_last_error=True)
k.OpenProcess.restype = W.HANDLE
env = dict(os.environ); env["MASHED_RE_NO_AUTO_HOOK"] = "1"
p = subprocess.Popen([str(ORIG / "MASHED.exe")], cwd=str(ORIG), env=env)
h = k.OpenProcess(0x0410, False, p.pid)
def rd(addr, n):
    b = ctypes.create_string_buffer(n); got = ctypes.c_size_t()
    ok = k.ReadProcessMemory(h, ctypes.c_void_p(addr), b, n, ctypes.byref(got))
    return b.raw if ok and got.value == n else None
rows = []; last = None; t0 = time.perf_counter(); end = t0 + 60
while time.perf_counter() < end and p.poll() is None:
    t = rd(0x00898ac0, 30 * 0x34); ph = rd(0x0067eca4, 4)
    if t is None: time.sleep(0.01); continue
    cur = (t, ph)
    if cur != last:
        recs = []
        for i in range(30):
            b = t[i*0x34:(i+1)*0x34]; tag = int.from_bytes(b[0:4], "little")
            if tag: recs.append([i, tag, int.from_bytes(b[0x10:0x14], "little", signed=True), int.from_bytes(b[4:8], "little", signed=True)])
        rows.append({"ms": round((time.perf_counter()-t0)*1000, 2), "phase": int.from_bytes(ph, "little"), "recs": recs})
        last = cur
    time.sleep(0.001)
pid = p.pid
if p.poll() is None: p.kill()
Path(sys.argv[1]).write_text(json.dumps({"pid": pid, "rows": rows}))
print("pid", pid, "changes", len(rows))
