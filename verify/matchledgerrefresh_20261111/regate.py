"""MATCH-LEDGER-REFRESH: re-gate the 26 rows EQUIV-CHECK found byte-identical but REFUSED in the ledger.
Recompiles each source now (c4_reclass.eval_source = match.compare --bytes + relocheck.check) and gates it with
c4_reclass.gate_match (independent of hooks.csv). Writes regate.tsv beside this file."""
import csv, pathlib, sys
HERE = pathlib.Path(__file__).resolve().parent
ROOT = HERE.parents[1]
sys.path.insert(0, str(ROOT / "re" / "tools"))
import c4_reclass as c4  # noqa: E402
rows = [l.split("\t") for l in (HERE / "rows26.tsv").read_text().splitlines()[1:] if l.strip()]
ledger = {r["rva"].lower(): r for r in csv.DictReader(open(ROOT / "re/analysis/match_ledger.tsv"), delimiter="\t")}
out = [("rva", "source", "gate", "detail", "ledger_verdict", "ledger_relocheck", "ledger_pct")]
n_ok = 0
for i, (rva, src) in enumerate((r[0], r[1]) for r in rows):
    ev = c4.eval_source(str(ROOT / src))
    ok, detail, _ = c4.gate_match(c4.norm_rva(rva), src, ev)
    L = ledger.get(rva.lower(), {})
    n_ok += ok
    out.append((rva, src, "PASS" if ok else "REFUSED", detail, L.get("verdict", "?"), L.get("relocheck", "?"), L.get("best_pct", "?")))
    print(f"[{i+1}/{len(rows)}] {rva} {'PASS' if ok else 'REFUSED'} {detail}", flush=True)
with open(HERE / "regate.tsv", "w", newline="") as f:
    csv.writer(f, delimiter="\t").writerows(out)
print(f"PASS {n_ok}/{len(rows)}")
