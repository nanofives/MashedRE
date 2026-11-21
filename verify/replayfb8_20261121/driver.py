# REPLAY-FALLBACK-8 / PROV-INSITU-REF1-NOSRC driver. Run under test-slot; run_arm.py takes the game lock per run.
#   py -3.12 verify/replayfb8_20261121/driver.py c0|census|verdict|ws|rep <rva,...>
# c0: no-body control. census: --prov-count over every rva of SOURCES.tsv. verdict: prov_bisect singles per step.
# ws: one --prov-wlog 10 run per listed rva (bodies of its step), then prov_writeset.py over those runs.
# rep: repeat single runs for DIVERGED / CRASH (ARM_EXTRA=--crash-catch 20 for the fault address).
import csv, subprocess, sys
from pathlib import Path
H = Path(__file__).resolve().parent
WT = H.parents[1]
D = Path("D:/MashedWT/REPLAY-FALLBACK-8_data")
R = "D:/MashedWT/RO-REF-NAME_data"
ARM = H / "run_arm.py"
S = list(csv.DictReader(open(H / "SOURCES.tsv"), delimiter="\t"))
step_of = {r["rva"].lower(): r["step"] for r in S}
cmd, args = sys.argv[1], sys.argv[2:]


def arm(label, *extra):
    print(f"== {label} {' '.join(extra)}", flush=True)
    return subprocess.call(["py", "-3.12", str(ARM), label, *extra], cwd=WT)


if cmd == "c0":
    arm("c0")
    subprocess.call(["py", "-3.12", "re/frida/prov_compare.py", str(D / "c0"), f"{R}/ref1", f"{R}/ref2"], cwd=WT)
elif cmd == "census":
    arm("census", "--prov-count", ",".join(step_of))
elif cmd == "verdict":
    for step in (args or ["fb8", "nosrc"]):
        rv = [r for r, s in step_of.items() if s == step]
        subprocess.call(["py", "-3.12", "re/tools/prov/prov_bisect.py", "--bodies", str(D / f"bodies_{step}.json"),
                         "--census", str(D / "census"), "--ref", f"{R}/ref1", "--ref2", f"{R}/ref2", "--tag", step,
                         "--rvas", ",".join(rv), "--group", "1", "--any-segment", "--data", str(D), "--arm", str(ARM),
                         "--arm-locks"], cwd=WT)
elif cmd == "ws":
    rvas = args[0].lower().split(",")
    for step in ("fb8", "nosrc"):
        mine = [r for r in rvas if step_of[r] == step]
        runs = []
        for r in mine:
            L = f"ws_{r[2:]}"
            if not (D / L / "prov_install.json").exists():
                arm(L, "--prov", str(D / f"bodies_{step}.json"), "--prov-rvas", r, "--prov-wlog", "10")
            if (D / L / "prov_install.json").exists():
                runs.append(str(D / L))
        if runs:
            subprocess.call(["py", "-3.12", "re/tools/prov/prov_writeset.py", "--bodies", str(D / f"bodies_{step}.json"),
                             *sum((["--run", x] for x in runs), []), "--allowed-ref", f"{R}/ref1,{R}/ref2",
                             "--rvas", ",".join(mine), "--out", str(H / f"WRITESET_{step}.tsv"),
                             "--stores-out", str(D / f"ws_stores_{step}.tsv")], cwd=WT)
elif cmd == "rep":
    for r in args[0].lower().split(","):
        L = f"rep_{r[2:]}"
        arm(L, "--prov", str(D / f"bodies_{step_of[r]}.json"), "--prov-rvas", r)
        subprocess.call(["py", "-3.12", "re/frida/prov_compare.py", str(D / L), f"{R}/ref1", f"{R}/ref2"], cwd=WT)
