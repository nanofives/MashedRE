#!/usr/bin/env py -3.12
"""survey_ab_reach.py — how much of the mutator pool can the snapshot/restore
A/B lane actually reach?

The lane (AiControllerAB.cpp) verifies a mutator by snapshotting its write
surface, running the port, restoring, running the original, and comparing. That
only works if the write surface is KNOWABLE. Two things make it unknowable:

  * INDIRECT DISPATCH — a call/jmp through a register or vtable slot cannot be
    followed statically, so anything it writes is outside the snapshot set. An
    incomplete restore yields a confident GREEN built on unrestored state, which
    is worse than no verification.
  * A large UNRESOLVED store count — bases the tracker cannot attribute. Those
    are readable by a human, but each one is analysis time.

This measures both across the pool so the lane can be sized before a session is
spent per target. Depth 2 by default: deeper is more honest but the cost grows
fast, and a target with indirect dispatch at depth 2 is already disqualified.

Usage:
  py -3.12 scripts/survey_ab_reach.py <candidates.tsv> [--depth 2] [--limit N]
"""
import bisect
import collections
import csv
import io
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import write_surface as ws

ROOT = Path(__file__).resolve().parents[1]


def main():
    src = sys.argv[1]
    depth = int(sys.argv[sys.argv.index("--depth") + 1]) if "--depth" in sys.argv else 2
    limit = int(sys.argv[sys.argv.index("--limit") + 1]) if "--limit" in sys.argv else 10**9
    base, data = ws.load()
    starts = sorted({int(r["rva"], 16) for r in
                     csv.DictReader(io.open(ROOT / "hooks.csv", encoding="utf-8"))
                     if r["rva"] and not r["rva"].startswith("#")
                     and int(r["rva"], 16) >= 0x400000})
    rows = [l.split("\t")[0] for l in io.open(ROOT / src, encoding="utf-8")
            if l.startswith("0x")][:limit]

    verdict = collections.Counter()
    clean, buckets = [], collections.Counter()
    for r in rows:
        try:
            stores, tree, indirect = ws.walk(int(r, 16), depth, base, data, starts)
        except Exception:
            verdict["ERROR"] += 1
            continue
        unres = sum(1 for s in stores if s[2] in ("unresolved", "arg_deref"))
        known = sum(1 for s in stores if s[2] in ("abs", "glob_deref", "indexed"))
        if indirect:
            verdict["indirect — NOT reachable"] += 1
        elif unres == 0 and known == 0:
            verdict["no writes found (recheck)"] += 1
        elif unres == 0:
            verdict["FULLY resolved"] += 1
            clean.append((r, known, len(tree)))
        elif unres <= 5:
            verdict["resolvable (<=5 to read)"] += 1
            clean.append((r, known, len(tree)))
        else:
            verdict["heavy (>5 unresolved)"] += 1
        buckets[min(unres // 5 * 5, 40)] += 1

    n = sum(verdict.values())
    print(f"A/B REACH over {n} mutator candidates (call-tree depth {depth}):\n")
    for k, v in verdict.most_common():
        print(f"  {k:28s} {v:5d}  ({v*100//max(n,1)}%)")
    reach = verdict["FULLY resolved"] + verdict["resolvable (<=5 to read)"]
    print(f"\n  REACHABLE (no indirect dispatch, <=5 stores to resolve): {reach}"
          f"  ({reach*100//max(n,1)}%)")
    io.open(ROOT / "re/analysis/plans/ab_reachable.tsv", "w").write(
        "".join(f"{r}\t{k}\t{t}\n" for r, k, t in sorted(clean, key=lambda x: x[2])))
    print(f"  wrote re/analysis/plans/ab_reachable.tsv ({len(clean)} rows,"
          f" smallest call tree first)")


if __name__ == "__main__":
    main()
