# RESULT d2ref: the D2 refresh chain on the current default (2026-10-17)

Branch `work/d2ref` from `race/first-frame-parity` `6242c84b6` (default G-CTRL `262504cc / 39fc6210 / 6797ffe5`). Model
claude-opus-5-5, account2. PREREG `PREREG_d2ref.md` (`e530fa3b7`) and amendment 1 (`d656f5948`), each committed before its runs.
Exes (`MASHED_NO_DEPLOY=1`, `D:\MashedWT\d2ref_data\exe`): `b_4c841df0` (base), `t_8c510943` (+ `MASHED_A5_RACE_TIMER`).
Private roots `D:\MashedWT\d2ref_root` / `_root2` / `_root13`. D2 reference `orig_solo3.msd` (slip 0.1937 / 0.2498,
driving-median 1943.57 = the bounds' centres).

## 0. Verdict

**The faithful stack passes D2 for the first time:** the original's speed refresh (`MASHED_D2LAT_REFRESH`, `0x004710a1..0x004710da`
with `0x004c3ac0`'s LUT, U-9212) + its spawn frame (`MASHED_GRID_DIR`, `FUN_00408b00`, U-9720) + its A5 race timer (new
`MASHED_A5_RACE_TIMER`, `0x0046e0a9`, U-9751). On 3/3 runs: slip 0.1939..0.1942 / 0.2545, driving-median 1960.31..1963.27,
H1 398/400. The 13-track replay is 869 rows, 0 RED. **But (b) trades regime-B car 3 for car 2** (car 2 passes regime B for the
first time; car 3 fails `c1#` 52 [17, 46]), so under the rule the stack stays default OFF. **User decision.**

## 1. D2 (each run alone, 90 s, `score_all.py d2`; `arms/d2/SCORE_D2_d2ref.txt`, `SCORE_D2_d2ref_a1.txt`)

| arm | knobs | slip 1500-2000 | slip 2000-2600 | driving-median | H1 | verdict |
|---|---|---|---|---|---|---|
| DEF | none | 0.1890 x3 | 0.2476..0.2483 | 1912.01..1914.84 | 398 | 3/3 |
| R | refresh | 0.1955 | 0.2549 OUT | 1812.98 OUT | 9 | 0/3 (pin) |
| G | grid | 0.2007 OUT | 0.2634 OUT | 1758.25 OUT | 399 | 0/3 |
| RG | refresh + grid | 0.1917 | **0.2557 OUT** | 1976.01..1981.23 | 398 | 0/3 |
| RY | refresh + exact yaw (diagnostic) | 0.1952 | **0.2551 OUT** | 1944.41..1945.37 | 398 | 0/3 |
| RGB | RG + fix B | 0.1981 OUT | 0.2502 | 1851.82 OUT | 398 | 0/3 |
| T5 | race timer | 0.1864 OUT | 0.2470 | 1931.32..1931.34 | 398 | 0/3 |
| **RGT** | **refresh + grid + race timer** | **0.1939 / 0.1942 / 0.1941** | **0.2545 x3** | **1960.31 / 1963.27 / 1962.62** | **398** | **3/3** |
| RYT | RY + race timer | 0.1956 | 0.2550..0.2552 OUT | 1931.91..1936.05 | 398 | 0/3 |

All runs 1626..1635 `motion_diag` lines. The race timer advances 50 per step in the D2 recipe (log: 0, 29950, 59950 at steps 0 /
600 / 1200), so the original's grip ramp (`local_6c` from 1 at 20 s to 0 at 30 s, `_DAT_005cea7c` / `_DAT_005cea74`) covers the
last ~430 of ~1630 steps; the port's stub had kept it at 1.

## 2. RGX (= RGT without the diagnostic log) on the other gates

| gate | measured |
|---|---|
| G-NOOP (knobs off, same exe) | `H_DEF` `262504cc / 39fc6210 / 6797ffe5`; `B_DEF_K0/K1/K26` `f410560e / f9a1f85c / cdfe437f` (= the default) |
| G-CTRL `H_RGX` | `1fbc77ae / 29a08810 / d3a20f4c` |
| D3 (b) (`arms/b/SCORE_B.txt`) | A 2/3 (cars 2, 3, as the default); **B 2/3 = cars 1, 2** (default cars 1, 3): car 2 passes, **car 3 fails `c1#` 52 [17, 46]**; C 0/3 |
| 13 tracks (`tracks_rgx_scores_diff.txt`) | 39 frames, 869 rows, 0 RED; four frames trade one cited row (Roundabout 1091, Storm 928, Dump 735, Training 924) |

## 3. Code (`d656f5948`)

`MASHED_A5_RACE_TIMER=1` (default OFF): `VehiclePhysics_StepCar` copies the exe's `DAT_007f0ff8` into A5's `g_raceTimer` before
each step; `MASHED_A5_RACE_TIMER_LOG=1` logs it every 600 slot-0 steps. Since c2k93 the exe's `DAT_007f0ff8` follows the
original's order (`MASHED_AI_CLOCK_LATE`).

## 4. Tracker transaction (re-classify, no C-level change; `apply_trackers.py`)

Notes on U-9212 (the stack and its gates), U-9720 (the grid member), U-9751 (ported behind the knob, measured); one CHANGELOG
line. No new U-id.

## 5. User decisions needed

1. Flip the stack (`MASHED_D2LAT_REFRESH` + `MASHED_GRID_DIR` + `MASHED_A5_RACE_TIMER`) default ON? It is the original's code on
   all three counts, passes D2 and the 13 tracks, and brings regime-B car 2 into its envelope, but regime-B car 3 leaves its
   envelope (one item, `c1#` 52 against 46). The rule says OFF.

## 6. Follow-ups

- Regime-B car 3 under the stack: U-9753 (car 3's lookahead state from call 44) is the open item on that car.
- Re-score D2 / (b) after any flip; the D2 margin on slip 2000-2600 is 0.0004 under the bound.

## 7. Provenance

RUNLOGs `arms/d2`, `arms/b`, `arms/h`; scores in `arms/d2`, `arms/b`; 13-track outputs on `D:\MashedWT\d2ref_data\tracks\rgx`.
Cited D2 logs in git: `D2_RGT_1..3.motion_diag.log` (force-added). Everything else on `D:\MashedWT\d2ref_data\verify`
(`MOVED_OFF_GIT.txt`).
