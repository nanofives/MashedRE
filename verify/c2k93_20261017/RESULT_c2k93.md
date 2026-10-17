# RESULT c2k93: car 2 from window call 93 against o_reg_r13 (2026-10-17)

Branch `work/c2k93` from `work/u9745ov` `bef7c8882`. Model claude-opus-5-5, account2. No new code: exe `c1_e1b6a390` (the u9745ov
build), existing knobs only. PREREG `PREREG_c2k93.md` (`4b78db88d`, committed before the scoring arms; section 1 lists the
exploration already run). Reference: `verify/d3_regimes_20261013/o_reg_r13.msd.aistep.csv` (regime B, won the slow-line roll, as
the port's K1 does).

## 0. Verdict

- **The split at call 93 is the lookahead's square root.** The original's spline lookahead (`FUN_00443dc0`) uses the RW LUT
  bodies; the port's default uses exact sqrt. Fed the original's own inputs, the LUT form gives o_reg_r13's target at 93..99
  (e.g. (-1.002, -11.643) at 93), the exact form gives the port's (-1.057, -12.022). The knob exists: `MASHED_AI_LUT=5` (U-9752).
- **Live, the LUT form alone does not hold call 93**: the choice sits inside the LUT/exact difference, and by then car 2's
  position is 2.2e-3 off. That drift starts at call 33 (the first A4 frame after the contact coast) and comes from `+0x9e4`
  not being refreshed after A4 (U-9212): `MASHED_D2LAT_REFRESH` cuts it 5x (0.45e-3 at 92). With both, car 2's command
  mismatches against o_reg_r13 fall from 116 to 96 of 220 (`WINDOW_MISMATCH_vs_r13.txt`).
- **The refresh cannot be a default yet:** it pins the D2 player (H1 9/400, 6/6 runs), as d2lateral measured.
- **`MASHED_AI_LUT=5` is now a default-ON candidate:** on the current default + `MASHED_AI_CLOCK_LATE` it keeps the (b) pass set
  (A 2/3, B 2/3, C 0/3; the A car 2 / car 3 trade d3b and dflip saw is gone), D2 3/3, 13 tracks 0 RED. **Recommendation: flip it
  together with or after `MASHED_AI_CLOCK_LATE`** (user decision).

## 1. Evidence

| item | measured |
|---|---|
| SACL car 2 vs r13 | equal on 57..92; at 93 target (-1.056, -12.016) against (-1.002, -11.643); both then cycle over three consecutive points, the port's set one point on |
| lookahead replay, mode-0 rows of the window (`LOOK_REPLAY_FLIPS_MODE0.txt`) | flips (> 0.01) LUT / exact: car 1 1 / 45, 1 / 38, 3 / 48; car 2 0 / 4, 4 / 24, 6 / 9; car 3 44 / 74, 53 / 66, 15 / 44 (r1 / r13 / r19) |
| replay at r13 car 2 calls 93..99 | LUT = original to 3 decimals on every call; exact = the port's points on 93, 94, 95, 98 |
| car 2 \|position\| difference to r13 (x1e-3) at calls 33 / 59 / 92 | SACL5 0.17 / 1.09 / 2.18; SACLR 0.10 / 0.23 / 0.45 |
| car 2 command mismatches vs r13 (220 calls) | SACL 116, SACL5 112, SACLR 114, SACLR5 96 |

With refresh + LUT the remaining car 2 differences are steer +-1 at the same target and speed (calls 79..91) and target flips at
54, 95, 96, 101, ... with positions 1e-3 apart.

## 2. Scoring (current default + `MASHED_AI_CLOCK_LATE`)

| arm | A | B | C | D2 (3 runs, each alone) | 13 tracks | G-CTRL |
|---|---|---|---|---|---|---|
| CL (u9745ov) | 2/3 | 2/3 | 0/3 | 3/3 | 866, 0 RED | `dd822489 / 35f5fea0 / da12bbe0` |
| CL5 | 2/3 | 2/3 | 0/3 | **3/3** (0.1890; 0.2476 / 0.2476 / 0.2480; 1912.01 / 1912.01 / 1909.37; H1 398/400) | **869, 0 RED** (+1 `C27-CAR-PART-OFFSET` on Roundabout 1091, Storm 928, Training 884, cited) | `262504cc / 39fc6210 / 6797ffe5` |
| CLR | 2/3 | 2/3 | 0/3 | **0/3**: slip 2000-2600 0.2549, driving-median 1812.98, H1 9/400 (wall pin) | not run | not run |
| CLR5 | 2/3 | 2/3 | 0/3 | **0/3**, same numbers | not run | not run |

Regime-B car 2 fails in every arm by one on `c0#` (38 against [13, 37]); `c1#` / `steer#` 56 / 93 (CL), 51 / 88 (CL5), 53 / 90
(CLR), 55 / 92 (CLR5).

## 3. Tracker transaction (re-classify, no C-level change; `apply_trackers.py`)

New U-9752 (lookahead LUT default), U-9753 (car 3 lookahead state run from call 44); U-9212 note (refresh: drift 5x smaller, D2
still pinned); one CHANGELOG line.

## 4. User decisions needed

1. `MASHED_AI_LUT=5` default: recommended ON, with or after `MASHED_AI_CLOCK_LATE` (that is the base it was scored on).
2. `MASHED_AI_CLOCK_LATE` default (from u9745ov): recommended ON.

### 4.1 Flipped (user decision 2026-10-17)

Both default ON: `MASHED_AI_CLOCK_LATE` (opt-out `MASHED_NO_AI_CLOCK_LATE=1`) and `MASHED_AI_LUT=5` (opt-out `MASHED_NO_AI_LUT=1`;
`MASHED_AI_LUT=<mask>` still overrides). Exe `c832e81d`, `arms/flip/RUNLOG.txt`: G-CTRL default `262504cc / 39fc6210 / 6797ffe5` x2
(= `H_CL5`); default B dumps K0 / K1 / K26 `f410560e / f9a1f85c / cdfe437f` (= `B_CL5_*`); both opt-outs `a34714b4 / 0f8ef6df /
852a255d` (the previous default); `MASHED_NO_AI_LUT=1` alone `dd822489 / 35f5fea0 / da12bbe0` (= `H_CL`). The (b), D2 and 13-track
numbers of the CL5 row of section 2 are the new default's. U-9750 and U-9752 RESOLVED (`apply_trackers_flip.py`).

## 5. Follow-ups

- U-9212's D2 chain (refresh + spawn yaw) is what stands between the port and car 2's position tracking past call 93.
- U-9753: car 3's lookahead state on the original around call 44.
- Car 2's +-1 steer at equal target and speed (calls 79..91): the heading error term of `FUN_00416250` at 1e-3 position
  difference.

## 6. Provenance

`arms/RUNLOG.txt` (exploration), `arms/b`, `arms/d2`, `arms/h` RUNLOGs; scores `arms/b/SCORE_B.txt`,
`arms/d2/SCORE_D2_c2k93.txt`, `arms/d2/SCORE_D2_c2k93_cl5.txt`, `tracks_cl5_scores_diff.txt`. 13-track outputs on
`D:\MashedWT\u9749_data\tracks\cl5`.
