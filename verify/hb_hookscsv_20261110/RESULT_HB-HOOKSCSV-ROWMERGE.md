# RESULT HB-HOOKSCSV-ROWMERGE (2026-11-10)

Verdict: GREEN. Row-level 3-way merge for hooks.csv keyed by rva, wired into the merge driver that
is already registered (`hooksport`). No `.ps1` / `.sh` edit, no `.gitattributes` change, no git
config change.

## What changed
- `scripts/hookscsv_merge.py` (new): the merge. Usable as a git driver (`--driver %O %A %B`) or
  standalone (`BASE OURS THEIRS [-o OUT]`, exit 1 on conflict). Rules are in its docstring.
- `scripts/backfill_port.py`: `--merge-driver` now delegates to `hookscsv_merge.driver`. The old
  line-based driver (strip `port`, `git merge-file`, port back ours-first) stays callable as
  `--merge-driver-lines`.
- `scripts/test_hookscsv_merge.py` (new): self-test, `[N/M] name PASS` lines.

Base: branch includes cca10d77d (PORT-OT-FRONTEND with the HB-HOOKSCSV-COMMAS repair), fast-forward.

## Merge rules (summary)
- Key = `rva` cell, lowercased. Only non-comment records are keyed: a record whose line or first
  cell starts with `#` (including the quoted `"# exe_file: ...` lines) or is blank is a comment.
- Row changed on one side (edit or delete): that side. Same change on both: clean. Different
  changes: CONFLICT with the differing field names and base/ours/theirs values, git markers around
  the record, exit 1. Delete vs edit: CONFLICT.
- Added rows: inserted right after the nearest preceding data row of the adding side that is in
  the output (TOP when none). hooks.csv is not rva-sorted (556 inversions in 6155 rows today), so
  rva order is not used. Added on both sides: identical = one copy, different = CONFLICT.
- Comment records are grouped by the data row they follow (TOP before the first row) and each group
  is 3-way merged as a whole. Today all 32 comment records sit in the TOP group.
- Order = ours. Header = ours' bytes. Line endings: records split on `\n` with `\r` kept, a row
  taken from theirs is written with ours' EOL, an unchanged row is byte-identical to ours.
- Pre-`port` branch: a version missing a trailing column counts as "not changed" in it, so such a
  branch merges by its other cells (old driver's purpose, covered by test 19).
- Malformed rows (cell count != 11) are compared as raw text (field `<raw>`).

## Evidence
`py -3.12 scripts/test_hookscsv_merge.py --replay` under test-slot: **23/23 PASS**
(`verify/hb_hookscsv_20261110/selftest_HB-HOOKSCSV-ROWMERGE.log`).
- 21 synthetic cases: different adjacent rows clean, same row same change clean, same row different
  halts with field names, port-only difference halts naming `port`, adds on either side, add after a
  row the other side deleted, add/add same and different, the four delete cases, comment edits on
  one side and on both, comment line sharing a key prefix, CRLF kept against an LF theirs, header
  bytes kept, pre-`port` theirs, one-sided repair of a malformed row, no-op identical.
- Test 22: a real `git merge` in a scratch repo through `backfill_port.py --merge-driver` with
  adjacent-row edits on two branches: rc 0, expected bytes. Control, same merge with the driver
  disabled (`git merge-tree`): rc 1 (conflict). This is the PORT-OT-FRONTEND failure shape.
- Test 23, replay: base = merge-base(3167e6545, 6144429be) = a90c0355a, ours = 6144429be,
  theirs = 3167e6545. Theirs changed 11 rows: 5 taken from theirs (00404830, 00428390, 004464c0,
  0041f360, 00420870), 5 identical on both sides, and one halt, on 0x00496900 only:
  `differ in port -- port: base='none' ours='none' theirs='called'` (ours also changed exe_file
  the same way theirs did, so the only differing field is the derived `port`). Taking ours' side of
  that one record, the output is byte-identical to the lead's hand merge cca10d77d:hooks.csv
  (after CRLF normalisation; git show emits the stored LF form).

## Train registration
Nothing to add. `merge.hooksport.driver = py -3.12 scripts/backfill_port.py --merge-driver %O %A %B`
is already in the shared repo config (every worktree, including the train's, reads it), and
`.gitattributes` on orch/integration already has `/hooks.csv merge=hooksport`. The driver runs the
merging worktree's own `scripts/backfill_port.py`, so the row merge takes effect for merges done in
a checkout that has this branch merged (the merge of this branch itself still uses the old driver).
A clone without that config line falls back to git's text merge, as before. merge_train.ps1 was not
touched.

## Notes
- The port-only halt is by spec. A follow-up could auto-resolve a `port`-only conflict by
  re-deriving with `scripts/backfill_port.py` after the merge; not done here.
- No tracker was edited. Tests use in-memory text, a temp repo, and `git show` blobs.
