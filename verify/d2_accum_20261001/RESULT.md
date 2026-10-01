# D2 attempt 11 — RESULT

Registered in [`PREREG.md`](PREREG.md) (commit `51531edf`), **not amended**. Branch
`race/first-frame-parity`. Tools: `re/tools/statediff/a11_accum.py` (registered),
`re/tools/statediff/a11_resid.py` (diagnostic). Both read-only; neither executes a game.

## 1. The registered gates

| | threshold | ORIGINAL | PORT | verdict |
|---|---|---|---|---|
| **S1** KA1-a join unique | off 0 < 1e-5, all others > 1e-2 | n/a | **5.339e-07** at off 0; next best **9.18e-02** (off ±1) | **PASS** |
| **S1** KA1-b `cMag/fMag/ld0/m78` | median ≤ 1e-4, p95 ≤ 1e-3 | n/a | median **4.1e-07 .. 8.5e-07**, p95 ≤ 2.3e-06 | **PASS** |
| **S1** KA1-c `frac` | median abs ≤ 1e-4 | n/a | **2.564e-07** (max 6.1e-07) | **PASS** |
| **S1** KA1-d `accum` | median rel ≤ 1e-3, cosine ≥ 0.9999 | n/a | **1.375e-08**, cosine **1.000000000** (min 1.000000000) | **PASS** |
| **S2** KA2-control (true accum) | median ≤ 1e-3 | n/a | **1.278e-04** | **PASS** |
| **S3** KA2-est | median ≤ 1e-3 | **1.281e-02** | 1.278e-04 | **FAIL (orig)** |
| **S4** coverage | ≥200/side, ≥30/band both sides | 1446 steps | 1628 steps | PASS overall; **no band has n≥30 on both sides** |
| **S5** `pred` vs measured `T_rest` | ratio ∈ [0.75,1.33], every band n≥30 | **0.066 .. 0.220** | **−7.04 .. 4.29** | **FAIL (both)** |

n = 1628 port steps (median speed 47.8 overall), 1446 original steps.

> **S3 and S5 failed. The registered consequence is a STOP: no input is named, no fix is
> authored, the threshold is not amended.** Decision rule R1–R6 never executed; the outcome is
> R6 in substance — *the lane closes without a named input* — but it closes with a measurement,
> not a null.

## 2. What the registered lane DID establish: `accum` is not the diverging input

S1 passed by three orders of margin: the estimator reproduces `friction_diag.log`'s verbatim
`accum` from the **render-tick snapshot alone**, on 1628 of 1628 frames, with cosine
`1.000000000`. So `accum` — §23.4's one unmeasured term — is now measured on **both** sides.

| band | ORIG `T_accum` | PORT `T_accum` | ratio | n(o) | n(p) | med speed o/p |
|---|---:|---:|---:|---:|---:|---|
| 100-150 | −1.0026 | −0.7718 | 0.77 | 45 | 19 | 126.2 / 115.3 |
| 150-260 | −1.1040 | −0.8803 | 0.80 | 73 | 13 | 189.5 / 194.9 |
| 1000-1500 | −1.1562 | −0.6194 | 0.54 | 200 | 19 | 1280.9 / 1275.5 |
| 1500-2000 | −1.3468 | −1.3396 | **0.99** | 339 | 20 | 1778.7 / 1676.5 |

`T_accum = linTerm*(accum·u)` is **−0.59 .. −1.35 on the original in every band**, where
`T_rest` is **−4.8 .. −20.4**. The accumulator cannot carry `T_rest`: it is the wrong size by
4x–15x, and it **agrees cross-side** where the original is best populated (0.99 at 1500-2000,
n=339/20).

## 3. What the double failure IS — `T_rest` is not an A6a quantity

With `accum` known, §23's `T_rest` splits **exactly** (identity residual **1.776e-14**, both
sides, every step):

```
T_rest == T_accum + curv + resid
  T_accum = linTerm*(accum.u)
  curv    = |v+w| - |v| - w.u,  w = linTerm*(ctrl+accum)   >= 0 by convexity
  resid   = s_mid - |v_post(f-1) + w|                       the UNEXPLAINED part
```

`curv < 0` on **0 of 3074** steps, as convexity requires — the split is sound.

| band | side | n | `T_rest` | `T_accum` | `curv` | **`resid`** | resid share |
|---|---|---:|---:|---:|---:|---:|---:|
| 70-100 | ORIG | 6 | −8.1688 | −0.5920 | +0.4992 | **−7.8859** | **0.965** |
| 100-150 | ORIG | 45 | −13.9758 | −1.0026 | +0.1603 | **−12.9566** | **0.927** |
| 150-260 | ORIG | 73 | −9.6704 | −1.1040 | +0.1102 | **−8.5290** | **0.882** |
| 260-500 | ORIG | 60 | −4.8117 | −1.0575 | +0.1172 | **−4.0489** | **0.841** |
| 500-1000 | ORIG | 159 | −4.8211 | −1.0250 | +0.1947 | **−4.3643** | **0.905** |
| 1000-1500 | ORIG | 200 | −8.5717 | −1.1562 | +0.3034 | **−7.8728** | **0.918** |
| 1500-2000 | ORIG | 339 | −20.4438 | −1.3468 | +0.4093 | **−19.4471** | **0.951** |

> **`T_rest` is 88–96% `resid` on the original in every band.**

### `resid` is OUTSIDE A6a — proven, not inferred

The port's `a6a_dump.log` carries `act.vel` = A6a's **own entry velocity** (`g_a6aFrame.vel`,
`Integrate2.cpp:235-239`, read before any write). Rebasing `resid` on it:

| band | n | `dEntry` = \|act.vel(f)\|−\|snap.vel(f−1)\| | `resid` | **`residA`** (rebased) |
|---|---:|---:|---:|---:|
| 70-100 | 38 | −0.0329 | −0.0328 | **−0.0000** |
| 150-260 | 13 | −0.1720 | −0.1706 | **−0.0000** |
| 500-1000 | 15 | −2.2260 | −2.2256 | **−0.0001** |
| 1000-1500 | 19 | −6.6972 | −6.6963 | **−0.0001** |
| 1500-2000 | 20 | −11.6080 | −11.6064 | **−0.0001** |

> **`residA` is −0.0000 / −0.0001 in every band, and `dEntry` equals `resid` to four decimals.**
> So W1 + `accum` + curvature account for the **whole** of A6a's contribution, exactly, and the
> carrier of `T_rest` is a velocity change that happens **before A6a's entry**.

### What that change is, measured (port side; the original has no such channel)

| band | n | `dvx` | `dvy` | `dvz` | `dvMag` | `dvLong` | **`dvPerp`** | scale `σ` |
|---|---:|---:|---:|---:|---:|---:|---:|---:|
| 70-100 | 38 | −0.0023 | +0.0026 | −0.0277 | 0.0329 | −0.0329 | **0.0000** | 0.999599 |
| 150-260 | 13 | +0.0153 | +0.0000 | −0.0757 | 0.1720 | −0.1720 | **0.0000** | 0.999109 |
| 500-1000 | 15 | +0.7174 | −0.0000 | +2.1072 | 2.2260 | −2.2260 | **0.0000** | 0.996975 |
| 1500-2000 | 20 | +11.5789 | −0.0000 | +0.7831 | 11.6080 | −11.6080 | **0.0000** | 0.993076 |

- **It is a pure scalar multiply of the velocity.** The per-component ratio
  `act.vel(f)[k] / snap.vel(f−1)[k]` has a spread of **3.769e-08 median, 1.011e-07 max**, over
  **1628 of 1628** frames. `dvPerp` is exactly 0.0000 — no direction change at all.
- **It is not gravity**: `dvy ≈ 0`.
- **It is not grip-clamp #6's `kVel`**: `σ / kVel(f−1)` ranges **1.057 .. 7.042** across bands,
  with no constant relationship (`arm` histogram `{0: 1540, 1: 87, -1: 1}`, `clampRan` 1627/1628).
- **On the port it is very close to quadratic in speed**: `(1−σ)·s / s²` = **4.8e-6 / 4.6e-6 /
  4.5e-6 / 4.6e-6 / 4.1e-6 / 4.1e-6 / 4.1e-6** across the seven bands, i.e. a `v²` drag with a
  coefficient flat to ±8% over a **20x** speed range.

### The cross-side divergence, in the new channel

| band | ORIG `resid` | PORT `resid` | **ORIG/PORT** | ORIG `resid`/s² | PORT `resid`/s² |
|---|---:|---:|---:|---:|---:|
| 70-100 | −7.8859 | −0.0328 | **240x** | 9.8e-04 | 4.8e-06 |
| 100-150 | −12.9566 | −0.0599 | **216x** | 8.1e-04 | 4.6e-06 |
| 150-260 | −8.5290 | −0.1706 | **50x** | 2.4e-04 | 4.5e-06 |
| 260-500 | −4.0489 | −0.5596 | 7.2x | 2.9e-05 | 4.6e-06 |
| 500-1000 | −4.3643 | −2.2256 | 2.0x | 6.5e-06 | 4.1e-06 |
| 1000-1500 | −7.8728 | −6.6963 | 1.18x | 4.8e-06 | 4.1e-06 |
| 1500-2000 | −19.4471 | −11.6064 | 1.68x | 6.1e-06 | 4.1e-06 |

> **At high speed the two sides' pre-A6a loss agrees (1.18x–1.68x) and is quadratic on both.
> At low and mid speed the ORIGINAL has a large, strongly NON-quadratic additional loss the port
> does not have at all — 50x to 240x.** That is exactly the shape §23.4 described for `T_rest`
> ("−42.6/frame, decaying 6x at constant speed … the port's is −0.43"), now localised to code
> **outside A6a**.

**[UNCERTAIN]** The original's `resid` is a scalar measured model-free; whether it too is a
**pure** scalar multiply cannot be decided from a `.msd` snapshot, because the original has no
`act.vel` channel. That is the one thing the next lane must instrument.

## 4. The scored arm — 3 of 3, no-change control (this lane changed no source)

Build at `37220f4d`: `[exe] all 219 objects up to date`, `[asi] all 421 objects up to date`,
`=== Build OK ===`. Dual-copy guard **`allowlisted=122 NEW=0`**.
`MASHED_MEASURE_SOLO` confirmed in effect: `mashed_re.log` reports **`participants=1`**.
Arm: `a8_run_port.py <dir> 90 MASHED_MEASURE_SOLO=1 MASHED_TRACK_SEL=12
MASHED_STEER_HOLD_AFTER=0`, three runs, own PIDs only (25152 / 37400 / 43504), all muted,
`MASHED_WIN_POS=left-bl`.

| metric | port | n | median speed | PASS interval | verdict |
|---|---:|---:|---:|---|---|
| slip 1500-2000 | **0.2033** | 20 | 1676.53 (in band) | 0.18855 .. 0.19635 | **FAIL**, +3.5% past the upper bound |
| slip 2000-2600 | **—** | 0 | — | 0.24488 .. 0.25487 | **UNSCORABLE** |
| driving-median | **1355.66** | 54 | 1355.66 | 1904.70 .. 1982.44 | **FAIL**, −30.2% below the mean |

Identical to every printed digit on **3 of 3**, which meets §3b's determinism precondition, and
equal to §21.6 / §22.2 / §22.4 / §23.3. Whole-window median horizontal speed over the first 1080
frames **26.33** (n=1080). Bounds unchanged and not renegotiated. (The median speed in the slip
band is reported here as the median of `motion_diag`'s `horiz` over the band, **1676.53**;
§23.3's `1683.53` is the same runs under `a8_slip_axis`'s own speed field. The scored metric
itself, `0.2033` at `n=20`, is identical.)

## 5. Verdict

**D2 stays REOPENED. FAIL on both scorable metrics, and no fix authored** — S3 and S5 failed, so
the registered rule did not execute.

**Routes closed by this lane, cumulative with §23.4's ten:**

11. **`accum` (`l_b8`/`l_b4`/`lin_b0`) as the diverging input** — measured on both sides for the
    first time, agrees 0.99x at 1500-2000 (n=339/20), and is 4x–15x too small to carry `T_rest`
    on either side.
12. **`frac`, `l_d0`, `m78`, `cMag`, the tangential force `Sf − Sc`** — all reconstructed exactly
    (S1) and all reported in `diag_compare.txt`; none of them can matter, because the term they
    feed is itself too small by construction.
13. **The whole of A6a as the home of `T_rest`** — `residA → 0.0000` on the port proves W1 +
    `accum` + curvature exhaust A6a's contribution.

**NEXT LANE, and it is upstream of everything §21–§23 examined**, with sub-lane C having already
narrowed it. Three jobs, in this order:

1. **Log the port's `G`** — record bytes `0x150 / 0x154 / 0x158` — plus A5's actual `fVar4` and
   `local_70`, as one default-OFF diagnostic line. That makes `l70G`'s 0.85 ratio at 1000-1500
   splittable into `G` and `local_70`, and it is a known-answer check on T1's back-out at the
   same time. Cheap: one source change, one port run, no original run.
2. **Instrument the ORIGINAL's A6a entry velocity** so its σ stops being a back-out: an **entry
   hook** on `0x00467650` snapshotting `+0x9b0..0x9b8` before any write (the technique §21.9 used
   for `l_60` via `RwV3dLength`'s pointer argument — entry-only, one function, short run; memory
   `frida-interceptor-is-entry-only`). This also decides whether the original's pre-A6a change is
   a **pure** scalar multiply as the port's is on 1628/1628, which the `.msd` cannot answer.
3. **Find the original's further sub-500 sink by RVA.** It is **not** A6a (`residA → 0.0000`),
   **not** `0x0046ef70`'s impulse (the original's `resid < 0` on **1446 of 1446** steps against
   only 23 fixup frames), and **not** C1 alone (T2/T3). Resolve the `[UNCERTAIN]` above first —
   get original samples below 500 that are not post-bounce, or cut on slip at matched speed —
   because if the sink is slip-coupled it is the same feedback loop §21.10 measured out, and if it
   is speed-coupled it is a new, independent term.

**Still open and untouched here:** U-9160 (substep budget 3–4 against the original's fixed 2 at
`0x00469ad4`); the port's 40-70 residency and §20.14's `−0.1` reverse-gate duty cycle; D1-residue
R1. **U-9156 stays OPEN** — it is the trap, not the framing.

## 5b. SUB-LANE C — the C1 hypothesis, pre-registered separately in [`PREREG_C.md`](PREREG_C.md)

The pre-A6a scalar has exactly one ungated in-window candidate: **A5
`VehicleWheelForceIntegrate`, RVA `0x0046ddb0`, Phase 4** (`ForceIntegrator.cpp:86-90` and
`:164-168`, read verbatim this session; A5 runs at `VehicleControl.cpp:207`, A6a at `:215`). Its
gravity add is exactly zero, so the change stays collinear — which the measured
`dvPerp = 0.0000` requires. The other in-window candidate, `Integrate2.cpp:215-216`, is gated on
`Ri(v,0x1f0)` and its scalar is speed-**independent**, so it cannot make a `v²` law.

Back-out: `l70G = (1−σ)/(linTerm · s_mid_prev)`, `G = f(0x150)·f(0x154)·f(0x158)`,
`local_70 = l70G/G`. Falsifiable because `local_70`'s construction (`:93-161`) has **no speed
term at all** and cannot leave `[0,2]`.

| gate | bar | result | verdict |
|---|---|---|---|
| **KA4** σ(resid) vs σ(act.vel), port | median ≤ 1e-5, p95 ≤ 1e-4 | **7.125e-06**, p95 **8.841e-05** (n=1628) | **PASS** |
| **T1** port `l70G` constant across bands | max/min ≤ 2.0 | **1.053** (0.2342 .. 0.2467, 8 bands) | **PASS** |
| **T2** original `local_70` ∈ [0, 2.0] | every band n≥10 | **1.000 .. 182.5** | **FAIL** |
| **T3** original `local_70` constant | max/min ≤ 2.0 | **182.4** | **FAIL** |

| band | side | n | σ | `s_mid_prev` | `resid` | **`l70G`** | `G` | `local_70` | med speed |
|---|---|---:|---:|---:|---:|---:|---:|---:|---:|
| 100-150 | ORIG | 45 | 0.892122 | 126.24 | −12.9566 | **52.96** | 0.29025 | **182.5** | 126.2 |
| 100-150 | PORT | 19 | 0.999475 | 129.42 | −0.0599 | **0.2462** | — | — | 115.3 |
| 150-260 | ORIG | 73 | 0.950692 | 193.37 | −8.5290 | **15.74** | 0.29025 | **54.23** | 189.5 |
| 260-500 | ORIG | 60 | 0.988507 | 379.53 | −4.0489 | **1.728** | 0.29025 | **5.952** | 371.1 |
| 500-1000 | ORIG | 159 | 0.995100 | 824.27 | −4.3643 | **0.4529** | 0.29025 | **1.560** | 821.6 |
| 500-1000 | PORT | 15 | 0.996975 | 735.78 | −2.2256 | **0.2467** | — | — | 735.8 |
| **1000-1500** | **ORIG** | **200** | 0.993795 | 1283.20 | −7.8728 | **0.2904** | 0.29025 | **1.000** | 1280.9 |
| **1000-1500** | **PORT** | **19** | 0.994750 | 1276.96 | −6.6963 | **0.2467** | — | — | 1275.5 |
| 1500-2000 | ORIG | 339 | 0.989006 | 1778.81 | −19.4471 | **0.374** | 0.29025 | **1.288** | 1778.7 |
| 1500-2000 | PORT | 20 | 0.993077 | 1683.83 | −11.6064 | **0.2467** | — | — | 1676.5 |

> **T1 confirms C1 is the port's entire pre-A6a scalar.** `l70G` is constant to **5.3%** across a
> **20x** speed range — exactly what C1 predicts and what no other mechanism would give.
>
> **T2/T3 refute C1 as the whole of the ORIGINAL's**, and localise the refutation precisely: the
> original's `G` is **exactly constant** (`0.15 × 1.5 × 1.29 = 0.29025`, one distinct value on
> all 1446 steps, as are its three fields), so every band's `local_70` is directly comparable —
> and at **1000-1500 it is `1.000`**, the legal no-reduction value, rising only to `1.288` /
> `1.560` at the neighbouring bands, then exploding to `5.95 / 54.2 / 182.5` below 500.

**Registered consequence (D4): T2 and T3 failed, so D1/D2 did not execute. No term is named and
no fix is authored** — and D3 refused a fix on this sub-lane in advance in any case, because the
port's `G` is in no existing log, so the second side is unchecked
(`cross-side-fit-needs-both-sides-checked`).

**What it nonetheless establishes, by measurement:**

1. **C1 (`0x0046ddb0` Phase 4) is real, in-window, and is the port's whole pre-A6a velocity
   sink.** T1 at 1.053 over 8 bands.
2. **Above ~1000 speed the original behaves as C1 too, with a legal `local_70` of 1.000–1.288.**
   The cross-side `l70G` ratio there is **0.85** (1000-1500, n=200/19, matched speed 1280.9 vs
   1275.5) and **0.66** (1500-2000, n=339/20) — both inside [0.5, 2.0], i.e. **not** a divergence
   by D1's own threshold.
3. **Below ~500 speed the original has a further velocity sink that C1 cannot produce** (it would
   need `local_70` up to 182.5 where the code's range is `[0,2]`) **and that the port does not
   have at all.**

**[UNCERTAIN]** Whether that further sink is speed-coupled or slip-coupled **cannot be decided
from these bands**: the original's samples below 500 are its single post-bounce pass (§21.5 — it
goes below 100 horizontal once per race, for 6–8 frames of 6658), so "low speed" and "heavily
sideways" are the same frames in this capture. Deciding it needs original-side samples at low
speed that are **not** post-bounce, or a slip-resolved cut at matched speed.

## 6. The instrument lesson this attempt paid for

**A global median can pass a gate that every band fails.** KA2 passed on the port at `1.278e-04`
while S5 failed there by factors of 4 to 7 in individual bands — because the port spends most of
its frames in the low-speed trap (whole-window median horizontal speed **26.33**), so a
whole-run median is a statement about the trap and says nothing about the 20 frames at 1500-2000.
**Band-resolve every gate whose subject is speed-dependent**, and keep an unbanded gate only as a
smoke test. S5 existed precisely because of this, and it is what caught the failure.

Second: **a verified-exact estimator is worth more as a subtraction than as a measurement.**
S1 did not name anything by itself. Its value was turning `T_rest` from one lumped term into
`T_accum + curv + resid` with an exact identity, which is what localised the defect to code
nobody had looked at.

## 7. Artefacts

`s1_selftest.txt` (S1), `s2s3_port.txt` (S2/S3-est), `s3_orig.txt` (S3-orig),
`diag_compare.txt` (S5 + the banded term table), `diag_resid.txt` (the exact split + the axis
decomposition), `diag_scale.txt` (the pure-scalar test and the `kVel` exclusion),
`scored.txt`, `sc1/` `sc2/` `sc3/` (the three scored runs).
Inputs, unchanged and not re-run: `verify/d2_gain_20261001/p1/` (port),
`verify/d2_bounce_20260930/orig_fp2.msd` (original). `original/` was not modified.
