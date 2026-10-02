# RESULT — D2 attempt 17, STEP 2: `+0xb0c` is a SYMPTOM, and where the recovery actually breaks

Executes `PREREG_STEP2.md` (committed `78314357`, unrun) and `PREREG_STEP2B.md`
(committed `683833a2`, unrun). **No gate was amended.** Gate KA failed and is reported
as a failure; KA2 is a separately pre-registered, different question, not a loosened KA.

Every number below carries `n`, median speed and `d` (frames from release, `L = 0`).

---

## 1 The gates, in order

| gate | what it asked | result | verdict |
|---|---|---|---|
| **KA** snapshot | recompute `+0xb0c` from the `.msd`'s own inputs, `rel <= 1e-4` on `>= 99 %` | `orig_bp1` **0.948789**, `orig_bp2` **0.940083**, `orig_solo3` **0.945329** (all best at `lam = -1`) | **FAIL** |
| **KA** live | same, cross-call on the A4-entry hook | **0.971698** (2266/2332) | **FAIL** |
| **KA2** live | same law, budget **4 ulps of `max(speed,1)`** | **1.000000** (2332/2332); ratio median **0.1596**, p99 **1.573**, **max 2.034** | **PASS** |
| **D-0** | the appended `vel=`/`fwd=` must not move any pre-existing token | **0** differing (line, token) pairs over **1625** shared lines, 28 shared tokens, B-only exactly `{vel, fwd}`, line delta 1 | **PASS** |
| **DR** | first diverging INPUT at matched `d`, order `fwdlen, speed, vellen, mis`, tol 2 % | **`speed` (+0x9e4) at `d = 0`**, O `0` vs P `0.65`, gap 65 %; `fwdlen` gap **0.0000 %** at that `d` | **named** |
| **CB** | `fVar5 = max(1500 - b0c, 500)` gap over `d = 0..400` | **66.65 % at `d = 372`** | **CB-LARGE** |

### 1.1 Why KA failed, measured not asserted

KA's own worst miss: `seq = 938`, `speed = 757.258`, `pred = 0.00055225714`,
`stored = 0.00054163329`. The absolute disagreement is `1.06e-5` on operands of
magnitude `757`; one float32 ulp there is `4.51e-5`, so the "miss" is **0.24 of one
ulp**. `+0xb0c` is algebraically `speed - |dot|`, so a tolerance relative to the
*result* measures the subtraction's cancellation, not the law. KA asked a question this
quantity cannot answer, on **both** routes. That is a fact about `+0xb0c` and it is the
first half of this attempt's answer.

### 1.2 KA2, and what it establishes

Over the whole live capture, with the writer's own inputs at the writer's own phase, the
original's stored `+0xb0c` reproduces

```
+0xb0c = 0                                  if speed == 0.0          0x0047072c
+0xb0c = (1.0 - |dot| / speed) * speed      otherwise                0x00470724
```

on **2332 of 2332** consecutive-call pairs, worst case `2.034` ulps of its own operands.
`n = 2332`, median speed `726.724`, covering `d = -890 .. +1443`. Regime split: 890 rows
through the `speed == 0.0` branch, 1442 through the formula.

So **the transcription in `RESULT_STEP1.md` §2.1 is exact**, and the port computes that
same expression (`VehicleControl.cpp:115`, `PhysicsChainHooks.cpp:302`; `vc::kOne` is
`1.0f` and `_DAT_005cc320` reads `0x3f800000` in the live image, self-checked by the
probe at arm time together with `0x005d757c = 0.0`, `0x005cd0ac = 1500.0`,
`0x005ccd04 = 500.0`).

**Therefore `+0xb0c` cannot diverge on its own arithmetic.** It is a symptom of its
seven inputs.

### 1.3 Probe coverage

`calls 2333, mine 2333, skipped 0, capped false, err null` — A4 fires exactly once per
frame for slot 0 (2333 calls against 2334 captured frames), so there is no substep
multiplicity to account for, and nothing was dropped.

---

## 2 The release frame had to be re-derived, and that matters

`a8_launch.py`'s `--orig-release 886` is **capture-specific** and is wrong for
`orig_sl1.msd`. The capture carries its own release marker (`+0xbf8` leaves 0 at the
rev-charge release, `0x0046d7a2`) and it reads **`sdframe = 890`**; first `speed > 0` is
`sdframe = 892`. With `R = 886` the original's peak lands at `d = 99` and `+0xb14`
engages at `d = 19`; with **`R = 890`** both snap back onto the published values:

| | peak | `b14` engages | post-trough `>= 100` | median |
|---|---|---|---|---|
| ORIGINAL `orig_sl1.msd`, R=890 | **1832.40 at `d` = 95** | **`d` = 15** | 398/400 = **99.5 %** | **1333.9** |
| PORT `p1`, R=1 | **1835.50 at `d` = 95** | **`d` = 15** | 243/400 = **60.8 %** | **132.8** |
| best-fit lag `d`=16..95 (n=80) | | | | **L = 0 at 0.19 %** |

Identical to attempts 15 and 16 to every digit, on both arms.

---

## 3 DR's answer, and its honest size

**`speed` (+0x9e4) at `d = 0`: ORIGINAL `0`, PORT `0.65`.** `fwdlen` — the only term
ordered before it — has gap `0.0000 %` at that `d` and **never** exceeds 2 % anywhere in
`d = 0..400`; the forward vector is unit to 7 decimals on both sides throughout.

The magnitudes must be read with the gap: `0` against `0.65` on a floor of `1.0` is 65 %
by construction, and `0.65` is **0.035 %** of the peak. What it says plainly is that the
**original's car is at bit-exact rest at release and the port's is already creeping.**
The original is at exactly `0.000000` on 890 consecutive probe rows; the port's frame
before `d = 0` carries `vel = (0, -216.668, 0)`, a purely vertical drop, so the port
settles its car onto the track where the original places it at rest.

Per-term first exceedance (diagnostic, not the gate):

| term | first `d` > 2 % | ORIG | PORT |
|---|---:|---:|---:|
| `fwdlen` | **never** | | |
| `speed` | 0 | 0 | 0.65 |
| `vellen` | 0 | 0 | 0.637358 |
| `mis` | 21 | 2.6346e-05 | 5.3718e-05 |
| `b0c` | 1 | 0 | 0.00831162 |

`mis` at `d = 21` is two near-zero misalignments separated by `2.7e-05` and flagged only
by the pre-registered `1e-3` floor; both cars are driving straight there.

`+0xb0c` first exceeds at `d = 1`, **after** `speed` and `vellen` at `d = 0`, which is
DR's point: `b0c = speed - |dot|`, and with the port moving at `0.75` and near-perfectly
aligned at `d = 1`, `0.00831162` is exactly the residue that speed implies.

---

## 4 CB, and the window where causation can actually flow

CB over the registered `d = 0..400` is **CB-LARGE**: `max |fVar5_O - fVar5_P| / max(..)`
= **66.65 % at `d = 372`**; `max |b0c|` O **1871.69**, P **1747.42**; the `500.0` lower
clamp engages on O 2 frames, P 1.

That verdict stands as registered. But `d = 372` is 270 frames past the trough, where
the two cars are in **unrelated states** (median speed over `d` 301-400: O **2156.86**,
P **109.31**, n=100). Reported alongside, not as an amendment:

> restricted to `d = 0..101` — the window in which the two arms are still the same
> manoeuvre — **`max fVar5 gap = 3.9892 %`, n = 102**, i.e. inside CB's own 5 %
> CB-SMALL threshold.

And in that same window the two arms are indistinguishable in the quantity that matters:

| window | n | median speed ORIG | median speed PORT |
|---|---:|---:|---:|
| `d` 0-101 | 102 | **609.07** | **609.43** |
| `d` 102-200 | 99 | 157.76 | 167.47 |
| `d` 201-300 | 100 | **1199.40** | **350.58** |
| `d` 301-400 | 100 | 2156.86 | 109.31 |
| `d` 401-520 | 120 | 1864.79 | 49.26 |

**Through launch, peak and trough the port tracks the original to 0.06 % on median
speed.** `+0xb0c`'s whole channel over that window is bounded at 3.99 %. It is not the
~10x recovery deficit, and nothing downstream of it is either, until the arms have
already separated.

---

## 5 Where the recovery actually breaks — the next term, reported NOT fixed

**Descriptive, not a pre-registered gate.** The rule below was chosen after seeing §4's
window table and is labelled as such: *the first `d >= 102` at which the port's speed is
more than 2 % below the original's and stays so for 20 consecutive `d`.*

> **`d = 222`** (ORIG `426.86`, PORT `415.81`).

Per-sample `dspeed` at matched `d` (per-sample, never endpoint totals — memory
`rate-stats-per-sample-not-totals`), against the drive force and the contact flag:

| window | n | median `dspeed` O | median `dspeed` P | median speed O / P | median &#124;b14<sub>xz</sub>&#124; O / P | gnd O / P |
|---|---:|---:|---:|---|---|---|
| `d` 102-200 | 98 | +10.58 | +6.70 | 157.8 / 167.1 | 1.352e6 / 1.319e6 | 4 / 4 |
| `d` 200-222 | 22 | +17.59 | **+19.06** | 335.5 / 346.6 | 1.679e6 / 1.678e6 | 4 / 4 |
| **`d` 222-250** | **28** | **+27.87** | **+4.86** | **805.7 / 660.9** | **2.381e6 / 2.233e6** | **4 / 4** |
| `d` 250-300 | 50 | +14.51 | **-5.40** | 1669.5 / 231.9 | 2.617e6 / 1.405e6 | 4 / 4 |
| `d` 300-400 | 100 | +4.17 | +2.26 | 2156.9 / 110.5 | 3.225e6 / 1.199e6 | 4 / 4 |

The decisive row is **`d` 222-250**:

- the drive force agrees to **6.2 %** (`2.381e6` vs `2.233e6`),
- all four wheels are grounded on both sides (`gnd = 4.0`, exact, every frame),
- both cars are aligned and in the same gear,
- and the port's speed gain is **5.7x short** (`+27.87` against `+4.86` per frame).

At `d` 200-222, immediately before, the port is **faster** (`+19.06` against `+17.59`)
on the same force. So the deficit switches on inside a 28-frame window, with the drive
force held nearly constant across it.

**Named term, unfixed:** the consumer of `+0xb14/+0xb1c` — the velocity integration and
its clamp chain — **not** `+0xb0c`, **not** the drive force, **not** the contact state.
It is reported without a fix because `PREREG_STEP2.md` §7 requires live confirmation on
the running original first, and that test has not been run.

### 5.1 The gearbox is a consequence, not a cause

Gear (`+0x490`) and its timer (`+0x494`) track **exactly** through `d = 0..250`
(0 -> 1 -> 2 -> 0, timers within 50-100). After that the port falls to gear 0 and stays:

| `d` | 250 | 280 | 300 | 350 | 400 | 450 |
|---|---|---|---|---|---|---|
| ORIG gear / speed | 1 / 1212 | 1 / 1731 | 2 / 1990 | 3 / 2223 | 2 / 1439 | 3 / 2225 |
| PORT gear / speed | 1 / 537 | **0** / 182 | **0** / 168 | **0** / 233 | **0** / 15 | **0** / 68 |

Over `d = 200..520` the original spends 120 frames in gear 2 and 76 in gear 3; the port
spends **283 of 321 frames in gear 0** and never leaves gear 1. Gear 0 at 180 speed is
the *correct* response to being slow, so this follows the speed collapse of §5 rather
than causing it. It is recorded because the upshift test at `Integrate2.cpp:131-143`
reads `fVar5 = 1500 - b0c`, i.e. `+0xb0c`'s own channel — and §4 bounds that channel at
3.99 % through `d = 0..101`, before the gears diverge at all.

---

## 6 The one non-faithfulness found, named and NOT fixed

`RESULT_STEP1.md` §4: both port copies evaluate the dot product in a different
association order from the original.

```
ORIGINAL   0x004706db..0x00470701     dot = (fwd.y*vel.y + fwd.x*vel.x) + fwd.z*vel.z
PORT       VehicleControl.cpp:111     dot = (fwd.z*vel.z + fwd.x*vel.x) + fwd.y*vel.y
           PhysicsChainHooks.cpp:298  (same order)
```

Deliberately not fixed this attempt. It is float-rounding sized, it cannot be the
defect, and claiming it honestly needs the full promotion leg on A4 `0x00470670` (a
0x321-byte function) — a cost with no measurable return. It is recorded with its RVA and
both file:line so the next attempt can take it with the leg attached.

---

## 7 STEP 4 — the four verdicts

Build behaviourally identical to attempts 15 and 16 (§1 gate D-0 and §9 leg 1); the only
`mashedmod/src` change in this attempt is the diagnostic `fprintf`. No fix was authored,
so the dual-copy guard is **NEW = 0 by construction** (no new body at any RVA).

**(a) `+0xb0c` tracks within tolerance at matched `d`:** **FAIL** — it first exceeds 2 %
at `d = 1` (O `0`, P `0.00831162`) and the late regime is unbounded. **No fix was
applied, and §1.2/§3 establish it as a symptom rather than a term to fix.**

**(b) launch:** **PASS** — `L = 0` at **0.19 %**, `+0xb14` engages at `d = 15` on both
arms, peak **1835.50 at `d` = 95** against the original's **1832.40 at `d` = 95**.

**(c) recovery, H1 (`>= 50 %` of 400 post-trough frames `>= 100` **AND** median
`>= 900`):** **INCONCLUSIVE** — **243/400 = 60.8 %** (fraction passes), median **132.8**
(median fails), against the original's **398/400 = 99.5 %** and **1333.9**.

**(d) the three metrics against the unchanged `d81a8df6` bounds, 3 runs, §16.7 arm
(`MASHED_MEASURE_SOLO=1 MASHED_TRACK_SEL=12 MASHED_STEER_HOLD_AFTER=0`),
participants=1 confirmed from the game's own `MATCH-SEED rule=0 participants=1`
line:** **FAIL 3 of 3**, and `p1`/`p2`/`p3` agree to **every digit** (determinism,
§3b satisfied).

| metric | bound | PORT | n | median speed | median `d` |
|---|---|---|---:|---:|---:|
| slip 1500-2000 | 0.18855 .. 0.19635 | **0.1983** | 19 | 1665.05 | **85** |
| slip 2000-2600 | 0.24488 .. 0.25487 | **UNSCORABLE** | **0** | — | — |
| driving-median | 1904.70 .. 1982.44 | **1019.77** | 76 | 1019.77 | **79** |

§26.10's median-frame guard fires on every scored row: the ORIGINAL populates the same
three at median `d` **718 / 915 / 835** (n = 312 / 553 / 1176, median speed 1790.86 /
2218.87 / 1944.28). The magnitudes are therefore not readable as physics errors; the
bounds are still scored and are still not met.

---

## 8 Why this is not a `+0xb0c` fix, stated once

1. The writer's law reproduces on the running original **2332/2332** at 4 ulps (KA2).
2. The port computes that same law, with constants verified against the binary.
3. Through `d = 0..101` the two arms' median speed differs by **0.06 %** and
   `+0xb0c`'s entire A6a channel is bounded at **3.99 %** (n = 102).
4. The recovery gap opens at **`d = 222`**, with the drive force agreeing to 6.2 %, the
   contact identical, and the per-frame speed gain short by **5.7x** (n = 28).

A fix to `+0xb0c` would have to change a value that is already correct where it could
matter. There is no faithful single-producer change available, and the only forms
available would be a clamp or a fitted constant, both forbidden.

---

## 9 Collateral

**Leg 1 — paired, same side, attempt-15 build against this build.** `collateral.py
--mode paired`, A = `verify/d2_writer_20261001/r1`, floor-A = `r2`, B = `p1`, floor-B =
`p2`. 1626 aligned frames, **70 paired fields, 0 of 69 divergent, all 69 within the
noise floor on every aligned frame** (`av`, `b0c`, `b14[0..2]`, `bodyH`, `d[]`, `fl[]`,
`ftot[]`, `gb498`, `gb49c`, `gear`, `gnd`, `gt[]`, ...). Unpaired: **B-only exactly the
6 new fields** `vel[0..2]`, `fwd[0..2]`. **No outside-scope rows.** The diagnostic
append is behaviourally inert at field level with a measured floor, independently of
gate D-0's token check.

**Leg 2 — cross-side, banded.** `--mode banded`, A = `orig_sl1.msd`, floor-A =
`orig_bp1.msd`, B = `p1`, floor-B = `p2`, banded on `msd+0x9e4`, scope
`scope_a6a.txt`, 14 fields mapped by name to their record offsets.

> **All SIX bands are `!!` OFF-REGIME** (median frame indices 1023-1556 against
> 68-381). Per §26.10's standing rule **not one row was read**, and none is reported.

That is itself the finding of §4 restated: by the time the arms share a speed band they
are hundreds of frames apart, which is exactly why the matched-`d` table was the right
instrument and the banded one is not. Three fields are within the floor in **every**
band and are exact on both sides: `msd+0x498` (40000), `msd+0x49c` (4000), `msd+0x9e0`
(4, all wheels grounded). **No outside-scope divergence is claimed from this leg**,
because no row in it is readable.

---

## 10 Artefacts

```
verify/d2_b0c_20261002/
  PREREG_STEP2.md   PREREG_STEP2B.md   RESULT_STEP1.md   RESULT_STEP2.md
  orig_sl1.msd                      2334 frames, live original + --slide-probe
  orig_sl1.msd.slideprobe.csv       2333 rows, the A4-entry capture
  orig_sl1.msd.provenance.json
  p1/ p2/ p3/                       three port runs, §16.7 arm, identical to every digit
  cross_d.csv  cross_d520.csv       matched-d cross tables
  collateral_paired.csv  collateral_banded.csv
re/tools/fold_sweep.py              folded-base capstone sweep, self-checked on +0xbf8
re/tools/statediff/a17_slide.py     the reducer (KA, KA2, DR, CB, matched-d cross)
re/frida/scenario_launch.py         --slide-probe / --slide-probe-limit
```
