# PRE-REGISTRATION (UNRUN) — leg F1: localize the long-run nondeterminism

Date 2026-10-07. Follows `verify/d3_consumer_20261007/RESULT_E1.md` §5, which measured the
blocker: three 240 s Training captures with identical knobs produced three different matches
(round-2/3 score tuples, 5 vs 3 vs 3 rounds, max AI lap 8/1/4), while aggregate scorers stayed
stable. User decision 2026-10-07: **fix long-run determinism first, then run E2 with a real
outcome gate.**

**Status at commit time: UNRUN.** Nothing below has been executed and no source has been edited.

## 0. Scope

- F1 is **diagnosis only**. It makes **no** decision about a fix and ships **no** code.
- No C-level. `original/` untouched. No default-ON change.
- The concrete fix mechanism is deliberately **not** registered here: F1 exists because the source
  of the divergence is not yet known, and registering a mechanism before measuring it would be
  guessing. F2 is registered separately, after F1 reports.

## 1. Definitions, fixed now so F2 cannot move the goalposts

| name | definition |
|---|---|
| `R-PREFIX` | the number of leading frames over which two runs' `MASHED_AI_STEPDUMP` rows are **cell-identical** across all 77 columns, for every dumped car, matched on `(frame, seq, v)` |
| `R-ROUND` | the ordered list of `RULE-EVAL` lines (`round`, `r`, `timer`, `scores`) from `mashed_re.log`, compared as text |
| `R-SCORER` | criterion (e)'s six digits (`ai_speed_env.py --check`) and criterion (b)'s band count (`ai_ctrl_window.py --check`) |

**The target F2 must hit, registered now:** `R-ROUND` **identical across 3 repeats** of a 240 s
run with identical knobs. That is the property E2's outcome gate needs and the only one that
counts as "fixed". `R-SCORER` already reproduces and is therefore not evidence of a fix.

## 2. F1 measurements (all offline, on captures already committed at `4a0efcc2`/`6ade0f85`)

No new game runs. Inputs: `verify/d3_consumer_20261007/{L4,L4b,L4c}.csv` and their `.log`s — three
runs with identical knobs, so any difference between them is the nondeterminism itself.

| step | what is measured |
|---|---|
| `F1-PREFIX` | `R-PREFIX` for the pairs (`L4`,`L4b`), (`L4`,`L4c`), (`L4b`,`L4c`). Reported as a frame index and as a fraction of the run |
| `F1-FIRSTCOL` | at the first differing frame, **which columns** differ and by how much — the earliest-diverging quantity, named |
| `F1-RATE` | per-run frame count and mean frames/second, to quantify how much the wall-clock rate itself differs (13096 / 13792 / 13790 over 240 s are already known) |
| `F1-ROUND` | `R-ROUND` for the three runs, to confirm where in the match the outcomes first differ |

**Control that can fail (`F1-CTRL`):** the same `R-PREFIX` computation applied to a pair of runs
that are *supposed* to differ — `L4` vs `L10` (different `MASHED_ROUND_RULE`). If `R-PREFIX` for
the identical-knob pairs is not materially larger than for this pair, the metric is not measuring
what it claims and F1 reports that instead of a localization.

## 3. Decision rules

- `F1-PREFIX` large (divergence starts late, after a round boundary or an elimination) →
  the suspect is the round/elimination machinery, not the per-frame integrator.
- `F1-PREFIX` ≈ 0 (runs differ within the first frames) → the suspect is the frame timebase or a
  time-seeded PRNG, and a fixed-timestep mode is the candidate fix.
- `F1-FIRSTCOL` naming a single column → that quantity is the carrier; F2 targets its producer.
- `F1-CTRL` failing → report the metric as uninformative; do **not** proceed to F2 on it.

No fix is authored until F1 reports. A static source survey of the timestep plumbing, existing
determinism knobs, PRNG seeding and wall-clock reads in race logic is running in parallel on the
worker account; its findings are evidence for F2's design, not a substitute for F1's measurement.

## 4. Non-goals

Leg B; leg E2 (blocked on F2); any default-ON change; any edit to `original/`; any claim that
determinism is fixed on the strength of `R-SCORER`.
