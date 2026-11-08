"""Score a source whose RVA has no Ghidra size (not in re/console/pc_funcs.csv): force the size to the
compiled symbol's length, then report acc/regs/bytes_bad and where the function would end vs the next known entry.
    py -3.12 nosize.py <file.c> 0x<va>      (run from the worktree)"""
import pathlib, sys
sys.path.insert(0, "re/tools/match")
import match
src, va = pathlib.Path(sys.argv[1]), int(sys.argv[2], 16)
t = match.function_table()
nxt = min(e for e in t if e > va)
t[va] = nxt - va  # provisional upper bound: next known entry
res = [r for r in match.compare(src, "", quiet=True) if r["addr"] == va]
for r in res:
    n = r["bytes_n"]
    t[va] = n
    r2 = [x for x in match.compare(src, "", quiet=True) if x["addr"] == va][0]
    print(f"0x{va:08x}\tsym_len={n}\tend=0x{va+n:08x}\tnext=0x{nxt:08x}\tacc={100*r2['acc']:.1f}\tregs={100*r2['regs']:.1f}\tbytes_bad={r2['bytes_bad']}")
