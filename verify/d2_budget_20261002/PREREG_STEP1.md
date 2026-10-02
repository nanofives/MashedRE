# PRE-REGISTER — D2 attempt 18, STEP 1: the per-frame velocity budget at matched `d`

Written and committed **BEFORE** any reduction is run. Nothing in this file has been
executed. U-9177 names the question: over `d = 222..250` the drive force agrees to 6.2 %,
all four wheels are grounded on both sides, the cars are aligned and in the same gear, and
the median per-frame speed gain is ORIG **+27.87** against PORT **+4.86** (n = 28, median
speed O 805.7 / P 660.9). This step decomposes that gain into the writes that produce it,
in call order, by RVA, and names the first term that leaves tolerance.

`d` is frames from release, `L = 0` alignment, **`R = 890`** on `orig_sl1.msd` (the capture's
own `+0xbf8 != 0` marker, `0x0046d7a2`) and **`R = 1`** on the port's `motion_diag.log`
(`verify/d2_b0c_20261002/RESULT_STEP2.md` §2). Every number reported in the RESULT carries
`n`, median speed and `d`.

---

## 1 The arms — already captured, already committed, NOT re-run

| arm | file | release | provenance |
|---|---|---|---|
| ORIGINAL | `verify/d2_b0c_20261002/orig_sl1.msd` (2334 frames) | `R = 890` | `orig_sl1.msd.provenance.json`, git `d9db5e80` |
| PORT | `verify/d2_b0c_20261002/p1/motion_diag.log` (1627 lines) | `R = 1` | `p1/PROVENANCE.txt`, §16.7 arm, `MASHED_MEASURE_SOLO=1 MASHED_TRACK_SEL=12 MASHED_STEER_HOLD_AFTER=0` |

STEP 1 spends **no game run**. It is a reduction of two committed captures. `p2`/`p3` are
identical to `p1` to every digit (attempt 17 §7d), so one port arm is the whole port arm.

---

## 2 The terms, in CALL ORDER, by RVA

Per-frame physics order, from `FUN_00470c70`'s decode (`VehiclePhysicsRun.cpp:1003-1008`,
D2 §21.3) — identical on both sides, A4 before the substep loop
(`MASHED_A8_A4_FIRST` default-ON, `VehiclePhysicsRun.cpp:763-771`):

```
A4   0x00470670                      head: +0xb0c, zero b14/b18/b1c at 0x0047072c
A5   0x0046ddb0                      wheel axes + Phase-4 drag        ForceIntegrator.cpp:86-90,:164-168
A6a  0x00467650
       accum  0x0046833a..0x00468625   per-wheel friction + blend     Integrate2.cpp:489-565
       W1     0x0046862d..0x004686a2   v += linTerm*(ctrl + accum)    Integrate2.cpp:630-632
       store  0x004686cc               |v| -> +0x9e4                  Integrate2.cpp:636
       clamp6 0x004687f0..0x0046897b   W2 HIGH / W3 LOW lateral damp  Integrate2.cpp:727,:736,:742
A6b  0x00468980                      writes NO velocity (§26.4, static)
sub  0x004709a0  x N                 0x0046e9e0 -> 0x0046f6c0 -> 0x00469aa0 -> 0x0046ef70
```

The two snapshot-observable boundaries are the `+0x9e4` store at `0x004686cc` and the
render-tick snapshot itself. That gives exactly three budget terms:

| term | spans, by RVA | formula |
|---|---|---|
| **T_drive** | A6a W1's drive share, `0x0046862d..0x004686a2` | `linTerm * (ctrl(f) . u(f))`, `u = v(f-1)/s_post(f-1)`, `ctrl = (+0xb14,+0xb18,+0xb1c)` |
| **T_rest** | A4 `0x00470670` + A5 `0x0046ddb0` + A6a accum `0x0046833a..0x00468625` + W1's non-drive share + frame `f-1`'s substeps | `T_W1 - T_drive`, `T_W1 = s_mid(f) - s_post(f-1)` |
| **T_post** | A6a clamp #6 `0x004687f0..0x0046897b` + A6b `0x00468980` + frame `f`'s substeps `0x004709a0` | `s_post(f) - s_mid(f)` |

`s_mid(f) = +0x9e4(f)`; `s_post(f) = |+0x9b0..b8|(f)`.
`linTerm = dt * Rf(+0x54) * kDt`, `dt = 50.000004`, `kDt = 1/3000` exactly
(`_DAT_005cc948 = 0x39aec33e`), `Rf(+0x54)` read from each side's own record.

**Identity:** `T_drive + T_rest + T_post == s_post(f) - s_post(f-1)` holds **by
construction** (`T_rest` is defined as the residue of `T_W1`). The RESULT reports
`max|residual|` as a float-arithmetic self-consistency number and **labels it as a
construction identity, not as evidence**. This is stated here so it cannot later be
presented as a measurement.

`T_post` is named `T_post`, not `T_clamp`, deliberately: at the snapshot phase it lumps
clamp #6 with the substeps. §4 registers how that lump is split.

---

## 3 Precision, stated up front

The port's `motion_diag.log` prints `sp=%.2f` (= `+0x9e4`) and `vel=[%g,%g,%g]`
(`VehiclePhysicsRun.cpp:1210-1234`). At speed 660 that is an absolute resolution of
**0.01** on `s_mid` and **~0.001** on `s_post`. The terms under measurement are of order
**20-30 per frame**, so the quantization is **<= 0.05 %** of each term — immaterial for the
decision rule.

It is **NOT** immaterial for the clamp-no-op question: 0.01/660 = **1.5e-5**, while the
original's measured clamp cost is **2e-6** (§25.3). So the RESULT may state that the port's
`T_post` is large (its own 14.7 % median, §23.2) but **may not** claim the port's `T_post`
is "within noise of zero" anywhere, and may not compare the two sides' no-op-ness below
1.5e-5. Registered as a limit before the run.

---

## 4 The windows

| window | why |
|---|---|
| `d` 200..222 | the CONTROL: the port gains MORE here (+19.06 vs +17.59). A term that diverges at 222 must be inside tolerance here. |
| **`d` 222..250** | the DEFECT window (U-9177, n = 28) |
| `d` 250..260 | continuation, reported for shape only |

Matched `d`, never banded on speed (§26.10, memory `band-on-speed-compares-different-moments`).
The two arms' median speed in the defect window differs by 18 % (805.7 vs 660.9); a
`v^2` drag would therefore give the ORIGINAL **more** sink, i.e. less gain — the opposite
of what is observed — so the speed mismatch cannot manufacture the finding. A
matched-speed sub-cut is reported as a **sensitivity row, not a gate**, and only if it
reaches `n >= 10` on both sides.

---

## 5 Gates. A failing gate STOPS the step. No gate is amended or replaced in this session
## without the RESULT saying so explicitly.

### Gate EV — the capture carries the event itself
Over `d = 222..250`, on BOTH arms: `n >= 25`, `+0x9e0 == 4.0` on **every** frame, and
`|ctrl_xz| > 0` on **every** frame. Reported as counts, not asserted.
**FAIL => STOP**: the window is not the window U-9177 describes.

### Gate KA-R — KNOWN ANSWER, scored ON THE ORIGINAL
Published prior: the original's clamp #6 is a measured no-op —
`median(+0x9e4 / |+0x9b0..b8|) = 0.999998`, p25 `0.999955`, p75 `1.000044`, with exactly
23 frames above 1.02 (§23.2, n = 1447); and `0.999991..1.000020` in every band at substep
entry (§25.3, coverage 2331/2331).

Scored here, on `orig_sl1.msd` over `d = 0..400`: the ratio `s_mid/s_post` must lie in
**[0.9990, 1.0010]** on **>= 99 %** of frames with `s_post > 1.0`, and its median must lie
in **[0.9999, 1.0001]**.

**FAIL => STOP.** It would mean the original's `T_post` is not a no-op and §23/§25's
measurement of it does not reproduce, which invalidates reading `T_post` as "clamp #6 plus
an inert tail" on the reference side. This is the gate that exists because an estimator can
be exact on the port and wrong on the original (memory
`estimator-exact-on-one-side-can-be-wrong-on-the-other`).

### Gate KA-M — KNOWN ANSWER on `linTerm`'s input, scored ON THE ORIGINAL
`Rf(+0x54)` must take **exactly one distinct value** across all of `orig_sl1.msd`, and that
value must be `0.0010000000474974513` (the measured constant a10_gain.py carries, §24.1).
**FAIL => STOP**: `linTerm` is then not a constant on the reference side and `T_drive` is
not computable from the snapshot there, which is precisely the failure mode
`estimator-exact-on-one-side-can-be-wrong-on-the-original` names.

### Gate CO — coverage, counted not assumed
For each window and each arm the RESULT reports: frames in range, frames dropped because
`f-1` is missing, frames dropped because `s_post(f-1) <= 1e-6`, and frames dropped for a
non-finite field. **A window whose dropped count exceeds 10 % of its range is reported
UNREADABLE and not scored.**

### Gate SS — substep accounting (U-9160), reported not inferred
The RESULT reports, for the port arm over `d = 222..250`, how many frames carry a non-zero
`T_post` and the median `T_post` on those frames, and states explicitly that the offline
instrument **cannot** separate clamp #6 from the substeps. If `T_post` is the term named by
§6's rule, §7's live leg is REQUIRED before any fix, and this is registered as a blocking
condition now, not decided after seeing the number.

---

## 6 The decision rule — registered, in call order

Thresholds are attempt 10's, reused verbatim rather than re-chosen
(`verify/d2_gain_20261001/PREREG.md`; `a10_gain.py:243-259`). For each term `T`, with
medians `mo`/`mp` over the window and `W1o = median(T_W1)` on the ORIGINAL,
`resid = |median(dS)_O - median(dS)_P|`:

```
(a)  |mp - mo|  >=  0.20 * |W1o|
(b)  |mp - mo|  >=  0.30 * resid
```

A term is **OUT OF TOLERANCE** iff (a) AND (b).

**The rule:** walk the terms in call order `T_drive -> T_rest -> T_post`. The **diverging
term** is the FIRST term that is OUT OF TOLERANCE in the defect window `d = 222..250`
**while every term before it is IN tolerance there**. If an earlier term is also out of
tolerance, the EARLIER one is named and the later one is reported as downstream of it.

If no term is out of tolerance: report **NO TERM NAMED**, and the conclusion is that the
gain deficit is not carried by a single snapshot-observable stage — in which case §7's live
leg is the only route and STEP 2's fix is not authored this attempt.

Control condition, reported alongside: the named term must be **IN** tolerance over
`d = 200..222`. If it is out of tolerance there too, the RESULT says so and the term is
reported as a standing divergence rather than as the switch-on at `d = 222`.

---

## 7 If the named term is `T_rest` or `T_post` — the live leg, and what it would use

Both are lumps. Splitting them needs the intra-frame boundaries, which the snapshot does not
carry. The route, registered now so it is not designed after seeing the answer:

- **ORIGINAL**: `scenario_launch.py --lat-bracket` — the EXISTING probe
  (`scenario_launch.py:875-920`), no new probe code. Entry hooks only, three sites: A6a
  entry `0x00467650` (ESI-filtered), A6b entry `0x00468980`, substep entry `0x004709a0`.
  It already samples `+0x9b0/b4/b8`, `+0x9d4/d8/dc`, `+0x9e4`, `+0x9e0`, `+0x9bc/c0/c4`.
  That splits `T_post` into [clamp #6] / [A6b] / [substep 1] / [substep 2 + next A4+A5],
  and the site-1 count per frame is the ORIGINAL's substep count (expected 2, §14.6).
- Its own known answer, to be scored on the ORIGINAL: `+0x9e4` read at A6b entry must equal
  the `.msd`'s `s_mid` for the matched frame, and `|v|` at A6b entry must equal the
  `.msd`'s `s_post`, each to 4 ulps on >= 99 % of matched frames. That is what would license
  reading the offline split as `[W1 | clamp6 + tail]`.
- **PORT**: a port-side split needs one default-OFF diagnostic at the matching phase. It is
  a source edit, so it gets its OWN pre-registration with a channel control (armed /
  unarmed / count), as attempt 12's `MASHED_A5GDIAG` did. Not written here.

---

## 8 What STEP 1 will NOT do

- No knob, no clamp, no fitted constant.
- No `mashedmod/src` change. STEP 1 is a reduction only; the dual-copy guard is therefore
  `NEW = 0` by construction for this step.
- No tracker mutation outside `re-classify`.
- AI slots 1+ (`VehiclePhysicsRun.cpp:702`) are not touched; they are D3 work.

## 9 The instrument

New reducer `re/tools/statediff/a18_budget.py`. It imports `a8_momentum.load_msd`/`f32`
for the original and parses `motion_diag.log` with the same matching-line ordinal
`a17_slide.py:90-118` uses (`"sp=" in line`, 0-based over matching lines only), so `d`
means the same frame in both tools. It re-implements nothing of a10_gain's arithmetic by
hand: `linTerm`, `KDT`, `M54` and the `(a)`/`(b)` thresholds are imported from
`a10_gain.py` so the reused rule is literally the reused code.
