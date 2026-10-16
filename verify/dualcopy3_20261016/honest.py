"""honest.py [<repo root>] : the burn-down metric the lint cannot blind: hooks.csv C3/C4 rows whose exe_file is set and
differs from file. Split in two: (A) file is a .cpp (a real evidence-copy vs exe-copy disagreement), (B) file is a plate
path (the evidence copy is not recorded, so the row cannot be compared mechanically)."""
import collections
import csv
import io
import pathlib
import sys

root = pathlib.Path(sys.argv[1] if len(sys.argv) > 1 else ".")
rows = list(csv.reader(io.StringIO((root / "hooks.csv").read_bytes().decode("utf-8"), newline="")))
h = rows[0]
ix = {c: i for i, c in enumerate(h)}
P = "mashedmod/src/mashed_re/"
A, B, tot = [], [], 0
for r in rows[1:]:
    if len(r) < len(h):
        continue
    c = r[ix["confidence"]]
    if c in ("C3", "C4"):
        tot += 1
        f, e = r[ix["file"]], r[ix["exe_file"]]
        if e and e != f:
            rec = (r[0], r[ix["name"]], c, f.replace(P, ""), e.replace(P, ""))
            (A if f.endswith(".cpp") else B).append(rec)
print(f"C3/C4 rows: {tot}   exe_file != file: {len(A) + len(B)}   (A) file is a .cpp: {len(A)}   (B) file is a plate path: {len(B)}")
print("A by dir:", dict(collections.Counter(a[3].split("/")[0] for a in A)))
if "--list" in sys.argv:
    for a in A:
        print("A", *a)
