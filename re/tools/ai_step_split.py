"""Split an AI step dump (<tag>.csv / <tag>.msd.aistep.csv) into one CSV per car.

Exists because re/tools/statediff/collateral.py's `csv:` channel keys frames by the
`frame` column, and an AI step dump interleaves 2-4 cars per frame, so the three
cars collide on the same frame key. Every D3 collateral review has needed this
split; the 2026-10-03 session found it done by hand in
verify/d3_noboost_20261003 (a1_v1.csv ...) with no tool behind it, so here it is.

Read-only on the input. Writes <stem>_v<N>.csv beside it, one per car present.

Usage: py -3.12 re/tools/ai_step_split.py <dump.csv> [<dump.csv> ...]
"""
import csv
import sys
from pathlib import Path


def split(path):
    p = Path(path)
    rows = list(csv.DictReader(open(p, newline="")))
    if not rows:
        print("%s: EMPTY" % p)
        return
    cols = list(rows[0].keys())
    stem = p.name[:-len(".csv")] if p.name.endswith(".csv") else p.name
    out = []
    for v in sorted({int(r["v"]) for r in rows}):
        sub = [r for r in rows if int(r["v"]) == v]
        q = p.with_name("%s_v%d.csv" % (stem, v))
        with open(q, "w", newline="") as f:
            w = csv.DictWriter(f, fieldnames=cols)
            w.writeheader()
            w.writerows(sub)
        out.append("%s (%d rows)" % (q.name, len(sub)))
    print("%s -> %s" % (p.name, ", ".join(out)))


def main(argv):
    if not argv:
        print(__doc__)
        return 2
    for a in argv:
        split(a)
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
