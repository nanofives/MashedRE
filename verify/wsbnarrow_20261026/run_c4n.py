"""D4-WSB-C4-NARROW driver: one canonical-scenario boot per function per leg.

Legs (canonical scenario = the census one: track 0, mode 10, 4 slots, slot 0 parked, 45 s hold):
  P  probe : MASHED_HOOK_ONLY=L2_C4N_<rva>, MASHED_C4N=1. The full-output in-process A/B in
             mashedmod/src/mashed_re/Collision/NarrowC4Probe.cpp; the port body runs behind the
             installed inline JMP on every call and the game keeps its results.
  H  hook  : MASHED_HOOK_ONLY=FUN_<rva> (the row's own registration), no A/B. Install + survive.
Both legs set MASHED_NO_SELFTEST=1: with the ShadowAB lane armed, ShadowAB::Run() calls
HookSystem::Install on nested port hooks that were never installed (Core/ShadowAB.h Run), which
would put more than one function live.

Every boot goes through mashed_lock.py run. scenario_launch.py spawns MASHED and kills only that
PID. Raw logs land in D:/MashedWT/D4-WSB-C4-NARROW_data; this script prints one line per boot.

  py -3.12 verify/wsbnarrow_20261026/run_c4n.py [--legs P,H] [--rvas 005757d0,...] [--stride 1]
"""
import argparse
import hashlib
import os
import pathlib
import re
import shutil
import subprocess
import sys
import time

HERE = pathlib.Path(__file__).resolve().parent
WT = HERE.parent.parent
LOCK = pathlib.Path(r"C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\orch-integration\re\orchestrator\mashed_lock.py")
ROOT = pathlib.Path(r"D:\MashedWT\D4-WSB-C4-NARROW_root")
DATA = pathlib.Path(r"D:\MashedWT\D4-WSB-C4-NARROW_data")
RVAS = ["005757d0", "00575880", "00575c60", "00575fe0", "00576640",
        "00577ec0", "005784a0", "00578610", "00578e50", "00579b50"]


def sha(p):
    return hashlib.sha256(p.read_bytes()).hexdigest()


def boot(rva, leg, stride, hold, attempt, run=""):
    tag = f"{leg}_{rva}_a{attempt}{run}"
    log = ROOT / "original" / "c4n_ab.log"
    if log.exists():
        log.unlink()
    env = dict(os.environ)
    env["MASHED_ROOT"] = str(ROOT)
    env["MASHED_NO_SELFTEST"] = "1"
    env.pop("MASHED_SHADOW_AB", None)
    env.pop("MASHED_PHYS_C4_SELFTEST", None)
    hooks = f"L2_C4N_{rva}" if leg == "P" else f"FUN_{rva}"
    manifest = DATA / f"manifest_{tag}.tsv"     # HookSystem: one "idx rva installed name" row per entry
    if manifest.exists():
        manifest.unlink()
    env["MASHED_HOOK_MANIFEST"] = str(manifest)
    if leg == "P":
        env["MASHED_C4N"] = "1"
        env["MASHED_C4N_STRIDE"] = str(stride)
    else:
        env.pop("MASHED_C4N", None)
    cmd = [sys.executable, str(LOCK), "run", "--label", "D4-WSB-C4-NARROW", "--wait", "3600", "--",
           sys.executable, str(WT / "re" / "frida" / "scenario_launch.py"),
           "--track", "0", "--mode", "10", "--cars", "4", "--hold", str(hold), "--hooks", hooks]
    t0 = time.time()
    p = subprocess.run(cmd, env=env, capture_output=True, text=True, encoding="utf-8",
                       errors="replace", cwd=str(WT))
    out = (p.stdout or "") + (p.stderr or "")
    (DATA / f"launch_{tag}.txt").write_text(out, encoding="utf-8")
    if ("game exited." in out or "TIMEOUT" in out or "script has been destroyed" in out
            or "game exited while waiting for" in out):
        state = "CRASH"
    elif "reached a running race" in out:
        state = "RACE_OK"
    else:
        state = "VOID"
    saved = None
    if log.exists():
        saved = DATA / f"c4n_{tag}.log"
        shutil.move(str(log), str(saved))
    summ = ""
    if saved:
        txt = saved.read_text(encoding="utf-8", errors="replace")
        sums = [l for l in txt.splitlines() if l.startswith("SUM ")]
        pb = [l for l in txt.splitlines() if "PATCHBYTE" in l]
        summ = (sums[-1] if sums else "no-SUM") + " | " + (pb[0] if pb else "no-PATCHBYTE")
    if manifest.exists():
        rows = [l.split("\t") for l in manifest.read_text(encoding="utf-8", errors="replace").splitlines()]
        live = [f"{r[1]}:{r[3]}" for r in rows if len(r) >= 4 and r[2] == "1"]
        summ += f" | installed={len(live)} {','.join(live[:4])}"
    pid = re.search(r"pid=(\d+)", out)
    print(f"[{leg}] {rva} attempt={attempt} state={state} pid={pid.group(1) if pid else '?'} "
          f"t={round(time.time() - t0)}s {summ}", flush=True)
    return state, out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--legs", default="P,H")
    ap.add_argument("--rvas", default=",".join(RVAS))
    ap.add_argument("--stride", type=int, default=1)
    ap.add_argument("--hold", type=int, default=45)
    ap.add_argument("--run", default="", help="suffix for log names (repeat runs)")
    a = ap.parse_args()
    DATA.mkdir(parents=True, exist_ok=True)
    asi = ROOT / "original" / "mashed_re_dev.asi"
    print(f"asi {sha(asi)}  MASHED.exe.unpatched {sha(ROOT / 'original' / 'MASHED.exe.unpatched')}", flush=True)
    jobs = [(leg, r) for r in a.rvas.split(",") for leg in a.legs.split(",")]
    for n, (leg, rva) in enumerate(jobs, 1):
        print(f"[{n}/{len(jobs)}] {leg} {rva} START", flush=True)
        for attempt in (1, 2):
            state, out = boot(rva, leg, a.stride, a.hold, attempt, a.run)
            if state == "RACE_OK":
                break
    print("done", flush=True)


if __name__ == "__main__":
    main()
