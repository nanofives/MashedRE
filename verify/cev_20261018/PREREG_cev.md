# PREREG cev: the contact-event freezes (U-9745 / U-9185 / U-9186) (2026-10-18)

Branch `work/cev` from `race/first-frame-parity` `5740f549a` (default G-CTRL `b1f49e7a / b1be0054 / d3a20f4c`). Model
claude-opus-5-5, account2. Worktree build `MASHED_NO_DEPLOY=1`, exe `96e7f26d`, private root `D:\MashedWT\cev_root`.
Written before any knob arm. The arms already run (`B_DEFL_*`, `B_EL_*`) carry only knobs measured in d3bB2.

## 0. Measured before this PREREG (no knob introduced)

- G-NOOP: `B_DEFL_K0/K1/K26` step dumps `e332af0c / 01ad5975 / 772f9fb7` = u9747's flip default dumps (the car-car log
  writes a side file only).
- Original contact schedule (`ovon.py` on all 18 `verify/d3_regimes_20261013` captures, override-timer onsets, k = call
  index in the car's (b) window). An AI<->AI contact sets two AI timers on one frame (`FUN_00422ba0` -> `FUN_00417370`).
  - A (13/13): car 2 at 23 and 60 (player), cars 2+3 at 205.
  - B (3/3): car 2 at 23 and 60, cars 1+3 at 159 (next ones 264+, outside the window).
  - C (2/2): car 2 at 23 and 60. No AI<->AI contact in the window.
  So U-9745's open question is answered: the original has one AI<->AI contact in the A and B windows and none in C.
- Port with the event knob (`B_EL_*`): car 2 at 23 and 59, then cars 1+3 at 63 in every regime (extra), A 1+3 at 132,
  2+3 at 215; B 1+3 at 140, 2+3 at 219; C only the extra 63. (b): EL A 3/3, B 2/3 (car 2 `c0#` 44), C 0/3.
- Cause of the call-63 contact (`pos13.py`, `sbs.py` against o_reg_r13): car 3's position leaves the original's at call
  42; the original's car 3 runs `FUN_00414a70` return 1 on 39..41 and 2 on 44..57 (accel 0, brake 255, speed
  2756 -> 1734), the default never calls `FUN_00414a70` (c3k44, U-9684) and car 3 keeps full throttle (2756 -> 3089),
  closing the 1-3 distance to 0.976 (original 1.759) at call 63.

## 1. Hypothesis

H1: with the countdown spawn now default (AI progress at GO = the original's, u9747 G-PROG), the ported
`FUN_00414a70` chain (`MASHED_PATHPROG` + `MASHED_TGT3`, stack TGT) picks the original's car ahead for car 3 and
gives the original's brake schedule, and with it the extra 1+3 contact at 63 goes away. c3k44 measured TGT before
u9747 (car 2 ahead of car 3 until call 52, return 2 from 53).

## 2. Arms (`run_arms.py`, 1200 frames, K0 / K1 / K26)

| arm | knobs on the default |
|---|---|
| `TGTT` | `MASHED_PATHPROG`, `MASHED_TGT3` + the TGT3 / PATHPROG traces |
| `TGTEL` | TGT + `MASHED_AI_CONTACT_EVENT` + the car-car / event logs |
| `TGTE` | TGT + `MASHED_AI_CONTACT_EVENT` (no logs; the scored dump) |
| `TGT` | TGT alone (no traces; the scored dump) |

Logs are side files; a traced arm's step dump must equal its untraced twin's (checked, else the traced one is not used).

## 3. Gates

| gate | rule |
|---|---|
| G-BRAKE | K1 (regime B) car 3 `ret14a70` (TGT3 trace / step dump): 1 on calls 39..41, 2 on 44..57 (o_reg_r13), within +-1 call |
| G-C63 | `TGTEL` K0 / K1 / K26: no AI<->AI contact at calls 60..70 |
| G-SCHED | `TGTEL` contact schedule inside the window equals the original's per regime (A: 2+3 at 205, B: 1+3 at 159, C: none; player contacts 23 and 59/60), +-3 calls |
| G-B | (b) on `ai_ctrl_window --check-regime`: no car that passes on the default (A cars 2, 3; B cars 1, 2) turns to FAIL |

A default proposal needs G-B plus D2 3/3 and 13 tracks 0 RED (default-ON rule), run only if G-B passes. A partial
result (G-BRAKE pass, G-SCHED fail) is reported with the first differing contact and its cause, no default proposal.

## 4. Out of scope

The 50-tick override-timer offset (1390 vs 1440, U-9745 tail) is noted, not fixed here, unless G-SCHED passes and it
is the remaining difference. U-9185 / U-9186's over-speed metric is not re-measured unless G-B passes.

## Amendment 1 (2026-10-18, after the section-2 arms, before these)

Section-2 result: TGT removes the extra 1+3 contact at 63 in every regime (B: 1+3 at 158, original 159), but car 2's
`FUN_00414a70` returns 1 from window call 0 (original 0 until 22), so car 2 brakes from the start and the player
contacts at 23 / 60 disappear; (b) TGT A 0/3 B 0/3 C 0/3, TGTE B 1/3 (car 3). u9747's G-ORDER (car 2 return 0 at call
1) was measured under `MASHED_AI_TICK_FIRST` (`S` + countdown spawn). New arms, same gates (G-BRAKE, G-C63, G-SCHED,
G-B), plus G-ORDER2: car 2 `ret14a70` = 0 on window calls 0..21 (o_reg_r13).

| arm | knobs on the default |
|---|---|
| `KT` | TGT + `MASHED_AI_TICK_FIRST` |
| `KTE` | KT + `MASHED_AI_CONTACT_EVENT` |
| `KTEL` | KTE + car-car / event / TGT3 logs |
| `S` | d3bB2's `S` on today's default (adds `MASHED_POS_MODEL`, `MASHED_ROUND_GRID(_ORDER)`) |

## Amendment 2 (2026-10-18, before any MASHED_PATHPROG_LATE arm)

Amendment-1 result: KT / KTE leave car 2 braking from call 0 (A 0/3, B 0-1/3); `S` on today's default gives the
original's player contacts (23, 59), no call-63 contact, B 1+3 at 169 (original 159), C none; (b) 0/3 x3, mostly
`c0#` over. Under `S`, cars 2 and 3's `FUN_00414a70` returns run one call behind the original's from call 39.
Cause measured (`progcmp2.py`, `o_wobj2` = a regime-B capture of the same recipe with the `FUN_00414a70` entry probe,
against `B_ST_K1` = `S` + TGT3 trace, step dump identical to `B_S_K1`): the port's own progress `0x008a96e8` at window
call k equals the original's at call k-1 for every AI car from k = 1 (k = 1: 29.72771 vs 29.72976; k = 2: 29.72976 =
original k = 1). The port runs `PathProg_FrameTick` in `UpdateRace`, before the opponents' physics; the original runs
`FUN_00425a40` (`0x0040fc6c`) before `FUN_0040da50` (`0x0040fc75`).

New knob `MASHED_PATHPROG_LATE` (default OFF): the wholesale tick after every car's physics, where arm R's shadow tick
already runs. Arms on the default: `SPL` = `S` + the knob; `SPLT` = + traces; `SPLE` = `SPL` without
`MASHED_AI_CONTACT_EVENT`; `TGTPL` = TGT + the knob (no tick first).

| gate | rule |
|---|---|
| G-NOOP2 | knob off on the new exe: `B_DEFL_K0/K1/K26` and `B_S_K1` step dumps unchanged (`e332af0c / 01ad5975 / 772f9fb7`, `5c5acdc3`) |
| G-LAG | `SPLT` K1: port own progress at call k = the original's at call k (o_wobj2), abs diff < 1e-3, k = 1..38, all AI cars |
| G-BRAKE, G-ORDER2, G-C63, G-SCHED, G-B | as section 3 / amendment 1, on `SPL` (scored) and `SPLT` (logs, if its dump equals `SPL`'s) |
