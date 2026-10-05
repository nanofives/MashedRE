# BASELINE — the window distance over-run, duration-matched and with zero spread on both endpoints

**RAN 2026-10-05.** Reconnaissance for a possible speed/distance leg. **Declared as reconnaissance,
not a pre-registered gate**, and committed because two of its numbers are additive to the record
while the rest merely reproduce what is already attributed.

**No C-level moved. No band moved. No code changed.** Captures: `A1.csv`, `R1.csv`, `R2.csv` (this
build, three repeats) against the committed `verify/d3_elim_20261003/o_t1`/`o_t2`/`o_t3`.
Tool: `re/tools/ai_armregime.py` plus the declared scorers `ai_speed_env.py` and `ai_ctrl_window.py`,
both proven unedited.

## What is ADDITIVE here

**1. The window arclength ratio, with zero measurement spread on either endpoint.**

| side | window arclength | captures/repeats agreeing |
|---|---|---|
| original | **23.5394** | 3 of 3 (`o_t1`, `o_t2`, `o_t3`) |
| port | **31.2075** | 3 of 3 (`A1`, `R1`, `R2`) |
| **ratio** | **1.32576** | — |

Both sides are bit-deterministic to four decimals. The port's three repeats are identical on every
printed `(e)` digit as well (`launch` 1426.4, `ft_median_m0` 2550.6, `ft_median` 3420.3, `wmax`
3814.7). **Note the earlier whole-capture distances (23.548 / 24.971 / 24.971) are NOT this
statistic** — they include post-elimination settling; over the scored window the three originals
agree exactly.

**2. The duration confound is CLOSED.** Both sides advance `clk_0ff4` by exactly **50 per frame**
and the window spans exactly **10950** clock units on each. 220 calls is one call per frame on both
sides. So the distance ratio is **duration-matched to the unit**, and the over-run is not an artefact
of the two sides covering 220 frames of different length.

## What merely REPRODUCES the record, and is reported so it is not mistaken for new

The command-share figures below match `verify/d3_elim_20261003/RESULT_STEP2.md:182-192` exactly;
they were re-measured from this build's capture as a self-test, not discovered.

| | original | port | source for the prior figure |
|---|---|---|---|
| `c4 == 255` share, car 1 | 0.6364 | 0.9773 | `RESULT_STEP2.md:182-192` |
| `c5 == 255` share, car 1 | 0.2136 | 0.0227 | same |
| accel byte set, car 1 | {0, **64**, 255} | {0, 255} | same (`c4 == 0x40` on 49 original calls, never by the port) |

**This build reproduces the committed baseline on the gated statistics**, which is what makes the
captures usable: `(e)` **PASS 3/3** with `launch` 1426.4 / 2053.0 / 2055.2 against the reference
1425.7 / 2052.5 / 2055.0, and `(b)` **FAIL 3/3** with 13 of 30 bands, all steer. Both are the
recorded state.

**And the gated speed statistics AGREE with the original**, which is the point worth keeping:

| gated stat, car 1 | original | port | delta |
|---|---|---|---|
| `launch` | 1425.7 | 1426.4 | **+0.05 %** |
| `ft_median_m0` (n = 100 on both) | 2551.6 | 2550.6 | **-0.04 %** |

The ungated `ft_median` differs by +51.7 % (3420.3 at n = 215 against 2254.8 at n = 124) — and
`ai_speed_env.py`'s own docstring declares it **reported, not gated**, because it is "not a
physics-only comparison". The differing `n` is the whole story: the port is at full throttle on
97.7 % of window calls, the original on 63.6 %.

## The conclusion this reconnaissance reached, and why NO leg was pre-registered

**The over-run is already fully attributed and I re-derived part of it.** `U-9186` holds the carrier:
**149 of 660 window calls** where the original lifts or brakes and the port holds full throttle,
**0 of 149 unexplained**, split cell-identical across `o_t1`/`o_t2`/`o_t3`. The per-car split
(`RESULT_STEP2.md`) is what makes the choice of next step non-obvious:

| car | `FUN_00414a70` (36) | mode 7 via `FUN_00414c30` (49) | `FUN_004148b0` + LOS (64) | total |
|---|---:|---:|---:|---:|
| **1** | **0** | **49** | **31** | **80** (36.4 % of its window) |
| all | 36 | 49 | 64 | 149 of 660 (22.6 %) |

- **Mode 7 (49 calls) is car 1's largest carrier and is STRUCTURALLY BLOCKED** — its loop iterates
  `FUN_00484c70` world-objects whose standalone copies are `.bss` zeros, and
  `verify/d3_modes37_20261002/RESULT_STEP2.md:201-203`'s rule stands: *"seeding the globals would not
  be a port."* Same blocker as modes 3/7.
- **`FUN_004148b0` (64 calls, 31 of them car 1's) is BLOCKED by U-9187** — measured **inert and
  unsafe** on 2026-10-03. Its real prerequisite is `FUN_00442a60` / `Spectator::ComputeDistances`
  (C2 `new`, **no body anywhere**), plus exe-side bodies for `0x0040e470`, `0x00442cc0`, `0x0046d4a0`.
- **`FUN_00414a70` (36 calls) is the only branch startable today** — C2 `mapped` with no body, callee
  `FUN_00414300` C2 with no port, no structural blocker recorded. **It carries 0 of car 1's calls**
  (cars 2: 10, car 3: 26).

**So nothing startable today moves car 1's 1.32576x at all**, and car 1 is the only car U-9191
covers. Pre-registering a leg before that choice is made would be registering gates for a target not
yet chosen, so none was written.

## Two corrections to my own earlier framing in this session

1. **I said the distance over-run "is a consequence of the same AI-command defect criterion (b) fails
   on". That is half right and the half matters.** Speed and the steer byte are mechanically coupled
   by construction — `m = err * speed * 0.0030034` (`0x00416648`/`0x00416656`), so a speed error is a
   proportional steer-byte error, and removing the start boost collapses `c1_median` 52.5/38.0/49.0 to
   **0.0**. But car 1's **heading** residual is **not** the speed residual: it is stable across the
   whole speed-tolerance ladder (0.790 / 0.8918 / 0.9764 deg at 1/5/10 % matching), and car 1's
   `c0_distinct` is bound by the heading-error envelope — the port's `err` spans (1.0, 4.23 deg]
   against the original's (1.0, 86.56 deg], which no speed scaling can manufacture. **Coupled, not
   the same defect.**
2. **The command-share numbers in §"reproduces" above are not a finding of mine.** They were already
   measured and attributed; I present them only as a self-test that this build reproduces the
   baseline.

## Constraint that binds any fix chosen from here

**Criterion (e) currently PASSES 3/3, and the speed-reducing option has already been rejected by a
recorded user decision** (`ROADMAP.md:2400`, commit `e0b35ff9`: KEEP the seed). The `NO_START_BOOST`
arm is the measured precedent: it cut the window excess from +41/+40/+34 % to +31/+21/+15 % and
**regressed (e) 6 of 6 by -46 % to -90 %**. So any intervention here must be gated on **not
regressing the two gated (e) statistics**, not merely on reducing the distance ratio.

Also carried forward (`RESULT_STEP2.md:142-148`): the `ORIG speed` counterfactual is a
**necessary-condition bound, not a promise** — it holds `err` fixed while in the live loop a changed
speed changes the trajectory and therefore `err`, and the same substitution makes cars 2 and 3 worse.
