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
