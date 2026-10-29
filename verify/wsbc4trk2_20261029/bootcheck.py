"""D4-WSB-C4-SOLV-TRK2 boot check (PREREG first leg): one plain launch of the original from the private root,
MASHED_HOOK_ONLY=__none__, alive at 25 s and no MASHED.exe Event 1000 since launch = PASS. Kills only the PID it spawned.
Run under the lock: py -3.12 <orch-wt>/re/orchestrator/mashed_lock.py run --label D4-WSB-C4-SOLV-TRK2 -- py -3.12 bootcheck.py
"""
import ctypes, datetime, os, subprocess, sys, time
from pathlib import Path

ROOT = Path(os.environ["MASHED_ROOT"]) / "original"
LOG = Path("D:/MashedWT/D4-WSB-C4-SOLV-TRK2_data/bootcheck.log")


def note(s):
    line = f"[bootcheck {datetime.datetime.utcnow().strftime('%H:%M:%SZ')}] {s}"
    print(line, flush=True)
    with open(LOG, "a", encoding="utf-8") as f:
        f.write(line + "\n")


def ps(cmd):
    return subprocess.run(["powershell", "-NoProfile", "-Command", cmd], capture_output=True, text=True).stdout


def alive(pid):
    k = ctypes.windll.kernel32
    h = k.OpenProcess(0x1000, False, pid)
    if not h:
        return False
    c = ctypes.c_ulong(); k.GetExitCodeProcess(h, ctypes.byref(c)); k.CloseHandle(h)
    return c.value == 259


ep = [x for x in ps("Get-PnpDevice -Class AudioEndpoint -PresentOnly | ForEach-Object { $_.Status + ' ' + $_.FriendlyName }").splitlines() if x.strip()]
note(f"pre-launch endpoints: {ep or '<none>'}")
if not ep:
    note("BLOCKED audio endpoint"); sys.exit(3)
t0 = datetime.datetime.now() - datetime.timedelta(seconds=5)
env = dict(os.environ); env["MASHED_HOOK_ONLY"] = "__none__"; env.setdefault("MASHED_MUTE", "1")
p = subprocess.Popen([str(ROOT / "MASHED.exe")], cwd=str(ROOT), env=env)
note(f"spawned pid={p.pid}")
time.sleep(25)
up = alive(p.pid)
cr = [x.strip() for x in ps(
    "Get-WinEvent -FilterHashtable @{LogName='Application'; Id=1000; StartTime=[datetime]'%s'} -ErrorAction SilentlyContinue"
    " | Where-Object { $_.Message -match 'MASHED.exe' } | ForEach-Object { (($_.Message -split \"`n\") | Select-Object -First 5 | ForEach-Object { $_.Trim() }) -join ' | ' }"
    % t0.strftime("%Y-%m-%dT%H:%M:%S")).splitlines() if x.strip()]
note(f"pid {p.pid} alive at 25 s: {up}; crash events since launch: {cr or 'none'}")
if up:
    subprocess.run(["taskkill", "/PID", str(p.pid), "/F"], capture_output=True)
    note(f"killed own pid {p.pid}")
if any("1baf48" in c.lower() for c in cr):
    note("BLOCKED audio endpoint (AV at 0x005baf48)"); sys.exit(3)
note("PASS" if up and not cr else "FAIL")
sys.exit(0 if up and not cr else 1)
