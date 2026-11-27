# RACE-PIZ-DEVICE CUT census arm: the RO-AUDIOINIT-DEPTH recipe (verify/audioinit_depth_20261127/depth_arms.py, itself the
# T2-RACEPATH-CENSUS ref1 recipe) with this row's root, data folder and lock label, the c8 counter list (count_list8.txt,
# 6445 entries) and snaps that bound the window "after the COURSE.LUA open":
#   0x0047a020:enter  CourseDescInit, single caller 0x00426e10 (track loader); it calls 0x0047b9b0 (0x0047a0d9) ->
#                     0x0047b8d0, whose RtFS open of COURSE.LUA is the exe arm's stop. Its first enter is the race load.
#   0x0047b8d0:enter  the lua-file loader (first enter is the boot powerups.lua through 0x0045bae0, info only)
#   0x00426e10:enter  the track loader (info only)
# Run under test-slot; the launch takes mashed_lock.
#   py -3.12 verify/racepiz_20261127/cut_census.py <label>
import os, subprocess, sys
from pathlib import Path

WT = Path(__file__).resolve().parent.parent.parent
LOCK = r"C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\orch-integration\re\orchestrator\mashed_lock.py"
D = Path("D:/MashedWT/RACE-PIZ-DEVICE_data/census")
ROOT = r"D:\MashedWT\RACE-PIZ-DEVICE_census_root"
RAW = ("0x00603400-0x00603800,0x00614400-0x00614800,0x00618000-0x00618400,0x006dcc00-0x006dd400,"
       "0x00773800-0x00773c00,0x007dd000-0x007dd400,0x007f0c00-0x007f1400")
SNAP = "0x00426e10:enter,0x0047a020:enter,0x0047b8d0:enter"

env = dict(os.environ, MASHED_ROOT=ROOT)
rc_all = 0
for L in sys.argv[1:]:
    out = D / L
    if out.exists():
        raise SystemExit(f"{out} exists; pick a new label")
    cmd = ["py", "-3.12", LOCK, "run", "--label", "RACE-PIZ-DEVICE", "--",
           "py", "-3.12", "re/frida/detorig.py", "--label", L, "--out", out.as_posix(), "--clock", "tick", "--boot",
           "--q0", "0x40000000", "--tick-raw", RAW, "--timeout", os.environ.get("TMO", "600"), "--tick-cap", "600",
           "--warp-tick", "300", "--warp-clock", "300", "--normalize", "re/frida/warp_normalize.tsv",
           "--load-sleep-ms", "0", "--boot-sleep-ms", "0",
           "--call-log-boot", "--call-log", "--read-wait", "--read-wait-rtfs", "--ckpt-ticks", "0,1,17,18,299,301,600",
           "--prov-count", (D / "count_list8.txt").as_posix(), "--prov-snap", SNAP]
    with open(D / f"{L}.log", "w") as f:
        rc = subprocess.call(cmd, cwd=WT, env=env, stdout=f, stderr=subprocess.STDOUT)
    rc_all |= rc
    print(f"[{L}] exit {rc}", flush=True)
    for line in (D / f"{L}.log").read_text(errors="replace").splitlines():
        if any(k in line for k in ("bootlen", "warp at", "race anchor", "=== rc", "exited", "error", "crash", "snap")):
            print("   ", line[:300], flush=True)
sys.exit(rc_all)
