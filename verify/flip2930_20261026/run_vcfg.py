r"""D4-FLIP-L29-L30 videocfg arms on the flipped default (copy of verify/wsfvcfg_20261026/run_vcfg.py: V1/V2 knob unset, V3 =0). Run under the machine lock, sandbox off:

    py -3.12 re/orchestrator/mashed_lock.py run --label D4-FLIP-L29-L30 -- py -3.12 verify/flip2930_20261026/run_vcfg.py V1 V2 V3

The exe's cwd is the private root (MASHED_ROOT), so videocfg.bin is ROOT/videocfg.bin. Kills only its own PIDs.
O1 is verify/wsfsave_20261026/run_wsf.py W2 with WSF_ROOT / WSF_DATA pointed at this step's root and data.
"""
import hashlib, os, shutil, subprocess, sys, time
from pathlib import Path

W = Path(__file__).resolve().parents[2]
ROOT = Path(os.environ.get("VCFG_ROOT", "D:/MashedWT/D4-FLIP-L29-L30_root"))
DATA = Path(os.environ.get("VCFG_DATA", "D:/MashedWT/D4-FLIP-L29-L30_data"))
EXE = Path(os.environ.get("VCFG_EXE", W / "mashedmod/build/mashed_re.exe"))
REF = Path("D:/MashedWT/D4-WSF-VIDEOCFG_data/O_V.bin")   # original first-run file, D4-WSF-VIDEOCFG O1
CANON = W / "scripts/canonical/videocfg_windowed.bin"
CFG = ROOT / "videocfg.bin"


def sha(p):
    return hashlib.sha256(Path(p).read_bytes()).hexdigest() if Path(p).exists() else "absent"


def say(*a):
    line = " ".join(str(x) for x in a)
    print(line, flush=True)
    with open(DATA / "run_vcfg.log", "a", encoding="utf-8") as f:
        f.write(time.strftime("%H:%M:%S ") + line + "\n")


def boot(tag, knob, secs=20):
    env = {k: v for k, v in os.environ.items() if not k.startswith("MASHED_")}
    env["MASHED_ROOT"] = str(ROOT)
    if knob is not None:
        env["MASHED_VIDEOCFG_WRITE"] = knob   # None = the default (ON since L-29 (a))
    p = subprocess.Popen([str(EXE)], cwd=str(ROOT), env=env)
    say(f"{tag}: standalone pid={p.pid} exe {sha(EXE)[:8]} knob={knob}")
    time.sleep(secs)
    alive = p.poll() is None
    if alive:
        p.kill()
    p.wait()
    say(f"{tag}: alive at {secs} s {alive}")


def diff(a, b):
    x, y = Path(a).read_bytes(), Path(b).read_bytes()
    n = sum(1 for i in range(min(len(x), len(y))) if x[i] != y[i]) + abs(len(x) - len(y))
    return n, len(x), len(y)


def arm_V1():
    if CFG.exists():
        CFG.unlink()
    boot("V1", None)
    if CFG.exists():
        shutil.copy2(CFG, DATA / "V1.bin")
        n, la, lb = diff(DATA / "V1.bin", REF)
        say(f"V1: videocfg.bin {sha(CFG)} size {la}; vs O_V.bin ({lb} bytes): {n} differ")
    else:
        say("V1: videocfg.bin absent")


def arm_V2():
    shutil.copy2(CANON, CFG)
    before = sha(CFG)
    boot("V2", None)
    say(f"V2: before {before} after {sha(CFG)} unchanged={before == sha(CFG)}")


def arm_V3():
    if CFG.exists():
        CFG.unlink()
    boot("V3", "0")
    say(f"V3: videocfg.bin {sha(CFG)}")


def main():
    DATA.mkdir(parents=True, exist_ok=True)
    (ROOT / "log").mkdir(exist_ok=True)
    for a in sys.argv[1:]:
        {"V1": arm_V1, "V2": arm_V2, "V3": arm_V3}[a]()
    if CFG.exists():
        CFG.unlink()
    return 0


if __name__ == "__main__":
    sys.exit(main())
