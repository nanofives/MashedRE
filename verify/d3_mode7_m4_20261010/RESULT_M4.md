# RESULT: mode 7 leg M4, the consumer `FUN_00414c30` + `FUN_00414490`

Pre-registration: `PREREG_M4.md` (`250fd7a6`, before any code). Knob `MASHED_MODE7` (default OFF; effective only with the M3
set `MASHED_REGISTRY + MASHED_PICKUPS_ORIG + MASHED_REG_CARS + MASHED_REG_CALLBACKS`). D-11074's four legs are now ported.

## 0. Verdict first

> **The mode-7 consumer runs verbatim and replays exact.** `FUN_00414c30` + `FUN_00414490` reproduce all 10,777 captured
> original calls (return, target and every nested `FUN_00414300` argument set) at PC_24 and PC_53. In the exe the consumer
> sees what the original's sees: `cc = 0` at every call and every car slot equal to that car's current record position.
> Knob ON, car 1 enters mode 7 on pool 0 from frame 97 as predicted (25 returns vs the original's 49), but **cars 2 and 3
> also enter mode 7** (52 and 43 returns vs 0): the exe's refdist bridge gives them refdist 0 where the original never does.
> (b) goes from 2/3 to 0/3 under the knob (report-only). Knob OFF unchanged.

| gate | result |
|---|---|
| **G-M4-REPLAY** (`o_c30a` + `o_c30b`, 10,777 calls) | **10,777 / 10,777** at PC_24 and PC_53: ret, cand and nested calls exact; `FUN_00414300` argument mismatches 0 (car 1 r2 98, car 3 r1 2,430, the rest 0) (`m4_replay_orig_pc{24,53}.txt`) |
| G-M4-OFF, knob OFF | `M4_CTRL`: `ae67b13a / c8317ce2 / 31db9384` PASS |
| G-M4-OFF, M3b set without `MASHED_MODE7` | `M4_M3B`: `b0cc1af6 / eaf2b78e / 8cd7507f` = `M3B_ON` (inert). **Prereg error:** the PREREG row named G-CTRL for this arm; its base is `M3B_ON`, since `MASHED_REG_CALLBACKS` already changes the race |
| **G-M4-ORDER** (exe, knob ON) | PASS: `cc = 0` at 17,189 / 17,189 calls; car slot == record `+0x958` on 68,747 / 68,747 |
| G-M4-STUBS | PASS: 0 calls into the type-0x15 arm |
| G-M4-COUNT (report) | `FUN_00414c30` returns 2: car 1 **25** (first frame 97, pool 0), car 2 **52** (first frame 16, pool 4), car 3 **43** (first frame 599); original 49 / 0 / 0 |
| (b) per regime (report) | `M4_ON`: car 1 FAIL (`c0_distinct` 9), car 2 FAIL (`c0_distinct` 43, `|steer|` 30.5), car 3 FAIL (`|steer|` 0); base `M3B_ON` 2/3 |
| collateral (report) | 253 / 497 fields past the floor (0), none before step frame 16 (car 2's look target); player trace from frame 24; `c9ec` within the floor; no D2 reopen candidate |
| G-M4-CTRL-ON (report) | `d57dd28d / b18f69f2 / 803b179c` |

## 1. What the original does (`o_c30a` / `o_c30b`, o_wobj2 recipe, 60 s)

`FUN_00414c30` runs from the AI tick, which `FUN_0040fc00` calls first (`0x0040fc6c` -> `FUN_00425a40` -> `0x00425a73`
`FUN_00418860` -> `FUN_00418560` -> `FUN_00416250` `0x004163c3`), before the car registrant, the pickup tick and the publish.
Measured: `cc = 0` at all 10,777 calls and car slots equal the cars' record positions on 43,084 / 43,084, so it reads the
previous frame's complete publish. Car 1 returns 2 on 49 consecutive calls (pool 0, refdist 0, held 0, sub-state 6) in both
runs; car 2's refdist is never 0 before the round-end freeze (0 / 890 calls). Car 3's 2,430 returns of 1 in `o_c30b` all sit
at the frozen post-round frame (type 0x11 slots).

## 2. Port

`Ai/AiModeSeven_naked.inc` (spec `re/tools/naked_specs/mode7_m4.json`), in `Ai/AiTargetClosest.cpp` next to P3: verbatim
`FUN_00414c30` (705 bytes incl. the `fstp / jmp` block at `0x00414eea`), `FUN_00414490`, `FUN_00484c70`, `FUN_00442cc0`,
`FUN_004c39b0` (LUT patched). Bindings: `FUN_0040e350` -> `aib_game_sub_mode`; the held word read at `0x004144b3` patched to
`TG_HeldWord(v)` -> `aib_held_powerup` (EAX / ECX preserved); P3's `TG_VehPtr`, `TG_Fwd`, `TG_4c3ac0`, `TG_414300`; the
type-0x15 frustum calls counted stubs. Wired in `ControlStep` at the `LAB_004163b7` point (mode still 0 after the P3 block),
committing mode 3 / 7 on 1 / 2 + LOS (`0x004163da` / `0x00416423`, `mov ebx, 7` at `0x0041643f`). The exe's mode-7 tail
(`ctrl[4] = 0x40`) already existed.

## 3. Findings

1. **The refdist bridge's zeros admit cars the original keeps out.** `FUN_00414c30`'s mode-7 arm needs `FUN_00442cc0(v) == 0`.
   The exe's bridge (`MASHED_REFDIST`, default ON) gives car 2 refdist 0 on 74 of the first 201 frames; the original's car 2
   has 0 on 0 / 890 pre-freeze calls. Car 2 then targets pool 4 from frame 16 (the original's car 2 instead collects pool 4
   by driving over it, and the exe's misses it by 0.065, RESULT_M3 finding 2). Filed U-9219.
2. Car 1 matches the original's mechanism (pool 0, refdist 0, from step frame 97) but for 25 returns instead of 49: the
   consumer returns 2 on pickup ticks 98..122, then 0 until car 1 collects pool 0 at tick 140 (`M4_ON.cb.tsv`); its refdist
   stays 0 throughout (192 zero frames before frame 400). [UNCERTAIN: which of the arm's conditions (|d| < 8, d.y window,
   heading > 0.866, LOS) stops it 18 ticks before the collection is not measured; the trace carries the inputs to decide.]
3. With the consumer on, three cars slow to accel 0x40 near pickups; (b) loses car 1 and car 3 as well. Not a transcription
   question (replay exact); it follows from finding 1 and from the exe's trajectories.
4. The prereg's G-M4-OFF row named the wrong base for the M3b arm (see §0); the inertness check is against `M3B_ON` and passes.

## 4. Next

D-11074's legs are all ported, each behind its own default-OFF knob. Open before any default flip (a user decision):
U-9219 (refdist zeros, the faithful `FUN_00442a60` route or a bridge fix), the car-2 opening-pickup miss, U-9215 (grid path),
U-9217 (original bump), U-9218 (car radius writer), the unported type-0x0A / 0x11 registrants.

## 5. Raw

`o_c30{a,b}.msd.c30.tsv` (+ aistep, provenance), `m4_replay_orig_pc{24,53}.txt`, `run_m4_replay.ps1`, `COLLATERAL_M4{,_stdout}.txt`,
`../d3_gatefire_20261008/M4_{CTRL,M3B,ON}.*` (`M4_ON.c30.tsv` = exe consumer trace, read with `re/tools/c30_report.py`).
