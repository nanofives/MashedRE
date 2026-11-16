# RO-REF-FIXED-LOAD arms: the reference recipe of RO-CTL-WARP-N (warpn_arms.sh, plain p-runs) with the two holds,
# --load-sleep-ms (phase 2) and --boot-sleep-ms (boot, this row). Run under test-slot; every launch takes mashed_lock.
#   py -3.12 verify/ro_20261116/fixload_arms.py <label> [<label> ...] [-- extra detorig.py args]
# Env: RW (1 = --read-wait, the recipe; default 1), LS (--load-sleep-ms, default 0), BS (--boot-sleep-ms, default 0).
# FS (1 = --read-wait-rtfs, the recipe; default 1). r1..r4 ran with FS=0 (read wait on the piz poll only).
# f1 (pilot) ran the first form of this script: LS 50, BS 50, no read wait.
import os, subprocess, sys
from pathlib import Path

WT = Path(__file__).resolve().parent.parent.parent
LOCK = r"C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\orch-integration\re\orchestrator\mashed_lock.py"
D = Path("D:/MashedWT/RO-REF-FIXED-LOAD_data")
ROOT = r"D:\MashedWT\RO-REF-FIXED-LOAD_root"
RAW = ("0x00603400-0x00603800,0x00614400-0x00614800,0x00618000-0x00618400,0x006dcc00-0x006dd400,"
       "0x00773800-0x00773c00,0x007dd000-0x007dd400,0x007f0c00-0x007f1400")

argv = sys.argv[1:]
extra = []
if "--" in argv:
    i = argv.index("--"); extra = argv[i + 1:]; argv = argv[:i]
D.mkdir(parents=True, exist_ok=True)
env = dict(os.environ, MASHED_ROOT=ROOT)
rc_all = 0
for L in argv:
    out = D / L
    if out.exists():
        raise SystemExit(f"{out} exists; pick a new label")
    cmd = ["py", "-3.12", LOCK, "run", "--label", "RO-REF-FIXED-LOAD", "--",
           "py", "-3.12", "re/frida/detorig.py", "--label", L, "--out", out.as_posix(), "--clock", "tick", "--boot",
           "--q0", "0x40000000", "--tick-raw", RAW, "--timeout", os.environ.get("TMO", "600"), "--tick-cap", "600",
           "--warp-tick", "300", "--warp-clock", "300", "--normalize", "re/frida/warp_normalize.tsv",
           "--load-sleep-ms", os.environ.get("LS", "0"), "--boot-sleep-ms", os.environ.get("BS", "0"),
           "--call-log-boot", "--call-log", *(["--read-wait"] if os.environ.get("RW", "1") == "1" else []), *(["--read-wait-rtfs"] if os.environ.get("FS", "1") == "1" else []), "--ckpt-ticks", "0,1,17,18,299,301,600", *extra]
    with open(D / f"{L}.log", "w") as f:
        rc = subprocess.call(cmd, cwd=WT, env=env, stdout=f, stderr=subprocess.STDOUT)
    rc_all |= rc
    txt = (D / f"{L}.log").read_text(errors="replace").splitlines()
    print(f"[{L}] exit {rc}", flush=True)
    for line in txt:
        if any(k in line for k in ("bootlen", "readwait", "warp at", "race anchor", "=== rc", "exited", "error", "crash")):
            print("   ", line[:300], flush=True)
sys.exit(rc_all)
