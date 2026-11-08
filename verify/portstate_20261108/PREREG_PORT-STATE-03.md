# PREREG PORT-STATE-03 (2026-11-08): frontend menu records at DAT_00898ac0, player-active words at DAT_007e96fc, and the swaps they unblock

Committed alone, before any arm of this step runs. Branch `orch/PORT-STATE-03` off `orch/PORT-STATE-01` @ `9c653e93b`
(PORT-STATE-01 is in the merge train, not yet on `orch/integration`).

## Change under test (the commits after this one)

All of it sits behind the existing knob **`MASHED_PORTSTATE_FRONTEND`** (default OFF, exe only, read once,
`Frontend/PortStateFrontend.{h,cpp}`). Knob OFF runs today's code on today's storage.

1. **Records move.** `MenuRecord g_records[31]` (`Frontend/MenuNavSM.cpp`, "0x00898ac0 analogue") becomes
   `g_records_store[31]` (knob OFF) and `MenuRecord* const g_records` = DAT_00898ac0 with the knob ON, the port store
   with it OFF (same shape as L-88 / `MASHED_PORTSTATE_RECORDS`, `Vehicle/VehiclePhysicsRun.cpp:132`). Every exe
   reader and writer already goes through `g_records` (`MenuNavSM.cpp`) or `Nav_Records()` (`exe_main.cpp`).
   The original's array is **30** entries: `0x0042d3e0` loops while `eax < 0x8990dc` with `eax = entry + 4`
   (`MenuEntryZero.h`, 0x0042d413), so entry 30 (0x008990d8..0x0089910b) is other original globals, among them
   DAT_008990e4, which the exe-compiled matched bodies 0x0042aad0 (writes 1) and 0x00428140 (tests, clears) use.
   With the knob ON the exe therefore never writes entry 30: the record capacity is 30 (populate loop and prompt-strip
   append guard) and `RecordsZero` skips its entry-30 memset. With the knob OFF both stay at 31, as today.
   Witness: the `MASHED_NAV_KEYS` per-frame line gets `rec=<fnv1a-32 of records 0..29>` (both knob states).
2. **0x0042d3e0 MenuEntryArrayInit.** With the knob ON, `RecordsZero` calls the matched export `MenuEntryArrayInit`
   (`Match/MatchS07_0042d3e0.cpp`, already the RVA's one body, exe-called at boot B7), then the host's documented wider
   clear of +0x20 / +0x30 on entries 0..29 (`MenuNavSM.cpp:398-402`, unchanged open parity question). Knob OFF: the
   `MenuEntryZeroBody` helper on the store, as today.
3. **Player-active words.** `MenuGameState::player_active[4]` becomes `player_active_off[4]`. With the knob ON,
   `Q_PlayerActive(i)` returns `Pred497450(i) != 0`, the matched 0x00497450 (`Match/MatchGFFU_00497450.cpp`,
   reads `[0x007e96fc + i*0x200]`). Its extra `player < 0` guard is not on that path: the only caller is the screen
   0x1c loop, `i` in 0..3 (`MenuNavSM.cpp`). Knob OFF: the port store with the guard, as today. Nothing in the exe
   writes 0x007e96fc or `player_active_off` (both stay 0), so both paths return 0 on every call.
4. **0x0042ad10** (lane B-frontier, port `none`). New exe-only wrapper `Match/MatchS06_0042ad10.cpp` compiling the
   byte-matched `re/match/s06/0042ad10.c` (static in the source; the DDK passes `idx` in EAX, so there is no .asi
   install). With the knob ON, `PromptStripAppend` calls it as `FUN_0042ad10(&g_record_count, tag, 0x40, 0x1ac, 0)`
   in place of the hand `PromptRowInit` (the same 9 stores: +0x00 tag, +0x0c..+0x0f 0xff, +0x14 0.6f, +0x18 64.0f,
   +0x1c 428.0f, +0x24 0). Knob OFF: `PromptRowInit` on the store, as today.
5. **0x004325c0** is not swapped here. The matched body reads the records at DAT_00898ac0 (unblocked by 1) but the
   exe's one body for this RVA also carries `MASHED_NO_ANIM_TRUNC` (L-32 opt-out), and a knob-ON-only swap would
   leave two bodies in the exe (a new dual-copy allowlist line). Filed as a decision, not implemented.

## Predictions

| id | arm / check | prediction |
|---|---|---|
| P1 | default (knob OFF) `H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26` | G-CTRL `8284be46/67931834/31fe043e`, B `1d5bf4f9` x3 (`merge_train.json` refs). The gate route runs no frontend |
| P2 | knob ON `H_PSF B_PSF_K0 B_PSF_K1 B_PSF_K26` | the same hashes as P1 |
| P3 | key script (`MASHED_GOTO=1,2,4,6 MASHED_NAV_KEYS=d,u,d,u,e`, STATE-01's), knob OFF vs knob ON | the per-frame `NAV_KEYS` lines, `rec=` included, identical. First run knob OFF twice: if the two OFF logs differ, P3 is "instrument not repeatable" and the RESULT says so |
| P4 | menu walk (`MASHED_NAV_DEMO=1`), knob OFF vs ON | not scored on frame hashes (STATE-01 P4: not repeatable run to run). Each run must self-exit 0 and write its walk BMPs |
| P5 | static | build OK both targets; rva-lint NEW=0 with no allowlist line added; one body per target for 0x0042d3e0, 0x00497450, 0x0042ad10 (`scripts/rva_body_scan.py`); `backfill_exe_file.py` then `backfill_port.py` run, port column changes reported. 0x0042ad10's exe call is knob-ON only: if `backfill_port.py` moves it from `none`, the RESULT says it depends on a default-OFF knob |

A P1 failure blocks the step (bisect the commits). A P2 or P3 failure is knob-ON evidence. It does not block (the
knob is default OFF), but the RESULT explains it before the step goes to review.

## Runner

`verify/savelink_20261023/run_arms.py` (`STACK['PSF']` from PORT-STATE-01), `A4_ROOT=D:/MashedWT/PORT-STATE-03_root`
(robocopy of `original/`), exe frozen in `D:/MashedWT/PORT-STATE-03_data/exe/`, `test-slot` outside and
`mashed_lock.py run --label PORT-STATE-03` inside. Menu runs: `verify/portstate_20261108/run_nav.py` with
`--data D:/MashedWT/PORT-STATE-03_data` (an optional argument this commit adds; the default stays STATE-01's folder).
