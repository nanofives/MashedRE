r"""D4-FU-SPANINIT-WRITERS arm driver (PREREG_D4-FU-SPANINIT-WRITERS.md), run_stp.py style. Run under the lock + test slot:

    py -3.12 <orch-wt>/re/orchestrator/mashed_lock.py run --label D4-FU-SPANINIT-WRITERS -- py -3.12 run_spw.py <arm>...

Arms: N (round trips: N_0 knob off, K_0 / K_P / K_G / K_1 / K_P0 with MASHED_SAVE_FIRSTRUN_WRITERS=1), S (boot with
MASHED_SPAN_BOOT_SEED=1 and an absent save, dump the seeded span), CMP (P1..P5), GATES (P6). Kills only its own PIDs.
"""
import hashlib, os, re, subprocess, sys, time
from pathlib import Path

W = Path(__file__).resolve().parents[2]
STEP = "D4-FU-SPANINIT-WRITERS"
ROOT = Path(os.environ.get("SPW_ROOT", f"D:/MashedWT/{STEP}_root"))
DATA = Path(os.environ.get("SPW_DATA", f"D:/MashedWT/{STEP}_data"))
EXE = Path(os.environ.get("SPW_EXE", W / "mashedmod/build/mashed_re.exe"))
RT = Path("D:/MashedWT/D4-WSF-SAVE-RT_data")
STP = Path("D:/MashedWT/D4-FU-SETTRACKPU_data")
IN = {"P": Path("D:/MashedWT/savelink_data/arm_P.bin"),
      "G": Path("D:/MashedWT/savelink_data/ghost_g1/gamesave_captured.bin"),
      "1": Path("D:/MashedWT/D4-WSF-SAVE-24EFC_data/F_1.bin")}
SPAN, SPAN_END, CNT, GATHER = 0x24A40, 0x24F60, 0x24EFC, 0x24F54
KNOB = {"MASHED_SAVE_FIRSTRUN_WRITERS": "1"}


def sha(p, n=16):
    return hashlib.sha256(Path(p).read_bytes()).hexdigest()[:n] if p and Path(p).exists() else "absent"


def say(*a):
    line = " ".join(str(x) for x in a)
    print(line, flush=True)
    with open(DATA / "run_spw.log", "a", encoding="utf-8") as f:
        f.write(time.strftime("%H:%M:%S ") + line + "\n")


def clean_env():
    return {k: v for k, v in os.environ.items() if not k.startswith("MASHED_")}


def run(tag, env, cwd, timeout):
    p = subprocess.Popen([str(EXE)], cwd=str(cwd), env=env)
    try:
        rc = p.wait(timeout=timeout)
    except subprocess.TimeoutExpired:
        p.kill(); rc = "TIMEOUT(killed own pid %d)" % p.pid
    return rc


def rt(tag, src, extra=None):
    scratch = DATA / f"{tag}_scratch"
    scratch.mkdir(parents=True, exist_ok=True)
    for f in ("mashed_re.log",):
        if (scratch / f).exists():
            (scratch / f).unlink()
    out = DATA / f"{tag}.bin"
    if out.exists():
        out.unlink()
    env = clean_env()
    env.update(MASHED_SAVE_RT_IN=str(src) if src else "none", MASHED_SAVE_RT_OUT=str(out),
               MASHED_SAVE_PATH=str(scratch / "rt_gamesave.bin"), MASHED_ROOT=str(ROOT))
    env.update(extra or {})
    rc = run(tag, env, scratch, 60)
    slots = ""
    if (scratch / "mashed_re.log").exists():
        m = re.findall(r"input slots:.*", (scratch / "mashed_re.log").read_text(errors="replace"))
        slots = m[-1] if m else ""
    say(f"{tag}: exe {sha(EXE, 8)} in {sha(src)} rc={rc} out {sha(out)} {slots}")


def arm_S():
    scratch = DATA / "S_scratch"
    scratch.mkdir(parents=True, exist_ok=True)
    dump = DATA / "S_span.bin"
    if dump.exists():
        dump.unlink()
    absent = scratch / "absent_gamesave.bin"
    if absent.exists():
        absent.unlink()
    log = ROOT / "mashed_re.log"
    off = log.stat().st_size if log.exists() else 0
    env = clean_env()
    env.update(MASHED_ROOT=str(ROOT), MASHED_SAVE=str(absent), MASHED_SPAN_BOOT_SEED="1",
               MASHED_SPAN_BOOT_SEED_DUMP=str(dump))
    rc = run("S", env, scratch, 120)
    new = ""
    for cand in (log, scratch / "mashed_re.log"):
        if cand.exists():
            b = cand.read_bytes()
            new += b[off if cand == log else 0:].decode("latin-1")
    lines = [l for l in new.splitlines() if "span boot seed" in l or "R2-2 gamesave load" in l]
    (DATA / "S_log_extract.txt").write_text("\n".join(lines) + "\n", encoding="utf-8")
    say(f"S: exe {sha(EXE, 8)} rc={rc} dump {sha(dump)} log: {lines}")


def diff(a, b, lo=0, hi=None):
    x, y = Path(a).read_bytes(), Path(b).read_bytes()
    hi = hi if hi is not None else min(len(x), len(y))
    return [i for i in range(lo, hi) if x[i] != y[i]], len(x) == len(y)


def slots_from_log():
    t = (DATA / "run_spw.log").read_text(encoding="utf-8")
    m = re.findall(r"K_0: .*input slots: game controllers (\d+), types (\d) (\d) (\d) (\d)", t)
    if not m:
        return None
    types = [int(v) for v in m[-1][1:]]
    return {i for i in range(4) if types[i] != 0}


def verdict(tag, ok, detail):
    say(f"{tag} {'HELD' if ok else 'FAILED'}: {detail}")
    return ok


def arm_CMP():
    O0 = RT / "O_0.bin"
    o = O0.read_bytes()
    A = slots_from_log()
    say(f"CMP: active slots A = {sorted(A) if A is not None else 'UNKNOWN (no K_0 input slots line)'}")
    res = []
    d, le = diff(DATA / "N_0.bin", STP / "N_0.bin")
    res.append(verdict("P1", le and not d, f"N_0 vs SETTRACKPU N_0: {len(d)} differ, sha {sha(DATA / 'N_0.bin', 8)}"))
    if A is not None:
        exp = {CNT} | {GATHER + i for i in A}
        d, le = diff(DATA / "K_0.bin", DATA / "N_0.bin")
        res.append(verdict("P2a", le and set(d) == exp, f"K_0 vs N_0 at {[hex(i) for i in d]}, expected {[hex(i) for i in sorted(exp)]}"))
        m = sum(1 for i in range(4) if (i in A) != (o[GATHER + i] == 0))
        d, le = diff(O0, DATA / "K_0.bin")
        tail = [hex(i) for i in d if i >= 0x24EF0]
        res.append(verdict("P2b", le and len(d) == 13 + m, f"O_0 vs K_0: {len(d)} differ (expected {13 + m}), at/after 0x24ef0: {tail}"))
    for k in ("P", "G", "1"):
        d, le = diff(IN[k], DATA / f"K_{k}.bin")
        res.append(verdict(f"P3_{k}", le and not d, f"F_{k} vs K_{k}: {len(d)} differ"))
    if A is not None:
        n0 = (STP / "N_P0.bin").read_bytes()
        exp = {GATHER + i for i in A if n0[GATHER + i] != 0}
        d, le = diff(STP / "N_P0.bin", DATA / "K_P0.bin")
        res.append(verdict("P4", le and set(d) == exp, f"SETTRACKPU N_P0 vs K_P0 at {[hex(i) for i in d]}, expected {[hex(i) for i in sorted(exp)]}"))
        d2, _ = diff(IN["P"], DATA / "K_P0.bin")
        say(f"   F_P vs K_P0 (report only): {len(d2)} differ at {[hex(i) for i in d2]}")
    s = DATA / "S_span.bin"
    if s.exists():
        sb = s.read_bytes()
        ospan = o[SPAN:SPAN_END]
        d = [i for i in range(min(len(sb), len(ospan))) if sb[i] != ospan[i]]
        exp = [0x4BC, 0x514, 0x515, 0x516]
        logok = "span boot seed" in (DATA / "S_log_extract.txt").read_text(encoding="utf-8")
        res.append(verdict("P5", len(sb) == 0x520 and d == exp and logok,
                           f"S dump vs O_0 span: {len(d)} differ at {[hex(i) for i in d]}, expected {[hex(i) for i in exp]}, log line {logok}"))
    else:
        res.append(verdict("P5", False, "no S dump"))
    say(f"CMP: {sum(res)} of {len(res)} held")


def arm_GATES():
    out = DATA / "arms"
    out.mkdir(parents=True, exist_ok=True)
    env = dict(clean_env(), A4_OUT=str(out), A4_ROOT=str(ROOT), A4_EXE=str(EXE))
    p = subprocess.run([sys.executable, str(W / "verify/savelink_20261023/run_arms.py"),
                        "H_DEF", "B_DEF_K0", "B_DEF_K1", "B_DEF_K26"], env=env, capture_output=True, text=True)
    (DATA / "gates.log").write_text(p.stdout + "\n--stderr--\n" + p.stderr, encoding="utf-8")
    say(f"GATES rc={p.returncode}")
    for f in ("H_DEF.step.csv", "H_DEF.gates.csv", "H_DEF.ptrace.log", "B_DEF_K0.step.csv", "B_DEF_K1.step.csv",
              "B_DEF_K26.step.csv"):
        say(f"GATES {f} sha8={sha(out / f, 8)}")


def main():
    DATA.mkdir(parents=True, exist_ok=True)
    for a in sys.argv[1:]:
        if a == "N":
            rt("N_0", None)
            rt("K_0", None, KNOB)
            for k in ("P", "G", "1"):
                rt(f"K_{k}", IN[k], KNOB)
            rt("K_P0", IN["P"], dict(KNOB, MASHED_SAVE_KEEP_IMAGE="0"))
        elif a == "S":
            arm_S()
        elif a == "CMP":
            arm_CMP()
        elif a == "GATES":
            arm_GATES()
        else:
            raise SystemExit("unknown arm " + a)
    return 0


if __name__ == "__main__":
    sys.exit(main())
