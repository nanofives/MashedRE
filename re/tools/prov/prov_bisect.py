"""PROV-INSITU group test (2026-11-11): run many DDK bodies in one replay, split the set that diverges until each wrong
body is alone. Run under test-slot; each game run takes the machine game lock by itself (mashed_lock.py run), so
other sessions can interleave between runs.

    py -3.12 re/tools/prov/prov_bisect.py --bodies verify/provinsitu_20261111/bodies_95.json --census D:/.../census \
        --ref D:/.../ref_a --ref2 D:/.../ref_b --tag t95 [--rvas a,b,c] [--max-runs 60]

Only bodies the census saw execute (calls_total > 0) are tested. A body whose group replays identically gets
IDENTICAL-FOR-N; a body alone in a diverging run gets DIVERGED with the first tick and blocks; a body alone in a run that
ends without ticks gets CRASH. State is appended to <data>/<tag>_state.jsonl so a stopped driver resumes.
"""
import argparse, json, pathlib, subprocess, sys, time

HERE = pathlib.Path(__file__).resolve().parent
ROOT = HERE.parents[2]
sys.path.insert(0, str(ROOT / "re" / "frida"))
from prov_compare import verdict  # noqa: E402

DATA = pathlib.Path("D:/MashedWT/PROV-INSITU_data")
STAMP = time.strftime("%H%M%S")
ARM = ROOT / "verify" / "provinsitu_20261111" / "run_arm.sh"
LOCK = pathlib.Path(r"C:\Users\maria\Desktop\Proyectos\Mashed\re\orchestrator\mashed_lock.py")
ARM_LOCKS = False


def run_arm(label, bodies, rvas, extra=()):
    # RO-MENU-REPLAY: --arm / --data / --arm-locks; an arm script that takes the game lock itself runs without LOCK
    pre = [] if ARM_LOCKS else ["py", "-3.12", str(LOCK), "run", "--label", "PROV-INSITU", "--"]
    cmd = pre + ["bash", str(ARM).replace("\\", "/"), label,
           "--prov", str(bodies).replace("\\", "/"), "--prov-rvas", ",".join(rvas), *extra]
    t = time.time()
    r = subprocess.run(cmd, capture_output=True, text=True)
    print(f"  run {label} ({len(rvas)} bodies) {time.time() - t:.0f}s", flush=True)
    print("   " + "\n   ".join(l for l in (r.stdout + r.stderr).splitlines() if "prov" in l or "rc=" in l or "error" in l)[:600], flush=True)
    return DATA / label


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--bodies", required=True)
    ap.add_argument("--census", required=True)
    ap.add_argument("--ref", required=True)
    ap.add_argument("--ref2", required=True)
    ap.add_argument("--tag", required=True)
    ap.add_argument("--rvas", default="")
    ap.add_argument("--max-runs", type=int, default=60)
    ap.add_argument("--pair", action="append", default=[], help="A,B: another reference pair (prov_compare --pair)")
    ap.add_argument("--group", type=int, default=0, help="start from chunks of this size (expected defect rate ~ 1/size)")
    ap.add_argument("--split-first", action="store_true", help="the whole set is known to diverge: start from its halves")
    ap.add_argument("--data", default="", help="RO-MENU-REPLAY: data folder (default the PROV-INSITU one)")
    ap.add_argument("--arm", default="", help="RO-MENU-REPLAY: arm script (default verify/provinsitu_20261111/run_arm.sh)")
    ap.add_argument("--arm-locks", action="store_true", help="the arm script takes the game lock itself")
    ap.add_argument("--any-segment", action="store_true",
                    help="RO-MENU-REPLAY boot mode: test every body the census saw run in any segment (boot, menu, load, race)")
    a = ap.parse_args()
    global DATA, ARM, ARM_LOCKS
    if a.data:
        DATA = pathlib.Path(a.data)
    if a.arm:
        ARM = pathlib.Path(a.arm).resolve()
    ARM_LOCKS = a.arm_locks
    bodies = {b["rva"].lower(): b for b in json.loads(pathlib.Path(a.bodies).read_text())["bodies"] if b["status"] == "OK"}
    cen = {r["rva"].lower(): r for r in json.loads((pathlib.Path(a.census) / "prov.json").read_text())["rows"]}
    want = [x.lower() for x in a.rvas.split(",") if x] or list(bodies)
    race = lambda c: (c.get("calls_total") or 0) - (c.get("calls_before_warp") or 0)   # calls from the warp on
    ran = (lambda c: (c.get("calls_total") or 0) > 0) if a.any_segment else (lambda c: race(c) > 0)
    todo = [r for r in want if r in bodies and ran(cen.get(r, {}))]
    print(f"{len(want)} bodies, {len(todo)} executed in the census", flush=True)
    state_f = DATA / f"{a.tag}_state.jsonl"
    done = {}
    if state_f.exists():
        for ln in state_f.read_text().splitlines():
            x = json.loads(ln)
            done[x["rva"]] = x
    first = [r for r in todo if r not in done]
    if a.group:
        stack = [first[i:i + a.group] for i in range(0, len(first), a.group)][::-1]
    else:
        stack = [first[len(first) // 2:], first[:len(first) // 2]] if a.split_first else [first]
    nrun = 0
    while stack and nrun < a.max_runs:
        grp = stack.pop()
        if not grp:
            continue
        nrun += 1
        label = f"{a.tag}_{STAMP}_{nrun:03d}_{len(grp)}"
        for attempt in range(3):
            d = run_arm(label if not attempt else f"{label}r{attempt}", a.bodies, grp)
            log = (DATA / f"{d.name}.log")
            # a Frida attach failure (detorig rc 3, "attach failed") is the harness, not the body: run it again
            if log.exists() and "attach failed" in log.read_text(errors="replace"):
                print(f"  attach failed, retry {attempt + 1}", flush=True)
                continue
            break
        label = d.name
        try:
            v = verdict(d, a.ref, a.ref2, [p.split(",") for p in a.pair])
        except Exception as e:
            print(f"  verdict failed: {e}", flush=True)
            v = {"verdict": "NO-TICKS", "identical_for": 0, "ticks": 0, "first": None, "rc": None}
        (DATA / f"{label}.verdict.json").write_text(json.dumps(v, indent=1))
        try:
            pv = json.loads((d / "prov.json").read_text())
            calls = {r["rva"].lower(): r for r in pv["rows"]}
            inst = {x["rva"].lower(): x for x in pv["installed"]}
        except Exception:
            calls, inst = {}, {}
        print(f"  -> {v['verdict']} {v['identical_for']} / {v['ticks']}", flush=True)
        if v["verdict"] == "IDENTICAL-FOR-N" or len(grp) == 1:
            with open(state_f, "a") as f:
                for r in grp:
                    kind = v["verdict"] if len(grp) == 1 or v["verdict"] == "IDENTICAL-FOR-N" else "?"
                    if kind in ("NO-TICKS", "INCOMPLETE"):
                        kind = "CRASH"
                    c = calls.get(r, {})
                    rec = {"rva": r, "verdict": kind, "n": v["identical_for"], "ticks": v["ticks"], "run": label,
                           "group": len(grp), "calls_total": c.get("calls_total"), "calls_to_anchor": c.get("calls_to_anchor"),
                           "calls_before_warp": c.get("calls_before_warp"), "calls_race": race(c) if c else None,
                           "notes": v.get("notes"),
                           "jmp_ok": inst.get(r, {}).get("jmp_ok"), "site_before": inst.get(r, {}).get("site_before"),
                           "first": v.get("first"), "rc": v.get("rc"),
                           "identical_by_segment": v.get("identical_by_segment"), "segments_match": v.get("segments_match")}
                    f.write(json.dumps(rec) + "\n")
                    done[r] = rec
            continue
        h = len(grp) // 2
        stack.append(grp[h:])
        stack.append(grp[:h])
    left = sum(len(g) for g in stack)
    print(f"runs {nrun}, verdicts {len(done)}, left {left}", flush=True)


if __name__ == "__main__":
    main()
