#!/usr/bin/env py -3.12
"""near_leaf_frontier.py — emit C2 NEAR-LEAF promotion candidates (round 237+, 2026-06-15).

A NEAR-LEAF is a C2 first-party function whose DIRECT callees are ALL already C3
(and tracked in hooks.csv). It is promotable RIGHT NOW via a VERBATIM naked port
whose `call rel32` to each callee is rewritten as `mov eax,<callee_abs>; call eax`:
at suspended-spawn (MASHED_RE_NO_AUTO_HOOK=1) both the original and the reimpl hit
the same real C3 callee, so the early_window diff is consistent.

This pool REFRESHES as leaves promote (a function with mixed C2/C3 callees becomes
a near-leaf once its last C2 callee reaches C3). It is the sustainable solo vein
once the zero-callee frontier (promote_frontier.py) drains.

Filter:
  near_leaf = status==C2 in hooks.csv
            ∩ first-party (not a LIBRARY_BANDS row)
            ∩ MIN_BODY <= body <= MAX_BODY
            ∩ has >=1 direct callee, ALL direct callees tracked & C3
            ∩ has >=1 caller that is C2+

OUTPUT: re/analysis/plans/near_leaf_frontier.tsv
  columns: rva  name  subsystem  size  n_callees  callees_c3  best_caller
Prints the smallest-first list to stdout.
"""
import csv
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import promote_frontier as pf

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OUT = os.path.join(ROOT, "re", "analysis", "plans", "near_leaf_frontier.tsv")
MIN_BODY = 5
MAX_BODY = 260


def main():
    a = pf.analyze()
    hooks = a["hooks"]
    dc = a["direct_callees"]
    callers = a["callers"]
    size_of = a["size_of"]
    start_set = a["start_set"]
    conf = {va: row.get("confidence", "") for va, row in hooks.items()}

    rows = []
    for va, row in hooks.items():
        if conf.get(va) != "C2":
            continue
        if not pf.is_first_party(row):
            continue
        sz = size_of.get(va, 0)
        if sz < MIN_BODY or sz > MAX_BODY:
            continue
        callees = dc.get(va, set())
        if not callees:
            continue  # pure leaf -> promote_frontier.py handles it
        # every callee must be a tracked function at C3
        if any(c not in start_set for c in callees):
            continue
        cstat = [conf.get(c) for c in callees]
        if not all(s == "C3" for s in cstat):
            continue
        c2_callers = [c for c in callers.get(va, ()) if conf.get(c) in pf.PROMOTABLE_CONF]
        if not c2_callers:
            continue
        best = sorted(c2_callers)[0]
        rows.append((va, row.get("name", ""), row.get("subsystem", ""), sz,
                     len(callees), ",".join("%08x" % c for c in sorted(callees)),
                     "%08x" % best))

    rows.sort(key=lambda r: r[3])  # smallest first
    with open(OUT, "w") as f:
        f.write("rva\tname\tsubsystem\tsize\tn_callees\tcallees_c3\tbest_caller\n")
        for r in rows:
            f.write("%08x\t%s\t%s\t%d\t%d\t%s\t%s\n" % r)

    print("NEAR-LEAF FRONTIER (C2 & first-party & callees-all-C3 & C2+ caller): %d" % len(rows))
    for r in rows[:30]:
        print("  %s sz~%-4d callees=%d %-9s callees_c3=%s" % (r[0], r[3], r[4], r[2], r[5]))


if __name__ == "__main__":
    main()
