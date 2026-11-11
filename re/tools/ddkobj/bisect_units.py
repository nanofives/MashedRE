"""Bisect the DDKOBJ unit set for units whose substitution makes a boot fail (DDKOBJ-SPIKE).

    py -3.12 re/tools/ddkobj/bisect_units.py <all_build.json> <out_tsv> [--knob on|off] [--exclude rva,...]

Each probe builds a variant from a subset (ddkobj.py build --units-file, out dir build/ddkobj_bisect), boots it once
through knob_boot.py inside `mashed_lock.py run` and classifies: FAIL = exited with an exception code, OK = ran to its
own exit or to the timeout. Finds one culprit by halving, adds it to the exclusions, and repeats until the full set
minus exclusions boots OK. One TSV row per probe. Run under test-slot.
"""
import json
import pathlib
import re
import subprocess
import sys

W = pathlib.Path(__file__).resolve().parents[3]
ORCH = W.parent / "orch-integration" / "re" / "orchestrator" / "mashed_lock.py"
OUT = W / "mashedmod" / "build" / "ddkobj_bisect"
ROOT = "D:/MashedWT/DDKOBJ-SPIKE_root"
DATA = "D:/MashedWT/DDKOBJ-SPIKE_data/bisect"


def first_rva(u):
    return u["rvas"][0]


def wait_quiet():
    """WORKER_RULES: no builds or game runs while the merge train holds PRIORITY-HOLD or a SOLO window is open."""
    import time
    while True:
        s = subprocess.run(["py", "-3.12", str(ORCH), "status"], capture_output=True, text=True).stdout
        if "PRIORITY-HOLD" not in s and "SOLO" not in s:
            return
        print("[bisect] waiting: merge-train PRIORITY-HOLD / SOLO open", flush=True)
        time.sleep(60)


def probe(units, tag, knob, log):
    wait_quiet()
    f = OUT.parent / "ddkobj_bisect_units.txt"
    OUT.parent.mkdir(parents=True, exist_ok=True)
    f.write_text("\n".join(units) + "\n")
    b = subprocess.run(["py", "-3.12", str(W / "re/tools/ddkobj/ddkobj.py"), "build", "--units-file", str(f),
                        "--out", str(OUT), "--link"], capture_output=True, text=True)
    if "link rc=0" not in b.stdout:
        return "LINKFAIL"
    kv = ["MASHED_DET_FRAMES=3000"] + (["MASHED_ORIG_TOP=1"] if knob == "on" else [])
    env_exe = str(OUT / "mashed_re_ddk.exe")
    cmd = ["py", "-3.12", str(ORCH), "run", "--label", "DDKOBJ-SPIKE", "--",
           "py", "-3.12", str(W / "verify/porttop_20261108/knob_boot.py"), tag, ROOT, "40"] + kv
    import os
    env = dict(os.environ, KNOB_EXE=env_exe, PORTTOP_OUT=DATA)
    pathlib.Path(DATA).mkdir(parents=True, exist_ok=True)
    r = subprocess.run(cmd, capture_output=True, text=True, cwd=str(W), env=env)
    m = re.search(r"rc=(-?\d+) \((0x[0-9A-F]+)\)", r.stdout)
    code = m.group(2) if m else "?"
    res = "FAIL" if code.startswith("0xC") else "OK"
    log.write(f"{tag}\t{len(units)}\t{res}\t{code}\t{','.join(units[:6])}{'...' if len(units) > 6 else ''}\n")
    log.flush()
    print(f"[probe] {tag} n={len(units)} -> {res} {code}", flush=True)
    return res


def main():
    rep = json.loads(pathlib.Path(sys.argv[1]).read_text())
    tsv = open(sys.argv[2], "a")
    knob = sys.argv[sys.argv.index("--knob") + 1] if "--knob" in sys.argv else "off"
    excl = set()
    if "--exclude" in sys.argv:
        excl = {x for x in sys.argv[sys.argv.index("--exclude") + 1].split(",") if x}
    allu = [first_rva(u) for u in rep["units"] if u["status"] == "ok"]
    n = 0
    while True:
        cur = [u for u in allu if u not in excl]
        n += 1
        if probe(cur, f"full{n}_{knob}", knob, tsv) != "FAIL":
            break
        lo = cur
        k = 0
        while len(lo) > 1:
            half = lo[:len(lo) // 2]
            k += 1
            if probe(half, f"b{n}_{k}_{knob}", knob, tsv) == "FAIL":
                lo = half
            else:
                lo = lo[len(lo) // 2:]
        excl.add(lo[0])
        print(f"[bisect] culprit {lo[0]}  exclusions now {sorted(excl)}", flush=True)
        tsv.write(f"CULPRIT\t{lo[0]}\n")
        tsv.flush()
    print(f"[bisect] done. exclusions: {sorted(excl)}")


if __name__ == "__main__":
    main()
