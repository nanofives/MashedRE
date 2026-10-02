## RESULT — D2 attempt 19

**The port's `T_post` is now SPLIT, and the carrier is `0x0046f6c0`'s airborne lateral-drift
velocity write — not grip-clamp #6, not A6b, not the parked damp, not the contact fixup, and
not the substep budget.** `d = 222..250`, **n = 29** each arm, median speed ORIG **790.9** /
PORT **633.2**, median frame index PORT **237**, `d` from release R = 1 (PORT) / **883** (ORIG,
re-derived from `orig_lb18.msd`'s own `+0xbf8` marker), `L = 0`.

---

### 1 The split of `T_post`, per producer and per substep

Instrument: `MASHED_D2SINK` (`mashedmod/src/mashed_re/Vehicle/D2SinkProbe.cpp`), default-OFF,
exe-only, samples `|v|` at every producer's own phase inside `[the +0x9e4 store 0x004686cc, the
render tick]`. `PREREG_STEP1.md` (`3837cfe5`, unrun). ORIG `T_post` **-0.00464** as a total,
reused from attempt 18's capture, not decomposed.

| term | site | n | median | share of `T_post` |
|---|---|---:|---:|---:|
| `D_clamp6` | `0x004687f0..0x0046897b` | 29 | **-2.668457** | **9.70 %** |
| `D_a6b` | `0x00468980` | 29 | +0.000000 | 0.00 % |
| `D_damp` | `0x00470948`, gate `+0x9f0 == 2` | 29 | +0.000000 | 0.00 % |
| `D_gap` | A4 exit -> loop top | 29 | +0.000000 | 0.00 % |
| `D_tail` | last `sub_end` -> render tick | 29 | +0.000000 | 0.00 % |
| `D_sub[0]` | substep 0, chunk 25.000000 ms | 29 | -8.298279 | 30.17 % |
| `D_sub[1]` | substep 1, chunk 25.000000 ms | 29 | -7.737854 | 28.13 % |
| `D_sub[2]` | substep 2, chunk **0.0000038147 ms** | 29 | -7.157898 | 26.03 % |

Per substep, by leg: `D_orient` (`0x0046e9e0`) **+0.000000** and `D_fixup` (`0x0046ef70`)
**+0.000000** on every substep; the whole of each substep is `D_wheel` (`0x0046f6c0`).

By CODE SITE, summing each site over the frame's substeps before the median:

| site | RVA | n | median | share |
|---|---|---:|---:|---:|
| **`SolveWheelContacts`** | **`0x0046f6c0`** | 29 | **-23.194031** | **84.33 %** |
| grip-clamp #6 | `0x004687f0..0x0046897b` | 29 | -2.668457 | 9.70 % |
| A6b / parked damp / A9 / fixup / gap+tail | — | 29 | +0.000000 | 0.00 % each |

**Three of U-9178's four candidate producers are eliminated by measurement.** The parked damp
never fires (`+0x9f0 == 0`, the field attempt 18 could not read). The contact fixup runs on a
median of **0** frames (2 fixups over all 29) and its net is exactly zero. A6b writes no
velocity, now measured on the port as well as the original.

**And `D_clamp6` = 9.70 % reproduces attempt 18's independent route** (the port's own measured
`l_60` = 676.82 through clamp #6's transcribed arms: 7.2038e-03 of 7.4031e-02, *"one tenth"*).
Two instruments, same answer — §23.2's refutation is confirmed a second time and `k`/`l_60` is
not the carrier.

### 1.1 Which write inside `0x0046f6c0` — and it is NOT the friction pair

`PREREG_STEP1B.md` (`4bbb5db7`, unrun). The function has three velocity sites:

| site | RVA | hits in window | median/frame | share of `D_wheel` |
|---|---|---:|---:|---:|
| `wcs_fric` (kFricVel 1000.0 arm) | `0x00470072..0x0047009c` | **0** | +0.000000 | 0.00 % |
| `wcs_imp` (kFricImp 10.0 arm) | `0x004700eb..0x0047010d` | **0** | +0.000000 | 0.00 % |
| **`wcs_drift`** (airborne lateral drift) | `WheelContactSolver.cpp:337` | **47** | **-23.194031** | **100.00 %** |

`vel += normalize(d) * -20.0` (`kDrift = _DAT_005cd61c`), 47 times across 29 frames.

### 1.2 Why it fires, and why its gate is NOT the defect

`PREREG_STEP1C.md` (`cd41ca04`, unrun). The gate is **byte-faithful**, read instruction by
instruction from `MASHED.exe.unpatched`:

```
004701e8  fcomp [0x5cc574]      ; vs 2.0
004701ee  fnstsw ax
004701f0  test ah,0x41          ; C3|C0
004701f3  jp  0x47044b          ; skip on GREATER and on UNORDERED; fall through on EQUAL and LESS
```

so the original's condition is **`0 < count <= 2.0`**, and
`WheelContactSolver.cpp:323`'s `(gc != kZero) && ((gc < kGroundThr) != (gc == kGroundThr))` is
`gc != 0 && (gc < 2 XOR gc == 2)` = **the same predicate**. **There is nothing to fix at the
gate.** The defect is its input:

```
wcs_cnt over d = 222..250, 87 solver calls, (bVar16, states, bVar4, iVar8):
  (4, 1111, 0, 4) x 37   -> drop fires, gate sees 3 > 2, drift SKIPPED
  (3, 0111, 0, 3) x  5   ->            gate sees 3 > 2, drift SKIPPED
  (2, 0011, 0, 2) x 31   ->            gate sees 2 <= 2, drift RUNS
  (2, 0110, 0, 2) x 16   ->            gate sees 2 <= 2, drift RUNS
```

On **47 of 87** calls **two wheels sit at state 0**, so `bVar16 == 2` and the gate opens.
`bVar4 == 0` on **all 87** calls, which **eliminates** the `(kState2Lo < fv) && bVar4` arm of
the demotion at `WheelContactSolver.cpp:170`. Two arms remain: the state-0 -> state-2 latch at
`:163` (`-2 < fv <= 0`) and `piVar9[0x15] == -1` at `:170`. **[U-9179]**

**The original's own number for the same site, already on disk**: the velocity is **bitwise
unchanged across `0x0046f6c0` on 2945 of 2945** samples of the §16.7 arm
(`verify/d2_bounce_20260930/orig_fp3.msd.fixupprobe.csv`, cited at
`WheelContactSolver.cpp:33-46`). None of the three sites fires on the original there. So this
is the confirmation `PREREG_STEP1B` §4 asked for, and it is **behavioural**, not static — no
new original-side probe was needed, and none was taken.

### 1.3 Why no instrument could see it before

`+0x9e0` cannot witness the gate's input. The original's **own** tail recomputes it:

```
0047044b  mov [edi+0x9e0], ebp          ; = 0
00470451  cmp [edi+0x198], ebp / 00470459  mov [edi+0x9e0], 0x3f800000   ; wheel 0 state != 0 -> 1.0
00470463 / 0047046b..00470477            ; wheel 1  -> += 1.0
0047047d / 00470485..00470491            ; wheel 2  -> += 1.0
00470497 / 004704a4..004704b0            ; wheel 3  -> += 1.0
```

i.e. **the count of wheels with `state != 0`** — which is exactly what
`VehiclePhysicsRun.cpp:343-357` `ReassertContacts` computes, **so `ReassertContacts` is not a
port-only construct, it is that tail.** Both sides therefore publish `4.0` while the gate
consumed `2`. Every reducer that reads `gnd` from the record (including `a18_budget.py`'s
`gnd == 4.0 on 29/29 both`) was reading the masked value.

---

### 2 The original's substep loop, transcribed — and an RVA citation corrected

**`0x00469ad4 mov ebx,2` is NOT the substep count.** It sits `0x34` bytes inside
`FUN_00469aa0`'s contact-history shift loop, which `D2_REOPEN_2026-09-29.md` §19.2 already
reads correctly; §20.9 then re-used the same RVA for "the frame contains two substeps". Both
cannot be true of one `mov`. `FUN_004709a0` has no time loop either — its
`0x00470ab0 cmp ebp,2` is the **retry** cap (already ported at `VehiclePhysicsRun.cpp:932`) and
the function is a **16-vehicle loop** (`0x004709f0 .. 0x00470c53 inc eax / cmp eax,0x10 / jl`).

**The substep loop is `0x00471106..0x00471141`, inside the dispatcher `FUN_00470c70`:**

```
00471106  mov  edi,[esp+0x10]     ; rem = the frame chunk, an INTEGER ms count
0047110a  test edi,edi
0047110c  jbe  0x471143           ; rem <= 0 -> no substep at all
00471110  cmp  edi,0x19           ; LOOP TOP, 25
00471113  mov  esi,edi
00471115  jbe  0x47111c
00471117  mov  esi,0x19           ; sub = min(rem, 25)
0047111c  test esi,esi
00471122  mov  [esp+0x20],esi
00471126  fild [esp+0x20]         ; (float)sub   <- INTEGER-sourced, so no residue step
0047112b  jge  0x471133
0047112d  fadd [0x5cc94c]         ; unsigned fixup, sub < 0 only
00471133  push ecx
00471134  fstp [esp]              ; arg0 = (float)sub
00471137  call 0x4709a0           ; THE SUBSTEP
0047113c  add  esp,8
0047113f  sub  edi,esi            ; rem -= sub
00471141  jne  0x471110           ; while (rem != 0)
```

fed by `chunk = min(remaining, 0x32)` at `0x00470f50..0x00470f61`, also integer, with the
per-frame budget pinned to `0x32` = 50 (`VehiclePhysicsRun.cpp:739-746`'s existing decode of
`DAT_007f1000`). **`50 -> 25, 25` is exactly 2, by integer arithmetic.** Per chunk the
dispatcher runs A4 for all 16 vehicles (`0x00471076`), then the post-A4 per-vehicle pass
(`0x004710b0`), then this loop; the outer chunk loop closes at
`0x00471143..0x00471151` (`remaining -= chunk; if (remaining) goto 0x470f50`).

`VehiclePhysicsRun.cpp:119`'s `kMaxSubstep = 25; // 0x19 (FUN_004709a0 inner chunk)`
misattributes the constant's home. The constant is right; its RVAs are `0x00471110` and
`0x00471117`.

This correction does not disturb attempt 18's live `4666/2333 = 2.0000` — that counts hits on
`0x004709a0` and is independent of which `mov` sets the bound.

**The port's own count, now measured: exactly 3, on 1629 of 1629 frames**, chunks
**25.000000 / 25.000000 / 0.0000038147** ms (`frameMs = dt*3000.0f = 50.0000038` against a
float `while (remMs > 0.0f)`). U-9160's "3-or-4" is **3**, flat, on this arm.

**And the residue step is not cheap:** at 3.81e-06 ms — one 6 500 000th of the frame — it still
costs **-7.157898**, **86 %** of what the full 25 ms substep costs. The cut inside
`0x0046f6c0` is **per-CALL, not dt-proportional**.

---

### 3 The carrier, and the fix — NONE AUTHORED, under the attempt's own registered rule

`PREREG_STEP1C.md` §4, registered before the run:

> *"A fix is authored in STEP 2 **only** if the run lands on a transcription defect at a cited
> RVA — a condition, a constant or a field offset in `0x0046f6c0`'s own code that differs from
> the disassembly. Anything else ... is reported named and UNFIXED, STEP 3 runs on the
> unchanged build."*

It did not. The site is faithful, the gate is faithful, and the defect is **which state the
port's wheel state machine leaves two wheels in** — which needs the state machine's own
per-wheel branch inputs on **both** sides (`fv`, `piVar9[0x15]`), neither of which is
instrumented and neither of which this attempt registered. **The rule is honoured, not
reinterpreted** (memory `pre-register-the-decision-not-the-diagnosis`).

**The substep budget is also deliberately NOT ported**, for a measured reason rather than an
omission: it carries **26.03 %** against the carrier's **84.33 %**, so porting
`0x00471106..0x00471141` now would change the default build's integration cadence and move the
launch and recovery statistics while leaving **~58 %** of the sink standing — confounding the
next attempt's measurement of the carrier. The loop is **transcribed in full** (§2) and is one
edit away once U-9179 closes. Registered in `PREREG_STEP23.md` §1 so it is not read as an
oversight.

**No knob, no clamp, no fitted constant, no threshold chosen to close the branch. No C-level
moved and no demotion**: `0x0046f6c0` is **C2/mapped**, which makes no behavioural claim, so a
measured divergence is not a demotion. AI slots 1+ (`VehiclePhysicsRun.cpp:702`) untouched.

**Promotion evidence, levels before -> after: NONE requested, NONE granted.**
`0x0046f6c0` C2 -> C2, `0x00470c70` C2 -> C2, `0x004709a0` C2 -> C2, `0x00470670` C3 -> C3.
`git diff 025e1642..HEAD -- mashedmod/src` touches only `if (armed)` call sites and the new
probe TU; the `.asi` relinked with **all 422 objects up to date** on every build, because all
five instrumented TUs are **exe-only** (checked against `asi_sources.rsp`).

---

### 4 Gates — three FAILED and are reported as failures, one decision rule REPLACED

| gate | asked | result | verdict |
|---|---|---|---|
| **CH** | unarmed run writes no sink file; `motion_diag.log` byte-identical | no sink file in `p_ctrl`; **byte-identical on all 1628 shared lines** | **PASS** |
| **CV** | five once-per-frame tags exactly once, no orphans, substep histogram | 1629 frames, `miss` 0 / `dup` 0 for all five, orphans **0**/**0**, `nsub` `{3: 1629}`, 0 unparsed, 0 rejected | **PASS** |
| **KA1** | `m(w1) == r9e4(w1)` exact float | **1629/1629 = 100.0000 %** | **PASS** |
| **KA2** | producer deltas telescope to `m(snap) - m(w1)` | **100.0000 %**, worst **2.030e-15** | **PASS** |
| **KA3** | `m(snap)` == `motion_diag` `\|vel\|`, 1e-6 rel | **70.3499 %**, worst 4.665e-06 | **FAIL** |
| **EV** | `n >= 25`, gnd4 / ctrl>0 `>= 90 %`, median speed within 25 % of 633.2 | n **29**, **100.00 %** / **100.00 %**, median speed **633.2**, median frame **237** | **PASS** |
| **KB1** | `m(wcs_in) == m(sub_orient)` exact | **97.7011 %** (85/87) | **FAIL** |
| **KB2** | site deltas telescope to `D_wheel` | **97.7011 %** (85/87) | **FAIL** |
| **KB3** | every substep emits `wcs_in` | **0 missing** | **PASS** |
| **WS** | `+0x9e0` has exactly ONE writer on the original | **NINE** | **FAIL** |

**KA3.** Cause measured, not waived: `motion_diag.log` prints `vel=[%g,%g,%g]` — six
significant decimal digits — while the probe prints `%.9g`. Re-rounding the probe's **own**
vector through `%g` scores **99.9386 %** (1628/1629, worst 1.565e-06) against **70.3499 %**
as-is. The bar is tighter than the channel it compares against can represent. **Not
re-thresholded.** It does not touch the reading: KA2 passes at 100.0000 % inside the probe's
own full-precision channel, `motion_diag` is never used to form a producer delta, and the two
independently-sourced `T_post` medians agree to **8.0e-06** relative (**-27.50393** against
**-27.50415**) — with `-27.50393` bit-for-bit attempt 18's figure from a different build and a
different run.

**KB1 / KB2.** The 2 misses of 87 are the **two retry frames**: on a reported contact the
`0x00470ab0` retry re-runs the substep, so `wcs_in` and `sub_orient` each appear twice and the
reducer pairs the **first** `wcs_in` with the **last** `sub_orient`. A pairing error in the
reducer, the same class attempt 18's KA-B hit. **Reported as failures, not re-paired.** They do
not touch §1.1's attribution, which walks the emission sequence in order and is pass-correct,
or `D_wheel`'s own value, which comes from the KA2-clean STEP 1 split.

**WS, and the instrument it named was the wrong one.** The gate asked for exactly one writer of
`+0x9e0`; `re/tools/dispsweep.py` finds **nine** (`0x0046bb8a`, `0x0046ddd1`, `0x0046ddf9`,
`0x004701cb`, `0x0047044b`, `0x00470459`, `0x00470477`, `0x00470491`, `0x004704b0`) — and that
failure is what exposed §1.3's masking tail, so the gate earned its keep. The instrument the
PREREG named, `fold_sweep.py`, matches **folded absolutes only**, so it cannot see
`fst [edi+0x9e0]` and returned *7 reads, 0 writes* — a true statement about folded encodings
and a false one about the field. Its own `+0xbf8` known answer PASSED, so it was working
correctly and simply does not answer this question. **Replacement**, written this session:
`re/tools/dispsweep.py` sweeps register-relative displacements with x87 stores classified by
mnemonic, and passes its own published known answer (`+0x9e4`'s writers `0x00467673` and
`0x004686cc`, both x87, both from attempt 18's §3).

**ONE DECISION RULE REPLACED, and why.** `PREREG_STEP1.md` §3's candidate list enumerated the
substep legs as `D_sub[0]`, `D_sub[1]`, `D_sub[2]` — it indexed **one code site by substep
ordinal**. A rule that ranks *code sites* cannot be evaluated on a list that splits a single
site into three rows: `0x0046f6c0` is one site called three times, and no call count can push
any one row above 50 % once the list divides it. **Rule 1' sums each site over the frame's
substeps before taking the median** — a regrouping of quantities **the same pre-registration
§2.1 already defined** (`D_orient[s]`, `D_wheel[s]`, `D_fixup[s]`), with no new threshold, no
new channel and no new window. As registered, rules 1 and 2 **both failed to fire** and the
tool printed rule 4's *"NO CARRIER DOMINATES -> report the split and STOP"*; that verdict is
reported above as the registered outcome. **Rule 2's non-firing is correct and is a finding**:
the substep budget is not the carrier.

---

### 5 STEP 3 — four verdicts on the unchanged build, 3 runs

Build `d9d9750e` = the default build plus the default-OFF probe. `participants=1` confirmed
from the game's own `MATCH-SEED rule=0 participants=1 teams=0 seed=6 engine=1` line. §16.7 arm
(`MASHED_MEASURE_SOLO=1 MASHED_TRACK_SEL=12 MASHED_STEER_HOLD_AFTER=0`), muted,
`MASHED_WIN_POS=primary-bl`, `MASHED_TITLE` on each, PIDs 41164 / 26984 / 35360 spawned and
reaped by `a8_run_port.py`, 1627 / 1629 / 1628 lines. **No fix was applied, so (a), (c) and (d)
are expected to reproduce attempt 18 — and they do, which is itself the probe-inertness
witness.**

**(a) `T_post` at `d = 222..250`: FAIL.** ORIG **-0.00464** vs PORT **-27.50393**,
`|delta| 27.49929` against bars 5.5560 / 8.6373, n = 29 each, median speed 790.9 / 633.2.
`dS` +27.74798 vs -1.04312; `T_W1` +27.78004 vs +25.07255. Identical to attempt 18 to five
decimals. **Substeps per frame: 3** — the loop was **not** ported, so 3 is the expected,
non-blocking reading (`PREREG_STEP23.md` §2).

**(b) launch: PASS.** `L = 0` at **0.19 %** (L=14 49.03 %, L=15 51.27 %, L=16 53.43 %, over
`d = 16..95`, n = 80); `+0xb14` engages at `d = 15` on both arms; peak **1835.50 at `d` = 95**
against the original's **1832.40 at `d` = 95**. **No regression** — there could not be one,
since nothing behavioural changed, and this is the statement that makes the point.

**(c) recovery, H1 (`>= 50 %` of the 400 post-trough frames `>= 100` AND median `>= 900`):
FAIL on the median leg.** **243/400 = 60.8 %** (fraction passes), median **132.8**, max 737.9,
trough 83.43 at `d` = 106 — against the original's **398/400 = 99.5 %**, median **1333.9**,
max 2477.9, trough 85.45 at `d` = 101.

**(d) the three D2 metrics against the UNCHANGED `d81a8df6` bounds: FAIL 3 of 3**, identical on
all three runs.

| metric | bound | PORT (s1 = s2 = s3) | n | median speed | median `d` | ORIG | ORIG n | ORIG median `d` |
|---|---|---:|---:|---:|---:|---:|---:|---:|
| slip 1500-2000 | 0.18855 .. 0.19635 | **0.1983** | 19 | 1665.05 | **85** | 0.1984 | 312 | 718 |
| slip 2000-2600 | 0.24488 .. 0.25487 | **UNSCORABLE** | **0** | — | — | 0.2517 | 553 | 915 |
| driving-median | 1904.70 .. 1982.44 | **1019.77** | 76 | 1019.77 | **79** | 1944.28 | 1176 | 835 |

§26.10's median-frame guard fires on all three (port medians `d` 79-85 against the original's
718-915), so the magnitudes are not readable as physics errors; the bounds are still scored and
still not met. **D2 does NOT close.**

---

### 6 Coupling found, reported not tuned

- **`ReassertContacts` (`VehiclePhysicsRun.cpp:343-357`) and `0x0046f6c0`'s grounded count are
  two counters of the same thing that disagree**: the solver counts `state == 1`, the tail
  counts `state != 0`. The original has both (`0x004701cb` and `0x0047044b..0x004704b0`), so
  the shape is faithful — but on the port the disagreement is **2 vs 4** and the masked value
  is what every reducer reads.
- **`+0x9e0` is read by the grip-clamp #6 gate** (`0x00468761`, attempt 18's `G4`), by A5
  (which zeroes it at `0x0046ddd1`), and by `a8_slip_axis.py:35`'s `>= 3.5` regime filter. A
  change to the grounded count moves all three.
- **The substep count couples to the contact-fixup cadence and to `BodyOrient_OmegaFromSteer`'s
  accumulator** (`VehiclePhysicsRun.cpp:968-977`, `(w + accum) * damp` per substep), so porting
  the 2-substep loop will move the yaw channel as well as the speed one. Rendering is not
  coupled: the port's render tick is once per `VehiclePhysics_StepCar`, outside the loop.
- **AI slots 1+ are NOT touched.** Nothing this attempt reached `VehiclePhysicsRun.cpp:702`'s
  fitted seed. If the substep loop is ported later it will change slots 1+ too, and that is
  D3's to report.

---

### 7 Collateral

**Leg 1, paired same-side with floors** (A = attempt 18's `s1`, B = this attempt's `s1`,
floor-A attempt 18's `s2`, floor-B this attempt's `s2`, `--speed sp`):

> **0 of 75 paired fields divergent. All 75 within the measured noise floor on every one of
> 1627 aligned frames. A-only 0, B-only 0.**

The loop's write set this attempt is **empty** (no behavioural change), so any divergent row at
all would have been an outside-scope row. **There are none. No outside-scope rows.**

**Leg 2, cross-side banded** (`--mode banded`, floor-A `orig_lb18.msd`, floor-B `s2`, scope
`scope_a6a.txt`, anchor `msd+0x9e0:ge:4.0`, 19 field maps): **all SIX bands are `!!`
OFF-REGIME** (median frame indices 1022-1556 against 66-379), so per §26.10 **not one row was
read and none is reported**. Three fields are exact on both sides in every band — `msd+0x498`
(40000), `msd+0x49c` (4000), `msd+0x9e0` (4). The matched-`d` cross-side review is the budget
itself (`budget_s1.csv`) and §1's split.

---

### 8 The diagnostic is KEPT, with the justification

`MASHED_D2SINK` and its 13 call sites stay in the tree rather than being removed:

- **default-OFF** (one `static const char*` test per site when unset) and **CH-verified inert**
  (0 of 75 fields past the floor over 1627 frames, and byte-identical `motion_diag` output);
- it is the **instrument U-9179's resolution path requires** — the next step extends it with the
  per-wheel state-machine line rather than rebuilding it, which is the lesson of
  `a8_launch.py`/`a8_medframe.py` being written twice;
- it is exe-only and cannot affect the `.asi`.

---

### 9 Commits

`3837cfe5` (PRE-REGISTER STEP 1, unrun), `11d4ccc2` (STEP 1 run), `4bbb5db7` (PRE-REGISTER
STEP 1B, unrun), `cd41ca04` (PRE-REGISTER STEP 1C, unrun), `d9d9750e` (STEP 1B + 1C run +
PRE-REGISTER STEP 2/3, unrun), and the STEP 3 + collateral + trackers + handoff commits.
**Nothing pushed.**

### 10 Still open

**U-9179** — the new blocker: which branch of `WheelContactSolver.cpp:160-175` leaves two
wheels at state 0, and what the original's per-wheel `fv` / `piVar9[0x15]` are at the same `d`.
**U-9178** — re-shaped, carrier named, decomposition no longer missing; what remains under it
is only U-9179. **U-9160** — re-scoped: original side closed (2), port side measured (3), and
it is a 1.5x multiplier with the loop transcribed and ready to port. U-9177 (named, now
decomposed), U-9176 (two RVAs), U-9156, U-9171 (measured not the carrier), §20.14's `-0.1` duty
cycle, D1-residue R1. **D3 modes 3/7 hold stands. D2 does not close.**
