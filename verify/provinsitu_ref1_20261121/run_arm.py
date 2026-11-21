# PROV-INSITU-REF1 arm: py -3.12 run_arm.py <label> [detorig.py args...]   (under test-slot; takes the game lock itself)
# The ref1 recipe (verify/ro_20261116/REFERENCE_RO-REF-NAME.json argv: boot mode, tick clock, q0 0x40000000, warp at
# tick 300, 600 ticks, read wait on the piz poll and RtFS, load/boot sleep 0, call logs, same checkpoints) plus the
# PROV options prov_bisect.py passes (--prov FILE --prov-rvas LIST). Private root, binaries identical to the ref1 root.
import os, subprocess, sys, time
from pathlib import Path

WT = Path(__file__).resolve().parent.parent.parent
LOCK = r"C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\orch-integration\re\orchestrator\mashed_lock.py"
D = Path("D:/MashedWT/PROV-INSITU-REF1_data")
ROOT = r"D:\MashedWT\PROV-INSITU-REF1_root"
RAW = ("0x00603400-0x00603800,0x00614400-0x00614800,0x00618000-0x00618400,0x006dcc00-0x006dd400,"
       "0x00773800-0x00773c00,0x007dd000-0x007dd400,0x007f0c00-0x007f1400")

L, extra = sys.argv[1], sys.argv[2:] + os.environ.get("ARM_EXTRA", "").split()   # ARM_EXTRA: e.g. --crash-catch on repeats
D.mkdir(parents=True, exist_ok=True)
out = D / L
if out.exists():
    import shutil; shutil.rmtree(out)
st = subprocess.run(["py", "-3.12", LOCK, "status"], capture_output=True, text=True).stdout.strip().splitlines()[:3]
with open(D / "lockstatus.log", "a") as f:
    f.write(f"{time.strftime('%Y-%m-%d %H:%M:%S')} {L}: {' | '.join(st)}\n")
cmd = ["py", "-3.12", LOCK, "run", "--label", "PROV-INSITU-REF1", "--",
       "py", "-3.12", "re/frida/detorig.py", "--label", L, "--out", out.as_posix(), "--clock", "tick", "--boot",
       "--q0", "0x40000000", "--tick-raw", RAW, "--timeout", os.environ.get("TMO", "600"), "--tick-cap", "600",
       "--warp-tick", "300", "--warp-clock", "300", "--normalize", "re/frida/warp_normalize.tsv",
       "--load-sleep-ms", "0", "--boot-sleep-ms", "0", "--call-log-boot", "--call-log", "--read-wait", "--read-wait-rtfs",
       "--ckpt-ticks", "0,1,17,18,299,301,600", *extra]
env = dict(os.environ, MASHED_ROOT=ROOT)
with open(D / f"{L}.log", "w") as f:
    rc = subprocess.call(cmd, cwd=WT, env=env, stdout=f, stderr=subprocess.STDOUT)
print(f"[{L}] exit {rc} {time.strftime('%H:%M:%S')}", flush=True)
for line in (D / f"{L}.log").read_text(errors="replace").splitlines():
    if any(k in line for k in ("bootlen", "prov", "warp at", "race anchor", "ticks ", "=== rc", "exited", "error", "crash")):
        print("   ", line[:300], flush=True)
sys.exit(rc)
