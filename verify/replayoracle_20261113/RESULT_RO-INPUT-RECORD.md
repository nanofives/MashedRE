# RESULT RO-INPUT-RECORD: record the player-0 input of the original and turn it into a replay.json input list

Date 2026-11-13. Branch `orch/RO-INPUT-RECORD` from `orch/integration` `69cd03777`. Row kind: harness/tool (no build, no gates).

## Verdict: GREEN for the round-trip. One gap named below.

## What was added
- `re/frida/js/detorig.js`: `cfg.rec`. The replacement CModule over `FUN_00497310` (`0x00497310`, the resolver `repl`) logs one row
  `(tick, control, value)` for player 0 whenever the value it returns differs from the previous return for that control.
  The logged value is the one the game consumes (the script's value when scripted). Native code only, no `Interceptor`
  callback, 8192-row buffer, overflow counted. Armed at the race anchor. Also `cfg.inputTick`: input rows keyed by tick
  after the anchor (`tickSchedule`), the `replay.json` unit, instead of by clock call.
- `re/frida/detorig.py`: `--rec` (writes `inputrec.json`) and `--input-json <replay.json>` (player-0 rows `[tick, 0, control, value]`).
- `re/tools/inputrec2replay.py`: `convert` (log to replay.json `input`, rows `[tick - anchor, 0, control, value]`, the
  DESIGN section 2 format) and `compare` (per-tick control state of two logs).
- `verify/replayoracle_20261113/run_arm_rec.sh`: run wrapper (copy of `run_arm_asi.sh`, own root and data dir).

## Round-trip (boot mode, tick clock, track 0 mode 10, `--clock tick --boot`, original `MASHED.exe`)
1. Run A `recA`: scripted input `inputrec_scripted_A.json` (7 rows) with `--rec` -> log `inputrec_A.json`, anchor tick 600:
   `[602,4,255] [700,0,255] [730,0,0] [800,1,255] [840,1,0] [900,0,255] [920,0,0]` (absolute ticks, i.e. anchor + 2, 100, 130, 200, 240, 300, 320).
2. `inputrec2replay.py convert` -> `inputrec_converted.json` (input list `[[2,0,4,255],[100,0,0,255],...]`).
3. Run B `recB`: `--input-json inputrec_converted.json --rec` -> log `inputrec_B.json`.
4. `inputrec2replay.py compare A B`: rows identical, **321 / 321 ticks identical** (3000 / 3000 with `--ticks 3000`), 0 overflow.
   `inputrec_compare.txt`. The two log files are byte-identical (same sha256 below).
Both runs: rc 0, 15 checkpoints, 3324 ticks hashed, 0 crashes.

## Gap (NO-GUESSING)
- The recorded values here are the scripted ones, because the only input source in this harness is the script. The
  logger sits after the scripted override, so it records what the game consumes, but a run with the unscripted resolver
  (`active` off, a human at the keyboard) was **not** run: it needs a person at the machine and an unattended worker
  cannot supply one. The unscripted path of `repl` is `return orig_cell(p, c)` unchanged, logged by the same code.
  [UNCERTAIN] untested with real device input.
- The in-ASI replay (`Core/ReplayAsi.cpp`) has no record mode: that needs a build, out of scope for this harness row.
- `dt = 0` rows apply in the same tick as the anchor, before or after that tick's resolver calls, so use `dt >= 1`.

## Raw data (not committed, `D:/MashedWT/RO-INPUT-RECORD_data`), sha256 in `raw_sha256_inputrec.txt`
inputrec.json A and B `4ea68373...b0ec` (both), `recA.log` `5c889fa1...`, `recB.log` `9fdee622...`, `recA/meta.json` `8051ec32...`.
Launches: 2, each under `test-slot` outside and `mashed_lock.py run --label RO-INPUT-RECORD` inside, private root
`D:\MashedWT\RO-INPUT-RECORD_root`, only the PIDs `detorig.py` spawned. `original\` untouched.
Gates: not run, harness row (WORKER_RULES "Row kinds").
