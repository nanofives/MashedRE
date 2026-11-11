"""EQUIV-CHECK mutation driver: the synthetic positives in one process, then one process per real proven row (the
first N of measure_v4.tsv's proven rows in equiv_mutate.py's seed-7 order), each capped at 300 s.

    py -3.12 verify/equivcheck_20261111/mutate_real.py [--n 25] [--jobs 3]
Writes verify/equivcheck_20261111/mutation.tsv (one row per mutant) and prints the per-class tally.
"""
import argparse
import collections
import concurrent.futures as cf
import pathlib
import random
import subprocess
import sys
import tempfile

HERE = pathlib.Path(__file__).resolve().parent
ROOT = HERE.parents[1]
TOOL = ROOT / "re" / "tools" / "match" / "equiv_mutate.py"
TSV = HERE / "measure_v4.tsv"


def run(args, timeout):
    out = pathlib.Path(tempfile.mkdtemp(prefix="mut_")) / "m.tsv"
    try:
        subprocess.run([sys.executable, str(TOOL), *args, "--out", str(out)], capture_output=True, text=True,
                       timeout=timeout, cwd=str(ROOT))
    except subprocess.TimeoutExpired:
        return None
    if not out.exists():
        return []
    return [ln.split("\t") for ln in out.read_text().splitlines()[1:] if ln and not ln.startswith("#")]


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--n", type=int, default=25)
    ap.add_argument("--jobs", type=int, default=3)
    a = ap.parse_args()
    real = []
    classes = {}
    for ln in TSV.read_text().splitlines()[1:]:
        f = ln.split("\t")
        if f[7] in ("EQUIV", "EQUIV-IF-CALLERS-IGNORE"):
            real.append((f[1], int(f[0], 16), [c for c in f[8].split(",") if c]))
            classes[f"0x{int(f[0], 16):08x}"] = [c for c in f[8].split(",") if c] or ["(byte-identical)"]
    random.Random(7).shuffle(real)
    rows = run([], 3600) or []          # synthetic positives only (no --tsv)
    print(f"[1/{a.n + 1}] synthetic {len(rows)} mutants", flush=True)
    timeouts = []
    with cf.ThreadPoolExecutor(a.jobs) as ex:
        futs = {ex.submit(run, ["--tsv", str(TSV), "--no-synthetic", "--pair", f"0x{rva:08x}"], 300): rva
                for _, rva, _ in real[:a.n]}
        for i, f in enumerate(cf.as_completed(futs), 2):
            rva = futs[f]
            r = f.result()
            if r is None:
                timeouts.append(rva)
                print(f"[{i}/{a.n + 1}] 0x{rva:08x} TIMEOUT (300 s)", flush=True)
                continue
            rows += r
            print(f"[{i}/{a.n + 1}] 0x{rva:08x} {len(r)} mutants " +
                  " ".join(f"{k}={v}" for k, v in collections.Counter(x[3] for x in r).items()), flush=True)
    # synthetic classes from test_equiv
    sys.path.insert(0, str(ROOT / "re" / "tools" / "match"))
    import test_equiv  # noqa: E402
    for name, (want, _) in test_equiv.CASES.items():
        if want != "NOT-PROVEN":
            classes[name] = test_equiv.run_pair(name).get("classes") or ["(none)"]
    per = collections.defaultdict(collections.Counter)
    for r in rows:
        for c in classes.get(r[0], ["?"]):
            per[c][r[3]] += 1
    tot = collections.Counter(r[3] for r in rows)
    lines = ["pair\tkind\tsite\tstatus\tverdict\tdetail"] + ["\t".join(r) for r in rows]
    lines += ["", f"# timeouts (300 s): {' '.join(f'0x{x:08x}' for x in timeouts)}", f"# total: {dict(tot)}"]
    lines += [f"# class {c}: " + ", ".join(f"{k}={v}" for k, v in sorted(n.items())) for c, n in sorted(per.items())]
    (HERE / "mutation.tsv").write_text("\n".join(lines) + "\n")
    print("TOTAL", dict(tot), "timeouts", len(timeouts))
    for c, n in sorted(per.items()):
        print(f"  {c}: " + ", ".join(f"{k}={v}" for k, v in sorted(n.items())))


if __name__ == "__main__":
    main()
