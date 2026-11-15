# RESULT C4RECLASS-DEMOTE-OP (run 49, L-166)

Tool-only row. No build, no gates, no tracker edit, no TSV. Branch `orch/C4RECLASS-DEMOTE-OP` on `orch/integration`
38a6f3ae5. Files: `re/tools/c4_reclass.py`, `re/tools/tests/test_c4_reclass.py`, this RESULT. The hashing code
(`gate_match` `src sha256`, `SHA_TOKEN`, `cmd_refresh`) is untouched, so C4RECLASS-SHA-EOL rebases onto it.

## Interface

### Route `demote` (C4 -> C3)

Two ways to run the same code path (`cmd_batch`):

```
py -3.12 re/tools/c4_reclass.py demote --tsv <file> [--result <RESULT.md>] [--apply] [--strict] [--report <out.tsv>] [--date YYYY-MM-DD]
py -3.12 re/tools/c4_reclass.py batch  --tsv <file> [--result <RESULT.md>] [--apply] [--strict] ...   # rows with route `demote`
```

- `demote` TSV header (tab separated, required): `rva`, `evidence`, `reason`. The route is implied.
- `batch` TSV: the usual `rva`, `route`, `evidence` with route `demote`, plus a `reason` column. Extra columns
  (`result`, ...) are ignored as before. This is the merge-train form: the train already runs
  `batch --tsv re/promotions/<ID>.tsv --result <result column> --apply --strict` for every added
  `re/promotions/*.tsv` that is not `-tag.tsv` / `-refresh.tsv`, and moves it to `re/promotions/applied/`, so a
  demote file needs no change to `merge_train.ps1`. Name it `re/promotions/<ID>.tsv` (or `<ID>-demote.tsv`), header
  `rva  route  evidence  result  reason`. Do **not** name it `-tag.tsv`: the train hands those to `tag`.
- Per row, line `[N/M] 0x<rva> demote PASS|REFUSED|SKIP <detail>`:
  - SKIP: row not at C4 (`row is C3, not C4 (no write)`). Second runs are no-ops.
  - REFUSED: no `hooks.csv` row; duplicate rva; empty `reason` or one with a tab/newline; evidence file missing;
    evidence file does not name the RVA (`0x<rva>` or the bare hex, leading zeros optional); the row's notes carry a
    `c4=` route tag (`c4=match`, `c4=libid`, `c4=frida-replay`): a row certified by a route is not demoted by this op.
  - PASS: `C4->C3: <reason>`.
- Dry run by default: verdicts, the summary line `batch: N pass, R refused, S skipped ... [by route: demote=N]` (demotions
  are counted apart from promotions in the bracket), and the CHANGELOG lines `--apply` would add.
- `--apply` (needs `--result`, as `batch`), for every PASS row:
  - `hooks.csv`: confidence `C4` -> `C3`. Status and every other column untouched. Notes get
    `<date> C4->C3 demoted (L-166): <reason>; evidence <evidence>; <RESULT>` appended (` | ` separator). Only those
    lines are re-serialized, the rest of the file stays byte for byte (same line-based I/O as the promotions, so
    `tracker_merge3.py` key-merges it).
  - `re/analysis/CHANGELOG.md`: one line per row below `<!-- ENTRIES -->`:
    `<date>  0x<rva>  <name>  C4->C3  demoted (L-166): <reason>; evidence <evidence>; evidence: <RESULT>`
    (CONFIDENCE.md "Demotion rules": demotions are loud).
  - `--strict` writes nothing when any row is REFUSED (exit 1).

### Two optional batch columns (same commit, for C4-NOTAG-87-REEARN steps 1 and 2)

- `reearn` (value `1`): a route `match` / `libid` / `frida-replay` row already at C4 whose notes carry **no** `c4=` tag
  is gated like a new promotion instead of SKIP. PASS keeps C4, appends the route note (`c4=match; match.py ... src
  sha256 ...`) and writes `C4->C4  re-earned route <route> (L-93): ...`. REFUSED leaves the row as it is. A row already
  tagged still SKIPs, so a second run is a no-op. Without the column (or an empty value) `batch` behaves as before:
  the existing test `[3/8] 0x00426090 match SKIP already C4` still passes.
  Reason: `batch` skips every C4 row, so an untagged C4 row could not take `c4=match` through the gated path, and
  `tag` appends text without recompiling.
- `frida_diff`: when the row is written (promoted, re-earned or demoted), its `frida_diff` cell is set to this value in
  the same write. For the 5 rows whose cell is the bare word `green`.

## Tests

`py -3.12 -m pytest re/tools/tests/test_c4_reclass.py -v` on this branch (output below). New tests:
`test_demote_dry_run_verdicts_write_nothing`, `test_demote_apply_touches_only_that_line_and_is_idempotent`,
`test_demote_refusals` (tagged row, evidence not naming the RVA, missing evidence, empty reason, all under `--strict`),
`test_demote_needs_reason_column`, `test_batch_route_demote_is_the_merge_train_path` (train-style header, demote and
match in one `--apply --strict`), `test_reearn_column_gates_an_untagged_c4_row`, `test_reearn_column_refusal_leaves_row`,
`test_frida_diff_column_replaces_the_cell_on_write`. All earlier tests unchanged.

```
[test-slot] acquired slot 4/8 after 600s
collecting ... collected 29 items
re/tools/tests/test_c4_reclass.py::test_dry_run_writes_nothing PASSED    [  3%]
re/tools/tests/test_c4_reclass.py::test_apply_touches_only_promoted_lines PASSED [  6%]
re/tools/tests/test_c4_reclass.py::test_apply_needs_result PASSED        [ 10%]
re/tools/tests/test_c4_reclass.py::test_strict_writes_nothing_on_refusal PASSED [ 13%]
re/tools/tests/test_c4_reclass.py::test_match_refusals[ev0-not byte-identical] PASSED [ 17%]
re/tools/tests/test_c4_reclass.py::test_match_refusals[ev1-relocheck BAD] PASSED [ 20%]
re/tools/tests/test_c4_reclass.py::test_match_refusals[ev2-shorter than the original body] PASSED [ 24%]
re/tools/tests/test_c4_reclass.py::test_toolchain_must_be_ddk PASSED     [ 27%]
re/tools/tests/test_c4_reclass.py::test_second_apply_is_a_noop PASSED    [ 31%]
re/tools/tests/test_c4_reclass.py::test_tag_dry_run_writes_nothing PASSED [ 34%]
re/tools/tests/test_c4_reclass.py::test_tag_apply_is_notes_only_and_idempotent PASSED [ 37%]
re/tools/tests/test_c4_reclass.py::test_tag_refuses_missing_row_and_strict PASSED [ 41%]
re/tools/tests/test_c4_reclass.py::test_batch_lever_column_tags_promoted_row PASSED [ 44%]
re/tools/tests/test_c4_reclass.py::test_refresh_changes_only_the_sha PASSED [ 48%]
re/tools/tests/test_c4_reclass.py::test_refresh_idempotent_when_current PASSED [ 51%]
re/tools/tests/test_c4_reclass.py::test_refresh_dry_run_writes_nothing PASSED [ 55%]
re/tools/tests/test_c4_reclass.py::test_refresh_refuses_below_100 PASSED [ 58%]
re/tools/tests/test_c4_reclass.py::test_refresh_refuses_relocheck_not_ok PASSED [ 62%]
re/tools/tests/test_c4_reclass.py::test_refresh_refuses_row_not_at_c4 PASSED [ 65%]
re/tools/tests/test_c4_reclass.py::test_refresh_refuses_other_source_and_missing_token PASSED [ 68%]
re/tools/tests/test_c4_reclass.py::test_refresh_strict_writes_nothing_on_one_refusal PASSED [ 72%]
re/tools/tests/test_c4_reclass.py::test_demote_dry_run_verdicts_write_nothing PASSED [ 75%]
re/tools/tests/test_c4_reclass.py::test_demote_apply_touches_only_that_line_and_is_idempotent PASSED [ 79%]
re/tools/tests/test_c4_reclass.py::test_demote_refusals PASSED           [ 82%]
re/tools/tests/test_c4_reclass.py::test_demote_needs_reason_column PASSED [ 86%]
re/tools/tests/test_c4_reclass.py::test_batch_route_demote_is_the_merge_train_path PASSED [ 89%]
re/tools/tests/test_c4_reclass.py::test_reearn_column_gates_an_untagged_c4_row PASSED [ 93%]
re/tools/tests/test_c4_reclass.py::test_reearn_column_refusal_leaves_row PASSED [ 96%]
re/tools/tests/test_c4_reclass.py::test_frida_diff_column_replaces_the_cell_on_write PASSED [100%]
============================= 29 passed in 1.98s ==============================
```
