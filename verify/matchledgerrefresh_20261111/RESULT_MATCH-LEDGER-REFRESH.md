# RESULT MATCH-LEDGER-REFRESH (2026-11-11): the 26 rows pass the gate today, and 0 need promotion

Worker `cmwce32wr3h7hsc1bygansfc6` (claude3, opus-5-5) for lead `cmwcaoc5e2x1psc1buqo1zr9j`. Branch
`orch/MATCH-LEDGER-REFRESH` from `orch/MATCH-EXACT-DATA` `6ed67d96d`. Match-only, no gates, no tracker write.

## Verdict

**GREEN, no-op on the trackers.** The fresh gate passes all 26 rows: a recompile today plus the relocheck that checks
data operands (MATCH-EXACT-DATA). `c4_reclass.py batch` reports `0 pass, 0 refused, 26 skipped (already C4)`. So
`--apply` has nothing to write, and hooks.csv and CHANGELOG are unchanged. No row stays refused.

## Where the 26 came from

`verify/equivcheck_20261111/measure_v4.tsv` gives verdict `EQUIV` with empty `classes` for 26 rows, which
`crosstab.py:28` labels "byte-identical today". EQUIV-CHECK measured against the `orch/integration` ledger, where all
26 read `REFUSED` with best_pct between 42.9 and 99.4. List: `rows26.tsv`.

## Why nothing was left to do

- **hooks.csv:** all 26 rows are already `C4`, with notes reading "C4 by route match (L-93)" dated 2026-11-09 or
  2026-11-10. The promotions landed through earlier batch runs. Only the ledger snapshot was older.
- **match_ledger.tsv:** on `orch/MATCH-EXACT-DATA`, the full ledger rebuild (`build_ledger.py`, 4,133 rvas) already
  lists all 26 as `PASS`, `relocheck OK`, `hooks_conf C4`. Its RESULT counts them among the "41 REFUSED->PASS" rows
  merged after the previous build. This branch does not touch the ledger, which avoids a conflict with that rewrite.

## Fresh check (this run)

`regate.py` calls `c4_reclass.eval_source` for each source (match.compare `--bytes` + relocheck.check with the
MATCH-EXACT-DATA relocheck) and `c4_reclass.gate_match`. That gate does not depend on hooks.csv, so it re-gates the
rows that are already C4. Result: **PASS 26/26**, every row byte-identical with relocheck OK (`regate.tsv`,
`regate.log`). Sizes range from 38 B (`0x005b10e0`) to 531 B (`0x00442a60`).

Then `py -3.12 re/tools/c4_reclass.py batch --tsv batch.tsv --report dryrun.tsv` ran as a dry run: 26 SKIP "already
C4 (no write)" (`dryrun.log`, `dryrun.tsv`).

## Files

`rows26.tsv`, `regate.py`, `regate.tsv`, `regate.log`, `batch.tsv`, `dryrun.tsv`, `dryrun.log`, this RESULT.
