"""Ladder counts of a hooks.csv (overall, game vs third-party-library, per subsystem) as JSON.
Usage: py -3.12 ladder_counts.py <hooks.csv or git rev:hooks.csv via stdin '-'> <out.json>"""
import collections, csv, io, json, sys

src, out = sys.argv[1], sys.argv[2]
text = sys.stdin.read() if src == "-" else open(src, encoding="utf-8", newline="").read()
rows = list(csv.DictReader(io.StringIO(text)))
LEVELS = ["C0", "C1", "C2", "C3", "C4"]


def lv(r):
    c = (r.get("confidence") or "").strip()
    return c if c in LEVELS else "other"


res = {"overall": collections.Counter(), "game": collections.Counter(), "library": collections.Counter(),
       "subsystem": collections.defaultdict(collections.Counter)}
for r in rows:
    c = lv(r)
    sub = (r.get("subsystem") or "").strip() or "(empty)"
    res["overall"][c] += 1
    res["library" if sub.startswith("third-party-library") else "game"][c] += 1
    res["subsystem"][sub][c] += 1
json.dump({k: (dict(v) if not isinstance(v, collections.defaultdict) else {s: dict(x) for s, x in v.items()})
           for k, v in res.items()} | {"rows": len(rows)}, open(out, "w"), indent=1, sort_keys=True)
print(out, len(rows), dict(res["overall"]))
