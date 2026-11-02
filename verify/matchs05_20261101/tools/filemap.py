"""Per source file: every // MATCH: / // HIDDEN: function, its score, and the port body / install the scanner finds.
    py -3.12 D:/MashedWT/MATCH-S05_data/tools/filemap.py   (from the worktree)"""
import csv, pathlib, re, sys
sys.path.insert(0, "re/tools/match")
import match

body = {}
for ln in open("D:/MashedWT/MATCH-S05_data/bodies.tsv"):
    r, m, b, i = ln.rstrip("\n").split("\t")
    body[int(r, 16)] = (b, i)
for src in sorted(pathlib.Path("re/match/s05").glob("*.c")):
    t = src.read_text(encoding="latin-1")
    tags = re.findall(r"^// (MATCH|HIDDEN): 0x([0-9a-fA-F]+)", t, re.M)
    try:
        res = {r["addr"]: r for r in match.compare(src, "", quiet=True)}
    except SystemExit as e:
        print(f"{src.name}\tCOMPILE-FAIL {str(e)[:80]}")
        continue
    parts = []
    for k, a in tags:
        a = int(a, 16)
        r = res.get(a)
        if r:
            ok = r["acc"] >= 1 or (not r["bytes_bad"] and r["bytes_n"] >= (match.ghidra_size(a) or 0))
            sc = "100" if ok else f"{100 * r['acc']:.1f}"
        else:
            sc = "?"
        b = body.get(a, ("(not in slice)", ""))[0]
        parts.append(f"{k[0]}{a:08x}={sc}[{b}]")
    print(f"{src.name}\t" + "  ".join(parts))
