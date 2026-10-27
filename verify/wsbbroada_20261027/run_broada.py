"""D4-WSB-C4-BROAD-A driver (copy of verify/wsbbvhfu_20261027/run_bvh3.py; tool v8, paths, label, row list and C4_TAG changed): one c4_racediff_v6.py run (OFF boot + ON boot) per RVA, each under the
machine lock, so exactly one hook is installed per ON boot.

  py -3.12 verify/wsbbvh_20261027/run_bvh2.py <rva> [<rva> ...]     (or --all for the 22 rows)

Needs MASHED_ROOT (private root). Raw output per RVA: D:/MashedWT/D4-WSB-BVH-WRITESET_data/<rva>/.
Summary (committed): verify/wsbbvh_20261027/racediff_summary.tsv (C4_SCENARIO=census: rva key + "_census"), one row per RVA, last run wins.
A run where both sides captured nothing (no title / not in race / crash) is re-run once.
"""
import hashlib, json, os, subprocess, sys, time
from pathlib import Path

HERE = Path(__file__).resolve().parent
WT = HERE.parent.parent
LOCK = WT.parent / "orch-integration" / "re" / "orchestrator" / "mashed_lock.py"
TOOL = HERE / os.environ.get("C4_TOOL", "c4_racediff_v8.py")   # addendum 1: C4_TOOL=c4_racediff_v9.py
DATA = Path("D:/MashedWT/D4-WSB-C4-BROAD-A_data")
SUMMARY = HERE / "racediff_summary.tsv"
ALL = ["0055abb0", "0055bb70", "0055c230", "0055deb0", "0055e050"]
COLS = ["rva", "verdict", "why", "installed", "off_sigs", "on_sigs", "off_calls", "on_calls",
        "spawned_pids", "samples_sha256", "attempts", "utc"]


def sha(p):
    return hashlib.sha256(p.read_bytes()).hexdigest() if p.exists() else ""


def one(rva):
    sfx = ("_census" if os.environ.get("C4_SCENARIO") == "census" else "") + os.environ.get("C4_TAG", "")   # tag: arm variant
    out = DATA / (rva + sfx)
    out.mkdir(parents=True, exist_ok=True)
    env = dict(os.environ)
    env["C4_OUT"] = str(out)
    cmd = [sys.executable, str(LOCK), "run", "--label", "D4-WSB-C4-BROAD-A", "--",
           sys.executable, str(TOOL), "--rvas", "0x" + rva]
    if os.environ.get("C4_WINDOW"):   # "lo hi" in A4 calls; default 300 1200
        lo, hi = os.environ["C4_WINDOW"].split()
        cmd += ["--lo", lo, "--hi", hi]
    with open(out / "transcript.txt", "w", encoding="utf-8") as tf:
        p = subprocess.Popen(cmd, cwd=str(WT), env=env, stdout=subprocess.PIPE,
                             stderr=subprocess.STDOUT, text=True, encoding="utf-8", errors="replace")
        pids = []
        for line in p.stdout:
            tf.write(line)
            if "SPAWNED_PID=" in line:
                pids.append(line.strip().split("SPAWNED_PID=")[1])
            if "own asi ranges" not in line:
                print("    " + line.rstrip(), flush=True)
        p.wait()
    res = out / "c4_racediff_result.json"
    smp = out / "c4_racediff_samples.json"
    r = {}
    if res.exists():
        r = next(iter(json.loads(res.read_text(encoding="utf-8")).values()), {})
    s = json.loads(smp.read_text(encoding="utf-8")) if smp.exists() else {"off": {}, "on": {}}
    key = str(int(rva, 16))
    offc = sum((s.get("off", {}).get(key) or {}).values())
    onc = sum((s.get("on", {}).get(key) or {}).values())
    return {"rva": "0x" + rva + sfx, "verdict": r.get("verdict", "NO-RESULT"), "why": ",".join(r.get("why", [])),
            "installed": r.get("installed", ""), "off_sigs": r.get("off_n", 0), "on_sigs": r.get("on_n", 0),
            "off_calls": offc, "on_calls": onc, "spawned_pids": "/".join(pids),
            "samples_sha256": sha(smp)}


def main():
    av = sys.argv[1:]
    rvas = ALL if av == ["--all"] else [a.lower().replace("0x", "").zfill(8) for a in av]
    rows = {}
    if SUMMARY.exists():
        for ln in SUMMARY.read_text(encoding="utf-8").splitlines()[1:]:
            f = ln.split("\t")
            rows[f[0]] = f
    for n, rva in enumerate(rvas, 1):
        attempts = 0
        while True:
            attempts += 1
            r = one(rva)
            harness_empty = r["off_calls"] == 0 and r["on_calls"] == 0
            if not harness_empty or attempts >= 2:
                break
            print(f"    re-run {rva}: both sides empty", flush=True)
        r["attempts"] = attempts
        r["utc"] = time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime())
        rows[r["rva"]] = [str(r[c]) for c in COLS]
        print(f"[{n}/{len(rvas)}] {r['rva']} {r['verdict']} {r['why']} (off {r['off_calls']} calls/{r['off_sigs']} sigs, "
              f"on {r['on_calls']}/{r['on_sigs']})", flush=True)
        SUMMARY.write_text("\t".join(COLS) + "\n" + "".join("\t".join(rows[k]) + "\n" for k in sorted(rows)),
                           encoding="utf-8")
    return 0


if __name__ == "__main__":
    sys.exit(main())
