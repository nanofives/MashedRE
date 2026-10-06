# PRE-REGISTRATION — give `0x00469df0` a call site

**COMMITTED UNRUN.** Nothing in §2..§5 has been executed. §1 is already measured and is
stated as a *result*, not a gate, so that a reader can tell the two apart.

Finding this rests on: [`re/analysis/CARCAR_CALLSITE_2026-10-05.md`](../../re/analysis/CARCAR_CALLSITE_2026-10-05.md).
Anchor `BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E` verified before
any tool ran and must be re-verified before arming each leg below.

## 0. The three things this pre-registration is designed not to do

1. **Not treat "linked" as "called".** Every leg that claims the call happens carries a
   counter that is read on a path which cannot execute unless the call executed
   (the `GameSave_LastReadBytes() == 151456` pattern).
2. **Not score a denominator without checking its population.** Every gate below prints
   its numerator, its denominator, AND the total it was drawn from, on one line.
3. **Not use a control whose expected value is indistinguishable from the default.**
   Each leg names its positive control and the value the default *cannot* produce.

## 1. ALREADY MEASURED (result, not gate) — the hull chain is exact and present

`re/tools/hull_invariants.py` over four committed original captures, two cars:
**10,278 of 10,278 non-degenerate frames** have the three SAT-hull edge lengths within
`1e-3` of `0.437600 / 0.977100 / 1.070616`, the invariants of `kContactHullBox`'s top
face; largest median deviation **4.43e-05**, eleven of twelve `<= 8.94e-08`. Denominator
excludes the 2 all-zero (pre-race) frames per capture, printed by the tool.

This is why the legs below are about ONE missing input and not about the hull.

## 2. LEG 1 — `G-PORTHULL`: does the port's own hull match?

**Change:** append columns to the existing default-OFF `MASHED_AI_STEPDUMP`
(`D3d9Render/TrackRenderer.cpp:3973`), **appended at the end** so every existing column
keeps its position: the twelve floats at `+0xa28/+0xa34/+0xa40/+0xa4c`, plus `rec_95c`
and the `+0x998/+0x99c/+0x9a0` triplet, plus `rec_4a4`, plus `sel_9a8` and `sel_9ac`.
No other file is touched. No default-path behaviour changes.

| gate | threshold | compared against | denominators |
|---|---|---|---|
| `G-PORTHULL` | each of `e01/e02/e03` within **1e-3** | `0.437600 / 0.977100 / 1.070616` | `>= 95 %` of rows whose four hull points are not all-zero; print `within / nondegenerate / total_rows` |
| `G-RADIUS` | `|rec_4a4 - 0.6780367493629456| <= 1e-3` | the original's value, constant on 4 of 4 captures and 2 cars | **100 %** of rows; print `ok / total_rows` |
| `G-SEL` | `sel_9a8 == 0` and `sel_9ac == 1` | the original's, constant on 10,286 of 10,286 frames | **100 %** of rows |
| `G-INERT` | **0** differing cells | the pre-existing columns of a baseline CSV captured from the committed build on the same recipe | `cols_before x rows`, both printed |

**Positive control that can FAIL, with an expected value the default cannot produce:**
`G-SLOT0-ZERO` — `rec_958/rec_95c/rec_960` must be **0.0 on 100 %** of rows *while*
`rec_998/rec_99c/rec_9a0` must be **non-zero and within 1e-3 of `own_x` / `own_z`** (the
`y` component is unconstrained) on `>= 95 %` of rows. The null hypothesis "the new columns
read nothing" predicts **both** triplets zero. The claim in
`CARCAR_CALLSITE_2026-10-05.md` §5 predicts one zero and one equal to a column measured by
a different code path. Those two outcomes are distinguishable by inspection.

**Registered FAIL clause.** If `G-PORTHULL` fails, legs 2 and 3 do **not** run and the
failure is reported as the finding: the port's hull is geometrically wrong and that is a
bigger defect than the missing call site.

## 3. LEG 2 — `G-SLOT0`: publish ring slot `[+0x9a8]`, and gate it ALONE

**Change:** `SyncContactRingMatrix` (`Vehicle/VehiclePhysicsRun.cpp:378-387`) publishes
the basis + position into **both** ring slots instead of only `I(r, 0x9ac)`. Default-ON
with the A/B revert `MASHED_RING_SLOT0=0`, per the v3 flag rule (a flag may only turn the
ported behaviour OFF). **No car<->car call is added in this leg.**

**Registered expectation, stated before the run: this is NOT inert.** Three sites read
the `+0x9a8`-selected slot and currently receive a zero matrix —
`Vehicle/VehicleControl.cpp:103`, `Vehicle/PhysicsChainHooks.cpp:536` and `:2749`, all
citing `0x0047068e`. So A4's and A6b's `wheelBlock` input changes. Predicting "no change"
here would be a control that agrees with the default by construction.

| gate | threshold | compared against | denominators |
|---|---|---|---|
| `G-NOREG-E` | every printed digit equal | `ai_speed_env.py --check`: `launch` **1426.4 / 2053.0 / 2055.2**, `ft_median_m0` **2550.6 / 2053.0 / 2278.2** | **3 of 3** cars, each statistic; print all six numbers both arms |
| `G-NOREG-B` | **<= 13** failing bands | the committed baseline's **13 of 30** (5 / 4 / 4) | 30 bands over 3 cars; print the per-car split AND the band names |
| `G-BANDS-UNEDITED` | both empty | `git diff --stat` and `git status --porcelain` on `re/tools/ai_ctrl_window.py` and `re/tools/ai_speed_env.py` | n/a — proof, not assertion |
| `G-KNOBOFF` | every printed digit equal | the committed build's own numbers, with `MASHED_RING_SLOT0=0` | 3 of 3 cars — proves the knob is a true revert |
| `G-DET` | identical to every printed digit | 3 repeats of the same arm | 3 of 3 |

**Positive control that can FAIL:** `G-SLOT0-LIVE` — re-run leg 1's dump and require
`rec_958/rec_95c/rec_960` to be **non-zero and within 1e-3 of `own_x`/`own_z` on >= 95 %**
of rows, i.e. the exact inverse of leg 1's `G-SLOT0-ZERO`. The default produces zero;
only the change can produce agreement with `own_x`/`own_z`. The same two columns therefore
carry a control in both directions, and leg 1 establishes the "before" value before leg 2
can be believed.

**Registered FAIL clause.** If `G-NOREG-E` or `G-NOREG-B` fails, **STOP**: revert the
commit, report the regression with its numbers, and do **not** flip the default or widen
the threshold. Shipping a faithful change that regresses a met criterion is a user
decision (the precedent is the `MASHED_NO_START_BOOST` keep/remove call, U-9185/U-9142).
Leg 3 does not run.

## 4. LEG 3 — `G-CARCAR`: the pair loop

**Change:** insert the §3 loop of the finding note immediately before the `break;` at
`Vehicle/VehiclePhysicsRun.cpp:1093`, calling

```cpp
Collision::VehicleCarCarContact(recJ, recI, pass)   // 0x00469df0 via 0x00470bcd
```

**with `recJ` first** — the other car, `in_EAX`; `recI` second — the self car, `param_1`.
Default-ON with the revert `MASHED_CARCAR_CONTACT=0`. Gates `g_participantCount` and
`Fi_GameMode()` (which returns 6, in `{6,7,10,0xb}`) as the original does.

| gate | threshold | compared against | denominators |
|---|---|---|---|
| `G-CALLED` | `>= 1` entry **and** `>= 1` non-zero return | 0 — the current state, proved by the absence of any call site | a 4-car race; print entries, non-zero returns, and the `(i,j)` pair histogram |
| `G-NOOPP` | **exactly 0** entries | `G-CALLED`'s count with opponents stepped | same recipe with `MASHED_MEASURE_NOOPP=1` (`TrackRenderer.cpp:3474`, `aiCarN = 0`) |
| `G-PAIR` | `j > i` on **100 %** of calls, and `recJ != recI`, both non-null | the loop's own `j = i+1` bound | all calls; print the count |
| `G-NOREG-E` / `G-NOREG-B` / `G-BANDS-UNEDITED` / `G-KNOBOFF` / `G-DET` | as in §3, re-run with the loop live | the §3 arm's numbers, not the pre-leg-2 ones | as in §3 |

`G-NOOPP` is the control that can fail: with no opponent stepped, no pair can be in range,
so a non-zero count would prove the counter is wired to something other than the call.

**What leg 3 does NOT claim, registered now so it cannot be claimed later.**
`0x00469df0` is **C2**. A call site is not a behavioural diff, so **no C-level moves** on
this RVA or on `0x004709a0` (which has no port body at all — the port reproduces its
structure across `VehiclePhysics_StepCar` and its caller, it does not implement the
function). `re-classify` is used only to correct `ContactStubs.cpp:100-110`'s stale
U-9155 citation and to record the two comment-level plate corrections
(`in_EAX` is not `this`; `[699]` is byte `0xAEC`, not `0xAF0`).

## 5. What would make the whole thing a dead end, said in advance

- **(b) does not move.** Expected as the base case, not as a disappointment: the
  2026-10-02 counterfactual matrix had **no arm passing (b) on any car**, and
  `ROADMAP.md:1929-1932` records that the opponents already move the player's median speed
  **2538 → 691** with `0x00469df0` never running. The coupling that does that is shared
  mutable state, and nothing in legs 1..3 touches it.
- **Leg 2 regresses.** Then the finding is that the port's A4/A6b have been running on a
  zero `wheelBlock` and are tuned around it — which is a D2 question, reported and handed
  over, not fixed here.
- **Port-only scaffolding gets blamed.** Memory
  `broadphase-standin-plane-test-was-the-sink` / `reassertcontacts-promotion-is-port-only`:
  before blaming the transcription at `0x00469df0`, check the ORIGINAL for the bound being
  blamed.
