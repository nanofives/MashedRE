# PREREG RO-EXE-REPLAY-KNOB (2026-11-13)

Row: replay oracle 7, `MASHED_REPLAY=<replay.json>`, default OFF. Kind: top-down / harness exe knob (WORKER_RULES
"Row kinds"). Design: `verify/replayoracle_20261111/DESIGN_REPLAY-ORACLE.md` section 3. Committed alone, before any arm.

## Change under test
- `mashedmod/src/mashed_re/Match/PortTop_Replay.cpp` (new, exe only): replay.json reader, QPC IAT wrapper scoped to the
  call site inside the matched 0x004950b0 body on the main thread, tick dispatcher `PortTop_Replay_00493390`, warp at a
  fixed tick, CRT `srand` and RW ring pins, race-anchor check, scripted input dispatcher `PortTop_Replay_00497310`,
  RW ring dispatcher `PortTop_Replay_00534870` (own ring), raw poll "no device", init + per-tick self-test.
- `Match/PortTop_00497310.cpp`, `Match/PortTop_00534870.cpp` (new, exe only): the byte-matched sources
  `re/match/s16/00497310.c`, `re/match/rwl03/00534870.c` as exe bodies `PortTop_Bound_00497310` / `PortTop_Bound_00534870`.
- `Match/MatchS15_00493480.cpp` (exe branch): 0x00493390 binds to `PortTop_Replay_00493390`, which calls
  `PortTop_Bound_00493390` directly when the knob is off.
- `Match/PortTop_00492370.cpp`: `Replay_Init` at WinMain entry, `MASHED_OT_TIMER` implied by the knob, the 0x004967e0
  stand-in skips the hand poll under the knob, `Replay_Summary` at WinMain exit.
- `mashedmod/exe_sources.rsp`: the three new TUs.

Not in this row (blocked or out of scope, named in the RESULT): the race itself (PORT-OT-RACE open: no exe body for
the cooked input 0x00496530, the RW RNG wrappers 0x00472650 / 0x00472690 / 0x004b44f0 / 0x004b4510, CRT `rand` callers
0x004c03b0 / 0x00564310); timeGetTime / GetTickCount wrappers (readers 0x005ab040 / 0x004a1570 have no exe body);
per-tick hashing (RO-EXE-TICK-TRACE).

## Arms (both builds, `MASHED_NO_DEPLOY=1`; boots through test-slot outside, `mashed_lock.py run` inside)
Root `D:/MashedWT/RO-EXE-REPLAY-KNOB_root`, `MASHED_DET_FRAMES=600`, `MASHED_WIN_POS=primary-bl`,
`verify/porttop_20261108/knob_boot.py`.

| arm | exe | env |
|---|---|---|
| vs_off | VS2022 | none |
| vs_top | VS2022 | `MASHED_ORIG_TOP=1` |
| vs_rep_a | VS2022 | `MASHED_ORIG_TOP=1 MASHED_REPLAY=replay_selftest.json` |
| vs_rep_b | VS2022 | same as vs_rep_a (repeat) |
| ddk_top | object build | `MASHED_ORIG_TOP=1` |
| ddk_rep | object build | `MASHED_ORIG_TOP=1 MASHED_REPLAY=replay_selftest.json` |
| vs_rep_nophase | VS2022 | `MASHED_ORIG_TOP=1 MASHED_REPLAY=replay_selftest_nophase.json` (warp without the phase write; run only if vs_rep_a stops after the warp) |

## Predictions (pass criteria)
1. Default OFF: vs_off and vs_top exit rc 0 after 600 frames and print no `[REPLAY]` line. The default build's code path
   is unchanged (the only default-path difference is one extra call level 0x00493480 -> PortTop_Replay_00493390 ->
   PortTop_Bound_00493390, reached only under `MASHED_OT_TIMER`, itself under `MASHED_ORIG_TOP`).
2. Object build: `ddkobj.py check` IDENTICAL for 0x00497310 and 0x00534870 (new units), and still IDENTICAL for
   0x004950b0 / 0x00493390 / 0x00493480. A refusal keeps the VS2022 body and is reported with its reason.
3. Knob arms (vs_rep_a, vs_rep_b, ddk_rep): the summary line reads `[REPLAY] self-test PASS (0 of N checks failed)`:
   - init: RW ring port = independent model over 0x136 + 512 draws, cursors equal, 0x007dc578 restored; `srand`/`rand`
     = CRT LCG (0x343fd / 0x269ec3) for 8 draws; 0x00497310 body = model on non-joystick records, dispatcher returns the
     script for player 0 only while active;
   - per tick: `DAT_007f1030` after the matched 0x00493390 = (Q_model * 3,000,000) / QPF, Q_model from q0, boot calls,
     tick count and the warp rebase, 0 failures over every tick;
   - ticks = 0x00493480 calls.
4. Determinism: in the `[REPLAY] tick` lines of vs_rep_a and vs_rep_b, the fields tick, Q, `DAT_007f1030` and phase are
   identical (the wrapped / boot / passed call counts are reported, not compared: "passed" counts real-time callers).
5. Warp at tick 300: either it runs (DAT_00771968 was 1) or it is REFUSED with the phase it saw. Both are recorded.
   [UNCERTAIN] which phase the ORIG_TOP path is in at tick 300, and what phase 2 does on the exe without PORT-OT-RACE.
   If vs_rep_a stops after a warp that ran, vs_rep_nophase separates the phase write from the rest of the warp.
6. Race anchor: predicted NOT reached (PORT-OT-RACE open). Input rows applied: 0.
7. Gates: not run in the worker (top-down row, WORKER_RULES). Default must remain OFF.

Verdict rule: GREEN when 1, 2 (or refusals reported), 3, 4 hold and 5 / 6 are recorded. RED on any self-test failure, a
`[REPLAY]` line without the knob, or a crash that is not the U-9677 piz collision.
