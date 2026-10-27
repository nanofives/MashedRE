r"""D4-FU-SETTRACKPU arm driver (PREREG_D4-FU-SETTRACKPU.md), run_span.py style. Run under the machine lock, sandbox off:

    py -3.12 <orch-wt>/re/orchestrator/mashed_lock.py run --label D4-FU-SETTRACKPU -- py -3.12 run_stp.py <arm>...

Arms: N (new exe save round trip: none, F_P, F_G, F_1, F_P with MASHED_SAVE_KEEP_IMAGE=0), CMP (the tables),
W (ORIGINAL first-run boot to mode select with the NAV_WATCH_WRITE hardware watch, then its own save),
GATES (verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26). Private root ROOT (contains
original\, ASI loader renamed to dinput8.dll.off). Kills only its own PIDs.
"""
import hashlib, os, shutil, subprocess, sys, time
from pathlib import Path

W = Path(__file__).resolve().parents[2]
HERE = Path(__file__).resolve().parent
STEP = "D4-FU-SETTRACKPU"
ROOT = Path(os.environ.get("STP_ROOT", f"D:/MashedWT/{STEP}_root"))
ORIG = ROOT / "original"
DATA = Path(os.environ.get("STP_DATA", f"D:/MashedWT/{STEP}_data"))
EXE = Path(os.environ.get("STP_EXE", W / "mashedmod/build/mashed_re.exe"))
RT = Path("D:/MashedWT/D4-WSF-SAVE-RT_data")
E24 = Path("D:/MashedWT/D4-WSF-SAVE-24EFC_data")
SPI = Path("D:/MashedWT/D4-WSF-SAVE-SPANINIT_data")
IN = {"P": Path("D:/MashedWT/savelink_data/arm_P.bin"),
      "G": Path("D:/MashedWT/savelink_data/ghost_g1/gamesave_captured.bin"),
      "1": E24 / "F_1.bin"}
WATCH = "0x008a95ac,0x007f105c,0x007f10a8,0x007f10f4"
SPAN, SPAN_END, CNT = 0x24A40, 0x24F60, 0x24EFC

sys.path.insert(0, str(HERE))
from predict import bucket  # noqa: E402


def sha(p, n=16):
    return hashlib.sha256(Path(p).read_bytes()).hexdigest()[:n] if p and Path(p).exists() else "absent"


def say(*a):
    line = " ".join(str(x) for x in a)
    print(line, flush=True)
    with open(DATA / "run_stp.log", "a", encoding="utf-8") as f:
        f.write(time.strftime("%H:%M:%S ") + line + "\n")


def rt(tag, src, extra=None):
    scratch = DATA / f"{tag}_scratch"
    scratch.mkdir(parents=True, exist_ok=True)
    out = DATA / f"{tag}.bin"
    if out.exists():
        out.unlink()
    env = {k: v for k, v in os.environ.items() if not k.startswith("MASHED_")}
    env.update(MASHED_SAVE_RT_IN=str(src) if src else "none", MASHED_SAVE_RT_OUT=str(out),
               MASHED_SAVE_PATH=str(scratch / "rt_gamesave.bin"), MASHED_ROOT=str(ROOT))
    env.update(extra or {})
    p = subprocess.Popen([str(EXE)], cwd=str(scratch), env=env)
    try:
        rc = p.wait(timeout=60)
    except subprocess.TimeoutExpired:
        p.kill(); rc = "TIMEOUT(killed own pid %d)" % p.pid
    say(f"{tag}: exe {sha(EXE, 8)} in {sha(src)} rc={rc} out {sha(out)}")


def cmp(a, b, label):
    if not (Path(a).exists() and Path(b).exists()):
        say(f"  {label}: missing file"); return None
    x, y = Path(a).read_bytes(), Path(b).read_bytes()
    d = [i for i in range(min(len(x), len(y))) if x[i] != y[i]]
    bk = {}
    for i in d:
        bk[bucket(i)] = bk.get(bucket(i), 0) + 1
    say(f"  {label}: len_eq={len(x) == len(y)} differ={len(d)} {bk}")
    return d


def arm_CMP():
    O0 = RT / "O_0.bin"
    pred = DATA / "predicted_first_run.bin"
    p = subprocess.run([sys.executable, str(HERE / "predict.py"), str(O0), str(ROOT / "extracted/POWERUPS_ALL.LUA"),
                        str(pred)], capture_output=True, text=True, check=True)
    say("CMP static model:\n" + p.stdout.strip())
    say("CMP Q2 first run")
    cmp(DATA / "N_0.bin", pred, "N_0 vs predicted")
    cmp(O0, DATA / "N_0.bin", "O_0 vs N_0 (after)")
    cmp(O0, SPI / "N_0.bin", "O_0 vs SPANINIT N_0 (before)")
    if (DATA / "O_W.bin").exists():
        cmp(O0, DATA / "O_W.bin", "O_0 vs O_W (this step's original first run)")
    say("CMP Q3 keep path (output vs input)")
    for k in ("P", "G", "1"):
        cmp(IN[k], DATA / f"N_{k}.bin", f"F_{k} vs N_{k}")
    say("CMP Q4 KEEP_IMAGE=0 rebuild over F_P")
    d = cmp(SPI / "N_P0.bin", DATA / "N_P0.bin", "SPANINIT N_P0 vs N_P0")
    if d is not None:
        say(f"    outside 0x24cb0..0x24db3: {len([i for i in d if not (0x24CB0 <= i < 0x24DB4)])}")
    cmp(IN["P"], DATA / "N_P0.bin", "F_P vs N_P0")


def arm_W():
    for n in ["gamesave.bin", "videocfg.bin"] + sorted(x.name for x in ORIG.glob("contcfg*.bin")):
        if (ORIG / n).exists():
            (ORIG / n).unlink()
    say("W: before boot gamesave", sha(ORIG / "gamesave.bin"), "videocfg", sha(ORIG / "videocfg.bin"))
    env = dict(os.environ, MASHED_ORIG_DIR=str(ORIG), NAV_KEEP_VIDEOCFG="1", NAV_WATCH_WRITE=WATCH)
    p = subprocess.run([sys.executable, str(W / "re/frida/nav_champ_probe.py"), "--plan", "savewrite"],
                       env=env, capture_output=True, text=True, timeout=300)
    (DATA / "W_probe.log").write_text(p.stdout + "\n--stderr--\n" + p.stderr, encoding="utf-8")
    say(f"W: probe rc={p.returncode}")
    for l in p.stdout.splitlines():
        if "WATCH" in l or "SAVEWRITE" in l or "pid=" in l:
            say("   " + l.strip())
    if (ORIG / "gamesave.bin").exists():
        shutil.copy2(ORIG / "gamesave.bin", DATA / "O_W.bin")
    say("W: after gamesave", sha(DATA / "O_W.bin"))


def arm_GATES():
    out = DATA / "arms"
    out.mkdir(parents=True, exist_ok=True)
    env = dict(os.environ, A4_OUT=str(out), A4_ROOT=str(ROOT), A4_EXE=str(EXE))
    p = subprocess.run([sys.executable, str(W / "verify/savelink_20261023/run_arms.py"),
                        "H_DEF", "B_DEF_K0", "B_DEF_K1", "B_DEF_K26"], env=env, capture_output=True, text=True)
    (DATA / "gates.log").write_text(p.stdout + "\n--stderr--\n" + p.stderr, encoding="utf-8")
    for f in ("H_DEF.step.csv", "H_DEF.gates.csv", "H_DEF.ptrace.log", "B_DEF_K0.step.csv", "B_DEF_K1.step.csv",
              "B_DEF_K26.step.csv"):
        say(f"GATES {f} sha8={sha(out / f, 8)}")


def main():
    DATA.mkdir(parents=True, exist_ok=True)
    for a in sys.argv[1:]:
        if a == "N":
            rt("N_0", None)
            for k in ("P", "G", "1"):
                rt(f"N_{k}", IN[k])
            rt("N_P0", IN["P"], {"MASHED_SAVE_KEEP_IMAGE": "0"})
        elif a == "CMP":
            arm_CMP()
        elif a == "W":
            arm_W()
        elif a == "GATES":
            arm_GATES()
        else:
            raise SystemExit("unknown arm " + a)
    return 0


if __name__ == "__main__":
    sys.exit(main())
