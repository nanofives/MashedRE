# RESULT cev: the contact-event freezes (U-9745 / U-9185 / U-9186) (2026-10-18)

Branch `work/cev` from `race/first-frame-parity` `5740f549a` (default G-CTRL `b1f49e7a / b1be0054 / d3a20f4c`). Model
claude-opus-5-5, account2. PREREG `PREREG_cev.md` (`e4f78ecfc`), amendment 1 (`a492200f2`), amendment 2 with the knob
(`43e2cd87c`), each committed before the arms it governs. Exes (`MASHED_NO_DEPLOY=1`): base `96e7f26d`, final `42345f2c`.
Private root `D:\MashedWT\cev_root`. Original references: the 18 `verify/d3_regimes_20261013` captures, `o_wobj2`
(`verify/d3_modes37_20261009`, regime B, `FUN_00414a70` entry probe).

## 0. Verdict

1. **The original does have AI<->AI contacts in the (b) windows**, at fixed calls per regime (`ovon.py`, override-timer
   onsets, both AI timers set on one frame by `FUN_00422ba0` -> `FUN_00417370`):
   A (13/13) car 2 at 23 and 60 (player), cars 2+3 at 205. B (3/3) car 2 at 23 and 60, cars 1+3 at 159. C (2/2) car 2 at
   23 and 60 only. U-9745's open question is answered.
2. **The port's first extra AI<->AI contact (cars 1+3 at call 63, every regime, under `MASHED_AI_CONTACT_EVENT`) is car 3's
   missing `FUN_00414a70` brake chain** (U-9684): the original's car 3 returns 1 on 39..41 and 2 on 44..57 (speed
   2756 -> 1734); the default never calls `FUN_00414a70`, car 3 keeps full throttle (-> 3089) and closes the 1-3 distance
   to 0.976 (original 1.759) at call 63.
3. **Under the ported chain (`S`), the AI cars' path progress was one tick stale.** The port's `0x008a96e8` at window call
   k equals the original's at k-1 for every AI car (`progcmp2.py` against `o_wobj2`). `PathProg_FrameTick` ran in
   `UpdateRace`, before the opponents' physics; the original runs `FUN_00425a40` (`0x0040fc6c`) before `FUN_0040da50`
   (`0x0040fc75`). New knob **`MASHED_PATHPROG_LATE`** (default OFF; no-op without `MASHED_PATHPROG`) runs the tick after
   every car's physics. With it the progress is exact at k = 1, 2 and within 1e-4 for every car to call 22, and all three
   cars' `FUN_00414a70` returns equal the original's through call 59 (car 3: 1 on 39..41, 2 from 44; car 2: 0 to 22, 1
   from 23, 1 on 44..45). The first `FUN_00414a70` divergence moves from call 39 to 134..190 in every regime.
4. **(b) does not pass**: `S` + the knob (`SPL`) is 0/3 in every regime, so no default change is proposed. The contact
   schedule is the original's in C, the call-63 contact is gone everywhere, B has 1+3 at 169 (original 159) plus an extra
   2+3 at 189, A misses the original's 2+3 at 205. The steering residual that feeds the failures starts before any AI<->AI
   contact: car 1's lookahead node flips at call 21 (`look_z` -7.653 / -9.199) with the position equal to 1e-3 and the
   speed 0.3 lower (1763.487 / 1763.197); car 2's progress drifts after its player contact at 23 (up to 0.0018 by call 38, the
   U-9746 post-contact speed).

## 1. Gates

| gate | measured | verdict |
|---|---|---|
| G-NOOP | `B_DEFL_K0/K1/K26` `e332af0c / 01ad5975 / 772f9fb7` = u9747's flip default | PASS |
| G-NOOP2 | knob off on `42345f2c`: same three, `B_S_K1` `5c5acdc3`, `H_DEF` `b1f49e7a / b1be0054 / d3a20f4c` | PASS |
| G-LAG | `SPLT` K1 vs o_wobj2: exact at k = 1, 2; all cars < 1e-4 to call 22; cars 1, 3 < 4e-4 to 38; car 2 up to 0.0018 on 23..38 (after its contact at 23) | PASS for the lag; car 2's post-contact drift is U-9746 |
| G-BRAKE | `SPL` K1 car 3 `ret14a70` 1 on 39..41, 2 on 44..57 (o_reg_r13 the same) | PASS (exact) |
| G-ORDER2 | `SPL` K1 car 2 `ret14a70` 0 on 0..22, 1 from 23 | PASS |
| G-C63 | no AI<->AI contact on calls 60..70 in `SPL` K0 / K1 / K26 (the port's `EL`: 1+3 at 63 in all three) | PASS |
| G-SCHED | `SPL`: C = original; B 1+3 at 169 (159) + extra 2+3 at 189; A none (2+3 at 205) | FAIL (A, B) |
| G-B | `SPL` A 0/3, B 0/3, C 0/3 (default A 2/3, B 2/3) | FAIL |

## 2. D3 (b) (`arms`, `arms2`)

| arm | A | B | C | AI<->AI contacts in the window |
|---|---|---|---|---|
| DEF (`DEFL`) | 2/3 | 2/3 (cars 1, 2) | 0/3 | (no event) |
| `EL` (event) | 3/3 | 2/3 (car 2 `c0#` 44) | 0/3 | 1+3 at 63 in all; A 1+3 132, 2+3 215; B 1+3 140, 2+3 219 |
| `TGT` / `TGTE` | 0/3 / 0/3 | 0/3 / 1/3 | 0/3 / 0/3 | car 2 returns 1 from call 0 (brakes), player contacts lost |
| `KT` / `KTE` (+ tick first) | 0/3 / 0/3 | 1/3 / 0/3 | 0/3 / 0/3 | car 2 the same |
| `S` | 0/3 | 0/3 | 0/3 | 23, 59; B 1+3 at 169; returns one call late from 39 |
| `SPL` (`S` + `MASHED_PATHPROG_LATE`) | 0/3 | 0/3 | 0/3 | 23, 59; B 1+3 169, 2+3 189; C = original |
| `SPLE` (`SPL` without the event) | 0/3 | 0/3 | 0/3 | (no event) |
| `TGTPL` (TGT + the knob) | 0/3 | 0/3 | 0/3 | (no event) |

`EL` A 3/3 is the event knob alone on today's default (d3bB2 measured it on an older default). It is not proposed: B and C
do not move, and its schedule carries the extra call-63 contact in every regime.

Traced-arm check: `TGTEL_K1` differs from `TGTE_K1` (`4a6bd1ef` / `5ca95fdc`) and is not scored; every other traced or
logged arm equals its plain twin.

## 3. Code (`43e2cd87c`)

- `Race/PathProgStep.cpp/.h` `PathProg_Late()`: `MASHED_PATHPROG_LATE=1`.
- `TrackRenderer.cpp`: `UpdateRace` skips `PathProg_FrameTick` under the knob; the end of `UpdateCar` runs it after the
  opponents' physics, before the refdist producer (the place arm R's shadow tick already has).

## 4. Tools

`ovon.py` (override-timer onsets per car, window call index), `pos13.py` (per-call position deltas and pair distance),
`progcmp2.py` (own progress at each `FUN_00414a70` call, original wobj_ins joined on the AI clock vs port TGT3 trace),
`c0diff.py` (where the steer bytes differ, cumulative `c0#` / `c1#`).

## 5. Tracker transaction (re-classify, no C-level change; `apply_trackers.py`)

U-9745 note (the original's schedule, the call-63 cause, the knob, `SPL` results); U-9684 note (car 3's brake chain exact
under `SPL`); hooks.csv notes on `00408610`, `00414a70`; one CHANGELOG line.

## 6. Follow-ups

- The remaining (b) failures under `SPL` start before any AI<->AI contact: car 1's lookahead node flip at call 21 on a
  0.3 speed difference (physics residual, D2 class), and car 2's post-contact progress (U-9746).
- `MASHED_PATHPROG_LATE` belongs with `MASHED_PATHPROG` in any future default proposal of the ported chain; on its own it
  changes nothing.
- The 1390 / 1440 override-timer sample offset (U-9745 tail) is unchanged.
