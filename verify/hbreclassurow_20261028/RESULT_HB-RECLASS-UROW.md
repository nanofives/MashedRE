# RESULT: HB-RECLASS-UROW (re-classify gains a "resolve an UNCERTAINTIES row" operation)

Date 2026-10-28. Branch `orch/HB-RECLASS-UROW` from `orch/integration` 327fd83c6. Text-only step: no build, no game run,
no `.ps1` edited, no default or knob touched.

## 0. Verdict first

> **GREEN.** `re/tools/uncertainty_resolve.py` is a documented re-classify operation. U-9203 is resolved through it and
> D-11073 is rewritten through `deferred_close.py rewrite`. Self-test 10/10, `lint_rva_bodies.py` 0 NEW (16 known).

## 1. The operation

- Helper: `re/tools/uncertainty_resolve.py resolve --reason --evidence [--partial] U-NNNN ...`, modeled on `deferred_close.py`.
- It strikes the Type cell (the U-9201 / U-9205 in-place convention), prefixes the Statement with
  `**RESOLVED <date>** (<evidence>): <reason>. Prior statement: ...`, sets Evidence missing / Path to resolution to `n/a`
  and Blocks to `nothing` (`--partial` keeps Blocks), keeps 7 cells, and prepends one CHANGELOG line per row.
- Refuses: unknown ID, already struck ID, missing evidence file, empty reason, a row without 7 cells, ` | ` or a newline
  in a cell. All-or-nothing across IDs. Line endings preserved per line (UNCERTAINTIES.md mixes LF and CRLF, so each
  edited row keeps its own ending; the diff is 1 line).
- Docs: `.claude/skills/re-classify/SKILL.md`, section "Resolve an UNCERTAINTIES row", placed after "Add a DEFERRED row".
- Self-test: `re/tools/tests/test_uncertainty_resolve.py` (fake repo root via monkeypatch, 10 cases: strike plus EOL plus
  CHANGELOG order, idempotent second run writes nothing, 6 refusals, all-or-nothing, partial keeps Blocks).
  `py -3.12 -m pytest re/tools/tests/test_uncertainty_resolve.py` gives `10 passed` (pytest takes its slot automatically).

## 2. Tracker actions applied

| row | operation | evidence |
|---|---|---|
| U-9203 | `uncertainty_resolve.py resolve` (full, not partial: the cause is fixed, only the default flip is pending) | `verify/d3subph3_20261028/RESULT_d3subph3.md` |
| D-11073 | `deferred_close.py rewrite --when`: phase 3 merged, only D3-SUBSTATE-FLIP remains, flip should carry `MASHED_P3_RESET_PAIR` | same file |

U-9203 re-run is refused (`already struck`, exit 1), which is the idempotence check on the real file.
CHANGELOG got 2 lines (U-9203, D-11073). No function changes C-level.

## 3. Checks

- `lint_rva_bodies.py`: `16 finding(s): CROSS-TARGET=16 | allowlisted=16 NEW=0`, OK.
- Default gates: not run, no exe change (G-CTRL / B refs in `merge_train.json` apply unchanged).

## 4. Limits

- The U-9203 row still says the knob is default OFF; the flip stays the D3-SUBSTATE-FLIP decision.
- D-11073's title cell (the long phase-3 narrative) is not editable by `deferred_close.py rewrite` (it takes only why / when /
  tag), so it keeps its history text. The rewritten "Re-pickup when" cell carries the current state.
