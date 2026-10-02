## RESULT — D2 attempt 20

**U-9179 is RESOLVED, its producer is FIXED, and the sink it carried is gone.** `wcs_drift`
fires **0** times (was 47 in the window), `0x0046f6c0`'s share of `T_post` is **-0.00 %**
(was 84.33 %), `T_post` is **inside both of attempt 18's bars for the first time in the D2
re-open**, recovery **H1 PASSES both legs** (was a FAIL on the median leg), and **2 of the 3
D2 metrics are inside their unchanged `d81a8df6` bounds** — the 2000-2600 slip band being
scored at all for the first time in the re-open. The third, `driving-median`, is **1.3 %
below its lower bound**, so **D2 still does not close on the three-metric gate.** Branch
`race/first-frame-parity`, nothing pushed.

Per-attempt detail: `RESULT_STEP1.md`, `RESULT_STEP2.md`, `RESULT_STEP34.md`.
Pre-registrations, all committed **unrun**: `PREREG_STEP2.md` (`49bf17c9`),
`PREREG_STEP34.md` (`35a4c7b4`), `PREREG_STEP5.md` (`052a678a`).

---

### 1 STEP 1 — the transcription diff: NO DEFECT

`0x0046f6c0`'s per-wheel 3-state machine read instruction by instruction from
`MASHED.exe.unpatched` and diffed against `WheelContactSolver.cpp:140-207`. Listings
committed (`disasm_head_*`, `disasm_statemachine_*`, `disasm_combine_*`,
`disasm_collector_*`).

| original | RVA | port | match |
|---|---|---|---|
| `bVar4 = (cVar13 != 0) && (iVar8 == 0)` | `0x0046f827..0x0046f8f1` | `:141-158` | yes |
| ARM A `(fv > 0.02 && bVar4) \|\| key == -1 -> 0`, else `1` | `0x0046f909..0x0046f930` | `:178-183` | yes |
| ARM B `-2.0 < fv <= 0.0 -> 2`, else stay `0` | `0x0046f935..0x0046f957` | `:167-177` | yes |
| the projection block (state-inert, always latches) | `0x0046f95d..0x0046f9be` | `:170-176` | yes |
| accumulate `[ecx+0x68/0x6c/0x70] * [ecx-4]` | `0x0046f9c0..0x0046fa0f` | `:184-197` | yes |
| combine `bVar16 = #{state == 1}` | `0x0046fa26..0x0046fae3` | `:202-207` | yes |
| the 4-wheel drop, `argmax \|fv\|` + tie-break | `0x0046fae3..0x0046fbbd` | `:218-230` | yes |
| the `bVar16 == 3` promotion | `0x0046fc09..0x0046fd29` | `:241-259` | yes |

Constants **byte-read**, not re-used from the port's own annotations:
`[0x005ce18c] = 0.019999999552965164`, `[0x005ceaa0] = -0.004999999888241291`,
`[0x005cc34c] = -2.0`, **`[0x005d757c] = 0x00000000 = 0.0f`** (the one comparand the port
had named rather than cited), `[0x005cea98] = -0.1` (double), `[0x005cea90] = 0.1` (double),
`[0x005cc574] = 2.0`. The two values the port assumed to be zero are **writes in the
binary**: `xor ebp,ebp` at `0x0046f6d3` and `mov [esp+0x10],ebp` at `0x0046f70e`.

What STEP 1 contributed was not a defect but a **reduction**: `fv` (`+0x194`) and `key`
(`+0x1ec`) have exactly two writers — the init loop's `10.0f` / `-1` reset and the
classifier `0x0046cc40`'s fill, which runs **only on a new contact**
(`CarWorldContacts.cpp:125-130`). So a wheel the classifier does not fill satisfies **both**
of U-9179's remaining arms at once, and the question moves one step upstream.

### 2 STEP 2 — the per-wheel comparison

Matched `d`, `L = 0`, `R` from each capture's own `+0xbf8` (ORIG **897**, PORT **1**).

**Inputs.** Carrier window `d = 222..250`: ORIG `n = 58` solver calls / 29 frames, median
speed **850.26**, median frame 1133; PORT `n = 89` / 29 frames, median speed **652.99**,
median frame 237.

| | ORIG | PORT | &#124;delta&#124; |
|---|---:|---:|---:|
| **`K` = wheels with `key != -1`** | **4.000** | **2.000** | **2.000** |
| `F` = wheels with `-2.0 < fv <= 0.0` | 2.000 | 2.000 | 0.000 |
| `S1` = wheels with `stateOut == 1` | 4.000 | 2.000 | 2.000 |
| arms | `A-hold` 232 (100 %) | `A-hold` 257, **`A-demote-key` 99** | — |
| state words | `(1,1,1,1)` x58 | `(1,1,1,1)` x37, `(0,0,1,1)` x31, `(0,1,1,0)` x16, `(0,1,1,1)` x5 | — |

Launch control window `d = 0..100` (ORIG `n = 203` median speed 624.31; PORT `n = 304`
median speed 624.69): `K` **4 = 4**, `S1` **4 = 4**, arms 100 % `A-hold` on both. The defect
is not a constant property of the port's contact chain.

**Decision rule 1 fired on `K`.** The diverging term is the contact key, the arm is
**`A-demote-key`** (`0x0046f91a`/`0x0046f91f`), and the `:163` latch arm fires **0** times
with `bVar4 == 0` on every call — so U-9179's second candidate arm is **eliminated by
measurement**.

**Coverage and known answers.**

| gate | asked | result | verdict |
|---|---|---|---|
| COUNT-FIRST | `< 400/s` before sampling | **2.012 solver calls/frame = ~123/s**, `otherEdi` 0, `edi == rec` on every self-check row | **PASS** |
| CV ORIG | rows, drops, calls/frame | 4688 rows, 0 dropped, `{2: 2308, 3: 24}` | **PASS** |
| CV PORT | calls, unparsed, substeps | 4664 complete calls, 0 unparsed, `{3: 1522}` | **PASS** |
| **KA-O** | replay reproduces the ORIGINAL's own states, `>= 90 %` | **4686/4687 = 99.9787 %** | **PASS** |
| KA-P (pre-drop) | `state'` == the PORT's measured `wcs_sm`, `>= 99.5 %` | **4663/4663 = 100.0000 %** | **PASS** |
| KA-P (entry-pair) | `>= 99.0 %` | **67.2957 %** | **FAIL** |
| EV | `n >= 25`, port median speed within 25 % of 633.2 | n = 29 each, 652.99 | **PASS** |

**KA-O is the strong one:** the rule transcribed in STEP 1 reproduces the running
original's four wheel states, including the drop and the promotion, on all but one
consecutive pair. STEP 1 is therefore confirmed **behaviourally**, not only statically.

**Upstream, by code site.** All 99 `key == -1` rows carry `fv == 10.0` **exactly** — the
init-loop reset — so the classifier never reached its fill. The new per-wheel stage channel
(`wcs_cls`) says which gate stopped it, and stage 7 agrees with `key != -1` on **356/356**:

| wheel | stage 1 (failed SAT half-plane 3, `:116`) | stage 3 (failed the approach test, `:119`) | stage 7 FILLED |
|---|---:|---:|---:|
| 0 | **52** | 0 | 37 |
| 1 | **31** | 0 | 58 |
| 2 | 0 | 0 | 89 |
| 3 | 0 | **16** | 73 |

### 3 The diverging producer, and its live confirmation

**`ProduceTerrainBatch`'s admission test** (`ContactProducer.cpp:77-81`) was a
**plane-distance** test, and the 256-entry store (`:26`) was **saturated on 3565 of 3565
solver calls**. A plane is unbounded, so on a largely coplanar track it admitted ground
triangles from anywhere on the surface, the loop then stopped scanning, and the triangles
under wheels 0 and 1 were never offered to the classifier.

**The ORIGINAL has no cap.** Its collector `LAB_00468b80` increments `DAT_0088e60c`
unconditionally at `0x00468d6c..0x00468d73` (`mov ecx,[0x88e60c] / inc ecx /
mov [0x88e60c],ecx`) with **no bound test anywhere** in `0x00468b80..0x00468d7c`; the
locality comes entirely from the BSP walk `FUN_00538c80` this function stands in for.
`hooks.csv` row 1276 (U-9156) already recorded the stand-in; attempt 20's contribution is to
**measure that it is U-9179's producer**.

**Live confirmation, three arms on the running port** (`d = 222..250`, 29 frames each):

| arm | admission test | scan | `nEntries` | `nPass` | `K` | `S1` | stages | **`wcs_drift`** |
|---|---|---|---:|---:|---:|---:|---|---:|
| `plane` (legacy) | plane distance | stops at cap | 256 | 379 | 2.00 | 2.00 | `1`:83 `3`:16 `7`:257 | **47** |
| `planefull` | plane distance | full | 256 | 379 | 2.00 | 2.00 | identical | **47** |
| `local` | **AABB + radius** | full | **17** | **17** | **4.00** | **4.00** | **`7`:348 only** | **0** |

Stated separately: `planefull` shows the **cap alone is not demonstrated to be sufficient**
— it scans everything, still stores 256, and is identical to legacy. What fixes it is the
**admission test**, and `local` lands on the original's own numbers at matched `d`.

### 4 The fix, and the promotion evidence

`ContactProducer.cpp:67` — the admission test is now **spatial** (the triangle's AABB grown
by the query `radius` must contain the query centre) and is the **default**;
`MASHED_D2_BATCHMODE=plane` is kept as the diagnostic pre-fix arm only. **No new numeric
constant** (same `center`, same `radius` = the record's own `+0x4a4`), no knob on the
shipping path, no clamp, no fitted value, no threshold chosen to close a branch. The
justification was registered before the change landed and is geometric: a plane is
unbounded, so a plane-distance test is not a locality test, and the AABB form is a
conservative superset of sphere-vs-triangle, so it cannot drop a triangle the narrow phase
would have used.

**PROMOTION, levels before -> after: NONE requested, NONE granted.**
`0x0046f6c0` **C2 -> C2**, `0x0046cc40` **C2 -> C2**, `0x00468d80` **C2 -> C2**,
`0x00538c80` **C1 -> C1**, `0x00470c70` **C2 -> C2**, `0x004709a0` **C2 -> C2**.
`ProduceTerrainBatch` is **port-only scaffolding with no RVA of its own**, so `run_diff`
path1 and `run_verify_hook` path2 have nothing to install and nothing to call — there is no
export, and manufacturing a diff here would have been a false claim. The evidence offered is
behavioural and cross-side: the ORIGINAL's own per-wheel state at matched `d` reproduced by
the port, plus KA-O at 99.9787 %. Guards: `rva-lint` **`allowlisted=122 NEW=0`** on every
build; `[asi] all 422 objects up to date`; all six edited TUs are **exe-only**.

**The A/B is a verified control, not a remembered baseline.** The `plane` arm reproduces
attempt 19 **bit-for-bit** on both `pre1` and `pre2`: slip 1500-2000 **0.1983** (n = 19),
slip 2000-2600 **UNSCORABLE** (n = 0), driving-median **1019.77** (n = 76), median `d`
85 / — / 79.

### 5 STEP 5 — the integer substep loop, separately pre-registered, KEPT by its own rule

`VehiclePhysicsRun.cpp:918-922`/`:1076` is now integer, transcribed verbatim:
`chunk = min(remaining, 0x32)` (`0x00470f50..0x00470f61`),
`sub = min(rem, 0x19)` (`0x00471110`/`0x00471117`), `fild` (`0x00471126`),
`rem -= sub` / `jne` (`0x0047113f`/`0x00471141`).

| gate | asked | result | verdict |
|---|---|---|---|
| **G5-1** | `nsub == {2: N}`, chunks `25.000000 / 25.000000`, no residue step | **`{2: 1626}`**, chunks 25.000000 / 25.000000, residue gone | **PASS** |
| **G5-2** | `a8_launch` `L = 0` at `<= 1.00 %` | **0.19 %**, unchanged | **PASS** |
| **G5-3** | drift 0, `T_post` in bars, recovery H1, both slip metrics in bounds | drift **0**, `T_post` **-0.61485**, H1 398/400 + median 1362.7, slip **0.1943** / **0.2524** both in bounds | **PASS** |

All three passed, so the step was **KEPT** by its registered decision rule. The ORIGINAL's
own count, measured this attempt and not inherited: **2.012 solver calls/frame**
(4672 over 2321 A6a frames, histogram `{2: 2308, 3: 24}`).
**STATED RESIDUAL, not ported:** the outer chunk loop `0x00471143..0x00471151`; at
`dt = 1/60` `remI == 50` so it iterates once and cannot be distinguished, **[UNCERTAIN]**
for any `dt` making `remI > 50`.

### 6 STEP 4 verdicts, with numbers

**(a) `wcs_drift` and `T_post` at `d = 222..250` — PASS.** `participants=1` confirmed from
the game's own `MATCH-SEED rule=0 participants=1 teams=0 seed=6 engine=1`. n = 29, median
speed 737.3, median frame 237, `L = 0`. `a19_split` CV PASS, KA1 100.0000 %, KA2 100.0000 %,
EV PASS; **KA3 52.7060 % FAIL** — the same `%g` six-significant-digit channel limit attempt
19 measured, **not re-thresholded**, and it forms no producer delta.

```
wcs_drift        0 firings in the window AND 0 in the whole capture   (was 47 in the window)
0x0046f6c0       median +0.000000   share -0.00 %                     (was -23.194031 / 84.33 %)
grip-clamp #6    median -0.612549   share  99.64 %                    (the whole remainder)
T_post   ORIG -0.00464   PORT -0.61485   |delta| 0.61021   bars 5.5560 / 8.6373   INSIDE BOTH
```

**(b) launch — PASS.** `L = 0` at **0.19 %**; peak **1835.50 at `d` = 95** against the
original's **1832.40 at `d` = 95**; `+0xb14` engages at `d` = 15 on both arms. Unchanged
from attempt 19, so **no regression**.

**(c) recovery under H1 — PASS, both legs, having FAILED before.**

| | fraction `>= 100` | median | max |
|---|---:|---:|---:|
| ORIGINAL | 398/400 = **99.5 %** | **1333.9** | 2478.3 |
| PORT pre-fix | 243/400 = 60.8 % | **132.8** | 737.9 |
| **PORT post-fix** | **398/400 = 99.5 %** | **1362.7** | 2374.0 |

**(d) the three D2 metrics, 3 runs, against the UNCHANGED `d81a8df6` bounds — 2 of 3 PASS.**

| metric | bound | PORT (t1/t2/t3) | n | median `d` | PORT pre-fix | ORIG | ORIG n | ORIG median `d` | verdict |
|---|---|---:|---:|---:|---:|---:|---:|---:|---|
| slip 1500-2000 | 0.18855 .. 0.19635 | **0.1943** (all 3) | 353 | **605** | 0.1983 (n=19) | 0.1937 | 314 | 720 | **PASS** |
| slip 2000-2600 | 0.24488 .. 0.25487 | **0.2524** (all 3) | 557-562 | **718** | UNSCORABLE (n=0) | 0.2498 | 540 | 909 | **PASS** |
| driving-median | 1904.70 .. 1982.44 | **1852.66 / 1861.43 / 1854.65** | 1367/1340/1364 | **637** | 1019.77 (n=76) | 1937.89 | 1154 | 824 | **FAIL** by ~1.3 % |

§26.10's median-frame guard, applied **before** any magnitude was read: the port's scored
populations moved from median `d` **79-85** to **605 / 718 / 637** against the original's
**720 / 909 / 824** — the same ordering and regime, within 15-25 %. The guard no longer
disqualifies the readings. Populations stopped being degenerate: `n` 19 -> 353,
**0 -> 557-562**, 76 -> ~1360. `av.y` cross-check under the same filter: band 1500-2000
ORIG +1.142 / PORT +1.131; band 2000-2600 ORIG +1.466 / PORT +1.474.

**Reported, not acted on:** the faithful substep count moved slip **toward** the original
(0.1914 -> 0.1943 against 0.1937) and driving-median **away** by ~24 units (1879.59 ->
~1856). STEP 5's registered rule excluded the driving-median as a gate precisely so that one
metric improving could not justify keeping a change, and that rule was honoured in both
directions.

### 7 Collateral, and the outside-scope rows

**Leg 1, paired same-side with floors** (A `pre1`, B `s1`, floor-A `pre2`, floor-B `s2`,
`--speed sp`): 76 fields, 1626 aligned frames, **A-only 0, B-only 0**, both same-arm floors
**0 on every field** (the port is deterministic on this arm). **53 of 75 paired fields
diverge, first past the floor at frame 239 (`d` = 238)** — inside the carrier window and not
earlier, which is the independent confirmation that the launch phase is untouched.

**The honest statement: this leg cannot produce an outside-scope row by construction.** The
fix changes which triangles the classifier sees, so its transitive write set is the whole
vehicle-dynamics state, and `motion_diag.log` contains **only** vehicle-dynamics fields.
Calling those 53 "outside-scope" would be an artefact of the channel, not a finding.

**Leg 1b, STEP 3 vs STEP 3 + 5** (A `s1`, B `t1`, floors `s2` / `t2`): 53 of 75 diverge,
first at **frame 4** — immediate, as an integration-cadence change must be — and small
(`vel[0]` med|A-B| 2.67 on a median of -37, `av[1]` 0.00201 on 1.085). Same in-scope
argument applies.

**Leg 2, cross-side banded at matched `d`** (A `orig_solo3.msd`, B `s1`, floor-A
`orig_solo4.msd`, floor-B `s2`, anchor `msd+0x9e0:ge:4.0`, 18 field maps): the anchor is
satisfied on **both** arms and **1625 frames align** — attempt 19 could not read a single
band. **3 of 7 bands are now ON-regime** (`500-1000` medFrm 1514/796, `1000-1500` 1530/822,
`2000-2600` 1794/940); `100-150`, `150-260`, `260-500` and `1500-2000` are `!!` OFF-REGIME
and **no row in them is read**, per §26.10. Floors are 0 here too, so `gap/floor` prints
`0-floor` on every row and the ranking carries no information; the band **medians** are what
is read. Reported, not re-thresholded.

**OUTSIDE-SCOPE ROW — one, inherited and not introduced: `msd+0x4a4`.** It reads **0.67804**
on the original in every band and **692.302** on the port, a factor of ~1021, and it is
**identical in the pre-fix and post-fix arms** (`susp=692.302` on line 1 of both `pre1` and
`s1`). It matters more now than before, because it is the field this fix's own `radius`
comes from. Filed as **U-9181**.

**AI slots 1+ — mechanically in the blast radius, magnitude NOT measured.**
`ProduceTerrainBatch` is called from `SolveWheelContacts` (`VehiclePhysicsRun.cpp:1007`) for
**whichever car is being stepped**, so the fix changes the contact batch for AI cars too.
Every run here was `MASHED_MEASURE_SOLO=1` with `participants=1` confirmed, so **no AI car
existed** and the magnitude is **[UNCERTAIN]**. Nothing touched
`VehiclePhysicsRun.cpp:702`'s fitted seed and **nothing was tuned**. This is D3's to measure.

### 8 Gates and rules — what failed, and the one correction to a previous claim

**Three gates FAILED and are reported as failures, none re-thresholded.**
**KA-P entry-pair 67.2957 %** — cause measured: on the PORT, `ReassertContacts`
(`VehiclePhysicsRun.cpp:347`) **promotes every state-0 wheel back to 1** between solver
calls, so the state at the next entry is not the state the solver left; the ORIGINAL's
counterpart tail `0x0047044b..0x004704b0` only **counts** `state != 0`, which is why KA-O
passes with the same replay. **KA3 52.7060 %** — the `%g` six-digit channel limit, already
diagnosed in attempt 19. **WS**, inherited from attempt 19, unchanged.

**No decision rule was replaced this attempt.** Every rule in `PREREG_STEP2.md` §3,
`PREREG_STEP34.md` §3 and `PREREG_STEP5.md` §2 was evaluated as written and fired as
written, including STEP 5's exclusion of the driving-median from its own gate.

**One rule was ADDED mid-attempt and is named here because it changed how a failure was
read:** a port run that produces no `motion_diag.log` is **retried up to 3 times** before
being called a failure. Two of this session's port boots failed transiently with the car at
`speed=0.00` and an early demo exit, and the **identical control** — the same binary with
the knob unset — failed once and then succeeded. Without the retry rule the `planefull` and
`local` arms would have been recorded as crashes caused by the knob, which they were not
(memory `shadow-lane-failure-windows`). It was registered in `PREREG_STEP34.md` §3 before
the STEP 4 runs.

**CORRECTION to attempt 19 `RESULT.md` §1.3**, which recorded `ReassertContacts` as *being*
the original's tail `0x0047044b..0x004704b0`. Measured: only the **counting** half is
faithful; the promotion at `:347` and the normal writes at `:350-352` are **port-only**.

### 9 Trackers

Through `re-classify` only. **U-9179 RESOLVED** (arm named, producer found, fix landed),
**U-9178 RESOLVED** (`T_post` inside both bars; the phase-bias finding stands and is not
withdrawn), **U-9160 RESOLVED** (loop ported, `nsub {2: 1626}`, outer-chunk residual
carried). New: **U-9180** (which term carries the remaining 1.3 % of `driving-median` —
grip-clamp #6's -0.612549 is the only non-zero `T_post` producer left and is too small to be
the whole gap, so the carrier is **not** assumed) and **U-9181** (`+0x4a4`).
`hooks.csv` row 1276's `ProduceTerrainBatch` note corrected. `CHANGELOG.md` prepended
immediately below the `<!-- ENTRIES -->` line; 1034 -> 1035 lines, nothing rewritten.
**No C-level moved.**

### 10 Commits

`49bf17c9` (STEP 1 + PRE-REGISTER STEP 2, step 2 unrun), `35a4c7b4` (STEP 2 run +
PRE-REGISTER STEP 3/4, unrun), `af9e299a` (STEP 3 fix landed + STEP 4),
`052a678a` (PRE-REGISTER STEP 5, unrun), and the STEP 5 + trackers + handoff commit.
**Nothing pushed.**

### 11 Still open

- **U-9180** — `driving-median` 1.3 % below bound. **D2's only remaining metric failure.**
- **U-9181** — `+0x4a4` reads 0.67804 (ORIG) vs 692.302 (PORT) and now drives the new
  admission test.
- **The outer chunk loop** `0x00471143..0x00471151`, not ported, indistinguishable at
  `dt = 1/60`.
- U-9177, U-9176, U-9156, U-9171, §20.14's `-0.1` duty cycle, D1-residue R1.
- **AI slots 1+**: in the fix's blast radius, magnitude unmeasured, D3's to report.
- **D3 modes 3/7 hold stands. D2 does not close.**
