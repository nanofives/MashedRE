# RESULT PORTCELL-BACKFILL-217 (BLOCKED, nothing applied)

Base: orch/integration b34672ee2. Source: D:/MashedWT/HUDINIT-A-rebase_data/hooks.csv.full-backfill (6429 data rows, current hooks.csv 6431).

## Drift check (read-only compare, rva-keyed, run from a scratch script, not committed)
- Port-cell drift vs current hooks.csv: **221** rows (task said 217, 4 more appeared since).
- 215 rows: current `''` -> file `none`. exe_file is empty on all 215 (derivable without a map).
- 2 rows: current `''` -> file `called` (not 'none').
- 4 rows: current `called` -> file `body`/`none` (00427ca0, 00556cc0, 00556d70, 00557110, ...). The file is STALE for these: current tree has PortTop_* / Match bodies (exe_file differs in 5 rows). Skipped: applying would regress them.
- 2 rows in current hooks.csv absent from the file (00550c50, 00550da0): untouched.
- Blank port cells in current hooks.csv overall: 219.

## Why nothing was applied
`scripts/backfill_port.py` derives `port` from `mashedmod/build/mashed_re.map` + `mashed_re.exe` and refuses a map older than the newest `mashedmod/src` file. The only map on this machine is main checkout's, dated 2026-10-24 (stale); no fresh map exists and WORKER_RULES forbids a build for a tracker row. `--allow-stale` would rewrite every `called`/`body` cell from a 19-day-old map, not just the drifted cells, and hand-copying cells from the file is an ad-hoc edit. Neither is allowed.

## Options (decision for lead)
1. Run a build (or point `--build-dir` at a current one, e.g. the merge train's), then `py -3.12 scripts/backfill_port.py` and commit exactly the changed port cells.
2. Allow a one-off copy of the 215 blank->`none` cells (map-independent: exe_file empty), leaving the 6 others to option 1.
