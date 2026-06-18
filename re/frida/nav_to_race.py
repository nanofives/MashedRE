# Drive the ORIGINAL MASHED.exe into a canonical Quick-Battle race, then DETACH Frida
# leaving MASHED running in the race (no kill) so a TTD recorder can attach to a clean,
# un-instrumented process. Reuses the proven nav_agent.js recipe (same sequence as
# phys_c4_telemetry.py / scenario_attach_probe.py). OS-input-free (in-process
# FUN_00497310 return override). Prints "RACE_PID=<pid>" as the LAST line on success.
#
# Stock original (MASHED_RE_NO_AUTO_HOOK=1) so the trace is the reference behavior.
#
# Usage: py -3.12 re/frida/nav_to_race.py
import os, shutil, subprocess, sys, time
from pathlib import Path
import frida

ROOT = Path(__file__).resolve().parent.parent.parent
ORIG = ROOT / "original"; EXE = ORIG / "MASHED.exe"
NAV  = (Path(__file__).resolve().parent / "nav_agent.js").read_text(encoding="utf-8")
# CREATE_NEW_PROCESS_GROUP | DETACHED_PROCESS | ABOVE_NORMAL_PRIORITY_CLASS.
# ABOVE_NORMAL is deliberate: when launched from the capture Scheduled Task (default
# task priority 7 = BELOW_NORMAL), a child MASHED inherits BELOW_NORMAL and gets
# CPU-starved -> the render/physics loop crawls and TTD traces come out sparse
# (FastSqrt 444/9s vs 6031/10s when launched normally, 2026-06-17). Forcing the class
# at spawn makes the game run full-speed regardless of the parent's priority. Priority
# affects only scheduling, never the bit-level computation, so trace fidelity is intact.
NPG, DET, ABOVE = 0x00000200, 0x00000008, 0x00008000

def main():
    canon = ROOT / "scripts" / "canonical" / "videocfg_windowed.bin"
    if canon.exists():
        shutil.copy2(str(canon), str(ORIG / "videocfg.bin"))
    env = dict(os.environ); env["MASHED_RE_NO_AUTO_HOOK"] = "1"
    dev = frida.get_local_device()

    proc = sess = None
    for attempt in range(5):
        proc = subprocess.Popen([str(EXE)], cwd=str(ORIG), env=env,
            stdin=subprocess.DEVNULL, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL,
            creationflags=NPG | DET | ABOVE)
        time.sleep(0.2)
        try: sess = dev.attach(proc.pid); break
        except Exception:
            try: proc.kill()
            except Exception: pass
            time.sleep(1.0)
    if sess is None:
        print("attach failed after 5 retries"); print("RACE_PID=0"); return 4

    scr = sess.create_script(NAV)
    scr.on("message", lambda m, d: print("  agent:", m.get("description")) if m["type"] == "error" else None)
    scr.load(); E = scr.exports_sync; E.init()

    def wait(pred, t):
        end = time.time() + t
        while time.time() < end:
            if pred(): return True
            time.sleep(0.1)
        return False
    def press(c, ms=180): E.press(c, ms); time.sleep(ms / 1000.0 + 0.3)
    def confirm_to(target, tries=6):
        for _ in range(tries):
            if E.depth() >= target: return True
            press(4)
            if wait(lambda: E.depth() >= target, 2.0): return True
        return E.depth() >= target

    rc = 3
    try:
        print("booting to menu...")
        if not wait(lambda: E.phase() == 3 and E.depth() >= 1, 30):
            print("never reached title"); print("RACE_PID=0"); return 2
        time.sleep(1.0)
        # Quick Battle, Arctic (race_refs recipe — identical to phys_c4_telemetry.py).
        confirm_to(2); time.sleep(0.4); press(4); time.sleep(0.8)
        confirm_to(3)
        E.setsel(1); time.sleep(0.3)
        confirm_to(4, 4); confirm_to(5, 4)
        press(4); time.sleep(1.5)
        for _ in range(5):
            if E.phase() != 3: break
            press(4); time.sleep(1.5)
        if E.phase() != 3:
            print(f"in race (phase={E.phase()})"); rc = 0
        else:
            print(f"NOT in race (phase={E.phase()})"); rc = 3
    finally:
        # leave MASHED running: revert Frida instrumentation WITHOUT killing the process
        try: scr.unload()
        except Exception: pass
        try: sess.detach()
        except Exception: pass

    print(f"RACE_PID={proc.pid if rc == 0 else 0}")
    return rc

if __name__ == "__main__":
    sys.exit(main())
