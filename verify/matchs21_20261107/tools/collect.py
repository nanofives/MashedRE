"""MATCH-S21 results: best score per slice function over re/match/s21/*.c (bytes rule as pilot_collect.py),
raw g2s score, source sha256. Run from the worktree:
    py -3.12 collect.py > verify/matchs21_20261107/results.tsv"""
import csv, hashlib, pathlib, sys
sys.path.insert(0, "re/tools/match")
import match

D = pathlib.Path("D:/MashedWT/MATCH-S21_data")
rows = list(csv.DictReader(open(D / "slice_rows.tsv"), delimiter="\t"))
raw = {r["rva"]: r for r in csv.DictReader(open(D / "g2s" / "results.tsv"), delimiter="\t")}
best = {}
# Rows with no Ghidra size (not in re/console/pc_funcs.csv): size forced to the compiled symbol's length when that
# symbol ends at or before the next known entry (same rule as S19's no-size rows). Marked "nosize" in the by column.
T = match.function_table()
NOSIZE = {}
for r in rows:
    va = int(r["rva"], 16)
    if va in T:
        continue
    src = pathlib.Path(f"re/match/s21/{va:08x}.c")
    if not src.exists():
        continue
    nxt = min(e for e in T if e > va)
    T[va] = nxt - va
    try:
        rr = [x for x in match.compare(src, "", quiet=True) if x["addr"] == va]
    except SystemExit:
        rr = []
    if rr and rr[0]["bytes_n"] <= nxt - va:
        T[va] = rr[0]["bytes_n"]; NOSIZE[r["rva"]] = (rr[0]["bytes_n"], nxt)
    else:
        del T[va]
for src in sorted(pathlib.Path("re/match/s21").glob("*.c")):
    try:
        res = match.compare(src, "", quiet=True)
    except SystemExit as e:
        print(f"# {src.name}: {str(e).splitlines()[0][:100]}", file=sys.stderr)
        continue
    for r in res:
        k = f"0x{r['addr']:08x}"
        how = "insn"
        if r["acc"] < 1 and not r["bytes_bad"] and r["bytes_n"] >= (match.ghidra_size(r["addr"]) or 0):
            r = dict(r, acc=1.0, regs=1.0); how = "bytes"
        if k in NOSIZE:
            how = "nosize" if not r["bytes_bad"] else "nosize-bytes-differ"
        if k not in best or (r["acc"], r["regs"]) > (best[k][0], best[k][1]):
            sha = hashlib.sha256(src.read_bytes()).hexdigest()[:16]
            best[k] = (r["acc"], r["regs"], how, src.as_posix(), sha)
print("rva\tsize\tsubsystem\tconf\texe_body\traw_g2s%\tbest%\tregs%\tby\tsource\tsource_sha256_16")
for r in rows:
    k = r["rva"]; n = match.ghidra_size(int(k, 16))
    b = best.get(k)
    print("\t".join([k, str(n), r["sub"], r["conf"], r["exe_file"].split("/")[-1] or "-", raw.get(k, {}).get("acc", "") or "compile",
                     f"{100*b[0]:.1f}" if b else "-", f"{100*b[1]:.1f}" if b else "-", b[2] if b else "-",
                     b[3] if b else "-", b[4] if b else "-"]))
