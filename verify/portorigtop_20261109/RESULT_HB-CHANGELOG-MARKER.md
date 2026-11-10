# RESULT HB-CHANGELOG-MARKER (2026-11-10): CHANGELOG entries back below the marker; shared C1-row helper anchors on the marker line

L2 worker `cmwb3bps015lwsc1bjg9b2bc5` for lead `cmw7xu1960f37p41557tefpnd`. Branch `orch/HB-CHANGELOG-MARKER` on
`orch/MATCH-40BB10` `fee2c5081`. Nothing in `mashedmod/`.

## Cause

The (none)->C1 CHANGELOG lines of POWERUPINIT-B, POWERUPINIT-C and MATCH-40BB10 were written by ad-hoc inline Python,
because the re-classify skill had no helper for a game C1 row. That code inserted after
`text.index("<!-- ENTRIES -->")`, the first textual match. The first match is header line 13
("**prepended immediately below the `<!-- ENTRIES -->` marker**, ..."), not the marker line, so each insert split line
13 and left its entries in the header.

The committed helpers were checked. Each anchors on a line equal to the marker and refuses otherwise:
- `l.strip() == MARKER`: c4_reclass.py:108, libid_reclass.py:179, deferred_add.py:108, deferred_close.py:54,
  uncertainty_resolve.py:76
- `l.rstrip('\r') == MARKER`: re/analysis/negctl/reearn_apply.py:96
- `nl + marker + nl`: c2_batch.py:185-189

The other CHANGELOG writers in the tree are retired one-shot batch scripts (`scripts/reclassify_batch_*.py`,
`scripts/r1/*`). They append at the end, and the skill already forbids per-slice scripts, so they were left alone.

## Fix

- New `re/tools/c1_add.py`: the shared helper for a game C1 row.
  - Appends the 11-field hooks.csv row (C1, mapped, `exe_file` / `port` empty, the file's own EOL).
  - Prepends one `(none)->C1` CHANGELOG line below the one line equal to `<!-- ENTRIES -->`.
  - Refuses when the row exists, when the `--result` file is missing, and when the number of lines equal to the
    marker is not exactly 1. `marker_index` never counts a line that only contains the marker.
- New `re/tools/tests/test_c1_add.py`, 6 tests on a fake repo root whose header prose contains the marker inline,
  as line 13 does:
  - the insert lands after the marker line, and the header line stays byte-equal
  - the CRLF hooks row has 11 fields
  - an existing row is refused
  - a prose-only marker is refused, and so is a doubled marker
  - a dry run writes nothing
- `.claude/skills/re-classify/SKILL.md`: a new subsection "C1 row for a game function that has none" points at the
  helper.

## Move (one commit, efdf6245d)

On orch/MATCH-40BB10, `fee2c5081` had already rejoined header line 13 and moved that branch's own 0x0040bb10 entry
below the marker. Then lines 14..100, 87 dated entries (2026-11-01..2026-11-10), went to just below the marker line, in
their current order:
- Before the move, the marker line is 115. After, it is 28.
- Line count 8377 -> 8377, and the multiset of lines is unchanged (asserted by the move script).
- Nothing was deleted or rewritten.

The 2026-11-01 entry at line 100 is included. The brief said "2026-11-06..11-10", but every dated line above the
marker moved.

## Checks

- `py -3.12 -m pytest -q re/tools/tests/test_c1_add.py re/tools/tests/test_c4_reclass.py
  re/tools/tests/test_libid_reclass.py re/tools/tests/test_uncertainty_resolve.py`: 32 passed.
- `py -3.12 scripts/lint_rva_bodies.py`: OK, 0 new.
- `git diff --stat orch/MATCH-40BB10 -- mashedmod/`: empty.
