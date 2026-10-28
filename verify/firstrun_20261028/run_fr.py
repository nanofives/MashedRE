r"""D5-FIRSTRUN standalone first-run arms. Run under the machine lock, sandbox off:

    py -3.12 re/orchestrator/mashed_lock.py run --label D5-FIRSTRUN -- py -3.12 verify/firstrun_20261028/run_fr.py FR1 FR2

ROOT is a private robocopy of original\ (ROOT\original\...). The exe's cwd is ROOT. Only files inside ROOT are deleted.
FR1 / FR1b: no videocfg.bin (ROOT and ROOT\original) and no contcfg*.bin anywhere under ROOT, default env, 20 s boot.
FR1G / FR1Gb: the same with the gate recipe's base env (run_arms.py base_env, 1200 frames).
FR2: second boot with the last written videocfg.bin in place (must not be rewritten). Kills only its own PIDs.
"""
import hashlib, os, shutil, subprocess, sys, time
from pathlib import Path

W = Path(__file__).resolve().parents[2]
ROOT = Path(os.environ.get("FR_ROOT", "D:/MashedWT/D5-FIRSTRUN_root"))
DATA = Path(os.environ.get("FR_DATA", "D:/MashedWT/D5-FIRSTRUN_data"))
EXE = Path(os.environ.get("FR_EXE", W / "mashedmod/build/mashed_re.exe"))
REF = Path("D:/MashedWT/D4-WSF-VIDEOCFG_data/O_V.bin")
CFG = ROOT / "videocfg.bin"


def sha(p):
    return hashlib.sha256(Path(p).read_bytes()).hexdigest() if Path(p).exists() else "absent"


def say(*a):
    line = " ".join(str(x) for x in a)
    print(line, flush=True)
    with open(DATA / "run_fr.log", "a", encoding="utf-8") as f:
        f.write(time.strftime("%H:%M:%S ") + line + "\n")


def contcfgs():
    return sorted(str(p.relative_to(ROOT)) for p in ROOT.rglob("contcfg*.bin"))


def vlog_lines():
    p = ROOT / "log/mashed_re.log"
    if not p.exists():
        return []
    return [ln.rstrip() for ln in p.read_text(errors="replace").splitlines() if ln.startswith("[videocfg]")]


GATE_ENV = dict(MASHED_MUTE='1', MASHED_TRACK_VIEW='Training', MASHED_CAR='1', MASHED_ROUND='1', MASHED_ROUND_RULE='0',
                MASHED_GAME_MODE='10', MASHED_WIN_POS='primary-bl', MASHED_DETERMINISTIC='1', MASHED_DET_FRAMES='1200')


def boot(tag, secs=20, extra=None):
    env = {k: v for k, v in os.environ.items() if not k.startswith("MASHED_")}
    env["MASHED_ROOT"] = str(ROOT)
    env.update(extra or {})
    p = subprocess.Popen([str(EXE)], cwd=str(ROOT), env=env)
    say(f"{tag}: standalone pid={p.pid} exe {sha(EXE)[:8]} env {sorted(extra) if extra else 'default, no MASHED_* set'}"
        f" __COMPAT_LAYER={env.get('__COMPAT_LAYER', 'unset')}")
    time.sleep(secs)
    alive = p.poll() is None
    code = p.returncode
    if alive:
        p.kill()
    p.wait()
    say(f"{tag}: alive at {secs} s {alive} (exit {code})")
    return alive


def fields(b):
    i32 = lambda o: int.from_bytes(b[o:o + 4], "little")
    cstr = lambda o: b[o:b.index(0, o)].decode("latin-1")
    return (f"+0x0 {i32(0)} name '{cstr(4)}' label '{cstr(0x104)}' +0x184..+0x198 "
            f"{i32(0x184)}/{i32(0x188)}/{i32(0x18c)}/{i32(0x190)}/{i32(0x194)}/{i32(0x198)}")


def arm_FR1(tag="FR1", extra=None):
    for p in (CFG, ROOT / "original/videocfg.bin"):
        if p.exists():
            say(f"{tag}: removing scratch {p} ({sha(p)[:8]})")
            p.unlink()
    for rel in contcfgs():
        say(f"{tag}: removing scratch {rel}")
        (ROOT / rel).unlink()
    (ROOT / "log").mkdir(exist_ok=True)
    n0 = len(vlog_lines())
    say(f"{tag}: before: videocfg.bin {sha(CFG)}, original/videocfg.bin {sha(ROOT / 'original/videocfg.bin')}, "
        f"contcfg {contcfgs()}")
    alive = boot(tag, extra=extra)
    say(f"{tag}: after: videocfg.bin {sha(CFG)}, contcfg {contcfgs()}, log {vlog_lines()[n0:]}")
    if CFG.exists():
        b = CFG.read_bytes()
        shutil.copy2(CFG, DATA / f"{tag}_videocfg.bin")
        say(f"{tag}: written {len(b)} bytes: {fields(b)}")
        if REF.exists():
            r = REF.read_bytes()
            d = [i for i in range(min(len(b), len(r))) if b[i] != r[i]]
            say(f"{tag}: vs wsfvcfg O_V.bin ({sha(REF)[:8]}): {len(d)} bytes differ at {[hex(i) for i in d]}; "
                f"O_V fields: {fields(r)}")
    say(f"{tag}: verdict alive={alive} videocfg_written={CFG.exists()} contcfg_written={bool(contcfgs())}")


def arm_FR1b():
    arm_FR1("FR1b")


def arm_FR1G():
    arm_FR1("FR1G", GATE_ENV)


def arm_FR1Gb():
    arm_FR1("FR1Gb", GATE_ENV)


def arm_FR2():
    before = sha(CFG)
    m0 = CFG.stat().st_mtime if CFG.exists() else None
    n0 = len(vlog_lines())
    alive = boot("FR2")
    after = sha(CFG)
    m1 = CFG.stat().st_mtime if CFG.exists() else None
    say(f"FR2: videocfg.bin {before[:8]} -> {after[:8]}, mtime same {m0 == m1}, contcfg {contcfgs()}, "
        f"log {vlog_lines()[n0:]}")
    say(f"FR2: verdict alive={alive} unchanged={before == after and m0 == m1} contcfg_written={bool(contcfgs())}")


if __name__ == "__main__":
    DATA.mkdir(parents=True, exist_ok=True)
    for a in sys.argv[1:]:
        globals()["arm_" + a]()
