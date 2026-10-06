# RESULT — LEG 1, `G-PORTHULL` and friends: 6 of 6 gates PASS

Pre-registration: [`PREREG_CARCAR.md`](PREREG_CARCAR.md) §2, committed **UNRUN** at
`3404612d` together with the finding note
[`re/analysis/CARCAR_CALLSITE_2026-10-05.md`](../../re/analysis/CARCAR_CALLSITE_2026-10-05.md).
Nothing below was measured before that commit.

**No C-level moved. No band moved. No tracker mutated by this leg. `original/` untouched.**
The only source change is 19 **appended** columns on the **default-OFF**
`MASHED_AI_STEPDUMP` (`D3d9Render/TrackRenderer.cpp`), all of them raw
`VehiclePhysics_RecordF32` / `RecordI32` reads. **Legs 2 and 3 did NOT run** — see §4.

## 1. The runs

| arm | build | command |
|---|---|---|
| `P1` | HEAD + the 19 appended columns | `py -3.12 re/tools/sa_capture.py verify/d3_carcar_20261005/P1 8,30,60 MASHED_MUTE=1 MASHED_TRACK_VIEW=Training MASHED_CAR=1 MASHED_ROUND=1 MASHED_WIN_POS=primary-bl MASHED_TITLE="d3 carcar P1" MASHED_AI_STEPDUMP=verify/d3_carcar_20261005/P1.csv` |
| `B1` | the committed build at `3404612d`, the instrument `git stash`ed out and rebuilt | same recipe, `MASHED_AI_STEPDUMP=verify/d3_carcar_20261005/B1.csv` |

`B1` exists only so `G-INERT` compares against a real baseline instead of an assertion.
The previous session's `A1.csv` was **not** reused: it was captured 38 commits ago and its
build SHA was not recorded.

## 2. The gates, each with its threshold, the number it is compared against, and its denominators

| gate | threshold | measured | denominators | verdict |
|---|---|---|---|---|
| `G-PORTHULL` | each of `e01/e02/e03` within **1e-3** of `0.437600 / 0.977100 / 1.070616` | medians `0.437600 / 0.977100 / 1.070616`, deviations **7.55e-10 / 1.61e-08 / 1.90e-07** | within tol on **7772 of 7772** non-degenerate = **1.000000** vs min-frac 0.950000; **7775** total rows, so **3** all-zero pre-race rows excluded | **PASS** |
| `G-RADIUS` | `|rec_4a4 - 0.6780367493629456| <= 1e-3` | single distinct value **0.678002715**, deviation **3.40e-05** | **7775 of 7775** rows (100 % required) | **PASS** |
| `G-SEL` | `sel_9a8 == 0` and `sel_9ac == 1` | distinct sets `{0}` and `{1}` | **7775 of 7775** rows (100 % required) | **PASS** |
| `G-SLOT0-ZERO` (control, part a) | slot `[+0x9a8]`'s translation row all-zero on **100 %** | all-zero | **7775 of 7775** rows | **PASS** |
| `G-SLOT0-ZERO` (control, part b) | slot `[+0x9ac]`'s row within **1e-3** of `own_x` / `own_z` on **>= 95 %** | agrees | **7772 of 7775** = **0.999614** vs 0.95; non-zero on 7772 of 7775 | **PASS** |
| `G-INERT` | **0** differing cells | **0** | **0 of (46 pre-existing cols x 7775 shared `(frame,seq,v)` keys = 357,650)**; `P1` 7775 rows, `B1` 7955 rows, and `B1`'s column list is a verbatim **prefix** of `P1`'s | **PASS** |

Scored by an inline reader over the two committed CSVs; the original-side reference edges
come from `re/tools/hull_invariants.py`, which produced them from committed `.msd`
captures and not from this run.

## 3. What the control actually bought, because it is the point of the design

`G-SLOT0-ZERO` was registered with **two halves that the null hypothesis cannot separate**:
"the new columns read nothing" predicts both ring slots zero; the finding note's §5
predicts slot `[+0x9a8]` zero **and** slot `[+0x9ac]` equal to `own_x` / `own_z`, a value
produced by a different code path (`Ai::StepLocals`, not the record). The measurement
returns the second: **0 of 7775 vs 7772 of 7775.** So the instrument is live and the port
really does leave one of the two ring slots unpublished — the claim is not an artefact of
reading a dead offset.

Two observations, recorded as observations and **not** as gates:

- The port's hull edge spread is far tighter than the original's: `e01` ranges
  `[0.437598, 0.437602]` on `P1` against `[0.437547, 0.437653]` on `orig_sl1.msd`. Both
  pass the same `1e-3`. Consistent with the port's basis being orthonormalised each
  substep while the original's x87 chain drifts; **not established**, and nothing here
  depends on it.
- `P1` and `B1` ran to different lengths (7775 vs 7955 dump rows) while agreeing on
  **0 of 357,650** cells over the shared keys. The length difference is the known
  run-to-run spread with opponents updated (`ROADMAP.md`, the 2026-09-29 controlled-arm
  amendment), not a divergence in content.

## 4. Why legs 2 and 3 did NOT run, and what they need

Leg 1 confirms the finding note's §5 on the port itself: the hull and the radius are
**exact and present**, and the **only** missing input is ring slot `[+0x9a8]`. Leg 2
publishes it, and `PREREG_CARCAR.md` §3 registers in advance that **this is not inert** —
`Vehicle/VehicleControl.cpp:103`, `Vehicle/PhysicsChainHooks.cpp:536` and `:2749` all read
`[0x9a8]*0x40 + 0x928` and currently receive a zero matrix, so A4's and A6b's `wheelBlock`
input changes inside the D2-certified player solver.

That leg owns five gates — `G-NOREG-E` (six numbers, 3 of 3 cars), `G-NOREG-B`
(<= 13 of 30 bands), `G-BANDS-UNEDITED`, `G-KNOBOFF`, `G-DET` (3 repeats) — plus the
inverse control `G-SLOT0-LIVE`, whose "before" value leg 1 has now established. It is a
capture block of its own and was not started at the end of a session; the pre-registration
and its FAIL clause stand unchanged and unrun.

**Leg 3 is not startable before leg 2 passes**, and that is a result rather than a
scheduling note: with slot `[+0x9a8]` all-zero, the proximity gate at `0x00470b44..0x00470ba9`
compares two `(0,0,0)` centroids and reduces to `0 < radSum²`, i.e. **true for every pair
on the track at every substep**, and `CarCarContacts.cpp:93`/`:97` would use the contact
point's absolute world position as the angular lever arm instead of a ~0.5-unit arm.
Wiring the call now would ship a default path that fires on all pairs with lever arms
wrong by one to two orders of magnitude. That is not a faithful call site.
