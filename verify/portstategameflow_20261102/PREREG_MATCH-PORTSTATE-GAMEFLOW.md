# PREREG MATCH-PORTSTATE-GAMEFLOW (2026-11-02)

L-78 (c) (Mariano 2026-11-01), port-state migration 3/3. Base: `orch/MATCH-PORTSTATE-RECORDS` @ d5a57c978 (2/3),
itself on `orch/MATCH-PORTSTATE-COURSE` @ 622335487 (1/3). Neither is on `orch/integration` yet.

Committed alone, before any arm of this step runs.

## What the original keeps, and what the exe kept

The original runs the session from the 7-case machine 0x004929d0 on the phase word `DAT_00771968`
(`re/analysis/game_state/0x004929d0.md`). The race's course sits in four index words. Writers, read with
`re/tools/decomp_pc.py` on a read-only pool clone (2026-11-02):

| word | original writers | original readers on the race path |
|---|---|---|
| `0x00771968` phase | 0x00492770 (= 1, at 0x0049278e), 0x00493590 (= 1), 0x004929d0 cases (1 -> 2 / 4, 2 -> 3, 3 -> 5 / 7, 4 -> 1 / 6, 5 -> 4, 6 -> 3, 7 -> 3) | 0x004929d0, 0x00492d10, 0x00492d30, 0x00492e90 |
| `0x0067e9fc` game mode | 0x0043df00 (= 0xb), 0x0043dfd0 (frontend machine, before the launch) | 0x0042f6a0 (getter) |
| `0x0067f17c` row | 0x0043dfd0 | 0x00430790 (getter); the launch's `FUN_0040e170(DAT_0067f17c)` in 0x0043dfd0 |
| `0x0067ea94` mode-10 row | 0x00431d00 (= 0), 0x0043dfd0 screens 0x12 / 0x18 (cursor 0..0xc) | 0x00431d70 (getter): 0x0046b540, 0x0040d040, 0x0040d110 |
| `0x0063ba7c` selected course | 0x0040e170 only (callers 0x0043df00 with 0, 0x0043dfd0 with `DAT_0067f17c`) | 0x0040d250, 0x0040d440 |
| `0x0063ba78` loaded course | 0x0040d45b (in 0x0040d440 = case 2 of 0x004929d0) | 0x00410b30 (T04) |

0x0046b540's handling selector (0x0046b56c..0x0046b585): `v = DAT_0067f17c; if (DAT_0067e9fc == 10) v =
DAT_0067ea94; type = 0x0040ce80(v) = (*(int**)0x005f2770)[v][1]`.

The exe runs the session from `Race::GameFlow` / `RaceSession` (menu route) and from the dev-race route in
`exe_main.cpp` (`MASHED_TRACK_VIEW` + `MASHED_ROUND`, the gate scenario). There `0x0067e9fc` is written after
the course load, `0x0067f17c` / `0x0067ea94` / `0x0063ba7c` / `0x0063ba78` have no exe writer, and the
standalone selector is the constant 6 (`Vehicle/VehicleInit.cpp` `HandlingTypeIndex`). This is the blocker the
COURSE RESULT names for T13 0x0040ce80: only the Training row is right.

## Change (one knob, exe only)

`MASHED_PORTSTATE_GAMEFLOW` (default OFF, read once). `Race/GameModeState.{h,cpp}` (new). Knob OFF, every entry
point returns before touching anything.

1. **Phase word.** `GameModeState_SetPhase` writes `0x00771968` at the exe's transitions with the original's
   value: boot 1 (0x00492770), launch 2 (case 1 -> 2), race running 3 (case 2 -> 3), pause 7 / resume 3
   (case 3 <-> 7), round end 5 (case 3 -> 5), back to the frontend 1. Menu route: `GameFlow_RequestRace`,
   the LoadingRace -> InRace step, `GameFlow_SetPaused`, `GameFlow_RequestResults`, `GameFlow_RequestExit`,
   `MenuLaunchRace`. Dev-race route: 2 before the load, 3 after `StartMatch`. One log line per write.
   The exe has no cases 4 / 6 (results screen / replay), so 5 goes straight to 1 on exit. The exe's own reader of
   the word is the b5c self-test gate (`Collision/RwpIntegrator.cpp:59`), armed only by its own env knob.
2. **Game mode before the load.** `GameModeState_PreLoad` writes `0x0067e9fc` before the course load (menu
   route: the launch mode; dev-race route: the value its R6 block writes after the load). The original's
   frontend writes it before the launch.
3. **Course index.** At course load (`TrackRenderer::Load`, right after `CourseTableState_OnCourseLoad`):
   k = the row of the original course table whose record's dword 0 is the loaded `Course_Id`
   (`CourseTableState_IndexOf`, over the transcribed tables). Writes `0x0067f17c = k`, `0x0067ea94 = k` when
   `0x0067e9fc == 10`, `0x0063ba7c = k`, `0x0063ba78 = 0x0063ba7c`. In the original's mode 10 the loaded course
   is row `DAT_0067f17c` and the handling row is `DAT_0067ea94`, so `DAT_0067f17c = DAT_0067ea94 = k` is the only
   original state in which both match the track the exe loaded. On the original's Quick Battle both are 0
   (Training).
4. **Selector.** The exe arm of `VehicleInit` (0x0046b540) calls `GameModeState_HandlingType`: the original's
   chain over the published words, `0x0040ce80` being `PtrTable5f2770Get` (`Util/PromoLoop_round49.cpp`, now in
   both targets, one body) and `0x0042f6a0` the matched `GetRaceSubMode`. If the course table row is not seeded
   (`MASHED_PORTSTATE_COURSE` OFF) it returns false and the constant stays.

`run_arms.py`: `PSG` = COURSE + RECORDS + GAMEFLOW, `PSGO` = GAMEFLOW alone, `DEFA` / `PSGA` = DEF / PSG on Arctic
(`MASHED_TRACK_VIEW=Arctic`).

## Predictions

Gate runner `verify/savelink_20261023/run_arms.py`, private root `D:/MashedWT/MATCH-PORTSTATE-GAMEFLOW_root`,
exe copied out first, under `test-slot` and `mashed_lock.py run --label MATCH-PORTSTATE-GAMEFLOW`.
Arms: `H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26 H_PSG B_PSG_K0 B_PSG_K1 B_PSG_K26 H_PSGO B_DEFA_K0 B_PSGA_K0`.

| id | prediction |
|---|---|
| P1 | knob OFF: H_DEF 8284be46 / ba407a16 / 31fe043e, B_DEF_K0 / K1 / K26 1d5bf4f9 x3 (`merge_train.json` refs) |
| P2 | H_PSG log, Training (Course_Id 30): `course=30 k=0 mode=10 row=0 qbrow=0 sel=0 ld=0 v_rec_id=30 v_key=6`, then `a3 mode=10 v=0 type=6`. Phase lines: `-> 1` first, then `1 -> 2` before the course line, then `2 -> 3`, and no later phase line |
| P3 | H_PSG: step 8284be46, ptrace 31fe043e, B_PSG x3 1d5bf4f9. gates.csv 67931834 (the RECORDS-ON value by construction, `RESULT_MATCH-PORTSTATE-RECORDS.md`), with the five absolute columns masked equal to H_DEF on every row (`gates_mask.py`) |
| P4 | H_PSGO (GAMEFLOW alone, table not seeded): the course line reads `v_rec_id=-1 v_key=-1`, no `a3` line, hashes = P1 |
| P5 | Arctic (Course_Id 0, row 3, record 0x005f2584 dword +4 = 0x12): B_PSGA log `course=0 k=3 ... v_rec_id=0 v_key=18` and `a3 mode=10 v=3 type=18`. B_PSGA_K0 step hash != B_DEFA_K0 step hash (type 18 = 100 / 42000 against type 6 = 105 / 40000, `kHandlingTable`) |

P5 is a behaviour change on non-Training tracks with the knob ON. It is not a gate move (the gates race Training).
The original reaches row 3 for handling by a non-10 mode with `DAT_0067f17c = 3`, or by mode 10 with
`DAT_0067ea94 = 3`; no original-side Arctic arm is run here, so P5 shows only that the exe now reads the row
the original's selector reads from the same words.

Any gate hash other than P1 / P3 with the knob ON is a decided-flip decision for the lead (not flipped here).

## Re-check of the blocked T-rows (knob ON, from the witness and the code)

T13 0x0040ce80 (selector input), T13 0x00414490 (held word 0x0088fc88, not part of this migration: powerup state),
T10 rows (0x0040b430, 0x0040b6e0, 0x0040b700, 0x0040d440, 0x0040e340, 0x0040e450, 0x0040e460, 0x00432080),
T07 0x00411750 (`0x0042f6a0() == 2` arm), T04 0x00410b30 (`DAT_0063ba78`). The RESULT lists each row's remaining
blocker.
