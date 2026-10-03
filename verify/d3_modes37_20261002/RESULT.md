# SESSION RESULT — D3 re-baseline + the modes-3/7 test (2026-10-02)

Branch `race/first-frame-parity`, nothing pushed. **No game code was edited.** The only
code changes are two read-only analysis tools.

## Verdict

> **D3 does NOT close. The 2026-09-29 closure path — "D3 closes by porting behaviour modes
> 3 and 7" — is REFUTED by its own pre-registered gate, so the port was not written.**
> Criterion **(e) still PASSES 3/3** and criterion **(b) still FAILS 3/3**, both unchanged
> by D2 attempt 20, which moved the AI window by **exactly zero** under a proved-live
> control. (b) is now **decomposed into four measured carriers**, and the dual-copy
> hypothesis turned out to be the **same** hypothesis, already measured.

## Index

| step | record | commit |
|---|---|---|
| 0 — user decisions | `ROADMAP.md`, `DEFERRED.md` D-11071, `re/NEXT_SESSION.md` | `1cbd4678` |
| 1 — pre-registration | `verify/d3_rebase_20261002/PREREG_STEP1.md` (unrun) | `867de577` |
| 1 — result | `verify/d3_rebase_20261002/RESULT_STEP1.md` | `409cd086` |
| 2 — pre-registration | `verify/d3_modes37_20261002/PREREG_STEP2.md` (unrun) | `a317d25c` |
| 2 — result | `verify/d3_modes37_20261002/RESULT_STEP2.md` | `7815c882` |
| 2B — dual-copy + collateral | `.../RESULT_STEP2B_DUALCOPY.md` | `fff61ba7` |
| 3 — trackers, roadmap, handoff | this file | `7c07c9c8` |

## STEP 0 — the two user decisions, recorded (`1cbd4678`)

1. **D2 is PARKED, not closed, and stays re-openable.** 2 of 3 metrics inside their
   unchanged `d81a8df6` bounds (slip 1500-2000 **0.1943**, slip 2000-2600 **0.2524**);
   launch `L = 0` at 0.19 % and recovery H1 398/400 = 99.5 % both PASS; `driving-median`
   **1852.66 / 1861.43 / 1854.65** against a lower bound of **1904.70**, ~1.3 % under,
   **carrier not identified**. The bounds were not moved. Written as a **PARKED block
   above the REOPENED block** in `ROADMAP.md` §D2 (the REOPENED block left as history), as
   `DEFERRED.md` Active row **D-11071** with the full re-pickup condition, and in
   `re/NEXT_SESSION.md`.
2. **D3 is UNBLOCKED.** Written as an **UNBLOCKED note above the BLOCKED note** in
   `ROADMAP.md` §D3 (the BLOCKED note left as history).

## STEP 1 — the re-baseline. What attempt 20 moved for the AI: **exactly zero.**

| gate | outcome |
|---|---|
| G1-DET | **PASS** — `r1`/`r2`/`r3` agree to every printed digit on all (e) and (b) lines |
| G1-BOOT | `r3` failed to boot twice (early exit after `t30`, then after `t08`); **reported as failures**. The 220-call window completed in all three runs and G1-DET's exact agreement proves the measurement is unaffected. |
| G1-E | **(e) PASSES 3/3** |
| G1-B | **(b) FAILS 3/3**, 13 of 30 bands (was 15 on 2026-09-29) |
| G1-ATTRIB | **zero**, with a positive control |

**(e), all six gated values, `regime0=1` and `flag=[0]` on every car:**

| car | `launch` (band) | `ft_median_m0` (band) | n |
|---|---|---|---:|
| 1 | **1426.4** (1397.2..1454.2) | **2550.6** (2500.6..2602.6) | 220 / 100 |
| 2 | **2053.0** (2011.5..2093.6) | **2053.0** (2011.5..2093.6) | 220 / 23 |
| 3 | **2055.2** (2013.9..2096.1) | **2278.2** (2232.5..2323.7) | 220 / 39 |

**(b), the 13 failing bands.** n = 220 per car, median **call index 109.5** on every row,
median window speed **3420.3 / 3334.4 / 3496.9**:

| car | failing | values vs band |
|---|---:|---|
| 1 | 5 | `c0_distinct` 6 [13,37]; `c1_distinct` 103 [17,70]; `steer_distinct` 108 [29,96]; `c1_median` 52.5 [0,0]; `abs_steer_median` 52.5 [0,23] |
| 2 | 4 | `c1_distinct` 94; `steer_distinct` 122; `c1_median` 38.0; `abs_steer_median` 57.5 |
| 3 | 4 | `c1_distinct` 100; `steer_distinct` 119; `c1_median` 49.0; `abs_steer_median` 49.0 |

**All five distinct failing bands are STEER bands.** `accel_distinct`, `accel_median`,
`brake_distinct`, `brake_median` and `c0_median` **pass on all three cars**.

**G1-ATTRIB.** `MASHED_D2_BATCHMODE=plane` reproduces the default arm on **every printed
digit** of both scorers, all three cars → attempt 20's admission test moved **0** of these
numbers. **The knob was proved live rather than assumed:** the full-CSV diff of `r1`
against `p1` first differs at **row 1365, car 1, AI-step call 455** — every column of
every earlier row identical on all three cars — then diverges on `own_x`/`own_z` **2824**
rows, `rec_9e4` **2823**, `curv` 2811, **`c1` 1765**. Structural corroboration:
`SolveWheelContacts` is called at `VehiclePhysicsRun.cpp:1028` inside the per-car substep
loop with no slot gate, and calls `ProduceTerrainBatch` at `WheelContactSolver.cpp:148`.

**Stated limit:** the integer substep loop (`015537a2`) landed with **no revert knob**, so
its share is not separable in this step. The HEAD-vs-2026-09-29 delta is a **3-day,
30-commit** delta and is attributed to nothing.

## No seed replacement

`VehiclePhysicsRun.cpp:702`'s fitted AI start seed (`+0xbf8 = 1`, `+0xbf4 = 1300`) was
**not touched**. Replacing it required proving the AI's own pre-race input drives it on the
**running original**; that proof was never reached because STEP 2 stopped at its registered
gate. **No seed work was done and none is claimed.**

## STEP 2 — the modes-3/7 live test. **G2-SIM REFUTED; the port was not written.**

Live original capture: `scenario_launch.py --track 0 --mode 10 --cars 4 --car 0
--poke-ctrl-slots --statediff-aistep --hold 60` → 3632 frames, **5463 AI-step calls**,
cars [1,2,3], `joinMiss=0`.

| gate | outcome |
|---|---|
| **G2-MODE** | **PASS.** Original `ai_mode` over the window: car 1 `{0:124, 3:16, 7:80}`, car 2 `{0:171, 3:49}`, car 3 `{0:196, 3:24}`. Port `{0:220}` on all three. |
| **G2-JOINT** | **FAIL**, reported as a failure and **not re-thresholded**. |
| **G2-STEER** | measured; exposes two further carriers. |
| **G2-SIM** | **REFUTED** by the rule registered unrun at `a317d25c`. |

**G2-JOINT failed, and it partly refutes the kickoff's framing.** Non-full-throttle window
calls on the original, and how many are **mode 0**: car 1 **96 / 6**, car 2 **41 / 0**,
car 3 **66 / 52**. The gate required *every* non-full-throttle call to be `mode != 0`; on
car 3 **79 % of the lifting happens in mode 0**. So "modes 3 and 7 are what make the
original brake and lift where the port holds throttle" is **true for car 2, mostly true
for car 1, and false for car 3**. It concerns the accel/brake bands, which already pass.
(The port does hold throttle: 5 / 10 / 5 non-full-throttle calls against 96 / 41 / 66.)

**G2-SIM.** `ai_band_sim.py`'s own validation gate was **kept at ≥95 % and PASSED at
220/220 = 1.000 on all three cars**, so the refutation is not a tooling artefact.

| car | arm | `c1_distinct` | `abs_steer_median` |
|---|---|---:|---:|
| 1 | baseline | 103 | 52.5 |
| 1 | **ORIG modeseq** | **58** ✔ enters | **15.0** ✔ enters |
| 2 | baseline | 94 | 57.5 |
| 2 | **ORIG modeseq** | **99** ✘ **moves AWAY** | 47.0 |
| 3 | baseline | 100 | 49.0 |
| 3 | **ORIG modeseq** | 97 | 24.5 |

The registered rule required **both** statistics to move toward band on **all three
cars**. Car 2's `c1_distinct` moves away (94 → 99) → **REFUTED**, and per the registration
**the port was not written**.

**The refutation is not a technicality.** The matrix's strongest arm — original mode **and**
curvature together — still leaves (b) failing on all three cars (2 / 4 / 3 bands), and
**no arm anywhere in the matrix — mode, curvature, speed, error, or any combination —
passes (b) on any car.**

**Stated limit, as registered:** these arms splice the original's inputs onto the port's own
trajectory, which **bounds** the mechanism's contribution and does not forecast a real
port's score. What it does establish is that **modes 3/7 alone are demonstrably not
sufficient.**

## (b), decomposed — the session's substantive finding

| carrier | measurement | status |
|---|---|---|
| **`int mode = 0;`** `Ai/AiStandalone.cpp:844` — the `0x0041665c` multiplier (`m *= curv*0.05` when `mode==0 && curv>20`) never switches off | best single arm on cars 1 and 3. The original's `mode != 0` calls carry `curv` medians **125.8 / 91.3 / 132.9**: **it stops amplifying steer by curvature exactly where curvature is extreme**, and the port, pinned in mode 0, amplifies hardest precisely there | real, large, **not sufficient** |
| **`curv` 2-8x high** — `FUN_00443440` @ `0x004162b0` (`AiStandalone.cpp:837`) | `mode==0` vs `mode==0`: **52.04 / 52.81 / 50.08** vs **6.61 / 9.05 / 23.64**; best single arm on car 2 (`abs_steer` 57.5→26.5, `c1D` 94→75) | **U-9183** — confounded by position; needs a position-matched re-measure |
| **speed +33..52 %** in the window; `m = err * speed * 0.0030034` makes speed a linear multiplier | 3421.7 / 3346.2 / 3495.9 vs 2254.8 / 2524.1 / 2554.6 | real; bounded by (e) passing on `[0,k)` |
| **`c0_distinct = 6`** on car 1 | **identical in all seven arms**, floor 13 | **U-9182**, unexplained |

## STEP 2B — the dual-copy hypothesis: ANSWERED, and it is the same hypothesis

**The window provably runs `ControlStep` (`FUN_00416250`)** — the original's window carries
`ai_mode == 7` on **80 of car 1's 220 calls**, mode 7 is committed **only** at `0x0041642f`
inside `FUN_00416250`, and `ControlStepM49` never commits a mode at all. `VehicleStep`
(`AiStandalone.cpp:1600-1617`) selects by `game_mode_fd0` (`DAT_007f0fd0`).

Of the 8 rows demoted C3→C2 on 2026-09-29, **exactly one bears on (b) here**:
**`0x00416250`**, defect `int mode = 0;` — the same defect modes-3/7 needed.

**Correction filed as U-9184.** `DUAL_COPY_FIX_2026-09-29.md:177-180` names the pinned
`rate1` and the velocity-derived heading as load-bearing and says *"D3 criterion (b) …
runs on these."* **It does not.** `rate1` exists only in `ControlStepM49` (`:983`) and
`ControlStepM8` (`:1102`); `SteerAngleError` is called only from those two (`:990`,
`:1109`); and `ControlStep` calls the **correct** body-forward `SteerAngleErrorFwd` at
`:858`. Those paths need `fd0 ∈ {4,8,9}`, which this recipe does not select. **The
demotions themselves stand** — the C3 evidence genuinely does not cover the exe copies —
only their stated bearing on (b) is corrected. `0x00418860`'s missing force-step is for
**vehicle 0** (the player), and (b) scores cars 1..3. `0x00443080`'s exe literal is
re-confirmed harmless.

## Collateral review — all divergent rows are OUTSIDE scope

No game code changed, so there is no shipping pre/post pair.

- **Floor, measured directly: 0.** `r1`/`r2`/`r3` are **byte-identical on every column
  over their entire common prefix** (all three pairwise diffs return no differing row;
  4193 / 7975 / 7173 rows). Consequently `collateral.py`'s **derived** floor on this data
  is **too generous** (it produced `c1` 189, `look_z` 6.73 from that same repeat pair) and
  its "first frame past the floor" column is a conservative bound. The direct diff is the
  authority and is what is quoted.
- **Paired, same side** (`r1` vs `p1`, with floor): **10 of 35 fields diverge and every one
  is `outside` scope** — `look_z`, `look_best`, `look_idx`, `march_n`, `march_idx0`,
  `hist_d8`, `curv`, `rec_b0c`, `rec_9e4`, `c1`. **25 fields within floor on every aligned
  frame**, including `ai_mode`, `c0`, `c4`, `c5`, `flag_a368`, `own_x`, `own_z`,
  `tgt_7ffc`. The scored window closes at **row 659**; first divergence is **row 1365**.
  **No outside-scope row intrudes into the measurement.**
- **Cross-side at MATCHED CALL INDEX** (the primary cross-side arm, not speed-banded):
  `ai_mode_split.py`, median index **109.5 on both sides**. Outside-scope rows: **`curv`**
  (2-8x) and **`rec_9e4`** (+33-52 %), both with the position confound stated.
- **Cross-side speed-banded** (reported, **not relied on**): only **one** shared band
  (2000-2600, n = 69 vs 48) and its median frame indices are **934 vs 530** — different
  moments, so no conclusion is drawn. Useful negative: **24 fields within the noise floor
  in every shared band**.

## D2 REOPEN CANDIDATE — one row, informational

| trigger | finding | evidence |
|---|---|---|
| the **contact collector** (`ProduceTerrainBatch` / `MASHED_D2_BATCHMODE`) | Attempt 20's spatial admission test **does** change AI-car trajectories: 2823 rows of `rec_9e4` and 1765 of `c1` differ between `local` and `plane`, first at AI-step call 455. **This is the first measurement of attempt 20 on AI slots** — attempt 20 ran `participants=1` throughout and left the magnitude `[UNCERTAIN]`. **It is not a defect**: it moves nothing in D3's scored window and (e) still passes to 0.05 %. Filed to honour D-11071's re-pickup condition. | `verify/d3_rebase_20261002/r1.csv` vs `p1.csv` |

**No D2 code was changed in this session.**

## Gates / rules replaced: NONE. One arm ADDED, declared.

- **No band was moved.** `re/tools/ai_ctrl_window.py` and `re/tools/ai_speed_env.py` are
  byte-identical to their session-start state (`git diff --stat` on both is empty).
- **No decision rule was replaced**, and both failing gates (**G2-JOINT**, **G2-SIM**) are
  reported as failures.
- **One counterfactual ARM was added** to `ai_band_sim.py` after G2-STEER: a per-call
  `curvseq`, alongside the **pre-registered** `modeseq`. Declared in `RESULT_STEP2.md` and
  in the source docstring. It changes no constant, no band and no decision rule, and the
  four pre-existing arms are untouched by construction (both parameters default to `None`).
- The tool's own ≥95 % validation gate was **kept** and passed at 1.000.

## Guards — both unchanged

- **Power-ups:** `pwsh -NoProfile -File re/tools/pu_replay/sweep.ps1` → **11/11 decision
  CLEAN**, contact CLEAN 10/11, `g3` **DIVERGES** = the known R_FLAME 2-of-546 residue.
  Criterion (c) still MET.
- **Modes:** rule-3 oracle → **GREEN**. `SegmentCheck` **2447/2447** MISMATCH 0 with
  **2 segment-ends**; `EvaluateResult` **2/2**; `FinishOrder` **3389/3389**. (a)-(d) MET.
- **Build:** `mashedmod\build.bat` clean, both targets, `[asi] all 422 objects up to date`.

## D3 criteria — verdicts

| third | criterion | verdict |
|---|---|---|
| Modes | (a)(b)(c)(d) | **MET** — re-confirmed GREEN today |
| Powerups | (a)(b)(c) | **MET** — re-confirmed 11/11 today; R_FLAME 2-of-546 residue |
| AI | (a)(c)(d) | MET |
| AI | **(e)** speed | **MET 3/3**, all six gated values inside band |
| AI | **(b)** steering | **NOT MET, 3/3 cars, 13 bands** — the sole D3 blocker |

**D3 is NOT CLOSED.**

## Still open

- **D3:** criterion (b) — **U-9182** (`c0_distinct` = 6 on car 1), **U-9183** (the 2-8x
  curvature gap), **U-9184** (the corrected dual-copy↔(b) link).
- **Next, and it is a measurement not a port:** resolve U-9183 **position-matched** rather
  than index-matched, and U-9182's branch split — **both are offline work on committed
  captures, no new run, no build**. Do them before spending a session on the
  `FUN_00414c30` + producer-chain port.
- **D2 (parked, D-11071):** U-9180, U-9181.
- Elsewhere: U-9177, U-9176, U-9156, U-9171, §20.14's `-0.1` duty cycle, D1-residue R1,
  the unported outer chunk loop.

## Hygiene

Every `mashed_re.exe` spawned and killed **by PID** by `sa_capture.py`; every `MASHED.exe`
by `scenario_launch.py`. **No blanket kill by name.** No worktree created. Ghidra accessed
only through read-only pool clone `Mashed_pool0`; **the master project was never written**.
`original/MASHED.exe` untouched; **no `unlock_*` patch applied**. All launches muted with
`MASHED_TITLE` set and `MASHED_WIN_POS=primary-bl`; `--poke-ctrl-slots` on every race
capture; `MASHED_NAV_DEMO` never used. Frida **entry hooks only**. Nothing pushed.
