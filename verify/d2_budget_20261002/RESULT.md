## RESULT — D2 attempt 18

**The per-frame velocity budget at matched `d` (STEP 1), terms in CALL ORDER by RVA.**
`re/tools/statediff/a18_budget.py`; `linTerm = 1.6666668613120733e-05` and the (a)/(b) bars
imported from `a10_gain.py`, not restated. `d = 222..250`, **n = 29 each arm**, `+0x9e0 ==
4.0` on 29/29 both, `|ctrl_xz| > 0` on 29/29 both, median speed O **790.9** / P **633.2**,
gear `[0,1]` both, median `|ctrl_xz|` 2.385e6 / 2.223e6.

| term | RVA span | ORIG | PORT | &#124;delta&#124; | (a) | (b) | OUT |
|---|---|---:|---:|---:|---|---|---|
| T_drive | `0x0046862d..0x004686a2` | +33.20455 | +27.08079 | 6.12376 | PASS | no | |
| T_rest | `0x00470670` + `0x0046ddb0` + `0x0046833a..0x00468625` | -3.95911 | -2.50577 | 1.45334 | no | no | |
| **T_post** | `0x004687f0..0x0046897b` + `0x00468980` + `0x004709a0`×N | **-0.00464** | **-27.50393** | **27.49929** | **PASS** | **PASS** | **OUT** |
| dS | | +27.74798 | -1.04312 | 28.79110 | | | |
| T_W1 | | +27.78004 | +25.07255 | | | | |

**Identity residual `0.000e+00`** — and it is a **construction** identity (`T_rest` is defined
as `T_W1 − T_drive`), labelled as such in `PREREG_STEP1.md` §2 before the run, **not**
evidence. `T_rest` inside both bars means **U-9171's A5 drag is not the carrier here**.

**Gate results.** **KA-M PASS** (`+0x54` one distinct value `0.0010000000474974513`).
**CO PASS** (ORIG 1442 steps, drops `{no_prev 1, zero_prev 891, nonfinite 0}` — the 891 are
`d < 0` at-rest frames; PORT 1625 steps, drops `{1, 0, 0}`). **EV PASS**. **SS reported**,
and it made the split blocking. **CV PASS** (`a6a 2333 / a6b 2333 / sub 4666`, orphans 0,
`skipped 0`, `err null`, pattern `0,2,1,1` on **2333/2333 = 100.00 %**). **INV leg 1 PASS**
both sides (round-trip 100 %, worst 2.090e-15 / 9.414e-15). **KA-R2 PASS on the original**
(fixup pop 11/396 = 2.78 %; complement 385/385 in range; median **1.000006097**, p25
0.999956816, p75 1.000044055). **INV legs 2 and 3 FAIL** — and §3 below shows why: the
inversion was invalid, so the gate did its job.

**Gates replaced / failed, and why.** **KA-R FAILED** (97.22 % against a 99 % in-range bar)
and was **RETIRED, not loosened**: the 11 out-of-range frames are exactly the 11 above 1.02,
and §23.2's own published prior is **23/1447 = 1.59 %**, so a 99 % bar is **unsatisfiable
against the gate's own prior**. **KA-R2** asks a different question on the complement of a
counted fixup population and was pre-registered in `PREREG_STEP1B.md` (`d14dcc54`) before
being scored. **KA-B FAILED** (37.76 % within 4 ulps, worst 1.42e7) and is reported as a
failure, not amended: it paired the probe's A6a-call ordinal with the `.msd`'s render-tick
frame index as one counter. It cannot touch the registered reading, because **`+0x9e0` at
A6a entry has exactly ONE distinct value — `4` — across all 2333 frames.**

**The diverging term: `T_post`**, and it is a **STANDING divergence, not an onset at
`d = 222`.** It is OUT at `d = 200..222` too (ORIG -0.00401, PORT -6.73326, bars 3.6891 /
0.6394), the port's sink **triples** (-6.73 / -27.50 / -41.47) while its `T_W1` stays flat
(+24.75 / +25.07 / +18.27) and is within **9.7 %** of the original's in the defect window.
`d = 222` is where `|T_post|` crosses `T_W1` and the net gain turns negative. **U-9177's
framing is corrected.**

**The live confirmation (STEP 2), on the running original, existing `--lat-bracket` only.**
`PREREG_STEP2.md` (`3c3e72f4`, unrun). A6a reads `+0x9e0` twice and **writes it zero times**
over all 1243 instructions of `0x00467650..0x0046897b`, so A6a entry is bit-identical to the
gate's own value. **BRANCH OPEN: `G4` true 29/29, 23/23, 11/11** — so `RESULT_STEP1B.md`
§2.2's "the clamp's stores appear not to execute" is **WITHDRAWN** (A5's zero at
`0x0046ddd1` is real but its per-wheel rebuild restores 4.0 before A6a every frame). Then the
**post-hoc diagnostic, labelled as such**: `+0x9e4` is written only pre-clamp (two literal
writers `0x004686cc`/`0x0046bc36` plus A6a's entry `fstp 0x00467673`; the folded sweep over
622 511 instructions gives 4 reads / 0 writes, its `+0xbf8` known answer **PASSING**), so
`R_c = |v|@A6b / (+0x9e4)@A6b` **is** clamp #6's own ratio with the clamp's own axis:

| window | n | med speed @A6b | `1-R_c^2` | floor at `k >= 0.1249` | measured / demanded |
|---|---:|---:|---:|---:|---:|
| `d` 200..222 | 23 | 362.3 | 3.4453e-02 | 1.5075e-02 | **2.29** |
| **`d` 222..250** | **29** | **879.9** | **6.2431e-04** | **3.8398e-04** | **1.63** |
| `d` 250..260 | 11 | 1380.5 | 2.4712e-03 | 1.3794e-03 | **1.79** |

**The original's grip-clamp #6 costs MORE than its own arithmetic demands, in every window.
It behaves exactly as transcribed.** `U-9160`'s original side falls out of gate CV:
**`4666/2333` = exactly `2.0000` substeps per frame**, the first live witness for
`0x00469ad4 mov ebx,2`. Determinism control: `orig_lb18.msd` (release **883**,
`--lat-bracket`) reproduces `orig_sl1.msd` (release **890**, `--slide-probe`) **to every
printed digit at matched `d`**.

**The fix, and the promotion evidence. NONE AUTHORED — and the reason is a measurement, not
a refusal.** Two findings removed the premise a fix would have rested on. (i) Fed with the
**PORT's own measured `l_60` = 676.82** (from its own `wle4`/`wld4`, `Integrate2.cpp:434` /
`:449`, accumulated `:473`), the port is on the **HIGH arm on 29/29** frames with
`G = 458 880` and `k = 0.190822`, so clamp #6 costs **7.2038e-03** of a measured
**7.4031e-02** — **one tenth**, and closing that needs **14.6 deg** of post-clamp body
rotation against **1.11 deg** measured. **§23.2's attribution of the port's 14.7 %/frame to
grip-clamp #6 by elimination is REFUTED, and `k`/`l_60` is NOT the carrier** (both sides' `k`
within **1.5x**; §26.3's LOW-arm 0.540954 does not describe the port at matched `d`).
(ii) The instrument itself is **phase-biased in opposite directions**: **51x understated on
the original** (6.2431e-04 at the clamp's phase against 1.2194e-05 at the render tick, same
capture; `R > 1` on 14/29 against 2/29) and **10.3x overstated on the port**. So §23.2's
"14.7 % vs 0.0002 %" and STEP 1B's **6 072x** factor are **instrument artefacts in unknown
part** — `T_post`'s naming survives, its decomposition does not. **[U-9178]** There is no
faithful single-producer change available: clamp #6 is byte-faithful (§21.5) and now also
measured consistent at its own phase, and the only forms available would be a knob, a clamp
or a fitted constant. **No `mashedmod/src` change at all this attempt — `NEW = 0` by
construction, `git diff 6e512717..HEAD -- mashedmod/src` is empty. No C-level moved; no
`hooks.csv` row touched.** Also withdrawn here before anything was built on it: this
attempt's **own** snapshot inversion of `(k, s)` (§3 of `RESULT_STEP1B.md`), for three
measured reasons — the body rotates **1.3986 deg**/frame (ORIG) and **1.1130 deg** (PORT)
inside the substep loop after A6a against a 2.55 deg misalignment; the original's `k` band is
**172x** wide with only **15 of 29** frames solvable; and the reported medians do not satisfy
the relation they were inverted from.

**STEP 3, four verdicts.** Build byte-identical to attempt 17; participants=1 confirmed from
the game's own `MATCH-SEED rule=0 participants=1` line; 3 runs (s2/s3 byte-identical, s1
differs only in tail capture length 1630 vs 1628).
**(a)** per-frame gain at `d = 222..250`: **FAIL** — no fix applied; `dS` **+27.74798** vs
**-1.04312**, `T_post` **-0.00464** vs **-27.50393** (n=29 each, median speed 790.9 / 633.2).
**(b)** launch: **PASS** — **L = 0 at 0.19 %** (L=14 49.03 %, L=15 51.27 %, L=16 53.43 %),
`+0xb14` engages at `d` = 15 on both arms, peak **1835.50 at `d` = 95** against the
original's **1832.40 at `d` = 95**.
**(c)** recovery, H1 (`>= 50 %` of 400 post-trough frames `>= 100` **AND** median `>= 900`):
**INCONCLUSIVE** — **243/400 = 60.8 %** (fraction passes), median **132.8** (median fails),
against **398/400 = 99.5 %** and **1333.9**.
**(d)** three metrics against the **unchanged** `d81a8df6` bounds, §16.7 arm: **FAIL 3 of 3**.

| metric | bound | PORT | n | median speed | median `d` | ORIG median `d` |
|---|---|---:|---:|---:|---:|---:|
| slip 1500-2000 | 0.18855 .. 0.19635 | **0.1983** | 19 | 1665.05 | **85** | 722 (n=318) |
| slip 2000-2600 | 0.24488 .. 0.25487 | **UNSCORABLE** | **0** | — | — | 916 (n=554) |
| driving-median | 1904.70 .. 1982.44 | **1019.77** | 76 | 1019.77 | **79** | 844 (n=1194) |

§26.10's median-frame guard fires on all three, so the magnitudes are not readable as physics
errors; the bounds are still scored and still not met.

**The next term, named and reported UNFIXED [U-9178].** The PORT's remaining **~90 %** of
`T_post` — **6.68e-02 of 7.40e-02** at `d = 222..250` (n=29, median speed 653.0) — has no
identified producer. Candidates, all inside `[0x004686cc -> the render tick]`: clamp #6, A6b
`0x00468980`, A4's tail parked damp `0x00470948` / `VehicleControl.cpp:278-282` (gate
`+0x9f0 == 2`), and the **3-or-4** substeps of `VehiclePhysicsRun.cpp:915` each reaching
`VehicleContactFixup`'s three anchored writes (`ContactFixup.cpp:261-265` `0x0046f52c`,
`:307-309` `0x0046f5ba`/`0x0046f5c0`, `:326-328` `0x0046f5f3`). **The port's `+0x9f0` and
`+0x9ec` are not in `motion_diag.log`**, so which fires cannot be decided from the committed
captures — the next step is the **port half of the A6b-phase diagnostic** (default-OFF, with
a channel control). And the **ORIGINAL's own 51x** needs explaining too: at this `d` its
interval provably contains clamp #6 and nothing else (`+0x9f0 == 0`, `+0x9ec == 0`,
`+0xb20 == 1`, `+0x2c`/`+0x34` == 0, `+0x18c` == 1.0, `+0x1f0` matching none of the five
track literals, all 29/29; A6b writes no velocity per §26.4), **so one of §22.2's four
"velocity bitwise unchanged across the substep members" statements does not hold here.**

**Collateral.** Leg 1, paired same-side (attempt-17 `p1` vs this attempt's `s1`, floors
`p2`/`s2`): **0 of 75 paired fields divergent, all 75 within the measured noise floor on
every one of 1627 aligned frames**, A-only **0**, B-only **0**. **No outside-scope rows.**
Leg 2, cross-side banded (`--mode banded`, floor-A `orig_sl1.msd`, floor-B `s2`, scope
`scope_a6a.txt`): **all SIX bands are `!!` OFF-REGIME** (median frame indices 1016-1675
against 68-381), so per §26.10 **not one row was read and none is reported**; three fields
are exact on both sides in every band — `msd+0x498` (40000), `msd+0x49c` (4000), `msd+0x9e0`
(4). The matched-`d` cross-side review is the budget itself (`budget.csv`, `budget_s1.csv`,
`clampinv.csv`, `gate.csv`).

**Instrument caveats recorded.** `findoffset.py --writes` is blind to **x87 stores** (it
missed `0x00467673 fstp [esi+0x9e4]`, a real write) as well as to computed bases. The port's
`sp=%.2f` puts a 1.5e-5 floor on `R`, registered before the run; the port's `1-R` is 2515x
that floor, the original's comes from the full-float `.msd`.

**Commits:** `5cc22d0c` (PRE-REGISTER STEP 1, unrun), `c0a0f5cc` (STEP 1 run), `d14dcc54`
(PRE-REGISTER STEP 1B, unrun), `1174da20` (STEP 1B run), `3c3e72f4` (PRE-REGISTER STEP 2,
unrun), `0467723e` (STEP 2 run), `aaebf8e9` (STEP 3 + collateral + trackers via
`re-classify`), and the handoff commit (NEXT_SESSION, ROADMAP §D2, info pane, this file).
Nothing pushed.

**Still open:** **U-9178** — the phase-biased instrument and the port's unidentified ~90 %
`T_post` sink; that is now D2's blocker. U-9177 (named, decomposition blocked by U-9178);
U-9176 (now **two** RVAs: A4 `0x004706db..0x00470701` and clamp #6's
`0x00468771..0x00468793` against `Integrate2.cpp:666`); U-9160 (port half: 3-or-4 substeps);
U-9156; U-9171 (measured **not** the carrier at matched `d`); §20.14's `-0.1` duty cycle;
D1-residue R1. **U-9173 is RESOLVED.** AI slots 1+ keep the fitted seed at
`VehiclePhysicsRun.cpp:702` — not touched, D3 work. D3 modes 3/7 hold stands; **D2 does not
close.**
