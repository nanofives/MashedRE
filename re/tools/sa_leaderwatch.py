"""Knob-took witness for the FUN_004148b0 wiring (D3, 2026-10-03). READ-ONLY on the game.

Pre-registration: verify/d3_leader_20261003/PREREG_WITNESS.md (leg W-DATA, port side).

Spawns the STANDALONE mashed_re.exe exactly the way re/tools/sa_capture.py does and polls
its memory with ReadProcessMemory -- no injection, no Frida, no game-code change, no build.
It samples EVERY address `Ai/AiLeaderTimer.cpp`'s LeaderTimer reads, so the question "would
wiring it into mashed_re.exe be inert?" is answered from data rather than from reasoning.

WHY THESE ARE THE ORIGINAL'S ABSOLUTE VAs AND NOT THIS EXE'S STATICS. exe_main.cpp:56
VirtualAlloc-maps 0x00500000..0x009fffff into the standalone, so MASHED's .data/.rdata/.bss
address range is READABLE in mashed_re.exe. Whether it is POPULATED is exactly what this
tool measures. (The .text range 0x00400000..0x004fffff is NOT mapped --
Compat/StandaloneRvaThunks.h:7 -- which is leg W-SAFE and needs no run.)

W-BASE, registered before the run because "all zeros" is also what a bad base prints:
  1. the reader also samples 0x0089a52c+v*0x74 (ai_mode) and 0x007f0ff4 (frame), which the
     same run's MASHED_AI_STEPDUMP logs independently from inside the game;
  2. 0x007f0ff4 must ADVANCE across samples;
  3. at least one sampled address must be NON-ZERO.
Any leg failing makes the result VOID, not interpretable.

Usage:
  py -3.12 re/tools/sa_leaderwatch.py <out_prefix> <seconds> [ENV=VAL ...] [--hz 20]

Writes <out_prefix>.leader.json (summary) and <out_prefix>.leader.csv (every sample).
The process is spawned and killed BY PID; no blanket kill by name.
"""
import ctypes, json, os, struct, subprocess, sys, time
from ctypes import wintypes
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent.parent
EXE = ROOT / "mashedmod" / "build" / "mashed_re.exe"

# ---- every address LeaderTimer touches, cited to Ai/AiLeaderTimer.cpp ----
A_MODE368  = 0x0089a368   # :91  mode gate (== 2 -> return 0)
A_FLT360   = 0x0089a360   # :92  float, __ftol'd -> iVar1
A_IDX364   = 0x0089a364   # :93  idx364 (!= -1 -> E470)
A_BIAS374  = 0x0089a374   # :98  table bias
A_LIMITTBL = 0x005f2dd8   # :98  int limit table            [INITIALISED RODATA]
A_RANK     = 0x0089a4c4   # :99  RankAt  = base + v*0x74
A_TIMER    = 0x0089a4c8   # :108 TimerAt = base + v*0x74
A_PROG     = 0x008989b0   # :101/:106 via FUN_00442cc0, stride 4
A_FRAMEDT  = 0x007f1008   # :108 frame delta
A_THR_A8   = 0x005cd0a8   # :107                             [INITIALISED RODATA]
A_THR_A4   = 0x005cd0a4   # :116                             [INITIALISED RODATA]
A_THR_A0   = 0x005cd0a0   # :118                             [INITIALISED RODATA]
A_THR_35C  = 0x005cc35c   # :119 (4.0)                       [INITIALISED RODATA]
# ---- W-BASE control channels (known-written by the exe's own AI path) ----
A_AIMODE   = 0x0089a52c   # AiStandalone.cpp:856, stride 0x74
A_FRAME    = 0x007f0ff4   # AiStandalone.cpp:859
VSTRIDE    = 0x74
NSLOT      = 4

PROCESS_VM_READ = 0x0010
PROCESS_QUERY_INFORMATION = 0x0400

k32 = ctypes.WinDLL("kernel32", use_last_error=True)
k32.OpenProcess.restype = wintypes.HANDLE
k32.ReadProcessMemory.argtypes = [wintypes.HANDLE, ctypes.c_void_p, ctypes.c_void_p,
                                  ctypes.c_size_t, ctypes.POINTER(ctypes.c_size_t)]


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
    hz = 20.0
    if "--hz" in argv:
        i = argv.index("--hz"); hz = float(argv[i + 1]); del argv[i:i + 2]
    prefix, secs = argv[0], float(argv[1])

    env = dict(os.environ)
    env.setdefault("MASHED_WIN_POS", "primary-bl")
    for kv in argv[2:]:
        k, _, v = kv.partition("=")
        env[k] = v

    proc = subprocess.Popen([str(EXE)], cwd=str(ROOT), env=env,
                            stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    print("[pid] %d  (spawned and killed BY PID)" % proc.pid)
    h = k32.OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, False, proc.pid)
    if not h:
        proc.kill()
        sys.exit("OpenProcess failed: %d" % ctypes.get_last_error())

    rows, n_try, n_ok = [], 0, 0
    t0 = time.time()
    try:
        while time.time() - t0 < secs:
            n_try += 1
            r = {"t": round(time.time() - t0, 3)}
            r["frame"] = i32(h, A_FRAME)
            if r["frame"] is None:
                time.sleep(1.0 / hz); continue
            n_ok += 1
            r["mode368"] = i32(h, A_MODE368)
            r["flt360"] = f32(h, A_FLT360)
            r["idx364"] = i32(h, A_IDX364)
            r["bias374"] = i32(h, A_BIAS374)
            r["framedt"] = i32(h, A_FRAMEDT)
            r["thr_a8"] = f32(h, A_THR_A8)
            r["thr_a4"] = f32(h, A_THR_A4)
            r["thr_a0"] = f32(h, A_THR_A0)
            r["thr_35c"] = f32(h, A_THR_35C)
            for v in range(NSLOT):
                r["prog%d" % v] = f32(h, A_PROG + v * 4)
                r["rank%d" % v] = i32(h, A_RANK + v * VSTRIDE)
                r["timer%d" % v] = i32(h, A_TIMER + v * VSTRIDE)
                r["aimode%d" % v] = i32(h, A_AIMODE + v * VSTRIDE)
            tbl = read(h, A_LIMITTBL, 64 * 4)
            r["limittbl_nonzero"] = (sum(struct.unpack("<64i", tbl)) != 0) if tbl else None
            r["limittbl_head"] = list(struct.unpack("<8i", tbl[:32])) if tbl else None
            rows.append(r)
            time.sleep(1.0 / hz)
    finally:
        proc.kill()
        print("[pid] %d killed" % proc.pid)

    if not rows:
        sys.exit("no samples -- the process never became readable")

    cols = [k for k in rows[0] if k != "limittbl_head"]
    outc = Path(prefix + ".leader.csv")
    outc.parent.mkdir(parents=True, exist_ok=True)
    with open(outc, "w", newline="") as f:
        f.write(",".join(cols) + "\n")
        for r in rows:
            f.write(",".join("" if r.get(c) is None else str(r.get(c)) for c in cols) + "\n")

    # ---- W-BASE legs ----
    frames = [r["frame"] for r in rows if r["frame"] is not None]
    advanced = len(set(frames)) > 1
    nonzero = sorted({c for r in rows for c in cols
                      if isinstance(r.get(c), (int, float)) and r.get(c) not in (0, 0.0)
                      and c != "t"})
    summary = {
        "samples": len(rows), "tries": n_try, "ok": n_ok,
        "W_BASE_frame_advanced": advanced,
        "W_BASE_frame_range": [min(frames), max(frames)] if frames else None,
        "W_BASE_nonzero_fields": nonzero,
        "limittbl_nonzero_any": any(bool(r.get("limittbl_nonzero")) for r in rows),
        "limittbl_head_last": rows[-1].get("limittbl_head"),
        "prog_nonzero_any": any(r.get("prog%d" % v) not in (0.0, None)
                                for r in rows for v in range(NSLOT)),
        "thresholds_nonzero_any": any(r.get(k) not in (0.0, None) for r in rows
                                      for k in ("thr_a8", "thr_a4", "thr_a0", "thr_35c")),
        "aimode_seen": sorted({r.get("aimode%d" % v) for r in rows for v in range(NSLOT)
                               if r.get("aimode%d" % v) is not None}),
    }
    Path(prefix + ".leader.json").write_text(json.dumps(summary, indent=1))
    for k, v in summary.items():
        print("  %-26s %s" % (k, v))
    return 0


if __name__ == "__main__":
    sys.exit(main())
