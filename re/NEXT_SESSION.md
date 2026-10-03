# Next session kickoff

Updated 2026-10-02 at the close of the **D3 re-baseline + modes-3/7 test** session.
Branch `race/first-frame-parity`. Nothing is pushed. **No game code was edited this
session**; the only code changes are two analysis tools.

## The headline

> **D3 does NOT close, and the 2026-09-29 closure path is REFUTED by its own
> pre-registered gate.** Porting behaviour modes 3 and 7 would not close AI criterion (b),
> so **the port was not written**. (b) is now **decomposed into four measured carriers**
> rather than attributed to one cause, and the **dual-copy hypothesis turned out to be the
> same hypothesis**, already measured.

Read, in order and none of it long:
[`verify/d3_rebase_20261002/RESULT_STEP1.md`](../verify/d3_rebase_20261002/RESULT_STEP1.md),
[`verify/d3_modes37_20261002/RESULT_STEP2.md`](../verify/d3_modes37_20261002/RESULT_STEP2.md),
[`.../RESULT_STEP2B_DUALCOPY.md`](../verify/d3_modes37_20261002/RESULT_STEP2B_DUALCOPY.md).
Pre-registrations `PREREG_STEP1.md` (`867de577`) and `PREREG_STEP2.md` (`a317d25c`), both
committed **unrun**. New rows **U-9182**, **U-9183**, **U-9184**.

## Standing user decisions (recorded 2026-10-02, commit `1cbd4678`)

1. **D2 is PARKED, not closed, and stays re-openable.** 2 of 3 metrics inside their
   unchanged `d81a8df6` bounds; `driving-median` ~1.3 % under with its carrier **not
   identified**. Residuals are `DEFERRED.md` **D-11071**. **Re-pickup trigger:** any
   finding touching `+0x4a4`, the contact collector / `FUN_00538c80`, grip-clamp #6
   (`0x004687f0..0x0046897b`), the substep/chunk loop (`0x00470c70`), `ReassertContacts`,
   or player-car speed on Training → report a **"D2 REOPEN CANDIDATE"** row and do not
   change D2 code in that session.
2. **D3 is UNBLOCKED.**

## What is MEASURED and must not be re-derived

- **(e) PASSES 3/3** on the post-attempt-20 build: `launch` **1426.4 / 2053.0 / 2055.2**,
  `ft_median_m0` **2550.6 / 2053.0 / 2278.2** (n = 100 / 23 / 39). Bands unchanged.
- **(b) FAILS 3/3**, 13 bands, and **all five failing bands are STEER bands**
  (`c0_distinct`, `c1_distinct`, `steer_distinct`, `c1_median`, `abs_steer_median`).
  **accel, brake and `c0_median` PASS on all three cars.**
- **D2 attempt 20 moved the AI window by EXACTLY ZERO.** `MASHED_D2_BATCHMODE=plane`
  reproduces the default arm on every printed digit of both scorers. **The knob is proved
  live** — the full-CSV diff first differs at AI-step call **455**, 235 calls after the
  window closes (2823 rows of `rec_9e4`, 1765 of `c1`).
- **The port is fully deterministic** on this recipe: `r1`/`r2`/`r3` are byte-identical
  over their whole common prefix. Any `collateral.py` floor derived from such a pair on
  this data is **too generous**; use the direct diff.
- **The window provably runs `ControlStep` (`FUN_00416250`)** — the original's window
  carries `ai_mode == 7` on 80 of car 1's 220 calls, and mode 7 is committed **only** at
  `0x0041642f` inside that function.

## THE OPEN QUESTION — (b), decomposed

| carrier | best evidence | next move |
|---|---|---|
| **`int mode = 0;`** (`Ai/AiStandalone.cpp:844`) — the `0x0041665c` multiplier never switches off. The original's `mode != 0` calls carry `curv` medians **125.8 / 91.3 / 132.9**, i.e. it stops amplifying exactly where curvature is extreme | best single arm on cars **1 and 3**: `abs_steer_median` 52.5→15.0 and 49.0→24.5 | **real, large, NOT sufficient — do not re-test it** |
| **`curv` 2-8x high** — `FUN_00443440` @ `0x004162b0` (`AiStandalone.cpp:837`) | `mode==0` vs `mode==0`: **52.04 / 52.81 / 50.08** vs **6.61 / 9.05 / 23.64**; best single arm on **car 2** | **U-9183 — START HERE** |
| **speed +33..52 %** inside the window (a linear multiplier of steer magnitude) | 3421.7 / 3346.2 / 3495.9 vs 2254.8 / 2524.1 / 2554.6 | bounded by (e) passing on `[0,k)`; not independently actionable yet |
| **`c0_distinct = 6`** on car 1 | **identical in all seven counterfactual arms**, floor 13 | **U-9182** |

### START HERE — two OFFLINE measurements, no new run, no build

1. **U-9183, position-matched curvature.** The index-matched comparison above is **partly
   circular**: the two sides traverse at different speeds, so at the same within-window
   call index they are at different track positions, and curvature is a property of
   position. Re-bin both sides by `look_idx` / `look_blk` / `ai_spline_idx` (all three are
   already columns in the committed captures) and compare **within bin**. Only if the gap
   survives is `FUN_00443440` a defect — and then `0x00418560` (`SelectSpline`, demoted
   C3→C2 with three missing arms and a missing spline-index reset) is the first suspect.
2. **U-9182, the branch split.** `c0` is written at `0x00416697` (low-err branch) and
   `0x004167b1` (counter-steer, high branch). The decode rule is already in
   `ai_band_sim.py:63-71` (`hist_d8 == 360` → low, `hist_dc == 0` → high), so count the
   per-car split on **both sides from existing captures first**. Only hook the original if
   that is inconclusive.

**Do both before spending a session on the `FUN_00414c30` + producer-chain port.** This
session's evidence says that port would not close (b) on its own.

### If you do write the modes port, the scope is already measured

`FUN_00484c70` is **already ported and C3 with a GREEN Frida diff** — but in the
**asi-only** `Util/PromoLoop_round20.cpp`, so `mashed_re.exe` has no copy. `DAT_006e70d8`
is written only by `FUN_00484c90` (`0x00484cd4`) and `FUN_00485070` (`0x0048508a`);
`DAT_006dccb8` by `FUN_00484c90` (`0x00484ca4`), indexed by the registrar `FUN_00484cf0`
(`0x00484d2d`/`0x00484d35`), which has **13 callers**, most in the already-ported power-up
range including the dispatcher `0x0045bba0`.
**Porting `FUN_00484c70` alone is INERT** — both globals are `.bss` zeros, the count is 0,
`FUN_00414c30`'s loop never runs, no mode is ever set. **Seeding them is not a port.**
`FUN_00414c30` (704 bytes, `0x00414c30..0x00414ef0`) has no port at all; the only
reference in `mashedmod/src` is the asi-side call-through at `AiControlStep.cpp:92`, which
jumps into `MASHED.exe` and cannot work in the standalone.

### Dead ends — do not re-open

- **`rate1` pinned `0.0f`** (`0x00416a30` `:983`, `0x00417da0` `:1102`) and the
  **velocity-derived heading** (`0x00415e20` `:175`) are **not reachable on this recipe**.
  They live in `ControlStepM49`/`M8`, selected only when `fd0 ∈ {4,8,9}`, and `ControlStep`
  calls the **correct** body-forward `SteerAngleErrorFwd` at `:858`. `U-9184` records this
  and corrects `DUAL_COPY_FIX_2026-09-29.md:177-180`, whose claim that (b) "runs on" those
  two is wrong. **The demotions themselves stand.**
- **`0x00443080`'s exe literal** is harmless: `tgt_7ffc` is within the noise floor in every
  shared band, on top of the committed 6259/6259.
- **Attempt 20 as a cause of any (b) number.** Measured at exactly zero, with a live-knob
  positive control.

## Recipes (unchanged, copy-paste)

```
# standalone (b)/(e) capture
py -3.12 re/tools/sa_capture.py verify/<dir>/<tag> 8,30,60 MASHED_MUTE=1 \
    MASHED_TRACK_VIEW=Training MASHED_CAR=1 MASHED_ROUND=1 \
    MASHED_WIN_POS=primary-bl MASHED_TITLE="<label>" \
    MASHED_AI_STEPDUMP=verify/<dir>/<tag>.csv
# original (b)/(e) capture -- one run covers all three AI cars
py -3.12 re/frida/scenario_launch.py --track 0 --mode 10 --cars 4 --car 0 \
    --poke-ctrl-slots --statediff-out verify/<dir>/o1.msd \
    --statediff-car 1 --statediff-aistep --hold 60
# scorers (BANDS ARE NOT TO BE MOVED)
py -3.12 re/tools/ai_ctrl_window.py --check <csv>      # (b)
py -3.12 re/tools/ai_speed_env.py   --check <csv>      # (e)
# new this session, read-only
py -3.12 re/tools/ai_mode_split.py <csv>               # G2-MODE / G2-JOINT / G2-STEER
py -3.12 re/tools/ai_band_sim.py --orig <o.aistep.csv> --sa <sa.csv> --out <report.txt>
#   now carries modeseq + curvseq counterfactual arms; its >=95% validation gate stands
# guards
pwsh -NoProfile -File re/tools/pu_replay/sweep.ps1
py -3.12 re/frida/scenario_launch.py --track 0 --mode 10 --cars 4 --car 0 \
    --poke-ctrl-slots --oracle --rule 3 --hold 50
```

## Standing rules

Launch muted; `MASHED_TITLE` on every run; `MASHED_WIN_POS=primary-bl`;
`--poke-ctrl-slots` on race captures; **never** `MASHED_NAV_DEMO`. Track PIDs, kill only
yours. Frida **entry hooks only**. Ghidra pool clones `-readOnly`; never write the master.
Never `unlock_*` on `original/MASHED.exe`. Build via `mashedmod\build.bat` from PowerShell.
Trackers only through `re-classify`, preserving each file's line endings — `DEFERRED.md`,
`UNCERTAINTIES.md`, `hooks.csv`, `STUBS.md` are **CRLF**; `ROADMAP.md`,
`re/analysis/CHANGELOG.md` and this file are **LF**. Pre-register every gate; if one fails,
STOP, and state prominently any gate or rule you replace.

## Still open

**D3:** criterion (b) on all three cars — **U-9182**, **U-9183**, **U-9184**.
**D2 (parked, D-11071):** U-9180, U-9181.
**Elsewhere:** U-9177, U-9176, U-9156, U-9171, §20.14's `-0.1` duty cycle, D1-residue R1,
the unported outer chunk loop.
