# D2 attempt 20 — PRE-REGISTRATION, STEP 5: port the transcribed integer substep loop

Committed **UNRUN**. Permitted by `PREREG_STEP34.md` §4, whose condition — *"if (a)
passes"* — is **met**: `RESULT_STEP34.md` §3(a) reports `wcs_drift` at **0** firings and
`T_post` inside both bars. This is a **separate** step with its own gate, not part of STEP 3.

## 1 The change, transcribed, one edit

The original's loop, read instruction by instruction in attempt 19 `RESULT.md` §2 from
`MASHED.exe.unpatched` and **not re-derived here**:

```
00470f50..00470f61   chunk = min(remaining, 0x32)      ; INTEGER
00471106  mov  edi,[esp+0x10]      ; rem = chunk, an INTEGER ms count
0047110c  jbe  0x471143            ; rem <= 0 -> no substep
00471110  cmp  edi,0x19            ; LOOP TOP, 25
00471117  mov  esi,0x19            ; sub = min(rem, 25)
00471126  fild [esp+0x20]          ; (float)sub  <- INTEGER-sourced, so NO residue step
00471137  call 0x4709a0            ; THE SUBSTEP, arg0 = (float)sub
0047113f  sub  edi,esi             ; rem -= sub
00471141  jne  0x471110            ; while (rem != 0)
```

`50 -> 25, 25` is exactly **2**, by integer arithmetic.

The port today (`VehiclePhysicsRun.cpp:918-922`, `:1076`) runs a **float** loop
`while (remMs > 0.0f)` over `frameMs = dt * 3000.0f = 50.0000038f`, which yields
**3** substeps — `25.000000`, `25.000000`, `0.0000038147` — measured at
`nsub {3: 1626}` on this attempt's own `a1` capture.

**The edit:** make the budget and the loop counter **integers**, exactly as above:

```
int remI   = (int)frameMs;                      // the integer ms budget (50 at 60 Hz)
int chunkI = (remI < 0x32) ? remI : 0x32;       // 0x00470f50..0x00470f61
while (chunkI) { int subI = (chunkI < 0x19) ? chunkI : 0x19;   // 0x00471110/0x00471117
                 chunkMs = (float)subI;                        // 0x00471126 fild
                 ...substep...
                 chunkI -= subI; }               // 0x0047113f/0x00471141
```

**STATED RESIDUAL, not ported in this step:** the original's OUTER chunk loop
`0x00471143..0x00471151` (`remaining -= chunk; if (remaining) goto 0x470f50`), which re-runs
A4 for all 16 vehicles per 50-ms chunk. At `dt = 1/60` the port's `remI` is **50**, so
`chunkI == remI` and that outer loop iterates exactly **once** — it cannot be distinguished
on this arm. **[UNCERTAIN]** for any `dt` that makes `remI > 50`; recorded, not papered over.

**No new constant.** `0x32` and `0x19` are the original's own immediates at the RVAs cited,
and `kMaxSubstep = 25` already exists at `VehiclePhysicsRun.cpp:120` (its RVA citation is
corrected to `0x00471110`/`0x00471117` by attempt 19 §2). No knob, no clamp, no fitted
value.

**PROMOTION LEG.** `FUN_00470c70` (`0x00470c70`) is the RVA the loop belongs to and it is
already the port's own dispatcher body, so this is a **fix to the existing body at its own
RVA, not a parallel copy**. `run_diff` path1 / `run_verify_hook` path2 are **not**
applicable: `0x00470c70` is the per-frame 16-vehicle dispatcher, it has no isolatable
signature, and the port's equivalent is a C++ function with no installed inline-JMP — this
is `re/CONFIDENCE.md`'s non-repeatable-function case. **No C-level is requested:**
`0x00470c70` **C2 -> C2**, `0x004709a0` **C2 -> C2**. `dual_copy` must report `NEW = 0`.
The claim made is narrow and checkable: **the port's substep count becomes 2 per frame**,
against the original's measured 2 (this attempt's own `--wheelstate-probe` count-first run:
4672 solver calls over 2321 frames = **2.012/frame**, histogram `{2: 2308, 3: 24}`).

## 2 Gate, registered before the run

**G5-1 (the claim).** `nsub` histogram is `{2: N}` on the `a1`-equivalent sink capture, with
chunks `25.000000 / 25.000000` and **no residue step**.

**G5-2 (the launch must not regress).** `a8_launch.py` `L = 0` at `<= 1.00 %`. If it
regresses, **the launch is reported as FAIL** and that is the headline, exactly as
`PREREG_STEP34.md` §4 requires.

**G5-3 (nothing already passing may break).** `wcs_drift` stays at 0 firings;
`T_post` stays inside 5.5560 / 8.6373; recovery H1 keeps passing; the two slip metrics stay
inside their `d81a8df6` bounds.

**DECISION, registered now.** The step is **KEPT** only if G5-1 passes **and** G5-2 passes
**and** G5-3 passes. If any of the three fails, the edit is **REVERTED** and the failure is
reported with its numbers — the step is not kept on the strength of the driving-median
alone, and the driving-median is **not** a gate here. Whatever the driving-median does is
**reported, not used to decide**, because choosing to keep a change because one metric
improved is exactly the fitting this project forbids.

**Boot-transient rule** as in `PREREG_STEP34.md` §3: a run with no `motion_diag.log` is
retried up to 3 times before being called a failure.

## 3 Measurement set

3 scoring runs (`t1`, `t2`, `t3`) plus one sink run (`b1`) on the §16.7 arm
(`MASHED_MEASURE_SOLO=1 MASHED_TRACK_SEL=12 MASHED_STEER_HOLD_AFTER=0`), muted,
`MASHED_WIN_POS=primary-bl`, `MASHED_TITLE` per run, PIDs tracked and reaped,
`participants=1` confirmed from the game's own `MATCH-SEED` line, median frames reported
with every metric. Collateral: paired STEP-3-only (`s1`/`s2`) vs STEP-3+5 (`t1`/`t2`) with
floors, write set = the substep cadence, i.e. everything the substep body writes plus
`BodyOrient_OmegaFromSteer`'s accumulator (`VehiclePhysicsRun.cpp:968-977`, which attempt 19
`RESULT.md` §6 already named as coupled to the substep count).
