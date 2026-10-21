# RESULT soflip: MASHED_SUBSTEP_OUTER default ON, u9778's fiber form deleted - FLIPPED (2026-10-21)

Branch `work/soflip` from `fa50143b3` (work/substep merged). Deletion `59b2916df` (exe `683956fd`), then PREREG `PREREG_soflip.md`
with the seed (`27166911d`, exe `50558a73`), committed before any flip arm. Mariano's decision (2026-10-21). Builds with
`MASHED_NO_DEPLOY=1` (copies in `D:\MashedWT\soflip_data`), private roots `D:\MashedWT\soflip_root` / `soflip_root13`. Evidence in
`D:\MashedWT\soflip_data` (`MOVED_OFF_GIT.txt`, `arms\RUNLOG.txt` and `del\RUNLOG.txt`, copied here as `RUNLOG.txt` /
`RUNLOG_del.txt`).

## 1. Deletion (`59b2916df`)

Deleted: `Vehicle/SubstepInterleave.cpp` and its `exe_sources.rsp` line (fibers, `MASHED_SUBSTEP_INTERLEAVE`,
`MASHED_SUBSTEP_INTERLEAVE_ORDER`, `VehiclePhysics_InterleaveEnabled / InterleaveRun / IlInFiber / IlSwitchToMain`), `g_ilScratch`,
`IlYield` and its call in `StepCarImpl`, the snapshot's `g_ilScratch` field, and the `InterleaveEnabled()` arms of the A4 pre-phase
test and the player's step site in `TrackRenderer.cpp`.

Kept and renamed: the snapshot / restore with the per-slot `g_cSlot` scratch (`VehiclePhysics_OuterSnapshot / OuterRestore`,
`OuterSnap`), the AI-car handoff (`OuterCar` / `s_outer` / `OuterSameInput` / `s_outerMismatch` / `outerUndo`), and the io-mismatch
log line, now `[substep] outer: slot %d io differs ...`.

`MASHED_SUBSTEP_OUTER_ORDER=fiber`: kept as a control. `OuterRunToYield` / `VehiclePhysics_StepCarsOuter` drive the same phase
functions in the fiber's yield order with a per-car stage counter. No fiber API, none of the deleted code. `ORDER=car` kept too.

| arm (exe `683956fd`) | measured | expected |
|---|---|---|
| knob off B K0 / K1 / K26 | `7ab70c5f / 15262e4a / d1b4bc64` | same |
| knob off G-CTRL | `ce087436 / b0a2973b / c4f7a000` | same |
| `MASHED_SUBSTEP_OUTER=1` B K0 / K1 / K26 | `9bd86ffe / 433abcd2 / 1a1ff727` | same |
| `MASHED_SUBSTEP_OUTER=1` G-CTRL | `f78de789 / 43bbe0fb / 161b1a6a` | same |
| `ORDER=fiber` B K0 / `ORDER=car` B K0 | `9bd86ffe` / `7ab70c5f` | same |
| io mismatch lines (B_SX x3, B_SXF, H_SX, H_DEF) | 0 | 0 |

rva-lint (`scripts/lint_rva_bodies.py`, both builds): 25 findings, all allowlisted, NEW=0.

## 2. Gate table (exe `50558a73`, recipe F)

| gate | arm | measured | verdict |
|---|---|---|---|
| G-CTRL x2 | `H_DEF`, `H_DEF-2` | `f78de789 / 43bbe0fb / 161b1a6a`, both equal | as predicted |
| G-OPT | `H_NOSX` (`MASHED_NO_SUBSTEP_OUTER=1`) | `ce087436 / b0a2973b / c4f7a000` (= the fflip default) | PASS |
| G-NOFAITH | `H_NF` (`MASHED_NO_FAITH=1`) | `997f4117 / e6288905 / a63bdcfa` (= fflip's NO_FAITH) | as predicted |
| G-B | `B_DEF_K0/K1/K26` | `9bd86ffe / 433abcd2 / 1a1ff727`; flag_a368 0 / 0,1 / 0,2 | as predicted |
| G-B-OPT | `B_NOSX_K0/K1/K26` | `7ab70c5f / 15262e4a / d1b4bc64`; flag_a368 0 / 0,1 / 0,2 | as predicted |
| (b) | `vsum.py` (`vsum_b.txt`) | default A 3/3, B 3/3, C 3/3 (9/9); opt-out A 3/3, B 2/3 (car 2 FAIL, same values as fflip), C 3/3. No car PASS -> FAIL against fflip, B car 2 gained | PASS |
| io mismatch | `io differs` lines per arm log | 0 on B_DEF x3, B_NOSX x3, H_DEF, H_DEF-2, H_NOSX, H_NF | as predicted |
| G-D2 | `D2_DEF_1..3`, alone (`score_d2_def.txt`) | slip1500 0.1900 x3 / slip2000 0.2550 x3 / drvmed 1966.66, 1965.67, 1965.67, H1 398/400, 1632..1634 lines; all in band | PASS 3/3 |
| G-13 | `run13.py run` + `score` vs `cites_rres.json` (`tracks_def_score.txt`) | 39 frames, 877 rows, all cited, 0 RED, instrument gates PASS; score table identical to substep's `t13_SX_score.txt` | PASS |

Every arm ran once on the first attempt, no crash, no re-run, no orphan process. The 13 tracks ran in parallel with the B / G-CTRL
runner (separate roots, frame-count bound), D2 ran alone afterwards.

**Deviation from the predictions.** One, inside the band: D2 drvmed reads 1966.66 / 1965.67 / 1965.67 against substep's 1965.35 /
1965.67 / 1965.35. D2 is wall-clock bound (90 s, memory `d2-arms-are-wall-clock-bound`), and fflip's default read 1964.25..1966.66 on
an unchanged physics path. slip and H1 are identical. Everything else equals its PREREG value.

## 3. NO_FAITH

As stated in the PREREG: `MASHED_NO_FAITH=1` seeds neither `SUBSTEP_OUTER` nor `A4_ORDER`, so the outer driver never runs, and the
knob-off path is byte-equal after the deletion. Measured `997f4117 / e6288905 / a63bdcfa`, fflip's trio.

## 4. New default

FAITH stack (38 knobs) + u9774's five + `WCS_TRINORMAL` + `WCS_COMBINE_PC24` + `A5_RUBBER_LIVE` + `MODE_LIVE` + `SLOT_TYPE_LIVE` +
`SUBSTEP_OUTER`, on recipe F (`MASHED_ROUND_RULE=0 MASHED_GAME_MODE=10`). G-CTRL `f78de789 / 43bbe0fb / 161b1a6a`, B
`9bd86ffe / 433abcd2 / 1a1ff727`, (b) 9/9, D2 3/3, 13 tracks 877 rows 0 RED. Opt-out `MASHED_NO_SUBSTEP_OUTER=1` = the fflip default
`ce087436 / b0a2973b / c4f7a000`, B `7ab70c5f / 15262e4a / d1b4bc64`. `MASHED_NO_FAITH=1` `997f4117 / e6288905 / a63bdcfa`. The outer
driver needs the seeded `A4_ORDER`: with `MASHED_NO_A4_ORDER=1` no AI car enters it and the player steps per car.

Still open and out of scope here: the port-only snapshot / restore (substep section 5.1), U-9782 (retry counter), the round-tail
placement and the heading-assist order (substep section 5.3 / 5.4).
