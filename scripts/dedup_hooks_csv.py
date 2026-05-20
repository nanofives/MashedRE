#!/usr/bin/env python3
"""Deduplicate hooks.csv: collapse rows that share the same RVA.

Policy:
- Group by RVA (column 0, normalized to lowercase 8-hex).
- Within each group, KEEP the row with highest confidence (C4 > C3 > C2 > C1
  > C0 > blank). Ties broken by: row with non-blank `file` column wins; then
  row with longest `notes` wins; then earliest appearance.
- DROPPED rows do NOT disappear — their notes are merged into the kept row's
  notes column as `[merged-from-dup: <note1> | <note2>]` so no evidence is
  lost.
- If dropped rows had different subsystems than the kept row, append a
  `subsystem-drift: <other>` tag to the kept row's notes.
- Unique-RVA rows pass through unchanged. Non-RVA rows (header, comments) pass
  through unchanged in their original position.

Idempotent: run again and it's a no-op (no duplicates remain).
"""

from __future__ import annotations

import csv
import re
import shutil
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
HOOKS = ROOT / "hooks.csv"

HEX8 = re.compile(r"^(?:0x)?([0-9a-fA-F]{8})$")
CONF_ORDER = {"C4": 5, "C3": 4, "C2": 3, "C1": 2, "C0": 1, "": 0}


def normalize_row(row: list[str]) -> list[str]:
    while len(row) < 9:
        row.append("")
    return row


def conf_rank(r: list[str]) -> int:
    return CONF_ORDER.get(r[3].strip(), 0)


def pick_kept(rows: list[tuple[int, list[str]]]) -> tuple[int, list[str]]:
    """Pick the row to KEEP among duplicates. Returns (original_index, row)."""
    # Sort by (confidence desc, has-file desc, notes-length desc, original order asc)
    def key(item: tuple[int, list[str]]) -> tuple[int, int, int, int]:
        idx, r = item
        return (
            -conf_rank(r),
            0 if r[5].strip() else 1,         # non-blank file first
            -len(r[8].strip()),                # longer notes first
            idx,                                # earliest original position first
        )

    return sorted(rows, key=key)[0]


def merge_notes(kept: list[str], dropped: list[list[str]]) -> None:
    """Merge dropped notes + subsystem-drift tags into kept's notes column."""
    kept_sub = kept[2].strip()
    extra_notes: list[str] = []
    extra_subs: set[str] = set()

    for d in dropped:
        d_note = d[8].strip()
        d_sub = d[2].strip()
        if d_note and d_note != kept[8].strip():
            extra_notes.append(d_note)
        if d_sub and d_sub != kept_sub and not d_sub.startswith("third-party-library"):
            extra_subs.add(d_sub)

    tags: list[str] = []
    if extra_notes:
        # Truncate each merged note to 120 chars to keep the file readable
        joined = " | ".join(n[:120] + ("..." if len(n) > 120 else "") for n in extra_notes)
        tags.append(f"[merged-from-dup: {joined}]")
    if extra_subs:
        tags.append(f"[subsystem-drift: {', '.join(sorted(extra_subs))}]")

    if tags:
        existing = kept[8].rstrip()
        sep = " " if existing else ""
        kept[8] = existing + sep + " ".join(tags)


def main() -> int:
    backup = HOOKS.with_suffix(".csv.bak.dedup")
    shutil.copy2(HOOKS, backup)
    print(f"Backup at {backup.relative_to(ROOT)}")

    # Read all rows preserving order
    rows: list[list[str]] = []
    with open(HOOKS, encoding="utf-8-sig", newline="") as f:
        for row in csv.reader(f):
            rows.append(row)

    # Build per-RVA dup index. Non-RVA rows mapped to None group.
    groups: dict[str, list[tuple[int, list[str]]]] = {}
    for i, row in enumerate(rows):
        if not row:
            continue
        m = HEX8.match(row[0].strip()) if row else None
        if not m:
            continue
        rva = m.group(1).lower()
        groups.setdefault(rva, []).append((i, normalize_row(row)))

    dup_rvas = {rva: g for rva, g in groups.items() if len(g) > 1}
    print(f"Unique RVAs:        {len(groups)}")
    print(f"RVAs with dupes:    {len(dup_rvas)}")
    print(f"Total extra rows:   {sum(len(g) - 1 for g in dup_rvas.values())}")
    print()

    # For each dup group: pick kept, merge notes, mark dropped for deletion
    drop_indices: set[int] = set()
    n_subsystem_conflicts = 0
    n_confidence_drops = 0  # rows where dropped had LOWER confidence than kept (expected)
    n_confidence_warnings = 0  # rows where dropped had HIGHER confidence than kept (shouldn't happen, but safeguard)

    sample_lines: list[str] = []

    for rva, group in dup_rvas.items():
        idx_kept, kept_row = pick_kept(group)
        dropped_pairs = [p for p in group if p[0] != idx_kept]
        dropped_rows = [d[1] for d in dropped_pairs]

        # Apply merged notes + subsystem drift tag IN PLACE on the kept row
        merge_notes(kept_row, dropped_rows)
        rows[idx_kept] = kept_row

        # Count conflict types
        for d in dropped_rows:
            if conf_rank(d) > conf_rank(kept_row):
                n_confidence_warnings += 1
            elif conf_rank(d) < conf_rank(kept_row):
                n_confidence_drops += 1
            if d[2].strip() and d[2].strip() != kept_row[2].strip() and not d[2].strip().startswith("third-party-library"):
                n_subsystem_conflicts += 1

        # Mark for delete
        for d_idx, _ in dropped_pairs:
            drop_indices.add(d_idx)

        if len(sample_lines) < 5:
            sample_lines.append(
                f"  0x{rva}: kept conf={kept_row[3]!r} sub={kept_row[2]!r} "
                f"file={kept_row[5]!r}; dropped {len(dropped_rows)} row(s)"
            )

    # Write the survivors back in original order
    new_rows = [r for i, r in enumerate(rows) if i not in drop_indices]
    with open(HOOKS, "w", encoding="utf-8", newline="") as f:
        w = csv.writer(f, lineterminator="\n")
        for row in new_rows:
            w.writerow(row)

    print("Sample (first 5):")
    for line in sample_lines:
        print(line)
    print()
    print(f"Dropped:                  {len(drop_indices)} dup rows")
    print(f"Subsystem conflicts:      {n_subsystem_conflicts} (tagged as [subsystem-drift: ...])")
    print(f"Confidence demote drops:  {n_confidence_drops} (dropped row had lower confidence — expected)")
    if n_confidence_warnings:
        print(
            f"WARNING: {n_confidence_warnings} drops where the dropped row "
            f"had HIGHER confidence than the kept row — should not happen; "
            f"check the sort key logic"
        )
    print(f"Final row count:          {len(new_rows)}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
