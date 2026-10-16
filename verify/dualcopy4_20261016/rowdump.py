"""rowdump.py <rva-hex>... : print one hooks.csv row per RVA, field by field (handles multi-line quoted notes).
   py -3.12 rowdump.py 0041eda0 00431b70 [--notes N]   (N = max notes chars, default 1800)"""
import csv, io, pathlib, sys

root = pathlib.Path(__file__).resolve().parent.parent.parent
rows = list(csv.reader(io.StringIO((root / "hooks.csv").read_bytes().decode("utf-8"), newline="")))
h = rows[0]
args = [a for a in sys.argv[1:] if not a.startswith("--")]
n = 1800
if "--notes" in sys.argv:
    n = int(sys.argv[sys.argv.index("--notes") + 1])
    args = [a for a in args if a != str(n)]
want = {a.lower().replace("0x", "").zfill(8) for a in args}
for r in rows[1:]:
    if len(r) >= len(h) and r[0].lower() in want:
        print("=" * 70)
        for c, v in zip(h, r):
            if c == "notes" and len(v) > n:
                v = v[:n] + f" ...[+{len(v) - n}]"
            print(f"{c:11}: {v}")
