# FIX-00420420-CRASH arm: the PROV-INSITU-REF1 recipe (verify/provinsitu_ref1_20261121/run_arm.py argv exactly),
# taken under one mashed_lock --solo window so no other Mashed boot or piz load runs at the same time.
# Runs: c0 (no body, control) then b1 (body 0x00420420 from bodies309.json, --crash-catch 20).
# Usage (under test-slot): py -3.12 verify/fix_00420420_crash_20261121/arm.py inner   (called by the solo wrapper)
#        py -3.12 verify/fix_00420420_crash_20261121/arm.py                         (takes the solo window itself)
import os, subprocess, sys, time
from pathlib import Path

WT = Path(__file__).resolve().parent.parent.parent
LOCK = str(WT / "re/orchestrator/mashed_lock.py")
D = Path("D:/MashedWT/FIX-00420420-CRASH_data")
ROOT = r"D:\MashedWT\PROV-INSITU-REF1_root"
BODIES = "D:/MashedWT/PROV-INSITU-REF1_data/bodies309.json"
RAW = ("0x00603400-0x00603800,0x00614400-0x00614800,0x00618000-0x00618400,0x006dcc00-0x006dd400,"
       "0x00773800-0x00773c00,0x007dd000-0x007dd400,0x007f0c00-0x007f1400")


def arm(L, extra):
    out = D / L
    if out.exists():
        import shutil; shutil.rmtree(out)
    cmd = ["py", "-3.12", LOCK, "run", "--label", "FIX-00420420-CRASH", "--",
           "py", "-3.12", "re/frida/detorig.py", "--label", L, "--out", out.as_posix(), "--clock", "tick", "--boot",
           "--q0", "0x40000000", "--tick-raw", RAW, "--timeout", "600", "--tick-cap", "600",
           "--warp-tick", "300", "--warp-clock", "300", "--normalize", "re/frida/warp_normalize.tsv",
           "--load-sleep-ms", "0", "--boot-sleep-ms", "0", "--call-log-boot", "--call-log", "--read-wait", "--read-wait-rtfs",
           "--ckpt-ticks", "0,1,17,18,299,301,600", *extra]
    with open(D / f"{L}.log", "w") as f:
        rc = subprocess.call(cmd, cwd=WT, env=dict(os.environ, MASHED_ROOT=ROOT), stdout=f, stderr=subprocess.STDOUT)
    print(f"[{L}] exit {rc} {time.strftime('%H:%M:%S')}", flush=True)
    for line in (D / f"{L}.log").read_text(errors="replace").splitlines():
        if any(k in line for k in ("bootlen", "prov", "warp at", "ticks ", "=== rc", "exited", "rror", "CRASH", "mashed-lock")):
            print("   ", line[:300], flush=True)
    return rc


if __name__ == "__main__":
    D.mkdir(parents=True, exist_ok=True)
    if sys.argv[1:] == ["inner"]:
        procs = subprocess.run(["powershell", "-NoProfile", "-Command",
                                "Get-CimInstance Win32_Process | ? { $_.Name -match 'MASHED|mashed_re' } | % { \"$($_.ProcessId) $($_.CommandLine)\" }"],
                               capture_output=True, text=True).stdout.strip()
        print("live Mashed processes at solo start:", procs or "none", flush=True)
        if os.environ.get("ARM_SET") == "fix":   # second attempt: the fixed source (bodies_fix.json from provlink.py)
            for L in ("f1", "f2"):
                arm(L, ["--prov", str(D / "bodies_fix.json"), "--prov-rvas", "0x00420420", "--crash-catch", "20"])
        else:
            arm("c0", [])
            arm("b1", ["--prov", BODIES, "--prov-rvas", "0x00420420", "--crash-catch", "20"])
    else:
        print(subprocess.run(["py", "-3.12", LOCK, "status"], capture_output=True, text=True).stdout, flush=True)
        sys.exit(subprocess.call(["py", "-3.12", LOCK, "run", "--solo", "--ttl", "3600", "--label", "FIX-00420420-CRASH", "--",
                                  "py", "-3.12", str(Path(__file__).resolve()), "inner"], cwd=WT))
