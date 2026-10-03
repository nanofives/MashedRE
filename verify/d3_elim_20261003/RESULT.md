# RESULT — D3: the elimination confound, and what carries the AI over-speed

Two steps, each pre-registered **unrun** and each with its own write-up:

- **STEP 1** — `PREREG_STEP1.md` (`e8d038ab`) → [`RESULT_STEP1.md`](RESULT_STEP1.md) (`5e39160c`)
- **STEP 2 / 2B** — `PREREG_STEP2.md` + the amendment `PREREG_STEP2B.md` (`23cb9d7d`) →
  [`RESULT_STEP2.md`](RESULT_STEP2.md) (`46cd19df`)

Branch `race/first-frame-parity`, nothing pushed. The user's decision (`e0b35ff9`) keeps the
AI start boost, so it is **ON in every arm here**.

## The two verdicts

1. **The player-elimination confound's share of the 2026-10-03 arm A vs arm B delta is
   ZERO.** Measured with a default-OFF `MASHED_NO_ELIM` control proved live on two
   independent channels: **0 of 2640** scored byte-slots differ over the window and **0 of
   660** on speed and position. Every number in `verify/d3_noboost_20261003/RESULT.md`
   stands. U-9185 evidence item (a) is **RESOLVED**.
2. **The surviving over-speed is a COMMAND defect, not a physics one.** Pre-onset the port's
   whole force chain reproduces the original's speed to **0.98 / 0.23 / 0.23 %**. After the
   onset the original lifts or brakes on **149 of 660** window calls where the port holds
   full throttle, and **all 149** are three unported branches of `FUN_00416250`, with **zero
   unexplained**. U-9185 item (c) is **ANSWERED**; the three sources moved to **U-9186**.

## No code change beyond one default-OFF knob

`MASHED_NO_ELIM` (`TrackRenderer.cpp` `NoElim()`, read at exactly the two elimination
blocks). **No C-level moved, no band moved, no scorer was edited**, and `ea1` reproduces the
previous build's `a1` with **0 differing slots**, so the knob does not move the default build.

## Two gates failed, both reported rather than re-thresholded

- **`PREREG_STEP2.md` §2.0** — the accel/brake model reproduces the **port** 220/220 and the
  **ORIGINAL** only 0.636 / 0.936 / 0.750. §2.1–§2.5 **did not run and are reported
  nowhere**; `PREREG_STEP2B.md` replaced them unrun. The reason the gate refused is the
  finding: the model is the port's mode-0 arm and the port pins `mode = 0`.
- **`PREREG_STEP2B.md` G2B-LIVE** — the registered *"≥ 99 % of mode-7 calls carry
  `c4 == 0x40`"* measures **61.3 %**. The gate was written in the wrong direction; the exact
  relation is the converse, **49/49** on four runs.

## Three corrections to earlier filings

1. `VehicleStep(0)` runs in **neither** port arm, and not on the original either.
2. **Both** port arms eliminate the player (arm A window call 112, arm B 146).
3. On the **ORIGINAL** the player is alive through the whole scored window and dies on the
   **first frame after it**.

## What the next session does first

Wire `FUN_004148b0` into the standalone — **64 of the 149 calls, and no new reversing is
needed** (`0x004148b0` is C3 `impl`, `0x00416060` is C3 `impl` with a committed `frida_diff`;
all three TUs are `asi_sources.rsp`-only). **Behind a pre-registered knob-took witness
proving it is not inert** — `LeaderTimer`'s inputs may be `.bss` zeros in the standalone, the
trap `FUN_00484c70` already fell into. Full ordering in `re/NEXT_SESSION.md`.

## Disclosure

The rule-3 oracle guard writes `log/rules_oracle_rule3.json` by design, and this run
**overwrote the uncommitted modification that file carried at session start and that is not
this session's**. It was never staged and is in no commit here; both versions are GREEN with
MISMATCH 0 and differ only in run-length counters.
