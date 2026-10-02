# D2 attempt 20 — STEP 2: the per-wheel comparison. U-9179 RESOLVED to one arm and one producer.

Pre-registration `PREREG_STEP2.md`, committed **unrun** at `49bf17c9`. Reducer
`re/tools/statediff/a20_wheelstate.py`. Every metric below carries `n`, the median speed
and `d` with `L = 0` and `R` from **each capture's own `+0xbf8` marker**
(ORIG `R = 897`, PORT `R = 1`).

---

## 1 Coverage and the safety gate

**COUNT-FIRST (PREREG §1.2), run before any sampling run:** `--wheelstate-probe-count`,
`orig_ws0.msd` — site 0 fired **4672** times over **2321** A6a frames = **2.012/frame**,
≈ **123/s** over the 38 s hold, against the registered `< 400/s` bar and two orders under
the ~1000/s destabilise floor. **PASS.** `otherEdi = 0` on all 4672 calls and the
self-check reports `edi == rec == 0x8815a0` on every sampled row, so the register
convention is **measured, not assumed**, and only the player's record is solved.

**GATE CV — PASS both sides.**

| | ORIG `orig_ws1` | PORT `p_sm` |
|---|---|---|
| rows / calls | 4688 site-0 rows, **0** dropped by the `edi` filter | 4664 complete calls, **0** unparsed lines |
| frames | 2332 (`.msd`), release `+0xbf8 != 0` at frame **897** | 1522 |
| calls per frame | `{2: 2308, 3: 24}` | `{3: 1522}` |

The ORIGINAL's **2** calls/frame and the PORT's **3** are attempt 19's substep finding,
reproduced here from a different instrument on both sides.

## 2 The two known-answer gates

**GATE KA-O — PASS, 4686 / 4687 = 99.9787 %.** The rule transcribed in `RESULT_STEP1.md`
§2-§4 — `bVar4`, the two arms, the latch, then the 4-wheel drop and the `bVar16 == 3`
promotion — reproduces **the running original's own four wheel states** from its own logged
inputs on all but one consecutive pair (the single miss is `in=(1,1,1,1)
predicted=(0,0,0,0) actual=(1,1,1,1)`, one row). **STEP 1's transcription is therefore
confirmed behaviourally against the original, not only statically.**

**GATE KA-P — one leg PASS, one leg FAIL. Reported as a failure, cause measured.**

| leg | asked | result | verdict |
|---|---|---|---|
| pre-drop `state'` == the PORT's measured `wcs_sm` | `>= 99.5 %` | **4663/4663 = 100.0000 %** | **PASS** |
| entry-pair states reproduced on the PORT | `>= 99.0 %` | **3138/4663 = 67.2957 %** | **FAIL** |

**Cause of the FAIL, measured not waived:** on the PORT, `ReassertContacts`
(`VehiclePhysicsRun.cpp:343-357`) runs **between** solver calls and **promotes every
state-0 wheel back to 1** (`:347`), so the state at the next entry is not the state the
solver left. The ORIGINAL's counterpart tail `0x0047044b..0x004704b0` only **counts**
`state != 0` — it does not promote — which is why KA-O passes with the same replay.
**This sharpens attempt 19 `RESULT.md` §1.3**, which recorded `ReassertContacts` as "that
tail": the *counting* half is faithful, the *promotion* at `:347` and the normal writes at
`:350-352` are **port-only**. The FAIL does not touch any reading in §3: every port figure
there comes from the directly measured `wcs_sm` channel, whose own leg is 100.0000 %, and
the promotion cannot change the gate's input within a call (it runs after it). It is
reported as a failure and **not re-thresholded**.

**GATE EV — PASS.** `n = 29` frames each side at `d = 222..250`; the port's median speed
**652.99** is within 25 % of attempt 19's **633.2**. Median frame index ORIG **1133.0**
PORT **237.0** — far apart, which is the normal consequence of the two sides reaching the
same `d` at different absolute frames, and the comparison below is at **matched `d`**, not
at matched frame.

## 3 The comparison — and the term that diverges

### 3.1 Carrier window, `d = 222..250`, `L = 0`

ORIG `n = 58` solver calls over 29 frames, median speed **850.26**, median frame 1133.
PORT `n = 89` solver calls over 29 frames, median speed **652.99**, median frame 237.

| quantity | ORIG | PORT | &#124;delta&#124; |
|---|---:|---:|---:|
| **`K` — wheels with `key != -1`** | **4.000** | **2.000** | **2.000** |
| `F` — wheels with `-2.0 < fv <= 0.0` | 2.000 | 2.000 | 0.000 |
| `S1` — wheels with `stateOut == 1` | 4.000 | 2.000 | 2.000 |

```
ORIG arms (232 wheel-rows):  A-hold 232          (100.00 %)
PORT arms (356 wheel-rows):  A-hold 257, A-demote-key 99   (27.81 %)
ORIG stateOut words: (1,1,1,1) x58
PORT stateOut words: (1,1,1,1) x37, (0,0,1,1) x31, (0,1,1,0) x16, (0,1,1,1) x5
```

**DECISION RULE 1 FIRES ON `K`.** The diverging term is the **contact key `+0x1ec`**, and
the arm that consumes it is **`A-demote-key`** — the `piVar9[0x15] == -1` demotion at
`0x0046f91a`/`0x0046f91f`, `WheelContactSolver.cpp:178-179`. **The `:163` state-0 -> 2 latch
arm fires 0 times**, and `bVar4 == 0` on every call, so **U-9179's two remaining arms are
reduced to one and the other is eliminated by measurement.**

### 3.2 Launch control window, `d = 0..100`

ORIG `n = 203`, median speed **624.31**; PORT `n = 304`, median speed **624.69** — the two
sides are at matched speed here, which is what makes this a control.

| quantity | ORIG | PORT | &#124;delta&#124; |
|---|---:|---:|---:|
| `K` | 4.000 | **4.000** | 0.000 |
| `F` | 2.000 | 4.000 | 2.000 |
| `S1` | 4.000 | **4.000** | 0.000 |
| arms | `A-hold` 812 (100 %) | `A-hold` 1216 (100 %) | — |

**The control is clean on the term the carrier window names:** at launch the port's
classifier yields **all four** keys and the state word is `(1,1,1,1)` on 304/304, exactly
the original's. So the defect is **not** a constant property of the port's contact chain —
it appears only once the car is away from the start line. (`F` differs here in the opposite
direction while `S1` agrees, because `F` describes the *state-0* arm, which fires on
neither side; it is reported for completeness and names nothing.)

## 4 Upstream, by code site: why the key is missing

`fv == 10.0` **exactly, on all 99** `key == -1` rows — the init loop's reset
(`WheelContactSolver.cpp:95`). So the classifier `0x0046cc40` never reached its fill at
`CarWorldContacts.cpp:126-131` for those wheels. The new per-wheel **stage** channel
(`wcs_cls`, default-OFF) says which gate stopped it; stage 7 (FILLED) agrees with
`key != -1` on **356 of 356** wheel-rows, so the channel is consistent with the one it
explains.

| wheel | stage 1 — failed SAT half-plane 3 (`:116`) | stage 3 — failed the approach test (`:119`) | stage 7 — FILLED |
|---|---:|---:|---:|
| 0 | **52** | 0 | 37 |
| 1 | **31** | 0 | 58 |
| 2 | 0 | 0 | 89 |
| 3 | 0 | **16** | 73 |

Stage 1 means *no admitted triangle contains that wheel*. And the batch is **saturated at
its cap on 3565 of 3565 solver calls in the whole capture**: `g_terrainEntryCount == 256`
with `s_batchStorage[256]` (`ContactProducer.cpp:26`).

### 4.1 The first diverging producer — `ProduceTerrainBatch`'s admission test

`ContactProducer.cpp:77-81` admits a triangle when the query centre is within `radius` of
the triangle's **infinite plane**. On a largely coplanar track that admits ground triangles
from anywhere on the surface, the 256 slots fill with them, and the loop then **stops
scanning**, so the triangles actually under wheels 0 and 1 are never offered to the
classifier.

**The ORIGINAL has no cap.** Its collector `LAB_00468b80` increments `DAT_0088e60c`
unconditionally at `0x00468d6c..0x00468d73` (`mov ecx,[0x88e60c] / inc ecx /
mov [0x88e60c],ecx`) and there is **no bound test anywhere** in `0x00468b80..0x00468d7c`
(`disasm_fn.py`, committed as `disasm_collector_00468b80_00468d80.txt`). The locality comes
entirely from the BSP walk `FUN_00538c80` that `ProduceTerrainBatch` stands in for — which
`hooks.csv` row 1276 already records under U-9156 as *"appends every triangle whose PLANE is
within radius of the query centre instead of walking COLLI\*.BSP"*. **STEP 2's contribution
is to measure that this stand-in is the first diverging producer for U-9179.**

### 4.2 The three-arm A/B that confirms it on the running port

`MASHED_D2_BATCHMODE`, default-OFF at the time of these runs. Window `d = 222..250`,
`n = 89 / 89 / 87` solver calls, 29 frames each, median speed 652.99 / 652.99 / — .

| arm | admission test | scan | `nEntries` | `nPass` (uncapped) | `K` | `S1` | stages | **`wcs_drift` firings** |
|---|---|---|---:|---:|---:|---:|---|---:|
| `plane` (legacy default) | plane distance | stops at cap | 256 | 379 | 2.00 | 2.00 | `1`:83 `3`:16 `7`:257 | **47** |
| `planefull` | plane distance | full | 256 | 379 | 2.00 | 2.00 | `1`:83 `3`:16 `7`:257 | **47** |
| `local` | **AABB(tri) + radius contains centre** | full | **17** | **17** | **4.00** | **4.00** | **`7`:348 only** | **0** |

Three readings, stated separately:

1. **The cap is not the whole story on its own.** `planefull` scans every triangle and
   still admits only 379, of which 256 are stored — and its stage counts and drift firings
   are **identical** to legacy. So "raise the cap" is **not** shown to be sufficient by this
   arm, and it is not what is being fixed.
2. **The admission test is the defect.** With a spatial test the batch holds **17** entries,
   does not saturate, and **every one of 348 wheel-rows reaches stage 7** — `K = 4`,
   `S1 = 4`, the original's own numbers at matched `d`.
3. **`wcs_drift` fires 0 times.** That is STEP 4 criterion (a), met by the mechanism rather
   than by a threshold, and it also removes wheel 3's 16 approach-test rejections — because
   with the local triangles present the face normal the test dots against is the one under
   the car.

## 5 What this does NOT claim

- No C-level is moved and none is requested: `ProduceTerrainBatch` is **port-only
  scaffolding** with no RVA of its own. `0x0046f6c0` C2, `0x0046cc40` C2, `0x00468d80` C2,
  `0x00538c80` C1 — all unchanged.
- The `:163` latch arm is eliminated **for this window and this arm**; the statement is
  `0 of 356` wheel-rows at `d = 222..250`, not a claim about every scenario.
- AI slots 1+ (`VehiclePhysicsRun.cpp:702`) are untouched by STEP 2 — nothing here reached
  them. **[UNCERTAIN]** whether the STEP 3 fix moves them; it is measured in STEP 4's
  collateral, not predicted here.
- `0x0046bb47`, `RESULT_STEP1.md` §5's named candidate for an out-of-function state writer,
  is **not** on the original's per-frame path: KA-O's 99.9787 % leaves no room for one.
