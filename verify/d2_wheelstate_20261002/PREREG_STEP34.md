# D2 attempt 20 — PRE-REGISTRATION, STEP 3 (the fix) and STEP 4 (re-measure)

Committed **UNRUN**, before the fix is made the default and before any STEP 4 capture.
STEP 2 is already run and reported in `RESULT_STEP2.md`; this file registers only what
follows from it.

## 1 What STEP 2 established (not re-derived here)

`RESULT_STEP2.md`, with `n`, median speed and `d` (`L = 0`, `R` per capture) on every row:

- **The arm is named:** `A-demote-key`, the `piVar9[0x15] == -1` demotion at
  `0x0046f91a`/`0x0046f91f` (`WheelContactSolver.cpp:178-179`), **99 of 356** wheel-rows at
  `d = 222..250`. The `:163` latch arm fires **0** times, and `bVar4` is 0 on every call.
- **The diverging input is `key` (`+0x1ec`)**: ORIG `K = 4`, PORT `K = 2`,
  `|delta| = 2.000` whole wheels. Decision rule 1 fired on `K`.
- **The key is missing because the classifier never filled the wheel:** all 99 rows carry
  `fv == 10.0` exactly, the init loop's reset.
- **The classifier's rejecting gate, per wheel:** SAT half-plane 3 (`CarWorldContacts.cpp`,
  `:116`) on **83** rows (wheels 0 and 1), the approach-speed test (`:119`) on **16**
  (wheel 3). Stage 7 (FILLED) agrees with `key != -1` on **356/356**.
- **The first diverging PRODUCER:** `ProduceTerrainBatch`'s admission test
  (`ContactProducer.cpp:77-81`) is a **plane-distance** test, and the batch store is
  **saturated at its 256-entry cap on 3565 of 3565 calls**. A plane is unbounded, so
  distant coplanar track triangles are admitted ahead of the ones under the car and the
  local triangles are never offered to the classifier. The **ORIGINAL has no cap**: its
  collector `LAB_00468b80` increments `DAT_0088e60c` unconditionally at
  `0x00468d6c..0x00468d73` with no bound test anywhere in `0x00468b80..0x00468d7c`; the
  locality comes from the BSP walk `FUN_00538c80` that this function stands in for.
  `hooks.csv` row 1276 already records this stand-in under U-9156.
- **Three-arm A/B, already run** (`MASHED_D2_BATCHMODE`, default-OFF at the time):

  | arm | nEntries | nPass | K | S1 | `wcs_drift` in window |
  |---|---:|---:|---:|---:|---:|
  | `plane` (legacy) | 256 | 379 | 2.00 | 2.00 | **47** |
  | `planefull` (plane, full scan) | 256 | 379 | 2.00 | 2.00 | **47** |
  | `local` (spatial, full scan) | **17** | 17 | **4.00** | **4.00** | **0** |

## 2 STEP 3 — the fix, and exactly what is and is not claimed

**The change:** in `ProduceTerrainBatch` (`ContactProducer.cpp:67`) the admission test
becomes **spatial** — the triangle's AABB grown by the query `radius` must contain the query
centre — and that becomes the **default**, with the old plane test retained only as the
explicit diagnostic arm `MASHED_D2_BATCHMODE=plane` so the paired collateral has a pre-fix
side.

**Why this is a correctness fix and not a fitted one, registered before it lands:**
`ProduceTerrainBatch` stands in for `FUN_00538c80`, an RW broadphase whose output is the
collision triangles **near the query sphere**. A plane-distance test is not a locality test
at all, because a plane is unbounded; it was wrong by construction. The AABB-vs-sphere test
is the standard broadphase admission test and a **conservative superset** of the exact
sphere-vs-triangle test, so it cannot drop a triangle the narrow phase would have used.
**It introduces no new numeric constant** — it reuses the same `center` and the same
`radius` (the record's own `+0x4a4`) the plane test already used. No knob, no clamp, no
threshold chosen to close a branch, and no physics constant is touched.

**PROMOTION LEG — stated plainly, and it is NOT a C-level promotion.**
`ProduceTerrainBatch` is **port-only scaffolding**: it has no RVA of its own, and the
function it stands in for (`FUN_00538c80`, C1/mapped, `ContactStubs.cpp` no-op) is not being
claimed as ported. Therefore:
- **No C-level moves and none is requested.** `0x0046f6c0` C2 -> C2, `0x0046cc40` C2 -> C2,
  `0x00468d80` C2 -> C2, `0x00538c80` C1 -> C1.
- `run_diff` path1 / `run_verify_hook` path2 are **not applicable**: there is no RVA body to
  install and no export to call. This is `re/CONFIDENCE.md`'s stand-in case, not its
  non-repeatable-function clause, and the attempt must say so rather than manufacture a
  diff. The evidence offered is **behavioural and cross-side**: the ORIGINAL's own measured
  per-wheel state at matched `d` (`K = 4`, `S1 = 4`, state word `(1,1,1,1)` on 58/58 calls,
  arms 100 % `A-hold`), reproduced by the port after the fix, plus the KA-O gate at
  99.9787 % which confirms the transcribed rule against the running original.
- `dual_copy` guard must report `NEW = 0`.
- The `.asi` must be unaffected: all four edited TUs
  (`ContactProducer.cpp`, `ContactStubs.cpp`, `CarWorldContacts.cpp`,
  `WheelContactSolver.cpp`) are **exe-only** (`exe_sources.rsp` 23/24/25/28, absent from
  `asi_sources.rsp`). `ContactDeps.h` is shared, so asi objects may recompile; the gate is
  that the `.asi` relinks with no source change inside it.

## 3 STEP 4 — the four verdicts, registered unchanged

(a) **`wcs_drift` fires 0 times at `d = 222..250`** on the default build, as the ORIGINAL's
bitwise-unchanged witness implies, **and** `T_post` within attempt 18's bars (5.5560 /
8.6373). Substeps per frame will still be **3** (the loop is not ported in this attempt),
which is expected and non-blocking.
(b) **launch PASSES** — `a8_launch.py`, the same statistic attempt 19 reported at 0.19 %.
(c) **recovery** under H1: `>= 50 %` of the 400 post-trough frames `>= 100` **AND** median
`>= 900`.
(d) **the three D2 metrics, 3 runs, against the UNCHANGED `d81a8df6` bounds** — slip
1500-2000 `0.18855..0.19635`, slip 2000-2600 `0.24488..0.25487`, driving-median
`1904.70..1982.44` — on the §16.7 arm (`MASHED_MEASURE_SOLO=1 MASHED_TRACK_SEL=12
MASHED_STEER_HOLD_AFTER=0`), muted, `MASHED_WIN_POS=primary-bl`, `MASHED_TITLE` per run,
PIDs tracked and reaped, `participants=1` confirmed from the game's own `MATCH-SEED` line,
median frames via `a8_medframe.py`, and §26.10's median-frame guard applied before any
magnitude is read as physics.

**Boot-transient rule, registered because it already bit this session:** a run that produces
no `motion_diag.log` is **retried up to 3 times** before being called a failure (memory
`shadow-lane-failure-windows`; two of this session's five port boots failed transiently and
the identical control succeeded on the next boot).

## 4 The substep loop — a separate step, and the condition on it

If (a) passes, the transcribed 2-substep loop `0x00471106..0x00471141` MAY be ported as a
**separately pre-registered** step with its own promotion leg, and the launch reported as
**FAIL** if it regresses. It is **not** part of STEP 3.

## 5 Collateral, registered

`collateral.py` with a floor, three legs: (i) paired same-side pre-fix (`MASHED_D2_BATCHMODE
=plane`) vs post-fix (default), classified against the fix's write set — which is
`g_terrainBatch` contents, `g_terrainEntryCount`, `g_terrainPassCount` and, downstream,
every wheel record field the classifier writes (`+0x194`, `+0x1ec`, `+0x1f0`,
`+0x200..+0x208`, stride `0xc4`) and the wheel states `+0x198`; (ii) the same-side noise
floor from a second run of each arm; (iii) the cross-side banded leg at matched `d`, with
§26.10's off-regime rule applied. Any divergent field outside that write set is reported as
an **outside-scope row**.
