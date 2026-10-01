# D2 attempt 11 — PRE-REGISTRATION C: test the C1 hypothesis for the pre-A6a scalar

Written and committed **BEFORE any run of `a11_accum.py`'s sibling `a11_drag.py`**. A **new
question**, not an amendment of [`PREREG.md`](PREREG.md): that file's S3/S5 STOP stands, no input
to grip-clamp #6 was named, and no fix was authored on it. **This file is never amended either.**

## 1. What is already measured (committed `37220f4d`, do not re-derive)

Between the frame-(f−1) render-tick snapshot and A6a's entry at frame f, the port multiplies
`+0x9b0..0x9b8` by a **single scalar**: per-component ratio spread **3.769e-08 median /
1.011e-07 max on 1628 of 1628** frames, `dvPerp` exactly **0.0000**, `dvy ≈ 0`. The resulting
speed loss is **`(1−σ)·s / s² = 4.1e-6 .. 4.8e-6`** across seven bands spanning **20x** in speed,
i.e. a `v²` law flat to ±8%. It is **not** grip-clamp #6's `kVel` (`σ/kVel` = 1.057 .. 7.042).
It carries **88–96% of §23's `T_rest`** on the original.

## 2. The hypothesis, by RVA

**C1** — A5 `VehicleWheelForceIntegrate`, RVA **`0x0046ddb0`**, Phase 4. Port:
`mashedmod/src/mashed_re/Vehicle/ForceIntegrator.cpp:86-90` and **`:164-168`**, read verbatim
this session:

```
:86   fVar4 = vF(self,0x279)                      # byte 0x9e4 = s_mid(f-1)
:87   if (< kSpeedMin) fVar4 = 0
:88   fVar4 = vF(0x54)*vF(0x55)*vF(0x56) * fVar4   # bytes 0x150/0x154/0x158  == G
:89   if (self[0x278] != 0x40800000) fVar4 *= 0.5  # byte 0x9e0 grounded gate
:90   fVar4 = fVar4 * vF(self,0x15) * dt * kDt     # byte 0x54; dt*kDt == linTerm/m54
:164  fVar4 = 1.0 - local_70 * fVar4
:165  if (fVar4 < 0 || 1 < fVar4) fVar4 = 0
:166-168  vel *= fVar4                             # PURE SCALAR on all three
```

`vF(v,i)` indexes an `int*`, so byte = `4i` (`ForceIntegrator.h:17`) — hence the byte offsets
above. The gravity add at `:169-174` is **exactly zero** (`g_gravScale` / `g_gravX..Z` = 0,
`ForceIntegratorStubs.cpp:24-25`), so the change stays collinear, which is what the measured
`dvPerp = 0.0000` requires. Call order: A5 at `VehicleControl.cpp:207`, A6a at `:215`, so C1 is
**in-window**, and it is **ungated** (no env var, no default-OFF flag).

It is the **only** ungated in-window single-scalar multiply of the triple. The one other
in-window candidate, `Integrate2.cpp:215-216`, is gated on `Ri(v,0x1f0) ∈ {−1, −0x373738}` and
its scalar is **speed-independent**, so it cannot produce a `v²` law.

## 3. The back-out, and why it is falsifiable

`local_70` (Phases 2–3, `:93-161`) is an A5 **local**, not a record field. With `σ` measured and
every other factor a record field:

```
linTerm   = m54 * dt * kDt          (byte 0x54; == C1's :90 product, and == W1's linTerm)
l70G      = (1 - sigma) / (linTerm * s_mid_prev)        computable on BOTH sides
G         = f(0x150) * f(0x154) * f(0x158)              record fields, .msd only
local_70  = l70G / G
```

`l70G` needs only `σ`, `linTerm` and `+0x9e4`, all available on both sides.
`G` needs the raw record, so it is available on the **original** (`.msd`) and **not** on the port
(no existing log carries bytes `0x150/0x154/0x158`; `motion_diag`'s `gt=[..]` is the per-gear
**drive** table at `+0x478..+0x48c`, a different thing). Registered as a limitation up front.

**THE FALSIFICATION.** `local_70`'s construction has **no speed term anywhere**: drafting
proximity (`:94-120`), player count (`:123-125`), the race-timer ramp (`:127-134`), contact
counts (`:136-152`), `RubberBandGrip`/`RubberBandGate` (`:154-160`). On a **solo** run with steer
held, none of those varies with speed. Its value is a product of factors each ≤ 1 except
`(fVar5*local_6c + 1)` with `fVar5 ≤ 1`, so it cannot leave `[0, 2]`. Therefore:

- **T1.** On the **PORT**, `l70G` must be **near-constant across speed bands**: `max/min ≤ 2.0`
  over bands with `n ≥ 10`. **Fail ⇒ C1 does not alone account for the port's own σ; STOP.**
- **T2.** On the **ORIGINAL**, backed-out `local_70` must lie in **[0, 2.0]** in every band with
  `n ≥ 10`. **Fail ⇒ C1 cannot produce the original's `resid` with a legal `local_70`, so there
  is a FURTHER pre-A6a writer on the original. Report that as the finding; name no fix.**
- **T3.** On the **ORIGINAL**, `local_70` must satisfy `max/min ≤ 2.0` across those bands.
  **Fail ⇒ same consequence as T2.**
- **KA4.** `σ` derived from the model-free `resid` must agree with `σ` measured directly from
  the port's `act.vel`: median absolute difference **≤ 1e-5**, p95 **≤ 1e-4**.
  **Fail ⇒ the back-out's input is unsound; STOP, report nothing from it.**
- **T4.** Coverage: a band is quoted only with `n ≥ 10` on the side quoted.

Scope filter for every step: `+0x9e0 == 0x40800000` at **both** f−1 and f (so C1's `:89`
half-factor does not apply), and `s_prev > 1e-3`.

## 4. The DECISION RULE

Evaluated on the bands with `n ≥ 10` on both sides, `l70G` being the only cross-side-computable
quantity:

- **D1.** If `l70G` differs cross-side by a factor outside **[0.5, 2.0]** in the best-populated
  such band → **name `l70G`'s divergence**, then split it:
  - if the original's `G` varies across bands by more than **2.0x** while its `local_70` stays
    inside `[0,2]` and `max/min ≤ 2.0` → **name `G`**, the field product at record bytes
    `0x150 / 0x154 / 0x158`, as the diverging input;
  - else → **name `local_70`**, i.e. A5 Phases 2–3 (`ForceIntegrator.cpp:93-161`).
- **D2.** If `l70G` agrees inside [0.5, 2.0] in every band with `n ≥ 10` on both sides →
  **C1 is not the divergence**; report it as a matched term and close the sub-lane.
- **D3. REFUSAL, registered up front.** Whatever D1 names, **no fix is authored in this session
  on this sub-lane.** Reason: the port's `G` is not measurable from any existing log, so a
  cross-side `G` comparison does not exist yet, and §23's own instrument lesson plus memory
  `cross-side-fit-needs-both-sides-checked` both require the second side to be checked before a
  term is acted on. The deliverable here is a **named, RVA-anchored candidate with its
  falsification tests run**, and the next session's first job: log the port's
  `f(0x150)/f(0x154)/f(0x158)` and A5's actual `fVar4` / `local_70` (default-OFF diag), plus an
  **entry hook** on `0x00467650` reading `+0x9b0..b8` on the ORIGINAL so its `σ` stops being a
  back-out and becomes a direct measurement.
- **D4.** If `T1`, `T2` or `T3` fails, D1/D2 do **not** execute. Report the failure as written.

## 5. Unchanged

The §3 bounds (`slip 1500-2000` 0.18855..0.19635, `slip 2000-2600` 0.24488..0.25487,
`driving-median` 1904.70..1982.44), the §16.7 arm, and §23.4's fix invariant
(`median(+0x9e4/|+0x9b0..b8|) == 1.000` to 1e-3). No source change on either side for this
sub-lane; no game is executed by it. Data: `verify/d2_gain_20261001/p1/` and
`verify/d2_bounce_20260930/orig_fp2.msd`, both unchanged.
