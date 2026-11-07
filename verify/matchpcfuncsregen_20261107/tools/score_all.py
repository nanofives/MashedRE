"""Score every re/match/**/*.c with collect.py's rule against an old and a new pc_funcs.csv.
    py -3.12 score_all.py <old.csv> <new.csv> <out.tsv>   (run from the worktree)"""
import pathlib, sys, collections
sys.path.insert(0, "re/tools/match")
import match
cache = {}
_orig = match.compile_obj
def cc(src, fl, lang, tc, *a, **k):
    key = (str(src), fl, lang, tc)
    if key not in cache: cache[key] = _orig(src, fl, lang, tc, *a, **k)
    return cache[key]
match.compile_obj = cc
old, new, out = sys.argv[1:4]
rows = collections.defaultdict(dict)
for label, path in (("old", old), ("new", new)):
    match.FUNCS, match._FUNCS = pathlib.Path(path), None
    for src in sorted(p for d in (sys.argv[4:] or ["re/match"]) for p in pathlib.Path(d).glob("**/*.c")):
        try:
            res = match.compare(src, "", quiet=True)
        except SystemExit as e:
            rows[(src.as_posix(), -1)][label] = ("refused", None, None, str(e).splitlines()[0][:60]); continue
        for r in res:
            n = match.ghidra_size(r["addr"]); how = "insn"
            if r["acc"] < 1 and not r["bytes_bad"] and r["bytes_n"] >= (n or 0):
                how = "bytes"
            best = 1.0 if how == "bytes" else r["acc"]
            rows[(src.as_posix(), r["addr"])][label] = (how, round(100 * best, 1), n, "")
        print(label, src, flush=True)
with open(out, "w") as f:
    f.write("source\trva\told_by\told_best\told_size\tnew_by\tnew_best\tnew_size\tchanged\n")
    for (s, a), d in sorted(rows.items()):
        o = d.get("old", ("-", None, None, "")); n = d.get("new", ("-", None, None, ""))
        ch = "Y" if (o[:2] != n[:2]) else ""
        f.write(f"{s}\t0x{a:08x}\t{o[0]}\t{o[1]}\t{o[2]}\t{n[0]}\t{n[1]}\t{n[2]}\t{ch}\n" if a >= 0 else f"{s}\t-\t{o[0]}\t\t\t{n[0]}\t\t\t{'Y' if o[0]!=n[0] else ''}\n")
