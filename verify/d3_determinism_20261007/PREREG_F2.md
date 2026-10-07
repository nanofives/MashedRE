# PRE-REGISTRATION (UNRUN) — leg F2: make the 240 s run reproducible

Date 2026-10-07. Follows `RESULT_F1.md`. **Status at commit time: UNRUN.**

No C-level. `original/` untouched. No default-ON change. No source edit — F2 runs the existing
binary with knobs that already exist.

## 0. What changed since PREREG_F1

F1 could not localize because `R-PREFIX` failed its control. A static source survey of the timebase,
determinism knobs and PRNG seeding (worker account, 2026-10-07, `SURVEY_F2.md`) then reported two
facts that make the localization question moot for now:

1. **A determinism mode already exists and is OFF by default.** `MASHED_DETERMINISTIC=1`
   (`exe_main.cpp:8109`) replaces the wall clock with a frame counter (`exe_main.cpp:404-408`) and
   **pins the physics accumulator to one fixed step per frame** (`exe_main.cpp:2864`,
   `sim_real_dt = kDetStep`). `MASHED_DET_FRAMES=N` (`exe_main.cpp:8112`, `8833`) ends the run at a
   fixed frame index instead of a wall-clock kill — added precisely because a wall-clock kill lands
   at a different synthetic instant each run (`exe_main.cpp:395-400`).
2. **Every PRNG in the standalone is fixed-seeded; there is no `srand` and no time-derived seed
   anywhere** (survey §3). The AI PRNG is the ported RenderWare ring, opened once with a constant
   (`AiStandalone.cpp:677-693`). Its *stream* is deterministic; only its *call index* moves, and
   that index moves because of dt-driven branch flips (survey §5 item 3).

**The E1 captures F1 analysed did not set either knob.** `verify/d3_consumer_20261007/run_e1.ps1:17-23`
and `run_e1b.ps1:17-23` pass `MASHED_MUTE`, `MASHED_TRACK_VIEW`, `MASHED_CAR`, `MASHED_ROUND`,
`MASHED_WIN_POS`, `MASHED_AI_STEPDUMP` and optionally `MASHED_ROUND_RULE` — and nothing else. The
runs were stopped by `sa_capture.py`'s wall-clock kill at 240 s.

So E1's "round-level outcomes do not reproduce" was measured with the determinism mode **off**. F2's
first job is to find out whether it reproduces with the mode **on**. That is a knob check, not a fix,
and F2 claims nothing more than the knob check returns.

## 1. Runs

Four runs of the existing `mashedmod/build/mashed_re.exe`, sequential, each with a clean
`mashed_re.log`. Common environment, identical to E1's except for the two added knobs:

```
MASHED_MUTE=1  MASHED_TRACK_VIEW=Training  MASHED_CAR=1  MASHED_ROUND=1
MASHED_WIN_POS=primary-bl  MASHED_ROUND_RULE=4
MASHED_DETERMINISTIC=1  MASHED_DET_FRAMES=14400
MASHED_AI_STEPDUMP=<out>\<name>.csv
```

| run | differs from the others by |
|---|---|
| `F2a` | — |
| `F2b` | — (repeat) |
| `F2c` | — (repeat) |
| `F2x` | **`MASHED_SIM_HZ=59`** — the control |

14400 frames = 240 s on the synthetic clock (`DetTicks` = `g_det_frame * 1000 / 60`,
`exe_main.cpp:404-408`), matching E1's 240 s window.

`F2x` is the control F1 lacked. `MASHED_SIM_HZ` sets `kSimStep = 1/s_simHz`
(`exe_main.cpp:2865-2867`, `3047`), i.e. it changes the integration step itself, so it perturbs
driving state from the first step. Unlike `MASHED_ROUND_RULE` it cannot be inert on car motion.

## 2. Gates, fixed now

| gate | definition | passes when |
|---|---|---|
| **G-TOOK** | the knobs actually engaged: max `frame` in each `.csv`, and the frame count, per run | all four runs end at **frame 14399** (`DET_FRAMES` reached, not a wall-clock kill) |
| **G-REPRO** | **the registered target.** `R-ROUND` (PREREG_F1 §1: the ordered `RULE-EVAL` lines, compared as text) across `F2a`/`F2b`/`F2c` | **all three identical** |
| **G-PREFIX** | registered `R-PREFIX` (PREREG_F1 §1, membership differences included) for the three repeat pairs | **full overlap** for all three pairs |
| **G-CTRL** | `R-PREFIX(F2a, F2x)` — **this gate can fail** | `R-PREFIX(F2a,F2x)` < 10% of the run **and** materially below the repeat pairs |

`R-SCORER` is deliberately absent: PREREG_F1 §1 already recorded that it reproduces and is therefore
not evidence of a fix.

## 3. Void conditions — registered so a vacuous pass cannot be claimed

`MASHED_DETERMINISTIC` suppresses live and ambient input (`exe_main.cpp:2935`, gate `!g_det_clock`).
That is a behavioural change relative to the E1 captures, so:

- **VOID-ROUNDS.** If a run produces **fewer than 2 `RULE-EVAL` lines**, the match did not progress
  and `R-ROUND` is vacuous. G-REPRO cannot pass on it; report VOID.
- **VOID-TOOK.** If G-TOOK fails, nothing downstream is interpreted — the knob did not take and F2
  reports that, per memory `verify-the-harness-knob-actually-took`.
- **VOID-EMPTY.** If any `.csv` is empty or any run exits non-zero, that run is re-taken once; a
  second failure is reported, not retried further (memory `shadow-lane-failure-windows` requires two
  boots before believing a failure).

A G-REPRO pass under a VOID condition is **not** a pass.

## 4. Decision rules

- **G-TOOK pass, G-REPRO pass, G-CTRL pass** → the blocker was a missing harness knob, not a code
  defect. `R-PREFIX` is rehabilitated by a control that can fail, and **leg E2 is unblocked** and
  runs with its registered outcome gate under these knobs.
- **G-TOOK pass, G-REPRO FAIL** → determinism mode is genuinely insufficient. Per survey §5 note,
  the residual carrier is then **not** a wall-clock or PRNG read visible in
  `mashedmod/src/mashed_re/`, and F3 is a new localization leg — registered separately, on the
  rehabilitated `R-PREFIX` if G-CTRL passed.
- **G-CTRL FAIL** → `R-PREFIX` stays uninformative, exactly as in F1. G-REPRO's verdict still stands
  on its own (it does not depend on `R-PREFIX`), but no localization is claimed.
- **G-TOOK FAIL** → report the knob failure only. No other gate is read.

## 5. Non-goals

Leg B. Any source edit. Any default-ON change. Any edit to `original/`. Any claim that determinism is
fixed for the *interactive* game — F2 tests the capture harness only, and `MASHED_DETERMINISTIC`
remains OFF by default.
