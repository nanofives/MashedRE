r"""D4-WSF-SAVE-SPANINIT arm driver (PREREG_D4-WSF-SAVE-SPANINIT.md), run_24efc.py style. Run under the machine lock:

    py -3.12 <orch-wt>/re/orchestrator/mashed_lock.py run --label D4-WSF-SAVE-SPANINIT -- py -3.12 run_span.py N CMP

Arms: N (new exe: none, F_P, F_G, F_1, and F_P with MASHED_SAVE_KEEP_IMAGE=0), CMP (the table). The save round-trip
harness (MASHED_SAVE_RT_IN) exits before any asset load, so no game root is needed. Kills only its own PIDs.
"""
import hashlib, os, subprocess, sys, time
from pathlib import Path

W = Path(__file__).resolve().parents[2]
HERE = Path(__file__).resolve().parent
DATA = Path(os.environ.get("WSF_DATA", "D:/MashedWT/D4-WSF-SAVE-SPANINIT_data"))
EXE = Path(os.environ.get("WSF_EXE", W / "mashedmod/build/mashed_re.exe"))
RT = Path("D:/MashedWT/D4-WSF-SAVE-RT_data")
E24 = Path("D:/MashedWT/D4-WSF-SAVE-24EFC_data")
IN = {"P": Path("D:/MashedWT/savelink_data/arm_P.bin"),
      "G": Path("D:/MashedWT/savelink_data/ghost_g1/gamesave_captured.bin"),
      "1": E24 / "F_1.bin", "0": None}
SPAN, SPAN_END, CNT = 0x24A40, 0x24F60, 0x24EFC


def sha(p, n=16):
    return hashlib.sha256(Path(p).read_bytes()).hexdigest()[:n] if p and Path(p).exists() else "absent"


def say(*a):
    line = " ".join(str(x) for x in a)
    print(line, flush=True)
    with open(DATA / "run_span.log", "a", encoding="utf-8") as f:
        f.write(time.strftime("%H:%M:%S ") + line + "\n")


def rt(tag, src, extra=None):
    scratch = DATA / f"{tag}_scratch"
    scratch.mkdir(parents=True, exist_ok=True)
    out = DATA / f"{tag}.bin"
    if out.exists():
        out.unlink()
    env = {k: v for k, v in os.environ.items() if not k.startswith("MASHED_")}
    env.update(MASHED_SAVE_RT_IN=str(src) if src else "none", MASHED_SAVE_RT_OUT=str(out),
               MASHED_SAVE_PATH=str(scratch / "rt_gamesave.bin"))
    env.update(extra or {})
    p = subprocess.Popen([str(EXE)], cwd=str(scratch), env=env)
    try:
        rc = p.wait(timeout=60)
    except subprocess.TimeoutExpired:
        p.kill(); rc = "TIMEOUT(killed own pid %d)" % p.pid
    say(f"{tag}: exe {sha(EXE, 8)} in {sha(src)} rc={rc} out {sha(out)}")


def region(i):
    return ('magic' if i < 4 else 'block' if i < SPAN else 'table' if i < 0x24D10 else
            'state' if CNT <= i < CNT + 4 else 'gather' if 0x24F54 <= i < 0x24F60 else
            'span_rest' if i < SPAN_END else 'tail')


def diff(a, b):
    x, y = Path(a).read_bytes(), Path(b).read_bytes()
    d = [i for i in range(min(len(x), len(y))) if x[i] != y[i]]
    reg = {}
    for i in d:
        reg[region(i)] = reg.get(region(i), 0) + 1
    return len(x) == len(y), d, reg


def bucket(i):
    if 0x24CB0 <= i < 0x24DB4: return "set_track_powerups_rows"
    if i >= SPAN and i < SPAN + 13 * 0x30 and (i - SPAN) % 0x30 in range(0x10, 0x14): return "col4_unlock"
    if 0x24F2C <= i < 0x24F30: return "gate_4ec"
    if 0x24F54 <= i < 0x24F60: return "gather"
    if CNT <= i < CNT + 4: return "state_24efc"
    return "other"


def cmp(a, b, label, buckets=False):
    if not (Path(a).exists() and Path(b).exists()):
        say(f"  {label}: missing file"); return
    same, d, reg = diff(a, b)
    say(f"  {label}: len_eq={same} differ={len(d)} {reg}")
    if buckets:
        bk = {}
        for i in d:
            bk[bucket(i)] = bk.get(bucket(i), 0) + 1
        say(f"    buckets {bk}  other_offsets={[hex(i) for i in d if bucket(i) == 'other'][:16]}")
    return d


def arm_CMP():
    O0 = RT / "O_0.bin"
    pred = DATA / "predicted_first_run.bin"
    subprocess.run([sys.executable, str(HERE / "predict.py"), str(O0), str(E24 / "SF_0.bin"), str(pred)], check=True)
    say("CMP Q1/Q2 first run")
    cmp(DATA / "N_0.bin", pred, "N_0 vs predicted")
    cmp(O0, DATA / "N_0.bin", "O_0 vs N_0 (after)", buckets=True)
    cmp(O0, E24 / "SF_0.bin", "O_0 vs SF_0 (before, 24EFC exe)", buckets=True)
    cmp(O0, RT / "S1_0.bin", "O_0 vs S1_0 (before, RT exe)", buckets=True)
    say("CMP Q3 keep path (output vs input)")
    for k in ("P", "G", "1"):
        cmp(IN[k], DATA / f"N_{k}.bin", f"F_{k} vs N_{k}")
    say("CMP Q4 KEEP_IMAGE=0 rebuild over F_P")
    d = cmp(E24 / "SF_P.bin", DATA / "N_P0.bin", "SF_P (24EFC exe) vs N_P0")
    if d is not None:
        say(f"    outside span: {len([i for i in d if not (SPAN <= i < SPAN_END)])}")
    cmp(IN["P"], DATA / "N_P0.bin", "F_P vs N_P0")


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
        else:
            raise SystemExit("unknown arm " + a)
    return 0


if __name__ == "__main__":
    sys.exit(main())
