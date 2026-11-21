# PROV-INSITU-REF1 dynamic write-set leg (method of PROV-WRITESET-114): IDENTICAL-FOR-600 bodies whose static leg is
# not-covered, in groups, the first N calls of each single-stepped (--prov-wlog N) on the ref1 recipe, then
# re/tools/prov/prov_writeset.py over the runs. Under test-slot; run_arm.py takes the game lock per run.
#   py -3.12 ws_driver.py [--group 20] [--n 10] [--only-table]
import argparse, csv, json, subprocess, sys
from pathlib import Path
H = Path(__file__).resolve().parent
WT = H.parents[1]
D = Path("D:/MashedWT/PROV-INSITU-REF1_data")
R = "D:/MashedWT/RO-REF-NAME_data"
ap = argparse.ArgumentParser()
ap.add_argument("--group", type=int, default=20)
ap.add_argument("--n", type=int, default=10)
ap.add_argument("--only-table", action="store_true")
a = ap.parse_args()
rows = [r for r in csv.DictReader(open(H / "VERDICTS.tsv"), delimiter="\t")
        if r["verdict"].startswith("IDENTICAL") and r["wleg_static"] == "not-covered"]
rvas = [r["rva"] for r in rows]
groups = [rvas[i:i + a.group] for i in range(0, len(rvas), a.group)]
print(f"{len(rvas)} bodies, {len(groups)} groups", flush=True)
runs = []
for i, g in enumerate(groups):
    L = f"ws_g{i:02d}"
    runs.append(D / L)
    if a.only_table or (D / L / "prov_install.json").exists():
        continue
    print(f"[{i + 1}/{len(groups)}] {L} {len(g)} bodies", flush=True)
    subprocess.call(["py", "-3.12", str(H / "run_arm.py"), L, "--prov", str(D / "bodies309.json"), "--prov-rvas", ",".join(g),
                     "--prov-wlog", str(a.n)], cwd=WT)
ok = [str(r) for r in runs if (r / "prov_install.json").exists()]
cmd = ["py", "-3.12", "re/tools/prov/prov_writeset.py", "--bodies", str(D / "bodies309.json"),
       *sum((["--run", r] for r in ok), []), "--allowed-ref", f"{R}/ref1,{R}/ref2", "--rvas", ",".join(rvas),
       "--out", str(H / "WRITESET_TABLE.tsv"), "--stores-out", str(D / "ws_stores.tsv")]
sys.exit(subprocess.call(cmd, cwd=WT))
