---
session_id: leaderboard_d3
session_date: 2026-05-08
pool_slot: Mashed_pool13
bucket: leaderboard_d3
deferred_rows_consumed: D-7720..D-7755 + D-7960 (37 rows)
plates_written: 31 (30 C1 + 1 C0)
resolved_by_prior: 6
new_deferred: D-9340
assigned_ranges: U-3147..U-3166 (max 20) | S-3140..S-3159 (max 20) | D-9340..D-9399
---

## Session summary

Drained all 37 deferred rows from bucket `leaderboard_d3-cont1` (D-7720..D-7755 + D-7960).
Parent function: `Race::Tick` at `0x0040fc00`.

### Plates written (30 C1)

| RVA | Size | Notes |
|-----|------|-------|
| 0x00403250 | 582b | lap/round timer transition trigger; 8-player scan stride 0x4c |
| 0x004039f0 | 378b | countdown timer; 6-slot bonus table; case-10 game_mode==6 path |
| 0x00406ce0 | 1403b | crowd/actor animation + physics update; 19 callees; stops at first inactive entry |
| 0x0040ce00 | 125b | physics/simulation dispatch hub; 22 callee dispatch table |
| 0x0040d470 | 284b | arg-0 callee of Race::Tick general path |
| 0x0040da50 | 274b | Race::Tick general path |
| 0x0040dba0 | 40b | Race::Tick general path; tiny |
| 0x00413f20 | 42b | mode7+ branch; tiny |
| 0x004189a0 | 24b | Race::Tick general path; tiny |
| 0x0041a250 | 377b | case-10 (game_mode==6) path |
| 0x0041b540 | 229b | non-ghost branch 1 |
| 0x0041c090 | 45b | ghost branch 1 |
| 0x0041c180 | 310b | case-5 branch |
| 0x0041cc50 | 100b | non-ghost branch 2 |
| 0x0041d830 | 58b | ghost branch 2 |
| 0x0041d930 | 335b | Race::Tick general path |
| 0x0041dca0 | 207b | cases 4/7/8/9 |
| 0x0041e6c0 | 384b | case-2 branch |
| 0x004215a0 | 23b | Race::Tick general; tiny |
| 0x004222d0 | 28b | Race::Tick general; tiny |
| 0x00425a40 | 103b | pause handler (DAT_005f29c0 != 0) |
| 0x00425d50 | 235b | Race::Tick general path |
| 0x00475c10 | 273b | physics/track update |
| 0x00476440 | 146b | physics update |
| 0x004777d0 | 58b | physics update; tiny |
| 0x00477920 | 230b | physics update |
| 0x00485070 | 51b | Race::Tick general; tiny |
| 0x00488e70 | 887b | dynamic vertex buffer fill (shadow/reflection mesh); 128 entries; gate DAT_007030b4 |
| 0x00490380 | 271b | Race::Tick general path |
| 0x0045bba0 | 965b | powerup/vehicle collision dispatch; body at 0x00453f60; replay state machine |

### Plate written (1 C0 — cap exceeded)

| RVA | Size | Reason | New DEFERRED |
|-----|------|--------|--------------|
| 0x00412f30 | ~1744b | Exceeds 1500b per-function cap; listing-level only | D-9340 → leaderboard_d4-cont1 |

### Resolved by prior session (no new plates)

| RVA | D-ID closed | Prior session |
|-----|-------------|---------------|
| 0x0040e350 | D-7727 | game_state (C1, game mode getter) |
| 0x00490500 | D-7754 | effects_particle (C1, ~2371b) |
| 0x00426ab0 | D-7744 | camera_follow (C1, car-ptr camera callee) |
| 0x00467210 | D-7746 | camera_follow (C1, vehicle sub-object getter) |
| 0x00496930 | D-7755 | timer_d2 (C1, button state reader) |
| 0x00429310 | D-7960 | leaderboard_d3 DEFERRED row; already C1 in hooks.csv |

---

## Branch coverage map — Race::Tick (0x0040fc00) depth-2 callees

This is the first d3 pass over Race::Tick's depth-2 layer. Coverage by branch group:

| Branch / condition | Relevant RVAs | Status after this session |
|-------------------|---------------|--------------------------|
| DAT_007f0fd0 == 5 (crowd actor mode) | 0x00406ce0 | **RESOLVED** C1 |
| case-2 | 0x0041e6c0 | **RESOLVED** C1 |
| case-5 | 0x0041c180 | **RESOLVED** C1 |
| cases-4 / 7 / 8 / 9 | 0x0041dca0 | **RESOLVED** C1 |
| case-0xb (lap/timer trigger, DAT_007f1004) | 0x00403250 | **RESOLVED** C1 |
| case-10 (game_mode getter; returns 6 = replay) | 0x0040e350 | **RESOLVED** C1 (prior) |
| case-10 path when mode==6 (countdown/bonus) | 0x004039f0, 0x0041a250 | **RESOLVED** C1 |
| mode7+ branch | 0x00413f20 | **RESOLVED** C1 |
| ghost branch (2 callees) | 0x0041c090, 0x0041d830 | **RESOLVED** C1 |
| non-ghost branch (2 callees) | 0x0041b540, 0x0041cc50 | **RESOLVED** C1 |
| pause handler (DAT_005f29c0 != 0) | 0x00425a40 | **RESOLVED** C1 |
| general update sequence — physics hub | 0x0040ce00 | **RESOLVED** C1 (22-callee dispatch) |
| general update sequence — physics/track | 0x00475c10, 0x00476440, 0x004777d0, 0x00477920 | **RESOLVED** C1 |
| general update sequence — misc | 0x0040d470, 0x0040da50, 0x0040dba0, 0x004189a0, 0x0041d930, 0x004215a0, 0x004222d0, 0x00425d50, 0x00485070, 0x00490380 | **RESOLVED** C1 |
| general update — powerup/collision dispatch | 0x0045bba0 | **RESOLVED** C1 |
| general update — shadow/reflection VB fill | 0x00488e70 | **RESOLVED** C1 |
| general update — timer | 0x00496930 | **RESOLVED** C1 (prior) |
| FUN_00467210(0) → vehicle sub-obj getter | 0x00467210 | **RESOLVED** C1 (prior) |
| FUN_00467210(0) result → shadow mesh fill | 0x00488e70 | **RESOLVED** C1 |
| FUN_00467210(0) result → unknown large fn | 0x00412f30 | **DEFERRED** D-9340 (1744b, cap exceeded) |
| particle effects large fn | 0x00490500 | **RESOLVED** C1 (prior, effects_particle) |
| TimeTrial::Tick callee | 0x00429310 | **RESOLVED** C1 (prior) |

**Remaining depth-2 gap:** only `0x00412f30` (D-9340) needs a dedicated leaderboard_d4-cont1 session pass.

---

## New DEFERRED entry

| ID | RVA | Reason | Next session |
|----|-----|--------|-------------|
| D-9340 | 0x00412f30 | ~1744b; exceeds per-function cap; listing-level C0 plate at re/analysis/leaderboard_d3/0x00412f30.md | leaderboard_d4-cont1 |

---

## Tracker mutations

- **hooks.csv**: rows 1110–1145 replaced — 30 C1 new, 5 C1 resolved-by-prior (status note), 1 C0 deferred
- **DEFERRED.md**: D-7720..D-7755 + D-7960 deleted (all resolved); D-9340 added
- **SCRIBE_QUEUE.md**: entry added under Queued
- **STUBS.md** / **UNCERTAINTIES.md**: entries within U-3147..U-3166 / S-3140..S-3159 ranges; exact count in individual plates
