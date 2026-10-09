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

# [GATEFIRE GF1 2026-10-08] FUN_0040e470 CarSlotStateGet, the value branch 2 tests.
#   return *(int*)(*(int**)0x005f2770 + param_1*4 + 0x34)
# (re/analysis/race_results/0040e470.md; 14 bytes, no branches, no calls.)
# WHY THIS IS HERE. FUN_004148b0:104 is `if (E470(i) == 1) last = i`, and the PORT
# reads {v0: 0, v1/v2/v3: 2} — never 1 — so `last` stays -1 and branch 2 dies at
# site 105 (verify/d3_gatefire_20261008/RESULT_GF1.md section 3). Whether the
# ORIGINAL also reads 2 here has never been measured: the 2026-10-03 witness logged
# idx364 (which made the :94 call site unreachable) and never instrumented :104.
# This is a POINTER-CHASE, not a flat read — the table base is behind 0x005f2770.
A_SLOTPTR = 0x005f2770   # PTR_PTR, deref then +0x34 + v*4
SLOT_OFF = 0x34

# [SCOPE_CALLWISE 2026-10-08] FUN_0046c7b0 car_alive, the gate on the port's
# ControlStep visit set. Ai_Standalone_Tick does `if (s_host.car_alive(v) == 1)
# VehicleStep(v)`, and the port's per-car call counts came out wildly uneven --
# v1 1,446 / v2 3,177 / v3 13,858 of 13,858 frames (RESULT_CALLWISE.md s1). This
# polls the ORIGINAL's same field so the two can be compared as a fraction of race.
#   FUN_0046c7b0(v): *(int*)(0x008815a4 + v*0xd04) == 1
# STRIDE: the decompiler renders this `(&DAT_008815a4)[v*0x341]` -- DWORD-indexed.
# The byte stride is 0xd04 (memory offset-grep-misses-dword-index). Using 0x341
# bytes here would read a different field entirely.
A_ALIVE = 0x008815a4
ALIVE_STRIDE = 0xd04

kTickScale = 1.0 / 3000.0   # _DAT_005cc948, Ai/AiStandalone.cpp:611

# [D-11073 CADENCE 2026-10-08, opt-in --cam] phase-3 exit flag + camera-path state.
# Pre-registration: verify/d3_gatefire_20261008/PREREG_CADENCE.md.
#   DAT_00897fe0 exit flag (FUN_004430a0 writes, FUN_004430b0 reads); it is ALSO the
#   head of the struct FUN_00445aa0 receives as param_2: [1..3] f32 target, [7]/[8]
#   written by FUN_00448700 at 0x0044871e/0x00448723.
#   DAT_00639d70/74/78 clip handle / cursor / view (FUN_00405430, FUN_00405460).
#   DAT_00657448 clip-handle source read at 0x004270bd; zero static writers.
#   Camera entries: base 0x008964c0, stride 0xd8, count DAT_00898994 (FUN_004464c0);
#   entry+4 type (0 -> FUN_00445aa0), +0x3c/+0x44 x/z, +0xa8 the clear-site guard.
#   Input bytes 0x007f1042 + k*0x4c, k=0..7 (FUN_00445aa0 clear site (b)).
A_FLAG = 0x00897fe0
A_CLIP70, A_CLIP74, A_CLIP78 = 0x00639d70, 0x00639d74, 0x00639d78
A_H657448 = 0x00657448
A_NCAM = 0x00898994
A_CAM0 = 0x008964c0
A_IN0, IN_STRIDE, IN_N = 0x007f1042, 0x4c, 8
A_CD29B8 = 0x005f29b8
A_RULE0FD0 = 0x007f0fd0
A_BA88 = 0x0063ba88
A_F1A50 = 0x007f1a50
CAM_N, CAM_STRIDE = 7, 0xd8   # entries 1..6 sampled too (PREREG_CADENCE.md addendum 2)
# [D-11073 LEG 0 2026-10-08] PREREG_LEG0.md. All three globals are BSS.
#   DAT_00644158 track id, FUN_00426c00's return (0x00426c01); FUN_00442600 switches on it.
#   DAT_0089898c FUN_00442600's once-guard.
#   DAT_0063d588 phase-5 timer (FUN_0041da90 reads it; FUN_0041d930 adds _DAT_007f100c);
#   DAT_0063d584 the int counter FUN_0041d930 bumps in the same call (frame witness).
A_TRK = 0x00644158
A_G898C = 0x0089898c
A_T588, A_N584 = 0x0063d588, 0x0063d584
# [D-11073 LEG 1a 2026-10-09] PREREG_LEG1A.md G-ORIG. Owed by RESULT_LEG0.md section 4 item 5.
#   DAT_0067ea64 FUN_0042f500's return: picks FUN_0040dbd0's helper (0 -> FUN_0041b520 on
#   the case-10 arm). _DAT_007f100c the per-frame step (0x0040fc57).
#   0x00882194/0x00882198 + v*0xd04: what FUN_004103a0's exit arm copies (FUN_0046c750/730).
#   Records 0x0063c8d0 + k*0x74 (k=0..3): FUN_0041b520 -> FUN_0041ae20 reads +0x64 (index),
#   writes +0x50/+0x54/+0x58 (row of 0x005f3304, +0x50 doubled) and +0x70 = 0.
A_EA64, A_STEP100C = 0x0067ea64, 0x007f100c
A_C2194, A_C2198 = 0x00882194, 0x00882198
A_REC8D0, REC_STRIDE = 0x0063c8d0, 0x74
CAM_COLS = (["flag_fe0", "tgt_fe4", "tgt_fe8", "tgt_fec", "ffc", "f800",
             "d70", "d74", "d78", "h657448", "n994",
             "e0_type", "e0_x", "e0_z", "e0_a8", "cd_29b8", "rule_0fd0", "ba88", "f1a50"]
            + ["in%d" % k for k in range(IN_N)]
            + ["e%d_%s" % (k, f) for k in range(1, CAM_N) for f in ("type", "a8", "x", "z")]
            + ["trk_644158", "g_898c", "t_d588", "n_d584", "e0_y"]
            + ["team_ea64", "dt_100c"]
            + ["c2194_%d" % v for v in range(4)] + ["c2198_%d" % v for v in range(4)]
            + ["r%d_%s" % (k, f) for k in range(4) for f in ("i64", "f50", "f54", "f58", "i70")])

# [D-11073 LEG 1b 2026-10-09, opt-in --leg1b] PREREG_LEG1B.md section 2. Inputs of
# FUN_004103a0 / FUN_0040dbd0 not polled before, and the phase-5 reference for G-HOLD / G-EXIT:
#   DAT_007f1018 / DAT_007f1014 (0x004103db / 0x004103f6), DAT_0063ba78 (0x004103fb),
#   DAT_0063d7e4 and its +0x40 slot (FUN_0041ea80 / FUN_0041e960, called by FUN_00426c90),
#   DAT_008991bc (FUN_00429860), DAT_0067ed6c (FUN_00430820),
#   the AI mode-5 countdown 0x0089a4f8 + v*0x74 (FUN_00418560 0x004185ab),
#   the accel byte FUN_0046d7f0 reads: 0x007f103c + [0x007f1a14 + v*16]*0x4c (0x0046d801..0x0046d80a),
#   0x007f0a04/08 + v*12 (FUN_004103a0's exit-arm copies, 0x004104a7 / 0x004104b2),
#   record world position +0x958/+0x95c/+0x960 (0x00881ef8.. + v*0xd04).
A_F1018, A_F1014, A_BA78 = 0x007f1018, 0x007f1014, 0x0063ba78
A_D7E4, A_91BC, A_ED6C = 0x0063d7e4, 0x008991bc, 0x0067ed6c
A_CD4F8, AI_STRIDE = 0x0089a4f8, 0x74
A_CTRLIDX, A_ACC = 0x007f1a14, 0x007f103c
A_0A04 = 0x007f0a04
A_POS = 0x00881ef8
L1B_COLS = (["f1018", "f1014", "ba78", "d7e4", "d7e4_40", "g91bc", "ged6c"]
            + ["cd4f8_%d" % v for v in range(4)] + ["acc_%d" % v for v in range(4)]
            + ["a0a04_%d" % v for v in range(4)] + ["a0a08_%d" % v for v in range(4)]
            + ["%s_%d" % (c, v) for v in range(4) for c in ("px", "py", "pz")])

# [U-9201 2026-10-09, opt-in --u9201] RESULT_LEG2.md section 2 / U-9201. The race-camera struct
# DAT_00897fe0 (FUN_00446520's param_1) and the time inputs its main block reads, so the port's
# RaceCamera (which cites these offsets) can be compared field by field from the first phase-3 frame:
#   +0x40..+0x48 pos, +0x4c..+0x54 look target, +0x58 view window, +0x1c (cam[7]), +0x24 (DAT_00803344),
#   +0x964..+0x9a0 [0x259..0x268] smoothing / spring state, pair ids, blend, zoom;
#   DAT_007f1030 (int, FUN_0040fc00 L172: _ftol2 of _DAT_007f1010; main block L1039 reads it / 60000),
#   _DAT_007f1010 (float accumulator), DAT_007f1000 (its gate, L170), _DAT_007f100c (step),
#   DAT_007f0fc8 (jitter amplitude), DAT_00803344.
A_CAMS = 0x00897fe0
A_T1030, A_T1010, A_T1000, A_FC8, A_3344 = 0x007f1030, 0x007f1010, 0x007f1000, 0x007f0fc8, 0x00803344
U9201_F = [("cpx", 0x40), ("cpy", 0x44), ("cpz", 0x48), ("ctx", 0x4c), ("cty", 0x50), ("ctz", 0x54),
           ("cvw", 0x58), ("c24", 0x24)] +           [("s%03x" % o, o) for o in range(0x964, 0x994, 4)] + [("c99c", 0x99c), ("c9a0", 0x9a0)]
U9201_COLS = ([n for n, _ in U9201_F] + ["c1c", "c994", "c998", "t1030", "t1010", "t1000", "step100c", "fc8", "g3344"])

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
    cam = "--cam" in argv
    if cam:
        argv.remove("--cam")
    leg1b = "--leg1b" in argv
    u9201 = "--u9201" in argv
    if u9201:
        argv.remove("--u9201")
    if leg1b:
        argv.remove("--leg1b")
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
            "substate", "ord0", "ord1", "ord2", "ord3",
            "slotptr", "e470_0", "e470_1", "e470_2", "e470_3",
            "alive_0", "alive_1", "alive_2", "alive_3"]
    if cam:
        cols += CAM_COLS
    if leg1b:
        cols += L1B_COLS
    if u9201:
        cols += U9201_COLS
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
            # FUN_0040e470: deref the table pointer, then +0x34 + v*4. A null or
            # unreadable base records None (empty cell) rather than 0, so "table not
            # there" can never be mistaken for "table says 0".
            base = i32(h, A_SLOTPTR)
            r["slotptr"] = base
            for s in range(4):
                r["e470_%d" % s] = (i32(h, base + SLOT_OFF + s * 4)
                                    if base else None)
            for s in range(4):
                r["alive_%d" % s] = i32(h, A_ALIVE + s * ALIVE_STRIDE)
            if cam:
                r.update({
                    "flag_fe0": i32(h, A_FLAG), "tgt_fe4": f32(h, A_FLAG + 4),
                    "tgt_fe8": f32(h, A_FLAG + 8), "tgt_fec": f32(h, A_FLAG + 0xc),
                    "ffc": i32(h, A_FLAG + 0x1c), "f800": i32(h, A_FLAG + 0x20),
                    "d70": i32(h, A_CLIP70), "d74": f32(h, A_CLIP74), "d78": i32(h, A_CLIP78),
                    "h657448": i32(h, A_H657448), "n994": i32(h, A_NCAM),
                    "e0_type": i32(h, A_CAM0 + 4), "e0_x": f32(h, A_CAM0 + 0x3c),
                    "e0_z": f32(h, A_CAM0 + 0x44), "e0_a8": i32(h, A_CAM0 + 0xa8),
                    "cd_29b8": i32(h, A_CD29B8), "rule_0fd0": i32(h, A_RULE0FD0),
                    "ba88": i32(h, A_BA88), "f1a50": i32(h, A_F1A50)})
                for k in range(IN_N):
                    b = read(h, A_IN0 + k * IN_STRIDE, 1)
                    r["in%d" % k] = None if b is None else b[0]
                for k in range(1, CAM_N):
                    e = A_CAM0 + k * CAM_STRIDE
                    r["e%d_type" % k] = i32(h, e + 4)
                    r["e%d_a8" % k] = i32(h, e + 0xa8)
                    r["e%d_x" % k] = f32(h, e + 0x3c)
                    r["e%d_z" % k] = f32(h, e + 0x44)
                r.update({"trk_644158": i32(h, A_TRK), "g_898c": i32(h, A_G898C),
                          "t_d588": f32(h, A_T588), "n_d584": i32(h, A_N584),
                          "e0_y": f32(h, A_CAM0 + 0x40)})
                r.update({"team_ea64": i32(h, A_EA64), "dt_100c": f32(h, A_STEP100C)})
                for v in range(4):
                    r["c2194_%d" % v] = i32(h, A_C2194 + v * ALIVE_STRIDE)
                    r["c2198_%d" % v] = i32(h, A_C2198 + v * ALIVE_STRIDE)
                for k in range(4):
                    rb = A_REC8D0 + k * REC_STRIDE
                    r["r%d_i64" % k] = i32(h, rb + 0x64)
                    r["r%d_f50" % k] = f32(h, rb + 0x50)
                    r["r%d_f54" % k] = f32(h, rb + 0x54)
                    r["r%d_f58" % k] = f32(h, rb + 0x58)
                    r["r%d_i70" % k] = i32(h, rb + 0x70)
            if leg1b:
                d7 = i32(h, A_D7E4)
                r.update({"f1018": i32(h, A_F1018), "f1014": i32(h, A_F1014),
                          "ba78": i32(h, A_BA78), "d7e4": d7,
                          "d7e4_40": (i32(h, d7 + 0x40) if d7 else None),
                          "g91bc": i32(h, A_91BC), "ged6c": i32(h, A_ED6C)})
                for v in range(4):
                    r["cd4f8_%d" % v] = i32(h, A_CD4F8 + v * AI_STRIDE)
                    ci = i32(h, A_CTRLIDX + v * 16)
                    b = read(h, A_ACC + ci * 0x4c, 1) if ci is not None and 0 <= ci < 16 else None
                    r["acc_%d" % v] = None if b is None else b[0]
                    r["a0a04_%d" % v] = i32(h, A_0A04 + v * 12)
                    r["a0a08_%d" % v] = i32(h, A_0A04 + 4 + v * 12)
                    for c, off in (("px", 0), ("py", 4), ("pz", 8)):
                        r["%s_%d" % (c, v)] = f32(h, A_POS + off + v * ALIVE_STRIDE)
            if u9201:
                for n, o in U9201_F:
                    r[n] = f32(h, A_CAMS + o)
                r.update({"c1c": i32(h, A_CAMS + 0x1c), "c994": i32(h, A_CAMS + 0x994),
                          "c998": i32(h, A_CAMS + 0x998), "t1030": i32(h, A_T1030),
                          "t1010": f32(h, A_T1010), "t1000": i32(h, A_T1000),
                          "step100c": f32(h, A_STEP100C), "fc8": f32(h, A_FC8),
                          "g3344": f32(h, A_3344)})
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
    from collections import Counter as _C
    print("")
    print("=== FUN_0040e470 per car (branch 2 needs == 1 at FUN_004148b0:104) ===")
    print("  slotptr non-null on %d/%d samples"
          % (sum(1 for r in rows if r.get("slotptr")), len(rows)))
    for s in range(4):
        c = _C(r.get("e470_%d" % s) for r in rows)
        print("  v%d: %s" % (s, c.most_common(5)))
    allv = sorted({r.get("e470_%d" % s) for r in rows for s in range(4)
                   if r.get("e470_%d" % s) is not None})
    print("  values present: %s" % allv)
    print("  ANY car ever reading exactly 1? %s" % (1 in allv))
    print("")
    print("=== FUN_0046c7b0 car_alive per car (== 1 means VehicleStep runs) ===")
    for s in range(4):
        vals = [r.get("alive_%d" % s) for r in rows if r.get("alive_%d" % s) is not None]
        ones = sum(1 for x in vals if x == 1)
        print("  v%d: ==1 on %d/%d samples (%.1f%%)   values %s"
              % (s, ones, len(vals), 100.0 * ones / max(len(vals), 1),
                 _C(vals).most_common(4)))
    print("  PORT for comparison: v1 1446/13858 (10.4%), v2 3177 (22.9%), v3 13858 (100%)")
    print("  (the PORT reads {v0: 0, v1/v2/v3: 2} and never 1 -- RESULT_GF1.md s3)")
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
