# Frontend C3 gate-unblock plan (2026-05-26)

Captures the Ghidra decomp assessment of the gating C1 functions that block
frontend C3 candidates, so a future focused session can act without
re-decompiling. Done during the "focus on C3 promotion" goal.

## Delivered this session
- **0x0041f880** (gameplay) C1→C2 — flag→mode mapper; plate at
  `re/analysis/frontend_gate_unblock_u/0x0041f880.md`. Unblocked SlotFieldSet.
- **0x00422aa0 SlotFieldSet** (frontend) C2→C3 — impl + GREEN entity_field_set
  diff already existed (c3-batch-s-s1); caller-gate cleared by 0041f880 C2.
- **0x0042bfb0 MenuStateParamStore** (frontend) C2→C3 — c3_batch_u, new
  multi_arg_global_write arg_type.

## The 5 remaining easy gating functions (decompiled, C1→C2-ready)
All have clean mechanical decomp (transcripts captured in this session's
Ghidra read); each is a straightforward C1→C2 with a concise plate. None is
frontend (they're gameplay/render) — promoting them is *setup* that unblocks
the frontend dependents below.

| Gating fn | subsys | shape | unblocks (frontend dependent) | gate type |
|---|---|---|---|---|
| 0x00417450 | gameplay | pure-leaf grid-cell writer (x,y,val)→bool; sparse table DAT_007f1a9c idx + DAT_007f9a9c data, 0x80 stride | FrontendDirInput 0x00423040 | callee |
| 0x00417530 | gameplay | pure-leaf grid-cell eraser (x,y); same tables | FrontendDirInput 0x00423040 | callee |
| 0x004e6680 | render | list-walk RpClumpRender-ish; calls FUN_004c0ed0 + indirect vtable[2] | HandleArrayRelease 0x00426d90 | callee |
| 0x004e66d0 | render | RpClumpForAllAtomics (named); list-walk callback | 0x00425b90 | callee |
| 0x004516d0 | render | atomic-index dispatcher switch(0/1/2)→write param[1]+4/8/0xc; default printf "No such atomic" | 0x00425b90 | callee |

## The 3 hard gating functions (skip — too heavy for a gate-unblock)
- 0x00407e20 (~1700B FPU geometry) + 0x00408b00 (~1900B race-position FPU) —
  both callers of 0x00426cb0. Promoting either to C2 is a major effort.
- 0x0043dfd0 (>64k decomp, huge) — caller of 0x00494f30.

## Dependent C3 difficulty (after callee C2 promotions)
Even once gate-clear, the dependents are NOT easy C3:
- **FrontendDirInput 0x00423040**: impl exists (SlotZeroers_s1.cpp, install
  commented out). Diff needs an **injected-input arg_type** (idle menu = no
  buttons pressed → degenerate, same trap as the sort dispatcher 0x00424270).
  Must inject DAT_007f1042/1044-1047/1076 + menu_state + tab + repeat-timers
  DAT_006440ec.., call, read back timers + grid cells. Authorable but careful.
- **HandleArrayRelease 0x00426d90**: impl exists (MenuNearLeaves_s6.cpp).
  **Destructive** (frees a handle array via 004e6680) — synthetic diff unsafe;
  likely canonical-scenario only.
- **0x00425b90**: render/clump (RpClumpForAllAtomics callee) — live-state.

## Recommended next focused session (NOT a fanout)
1. Promote the 5 easy gating fns C1→C2 (concise plates; decomps in hand).
2. Author an `injected_input_observe` arg_type (generalizes state_machine_observe
   with grid-cell readback) and promote FrontendDirInput C2→C3 with
   button-pressed test vectors that exercise the branches.
3. Leave HandleArrayRelease + 0x00425b90 for canonical-scenario (runtime).

Net realistic C3 yield from the full chain: ~1-2 (FrontendDirInput), plus the
5 C2 promotions. The C3-per-effort ratio here is far below the early leaf
batches — confirms [[frontend-phase5-progress]]: the C3 lever needs upstream
Ghidra/scribe work and yields diminishing C3 returns.
