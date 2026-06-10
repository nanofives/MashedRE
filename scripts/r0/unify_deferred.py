#!/usr/bin/env python3
"""R0 DEFERRED-store unification (Phase R0, 2026-06-09).

Two parallel DEFERRED stores existed (root DEFERRED.md + re/DEFERRED.md,
audit finding #4). This script folds re/DEFERRED.md (6-col schema:
ID|RVA|Name|Subsystem|Pickup|Bucket) into root DEFERRED.md:

  - rows whose RVA is already C2+ in hooks.csv -> '## Cleared' (4-col schema)
    with outcome 'cleared 2026-06-09 R0: RVA already <Cx> in hooks.csv'
  - remaining rows -> '## Active' (5-col schema), pickup condition preserved
  - incoming IDs colliding with root IDs are renumbered past the global max
    (D-11031..) with a breadcrumb in the row

re/DEFERRED.md is replaced by a tombstone pointing at root DEFERRED.md.
Prints per-row dispositions. Backups to log/backups/.
"""
import csv
import re
import shutil
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
ROOT_DEF = ROOT / "DEFERRED.md"
RE_DEF = ROOT / "re" / "DEFERRED.md"
HOOKS = ROOT / "hooks.csv"
BK = ROOT / "log" / "backups"

ROW = re.compile(r"^\|\s*~{0,2}D-(\d+)")


def load_conf() -> dict[str, str]:
    conf = {}
    with open(HOOKS, newline="", encoding="utf-8") as f:
        for row in csv.reader(f):
            if not row or row[0].startswith("#") or row[0] == "rva":
                continue
            conf[row[0].lower()] = row[3]
    return conf


def main() -> int:
    conf = load_conf()
    root_lines = ROOT_DEF.read_text(encoding="utf-8").splitlines()
    re_lines = RE_DEF.read_text(encoding="utf-8").splitlines()

    root_ids = {int(m.group(1)) for l in root_lines if (m := ROW.match(l))}
    next_id = max(root_ids | {int(m.group(1))
                  for l in re_lines if (m := ROW.match(l))}) + 1

    to_active: list[str] = []
    to_cleared: list[str] = []
    n_renumbered = 0
    n_rows = 0
    for l in re_lines:
        m = ROW.match(l)
        if not m:
            continue
        n_rows += 1
        cells = [c.strip() for c in l.strip().strip("|").split("|")]
        if len(cells) != 6:
            print(f"FATAL: unexpected cell count {len(cells)}: {l[:80]}")
            return 1
        did, rva, name, subsystem, pickup, bucket = cells
        num = int(m.group(1))
        breadcrumb = ""
        if num in root_ids:
            breadcrumb = f"; renumbered from {did} 2026-06-09 R0 (ID collision)"
            did = f"D-{next_id}"
            next_id += 1
            n_renumbered += 1
        c = conf.get(rva.lower().removeprefix("0x"), None)
        title = f"0x{rva.lower().removeprefix('0x')} {name}"
        if c in ("C2", "C3", "C4"):
            to_cleared.append(
                f"| {did} | {title} | cleared 2026-06-09 R0 unification: RVA "
                f"already {c} in hooks.csv (was: {pickup}; bucket {bucket})"
                f"{breadcrumb} | 2026-06-09 |")
            print(f"CLEAR {did} {rva} ({c})")
        else:
            why = "from re/DEFERRED.md (unified 2026-06-09 R0)" + breadcrumb
            to_cleared_or = conf.get(rva.lower().removeprefix("0x"), "not-in-hooks.csv")
            to_active.append(
                f"| {did} | {title} | {why}; subsystem {subsystem}; "
                f"hooks.csv={to_cleared_or} | {pickup} | {bucket} |")
            print(f"KEEP  {did} {rva} ({to_cleared_or})")

    idx_cleared = next(i for i, l in enumerate(root_lines)
                       if l.startswith("## Cleared"))
    # insert actives at end of Active section (just before Cleared header,
    # trimming trailing blanks), cleared at end of Cleared table (before the
    # next section header).
    idx_next_after_cleared = next(i for i in range(idx_cleared + 1, len(root_lines))
                                  if root_lines[i].startswith("## "))

    pre = root_lines[:idx_cleared]
    while pre and pre[-1].strip() == "":
        pre.pop()
    mid = root_lines[idx_cleared:idx_next_after_cleared]
    while mid and mid[-1].strip() == "":
        mid.pop()
    out = (pre + to_active + [""] + mid + to_cleared + [""]
           + root_lines[idx_next_after_cleared:])

    before_total = sum(1 for l in root_lines if ROW.match(l)) + n_rows
    after_total = sum(1 for l in out if ROW.match(l))
    if after_total != before_total:
        print(f"FATAL: row count drift {before_total} -> {after_total}")
        return 1

    BK.mkdir(parents=True, exist_ok=True)
    shutil.copy2(ROOT_DEF, BK / "DEFERRED.md.pre_r0")
    shutil.copy2(RE_DEF, BK / "re_DEFERRED.md.pre_r0")
    ROOT_DEF.write_text("\n".join(out) + "\n", encoding="utf-8")
    RE_DEF.write_text(
        "# DEFERRED.md (tombstone)\n\n"
        "This store was unified into the root `DEFERRED.md` on 2026-06-09 "
        "(Phase R0; see re/analysis/AUDIT_2026-06-09.md finding #4 and "
        "scripts/r0/unify_deferred.py). All 112 rows were dispositioned "
        "there (stale rows -> '## Cleared', live rows -> '## Active'). Do "
        "not add rows here; the root file is the only DEFERRED tracker.\n",
        encoding="utf-8")

    print(f"\nDONE rows_moved={n_rows} to_active={len(to_active)} "
          f"to_cleared={len(to_cleared)} renumbered={n_renumbered}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
