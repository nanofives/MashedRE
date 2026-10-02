# PRE-REGISTRATION — D2 attempt 19, STEP 2 (the decision) and STEP 3 (the re-measure)

Committed **before the STEP 3 runs**. Base `cd41ca04`.

## 1 STEP 2 — the decision, taken under PREREG_STEP1C §4's own fix rule

STEP 1C landed on branch **(ii) STATE-0 WHEELS**:

```
wcs_cnt in d = 222..250, 87 solver calls, (bVar16, states, bVar4, iVar8):
  (4, 1111, 0, 4) x 37     -> drop fires, gate sees 3 > 2, drift SKIPPED
  (3, 0111, 0, 3) x  5     ->              gate sees 3 > 2, drift SKIPPED
  (2, 0011, 0, 2) x 31     ->              gate sees 2 <= 2, drift RUNS
  (2, 0110, 0, 2) x 16     ->              gate sees 2 <= 2, drift RUNS
wcs_drift state words: 0011 x 31, 0110 x 16   (47 = the 47 drift hits)
```

(The state word prints as an integer, so `11` is `0011` and `110` is `0110`.)

So on **47 of 87** solver calls **two wheels sit at state 0**, `bVar16 == 2`, and the
byte-faithful gate `0 < count <= 2.0` at `0x004701e8`/`0x004701f3` opens. `bVar4 == 0` on
**all 87** calls, which eliminates the `(kState2Lo < fv) && bVar4` arm of the demotion at
`WheelContactSolver.cpp:170` and leaves the `piVar9[0x15] == -1` arm and the
state-0 -> state-2 latch condition at `:163` as the two remaining mechanisms.

> **PREREG_STEP1C §4, quoted as written: "A fix is authored in STEP 2 only if the run lands
> on a transcription defect at a cited RVA — a condition, a constant or a field offset in
> `0x0046f6c0`'s own code that differs from the disassembly. Anything else ... is reported
> named and UNFIXED, STEP 3 runs on the unchanged build."**
>
> It did not. The gate is byte-faithful (verified instruction by instruction), the site is
> byte-faithful, and the defect is **which state the port's wheel state machine leaves two
> wheels in** — which needs the state machine's own branch inputs on BOTH sides, i.e. a
> second port-side probe and an original-side probe that this attempt has not registered.
>
> ## STEP 2 VERDICT: **NO FIX AUTHORED.** The rule is honoured, not reinterpreted.

**And the substep budget is explicitly NOT fixed either**, for a measured reason rather than
an omission: STEP 1 showed it carries **26.03 %** (substep 2 alone) and the carrier carries
**84.33 %**, so porting `0x00471106..0x00471141` now would change the default build's
integration cadence while leaving ~58 % of the sink standing, and it would move the launch and
recovery statistics at the same time — i.e. it would confound the next attempt's measurement
of the carrier. The loop is **transcribed** in `RESULT_STEP1.md` §6 and is ready to port in one
edit once the carrier is closed. **Registered here so the next attempt does not read the
omission as an oversight.**

## 2 STEP 3 — what is scored, and on what

The build under test is `cd41ca04` + the STEP 1C probe: **the default build with a
default-OFF diagnostic added and no behavioural change.** `git diff 025e1642..HEAD --
mashedmod/src` touches only `if (armed)` call sites and the new probe TU. So (a)-(d) are
expected to **reproduce attempt 18**; the point of running them is (i) that the probe is inert
on the scored metrics too, not only on `motion_diag` line equality, and (ii) the collateral.

Three runs, §16.7 arm, participants=1 confirmed from the game's own
`MATCH-SEED rule=0 participants=1` line, muted, `MASHED_WIN_POS=primary-bl`, `MASHED_TITLE` on
each, PIDs reaped by `a8_run_port.py`:

```
MASHED_MEASURE_SOLO=1 MASHED_TRACK_SEL=12 MASHED_STEER_HOLD_AFTER=0
```

| verdict | rule | tolerance / bound |
|---|---|---|
| **(a)** | `T_post` at `d = 222..250` vs the original's **-0.00464** | within **20 %** of the original's `\|T_W1\|` (`a18_budget.py`'s imported TOL_A, unchanged). Also: the port runs exactly **2** substeps/frame **only if the loop was ported** — it was not, so **3** is the expected and non-blocking reading, reported as-is. |
| **(b)** | launch, `a8_launch.py` | `L = 0`, `+0xb14` engages at `d = 15`, peak at `d = 95`. A regression here is a FAIL to report, not to tune around. |
| **(c)** | recovery, **H1** | `>= 50 %` of the 400 post-trough frames `>= 100` **AND** median `>= 900` |
| **(d)** | the three D2 metrics, 3 runs, against the **UNCHANGED** `d81a8df6` bounds | slip 1500-2000 **0.18855..0.19635**, slip 2000-2600 **0.24488..0.25487**, driving-median **1904.70..1982.44**; `n`, median speed and **median frame** (`a8_medframe.py`) printed next to each per §26.10 |

`n`, median speed **and** `d` accompany every number. ORIG reference `verify/d2_b0c_20261002/orig_sl1.msd`, release re-derived from its own `+0xbf8` marker (**890**), never inherited.

## 3 Collateral — registered before it is run

Per memory `collateral-review-after-every-attempt`, **always with a floor**:

- **Leg 1, paired same-side.** A = attempt 18's `verify/d2_budget_20261002/s1/motion_diag.log`,
  B = this attempt's `s1`, floors A = attempt 18's `s2`, B = this attempt's `s2`. This is the
  probe-inertness leg: **every** divergent field is a probe artefact unless it is inside the
  measured noise floor, because **no behavioural line changed**. Expected 0 divergent.
- **Leg 2, cross-side banded** at matched `d`, `--mode banded`, floor-A `orig_sl1.msd`,
  floor-B `s2`, scope `re/tools/statediff/scope_a6a.txt`. Per §26.10 a band whose median frame
  indices do not overlap is **OFF-REGIME** and **no row in it is read**.
- Any divergent field **outside** the loop's write set is reported as an **outside-scope row**.
  The loop's write set this attempt is **empty** (no behavioural change), so any divergent row
  at all is outside scope and is reported.

## 4 Trackers and handoff

`re-classify` only. **No C-level may move** — nothing was verified against the original this
attempt beyond static disassembly and existing captures. What is owed: U-9178's shape is
**replaced** by the measured carrier; U-9160 is **re-scoped** (original side closed, port side
measured at exactly 3, and it is a multiplier not the carrier); a **new** row for the
`0x0046f6c0` grounded-count defect; and the `0x00469ad4` RVA citation is **corrected** wherever
the trackers carry it.

## 5 What is NOT done

No knob, no clamp, no fitted constant. No threshold chosen to close the drift branch. AI slots
1+ (`VehiclePhysicsRun.cpp:702`) untouched — D3. Nothing pushed.
