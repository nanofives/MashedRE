# RESULT — branch 2 WIRED and reached 18,481 times. `W-TOOK` FAILS: it never fires at the real call site.

Date 2026-10-08. Pre-registration: `PREREG_WIRE.md`, committed **unrun** at `f6761555`.
**RAN.** Default-OFF behind `MASHED_WIRE_B2`. No C-level. `original/` untouched, `.asi` untouched.

Raw: `W2_{base,off,on,on_r2}.csv`, `Wdiag.gates.csv`.

## 0. Verdict first

> **`W-TOOK` FAILS, and the wiring is not at fault.** The branch is reached **18,481 times** in
> `ControlStep`, and `FUN_004148b0` returns **0 on every one of them**. `ctrl` is never written, so
> the ON and OFF arms are byte-identical.
>
> **The 201 firings in `RESULT_GF1C.md` were an artefact of the probe's cadence.** The gates probe
> called the predicate unconditionally for all 4 cars every frame (55,432 calls). The real call
> site is **car-alive gated** and runs 18,481 times, on a subset of frames where the firing
> condition never co-occurs.

| gate | verdict |
|---|---|
| `W-TOOK` | **FAIL** — `W2_off` and `W2_on` byte-identical (`e9cd8578`) |
| `W-KNOBOFF` | **PASS** — `W2_base` = `86b7b2bb`, the **tenth** build to match |
| `W-DET` | **PASS** — `W2_on` / `W2_on_r2` byte-identical |
| `W-SHAPE` | **NOT READABLE** — no firing frame exists to check |
| `W-NOREG-E` / `-B` | **NOT READABLE** — nothing changed to regress |

**Decision rule applied as registered:** "`W-TOOK` fails → the wiring is inert; report and stop, do
not widen scope to force an effect." No further knobs were added to chase a firing.

## 1. The wired site is reached — this is not a dead-code failure

`Wdiag.gates.csv`, cumulative counters placed on the **wired** call site itself (not the probe):

| car | reached | `ret != 0` | fire |
|---|---:|---:|---:|
| v0 | **0** | 0 | 0 |
| v1 | 1,446 | 0 | 0 |
| v2 | 3,177 | 0 | 0 |
| v3 | 13,858 | 0 | 0 |
| **total** | **18,481** | **0** | **0** |

`v0 = 0` is correct: `ControlStep` runs only for AI cars. The uneven v1/v2/v3 counts are the
`Ai_Standalone_Tick` gate `if (s_host.car_alive(v) == 1) VehicleStep(v)` — cars 1 and 2 are not
alive for most of this rule-4 scenario.

## 2. Why the probe fired and the wired site does not

Site 114 requires **both** `Prog(v) == 0` (the car's own refdist) **and** `6.5 < Prog(last)` where
`last` is the only state-`1` car, the player.

The refdist data is **identical in both captures** — car-0 refdist exceeds 6.5 on **2,149/13,858**
rows in `GF1c_on` and on **2,149/13,858** in `Wdiag`. So the difference is not the values.

It is **which frames each call site samples**:

- the gates probe runs at `TrackRenderer.cpp:3513`, unconditionally, **4 cars × every frame**;
- the wired branch runs inside `Ai_Standalone_Tick` (`:3511`), **car-alive gated**, 18,481 times.

On the frames `ControlStep` actually visits, `Prog(v) == 0` and `Prog(0) > 6.5` never co-occur.

**This retro-validates the caution already recorded** in `RESULT_GF1C.md` §3 — that the 201 were
measured by a probe which is a different call site and which mutates `TimerAt`/`RankAt`. That
caution turned out to be the whole story, and the firing count should not have been carried forward
as evidence the lane was close to working.

## 3. Two harness errors of mine, recorded

- **The first `W` batch was under-specified.** It set only `MASHED_SLOTSTATE_SEED` +
  `MASHED_SLOT_PLAYER`, omitting `MASHED_REFDIST` (which feeds `Prog` at `:101`) and
  `MASHED_A364_RESET` (without which `idx364 = 0` makes `:94`'s `E470(idx364) == 1` *true* — the
  player store I had just landed — pushing the table index to 25 and closing `:99`). Reporting
  "inert" off that would have been a false negative. Re-run as `W2_*` with the full stack; the
  verdict did not change, but the first batch could not have supported it.
- **A documented namespace trap, walked into.** I wrote `namespace mashed_re { namespace Ai {` in a
  TU already inside `mashed_re::D3d9Render`, producing `mashed_re::D3d9Render::mashed_re::Ai`. A
  comment a few hundred lines above, from the H3 leg, warns about exactly this. Flat `extern "C"`
  linkage now.

## 4. What IS established

- The wiring is **transcribed faithfully** (`PREREG_WIRE.md` §1: `ctrl[4]` untouched, early return
  before the mode commit and steer-history stores, LOS from own XZ to the predicate's output) and
  is **reached**. It is not dead code and not misplaced.
- The whole prerequisite chain works end to end: seed → player slot → `E470(0) == 1` → `last`
  resolves → the body runs its full gate chain, 18,481 times.
- The default build is untouched (`W-KNOBOFF`, tenth build).

## 5. What is NOT claimed

- **No C-level.** Nothing fired, so there is no behaviour to compare.
- Not that branch 2 is wrong. Every input it reads is now live, and it is returning 0 **correctly**
  for the state it sees.
- Not that it would stay inert in another scenario. `ControlStep`'s visit set is scenario-dependent
  (car-alive gating), and the original's 64 firings came from a different scenario and window.
- Not that (b)/(e) are unaffected by a *firing* branch 2 — that remains unmeasured, because it has
  never fired at the real call site.

## 6. Next

1. **The honest next question is the scenario, not the code.** The original fires branch 2 64 times
   in a 220-call window; the port's `ControlStep` visit set never satisfies the condition. Driving
   the port through the original's scenario (`o_t1`'s argv) is what would make the two comparable —
   `GF1-CALLWISE`, still unrun.
2. Car-alive gating (v1 1,446 / v2 3,177 / v3 13,858 of 13,858 possible) deserves its own look: if
   the port eliminates cars the original keeps alive, that is upstream of this lane entirely.
3. `MASHED_WIRE_B2` stays default-OFF. It is faithful, reached, and inert.
