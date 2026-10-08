"""Original-side ramp watcher for 0x0089a374 (bias374). READ-ONLY on the game.

Pre-registration / motivation: verify/d3_gatefire_20261008/RESULT_BIAS374.md section 5.

WHAT QUESTION THIS ANSWERS. bias374 is a monotone band index over elapsed race time
(port: 0->1->2->3->4 at frames 0/661/1262/2463/3664). The only original-side capture of it
is a 220-frame slice (verify/d3_leader_20261003/o1.msd.leaderprobe.csv, frames 794..1013 of
a 1014-frame capture) where it reads 0 on 512/512. That is too short to say whether the
original ramps at all. This tool samples the ORIGINAL over a full race so the two ramps can
be overlaid.

WHY A POLL AND NOT A HOOK. FUN_004148b0 fires only on a rare mode-5 path, which is why the
existing capture is a slice. scenario_launch.py's 0x004177b0 probe watches the finish-order
quartet at 0x0089a870, not 0x0089a374. A per-frame poll sidesteps both.

NO INJECTION. ReadProcessMemory only -- no Frida hooks of our own, no writes, no game-code
change, no build. scenario_launch.py drives the race and owns the process lifetime.

PROCESS HYGIENE (mandatory, see CLAUDE.md "MASHED process hygiene"). MASHED is NOT
single-instance and other sessions may be running their own. This tool:
  * snapshots the set of MASHED.exe PIDs BEFORE launching,
  * adopts only a PID that appears AFTER (the set difference),
  * refuses to guess if more than one new PID appears,
  * NEVER kills MASHED -- scenario_launch.py kills the pid it spawned.

Usage:
  py -3.12 re/tools/orig_rampwatch.py <out.csv> [--hz 10] [--wait 180] -- <scenario_launch args...>

Example:
  py -3.12 re/tools/orig_rampwatch.py verify/.../o_ramp.csv --hz 10 -- \
      --track 0 --mode 10 --cars 4 --car 0 --poke-ctrl-slots --hold 260
"""
import ctypes
import os
import struct
import subprocess
import sys
import time
from ctypes import wintypes
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent.parent
LAUNCH = ROOT / "re" / "frida" / "scenario_launch.py"

# ---- the ramp and everything that drives it ----------------------------------
# bias374's writer is the tickscale band ladder; in the port that is
# Ai/AiStandalone.cpp:1557-1577 (ported from FUN_004177b0).
A_BIAS374 = 0x0089a374   # the band index under test                       (int)
A_LAST370 = 0x0089a370   # "tickscale when the band last fired"            (float)
A_TICK0FF8 = 0x007f0ff8  # tickscale clock; ZEROED on mode-5 entry
                         # (Ai/AiState.h:87) -- which is exactly why its value
                         # in the original's window cannot be inferred.
A_CLK0FF4 = 0x007f0ff4   # the free-running frame clock (ticks 50/frame)
A_FRAMEDT = 0x007f1008   # frame delta (50 on both sides)
A_MODE368 = 0x0089a368   # slow-line difficulty flag
A_FLT360 = 0x0089a360    # difficulty row float (2.5 in the original)
A_IDX364 = 0x0089a364    # the -1 sentinel (expected -1 throughout)
A_DIFF7C = 0x0067ea7c    # RaceConfig.difficulty (scenario_launch.py:47)
A_SUBMODE = 0x0067e9fc   # FUN_0042f6a0 GetRaceSubMode
A_SUBSTATE = 0x0063ba8c  # render sub-state (3=driving, 5/6/7=standings)
A_ORDER = 0x0089a870     # finish-order quartet, stride 4 (all -1 at round init)

kTickScale = 1.0 / 3000.0   # _DAT_005cc948, Ai/AiStandalone.cpp:611

PROCESS_VM_READ = 0x0010
PROCESS_QUERY_INFORMATION = 0x0400

k32 = ctypes.WinDLL("kernel32", use_last_error=True)
k32.OpenProcess.restype = wintypes.HANDLE
k32.ReadProcessMemory.argtypes = [wintypes.HANDLE, ctypes.c_void_p, ctypes.c_void_p,
                                  ctypes.c_size_t, ctypes.POINTER(ctypes.c_size_t)]


def mashed_pids():
    """Set of live MASHED.exe PIDs. Used only for the before/after difference."""
    try:
        out = subprocess.run(["tasklist", "/FI", "IMAGENAME eq MASHED.exe",
                              "/FO", "CSV", "/NH"],
                             capture_output=True, text=True, timeout=30).stdout
    except Exception:
        return set()
    pids = set()
    for line in out.splitlines():
        parts = [p.strip('"') for p in line.split('","')]
        if len(parts) >= 2 and parts[0].lower().startswith("mashed"):
            try:
                pids.add(int(parts[1]))
            except ValueError:
                pass
    return pids


def read(h, addr, n):
    buf = (ctypes.c_char * n)()
    got = ctypes.c_size_t(0)
    ok = k32.ReadProcessMemory(h, ctypes.c_void_p(addr), buf, n, ctypes.byref(got))
    if not ok or got.value != n:
        return None
    return bytes(buf)


def i32(h, a):
    b = read(h, a, 4)
    return None if b is None else struct.unpack("<i", b)[0]


def f32(h, a):
    b = read(h, a, 4)
    return None if b is None else struct.unpack("<f", b)[0]


def main():
    argv = sys.argv[1:]
    if not argv:
        print(__doc__)
        return 2
    hz, wait_s = 10.0, 180.0
    if "--hz" in argv:
        i = argv.index("--hz"); hz = float(argv[i + 1]); del argv[i:i + 2]
    if "--wait" in argv:
        i = argv.index("--wait"); wait_s = float(argv[i + 1]); del argv[i:i + 2]
    out = Path(argv[0]); argv = argv[1:]
    if argv and argv[0] == "--":
        argv = argv[1:]

    before = mashed_pids()
    print("[hygiene] MASHED.exe PIDs already running (NOT touched): %s"
          % (sorted(before) or "none"))

    cmd = [sys.executable, str(LAUNCH)] + argv
    print("[launch] %s" % " ".join(cmd))
    proc = subprocess.Popen(cmd, cwd=str(ROOT))

    # ---- adopt exactly one NEW pid, or refuse ----
    pid, t0 = None, time.time()
    while time.time() - t0 < wait_s:
        new = mashed_pids() - before
        if len(new) == 1:
            pid = new.pop()
            break
        if len(new) > 1:
            proc.terminate()
            sys.exit("REFUSING to guess: %d new MASHED.exe PIDs appeared (%s). "
                     "Another session may have started one concurrently." % (len(new), sorted(new)))
        if proc.poll() is not None:
            sys.exit("scenario_launch exited (rc=%s) before a MASHED.exe appeared" % proc.returncode)
        time.sleep(0.5)
    if pid is None:
        proc.terminate()
        sys.exit("no new MASHED.exe within %.0fs" % wait_s)
    print("[adopt] polling pid %d (READ-ONLY; this tool never kills MASHED)" % pid)

    h = k32.OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, False, pid)
    if not h:
        print("OpenProcess failed: %d" % ctypes.get_last_error())
        proc.wait()
        return 1

    cols = ["t", "clk_0ff4", "tick_0ff8", "tickscale", "bias374", "last370",
            "framedt", "mode368", "flt360", "idx364", "diff_67ea7c", "submode",
            "substate", "ord0", "ord1", "ord2", "ord3"]
    rows = []
    try:
        while proc.poll() is None:
            clk = i32(h, A_CLK0FF4)
            if clk is None:
                time.sleep(1.0 / hz)
                continue
            tick = i32(h, A_TICK0FF8)
            r = {
                "t": round(time.time() - t0, 3),
                "clk_0ff4": clk,
                "tick_0ff8": tick,
                "tickscale": None if tick is None else round(tick * kTickScale, 6),
                "bias374": i32(h, A_BIAS374),
                "last370": f32(h, A_LAST370),
                "framedt": i32(h, A_FRAMEDT),
                "mode368": i32(h, A_MODE368),
                "flt360": f32(h, A_FLT360),
                "idx364": i32(h, A_IDX364),
                "diff_67ea7c": i32(h, A_DIFF7C),
                "submode": i32(h, A_SUBMODE),
                "substate": i32(h, A_SUBSTATE),
            }
            for s in range(4):
                r["ord%d" % s] = f32(h, A_ORDER + s * 4)
            rows.append(r)
            time.sleep(1.0 / hz)
    finally:
        proc.wait()
        print("[hygiene] scenario_launch exited rc=%s; MASHED pid %d was NOT killed by this tool"
              % (proc.returncode, pid))

    if not rows:
        sys.exit("no samples -- the process never became readable")

    out.parent.mkdir(parents=True, exist_ok=True)
    with open(out, "w", newline="") as f:
        f.write(",".join(cols) + "\n")
        for r in rows:
            f.write(",".join("" if r.get(c) is None else str(r.get(c)) for c in cols) + "\n")
    print("[out] %s  rows=%d" % (out, len(rows)))

    # ---- the answer, printed so it cannot be missed ----
    seen = [r["bias374"] for r in rows if r["bias374"] is not None]
    ticks = [r["tick_0ff8"] for r in rows if r["tick_0ff8"] is not None]
    clks = [r["clk_0ff4"] for r in rows if r["clk_0ff4"] is not None]
    print("\n=== ORIGINAL-side ramp ===")
    print("  clk_0ff4   range %s .. %s" % (min(clks), max(clks)) if clks else "  clk: none")
    print("  tick_0ff8  range %s .. %s" % (min(ticks), max(ticks)) if ticks else "  tick: none")
    if ticks:
        print("  tickscale  range %.3f .. %.3f  (seconds; bands at 2/12/22/42/60-62)"
              % (min(ticks) * kTickScale, max(ticks) * kTickScale))
    print("  bias374 distinct values: %s" % sorted(set(seen)))
    prev = None
    for r in rows:
        if r["bias374"] != prev:
            print("    t=%7.2fs  clk=%-8s tick=%-8s tickscale=%-8s -> bias374=%s"
                  % (r["t"], r["clk_0ff4"], r["tick_0ff8"], r["tickscale"], r["bias374"]))
            prev = r["bias374"]
    print("\n  LIVENESS: clk advanced = %s  (a flat clock makes every row above VOID)"
          % (len(set(clks)) > 1))
    return 0


if __name__ == "__main__":
    sys.exit(main())
