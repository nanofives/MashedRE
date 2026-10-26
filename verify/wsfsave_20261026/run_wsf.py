r"""D4-WSF-SAVE-RT arm driver (PREREG_D4-WSF-SAVE-RT.md). Run under the machine lock, sandbox off:

    py -3.12 re/orchestrator/mashed_lock.py run --label D4-WSF-SAVE-RT -- py -3.12 verify/wsfsave_20261026/run_wsf.py <arm>...

Arms: W1 W2 S1 S2 S3. Private root ROOT (contains original\, ASI loader renamed away); data in DATA. Kills only its own PIDs.
"""
import hashlib, os, shutil, subprocess, sys, time
from pathlib import Path

W = Path(__file__).resolve().parents[2]
ROOT = Path(os.environ.get("WSF_ROOT", "D:/MashedWT/D4-WSF-SAVE-RT_root"))
ORIG = ROOT / "original"
DATA = Path(os.environ.get("WSF_DATA", "D:/MashedWT/D4-WSF-SAVE-RT_data"))
EXE = Path(os.environ.get("WSF_EXE", W / "mashedmod/build/mashed_re.exe"))
F_P = Path("D:/MashedWT/savelink_data/arm_P.bin")
F_G = Path("D:/MashedWT/savelink_data/ghost_g1/gamesave_captured.bin")
SHIPPED = ORIG / "gamesave.bin.shipped"
CMP = W / "re/tools/save_fullfile.py"


def sha(p):
    return hashlib.sha256(Path(p).read_bytes()).hexdigest()[:16] if Path(p).exists() else "absent"


def say(*a):
    line = " ".join(str(x) for x in a)
    print(line, flush=True)
    with open(DATA / "run_wsf.log", "a", encoding="utf-8") as f:
        f.write(time.strftime("%H:%M:%S ") + line + "\n")


def cfg_files():
    return sorted(p.name for p in ORIG.glob("contcfg*.bin")) + sorted(p.name for p in ROOT.glob("contcfg*.bin"))


def probe_savewrite(tag, extra_env=None):
    env = dict(os.environ, MASHED_ORIG_DIR=str(ORIG))
    env.update(extra_env or {})
    p = subprocess.run([sys.executable, str(W / "re/frida/nav_champ_probe.py"), "--plan", "savewrite"],
                       env=env, capture_output=True, text=True, timeout=300)
    (DATA / f"{tag}_probe.log").write_text(p.stdout + "\n--stderr--\n" + p.stderr, encoding="utf-8")
    say(f"{tag}: probe rc={p.returncode} savewrite line:",
        [l.strip() for l in p.stdout.splitlines() if "SAVEWRITE" in l or "pid=" in l])


def arm_W1():
    shutil.copy2(F_G, ORIG / "gamesave.bin")
    probe_savewrite("W1")
    shutil.copy2(ORIG / "gamesave.bin", DATA / "O_G.bin")
    say("W1: in", sha(F_G), "out", sha(DATA / "O_G.bin"))


def arm_W2():
    for n in ["gamesave.bin", "videocfg.bin"] + cfg_files():
        if (ORIG / n).exists():
            (ORIG / n).unlink()
    say("W2: before boot gamesave", sha(ORIG / "gamesave.bin"), "videocfg", sha(ORIG / "videocfg.bin"), "contcfg", cfg_files())
    probe_savewrite("W2", {"NAV_KEEP_VIDEOCFG": "1"})
    for n, dst in (("gamesave.bin", "O_0.bin"), ("videocfg.bin", "V_0.bin")):
        if (ORIG / n).exists():
            shutil.copy2(ORIG / n, DATA / dst)
    say("W2: after gamesave", sha(DATA / "O_0.bin"), (DATA / "O_0.bin").stat().st_size if (DATA / "O_0.bin").exists() else 0,
        "videocfg", sha(DATA / "V_0.bin"), (DATA / "V_0.bin").stat().st_size if (DATA / "V_0.bin").exists() else 0,
        "contcfg", cfg_files())


def standalone_rt(tag, src, keep):
    scratch = DATA / f"{tag}_scratch"
    scratch.mkdir(exist_ok=True)
    out = DATA / f"{tag}.bin"
    if out.exists():
        out.unlink()
    env = {k: v for k, v in os.environ.items() if not k.startswith("MASHED_")}
    env.update(MASHED_SAVE_RT_IN=str(src) if src else "none", MASHED_SAVE_RT_OUT=str(out),
               MASHED_SAVE_PATH=str(scratch / "rt_gamesave.bin"))
    if keep:
        env["MASHED_SAVE_KEEP_IMAGE"] = "1"
    p = subprocess.Popen([str(EXE)], cwd=str(scratch), env=env)
    try:
        rc = p.wait(timeout=60)
    except subprocess.TimeoutExpired:
        p.kill(); rc = "TIMEOUT(killed own pid %d)" % p.pid
    say(f"{tag}: exe {sha(EXE)[:8]} in {sha(src) if src else 'none'} rc={rc} out {sha(out)}")


def arm_S(keep):
    pre = "S2" if keep else "S1"
    standalone_rt(f"{pre}_P", F_P, keep)
    standalone_rt(f"{pre}_G", F_G, keep)
    standalone_rt(f"{pre}_0", None, keep)


def arm_S3():
    for n in ["videocfg.bin"] + cfg_files():
        for d in (ORIG, ROOT):
            if (d / n).exists():
                (d / n).unlink()
    before = {p: sha(p) for p in ROOT.rglob("*") if p.is_file() and "TOASTART" not in p.parts}
    t0 = time.time()
    env = {k: v for k, v in os.environ.items() if not k.startswith("MASHED_")}
    env["MASHED_ROOT"] = str(ROOT)
    p = subprocess.Popen([str(EXE)], cwd=str(ROOT), env=env)
    say(f"S3: standalone pid={p.pid} exe {sha(EXE)[:8]}")
    time.sleep(30)
    alive = p.poll() is None
    if alive:
        p.kill()
    p.wait()
    new = [str(q.relative_to(ROOT)) for q in ROOT.rglob("*") if q.is_file() and "TOASTART" not in q.parts
           and (q not in before or sha(q) != before[q])]
    say(f"S3: alive at 30 s {alive}; files created or modified: {sorted(new)}")
    say(f"S3: videocfg.bin root {sha(ROOT / 'videocfg.bin')} original {sha(ORIG / 'videocfg.bin')} contcfg {cfg_files()}")


def main():
    DATA.mkdir(parents=True, exist_ok=True)
    if not SHIPPED.exists():
        shutil.copy2(ORIG / "gamesave.bin", SHIPPED)
    for a in sys.argv[1:]:
        {"W1": arm_W1, "W2": arm_W2, "S1": lambda: arm_S(False), "S2": lambda: arm_S(True), "S3": arm_S3}[a]()
    shutil.copy2(SHIPPED, ORIG / "gamesave.bin")
    canon = W / "scripts/canonical/videocfg_windowed.bin"
    shutil.copy2(canon, ORIG / "videocfg.bin")
    say("restored private root: gamesave", sha(ORIG / "gamesave.bin"), "videocfg", sha(ORIG / "videocfg.bin"))
    return 0


if __name__ == "__main__":
    sys.exit(main())
