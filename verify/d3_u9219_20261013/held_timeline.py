"""Report-only: per car, the held word (0x0088fc88 + v*0xb4, tested == 0 at 0x00414490) over the consumer calls of the
driving phase: calls with held == 0, first call (index from the first driving call) with held != 0, and the returns
of 2 split by held. usage: py -3.12 held_timeline.py <c30.tsv> <ba8c filter: 6 | any>"""
import collections, sys
path, flt = sys.argv[1], sys.argv[2]
rows = [l.rstrip("\n").split("\t") for l in open(path) if l.strip()]
ix = {k: i for i, k in enumerate(rows[0])}
per = collections.defaultdict(list)
for r in rows[1:]:
    if flt != "any" and r[ix["ba8c"]] != flt: continue
    per[int(r[ix["v"]])].append(r)
print("==", path)
for v in sorted(per):
    L = per[v]
    first = next((i for i, r in enumerate(L) if r[ix["held"]] != "0"), None)
    z = sum(1 for r in L if r[ix["held"]] == "0")
    r2 = collections.Counter("held0" if r[ix["held"]] == "0" else "held!=0" for r in L if r[ix["ret"]] == "2")
    print(f"  v{v}: driving calls {len(L)}, held==0 on {z}, first held!=0 at call {first}"
          f" (value {L[first][ix['held']] if first is not None else '-'}), ret2 by held {dict(r2)}")
