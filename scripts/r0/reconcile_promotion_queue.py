#!/usr/bin/env python3
"""R0 PROMOTION_QUEUE.md reconciliation (Phase R0, 2026-06-09).

Audit finding: ~30 rows sat in '## Queued' although CHANGELOG/hooks.csv show
their work merged. Per-row evidence gathered 2026-06-09:
  - rows whose promoted RVAs are all C3+ in hooks.csv (or whose demotions are
    recorded there, e.g. 004c5890 demoted by frida-sweep-q), or whose
    SWEEP-CRITICAL harness artifacts (arg_types in diff_template.js, CONFIG
    keys in run_diff.py) are present on this branch -> provably merged.
  - zero-yield-halt rows (no rvas) -> nothing to merge; closed.
  - EXCEPTIONS kept Queued: c3-batch-af-s1 (claimed 0x00473540+0x004736c0
    C2->C3; hooks.csv still shows C2, no promotion record) and c3-batch-af-s4
    (0x00401ee0 likewise). Evidence + reimpl files exist but central
    classification never landed — genuinely pending.

Moves merged/closed rows to '## Merged' with a reconciliation note appended.
"""
import re
import shutil
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
PATH = ROOT / "re" / "PROMOTION_QUEUE.md"
BACKUP = ROOT / "log" / "backups" / "PROMOTION_QUEUE.md.pre_r0"

KEEP_QUEUED = {"c3-batch-af-s1", "c3-batch-af-s4"}
ANNOTATE = {
    "c3-batch-af-s1": " || R0-RECONCILE 2026-06-09: NOT merged — hooks.csv "
                      "still shows 0x00473540/0x004736c0 at C2 with no "
                      "promotion record; evidence+reimpl exist; needs "
                      "re-classify drain.",
    "c3-batch-af-s4": " || R0-RECONCILE 2026-06-09: PARTIAL — 0x0043aee0 "
                      "landed C3, but 0x00401ee0 still C2 with no promotion "
                      "record; needs re-classify drain.",
}
NOTE = (" || reconciled=2026-06-09-R0: verified merged/closed (promoted rvas "
        "C3+ in hooks.csv, demotions recorded, or zero-yield session with "
        "nothing to merge; harness arg_types verified present in "
        "diff_template.js/run_diff.py)")


def main() -> int:
    text = PATH.read_text(encoding="utf-8")
    m = re.search(r"(## Queued\n)(.*?)(\n## Merged\n)(.*)", text, re.S)
    if not m:
        print("FATAL: section structure not found")
        return 1
    q_head, q_body, m_head, m_body = m.groups()

    stay, moved = [], []
    for line in q_body.splitlines():
        if not re.match(r"^20", line):
            if line.strip():
                stay.append(line)
            continue
        name = line.split()[1]
        if name in KEEP_QUEUED:
            stay.append(line.rstrip() + ANNOTATE[name])
        else:
            moved.append(line.rstrip() + NOTE)

    n_before = sum(1 for l in q_body.splitlines() if re.match(r"^20", l))
    out = (text[:m.start()]
           + q_head + "\n".join(stay) + ("\n" if stay else "")
           + m_head + "\n".join(moved) + "\n" + m_body)

    n_after_q = len([l for l in stay if re.match(r"^20", l)])
    n_after_m = len(moved)
    if n_after_q + n_after_m != n_before:
        print(f"FATAL: row drift {n_before} -> {n_after_q}+{n_after_m}")
        return 1

    BACKUP.parent.mkdir(parents=True, exist_ok=True)
    shutil.copy2(PATH, BACKUP)
    PATH.write_text(out, encoding="utf-8")
    print(f"OK queued_before={n_before} moved_to_merged={n_after_m} "
          f"kept_queued={n_after_q} ({sorted(KEEP_QUEUED)})")
    return 0


if __name__ == "__main__":
    sys.exit(main())
