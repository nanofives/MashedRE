# Mashed RE orchestrator — resume point (updated 2026-07-27 03:45 UTC)

MISSION: dual-lane — (A) fix the game per RE_MASTER_PLAN, (B) promote Ghidra functions. Maximize account2.

## LANE A (fix)

- **Current plan item:** A2 — correct the boot-AV stability **overclaim** and measure the real
  survival window of the full 2402-hook `.asi`. (Chosen over B5e because memory records B5e as
  PORT COMPLETE + merged at `021a9f38`; the account2 brief calling B5e "the open next big lane"
  was **stale** — do not re-open it on that basis.)
- **State: DONE this phase.** Measured, not estimated. `scripts/observe_runtime.ps1 95`, plain
  launch, no Frida, hooks INSTALLED via the dinput8 proxy, 3 runs:
  - run1 **SURVIVED 96 s** (ended by our own harness timeout kill, `0xFFFFFFFF` — not a crash)
  - run2 **AV `0xC0000005` at t=74.4 s**
  - run3 **AV `0xC0000005` at t=71.7 s**
  => 2 of 3 AV in a tight 71.7–74.4 s window. The "survives 90 s ×3" claim is **FALSE** and stays
  retracted. The pc=0x44 boot AV **is** genuinely closed (all 3 runs held the menu 70+ s with the
  full hook set). The surviving defect is the separate flaky ~71 s AV.
  CHANGELOG row `2026-07-27 MEASUREMENT boot/runtime survival`. Logs `log/runtime_timeline.txt`.
- **Next concrete step:** root-cause the ~71 s AV. **Nothing is known about its cause yet** — no
  dump was parsed this session. Start dump-first:
  `py -3.12 scripts/parse_minidump.py` on the newest dump, then bisect. Memory
  `project_71s_av_open` says it pre-dates `c140d1d3` and is invisible to the 16 s bisect.
- **Not blocking promotions** — `run_diff` finishes far inside the window (both Lane-B diffs ran
  clean this phase).
- **Evidence/branch:** `main`, uncommitted (see below). No worktree used.

## LANE B (promote)

- **Active pipeline lane:** new `arg_type` handler (ROI #1). Yield **2/2 = 100%**, far above the
  ~30% bar.
- **DONE this phase: `0x004c4270` + `0x004c42d0` C2 → C3.**
  - Authored new handler **`st0_ret_mat3_ptr`** in `re/frida/diff_template.js` — pointer-seeded
    0x30 scratch, nine f32 at `{0x00,04,08, 0x10,14,18, 0x20,24,28}`, pads zeroed, ST0 captured as
    a 64-bit double fingerprint. `ret` MUST be `'double'`, never `void`.
  - Reimpl `mashedmod/src/mashed_re/Math/MatrixOrthoResidual.cpp`, verbatim inline `__asm`, wired
    into **both** `build.bat` and `asi_sources.rsp`. Both targets build clean.
  - `log/diff_mat3_ortho_residual_4c4270.csv` 10/10 bit-identical (6 distinct fingerprints);
    `log/diff_mat3_norm_residual_4c42d0.csv` 10/10 (9 distinct). Hook-BYPASSED synthetic A/B
    ⇒ **C3, NOT C4** (deliberately not claimed).
- **Major finding — a wrong label was propagating.** `frontier_shape_refinement_2026-07-24.md:27-29`
  labels `0x004c4270/42d0/4360` "RwV3d bbox Y/X/Z accessors". **All three retracted.** They are not
  accessors: 4270/42d0 take one pointer arg and compute an `M*transpose(M)==I` orthonormality error
  metric (4270 = off-diagonal/orthogonality, 42d0 = diagonal/normality vs the `1.0f` at
  `0x005cc320`). `re/PROMOTION_QUEUE.md:285` had the disproof **48 days before** that plan was
  written and the plan never reconciled it. Plan doc now carries an inline correction block.
  Full derivation: `re/analysis/render/0x004c4270_0x004c42d0_matrix_residuals.md`.
- **Next concrete step (pick one):**
  1. `0x004c4360` — U-9022. Different shape (`SUB ESP,0x18`, reads `+0x30/+0x34/+0x38`). Needs full
     disasm; Ghidra lift quality there is unverified. Would likely need its own handler.
  2. `FUN_004c4530` — the shared C2 caller. Decompiling it resolves **U-9021** and probably names
     the whole cluster, which may cascade several render rows.
  3. Sweep for other `st0_ret_mat3_ptr`-shaped leaves now that the handler exists — that is how the
     ROI-#1 lane pays off beyond the 2 rows already landed.

## OPEN GATES / STOP-AND-ASK

- **D2 renderer commitment** — OPEN. RW-subset verbatim (770 rows + 217 stubs) vs adopting `librw`.
  Confirm before sinking M3/WS-E tokens.
- **D4 airborne bit-identity** — OPEN. Accept the A5 airborne 1-ULP float10 residual (U-8991) as
  C4-grounded, or invest in a naked-asm float10 shim.
- D1 (Option A), D3 (MP out for v1.0), D5 (M1-first) resolved — not blocking.
- **New this phase — needs a decision:** U-9023. `FUN_005c47e0` calls these first-party render
  leaves yet sits inside `0x5c0000–0x5c8000`, the range memory `feedback_library_skip_bands` calls
  the MSVC CRT band. That band is used to auto-exclude promotion candidates, so if its bound is
  wrong it has been silently skipping first-party rows. Worth a cheap check.

## LOCKS / WORKTREES HELD

None. Ghidra `Mashed_pool0` closed and released; `.pool_slot` removed. No worktrees. No stray
MASHED processes (only the 3 this phase spawned, all exited).

## UNCOMMITTED STATE

**Everything below is uncommitted — the user has not authorized a commit.**

```
 M UNCERTAINTIES.md                                        (+U-9021, U-9022, U-9023)
 M hooks.csv                                               (2 rows C2->C3; row count 5896 unchanged)
 M mashedmod/asi_sources.rsp                               (+MatrixOrthoResidual.cpp)
 M mashedmod/build.bat                                     (+MatrixOrthoResidual.cpp)
 M re/analysis/CHANGELOG.md                                (2 new rows at head)
 M re/analysis/plans/frontier_shape_refinement_2026-07-24.md  (label retraction block)
 M re/frida/ARG_TYPES.md                                   (regenerated; 115 handlers)
 M re/frida/diff_template.js                               (+st0_ret_mat3_ptr handler)
 M re/frida/hooks_registry.py                              (+2 entries)
?? mashedmod/src/mashed_re/Math/MatrixOrthoResidual.cpp
?? re/analysis/render/
```

TO RESUME: paste this whole block into a new account3 session with the orchestrator prompt.
