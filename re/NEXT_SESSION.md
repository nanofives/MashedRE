# Next session kickoff

Updated 2026-10-03 at the close of the **D3 start-boost A/B** session.
Branch `race/first-frame-parity`. Nothing is pushed. **No game code was edited this
session**; the only code changes are analysis tools.

## The headline

> **USER DECISION 2026-10-03 (Mariano) — the decision the block below left open is MADE:
> KEEP the AI start boost.**
>
> `mashedmod/src/mashed_re/Vehicle/VehiclePhysicsRun.cpp:703-707` stays in the default
> build, because it reproduces the original's launch: **(e) is MET 3/3, within 0.05 %** of
> the original's own 1425.7 / 2052.5 / 2055.0. Removing it is **rejected** — that trades a
> MET criterion for a still-NOT-MET one. `MASHED_NO_START_BOOST` remains a measurement knob
> only.
>
> The work that follows from it, in order:
> 1. **Control the player-elimination confound** (`race_[0].alive` differing at
>    `rt = 1.8667 s`, inside the scored window) so that every (b)/(e) number is single-cause.
> 2. **Attack the surviving +15..31 % AI over-speed** with the boost ON — the only route
>    that does not trade (e) against (b). Counterfactually it moves car 1's `c1_distinct`
>    80 → 50 and `steer_distinct` 86 → 61, both into band. Car 2's residual is `err`, not
>    speed.
>
> Recorded in `ROADMAP.md` §D3 ("D3 USER DECISION 2026-10-03"). No band moved, no C-level
> moved.

> **UPDATE 2026-10-03 (D3 START-BOOST A/B session, commits `2b3e2f47` .. `c79a3619`):
> the `MASHED_NO_START_BOOST=1` A/B that the START-HERE block below asked for is DONE.
> It did NOT close (b) and it REGRESSED (e) 6/6. There is now a USER DECISION open.**
>
> Arm A = default, arm B = `MASHED_NO_START_BOOST=1`, three repeats each, every repeat
> digit-identical, no failed boot, recipe/scorers/bands unchanged and both band files
> proven unedited. Fresh arm A reproduces `r1/r2/r3` on every printed digit.
>
> - **(b) 13 → 6 failing bands.** Car 1 5→4, car 2 4→2, **car 3 4→0 (passes outright)**.
>   `c1_median` 52.5/38.0/49.0 → **0.0** and `abs_steer_median` 52.5/57.5/49.0 →
>   **6.0/21.0/9.0**, all in band on all three cars. **But car 1 gains TWO NEW failing
>   bands**, `accel_distinct` and `brake_distinct`: it takes only `c4 = 255` and only
>   `c5 = 0` over the window. **(b) is still NOT MET on 2 of 3 cars.**
> - **(e) regresses 6/6, −46 % to −90 %.** `launch` 1426.4/2053.0/2055.2 → **200.5** on
>   every car, against the **ORIGINAL's own** 1425.7/2052.5/2055.0. **The original
>   demonstrably HAS a launch and arm A reproduces it to 0.05 %** — removing the seed
>   removes the port's only reproduction of a real original behaviour.
> - **Window speed +41/+40/+34 % → +31/+21/+15 %.** The seed is worth about HALF the
>   over-speed. A sustained **+15..31 %** survives the knob and is a **separate carrier**.
> - **The knob is proven live in flight**, not inferred: slots 1..3 read
>   `g_startBoosted` 0→1 with `+0xbf8 == 1` and `+0xbf4` decaying
>   **1100/900/700/500/300/100**; arm B never seeds them; **slot 0 is never seeded in
>   either arm**. `re/tools/sa_boostwatch.py`, `ReadProcessMemory`, no injection.
> - **Player physics is BIT-IDENTICAL between arms** over 4198 `player_trace` lines —
>   **no D2 REOPEN CANDIDATE on player physics.** One confound IS filed: `race_[0].alive`
>   differs at `rt = 1.8667 s`, **inside** the scored window, so the arm A vs arm B deltas
>   are the seed PLUS that divergence and this step did **not** separate them.
>
> Read [`verify/d3_noboost_20261003/RESULT.md`](../verify/d3_noboost_20261003/RESULT.md).
> **One gate was replaced and is stated prominently there**: the pre-registered base
> self-check `record[v] + 0x000 == v` is FALSE in the port and voided the first witness
> run; it was replaced by three stronger legs.

The block below is the PREVIOUS session's headline, left as history.

> **UPDATE 2026-10-02 (D3 OFFLINE session, commits `31fa2fe3` .. `077fc43c`): both
> START-HERE measurements below are DONE. `U-9183` and `U-9182` are RESOLVED, every
> pre-registered gate PASSED, and the recommended next target has CHANGED.**
>
> - **`curv` is faithful.** Position-matched, median `|dcurv|` is **0.0197 / 0.0131 /
>   0.0296** deg on n = 105 / 68 / 104. The 2-8x was a position artefact, on top of a
>   conditioning artefact (`mode == 0` selects the original's low-curvature calls; the
>   FULL-window `curv` medians are **94.14 / 11.40 / 38.28** original vs **52.04 / 52.81
>   / 50.08** port, and on car 1 the ORIGINAL is higher). **Do not port the curvature
>   chain, and `SelectSpline` is not a lead** — the original's `spline` argument equals
>   `0x801aa0 + ai_spline_idx*0x204` on 5463 of 5463 rows.
> - **`c0`'s arithmetic is bit-faithful** (recompute matches the logged byte **586/586**
>   on the port, **148/151** on the original). `c0_distinct` comes from ONE branch, MAG
>   `0x00416697`; everything else contributes only `0`. Car 1 split: **MAG 72 vs 24**,
>   MAGHI 62 vs 171, DEAD 54 vs 22, CTR1 32 vs 3, FF30 **0 vs 0**, CTRHI **0 vs 0**.
> - **What is left is a SUB-DEGREE residual across a HARD split.** At matched position the
>   median `|d signed err|` is only **0.945 / 1.296 / 0.466** deg, but the LO/HI split is a
>   discontinuity at `err = 0/360` and `|err|` is a 1-2 deg oscillation about it, so that
>   bias flips the band on **43 % / 24 % / 13 %** of matched calls. On car 1 it persists at
>   MATCHED SPEED (2606.0 vs 2606.8). New row **U-9185** carries it.
>
> Read [`re/analysis/D3_B_OFFLINE_2026-10-02.md`](analysis/D3_B_OFFLINE_2026-10-02.md)
> and [`verify/d3_offline_20261002/RESULT.md`](../verify/d3_offline_20261002/RESULT.md).
> **One gate was replaced, declared BEFORE the run** (`PREREG.md` A.0): the known-answer
> check could not run offline because the spline point array is runtime memory.

The paragraph below is the PREVIOUS session's headline, left as history.

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
| ~~**`curv` 2-8x high**~~ — `FUN_00443440` @ `0x004162b0` (`AiStandalone.cpp:837`) | **CLOSED 2026-10-02.** Position-matched median `|dcurv|` **0.0197 / 0.0131 / 0.0296** deg (n = 105 / 68 / 104); full-window medians **94.14 / 11.40 / 38.28** orig vs **52.04 / 52.81 / 50.08** port | **U-9183 RESOLVED — do NOT re-open, and do not port the curvature chain** |
| **speed — SPLIT IN TWO 2026-10-03.** A linear multiplier of steer magnitude via `m = err*speed*0.0030034` (`0x00416656`) | **(i) the start seed** (`VehiclePhysicsRun.cpp:703-707`) is worth about HALF: removing it takes the window medians from **3421.7 / 3346.2 / 3495.9** (+41/+40/+34 %) to **3169.4 / 2904.7 / 3011.0** (+31/+21/+15 %) vs the original's **2419.5 / 2397.0 / 2617.6**. **(ii) the remaining +15..31 % is a DIFFERENT, UNIDENTIFIED carrier** | **(i) MEASURED — the A/B is DONE, see the headline; removing it costs (e) 6/6. (ii) is now the open speed lead. U-9185** |
| **sub-degree steering-error bias across a HARD split** — `SteerAngleErrorFwd` `0x00416596`; LO/HI split at `err = 0/360` (`0x004165c0` / `0x004166cf`) | at matched position median `|d signed err|` **0.945 / 1.296 / 0.466** deg (target dir 0.28/0.48/0.45, **body heading 0.98/0.91/0.65**); flips the band on **43 / 24 / 13 %** of matched calls, and persists on car 1 at **matched speed** (2606.0 vs 2606.8) | **U-9185** — second, after the start-boost A/B |
| ~~**`c0_distinct = 6`** on car 1~~ | **ANSWERED 2026-10-02.** `c0` comes from ONE branch, MAG `0x00416697`; car 1 split **MAG 72 vs 24**, MAGHI 62 vs 171. The arithmetic reproduces the logged byte **586/586** (port), **148/151** (orig) | **U-9182 RESOLVED — do NOT port the `c0` magnitude path, it is bit-faithful** |

### START HERE — the ONE next move, and it is NOT a port

> The two offline measurements this section used to list are **DONE** (`U-9183` and
> `U-9182`, both RESOLVED 2026-10-02), and **the start-boost A/B below is DONE too**
> (2026-10-03, `verify/d3_noboost_20261003/RESULT.md`). Do not re-run any of the three.
> The old text is kept below the rule as history.

### THE DECISION THAT IS NOW THE USER'S, and nothing should be ported until it is made

The A/B measured a real and **adverse** trade. Three options, none taken:

1. **Keep the seed (status quo).** (e) MET 3/3, (b) 13 failing bands. The AI third stays
   blocked on (b).
2. **Remove the seed.** (b) 6 failing bands with car 3 passing outright, (e) fails 6/6 by
   46-90 %. **This trades a MET criterion for a still-NOT-MET one.**
3. **Keep the seed and attack the surviving +15..31 % over-speed instead.** Per the
   counterfactual matrix (`ai_band_sim.py`, validation 220/220 = 1.000 on all three cars)
   closing it would move car 1's `c1_distinct` 80→50 and `steer_distinct` 86→61 **into
   band**. **The only option that does not trade one criterion against the other**, and it
   is not costed yet.

### THE NEXT MOVE, if the user wants more measurement before deciding

**1. Separate the seed from the player-elimination confound. Cheapest thing left.**
With the seed ON the player is eliminated at `rt = 1.8667 s`, **inside** the scored window
(`race_[0].alive` 0 vs 1, written at `TrackRenderer.cpp:4664` via `RE::SegmentCheck` +
`EliminationCheck`, which read the **AI cars'** positions). `round_mode_` is true,
`g_aib.alive[0]` is cleared at `TrackRenderer.cpp:3722`, and the AI tick loop at
`AiStandalone.cpp:1708` runs `for (v = 0; v < 4; ++v)` gated on `car_alive(v)` — so
`VehicleStep(0)` runs in arm B and does not in arm A. **Until that is controlled, no arm A
vs arm B delta is single-cause.**

**2. Then the body heading, and car 2 is the car to probe.** It is still the larger median
share on all three cars in both arms, and on **car 2** it **GROWS** 0.9085 → **1.6059** deg
when the seed is removed — car 2 is also the car whose remaining (b) failure the
counterfactual attributes to `err`, not speed. The two separable candidates are unchanged
(see item 2 of the history block below).

**3. Separately: find what carries the +15..31 % over-speed that survives the knob.** It is
**not** the seed. This is option 3 above.

<details><summary>History: the start-boost A/B as it was briefed, now DONE</summary>

**1. Run the start-boost A/B first. One environment variable, no code change.**
`VehiclePhysicsRun.cpp:703-707` applies a **port-only start boost to `slot != 0`, i.e. to
exactly the cars criterion (b) scores** — `+0xbf8 = 1`, `+0xbf4 = 1300`, guarded by
`MASHED_NO_START_BOOST`. The port's window speed is **3421.7 / 3346.2 / 3495.9** against
the original's **2419.5 / 2397.0 / 2617.6** (**+41 / +40 / +34 %**), and
`m = err*speed*0.0030034` (`0x00416656`) makes speed a linear multiplier on the steer byte
while the extra distance travelled pushes the port into higher-curvature track inside the
same 220 calls. Suspect the port-only scaffold before re-suspecting a byte-faithful
transcription.

> Re-capture the port side with `MASHED_NO_START_BOOST=1` and re-score **BOTH (b) and
> (e)**. (e) passes today **with** the boost, so the seed cannot be removed on (b)'s
> evidence alone — if (e) regresses, that is a trade-off for the user, **not** a fix.
> Pre-register the decision rule before the run, as every D3 session has.

**2. Only if that does not close (b): the AI cars' body heading.** It is the larger median
share of the matched-position residual (**0.9796 / 0.9085 / 0.6506** deg). It reaches
`SteerAngleErrorFwd` as `(cos(a.yaw), sin(a.yaw))` from `TrackRenderer.cpp:3729`, with
`a.yaw` round-tripped through `Vehicle::VehiclePhysics_StepCar` (`:3315` in, `:3327` out)
— **not** from `rec+0x9d4`/`+0x9dc`, which is what `AiStandalone.cpp:939`'s comment claims
and which nothing in the standalone reads. Two separable candidates: the physics' own yaw
(a D2 surface measured through the AI) and the scalar reconstruction standing in for the
record's forward basis row (a port-only bridge, cheaper to test). The
`yerr * (6.0f*dt)` turn-rate limiter at `TrackRenderer.cpp:3416` / `:3676` is **not** a
suspect — it is in the legacy "AI v2" `else` branch, which the ported path does not take.

**3. `FUN_00414c30` + producer chain is RULED OUT for this carrier.** Its effect on (b) is
through `ai_mode`, already refuted as sufficient by the modes-3/7 arms, and `curv` and
`c0`'s arithmetic are now proved faithful — so that chain cannot reach what is left.

</details>

**One question for the USER, deliberately not decided.** Criterion (b)'s `c0_distinct`,
`c1_distinct` and `steer_distinct` count which side of a hard discontinuity a 1-2 deg
oscillation lands on; a faithful port can fail them. Whether to re-specify (b) on a
band-invariant statistic is a ROADMAP decision. **No band was moved.**

<details><summary>History: the two offline measurements, now both DONE</summary>


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


</details>

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
py -3.12 re/tools/ai_posmatch.py --orig <o.aistep.csv> --port <r.csv> --part A|B|COLL
#   NEW 2026-10-02. Implements verify/d3_offline_20261002/PREREG.md verbatim: the
#   position-matched curvature test (A), the c0 branch split with both known-answer
#   checks (B), and a DESCRIPTIVE collateral/err-decomposition leg (COLL, no pass/fail).
#   Its constants and bands are pre-registered and ARE NOT TO BE MOVED.
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

- **D3 (b) is still NOT MET** and is the phase's sole blocker. `U-9183` and `U-9182` are
  RESOLVED; **`U-9185`** now carries the remainder (the start-boost scaffold, the
  sub-degree heading residual, the knife-edge band split). `U-9184` stands as filed.
- `[UNCERTAIN]` the `look_z` tail on car 1 (matched-position median **0.7678**, p90
  **5.1128**) — below defect size at the median, but the only matched-position field
  other than speed with a non-trivial tail.
- **D2 parked, D-11071:** `U-9180`, `U-9181`. Two informational **D2 WATCH** rows were
  filed 2026-10-02 (`D3_B_OFFLINE_2026-10-02.md` §6): AI-slot speed is **+34..41 %** on
  track 0 / mode 10 with the start-boost scaffold in place (D-11071's parked metric was
  the **player** 1.3 % **short** on Training — opposite sign, different car class,
  different scenario, so it neither confirms nor refutes that carrier), and the body
  heading is a physics output. **No D2 code was read or changed.**
