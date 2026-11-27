# RO-AUDIOINIT-DEPTH arm: the ref1 recipe as run by T2-RACEPATH-CENSUS (verify/census_20261116/census_arms.py, the
# RO-REF-NAME argv), with this row's root, data folder and lock label, the c8 counter list (count_list8.txt, 6445
# entries) and six --prov-snap points that bound the window:
#   0x005b8910:enter  the current stop (first call)
#   0x0045d748:enter  first entry = end of bank 0 of the 0x0045d460 bank loop; the next store, mov [edi-8],esi at
#                     0x0045d6a5 with edi = 0x00604578 + 0x94, writes 0x00604604; no call lies between
#   0x0045d678:enter  0x0045d460 bank-loop head (split only)
#   0x004669b0:enter/leave, 0x0045d460:enter  the census audio-init split
# Run under test-slot; the launch takes mashed_lock.
#   py -3.12 verify/audioinit_depth_20261127/depth_arms.py <label>
import os, subprocess, sys
from pathlib import Path

WT = Path(__file__).resolve().parent.parent.parent
LOCK = r"C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\orch-integration\re\orchestrator\mashed_lock.py"
D = Path("D:/MashedWT/RO-AUDIOINIT-DEPTH_data")
ROOT = r"D:\MashedWT\RO-AUDIOINIT-DEPTH_root"
RAW = ("0x00603400-0x00603800,0x00614400-0x00614800,0x00618000-0x00618400,0x006dcc00-0x006dd400,"
       "0x00773800-0x00773c00,0x007dd000-0x007dd400,0x007f0c00-0x007f1400")
SNAP = "0x004669b0:enter,0x004669b0:leave,0x0045d460:enter,0x005b8910:enter,0x0045d678:enter,0x0045d748:enter"

env = dict(os.environ, MASHED_ROOT=ROOT)
rc_all = 0
for L in sys.argv[1:]:
    out = D / L
    if out.exists():
        raise SystemExit(f"{out} exists; pick a new label")
    cmd = ["py", "-3.12", LOCK, "run", "--label", "RO-AUDIOINIT-DEPTH", "--",
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
