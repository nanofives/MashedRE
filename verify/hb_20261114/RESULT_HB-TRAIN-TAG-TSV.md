# RESULT HB-TRAIN-TAG-TSV

**Base: this branch is built on the UNMERGED branch orch/MATCH-C4RECLASS-TAG-OP (e5d94e26f, adds `c4_reclass.py tag`). It must merge AFTER MATCH-C4RECLASS-TAG-OP.**

## Change
`re/orchestrator/merge_train.ps1`, function `Apply-Promotions`:
- Added `re/promotions/<ID>-tag.tsv` files (header `rva`, `tag`; optional `result` column) are split off from batch TSVs (before this change the `^re/promotions/[^/]+\.tsv$` glob would have fed them to `batch`).
- Each one runs `py.exe -3.12 re/tools/c4_reclass.py tag --tsv <t> --result <res> --apply --strict` in the merged worktree on the current head. `<res>` = the `result` column when present, else the TSV path itself.
- Success: `git mv` to `re/promotions/applied/`, `git add hooks.csv re/analysis/CHANGELOG.md`, commit `Apply tags <ID> ...`.
- Failure (nonzero exit, a refused row under --strict): returns `ok=false`, same as batch TSVs, so the branch is set aside.
- Batch TSV handling is unchanged.

## Test
Two cases added to the existing `-SelfTest` (fake repo with real merges, tracker section):
- T-A: `TA-tag.tsv` tags 0x00401000 `lever=volatile`: PASS (notes tagged, confidence stays C3, CHANGELOG line, TSV moved to applied/).
- T-B: `TB-tag.tsv` tags 0x00409999 (no hooks.csv row): PASS (`c4_reclass tag refused`, branch set aside).

Command: `pwsh -NoProfile -File re/orchestrator/merge_train.ps1 -SelfTest` (temp-dir fake repos, no build, no game).
Result: `SELFTEST done: 0 failure(s)`. Full log (gitignored, not committed): verify/hb_20261114/selftest_full.log, lines 15-16 are the two new cases.

```
SELFTEST PASS: tracker: a <ID>-tag.tsv is applied at merge time (c4_reclass tag --strict): notes tagged, confidence untouched, CHANGELOG line written, TSV moved to applied/
SELFTEST PASS: tracker: a refused tag row (no hooks.csv row) = whole branch set aside (promotions: c4_reclass tag refused re/promotions/TB-tag.tsv ([1/1] 0x00409999 tag REFUSED no hooks.csv row | tag: 0 tagged, 1 refused, 0 skipped (already tagged), 1 rows) ...)
```
