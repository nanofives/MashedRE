# Leg F1 — R-PREFIX / F1-FIRSTCOL: how far two AI stepdumps agree cell-for-cell,
# and which columns carry the first disagreement.
#
# Usage: py -3.12 re/tools/det_prefix.py A.csv B.csv [--top N]
#
# Rows are matched on (frame, seq, v). R-PREFIX is the largest F such that every
# shared key with frame < F agrees in every column. Keys present in only one run
# count as a disagreement at that frame.
import csv, sys
from collections import defaultdict


def load(path):
    rows = {}
    order = []
    with open(path, newline="") as f:
        r = csv.DictReader(f)
        cols = r.fieldnames
        for row in r:
            k = (int(row["frame"]), int(row["seq"]), row["v"])
            rows[k] = row
            order.append(k)
    return cols, rows, order


def main():
    pa, pb = sys.argv[1], sys.argv[2]
    top = 12
    if "--top" in sys.argv:
        top = int(sys.argv[sys.argv.index("--top") + 1])

    # POST-HOC (added after the registered F1 run, 2026-10-07): with --shared-only,
    # keys present in only one run are skipped instead of counting as a disagreement,
    # so the result isolates value divergence from dump-membership divergence.
    # This is NOT the registered R-PREFIX definition; report it separately.
    shared_only = "--shared-only" in sys.argv

    cols_a, a, _ = load(pa)
    cols_b, b, _ = load(pb)
    if cols_a != cols_b:
        print("COLUMN MISMATCH — not comparable")
        return 1
    cols = [c for c in cols_a if c not in ("frame", "seq", "v")]

    keys = sorted(set(a) | set(b))
    first_bad_frame = None
    first_bad = []           # (key, [(col, va, vb), ...])
    diff_cols = defaultdict(int)
    shared = 0
    for k in keys:
        ra, rb = a.get(k), b.get(k)
        if ra is None or rb is None:
            if shared_only:
                continue
            if first_bad_frame is None:
                first_bad_frame = k[0]
                first_bad.append((k, [("<row missing>",
                                       "present" if ra else "-",
                                       "present" if rb else "-")]))
            continue
        shared += 1
        d = [(c, ra[c], rb[c]) for c in cols if ra[c] != rb[c]]
        if d:
            for c, _, _ in d:
                diff_cols[c] += 1
            if first_bad_frame is None:
                first_bad_frame = k[0]
                first_bad.append((k, d))

    max_frame = max(k[0] for k in keys)
    print(f"A = {pa}")
    print(f"B = {pb}")
    print(f"shared (frame,seq,v) keys: {shared}   max frame: {max_frame}")
    if first_bad_frame is None:
        print(f"R-PREFIX = {max_frame + 1} (IDENTICAL over the whole overlap)")
        return 0
    print(f"R-PREFIX = {first_bad_frame}  "
          f"({100.0 * first_bad_frame / max(max_frame, 1):.2f} % of the run)")
    for k, d in first_bad:
        print(f"first differing key: frame={k[0]} seq={k[1]} v={k[2]}  "
              f"({len(d)} differing columns)")
        for c, va, vb in d[:top]:
            print(f"    {c:12s}  A={va:<22s} B={vb}")
        if len(d) > top:
            print(f"    ... and {len(d) - top} more")
    print(f"columns differing anywhere (top {top} by row count):")
    for c, n in sorted(diff_cols.items(), key=lambda kv: -kv[1])[:top]:
        print(f"    {c:12s}  {n} rows")
    return 0


if __name__ == "__main__":
    sys.exit(main())
