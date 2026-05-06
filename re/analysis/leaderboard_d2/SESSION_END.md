# leaderboard_d2 — Session End

**Session ID:** leaderboard_d2-20260506  
**Date:** 2026-05-06  
**Pool slot:** Mashed_pool14 (stale lock — used master project read-only instead; see note)  
**Parent queue entry:** leaderboard-cont1 (removed from re/queue.md)  
**Successor queue entry:** leaderboard_d3-cont1 (added to re/queue.md)

## Work completed

| D-ID | RVA | Name | Result | Confidence |
|------|-----|------|--------|-----------|
| D-6580 | 0x0040e560 | FUN_0040e560 | C1 mapped | C1 |
| D-6581 | 0x0040fc00 | Race::Tick | C1 mapped (callees deferred) | C1 |
| D-6582 | 0x00430b00 | TimeDisplay::SetEntry | C1 mapped; resolves S-2220 | C1 |
| D-6583 | 0x0042f790 | GhostMode::IsActive | C1 mapped; resolves S-2225 | C1 |
| D-6584 | 0x0040d040 | Course::ValidateCarsFinished | C1 mapped; resolves S-2226 | C1 |

## Bonus functions (from call chain investigation)

| RVA | Name | Confidence |
|-----|------|-----------|
| 0x004298e0 | TimeDisplay::InitForLap | C1 |
| 0x0040e360 | RaceMode::Set | C1 |
| 0x00431d70 | Course::GetLeaderIndex | C1 |
| 0x0041f320 | Car::GetState | C1 |
| 0x0041efc0 | Car::GetLapProgress | C1 |
| 0x004103a0 | TimeTrial::LapFinishProcessor | C1 |

## Stubs resolved

- S-2220 → TimeDisplay::SetEntry (C1)
- S-2225 → GhostMode::IsActive (C1)
- S-2226 → Course::ValidateCarsFinished (C1)

## New stubs (S-2600..S-2606)

- S-2600: FUN_00429840 (arg 0xb from D-6580; arg 1 from LapFinishProcessor)
- S-2601: FUN_00411870 (arg 0, D-6580)
- S-2602: FUN_0041e130 (arg 0, D-6580)
- S-2603: FUN_00429860 (gate in LapFinishProcessor)
- S-2604: FUN_00430820 (gate in LapFinishProcessor)
- S-2605: FUN_0041da90 (lap time output, LapFinishProcessor)
- S-2606: FUN_00411d60 (post-race action in mode 2)

## New uncertainty

- U-2607: D-6584 `iVar2 / 6 * 6` formula — unknown why divisor is 6 (hypothesis: 6 sectors per lap)

## D-6581 callees deferred (leaderboard_d3)

36 new D-IDs: D-7720..D-7755. See re/queue.md leaderboard_d3-cont1 entry.

3 already classified: FUN_0042f6a0, FUN_0042f500, FUN_00429310  
2 already deferred elsewhere: FUN_0045b350 (D-6761), FUN_004a2c48 (D-6765)

## Infrastructure note

**Pool14 stale lock:** `mashed_pool/Mashed_pool14.rep/idata/~index.dat` and `~00000000.db` directory remain from a previous crashed session. No Java processes were running. The pool slot was not openable via MCP. Fell back to master project (`Mashed.gpr`) in read-only mode. Pool14 may need manual cleanup of `~`-prefixed files inside its `.rep` directory before next use.

## Key globals discovered this session

| Address | Role |
|---------|------|
| 0x0063ba8c | race phase (7=post-lap, 6=finish) |
| 0x0067ea70 | ghost mode flag |
| 0x0067ea94 | leader/target car index |
| 0x0067d99c/994/98c | HUD time display init values |
| 0x008989e0 | HUD time display buffer (base; stride 0xc) |
| 0x007f1008 | raw integer delta-time |
| 0x007f100c | scaled float delta-time |
| 0x007f0fec | total race timer (cap 0xf731400) |
| 0x0063d830 | per-car state byte array |
| 0x0063dc48 | car struct array (stride 0x2ac) |
| 0x007f1a1c | car finish-order array (4 entries, stride 16b) |
