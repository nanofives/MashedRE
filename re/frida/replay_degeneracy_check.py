#!/usr/bin/env python3
"""replay_degeneracy_check.py — retroactively apply the non-degeneracy
assertion (run_diff.py, 2026-06-12) to every historical diff CSV in log/.

A past-GREEN CSV is a residual suspect when (a) every observation on both
sides is trivial AND (b) its hook's arg_type does not seed the observed
region (SEEDED_ARG_TYPES). Those runs carried no discriminating power; the
C3 promotions they justified should be re-examined (re-run at a populated
state, or degenerate_ok-acknowledged with reasoning).

Read-only: prints a report, mutates nothing.
"""

import csv
import glob
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))

from run_diff import SEEDED_ARG_TYPES, is_trivial    # noqa: E402
from hooks_registry import HOOKS                      # noqa: E402

ROOT = HERE.parent.parent
LOG = ROOT / "log"


def main():
    green = flagged_seeded = 0
    residual = []      # (hook_name, arg_type, rows)
    unknown_hook = []
    for p in sorted(glob.glob(str(LOG / "diff_*.csv"))):
        name = Path(p).stem[len("diff_"):]
        try:
            rows = list(csv.DictReader(open(p, newline="", encoding="utf-8")))
        except OSError:
            continue
        if not rows or any((r.get("match") or "").lower() not in ("true", "1")
                           for r in rows):
            continue            # only past-GREEN runs are interesting
        green += 1
        if not all(is_trivial(r.get("original")) and is_trivial(r.get("reimpl"))
                   for r in rows):
            continue            # discriminating run — GREEN stands
        hook = HOOKS.get(name)
        if hook is None:
            unknown_hook.append(name)
            continue
        at = hook.get("arg_type")
        if at in SEEDED_ARG_TYPES or hook.get("degenerate_ok"):
            flagged_seeded += 1
            continue            # seeded — all-zero result still discriminates
        residual.append((name, at, len(rows)))

    print(f"past-GREEN CSVs:                {green}")
    print(f"all-trivial but seeded (stand): {flagged_seeded}")
    print(f"no registry entry (renamed?):   {len(unknown_hook)}")
    for n in unknown_hook:
        print(f"    {n}")
    print(f"RESIDUAL SUSPECTS:              {len(residual)}")
    by_at = {}
    for n, at, c in residual:
        by_at.setdefault(at, []).append(n)
    for at, names in sorted(by_at.items(), key=lambda kv: -len(kv[1])):
        print(f"  arg_type={at}  ({len(names)})")
        for n in names:
            print(f"    {n}")


if __name__ == "__main__":
    main()
