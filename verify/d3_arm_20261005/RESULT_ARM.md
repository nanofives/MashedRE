# RESULT — U-9191: the two sides take DIFFERENT omega arms, and generated-vs-accumulated is still OPEN

**RAN 2026-10-05.** Pre-registration `PREREG_ARM.md`, committed unrun at `5f3d2bdd` and amended
unrun at `87531150` (G-PORTARM registered after G-ARM failed and **before** the port capture).
Anchor verified before arming: `MASHED.exe.unpatched` `BDCAE093…3C0E`, `launch.exe` `0150…8DA2`,
both equal to `CLAUDE.md`'s.

Tool output: `A1.armrate.txt` / `A1.armrate.json`. New tool `re/tools/ai_armrate.py`.
`re/tools/ai_yawrate.py` and `ai_posmatch.py` are **unmodified**; every shared primitive is imported
from them so the tools cannot drift.

**No C-level moved. No band moved. No D2 WATCH trigger touched.** One source edit under
`mashedmod/src/` (four appended dump columns, proven inert) and two comment-only corrections.

---

## Headline

**The port and the original take different omega arms, and the port cannot be fixed at the call
site.** `FUN_0046e9e0` forks on record `+0x10`: the original takes the **non-zero (torque-seed) arm
on 2558 of 3623 frames = 70.60 %**, while `VehiclePhysicsRun.cpp:1001` calls
`BodyOrient_OmegaFromSteer` **unconditionally**, i.e. the port takes the zero arm on **100 %** of
substeps. `BodyOrient_OmegaFromAngVel` — the function that implements the arm the original takes
most of the time — has **zero call sites in the whole tree**.

**And the port's record never carries the gate value**: `rec_10` is `0` on **220 of 220** windowed
rows, and **no file under `mashedmod/src/` writes vehicle record `+0x10`** (the only two sites are
reads). So honouring the gate at the caller would still select the steer arm every substep. The
missing port is the **`+0x10` producer**, not a one-line change.

**Generated-vs-accumulated is NOT answered, and the reason is now measured rather than assumed.**
The registered combined verdict is **INCONCLUSIVE**, and the post-hoc distance control shows why:
every pairing-free comparison available is confounded by the port covering **1.3258x** the distance
in the same 220 frames, and the one pairing-based gate is **VOID** on a join floor **3.5x too coarse**
for its threshold.

---

## 1. STEP 1 — G-ARM: **FAIL**, and the failure is the finding

Offline on the committed `verify/d3_elim_20261003/o_t1.msd` (car 1, an AI car; `rec_size 0xd04`,
`base_va 0x008822a4`), field `+0x10`, every parsed frame. `N = 3623`, matching RESULT_LEG3's own
count for the same file and parse.

| `+0x10` raw | `i32` | `f32` | count | share of `N = 3623` |
|---|---|---|---|---|
| `0x00000001` | 1 | 1.4013e-45 | **2558** | **70.6045 %** |
| `0x00000000` | 0 | 0.0 | 1065 | 29.3955 % |

- Registered PASS clause: `+0x10 == 0` on **>= 3442 of 3623**. Measured **1065 of 3623**. **FAIL.**
- Registered FAIL clause: non-zero on **> 181 of 3623**. Measured **2558 of 3623**. FAIL by 14x.
- `+0x10` is **not constant** over the capture, so this is a live per-frame fork, not a fixed mode.
- **`+0x10` is given no semantic name here.** No plate assigns it one; the two ports of the same
  gate (`Integrate2.cpp:527`, `PhysicsChainHooks.cpp:2139`) comment it only as "state +0x10". It is
  used exactly as the original branches on it: zero vs non-zero.

**The port side is static, so it needed no run** — the registered claim was checked by grep:
`VehiclePhysicsRun.cpp:1001-1003` calls `BodyOrient_OmegaFromSteer` / `BodyOrient_IntegrateStep` /
`BodyOrient_Heading` unconditionally inside the substep loop, and `BodyOrient_OmegaFromAngVel` has
no call site anywhere under `mashedmod/src/`. Both held.

**Per the registered FAIL clause, G-ACC was NOT run.** Comparing the `+0x144`/`+0x148`/`+0x14c`
accumulator across sides that provably take different arms is the torque-vs-rate error of
2026-10-05 in another costume. The amendment states explicitly that G-PORTARM does not revive it in
either branch, and it did not.

**This also corrects a claim that stood in our own source.** `BodyOrientationIntegrate.cpp`'s header
said *"our own A6_DIAG measured record +0x10 == 0 on every sample, so the wheel-force arm may well be
the live one"*. On the original, for an AI car, that is wrong on **70.60 %** of frames. The same
header's *"DELIBERATELY NOT WIRED YET. Nothing calls these functions"* was **stale** — the functions
have been live since the orientation law landed. Both corrected, comment-only.

## 2. STEP 2 — G-INERT: **PASS**

Four columns appended to the default-OFF `AiStepDump` (`rec_144`, `rec_148`, `rec_14c` as f32;
`rec_10` as i32), all raw record reads, nothing derived, nothing written.

- Header prefix preserved: **42 pre-existing columns unmoved**, exactly four appended (46 total), so
  `ai_posmatch.py`, `ai_headattrib.py` and `ai_yawrate.py` are unaffected.
- **0 mismatching cells of 323022** = 42 pre-existing columns x **7691** rows keyed `(frame, seq, v)`
  present in both `A1.csv` and the committed `verify/d3_leg3_20261005/L1.csv`. Adding read-only
  record reads changed nothing.

## 3. STEP 2b — G-PORTARM: **MISMATCH-UPSTREAM**

The port's `rec_10` over car 1's `ai_posmatch.window` rows: `0x00000000` on **220 of 220** rows
(100.0000 %), against the registered MISMATCH-UPSTREAM clause of zero on **>= 209 of 220**.

So the port's record does **not** carry the gate's input. Supporting static fact, grepped before the
gate was registered: **no file under `mashedmod/src/` writes vehicle record `+0x10`** — the only two
sites are the reads at `Integrate2.cpp:527` and `PhysicsChainHooks.cpp:2139`. The remedy is therefore
**not** local to `VehiclePhysicsRun.cpp:1001`: wiring the gate against an always-zero field would
reproduce exactly the behaviour the port already has.

## 4. STEP 3 — G-ONSET: **VOID**, on a floor I measured rather than inherited

Matched pairs **105**, pair distance median **0.02929**, max **0.11961**, all inside `R = 0.12` — the
same pairing population leg 3 scored, reproduced to five digits.

**The join's induced heading error is 0.087865 deg (median over the 105 matched pairs)**, against the
registered VOID threshold of **0.025 deg** (which is 10x below **0.25 deg**, the smallest number any
G-ONSET clause compares against). **0.0879 > 0.025, so G-ONSET is VOID and reports no verdict.**
Restricted to the **103 of 105** pairs where the heading actually moves it is **0.088920 deg** — so
this is not a construction zero being dressed up, it is a genuinely coarse join.

**Two consequences, stated separately because they differ in size.**

- **This does NOT touch leg 3.** G-DIRECT's **0.9866 deg** clears this floor at **11.2x**
  (0.9866 / 0.0879), above the 10x line. U-9191's magnitude stands unchanged.
- **It does contradict a figure in `PREREG_LEG3.md` §5c**, which states that one frame of slip costs
  *"a median 0.0000 deg of heading"*. My construction of the nominally same quantity, over the same
  105 matched pairs, gives **0.087865 deg** — which is within 2.5 % of RESULT_LEG3's separately
  reported **p90 of 0.0902 deg**. I am **not** adjudicating which construction §5c used; recorded as
  an unresolved discrepancy between two constructions, with the consequence that **any future gate on
  this join needs a threshold above roughly 0.88 deg** (10x the measured median), which rules out the
  sub-degree tercile test G-ONSET was built on.

## 5. STEP 3 — G-SPAN: registered verdict ACCUMULATED-consistent, but it does not survive the distance control

Pairing-free: each side's total swept heading over its own window, as the sum of per-step `wrap180`
increments. Both sides are **exactly 1 AI call per frame over 220 frames** (port frames 0..219,
original 845..1064), so the two windows are **frame-count matched** — checked, not assumed.

| side | rows | steps | `dH` | max per-step |
|---|---|---|---|---|
| original | 220 msd frames | 219 | **101.7307 deg** | 3.8340 |
| port | 220 | 218 | **159.3576 deg** | 2.5564 |

`|dH_port - dH_orig| = 57.6269 deg` against the registered ACCUMULATED-consistent clause of
**>= 0.50 deg**, i.e. 115x it. The alias VOID check passed with room: 3.8340 and 2.5564 against the
90.0 deg bound. **Registered verdict: ACCUMULATED-consistent.**

**And it should not be read as a physics finding.** The post-hoc control below is declared
unregistered precisely because G-SPAN's clause compares two totals over the same **frame count** and
says nothing about the two sides covering the same **distance** — and the port is independently
documented as running 15-31 % over the original's speed.

## 6. COLLATERAL — post-hoc, unregistered, descriptive. Run after the verdicts.

- **The port covers 1.3258x the arclength** in the same 220 frames: **31.2075** against **23.5394**.
  So most of G-SPAN's 1.5665x sweep excess is distance, not turning law.
- Normalising, `dH` per unit arclength is **5.106387** (port) against **4.321727** (original), ratio
  **1.1816**. **This correction is itself invalid**, and I am reporting it only to say so: the two
  totals are taken over **different stretches of road**, because the port travels 32.6 % further.
- **Tercile-wise comparison is confounded the same way** and the numbers show it plainly. The
  original's turning is concentrated in its last tercile (`dH` 99.8219 over 5.435 of arclength); the
  port reaches that corner a tercile earlier (T2 `dH` 44.5016, T3 115.1776 over 11.933). The
  per-tercile ratios swing **2.09 / 12.77 / 0.53** — a textbook instance of
  `speed-bands-compare-different-moments`, and exactly why no tercile statistic is reported as a
  finding.
- **Over the common spatial span `L = 23.5394`** the cumulative sweep difference is **non-monotonic**:
  `-0.66 / +0.56 / +7.38 / +38.70 / -9.80` deg at 20/40/60/80/100 % of `L`. At matched distance
  travelled the port has swept **9.7987 deg LESS** than the original, the opposite sign to G-SPAN's
  frame-matched excess. Two different controls give two different signs, so neither licenses a verdict.
- **The exactly-90.0000 deg start-heading difference is an artefact and is NOT filed.** The port's
  first windowed forward row is `(0.000000, 0.000000)` exactly — `atan2(0, 0) = 0` — while the
  original's is `(-0.000000, -1.000000)` = -90.0000 deg. From the port's **second** windowed row it
  reads **-90.2997 deg**, agreeing with the original to **0.30 deg**. This is the `atan2(0,0)` trap
  U-9188's third self-correction already records, and `ai_armrate.sweep` guards it, so no reported
  number is affected.

**Known limit on every heading number above, carried forward from `D3_B_OFFLINE_2026-10-02.md` §3.5:**
on the port, `rec+0x9d4`/`+0x9dc` is written as `cos(io.yaw)`/`sin(io.yaw)`
(`VehiclePhysicsRun.cpp:1004-1005`) where `io.yaw = BodyOrient_Heading(basis)`, i.e. a scalar
round-trip of the integrated basis; on the original it is the basis row itself. For a **heading**
comparison the two are equivalent, which is why the comparison is made, but the port's field is a
reconstruction and not an independent witness of its own basis.

## 7. Verdicts, collected

| gate | threshold vs measured, denominators spelled out | verdict |
|---|---|---|
| G-ARM | PASS needs `+0x10 == 0` on >= 3442 of 3623; measured **1065 of 3623**. FAIL needs non-zero > 181 of 3623; measured **2558 of 3623** | **FAIL** |
| G-INERT | 0 mismatching cells required; measured **0 of 323022** (42 cols x 7691 common rows) | **PASS** |
| G-PORTARM | MISMATCH-UPSTREAM needs `rec_10 == 0` on >= 209 of 220; measured **220 of 220** | **MISMATCH-UPSTREAM** |
| G-ACC | not run per G-ARM's registered FAIL clause | **NOT RUN** |
| G-ONSET | VOID above 0.025 deg; join median induced error **0.087865 deg** over 105 pairs | **VOID** |
| G-SPAN | ACCUMULATED-consistent needs >= 0.50 deg; measured **57.6269 deg** (219 vs 218 steps, 220 frames each side) | **ACCUMULATED-consistent** |
| **COMBINED** (rule fixed before running) | G-ONSET VOID + G-SPAN ACCUMULATED-consistent | **INCONCLUSIVE** |

**The registered combined verdict is honoured as written.** G-SPAN's clause passed, but the
pre-registration made the verdict a conjunction of G-ONSET and G-SPAN, and the post-hoc distance
control independently shows G-SPAN's number is not clean. Both point the same way: **INCONCLUSIVE.**

## 8. Where this leaves U-9191 and criterion (b)

- **U-9191's magnitude is unchanged and unchallenged.** 0.9866 deg at matched position, clearing the
  newly-measured join floor at 11.2x.
- **Its open question stays OPEN**, with a far better-specified blocker than "needs a Frida hook".
  Settling generated-vs-accumulated now requires **one of two** things, each needing its own
  pre-registration:
  1. **A frame-marked join** (memory `next-sample-pairing-needs-a-frame-marker`) so a sub-degree
     threshold is admissible. The position join's median induced error of 0.0879 deg caps any
     admissible claim at ~0.88 deg, which is above the quantity the tercile test needs to resolve.
  2. **Closing the 1.3258x distance over-run first**, so that frame-matched and distance-matched
     comparisons stop giving opposite signs. That is the speed defect already tracked under U-9185 /
     `verify/d3_noboost_20261003`, not a new item.
- **A NEW and separately actionable defect is filed instead:** the omega-arm divergence
  (§1, §3). It is structural, measured on both sides, and has a named missing input — the `+0x10`
  producer. It is a **candidate** carrier of the heading divergence and is **not** shown to be its
  cause: the arm duty-cycle difference is 100 % vs 29.40 % = **3.40x**, while the observed sweep
  excess is **1.5665x** and reverses sign under the distance control, so the arm alone **over-predicts**
  and other differences partly offset it. Stated as a candidate, not a mechanism.
- **This does not close criterion (b).** The 2026-10-02 counterfactual matrix had **no arm passing (b)
  on any car**, no band moved here, and nothing in this leg changes a scorer.
- **No C-level moves.** `0x0046e9e0` stays C2 `mapped`. Nothing here is behavioural evidence at an
  RVA: G-ARM is a static read plus an offline capture read, and no hook was installed or diffed.
- **Cars 2 and 3 remain out of scope.** The radius was not loosened and the window was not lengthened.

## 9. Owed, discharged in this session

- `BodyOrientationIntegrate.cpp:63` — `kAngVel`'s "angular velocity triple" corrected to **torque**;
  identifier spelling kept so no call site moves. **Comment-only.**
- `BodyOrientationIntegrate.cpp` header — the stale "DELIBERATELY NOT WIRED YET / nothing calls these
  functions" replaced with the measured arm fork and the reason the caller is **not** being changed.
  **Comment-only.**
- **U-9192 item 2 — `D3_B_OFFLINE_2026-10-02.md` §3.5 re-read, and it needs no correction.** §3.5
  derives the heading share openly (*"`head = angOf(target−own) − err` is recoverable"*), so it
  discloses the inversion U-9192 identifies rather than presenting it as an independent measurement.
  U-9192 stands as a methodological correction to how that share was **cited** downstream, not as a
  factual error in §3.5. §3.5's own `[UNCERTAIN]` — whether the residual is a physics defect, a
  scalar-yaw artefact, or a start-boost consequence — is still open and is the same question as
  U-9191's.
