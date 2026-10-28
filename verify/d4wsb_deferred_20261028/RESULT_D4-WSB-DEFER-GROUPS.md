# RESULT D4-WSB-DEFER-GROUPS: add-row op for DEFERRED and the 5 NOT-EXEC groups

Verdict: **GREEN**. Text-only, no build, no game run. Gates: not run (no code or default change).
Decision L-48 (a), Mariano 2026-10-28.

## 1. New re-classify operation: Add a DEFERRED row

- Script: `re/tools/deferred_add.py` (same shape as `deferred_close.py`: edits only `## Active`, prepends a
  `re/analysis/CHANGELOG.md` line below `<!-- ENTRIES -->`, preserves line endings).
- Documented in `.claude/skills/re-classify/SKILL.md`, section "Add a DEFERRED row".
- ID: highest `D-NNNN` in the file plus one (struck and Cleared rows counted). Gates: title, why, when, tag,
  reason mandatory. Refuses a vague re-pickup condition, a duplicate title in Active, a cell with ` | ` or newline,
  an `--rvas-id` that matches no TSV row. `--rvas-tsv/--rvas-id` appends the group's RVAs to the title.

## 2. Rows inserted (through that op, one call per group)

Input: `verify/d4wsb_deferred_20261028/disposition.tsv` (copy of the MAPPED-DISP table from 5f853cf38, 161 rows).

| DEFERRED id | group | RVAs |
|---|---|---|
| D-11080 | D4-WSB-DEF-BROAD | 26 |
| D-11081 | D4-WSB-DEF-BVH | 19 |
| D-11082 | D4-WSB-DEF-SOLV | 14 |
| D-11083 | D4-WSB-DEF-NARROW | 9 |
| D-11084 | D4-WSB-DEF-TAIL | 2 |

Why and re-pickup text are the ones proposed in RESULT_D4-WSB-MAPPED-DISP.md ("NOT-EXEC rows" section). Tag `physics/RenderWare`.

## 3. Verification (`verify_counts.txt`)

- Active-table rows: 180 before, 185 after (+5). Diff is +5 lines in `DEFERRED.md` and +5 in `re/analysis/CHANGELOG.md`, nothing else touched.
- RVAs inside the 5 new rows: 70, all unique, set-equal to the 70 `DEFERRED GROUP` rows of the TSV (26+19+14+9+2).
- Duplicate `D-` ids in Active: none. File still CRLF-only.
- Tracker lint: the repo has no lint for DEFERRED.md (`re/tools/lint_*` cover RVA bodies and inline asm only), so the checks above are the lint.
- `ROADMAP.md` quotes no Active-row count for DEFERRED (grep), nothing to fix.
