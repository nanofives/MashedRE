# TT-2 triage — the 155 sweep FAILs classified (2026-09-09)

Follow-on from `matchdiff_sweep_20260909.md`. The sweep left 155 `.data`-correspondence
failures that were a mix of real transcription defects and compiler/source-shape artifacts.
Eyeballing 155 delta strings invites confirmation bias, so **every class here is decided by a
test against the binary**, and rows no test explains are the shortlist.

Tool: `re/tools/matchdiff_triage.py` → `re/parity/matchdiff_triage.csv` (per-row class + the
full vector of tests that matched, so a reviewer can see what else fired).

## Result

| class | rows | share | what the test checks |
|---|---:|---:|---|
| **CANDIDATE** | **57** | 37% | nothing below explains it → hand review |
| WINDOW | 31 | 20% | our extras all sit within 0x200 of an address the original references (original cached a base in a register, used `[reg+disp]`) |
| FOLDED-MEMBER | 29 | 19% | `our_literal − orig_base` equals a displacement the **original itself uses**, i.e. the compiler folded `base+offset` into one address |
| WRAPPER | 16 | 10% | our export is a naked ABI shim / thin wrapper; following one level into the `*_Body` symbol in the same object resolves the diff to zero |
| UNROLLED | 10 | 6% | our extras form an arithmetic progression whose base or bound the original references (we enumerate, the original walks a base register) |
| INDUCTION | 9 | 6% | orig-only and our-only pair 1:1 within ±0x40 (loop rotated to `base−1`, index shifted) |
| INLINED | 3 | 2% | every orig-only address lies in the `.data` footprint of a function our code calls (original inlined a helper we call out to) |

**98 of 155 (63%) are explained artifacts.** Combined with the sweep's 931 clean rows, the
current honest reading is **1,029 of 1,086 comparable rows accounted for**.

## Two classes were only found by checking, and both had bitten the earlier numbers

- **FOLDED-MEMBER.** Verified by hand on `0x0041eda0`: the original computes
  `eax = slot*0x2ac + 0x63d9e0` and then accesses `[eax + 0x294]`; the port writes base
  `0x63dc74 = 0x63d9e0 + 0x294` directly. **Effective addresses are identical.** Ten-plus rows
  in the `0x41ed..0x41f2` range share exactly this pair. The test is not "the delta is small" —
  it is "the delta appears as a displacement in the original's own instruction stream".
- **WRAPPER.** Nine rows looked like the port was a stub (ratios down to 0.00 — `AiControlStep`
  compiled to 5 bytes against 2,015). They are not stubs. `PromptStripTwin` and
  `VideoDialogInit_i3` are `__declspec(naked)` ABI shims that `call` a `*_Body` in the same TU;
  `AiControlStep` is a one-line wrapper over `ControlStepBody<kStepMain>`. The analysis was
  reading only the exported shim. Following one level into local relocation targets resolves 16.
  **A size ratio alone would have produced a false "these C4 rows are stubs" alarm** — the
  `STUB?` marker in the tool's output is a prompt to look, never a verdict.

## The 57 candidates

By confidence: **C3 35, C4 22.** No candidate is a single-address delta — every one involves a
set, which is consistent with structural differences rather than typos.

Three systematic sub-patterns inside the shortlist, each likely one explanation rather than N
defects (stated as leads, not conclusions — none verified this session):

1. **5 RWP rows** (`RwpShapeActiveBitSet`, `RwpBodyMatrixRefresh`, `RwpBodyRefreshGate`,
   `RwpSolverContextSet`, `RwpBodyTableLookup`) each differ by exactly one extra address,
   **`0x00771968`** — and that is the phase-machine global `scenario_launch.py` pokes. Almost
   certainly one deliberate port-side gate, not five defects. Confirm and, if so, document it.
2. **RW math rows** (`FastSqrt`, `FastInvSqrt`, `Vec2Length`, `Vec2Normalize`, `RwV3dNormalize`)
   miss `0x007d3ff8`/`0x007d3ffc` — the RW LUT pointers. The port deliberately uses
   `Math/RwLutGuard.h` sentinels instead (memory `veccap-pilots`, VECCAP-1). Expect this to be a
   **known, documented divergence**; verify and annotate rather than "fix".
3. **9 rows still at ratio < 0.5** — residual wrapper depth. The union follows exactly one level;
   these delegate further. Extending to two levels is the cheapest next reduction.

`0x00404320 PerModeRenderMachine` is **not** in this list — it was confirmed and filed as
**U-9086** during the sweep pass.

## Standing limits

- A class label here is an *explanation*, not a clearance: a WINDOW or UNROLLED row is
  compatible with the port being correct, and compatible with it being wrong in a way this test
  cannot see. The classes bound the review, they do not replace it.
- CANDIDATE is not "defect". It means the six artifact tests do not apply, which is where hand
  review earns its keep.
- Nothing here executes anything. All of it is static: original bytes vs our compiled objects.

## Reproduce

    py -3.12 re/tools/matchdiff_sweep.py     # 931 PASS / 155 FAIL / 44 SKIP
    py -3.12 re/tools/matchdiff_triage.py    # classes + CANDIDATE shortlist

`re/console/cache/data_footprints.json` (218 KB) caches every original function's `.data`
footprint; delete it to rebuild. Both caches are regenerable and committed for offline reuse.

---

# Addendum - the three leads cleared (same day)

## Lead 1 - the 5 RWP rows: EXPLAINED, one cause, not five defects

All five (`RwpShapeActiveBitSet`, `RwpBodyMatrixRefresh`, `RwpBodyRefreshGate`,
`RwpSolverContextSet`, `RwpBodyTableLookup`) live in `Collision/RwpIntegrator.cpp`, and the
extra address `0x00771968` is the **session-phase global** (1=menu, 2=load+spawn, 3=race,
`re/frida/scenario_launch.py:41`). It is read by `b5cInRace()` (`RwpIntegrator.cpp:57`), the
guard on the in-process bit-identity self-test gated behind `MASHED_PHYS_C4_SELFTEST`
(`:35`, `:116`, `:143`, `:222`, `:268`, `:350`, `:414`).

A deliberate, env-gated port-side probe. **Resolved by inspection, NOT by a tool class** -
deliberately: an allowlist for "known port-side globals" is a mechanism that can hide a real
defect later, and five documented rows do not justify building one.

## Lead 2 - the RW-math rows: the hypothesis was WRONG, and the tool needed the fix

The prediction was "the port deliberately does not read the RW LUT pointers". The opposite is
true: `Math/RwLutGuard.h:74-75` reads `0x007d3ff8` **and** `0x007d3ffc` exactly as the original
does. The addresses were missing from the *function body* because `FastSqrt` calls
`RwLutGuard::Resolve` - an external symbol living in **another object**, which the
same-object union could not reach.

Fixed by indexing defined symbols across all 384 objects and following callees cross-TU.
All five rows (`FastSqrt`, `FastInvSqrt`, `Vec2Length`, `Vec2Normalize`, `RwV3dNormalize`)
now resolve to **WRAPPER** with an empty delta.

## Lead 3 - deeper wrapper following: done, and it needed an asymmetry to be sound

Depth was raised to 2 and extended cross-TU. **The first cross-TU attempt made things worse** -
CANDIDATE rose 57 to 62 - because folding a helper's globals into our side introduced
spurious *extras*: a helper's own state says nothing about the original.

The union is now **asymmetric**, and this is the load-bearing detail: it may only explain
addresses the original touches that our body does not ("we touch it inside a helper we call").
It never contributes extras, which are still computed from the function's own body.

Trade-off recorded in the source: every widening of the union weakens the test, because a real
defect can hide behind a helper that happens to touch the right address. Depth is capped at 2
and every followed symbol is written to the CSV `tests` column so a reviewer can see the
union that produced a verdict.

## Where the numbers land

| | before leads | after |
|---|---:|---:|
| classified | 155 | 154 |
| CANDIDATE | 57 | **57** (5 resolved by inspection, so **52 genuinely open**) |
| WRAPPER | 16 | 17 |
| FOLDED-MEMBER / WINDOW / UNROLLED / INDUCTION / INLINED | 82 | 80 |

CANDIDATE is unchanged in count but not in composition: the RW-math rows moved out to WRAPPER
and other rows moved in as the union stopped over-crediting. **52 rows (C3 36 / C4 21, minus
the 5 inspected) remain for hand review** - that is the real queue.

Nine rows still sit at size ratio < 0.5 (`TimerSlotTickDispatcher`, `LobbySlotListRender`,
`LogoOverlayTwin`, `HudStandingsRowUpdate`, ...). Depth 2 did not resolve them, so they are
either deeper delegation chains or genuinely partial ports. They are the highest-value slice
of the remaining queue and should be read first.

