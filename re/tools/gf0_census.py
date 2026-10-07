#!/usr/bin/env python3
"""GATEFIRE leg 0 — runtime substrate census scorer.

Scores the census columns appended to the MASHED_U9186_GATES dump against the
thresholds pre-registered in verify/d3_gatefire_20261008/PREREG_GATEFIRE.md s3.

MEASUREMENT ONLY. This reads CSVs; it never writes game state.

Usage:
    py -3.12 re/tools/gf0_census.py <off.csv> <on.csv> [...more arms]
    py -3.12 re/tools/gf0_census.py --arm OFF a.csv --arm ON b.csv

Every gate prints the ORIGINAL's committed figure beside the port's, because a
port-side number alone cannot say whether a value is live (memories
scratch-field-false-green, all-zero-reads-prove-nothing-alone).
"""
import csv
import sys
from collections import Counter

# Committed original-side figures. Sources cited per row so a reader can check
# them without trusting this file.
ORIG = {
    "idx364":    ("-1 on all 512 calls",            "RESULT_WITNESS.md:66"),
    "bias374":   ("0 on all 512 calls",             "RESULT_WITNESS.md:67"),
    "timer_4c8": ("0..1250, stepping by 50",        "RESULT_WITNESS.md:70"),
    "rank_4c4":  ("0",                              "RESULT_WITNESS.md:69"),
    "mode368":   ("0",                              "RESULT_WITNESS.md:64"),
    "flt360":    ("2.5",                            "RESULT_WITNESS.md:65"),
    "framedt":   ("50",                             "RESULT_WITNESS.md:68"),
    "tbl10":     ("1 (the only index ever used)",   "RESULT_WITNESS.md:106"),
    "thr_a8":    ("6.5",                            "RESULT_WITNESS.md:107"),
    "prog_96e8": ("[no original-side capture]",     "UNCERTAINTIES.md:61 says no writer in the port"),
    "refdist":   ("non-zero on 464/125/486/461 of 512", "RESULT_WITNESS.md:73"),
}

PER_CAR = ["timer_4c8", "rank_4c4", "prog_96e8", "refdist"]
SCALAR = ["idx364", "bias374", "mode368", "flt360", "framedt", "tbl10", "thr_a8"]
# The *_abs family must stay dead: it reads the original's absolute addresses,
# which are the blank image-pad standalone. A non-zero here means memory moved.
ABS_CTL = ["veh_type_abs", "state0_abs", "rec_x_abs", "rec_z_abs"]


def fmt_counter(c, limit=8):
    items = c.most_common(limit)
    body = ", ".join(f"{v!r}x{n}" for v, n in items)
    if len(c) > limit:
        body += f", ... ({len(c)} distinct total)"
    return body or "(none)"


def load(path):
    with open(path, newline="", encoding="utf-8") as f:
        return list(csv.DictReader(f))


def score(name, rows):
    print(f"\n{'=' * 72}\nARM {name}   rows={len(rows)}")
    if not rows:
        print("  *** EMPTY — nothing can be scored ***")
        return
    missing = [c for c in PER_CAR + SCALAR + ABS_CTL if c not in rows[0]]
    if missing:
        print(f"  *** MISSING COLUMNS (old-schema capture?): {missing}")

    print("\n-- scalars (one value per frame, repeated on all 4 rows) --")
    for col in SCALAR:
        if col not in rows[0]:
            continue
        c = Counter(r[col] for r in rows)
        orig, cite = ORIG.get(col, ("?", "?"))
        print(f"  {col:10} port: {fmt_counter(c)}")
        print(f"  {'':10} orig: {orig}   [{cite}]")

    print("\n-- per-car --")
    for col in PER_CAR:
        if col not in rows[0]:
            continue
        orig, cite = ORIG.get(col, ("?", "?"))
        print(f"  {col}   orig: {orig}   [{cite}]")
        for v in ("0", "1", "2", "3"):
            sub = [r[col] for r in rows if r["v"] == v]
            if not sub:
                continue
            c = Counter(sub)
            nz = sum(n for val, n in c.items() if _nonzero(val))
            print(f"    v{v}: non-zero {nz}/{len(sub)}"
                  f"  distinct={len(c)}  {fmt_counter(c, 5)}")

    print("\n-- GF0-CTL: the *_abs control family must stay dead --")
    for col in ABS_CTL:
        if col not in rows[0]:
            continue
        c = Counter(r[col] for r in rows)
        nz = sum(n for val, n in c.items() if _nonzero(val))
        verdict = "DEAD (ok)" if nz == 0 else f"*** {nz} NON-ZERO — memory moved ***"
        print(f"  {col:14} {verdict}")


def _nonzero(s):
    try:
        return float(s) != 0.0
    except ValueError:
        return bool(s and s not in ("0", "0x00000000"))


def main(argv):
    args, arms = argv[1:], []
    i = 0
    while i < len(args):
        if args[i] == "--arm":
            arms.append((args[i + 1], args[i + 2]))
            i += 3
        else:
            arms.append((args[i], args[i]))
            i += 1
    if not arms:
        print(__doc__)
        return 2
    for name, path in arms:
        score(name, load(path))
    print(f"\n{'=' * 72}")
    print("Reminder: a port-side value being non-zero does NOT by itself mean the")
    print("substrate is correct — compare against the orig line printed above, and")
    print("read PREREG_GATEFIRE.md s3's decision rules before acting on any row.")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
