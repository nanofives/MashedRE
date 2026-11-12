"""Verdict diff between equiv_measure.py TSVs over the same rows (EQUIV-NORETURN-RECHECK).

    py -3.12 verify/equivcheck_20261112/compare.py <label>=<tsv> <label>=<tsv> [...] [--admitted re/match/ADMITTED.tsv]

Prints, for each consecutive pair, the verdict counts and every row whose verdict changed (rva, source, old -> new,
new kind and detail head). A verdict is the column `verdict`, plus the register list for EQUIV-IF-CALLERS-IGNORE.
Then the proven rows of the last TSV that carry a `halts` entry, and (with --admitted) the verdict of every
ADMITTED.tsv row in the last TSV.
"""
import collections
import pathlib
import sys


def load(path):
    rows = {}
    lines = pathlib.Path(path).read_text().splitlines()
    head = lines[0].split("\t")
    for ln in lines[1:]:
        if not ln.strip():
            continue
        f = ln.split("\t")
        r = dict(zip(head, f + [""] * (len(head) - len(f))))
        rows[r["rva"]] = r
    return rows


def verdict(r):
    v = r["verdict"]
    if v == "EQUIV-IF-CALLERS-IGNORE":
        v += f"({r['regs']})"
    return v


def main():
    args = [a for a in sys.argv[1:] if not a.startswith("--")]
    adm = None
    if "--admitted" in sys.argv:
        adm = sys.argv[sys.argv.index("--admitted") + 1]
        args = [a for a in args if a != adm]
    runs = [(a.split("=", 1)[0], load(a.split("=", 1)[1])) for a in args]
    for lab, rows in runs:
        c = collections.Counter(r["verdict"] for r in rows.values())
        print(f"{lab}: {len(rows)} rows " + " ".join(f"{k}={v}" for k, v in sorted(c.items())))
    for (la, ra), (lb, rb) in zip(runs, runs[1:]):
        ch = [k for k in sorted(ra) if k in rb and verdict(ra[k]) != verdict(rb[k])]
        kinds = sum(1 for k in ra if k in rb and ra[k]["verdict"] == rb[k]["verdict"] == "NOT-PROVEN"
                    and ra[k]["kind"] != rb[k]["kind"])
        print(f"\n## {la} -> {lb}: {len(ch)} verdict changes, {kinds} NOT-PROVEN kind changes")
        for k in ch:
            a, b = ra[k], rb[k]
            print(f"{k}\t{b['source']}\t{verdict(a)} -> {verdict(b)}\t{b['kind']}\t{b['detail'][:140]}")
    lab, last = runs[-1]
    halted = [r for r in last.values() if r["verdict"] != "NOT-PROVEN" and r.get("halts")]
    print(f"\n## proven rows in {lab} with a path that ends at a no-return callee or trap: {len(halted)}")
    for r in sorted(halted, key=lambda r: r["rva"]):
        print(f"{r['rva']}\t{verdict(r)}\t{r['halts'][:300]}")
    if adm:
        print(f"\n## ADMITTED.tsv rows in {lab}")
        for ln in pathlib.Path(adm).read_text().splitlines()[2:]:
            f = ln.split("\t")
            r = last.get(f[0])
            print(f"{f[0]}\tadmitted {f[6]}\t{lab}: {verdict(r) if r else 'not in the row set'}"
                  + (f"\t{r['kind']}\t{r['detail'][:120]}" if r and r["verdict"] == "NOT-PROVEN" else ""))


if __name__ == "__main__":
    main()
