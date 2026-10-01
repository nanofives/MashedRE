# D2 attempt 10 — RESULT: the between-contact budget. Two safety thresholds FAIL, the
# registered rule does not execute, and NO fix is authored.

Pre-registration: [`PREREG.md`](PREREG.md), commit `bccaf40b`, written and committed before any
run. **Not amended.** Artefacts in this directory. Build at HEAD, `rva-lint allowlisted=122
NEW=0`, `mashedmod\build.bat` → `=== Build OK ===` with all 219 exe / 421 asi objects already
up to date (no source change was made in this lane).

---

## 1. Verdict first

* **The registered decision rule did NOT execute.** Two of its six safety thresholds failed on
  the port (**S1** 91.3% against the registered >= 99%, **S6** 1324 detected contact frames
  against 212 actual fixups), so the gap-aligned cross-side term table is **void** and its
  apparent verdict is **not reported as a result**.
* **Both failures have one cause, and that cause is itself the sharpest measurement in this
  lane**: the quantity `s_post - s_mid`, which on the ORIGINAL is zero on 99% of frames and
  large only on its 23 fixup frames, is large on the PORT on most frames. The registered
  detector was built on the original's behaviour and the port's grip-clamp #6 swamps it.
* **The diverging budget term is `T_clamp`** — the W2/W3 row of the pre-registered table,
  grip-clamp #6 at `0x004687f0..0x0046897b` (`Integrate2.cpp:727`/`:736`). That is **branch 5**
  of the registered decision rule: §21.5 proved that code byte-faithful, so this is the **same
  feedback loop** §21.2 / §21.5 / §21.10 already closed, measured in its most direct channel
  yet — **not a new independent term**. Registered consequence: **author no fix.** Done.
* **`T_drive` is RULED OUT** at matched speed, in every band with n > 5 on both sides. This is
  the matched-band drive-force measurement the kickoff recorded as **not** covered by §20.10's
  withdrawn reading, and it comes back **agreeing**.
* **D2 does not close.** Scored 3 of 3 as a no-change control (no source change was made):
  slip 1500-2000 **0.2033** (n=20, median speed 1683.53, in band) FAIL +3.4%; slip 2000-2600
  **UNSCORABLE** (n=0); driving-median **1355.66** (n=54, median speed 1355.66) FAIL −30.2%.

## 2. The six safety thresholds, as run

| | threshold | ORIGINAL | PORT | verdict |
|---|---|---|---|---|
| **S1** | `T_clamp <= +1e-3*s_from` on >= 99% of steps | **1446 / 1446 = 100.0%** | **1486 / 1627 = 91.3%** | **FAIL (port)** |
| **S2** | snapshot-phase `T_drive` == A6a-phase `linTerm*(ctrl.u)` within 10% on >= 90%, join unique | n/a | **1627 / 1627 = 100.0%**, medians equal to 4 dp; join offset `-2` matched 1557/1627 = 95.7% against < 0.3% for every other offset in `-4..+4` | **PASS** |
| **S3** | median `T_drive` rises >= 1.5x from `[150,260]` to `[1500,2000]` | +23.17587 (n=73) → +36.97565 (n=339), **ratio 1.595** | +22.34165 (n=12) → +34.69071 (n=20), **ratio 1.553** | **PASS both** |
| **S4** | `+0x9e0 == 4.0` on >= 95% of steps | 1446 / 1446 = 100.0% | 1627 / 1627 = 100.0% | **PASS** |
| **S5** | the arm took: `driving-median 1355.66`, gap-0 opening speed `251.48` | — | **1355.66** (n=54) on 3 of 3 runs; port `f=81` h = **251.4774** | **PASS** |
| **S6** | detector count == `world_contact.log` fixup count ± 1 | **23 == 23**, and all 23 are exactly `probe_frame+1` of `orig_fp2`'s site-0 fixups | **1324 against 212** | **FAIL (port)** |

**S1's 141 violations** (port): median `s_from` **33.1**, median `T_clamp` **+4.2421**, max
**+14.528**. A positive `T_clamp` means the speed *rose* between `0x004686cc` and the snapshot,
which grip-clamp #6 cannot do. 141 <= the run's 212 fixups, so this is consistent with those
being fixup frames — i.e. the same confound as S6, in the opposite direction, at the port's
low-speed residency.

**S2's by-product closes a question raised mid-lane.** `friction_diag.log`'s first line shows
`ctrl=(0, -13000103, 0)` and the original's `+0xb18` is exactly `0.0` on **2332 of 2332**
frames, which looked like a missing / invented vertical force. It is not: over the race the
port's `|ctrl.y|` has median **0.009** against `|ctrl.x|` **388849** and `|ctrl.z|` **786534**,
and the `-13000103` sample is the stationary pre-race frame (`spd = 0`). The registered
`b18 = 0` substitution is valid and the drive force has no hidden vertical component on either
side.

## 3. The measurement the threshold failures ARE — detector-free, contamination-proof

`ratio(f) = s_mid(f)/s_post(f) = +0x9e4(f) / |+0x9b0..b8|(f)`. On a non-contact frame the only
code between those two reads is grip-clamp #6 (A6a `0x00467650` and `VehicleContactFixup`
`0x0046ef70` are the only writers of `+0x9b0` per frame, §22.2/§22.4). **No detector and no gap
alignment is needed for the MEDIAN**: the fixup touches at most `N_fixup` frames, so any
quantile below `1 - N_fixup/N_frames` is immune to it, and the contamination is 1.6% on the
original and 13.0% on the port. Tool: `re/tools/statediff/a10_clampcost.py`.

| | frames (`s_post>1e-3`) | fixups | contam. | **median ratio** | p25 | p75 | frames ratio>1.02 |
|---|---:|---:|---:|---:|---:|---:|---|
| **ORIGINAL** | 1447 | 23 | 1.6% | **0.999998** | 0.999955 | 1.000044 | **23 (1.6%)** — exactly its 23 fixups |
| **PORT** | 1628 | 212 | 13.0% | **1.172734** | 1.028778 | 1.905301 | **1324 (81.3%)** |

> **The port loses a median 14.7% of its linear speed per frame between A6a's `+0x9e4` store
> (`Integrate2.cpp:636`, original `0x004686cc`) and the render-tick snapshot. The original loses
> 0.0002%.** The median frame is a non-fixup frame on both sides by the contamination bound, so
> the median is the clamp.

Median % of speed lost per frame, by snapshot speed band:

| band | ORIGINAL | PORT | n(orig) | n(port) |
|---|---:|---:|---:|---:|
| 40-70 | −0.004% | **+0.779%** (speed GAINED — see S1) | 2 | 270 |
| **70-100** | **+0.003%** | **−15.782%** | 6 | 38 |
| 100-150 | +0.001% | −1.898% | 45 | 19 |
| 150-260 | −0.002% | −0.427% | 73 | 12 |
| 260-500 | −0.002% | −0.049% | 60 | 14 |
| 500-1000 | −0.000% | −0.006% | 159 | 15 |
| 1000-1500 | −0.000% | −0.116% | 200 | 19 |
| 1500-2000 | +0.000% | −0.436% | 339 | 20 |

## 4. `T_drive` is RULED OUT — matched speed, detector-free, both sides

`T_drive(f) = linTerm * (ctrl(f) . u(f))`, `linTerm = 1.6666668613120733e-05 = 50.000004 *
0.0010000000474974513 * (1/3000)`; the port's `friction_diag.log` prints `linTerm=1.66667e-05`
verbatim, and `+0x54` is `0.0010000000474974513` on **2332 / 2332** original and **1628 / 1628**
port frames. No gap selection; all consecutive steps, binned by the step's from-speed.

| band | ORIG `T_drive` | PORT `T_drive` | **ratio** | ORIG `T_rest` | PORT `T_rest` | n(o) | n(p) |
|---|---:|---:|---:|---:|---:|---:|---:|
| 70-100 | +15.4218 | +14.8848 | **0.965** | −8.1688 | −0.1717 | 6 | 38 |
| 100-150 | +19.2834 | +17.3377 | **0.899** | −13.9758 | +0.1097 | 45 | 19 |
| 150-260 | +23.1759 | +22.3416 | **0.964** | −9.6704 | −0.4307 | 73 | 12 |
| 260-500 | +25.7547 | +19.4861 | 0.757 | −4.8117 | −0.4962 | 60 | 14 |
| 500-1000 | +34.5509 | +36.4667 | **1.055** | −4.8211 | −2.2156 | 159 | 15 |
| 1000-1500 | +35.1150 | +36.2421 | **1.032** | −8.5717 | −7.1214 | 200 | 19 |
| 1500-2000 | +36.9756 | +34.6907 | **0.938** | −20.4438 | −12.7062 | 339 | 20 |
| 40-70 | +15.3400 | **−3.7718** | −0.246 | +0.0736 | +2.4329 | **2** | 270 |

Six of seven comparable bands are inside ±10%; `260-500` is the one outside at 0.757. The
`40-70` row is **not** a cross-side divergence — the original has n=2 there, so there is nothing
to compare — but it is a clean statement of the port's trap as a force budget: **in the band
where the port spends 270 of 1628 frames, its control force points AGAINST its own velocity
(`T_drive = -3.7718`).** That is §20.14's `-0.1` reverse gate (port below it on 625/1352 frames
against the original's 25/1352) expressed in speed units per frame for the first time. It is
recorded, not acted on: it is a consequence of the trap's residency, and §21.5 measured that the
original passes below 100 horizontal once per race for 6-8 frames of 6658.

## 5. The ORIGINAL's free-flight budget, per frame (new, descriptive)

Gap 0 = frames 980..997, between the original's first two post-bounce contacts (979, 998).
`T_drive + T_rest + T_clamp == s_post(f) - s_post(f-1)` exactly; max identity residual
**3.553e-15** over 109 gap steps.

| f | s_from | s_to | dS | `T_drive` | `T_rest` | `T_clamp` |
|---:|---:|---:|---:|---:|---:|---:|
| 980 | 231.471 | 190.642 | −40.829 | **+1.778** | **−42.586** | −0.0202 |
| 982 | 158.937 | 132.760 | −26.177 | +3.927 | −30.108 | +0.0045 |
| 985 | 95.968 | 85.448 | −10.520 | +9.494 | −20.010 | −0.0038 |
| 986 | 85.448 | 89.023 | **+3.575** | +12.044 | **−8.464** | −0.0050 |
| 990 | 113.876 | 120.024 | +6.148 | +19.283 | −13.142 | +0.0074 |
| 993 | 148.692 | 161.561 | +12.869 | +22.002 | −9.127 | −0.0059 |
| 997 | 211.850 | 231.529 | **+19.679** | **+24.553** | **−4.887** | +0.0127 |
| | **net over gap 0** | | **+0.059** | **+268.247** | **−268.168** | **−0.0204** |

> **The original's post-bounce recovery is "the slip drag decays and the drive wins."** `T_rest`
> is **not** a function of speed: it is −34.5 at `s_from` 190.6 on the way down and −5.5 at
> 193.5 on the way up, **6x apart at the same speed**. `T_drive` is also suppressed right after
> the bounce (+1.78 at frame 980) and recovers to +24.6 by frame 997. And `T_clamp` is **−0.0204
> over the whole 18-frame gap** — grip-clamp #6 costs the original essentially nothing in free
> flight.
>
> Note that §22.4's "+11.63 net over gap 0" is in HORIZONTAL speed (219.89 -> 231.52). In full
> `|v|` the original's gap 0 is net **+0.059** (231.471 -> 231.529) and strongly non-monotonic
> (down to 85.4 at frame 985, back up). Any median taken over such a window mixes the two
> regimes; that is why the per-frame table above is the reportable form and why §22.4's
> gap-median framing could not name a term.

Per gap (medians per step), original:

| gap | frames | n | s_from->s_to | med dS | med `T_drive` | med `T_rest` | med `T_clamp` |
|---:|---|---:|---:|---:|---:|---:|---:|
| 0 | 980..997 | 18 | 231.47->231.53 | +7.267 | +17.1921 | −8.3930 | −0.0015 |
| 1 | 999..1010 | 12 | 207.10->181.73 | +1.627 | +17.9930 | −16.3705 | +0.0024 |
| 2 | 1012..1022 | 11 | 181.12->178.00 | +2.875 | +18.3623 | −15.4891 | +0.0034 |
| 3 | 1024..1034 | 11 | 178.88->187.30 | +4.470 | +19.1809 | −14.7093 | +0.0032 |
| 4 | 1036..1046 | 11 | 182.92->199.81 | +5.685 | +20.0259 | −14.3548 | −0.0021 |
| 5 | 1048..1059 | 12 | 189.45->224.62 | +8.136 | +20.7128 | −12.5754 | −0.0004 |
| 6 | 1061..1071 | 11 | 203.22->258.77 | +9.832 | +22.8796 | −13.0396 | −0.0024 |
| 7 | 1073..1083 | 11 | 225.62->313.14 | +13.090 | +25.2058 | −12.1106 | −0.0051 |
| 8 | 1085..1096 | 12 | 262.29->405.82 | +17.497 | +26.5150 | −9.6483 | −0.0006 |

## 6. The PORT's `T_rest` split (port-side only; the original's cannot be split from a snapshot)

From `friction_diag.log`'s verbatim `accum`, joined at offset `-2`:
`T_accum = linTerm*(accum.u)`, `T_2nd = T_rest - T_accum` (the second-order direction-change
part).

| cut | n | med `T_accum` | med `T_2nd` |
|---|---:|---:|---:|
| ALL | 1627 | −0.7880 | +1.6194 |
| 150-260 | 12 | −0.8826 | +0.0559 |
| 40-70 | 269 | −0.0987 | +2.5275 |
| 70-100 | 38 | −0.7373 | +0.4924 |
| 1500-2000 | 20 | −1.3396 | −11.3666 |

The port's wheel-force accumulator removes **−0.88 speed units per frame at 150-260** where the
original's whole `T_rest` is **−9.67**. **This is NOT reported as an 11x accumulator deficit**:
the original's `T_rest` could not be split (`accum` is a local, registered up front as a
limitation of this lane), and `T_2nd` on the port is of the same order as `T_accum`. Splitting
the original's `T_rest` needs an original-side witness on `l_b8/l_b4/lin_b0`, which is the next
lane's first job if anyone picks this up (see §8).

## 7. Scored table (3 of 3, no-change control — this lane changed no source)

| metric | port | n | median speed | PASS interval | verdict |
|---|---:|---:|---:|---|---|
| slip 1500-2000 | **0.2033** | 20 | 1683.53 (in band) | 0.18855 .. 0.19635 | **FAIL** +3.4% past the bound |
| slip 2000-2600 | **—** | 0 | — | 0.24488 .. 0.25487 | **UNSCORABLE** |
| driving-median | **1355.66** | 54 | 1355.66 | 1904.70 .. 1982.44 | **FAIL** −30.2% |

Identical to every printed digit on **3 of 3** runs (`p1`, `score2`, `score3`), which meets
§3b's determinism precondition and equals §21.6 / §22.2 / §22.4. Whole-window median horizontal
speed over the first 1080 frames **26.33**, grounded `+0x9e0 == 4.0` on **1080 / 1080**
(player_trace; §22.4's 26.36 was measured from `motion_diag`). Bounds unchanged. Dual-copy
guard **`allowlisted=122 NEW=0`**.

Instrument-insensitivity control, measured: `verify/d2_bounce_20260930/p1/player_trace.log`
(with the 7 MB `MASHED_A6ADUMP` armed) and `p2/player_trace.log` (without) are **identical on
1496 of 1496 common frames**, so arming `MASHED_COUPLING_DIAG` + `MASHED_A6ADUMP` for this
lane does not move the trajectory — and the three scored runs above (p1 with every log armed,
score2/score3 with none) returning the same digits confirms it end to end.

## 8. What this lane RULED OUT, and the next lane

**Ruled out by measurement in this lane:**
1. **The drive / control force `+0xb14/+0xb18/+0xb1c` is not the term.** Matched-band ratio
   0.899..1.055 on six of seven comparable bands (S2-validated estimator, 1627/1627). This is
   the measurement the kickoff said §20.10 did not cover.
2. **A hidden vertical control force.** `+0xb18` is exactly 0.0 on 2332/2332 original frames
   and the port's `|ctrl.y|` median is 0.009 against `|ctrl.z|` 786534.
3. **`linTerm`.** Identical on both sides: `+0x54` constant on 2332/2332 and 1628/1628,
   `kDt = 1/3000` exactly, the port prints `linTerm=1.66667e-05`.
4. **The "gain between contacts" framing itself.** It decomposes into exactly three terms, two
   of which agree cross-side, and the third is grip-clamp #6 — a route closed by §21.5. So
   **U-9156 is not an independent lever**, confirming §22.4's own `[UNCERTAIN]` rather than
   overturning it. **U-9156 can be closed as "resolved: not a lever".**

**The one clean target invariant this lane produces** (derived, not guessed, and robust to
<= 13% fixup contamination by construction):

> `median( +0x9e4 / |+0x9b0..0x9b8| )` over race frames must be **1.000** to 1e-3.
> ORIGINAL **0.999998** (n=1447). PORT **1.172734** (n=1628).

**Recommended next lane, and it is NOT inside A6a, `0x0046ef70`, or the clamp.** Every term in
the per-frame budget is now measured except one: the ORIGINAL's `T_rest` split. The original's
`T_rest` is a slip drag that reaches −42.6/frame and **decays 6x at constant speed** as the car
straightens; the port's is −0.43/frame at the same speed. Nothing in the closed routes explains
a drag that large or that decay. So the next lane should be:

> **Get the ORIGINAL's `accum` (`l_b8 / l_b4 / lin_b0`) and its blend `frac = (l_d0 - m78)/l_d0`
> as a measurement.** They are A6a locals, so the `.msd` cannot carry them, but the four
> per-wheel force vectors `p[0x1c..0x1e]` (record `wheelbase+0x70..0x78`) and the wheel offsets
> `p[-9..-7]` ARE record fields the `.msd` already has, and `accum` is a blend of their normal
> and tangential parts (`Integrate2.cpp:489-512`, `:543-565`). Fit it from the record on the
> PORT first, where `friction_diag.log` gives the true answer as a known-answer self-check
> (memory `cross-side-fit-needs-both-sides-checked` applies — check BOTH samplers), then apply
> the validated estimator to `orig_fp2.msd`. If the fit will not validate on the port, the
> fallback is an entry hook on whatever callee A6a uses for the per-wheel cross product, the
> same technique §21.9 used for `l_60` via `RwV3dLength`'s pointer argument.

**Still open and untouched by this lane:** U-9160 (substep budget 3-4 against the original's
fixed 2 at `0x00469ad4`, measured inert); the port's `40-70` residency and the `-0.1` reverse
gate's duty cycle (§20.14); D1-residue R1.
