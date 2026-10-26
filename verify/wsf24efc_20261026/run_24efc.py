r"""D4-WSF-SAVE-24EFC arm driver (PREREG_D4-WSF-SAVE-24EFC.md), verify/wsfsave_20261026/run_wsf.py style. Run under the
machine lock, sandbox off:

    py -3.12 <orch-wt>/re/orchestrator/mashed_lock.py run --label D4-WSF-SAVE-24EFC -- py -3.12 run_24efc.py <arm>...

Arms: W3 (original, F_1), SB (base exe, F_1), SF (fixed exe: F_P F_G F_1 none), SO (fixed, MASHED_SAVE_ORIGINAL=1, F_1),
CMP (the table). Private root ROOT (contains original\, ASI loader renamed away). Kills only its own PIDs.
"""
import hashlib, os, shutil, subprocess, sys, time
from pathlib import Path

W = Path(__file__).resolve().parents[2]
ROOT = Path(os.environ.get("WSF_ROOT", "D:/MashedWT/D4-WSF-SAVE-24EFC_root"))
ORIG = ROOT / "original"
DATA = Path(os.environ.get("WSF_DATA", "D:/MashedWT/D4-WSF-SAVE-24EFC_data"))
EXE = Path(os.environ.get("WSF_EXE", W / "mashedmod/build/mashed_re.exe"))
EXE_BASE = DATA / "mashed_re_base.exe"
RT = Path("D:/MashedWT/D4-WSF-SAVE-RT_data")
IN = {"P": Path("D:/MashedWT/savelink_data/arm_P.bin"),
      "G": Path("D:/MashedWT/savelink_data/ghost_g1/gamesave_captured.bin"),
      "1": DATA / "F_1.bin", "0": None}
SHIPPED = ORIG / "gamesave.bin.shipped"
OFF = 0x24EFC


def sha(p):
    return hashlib.sha256(Path(p).read_bytes()).hexdigest()[:16] if p and Path(p).exists() else "absent"


def say(*a):
    line = " ".join(str(x) for x in a)
    print(line, flush=True)
    with open(DATA / "run_24efc.log", "a", encoding="utf-8") as f:
        f.write(time.strftime("%H:%M:%S ") + line + "\n")


def arm_W3():
    shutil.copy2(IN["1"], ORIG / "gamesave.bin")
    env = dict(os.environ, MASHED_ORIG_DIR=str(ORIG))
    p = subprocess.run([sys.executable, str(W / "re/frida/nav_champ_probe.py"), "--plan", "savewrite"],
                       env=env, capture_output=True, text=True, timeout=300)
    (DATA / "W3_probe.log").write_text(p.stdout + "\n--stderr--\n" + p.stderr, encoding="utf-8")
    say("W3: probe rc=%s" % p.returncode, [l.strip() for l in p.stdout.splitlines() if "SAVEWRITE" in l or "pid=" in l])
    shutil.copy2(ORIG / "gamesave.bin", DATA / "O_1.bin")
    say("W3: in", sha(IN["1"]), "out", sha(DATA / "O_1.bin"))


def rt(tag, exe, src, extra=None):
    scratch = DATA / f"{tag}_scratch"
    scratch.mkdir(exist_ok=True)
    out = DATA / f"{tag}.bin"
    if out.exists():
        out.unlink()
    env = {k: v for k, v in os.environ.items() if not k.startswith("MASHED_")}
    env.update(MASHED_SAVE_RT_IN=str(src) if src else "none", MASHED_SAVE_RT_OUT=str(out),
               MASHED_SAVE_PATH=str(scratch / "rt_gamesave.bin"))
    env.update(extra or {})
    p = subprocess.Popen([str(exe)], cwd=str(scratch), env=env)
    try:
        rc = p.wait(timeout=60)
    except subprocess.TimeoutExpired:
        p.kill(); rc = "TIMEOUT(killed own pid %d)" % p.pid
    say(f"{tag}: exe {sha(exe)[:8]} in {sha(src)} rc={rc} out {sha(out)}")


def dword(p):
    b = Path(p).read_bytes()
    return int.from_bytes(b[OFF:OFF + 4], "little")


def diff(a, b):
    x, y = Path(a).read_bytes(), Path(b).read_bytes()
    d = [i for i in range(min(len(x), len(y))) if x[i] != y[i]]
    return len(x) == len(y), d


def arm_CMP():
    say("CMP: file | in 0x24efc | out 0x24efc")
    for tag, k in (("O_1", "1"), ("SB_1", "1"), ("SF_P", "P"), ("SF_G", "G"), ("SF_1", "1"), ("SF_0", "0"), ("SO_1", "1")):
        f = DATA / f"{tag}.bin"
        if f.exists():
            say(f"  {tag:5s} in {dword(IN[k]) if IN[k] else 'none'} out {dword(f)}")
    for a, b in ((DATA / "SF_P.bin", RT / "S1_P.bin"), (DATA / "SF_G.bin", RT / "S1_G.bin"),
                 (DATA / "SF_0.bin", RT / "S1_0.bin"), (DATA / "SF_1.bin", DATA / "SB_1.bin"),
                 (DATA / "SF_P.bin", DATA / "SF_1.bin"), (DATA / "SO_1.bin", DATA / "F_1.bin"),
                 (DATA / "O_1.bin", DATA / "F_1.bin"), (DATA / "SF_1.bin", DATA / "O_1.bin")):
        if a.exists() and b.exists():
            same_len, d = diff(a, b)
            outside = [i for i in d if not (OFF <= i < OFF + 4)]
            say(f"  {a.name} vs {b.name}: len_eq={same_len} differ={len(d)} in_dword={len(d) - len(outside)} "
                f"outside={len(outside)} first_outside={[hex(i) for i in outside[:8]]}")


def main():
    DATA.mkdir(parents=True, exist_ok=True)
    if not SHIPPED.exists():
        shutil.copy2(ORIG / "gamesave.bin", SHIPPED)
    for a in sys.argv[1:]:
        if a == "W3": arm_W3()
        elif a == "SB": rt("SB_1", EXE_BASE, IN["1"])
        elif a == "SF":
            for k in ("P", "G", "1", "0"):
                rt(f"SF_{k}", EXE, IN[k])
        elif a == "SO": rt("SO_1", EXE, IN["1"], {"MASHED_SAVE_ORIGINAL": "1"})
        elif a == "CMP": arm_CMP()
        else: raise SystemExit("unknown arm " + a)
    shutil.copy2(SHIPPED, ORIG / "gamesave.bin")
    shutil.copy2(W / "scripts/canonical/videocfg_windowed.bin", ORIG / "videocfg.bin")
    say("restored private root: gamesave", sha(ORIG / "gamesave.bin"), "videocfg", sha(ORIG / "videocfg.bin"))
    return 0


if __name__ == "__main__":
    sys.exit(main())
