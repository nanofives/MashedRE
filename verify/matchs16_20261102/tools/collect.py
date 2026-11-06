"""MATCH-S16 results: best score per slice function over re/match/s16/*.c (bytes rule as pilot_collect.py),
raw g2s score, source sha256. Run from the worktree:
    py -3.12 collect.py > verify/matchs16_20261102/results.tsv"""
import csv, hashlib, pathlib, sys
sys.path.insert(0, "re/tools/match")
import match

D = pathlib.Path("D:/MashedWT/MATCH-S16_data")
rows = list(csv.DictReader(open(D / "slice_rows.tsv"), delimiter="\t"))
raw = {r["rva"]: r for r in csv.DictReader(open(D / "g2s" / "results.tsv"), delimiter="\t")}
best = {}
for src in sorted(pathlib.Path("re/match/s16").glob("*.c")):
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
