"""MATCH-EXACT-DATA: compare match_ledger_before.tsv (778b8b91d) with the ledger rebuilt under the named-data check.

Writes refused.tsv (rows PASS before, REFUSED now, with hooks_conf and the relocheck detail) and prints a summary.
    py -3.12 verify/matchexactdata_20261111/compare.py
"""
import csv, io, pathlib, collections, subprocess

HERE = pathlib.Path(__file__).resolve().parent
ROOT = HERE.parents[1]


def load(p):
    with open(p, encoding="utf-8") as f:
        return {r["rva"]: r for r in csv.DictReader(f, delimiter="\t")}


def load_before():
    p = HERE / "match_ledger_before.tsv"          # not committed: the ledger as of 778b8b91d
    if p.is_file():
        return load(p)
    txt = subprocess.run(["git", "show", "778b8b91d:re/analysis/match_ledger.tsv"], cwd=ROOT, capture_output=True,
                         text=True, encoding="utf-8", check=True).stdout
    return {r["rva"]: r for r in csv.DictReader(io.StringIO(txt), delimiter="\t")}


before = load_before()
after = load(ROOT / "re/analysis/match_ledger.tsv")
cnt = collections.Counter()
rows = []
for rva, a in sorted(after.items()):
    b = before.get(rva)
    bv = b["verdict"] if b else "(new)"
    cnt[(bv, a["verdict"])] += 1
    if bv == "PASS" and a["verdict"] != "PASS":
        rows.append({"rva": rva, "hooks_conf": a["hooks_conf"], "subsystem": a["subsystem"], "source": a["source"],
                     "bytes_n": a["bytes_n"], "relocheck": a["relocheck"], "detail": a["detail"]})
for rva in sorted(set(before) - set(after)):
    cnt[(before[rva]["verdict"], "(gone)")] += 1
with open(HERE / "refused.tsv", "w", encoding="utf-8", newline="") as f:
    keys = ["rva", "hooks_conf", "subsystem", "source", "bytes_n", "relocheck", "detail"]
    w = csv.DictWriter(f, keys, delimiter="\t", lineterminator="\n")
    w.writeheader()
    w.writerows(rows)
print("before -> after verdict counts:")
for k, v in sorted(cnt.items()):
    print(f"  {k[0]:8} -> {k[1]:8} {v}")
print(f"PASS before, refused now: {len(rows)} (C4 in hooks.csv: {sum(r['hooks_conf'] == 'C4' for r in rows)})")
print(f"pass now: {sum(a['verdict'] == 'PASS' for a in after.values())} of {len(after)}")
