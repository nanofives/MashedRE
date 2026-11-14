"""PORT-TOP-RWAUDIO-TABLE-632BA8: add the rows of the master-created functions to re/console/pc_funcs.csv.
Takes ONLY the entries listed in apply_list.tsv from the read-only master dump (DumpFunctions.java after the write),
inserts them sorted by entry, and leaves every other row untouched (no full regen).
    py -3.12 verify/portorigtop_20261113/rwaudio_632ba8/ghidra/add_pcfuncs.py <dump_after.csv>"""
import csv, pathlib, sys
H = pathlib.Path(__file__).resolve().parent
W = H.parents[3]
PF = W / "re/console/pc_funcs.csv"
want = {l.split()[0].lower() for l in open(H / "apply_list.tsv") if l.strip() and not l.startswith("#")}
rows = list(csv.reader(open(PF, encoding="utf-8", newline="")))
hdr, old = rows[0], {r[0].lower(): r for r in rows[1:] if r}
d = list(csv.reader(open(sys.argv[1], encoding="utf-8", newline="")))
assert d[0] == hdr, (d[0], hdr)
dump = {r[0].lower(): r for r in d[1:] if r}
add = []
for k in sorted(want, key=lambda s: int(s, 16)):
    assert k in dump, f"{k} not in dump"
    assert k not in old, f"{k} already in pc_funcs"
    old[k] = dump[k]; add.append(dump[k])
with open(PF, "w", encoding="utf-8", newline="") as f:
    f.write(",".join(hdr) + "\n")
    for k in sorted(old, key=lambda s: int(s, 16)):
        f.write(",".join(old[k]) + "\n")
with open(H / "pcfuncs_added.csv", "w", encoding="utf-8", newline="") as f:
    f.write(",".join(hdr) + "\n")
    for r in add: f.write(",".join(r) + "\n")
print(f"added {len(add)}; pc_funcs rows now {len(old)}")
