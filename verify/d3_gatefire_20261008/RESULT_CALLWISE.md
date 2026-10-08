# RESULT — `GF1-CALLWISE` is NOT reachable by env alignment. The rule is not the discriminator.

Date 2026-10-08. Follows `RESULT_WIRE.md` §6 item 1, chosen by **USER DECISION (Mariano,
2026-10-08)**: "Drive the port through o_t1's scenario to make GF1-CALLWISE possible."

**RAN**, 2 arms at rule 0. No C-level, nothing default-ON, `original/` untouched.
Raw: `R0_{on,off}.gates.csv`, `R0_{on,off}.step.csv`.

## 0. Verdict first

> **Aligning the rule changed nothing.** At rule 0 the wired call site reports **exactly** the
> counters it reported at rule 4 — 18,481 reached, **0** non-zero returns, **0** firings — and
> `W-TOOK` still fails (`R0_off.step.csv` and `R0_on.step.csv` byte-identical, `9b299306`).
>
> **And `GF1-CALLWISE` cannot be produced by env mapping at all**, because the two sides construct
> the race differently, not merely with different parameters. §2.

| | rule 4 (`Wdiag`) | rule 0 (`R0_on`) |
|---|---:|---:|
| v1 / v2 / v3 reached | 1,446 / 3,177 / 13,858 | **1,446 / 3,177 / 13,858** |
| total reached | 18,481 | **18,481** |
| `ret != 0` | 0 | **0** |
| fire | 0 | **0** |

## 1. The identical reach counts are themselves a finding

The per-car visit counts are **bit-identical across the two rules**. So `ControlStep`'s visit set
is **rule-independent** — it is governed by `Ai_Standalone_Tick`'s
`if (s_host.car_alive(v) == 1) VehicleStep(v)` gate and nothing the race rule touches. That rules
the rule out as the discriminator between the probe's 201 firings and the wired site's 0, and
leaves `RESULT_WIRE.md` §2's diagnosis standing unchanged: the probe sampled frames
`ControlStep` never visits.

It also sharpens the open question from `RESULT_WIRE.md` §6 item 2: **cars 1 and 2 are alive for
1,446 and 3,177 of 13,858 frames** regardless of rule. That is 10% and 23%.

## 2. Why env alignment cannot produce `GF1-CALLWISE`

> **CORRECTED 2026-10-08 by [`SCOPE_CALLWISE.md`](SCOPE_CALLWISE.md).** This section's conclusion —
> that the differing race *construction* blocks `GF1-CALLWISE` and makes it "a harness task" — is
> **WRONG**. The comparison window is **call-indexed and self-anchoring**
> (`ai_ctrl_window.py:25`: car `v`'s calls `[i0, i0+220)`, `i0` = first call with `c4 != 0`), which
> is how criterion (b) already scores both sides; `PREREG_STEP2.md:10` records "median call index
> 109.5 on both sides". The original's per-call reference is **already committed**
> (`o_t3.msd.aistep.csv`, 64 firings = 31/4/29). What is actually missing is **two per-call columns
> on the port side plus a scorer** — one build and one run. The axis table below is still accurate
> as a description of the two drivers; only the conclusion drawn from it was wrong.


The scenario axes, checked rather than assumed:

| axis | `o_t1` (original) | port | aligned? |
|---|---|---|---|
| track | `--track 0` = **Training** (`scenario_launch.py:2694`) | `MASHED_TRACK_VIEW=Training` | **yes, already** |
| rule | `--rule 0` (default) | `MASHED_ROUND_RULE=0` this run | **yes, now** |
| player car | `--car 0` → `DAT_0067ea98` | **no equivalent** — the port's `MASHED_CAR` is a *vehicle-piz selector* for track-view mode (`exe_main.cpp:8450-8457`), not a car index | **no** |
| race construction | `scenario_launch` drives the **original** through its own menu into a real QuickRace (`--mode 10`), writing engine globals via Frida | the port runs its own race flow under `MASHED_TRACK_VIEW` + `MASHED_DETERMINISTIC` | **no** |

**The last row is the blocker.** These are two different drivers, not two spellings of one
scenario. `GF1-CALLWISE` pairs call *n* on one side with call *n* on the other; that requires the
same race construction, not merely the same track and rule.

**A correction:** in reporting the previous leg I listed `--car 0` vs `MASHED_CAR=1` as a scenario
mismatch. That was a misreading — `MASHED_CAR` is not the player-car index. The real mismatch is
that the port has no player-car-index knob at all.

## 3. What is NOT claimed

- Not that branch 2 would stay inert under the original's race construction. **Untested**, and it
  is exactly what `GF1-CALLWISE` would answer.
- Not that the car-alive gating is wrong. It is measured and rule-independent; whether the port
  eliminates cars the original keeps alive is **unmeasured** (`RESULT_WIRE.md` §6 item 2).
- No C-level. Nothing fired; there is nothing to compare.
- `W-SHAPE`, `W-NOREG-E`/`-B` remain unreadable for the same reason as before.

## 4. Next

1. **Measure the car-alive gating against the original** — do cars 1 and 2 spend 90% / 77% of the
   race not-alive on the original too? `orig_rampwatch.py` can poll `FUN_0046c7b0`'s substrate the
   same way it polled `E470`. This is cheap, and if the port eliminates cars early it is upstream
   of this lane entirely and would explain the empty visit set.
2. `GF1-CALLWISE` needs the port driven through a race constructed like the original's — a harness
   task, not an env change, and worth its own scoping before anyone attempts it.
3. `MASHED_WIRE_B2` stays default-OFF: faithful, reached, inert.
