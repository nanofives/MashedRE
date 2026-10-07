# PRE-REGISTRATION (UNRUN) — U-9186 `GATEFIRE`: port BOTH branches

Date 2026-10-08. **UNRUN at commit time.** Authorized by **USER DECISION (Mariano, 2026-10-08)**:
"pre-register the full GATEFIRE port — both branches, incl. `FUN_00414a70` + `FUN_00414300`."

Supersedes the withdrawn `H3-GATEFIRE` gate (`verify/d3_u9186_20261008/AMEND_H3.md` §3,
`RESULT_H3_STEP.md` §3), which was registered as instrumentation and is a port leg.

No C-level follows from any bridge in this leg (it inherits the 2026-10-06 decision:
knob-gated, non-bit-identical substrate acceptable, no C-level). `original/` untouched. The `.asi`
code path untouched. Nothing default-ON.

## 0. The objective, and the one number that decides it

The original fires U-9186's two branches **36** and **64** times over the 220-call window
(`verify/d3_elim_20261003/RESULT_STEP2.md:106`, `:108`; 149 of 660 across 3 cars at `:178`). The
standalone fires them **0** times, because `ControlStep` hardcodes `int mode = 0`
(`AiStandalone.cpp:844`) with the targeting chain elided at source level. This leg makes the two
branches *executable and fed*, then counts them.

The original captures `o_t1`/`o_t2`/`o_t3` (`verify/d3_elim_20261003/`) carry a live **`ret14a70`**
column and the per-call inputs, so **every one of the 149 calls is checkable call-by-call.** That
is the acceptance surface, not an aggregate.

## 1. The record this builds on — read before starting, do NOT re-derive

Three prior measurements already answer most of "what is missing". **Re-deriving them is the
failure mode recorded in memory `read-the-tracker-row-before-starting-its-work`.**

- **`UNCERTAINTIES.md:61` (U-9186)**, 2026-10-06, verbatim: branch 1 (`FUN_00414a70 == 2`, 36
  calls) "reads `FUN_00408a50` per-car progress (`0x008a96e8+v*0x30c`, no writer -> 0 for all cars
  -> degenerate, never returns 2)"; branch 2 (64 calls) "reads `RefDist`=`FUN_00442cc0`
  (`0x008989b0`, no writer -> 0), leader `0x0089a364` (never written), catch-up `0x0089a4c4/4c8`
  (only zeroed) -> returns 0, never fires."
- **`UNCERTAINTIES.md:63` (U-9187) + `verify/d3_leader_20261003/RESULT_WITNESS.md`**, 2026-10-03:
  wiring `FUN_004148b0` by adding TUs to `exe_sources.rsp` would be **"BOTH unsafe AND inert"** —
  it access-violates on its first callee and returns 0 on the first table test. That witness
  **resolved two inputs in passing** (§5, §6 below) and corrected a prior "no new reversing
  needed" claim. Its rule was applied and still applies: **"seeding the globals would not be a
  port"** (`verify/d3_modes37_20261002/RESULT_STEP2.md`).
- **H1a / H1b / H3**, 2026-10-08: three of the witness's six missing inputs have since landed.

**Registered tracker conflict.** U-9186's standing instruction orders the work "(1) add
`AiLeaderTimer.cpp`, `AiTargeting.cpp`, `AiLineOfSight.cpp` to `exe_sources.rsp` … (2) then
`FUN_00414a70` + `FUN_00414300`", but U-9187 measured step (1) as written to be unsafe and inert.
**This pre-registration resolves the conflict in U-9187's favour**: the `.rsp` addition is NOT the
mechanism (§4.1). U-9186's *ordering* (branch 2 before branch 1) is kept. Flag for `re-classify`:
U-9186's instruction text should be amended to match.

## 2. State of the six witness inputs, re-checked today

Ground truth is **`.rsp` membership**, not `hooks.csv`'s `exe_file` column — the split is purely by
response-file membership, there are no target ifdefs.

| # | input | witness 2026-10-03 | **today** | evidence |
|---|---|---|---|---|
| 1 | `FUN_0040e470` | missing | **RESOLVED** | exe body `Frontend/MenuStateMachine.cpp`, in `exe_sources.rsp` |
| 2 | `FUN_00442cc0` (RefDist getter) | missing | **OPEN — trivial** | body `Ai/PromoLoop_round1.cpp`, `asi` only (E=0 A=1). Reads `0x008989b0[v]` |
| 3 | `FUN_0046d4a0` | missing | **RESOLVED (H1b)** | `Vehicle/VehicleRecordPtr.cpp`, shared TU, E=1 A=1 |
| 4 | `FUN_00442a60` producer of `0x008989b0` | **"a real port, new reversing"** | **RESOLVED (H3)** | `Race/SpectatorDistances.cpp`, E=1, default-OFF `MASHED_REFDIST`; 26,994/53,992 rows non-zero |
| 5 | limit table `0x005f2dd8` | **RESOLVED by the witness** | transcribe | only index **10** is ever used, value **1**; head of 64 `[2,1,1,0,0,1,1,0,0,0,1,0,0,0,0,0]` (`RESULT_WITNESS.md:106`) |
| 6 | thresholds `0x005cd0a8/a4/a0`, `0x005cc35c` | **RESOLVED by the witness** | transcribe | **6.5 / 5.5 / 6.0 / 4.0**, read twice independently and agreeing to every digit (`:107`, `:23`) |

**Two upstream disagreements survive all six** (`RESULT_WITNESS.md:109-113`) and are the real
remaining risk for branch 2:

- **`idx364`** (`0x0089a364`): original **`-1` on all 512 calls**, port **`0`**. On the port this
  makes `idx364 != -1` true and calls `E470(0)`, a path the original never takes.
- **`bias374`** (`0x0089a374`): original **`0`**, port **`{0,1,2,3}`**. Written by `FUN_004177b0`,
  whose exe copy (`Race/RuleEngine.cpp`) was **demoted C3→C2 on 2026-09-29** as "the finish-order
  fragment only".

**Tracker defect found while checking, to be fixed via `re-classify`, not by hand:**
`hooks.csv` row `004148b0` carries `exe_file = mashedmod/src/mashed_re/Ai/AiStandalone.cpp`, but
`AiStandalone.cpp` contains **no body** for it — only comments at `:82`, `:846`, `:968`, `:1086`.
The row overstates exe-side coverage. (`0046d4a0`'s empty `exe_file` is the opposite artifact: the
body is a shared TU, so the column is redundant. Both are schema drift, not code defects.)

## 3. Leg 0 — RUNTIME substrate census. **Runs first; nothing else is read until it passes.**

The static census in `AMEND_H3.md` §3.1 proves only that an address is *addressed by compiled
code*, not that a value is *live in a race*. Leg 0 closes that, and it is the gate that stops this
leg from producing a number with no meaning (memories `scratch-field-false-green`,
`schedule-derived-counts-are-not-evidence`, `all-zero-reads-prove-nothing-alone`).

Instrument: extend `TrackRenderer::U9186GateDump()` (`TrackRenderer.cpp:4022`) with **append-only**
columns — never reorder, existing scorers key on position. Default-OFF via the existing
`MASHED_U9186_GATES`. Scenario and arms exactly as `run_h3.ps1`: `MASHED_DETERMINISTIC=1
MASHED_DET_FRAMES=14400`, three repeats, arms OFF / `MASHED_REFDIST=1`.

Reuse `re/tools/sa_leaderwatch.py` (read-only `ReadProcessMemory`, no injection) as the
independent second channel, as the witness did.

| gate | threshold |
|---|---|
| `GF0-REFDIST` | with `MASHED_REFDIST=1`, `0x008989b0 + v*4` is non-zero on the three AI cars on ≥1 frame. **Expected PASS** — H3 measured it. A failure means H3 regressed and nothing else is read |
| `GF0-IDX364` | report `0x0089a364`'s distinct values and their frame counts, both arms. The original is `-1` on 512/512. **This gate cannot fail; it is a measurement.** Its value decides §4.3 |
| `GF0-BIAS374` | same for `0x0089a374`. Original is `0` on 512/512 |
| `GF0-TIMER` | `0x0089a4c8 + v*0x74` — distinct values. Witness measured the port **dead at `0`** vs original `0..1250` stepping by 50. If still dead, branch 2 cannot fire and §4 must say so rather than wire around it |
| `GF0-PROG1` (**branch 1's blocker**) | `0x008a96e8 + v*0x30c` — distinct values per car. U-9186 says no writer → `0`. **If `0`, branch 1 is input-blocked and §5 does not start** |
| `GF0-CTL` (**negative control that must hold**) | the `_abs`-style dead columns stay dead, and a knob-off arm is byte-identical to `H3step.csv`'s SHA-256 `86b7b2bb` |
| `GF0-DET` | 3 repeats identical within each arm |

**Decision rule.** `GF0-PROG1` zero → **branch 1 is deferred with its blocker named**, and this leg
delivers branch 2 only. `GF0-TIMER` dead → branch 2's port is written but its counter is reported
as structurally-0, with the missing writer named. Neither case is a licence to seed a global.

## 4. Leg 1 — branch 2 (`FUN_004148b0 != 0 && FUN_00416060 != 0`, target 64)

### 4.1 The mechanism is a standalone body, NOT an `.rsp` addition

`AiLeaderTimer.cpp` and `AiTargeting.cpp` call into `0x00400000..0x004fffff`, which
`exe_main.cpp:56` leaves **unmapped** (`Compat/StandaloneRvaThunks.h:7`). Adding them to
`exe_sources.rsp` access-violates — measured, `RESULT_WITNESS.md` §W-SAFE. Instead:

- A new exe-side TU carrying a `LeaderTimer` body transcribed from `AiLeaderTimer.cpp:90-131`,
  with every absolute read replaced by the standalone's own accessor or a cited literal.
- `0x005f2dd8` → a named constant table from `RESULT_WITNESS.md:106`, **with the index expression
  preserved** (`bias374 + iVar1*5`, element size 4, `AiLeaderTimer.cpp:98`). Do not collapse it to
  "index 10 = 1": that is true of the original's inputs, not of the port's.
- `0x005cd0a8/a4/a0`, `0x005cc35c` → **6.5 / 5.5 / 6.0 / 4.0** as named literals with their
  addresses cited, the `H3-CONSTS` pattern. **Re-read them from `MASHED.exe.unpatched` rather than
  copying this file** — a transposed digit in a live constant has misled three legs before
  (memory `read-the-rva-plate-before-building-a-probe`).
- `FUN_00442cc0` → an exe-side body reading `0x008989b0[v]`, now fed by H3.
- The `__ftol` of `0x0089a360` must keep its forwarding semantics. `AiLeaderTimer.cpp:22-23`
  deliberately forwards to the original `FUN_004a2c48` "rather than assume truncate-vs-round"; the
  standalone cannot call it, so **the rounding must be resolved from the listing and the choice
  recorded**, not assumed. Registered as `GF1-FTOL`.
- LOS: `AiStandalone.cpp:282-288` already implements `FUN_00416060`'s exact tile test over
  `0x007f1a9c`/`0x007f9a9c` (loaded at `TrackRenderer.cpp:359`). **Lift it into a
  `LineOfSight(a,b)` shape and call it; do not add a second body** — `0x00416060` already has a
  duplicate-RVA history (`hooks.csv:654`, dual-install refused) and `lint_rva_bodies.py`'s
  allowlist is a burn-down list ROADMAP D4 zeroes.

### 4.2 Gates

| gate | threshold |
|---|---|
| `GF1-CONSTS` | all four thresholds + the used limit-table entries re-read non-zero from `MASHED.exe.unpatched`, each reported with file offset and hex bits. Any zero → stop |
| `GF1-FTOL` | the `0x0089a360` `__ftol` rounding is resolved from the listing, stated, and a counter-example input is shown to round the same way both ways, or the disagreement is reported |
| `GF1-REACH` | the ported `LeaderTimer` is **called** in a race — a call counter > 0, default-OFF. Distinct from "the branch returns 1" |
| `GF1-FIRE` (**the one that matters**) | `FUN_004148b0 != 0 && LOS != 0` count over the 220-call window, per car, vs the original's **64**. Report the raw count, not a ratio |
| `GF1-CALLWISE` | the per-call comparison against `o_t1`/`o_t2`/`o_t3`: of the 64 calls the original fires, how many does the port fire **on the same call index**. This is the acceptance surface |
| `GF1-INERT` (**can fail, and failing is the good outcome**) | OFF vs ON cell-for-cell on the stepdump. If identical, report INERT |
| `GF1-KNOBOFF` | knob-off stepdump hashes to `86b7b2bb` |
| `GF1-NOREG-E` / `-B` | criterion (e) six digits and (b) bands unchanged on the OFF arm |
| `GF1-DET` | 3 repeats identical per arm |

### 4.3 Registered hazards

- **`idx364` = 0 vs -1 is a behavioural fork, not a detail.** At `AiLeaderTimer.cpp:94` it selects
  whether `E470(idx364)` runs at all. If `GF0-IDX364` still reports `0`, the port executes a path
  the original never executes on any of 512 calls, and `GF1-FIRE` would be measuring a different
  function. **Resolve before `GF1-FIRE` is read.** Finding the writer of `0x0089a364` is in scope;
  seeding it is not.
- **`bias374`** likewise shifts the limit-table index. Its writer `FUN_004177b0`'s exe copy is C2
  (demoted 2026-09-29), so a disagreement here may be that demotion surfacing.
- `FUN_004148b0` is **stateful** (`AiLeaderTimer.cpp:26-28`) and in the original "fires only on the
  orchestrator's mode-5 path … which a normal race rarely reaches". A count of 0 may mean the
  scenario never reaches the path rather than that the port is wrong — **`GF1-REACH` exists to
  separate those two** (memory `absent-log-proves-nothing-run-a-control`).
- The 2026-07-28 `ds:` bug in that file (`:139-143`) disabled a mode gate on a default-installed
  hook for its whole life. The transcription is from the **C++ body**, not the naked thunks; do not
  carry the asm across.

## 5. Leg 2 — branch 1 (`FUN_00414a70 == 2`, target 36). **Gated on `GF0-PROG1`.**

Two verbatim ports from C2 plates, `bucket_ai_00407a40_00415880/0x00414a70.md` and `.../0x00414300.md`.

| | `FUN_00414a70` | `FUN_00414300` |
|---|---|---|
| span | `0x00414a70..0x00414c2e` (446 B) | `0x00414300..0x0041448d` (397 B) |
| args | 4 `(spline, float*, out, int vehIdx)` | 6 `(vehIdx, own XZ, ref XZ, tgt XZ, float radius, float* outLead)` |
| returns | `0`/`1`/`2` | `1`/`0` |
| globals (R) | `0x005d757c` 0.0f; `0x005cd0ac` speed-divergence; `0x005cc348` inactive-close | `0x005d757c`; `0x005cc320` **`+1.0f`**; `0x005cc558` dist² threshold |
| callees | `0046d4a0`✓ `0046d6d0`✗ `0046d510`✓ `00408a50`✓ `0040e370`✓ `00426bb0`✗ `004c3ac0`✓ `00414300` `0040e470`✓ | `0046d510`✓ `004c3b30`✓ |

✓ = already in `exe_sources.rsp`. **✗ = missing and must be added to scope:** `0046d6d0`
(`Util/PromoLoop_round54.cpp`) and `00426bb0` (`Util/PromoLoop_round31.cpp`), both C3 `impl`,
`asi`-only.

**Load-bearing plate conflict — use the bucket plate.** `0x00414300.md:33` explicitly corrects the
older note: `_DAT_005cc320` is **`+1.0f`** (bytes `00 00 80 3f` at `0x005cc320`), **not `-1.0`**.
`re/analysis/ai_update_d3/0x00414300.md:33`/`:73` still says `-1.0` and is **wrong**. A sign error
here flips the perpendicular-slope and normalize terms.

**Open `[UNCERTAIN]` carried from the plates, each to be reported not guessed:** `U-7570` (the `5`
in `5 - FUN_00426bb0()` is unproven), `U-7571` (`0x005cd0ac` / `0x005cc348` floats never read),
`U-7565` (`0x005cc558` never read). All three must be read from `MASHED.exe.unpatched` as part of
`GF2-CONSTS`. Neither plate records per-branch RVAs; `re/analysis/ai_update_d2/0x00414a70.md:40`
cites the `FUN_00414300` call site at `0x00414b98`. **Calling convention is plate-silent for both
— resolve from the listing before writing a signature.**

**The input blocker, stated plainly.** `FUN_00408a50` reads `0x008a96e8 + v*0x30c`. U-9186 says it
has no writer. Separately, **D-11072 leg A measured** that the port's racepct "is NON-MONOTONE
within a lap — 484/681/1287 small backward reversals per car vs the original's 0 — so it is NOT
range-mappable into `0x008a96ec`'s consumers". `0x008a96e8` and `0x008a96ec` are 4 bytes apart in
the same `0x30c` per-car stride. **[UNCERTAIN]: whether they are two fields of one record, and
whether leg A's negative result transfers to `0x008a96e8`, is unmeasured.** `GF0-PROG1` measures
the field; deciding the producer is a separate question this leg does not pre-commit.

| gate | threshold |
|---|---|
| `GF2-CONSTS` | `0x005cd0ac`, `0x005cc348`, `0x005cc558`, `0x005cc320` read from the unpatched image with offsets and hex bits; `0x005cc320` **must** read `00 00 80 3f`. Closes U-7571 / U-7565 |
| `GF2-CALLEES` | `0046d6d0` and `00426bb0` have exe-side bodies and are called, not thunked |
| `GF2-FIRE` | `FUN_00414a70 == 2` count over the 220-call window vs the original's **36** |
| `GF2-CALLWISE` | per-call agreement against `o_t*`'s live **`ret14a70`** column — the directly comparable channel |
| `GF2-INERT`, `GF2-KNOBOFF`, `GF2-NOREG-E`/`-B`, `GF2-DET` | as §4.2 |

## 6. Decision rules

- `GF0-PROG1` = 0 → **branch 1 is deferred, blocker named, leg delivers branch 2 only.** Do not
  widen scope to build a producer inside this leg.
- `GF0-TIMER` dead → branch 2's body still lands, and `GF1-FIRE` is reported as structurally-0 with
  the missing writer named. A port that cannot fire is still a port; a seeded global is not.
- `GF0-IDX364` ≠ `-1` → resolve the fork before `GF1-FIRE`/`GF2-FIRE` is interpreted.
- Any `*-FIRE` approaching the target → the lane's premise is confirmed and wiring the
  `FUN_00416250` branches to `ctrl` becomes the **next** registered leg, not this one.
- Any `*-FIRE` ≈ 0 with `*-REACH` > 0 → the port runs and the branch does not fire; report the next
  blocker rather than widening scope.
- Any `*-INERT` identical → report INERT, exactly as E2 and H3 did. **Do not chase an effect by
  enabling anything further in the same leg.**
- Report every registered threshold's verdict even when a corrected denominator passes, and name
  residual rows rather than waiving them.

## 7. Non-goals

Any default-ON change. Wiring `FUN_00416250`'s branches to `ctrl`. Any C-level promotion. Any edit
to `original/` or the `.asi` code path. Seeding any global. Adding a `lint_rva_bodies.py` allowlist
entry (consolidate instead). Shipping the `MASHED_RACEMETRIC_ARC` ON arm. Mode 7, which U-9186
defers until the standalone owns a world-object list.
