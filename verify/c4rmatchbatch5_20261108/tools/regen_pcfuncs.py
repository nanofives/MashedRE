"""C4R-MATCHBATCH-4 (copy of verify/c4rdxlibghidrafn_20261108/tools/regen_pcfuncs.py, output path only changed): rebuild re/console/pc_funcs.csv = DumpFunctions.java dump of master (read-only, after the
CreateLibFunctions write) + the 14 retained stale rows of MATCH-PCFUNCS-REGEN, sorted by entry, LF, same header.
    py -3.12 verify/c4rmatchbatch5_20261108/tools/regen_pcfuncs.py <dump.csv>
Prints the diff against the committed table (added / removed / changed entries)."""
import csv, pathlib, sys
W = pathlib.Path(__file__).resolve().parents[3]
PF = W / "re/console/pc_funcs.csv"
STALE = W / "verify/matchpcfuncsregen_20261107/retained_stale_rows.csv"
def rows(p):
    r = list(csv.reader(open(p, encoding="utf-8", newline="")))
    return r[0], {x[0].lower(): x for x in r[1:] if x}
hdr, old = rows(PF)
dh, dump = rows(sys.argv[1])
assert dh == hdr, (dh, hdr)
_, stale = rows(STALE)
new = dict(dump)
for k, v in stale.items():
    assert k not in new, k
    new[k] = v
add = sorted(set(new) - set(old)); rem = sorted(set(old) - set(new))
chg = sorted(k for k in set(old) & set(new) if old[k] != new[k])
with open(PF, "w", encoding="utf-8", newline="") as f:
    f.write(",".join(hdr) + "\n")
    for k in sorted(new, key=lambda s: int(s, 16)):
        f.write(",".join(new[k]) + "\n")
print(f"old {len(old)} -> new {len(new)}: added {len(add)} removed {len(rem)} changed {len(chg)} (stale kept {len(stale)})")
for k in rem: print("  removed", ",".join(old[k]))
for k in chg: print("  changed", ",".join(old[k]), "->", ",".join(new[k]))
out = W / "verify/c4rmatchbatch5_20261108/pcfuncs_added.csv"
with open(out, "w", encoding="utf-8", newline="") as f:
    f.write(",".join(hdr) + "\n")
    for k in add: f.write(",".join(new[k]) + "\n")
