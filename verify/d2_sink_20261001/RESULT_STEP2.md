# D2 attempt 12 — STEP 2 result: **R3 fires.** §24.3's ORIGINAL `resid` column is WITHDRAWN, and with it §24.4's sub-500 sink.

Pre-registration [`PREREG_2.md`](PREREG_2.md), commit `0d5ff8b7`, **not amended**.
Tool `re/tools/statediff/a12_entry.py` (read-only, executes no game, changes no source).
Inputs, both unchanged from attempt 10 and from the **same** `scenario_launch.py` invocation:
`verify/d2_bounce_20260930/orig_fp2.msd` (2332 frames) and
`orig_fp2.msd.fixupprobe.csv` (2331 site-2 rows, entry hook on `0x00467650`, ESI-filtered).
Raw output: [`step2_entry.txt`](step2_entry.txt).

## 1. All three gates PASS, and the join is EXACT

| gate | bar | result | verdict |
|---|---|---|---|
| **G1** count | n >= 1000 and within +/-2% of msd frames | **2331** vs 2332, diff **1** (bar 46.6) | **PASS** |
| **G2** join, known-answer | median rel <= 1e-5 **and** >= 100x the runner-up | best `d=+2` **0.000e+00** (bit-exact on 1443 frames), runner-up `d=+3` **9.896e-03** | **PASS** |
| **G3** non-degeneracy | entry vel differs from snapshot vel on >= 50% | **1432 of 1432** live frames (100.0%) | **PASS** |

The join is **bit-exact**, not approximate: the site-2 `+0x9e4` read at A6a entry equals
`msd[f-1].s_mid` to the last bit on every one of 1443 frames, which is what the two known RVAs
(`0x004686cc` writes it, `0x00467673` overwrites it one frame later) predict. The next-best shift
is wrong by 1%. The original's A6a-entry velocity is now a **measurement**.

## 2. M1 — the original's pre-A6a change IS a pure scalar

| | n | median spread | p95 | max | verdict |
|---|---:|---:|---:|---:|---|
| ORIGINAL per-component ratio spread | 1432 | **3.813e-08** | 7.498e-08 | 1.060e-07 | **PURE** |
| PORT (§24.3, for comparison) | 1628 | 3.769e-08 | — | 1.011e-07 | pure |

The two sides agree to the digit. The `.msd`-only [UNCERTAIN] of attempt 11 §24.3 is **closed**:
the original's pre-A6a velocity change is a pure scalar multiply, same as the port's.

## 3. M3 — the known-answer check FAILS in five of six bands. **R3.**

| band | n | `sigma_orig` direct | `resid` **direct** | §24.3 `resid` back-out | rel | verdict |
|---|---:|---:|---:|---:|---:|---|
| 100-150 | 45 | 0.999481 | **-0.0655** | -12.9566 | 0.995 | **OVER** |
| 150-260 | 73 | 0.999200 | **-0.1531** | -8.5290 | 0.982 | **OVER** |
| 260-500 | 60 | 0.998422 | **-0.5826** | -4.0489 | 0.856 | **OVER** |
| 500-1000 | 159 | 0.996500 | **-2.9222** | -4.3643 | 0.330 | **OVER** |
| 1000-1500 | 200 | 0.994600 | **-6.9934** | -7.8728 | 0.112 | ok |
| 1500-2000 | 339 | 0.992359 | **-13.5587** | -19.4471 | 0.303 | **OVER** |

Bar was rel <= 0.25 in every band with n >= 10. Worst band 100-150 at **0.995**, i.e. the
back-out is **198x** larger than the measurement there. **R3 fires as written.**

> ### §24.3's ORIGINAL `resid` column is WITHDRAWN. There is no sub-500 sink on the original.
>
> §24.4's `local_70` of **182.5 / 54.2 / 5.95** at 100-150 / 150-260 / 260-500 was an artefact
> of that back-out. The directly measured `sigma_orig` at 100-150 is **0.999481** — the original
> removes **0.05%** of its speed per frame there, not the 5% the back-out implied. Nothing with
> `local_70` out of `[0,2]` is needed, and **U-9170 is moot**: the question "is the sub-500 sink
> speed- or slip-coupled" has no referent, because there is no sub-500 sink.

## 4. M2 — the pre-A6a scalar AGREES across sides, in every band

Against step 1's **directly logged** port `sigma` (`a5g_diag.log`, same quantity, no back-out on
either side now):

| band | ORIG `sigma` | PORT `sigma` | ORIG 1-sigma / PORT 1-sigma | med speed o / p |
|---|---:|---:|---:|---|
| 100-150 | 0.999481 | 0.9994679 | 0.975 | 126.2 / 115.3 |
| 150-260 | 0.999200 | 0.9991312 | 0.921 | 189.5 / 194.9 |
| 260-500 | 0.998422 | 0.9984833 | 1.041 | 371.1 / 349.8 |
| 500-1000 | 0.996500 | 0.9969746 | 1.157 | 821.6 / 735.8 |
| 1000-1500 | 0.994600 | 0.9947493 | 1.028 | 1280.9 / 1275.5 |
| 1500-2000 | 0.992359 | 0.9930763 | 1.103 | 1778.7 / 1676.5 |

**0.92x to 1.16x over a 14x speed range.** The pre-A6a velocity sink is not a cross-side defect
at any speed. §24.4's "T2/T3 refute C1 as the whole of the ORIGINAL's" is **withdrawn**: C1 is
the whole of it on both sides, to within the band-matching error.

## 5. M4 — speed and slip are NOT separated in this capture, as predicted

| band | med slip `\|v_perp\|/\|v\|` | n |
|---|---:|---:|
| 100-150 | **0.727** | 45 |
| 150-260 | **0.478** | 73 |
| 260-500 | 0.199 | 60 |
| 500-1000 | 0.080 | 159 |
| 1000-1500 | 0.083 | 200 |
| 1500-2000 | 0.189 | 339 |

Slip rises monotonically as speed falls below 500, exactly the confound U-9170 named. Stated
explicitly, as the session brief requires: **this result does not separate speed from slip.** It
does not need to for the headline, because the headline is a *null* — the original's low-speed
pre-A6a loss is 0.05%/frame, which is small under either reading.

## 6. What the arithmetic now says, and where the lane goes

The total per-frame speed budget on the original splits into two measured halves:

```
  s_mid(f) - |snapVel(f-1)|  ==  [ |entryVel(f)| - |snapVel(f-1)| ]  +  [ s_mid(f) - |entryVel(f)| ]
                                          PRE-A6a                              INSIDE A6a
                                    MEASURED: -0.0655 @100-150            the remainder
```

§24.3 put 88-96% of `T_rest` **outside** A6a. That conclusion rests on `residA -> 0.0000`, which
was measured on the **PORT ONLY** (`g_a6aFrame.vel`, `Integrate2.cpp:235-239`); the original had
no such channel until now. With the channel in hand, the original's pre-A6a share at 100-150 is
**-0.0655 of a -13.98 `T_rest`, i.e. 0.5%**. So on the ORIGINAL the carrier is **inside A6a**,
and §24.6's closed route 13 ("the whole of A6a as the home of `T_rest`") was a port fact being
read as a both-sides fact — the same error §24.7 recorded one attempt earlier about a phase gap.

**Step 3 therefore moves back inside A6a at low speed**, with the pre-A6a half excluded by direct
measurement on both sides. That is pre-registered separately before anything is run.
