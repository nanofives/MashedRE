# PREREG — U-9191 leg 3: is car 1's heading residual GENERATED or ACCUMULATED?

**Status when committed: UNRUN.** No field has been added to the dump, no build has run, no capture
has been taken and no number computed. Results go in `RESULT_LEG3.md` here.

Parent: **U-9191** (filed 2026-10-05, `verify/d3_headattrib_20261005/RESULT_HEADATTRIB.md`). Gate:
**D3 criterion (b)**, the body-heading share on **car 1**. Leg 3 was promoted by that session's
G-SPEED PASS.

---

## 1. What leg 3 has to settle, and why the earlier legs cannot

Car 1's body heading differs from the original's by a median **0.8918 deg** at matched position,
matched commanded steer `(c0, c1)` and matched speed `rec_9e4` within 5 % (n = 31), stable across the
1/5/10 % ladder. Staleness, command and speed are each excluded.

But **matched position, command and speed is not matched history**, and a body heading is an
**integral** of past yaw rate. So the residual may be:

- **GENERATED** at the matched instant — the port's yaw rate differs *there*, and the producer chain
  (angular velocity `+0x9bc`/`+0x9c0`/`+0x9c4` → `BodyOrientationIntegrate` `FUN_0046e9e0` → basis →
  `io.yaw`) is the target; or
- **ACCUMULATED** before it — the port's yaw rate matches at the sampled call and the heading offset
  was banked earlier in the window, in which case the carrier is upstream and the heading is a
  readout, not a defect.

Nothing offline can separate these: **neither side's AI-car yaw rate exists in any committed
capture.** The port's `MASHED_AI_STEPDUMP` has no angular-velocity column, and every
`scenario_launch.py` probe that samples those offsets (`--lat-bracket`, `--axis-probe`) targets the
**PLAYER**. The original side needs **no new run** — `o_t1.msd` / `o_t2.msd` carry the full `0xd04`
record per render frame for **car slot 1, an AI car**.

## 2. Instrument

**Port side.** Append **seven** columns to the existing default-OFF `AiStepDump`, read with the
`VehiclePhysics_RecordF32` call it already uses for `+0x9e4` and `+0xb0c`:

| column | offset | what |
|---|---|---|
| `rec_958`, `rec_960` | `+0x958`, `+0x960` | the record's world X and Z (memory `msd-world-position-is-the-0x928-matrix-row`) |
| `rec_9d4`, `rec_9dc` | `+0x9d4`, `+0x9dc` | body forward X and Z |
| `rec_9bc`, `rec_9c0`, `rec_9c4` | `+0x9bc`, `+0x9c0`, `+0x9c4` | angular velocity x, y, z |

**Appended at the end**, so every existing column keeps its position and `ai_posmatch.py` /
`ai_headattrib.py` are unaffected.

**Original side.** `re/tools/statediff/msd_fields.py` on `o_t1.msd` at the **same seven offsets**.

**Pairing on the record's own position, not on `own_x`/`own_z`.** Both sides read `+0x958`/`+0x960`
— identical field, identical offset, same provenance — so the pairing cannot be contaminated by any
difference between what the AI *believes* its position is and where the physics record puts it.
Radius **R = 0.12**, the same value `ai_posmatch.py:24` uses and justifies.

**The quantity is read directly, never differenced.** `+0x9c0` *is* a rate. No `dt` is needed and no
numerical derivative is taken, so there is no differencing noise term.

## 3. Known-answer check — must pass before any cross-side number is read

**KA-1: is `+0x9c0` the yaw rate at all?** On the **original alone**, over its own consecutive
`.msd` frames, fit a single scale `k` in

```
wrap180( atan2(+0x9dc, +0x9d4)[frame k+1] - atan2(+0x9dc, +0x9d4)[frame k] )  ==  k * (+0x9c0)[frame k]
```

A single constant must fit the whole run, because `k` absorbs `dt` and the unit scale. **PASS if
Pearson correlation ≥ 0.95.** FAIL means `+0x9c0` is not the yaw-rate field, or its units are not
constant, and **leg 3 stops** — no cross-side number is reported. Registered this way because the
field identification is inherited from U-9175's notes about the **player**, not measured for an AI
car.

Reported alongside: the fitted `k` and the residual scatter, and the count of frames where
`+0x9bc` / `+0x9c4` (roll/pitch rate) are non-zero — U-9175 measured those as exactly 0 on the
original for the player and ~5e-10 noise on the port, and whether that holds for an AI car is not
established.

## 4. The validity floor — and why it is NOT a position bound

U-9190 was filed today because `sa_headwatch.py` bounded a **position** while claiming an **angle**.
Leg 3 must not repeat that, so its floor is in the **same units as the claim**.

**GATE-FLOOR.** Over the matched population, compute the **original's own frame-to-frame variability
of `+0x9c0`**: median `|omega_y(k+1) - omega_y(k)|` across its consecutive `.msd` frames. That is how
much the quantity moves within one sampling step of the coarser side, so it bounds what the
per-frame-vs-per-call granularity alone could manufacture — measured in yaw-rate units, on the same
field, by the same instrument.

**A cross-side rate difference is claimable only if its median exceeds that floor by ≥ 10x.** Below
10x the rate leg is **VOID** — which is explicitly **not** the same as "the rates agree", and must
not be reported as agreement.

**Deliberately rejected as a floor:** pairing the original against `o_t2.msd`. The original is
**deterministic** over this window (measured: 0 of 220 differing rows across three capture sessions),
so that floor would be exactly 0 — the same construction-zero trap U-9188's player floor fell into.
Recorded so the weaker option is not substituted later.

## 5. Gates

| gate | PASS | FAIL |
|---|---|---|
| **KA-1** | single-scale fit correlation ≥ **0.95** | below → `+0x9c0` is not the yaw rate as assumed; **leg 3 stops**, nothing cross-side reported |
| **G-DIRECT** | the **directly measured** heading difference (`atan2(+0x9dc, +0x9d4)`, both sides, matched position) has median_abs in **[0.45, 1.80]** deg on car 1 — i.e. within 2x either side of legs 1–2b's **0.8918** | outside that band → U-9191's residual was an artifact of the `err` inversion after all, and **U-9191 must be amended or withdrawn**. This is the gate that can kill the parent row. |
| **G-FLOOR** | cross-side median `\|Δomega_y\|` ≥ **10x** the original's own per-step variability | below → rate leg **VOID**, report the floor and both medians, draw **no** conclusion about generated-vs-accumulated |
| **G-RATE** | with G-FLOOR passed: cross-side median `\|Δomega_y\|` is **≥ 20 %** of the original's median `\|omega_y\|` → **GENERATED**, and the producer chain is the target | **≤ 5 %** → **ACCUMULATED**; the rate matches at the sampled call and the heading offset was banked earlier, so the carrier is upstream and U-9191 is a readout rather than a defect |

Between 5 % and 20 % on G-RATE the verdict is **INCONCLUSIVE**, reported with both medians and n, and
**no** attribution is made. `n < 30` on the matched population makes the whole leg **NO-VERDICT**, the
same floor legs 1–2b used.

**Registered prediction: ACCUMULATED.** Reasoning, so it can be held against me: legs 1–2b showed
car 1's heading residual is **insensitive** to matched speed across a 1–10 % ladder (0.790 / 0.8918 /
0.9764) and barely moved when the command was matched (0.9796 → 0.9417). A term being generated at
the sampled instant should track the instantaneous inputs; one that is inherited should not. **I was
wrong about H-STALE yesterday on this same row**, so this is offered with low confidence, and
G-DIRECT is deliberately written so it can kill the parent row rather than confirm it.

## 6. No-behaviour-change requirement

The seven columns are added inside a dump that is already default-OFF (`MASHED_AI_STEPDUMP` unset →
the function returns on its second line). **Required control, not optional:** the (b)-window AI step
dump from the new build must be **byte-identical on every pre-existing column** to the committed
`verify/d3_yaww_20261005/y1.csv` over the common prefix. Any difference means adding read-only
`RecordF32` calls changed behaviour, and the leg is void until that is explained. This is the same
control that validated `MASHED_AI_YAWW` at 6480/6480 rows.

## 7. What a result here does and does not license

- **No C-level moves.** Leg 3 adds dump columns and an analysis pass; it reimplements nothing at an
  RVA, so no `diff-original` leg is available or owed.
- **It does not close criterion (b).** The 2026-10-02 counterfactual matrix had **no arm passing (b)
  on any car**.
- **GENERATED would name a target, not a fix.** It would point at the angular-velocity producers and
  `BodyOrientationIntegrate`; porting or correcting anything there is a separate, separately
  pre-registered leg with its own promotion evidence.
- **A D2 WATCH row becomes owed if and only if G-RATE returns GENERATED**, because the angular
  velocity is a D2 surface. If it returns ACCUMULATED or VOID, no D2 code has been implicated and no
  WATCH row is filed.
