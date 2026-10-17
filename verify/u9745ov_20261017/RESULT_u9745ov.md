# RESULT u9745ov: car 2's steer during the second override (2026-10-17)

Branch `work/u9745ov` from `work/u9749` `4e98d54b5` (player heading assist default ON; merged by the orchestrator as
`30509b708`). Model claude-opus-5-5, account2. Exes (`MASHED_NO_DEPLOY=1`, `D:\MashedWT\u9749_data\exe`): `f1_2ca9a02b`
(the flip, used for the slot-2 dump), `c1_e1b6a390` (= `ad4eadc49`, the knob). PREREG `PREREG_u9745ov.md`, committed in
`ad4eadc49` before any knob arm. Original references: `o_c2r1` (d3bB2) and the `d3_regimes_20261013` captures; no new capture.

## 0. Verdict

The residual was not in the override. Two things:

1. **The reference was in the other roll outcome.** `FUN_004177b0`'s one-shot band-0 roll (`0x00417c57..0x00417c8a`, 20% for row 2
   from `0x005f30a0`) sets the global slow-line flag `DAT_0089a368`, which moves every AI car's lookahead. `o_c2r1` / `o_pw1` lost
   it; `o_reg_r13` (the regime-B reference), r15, r18 won it (1), r19 (regime-C reference), r20 got 2. The port's K1 wins (1), K26
   gets 2, K0 loses: the port's regimes are the original's roll outcomes.
2. **The port's roll fired one call early** (call 60 against the original's 61), because `FUN_0040fc00` adds the budget to
   `DAT_007f0ff4` / `DAT_007f0ff8` at its end (`0x0040fe4f..0x0040fe64`), after `FUN_00425a40` (`0x0040fc6c`), and the port added it
   before the AI tick (U-9750).

Ported behind **`MASHED_AI_CLOCK_LATE=1` (default OFF)**. With it the roll fires at call 61 in every regime, and car 2's commands
equal `o_reg_r13`'s on calls 57..92 (first divergence after call 56 moves from 60 to 93). On the plain default it keeps the (b) pass
set (A 2/3, B 2/3, C 0/3), D2 3/3, and the 13-track score file is identical. **Recommendation: default ON** (faithful from the
bytes, passes the rule; user decision).

## 1. How it was found

- `B_SAR2F_K1` (slot-2 MSD1 dump on the flipped default) against `o_c2r1.msd`, `fieldtrace.py`: through the second override car 2's
  heading is within 6e-5, yaw rate within 1.4e-4, speed 1340.51 / 1341.14. Physics is not the cause.
- `datarefs_89a4fc.txt`: the override timer is read and written only by `FUN_00417370`, `FUN_004173b0` and `FUN_00418560`'s tail
  (`0x004187d5..0x004187f0`). `decomp_418560_417370.txt`: the tail runs after `FUN_00416250` (line 127) and `FUN_00417640` and
  overwrites the control bytes with the held ones (`0x0089a50c..0x0089a518`). `FUN_00416250` does not read the timer.
- `sbs.py` with AI-state columns: at call 60 the port sets `flag_a368` and car 2's lookahead jumps (x -0.580 against -1.317,
  curvature 13.4 against 2.36); `o_c2r1` keeps 0.
- Roll outcome per capture (car 2 window, `flag_a368`): `o_reg_r13` / r15 / r18 first 1 at call 61, r19 / r20 first 2 at call 61,
  13 others and `o_c2r1` / `o_pw1` 0 on all 220 calls; port `B_DEF_K1` / `B_P_K1` / `B_SA_K1` 1 at call 60, K26 2 at call 60, K0 0.
- Against `o_reg_r13`, the port's car 2 steer from call 60 is the original's one call earlier (port 6, 4, 2, -7 on 60..63; original
  9, 6, 4, 2, -7 on 60..64) and equal from 64.
- `disasm_40fc00_calls.txt` / `disasm_0ff8_writers.txt`: the order in `FUN_0040fc00` (store `0x0040fc63`, `FUN_00425a40`
  `0x0040fc6c`, ... add `0x0040fe5e` / `0x0040fe64`, RET `0x0040fe6c`). `datarefs_7f0ff8.txt`: the readers.

## 2. Code (`ad4eadc49`)

`Ai_AdvanceClock` stores `DAT_007f1008` before the tick; under `MASHED_AI_CLOCK_LATE` the `DAT_007f0ff4` / `DAT_007f0ff8` add moves to
`Ai_AdvanceClockLate`, called at the end of the race frame (after the AI tick, the physics, the drain).

## 3. Gates

| gate | measured | verdict |
|---|---|---|
| G-NOOP | knob off on `e1b6a390`: `B_SA_K1` `c517e79f`, `B_DEF_K0` `c965c713`, `B_DEF_K26` `e3807ba2` (= u9749's knob-alone dumps, the new default), `H_DEF` `a34714b4 / 0f8ef6df / 852a255d` | PASS |
| G-ROLL | `B_CL_K0` 0 on all calls; `B_CL_K1` first 1 at call 61; `B_CL_K26` first 2 at call 61 (`B_SACL_*` the same) | PASS |
| G-STEER2 | `B_SACL_K1` car 2 against `o_reg_r13`: equal on calls 57..92 (`SBS_SACL_K1_vs_r13_car2.txt`); call 56 (7 / 8) differs with and without the knob | PASS on 57..80; the registered range started at 56, which predates the roll |
| window | command mismatches over 220 calls against `o_reg_r13` (`WINDOW_MISMATCH_vs_r13.txt`): car 2 120 -> 116 (first after 56: 60 -> 93); car 1 110 -> 106; car 3 149 -> 149 | reported |

## 4. D3 (b) (`arms/b/SCORE_B.txt`)

| arm | A | B | C | note |
|---|---|---|---|---|
| DEF (new default) | 2/3 | 2/3 | 0/3 | u9749's `P` arm |
| **CL** | **2/3** | **2/3** | 0/3 | same pass set; B car 2 items `c0#` 38 `c1#` 56 `steer#` 93 (DEF `c1#` 55 `steer#` 89) |
| SA (S + A4 order) | 0/3 | 0/3 | 0/3 | |
| SACL | 0/3 | 0/3 | 0/3 | B car 2 `c1#` 63 `steer#` 100 (SA 64 / 101) |

No car that passes on the default turns to FAIL.

## 5. G-CTRL, D2, 13 tracks (knob alone)

| item | measured |
|---|---|
| G-CTRL `H_CL` | `dd822489 / 35f5fea0 / da12bbe0` (gates 54713 lines, ptrace 27356, against 53993 / 26996 on the default: the round outcomes move) |
| D2 `D2_CL_1..3` (each alone) | 1628 / 1627 / 1628 lines; slip 0.1890 x3, 0.2476 / 0.2480 / 0.2476; driving-median 1912.01 / 1909.37 / 1912.01; H1 398/400 x3. 3/3 in bounds; `D2_CL_2` = the default's `D2_P_1` (`0413e529`) |
| 13 tracks | 39 frames, 866 rows, 0 RED; score file identical to the default's |

## 6. Other finding (U-9751)

The `DAT_007f0ff8` reader list includes A5 (`FUN_0046ddb0`, `0x0046e0a9`): the grip ramp `local_6c` is 1 until 20 s of race time
(60000 at `_DAT_005cea7c`) and falls to 0 at 30 s (`-1/30000` at `_DAT_005cea74`). The exe's A5 reads `g_raceTimer`
(`ForceIntegratorStubs.cpp:23`), which nothing in the exe writes, so the ramp never engages. Not measured here.

## 7. Tracker transaction (re-classify, no C-level change; `apply_trackers.py`)

New U-9750 (clock order) and U-9751 (A5 race-timer stub); U-9745 note; hooks.csv notes on `0040fc00`, `004177b0`, `0046ddb0`;
one CHANGELOG line. U-9749 was the highest id on `4e98d54b5` and on `race/first-frame-parity` at `30509b708`.

## 8. User decisions needed

1. `MASHED_AI_CLOCK_LATE` default: recommended ON (opt-out at the flip).

## 9. Follow-ups

- Car 2 against `o_reg_r13` from call 93 (`WINDOW_MISMATCH_vs_r13.txt`), and cars 1 / 3 on the same stack.
- U-9751: point A5 at `DAT_007f0ff8` behind a knob; D2's 90 s runs pass 20 s, so D2 must be re-scored.
- Score (b) for regime B against the regime-B references only (`o_reg_r13` won the roll; `o_c2r1` / `o_pw1` did not), so a
  side-by-side never mixes roll outcomes.

## 10. Provenance

RUNLOGs `arms/RUNLOG.txt` (slot-2 dump arms on `2ca9a02b`), `arms/b`, `arms/h`, `arms/d2`, every line read back for its knobs.
In git: PREREG, this RESULT, `apply_trackers.py`, the decomp / disasm / datarefs files, `SBS_SACL_K1_vs_r13_car2.txt`,
`WINDOW_MISMATCH_vs_r13.txt`, `tracks_cl_scores_diff.txt`, `B_SACL_K1.step.csv`, `B_CL_K1.step.csv`, the D2 `motion_diag` logs
(force-added). Everything else on `D:\MashedWT\u9749_data\verify\u9745ov` (`MOVED_OFF_GIT.txt`).
