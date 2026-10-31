"""score.py <file.c>... : one line per // MATCH function: va, exact%, verdict.
BYTES-OK = whole obj symbol equal to the original in every unmasked byte (relocations masked), over at least
Ghidra's body size (pc_funcs.csv body_size) -- the pilot's --bytes criterion."""
import sys, pathlib, csv
W = "C:/Users/maria/Desktop/Proyectos/Mashed/.worktrees/MATCH-S01"
sys.path.insert(0, W + "/re/tools/match")
import match
SZ = {int(x["entry"], 16): int(x["body_size"]) for x in csv.DictReader(open(W + "/re/console/pc_funcs.csv"))}
for f in sys.argv[1:]:
    try:
        rs = match.compare(pathlib.Path(f), "/O2 /GS /Ob1" if "// FLAGS" not in open(f).read() else "", quiet=True)
    except BaseException as e:
        print(f"{f}\tERROR\t{str(e)[:120]}"); continue
    for r in rs:
        ok = "BYTES-OK" if not r["bytes_bad"] and r["bytes_n"] >= SZ.get(r["addr"], 1 << 30) else f"bad{len(r['bytes_bad'])}/{r['bytes_n']}"
        print(f"0x{r['addr']:08x}\t{100*r['acc']:.1f}\t{ok}\t{pathlib.Path(f).name}")
