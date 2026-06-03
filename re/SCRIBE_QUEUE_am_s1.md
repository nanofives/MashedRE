# SCRIBE_QUEUE fragment — batch_am session 1 (am_s1)

Author-only promote-c2 pass (gameplay campaign 2/~5, first slice 0x0041e140..0x0041efe0).
Bucket plates are the C2 deliverable; central finalize (ghidra-sweep + re-classify)
writes hooks.csv / trackers and commits.

## Queued

2026-06-03  am_s1  bucket=re/analysis/bucket_gameplay_0041e140_0041efe0  confidence=C1->C2  rvas=0041e140,0041e150,0041e170,0041e1c0,0041e1e0,0041e4b0,0041e8d0,0041e8e0,0041e900,0041e910,0041e920,0041e930,0041e940,0041e950,0041e9f0,0041ea00,0041ea20,0041ea30,0041ea40,0041ea50,0041ea60,0041ea70,0041ec00,0041ecc0,0041eeb0,0041efe0  note=subsystem_observed=gameplay (confirmed 24/26); RECOMMEND reclass gameplay->render for 0041e4b0 + 0041ecc0 (pure RpMaterial.color writes, no game-state mutation); accessor family 0041e8d0..0041ea70 (16) callers ALL in Frontend 0x00426xxx-0x00427xxx region (flagged frontend, KEPT gameplay pending object id); library_skip=NONE; needs_function_create=NONE; actual pool slot=Mashed_pool1

## Notes for the sweep

- **Count**: 26 RVAs, 26 plates authored in the bucket dir. **None drift-skipped** —
  all 26 were `gameplay/C1/mapped` in hooks.csv at session start (existing C1 plates
  in `re/analysis/bucket_0041e140/0x<rva>.md`; deepened, not restarted). All 26
  returned non-null `function_at` + `decomp_function`. Every plate uses the
  `## Mechanical description` heading and `confidence_target: C2`.

- **POOL SLOT**: pre-assigned **Mashed_pool1** was free (no top-level `.lock`/`.lock~`;
  shared MCP reported 0 open sessions). Opened read-only
  (`project_program_open_existing`, program `MASHED.exe`, image_base 0x00400000 so
  RVAs map 1:1), `program_close`d cleanly. Recorded in `.pool_slot_am_s1`.

- **SUBSYSTEM — bucket is genuinely MIXED but dominantly gameplay; three coherent
  groups:**

  1. **Lap-time / best-time comparison display widget (gameplay)** — a `this`-manager
     (offsets 0x1b4 state-code, 0x1b8 cached, 0x1c0 intro-frame, 0x1c4 input-pulse,
     0x1c8 record-loader-state, 0x188/18c/190 baseline triple), driven by
     `FUN_0041e6c0`:
     - `0041e150` triple→total-hundredths, `0041e170` total-hundredths→triple (exact
       inverses; pure int math), `0041e1c0` (__fastcall triple writer, 3rd field via
       CRT ftol `FUN_004a2c48`), `0041e1e0` (719-byte per-tick state machine: intro
       hold <0x51, baseline capture, record-table loader {0,1,2}, paced walk <0x47,
       input-event id 7 branch, 9:59:99 no-record sentinel→all-`-1`),
       `0041e4b0` (12-atomic RGBA palette swap on state change, see reclass below),
       and `0041e140` (getter `DAT_0063d7e0`, caller `FUN_00429e10`).
     - Saved-time slot accessors `FUN_00429a70/90/80` and record walkers
       `FUN_004115c0/00411580` and event poll `FUN_0040e350` are out-of-bucket callees
       (bare `[UNCERTAIN]` in plates, not minted).

  2. **Singleton-object accessor/dispatch family (0041e8d0..0041ea70, 16 fns)** — a
     matched pair-per-offset layer over the singleton `*(DAT_0063d7e4)`:
     - 8 **dispatch thunks** `mov ecx,[0x63d7e4]; jmp [ecx+OFF]` (tail-call the method
       at object-offset OFF) — 0041e8d0(+0x1c) e8e0(+0x20) e900(+0x28) e910(+0x2c)
       e920(+0x30) e930(+0x34) e940(+0x38) e950(+0x3c). Verified two-level via listing
       (`8b0de4d76300 / ff611c`), NOT `jmp [DAT+off]` as the C1 plates implied.
     - 8 **field getters** `mov eax,[0x63d7e4]; mov eax,[eax+OFF]; ret` — 0041e9f0..
       0041ea70 at the SAME offsets {0x1c,0x20,0x28,0x2c,0x30,0x34,0x38,0x3c}.
     - **Caller symmetry (verified each)**: thunk+getter for a given OFF share ONE
       dedicated wrapper in the Frontend region — +0x1c/+0x20→`FUN_004270f0`,
       +0x28→`FUN_00426b40`, +0x2c→`FUN_00426c30`, +0x30→`FUN_00426c10`,
       +0x34→`FUN_00426c70`, +0x38→`FUN_00426c50`, +0x3c→`FUN_00426640`. All callers
       in 0x00426xxx-0x00427xxx (Frontend/menu). **Flagged for frontend** but KEPT
       gameplay pending object identity (`*(DAT_0063d7e4)` type is `[UNCERTAIN]`; +0x24
       slot is not in this bucket).

  3. **Vehicle/race "slot" lifecycle (gameplay/vehicle, on the 0x2ac-stride 4-slot
     array `DAT_0063d9e0`)** — callers in 0x0040cxxx:
     - `0041ec00` slot teardown (destroy +0x18 RW handles for child subset
       {0x3b..0x3e,0x40} via `FUN_0053d090/0053d060`, `RpClumpDestroy` the +0x26c
       clump, memset 0x2ac, notify `FUN_00422a80`, free shared `DAT_0063d7e8` via
       `FUN_004c5930` when ref-count `DAT_0063e498`==1, clear per-slot flag, decrement
       ref-count). Callers `FUN_0040cf80`, `FUN_0041ffb0`.
     - `0041ecc0` two-color material recolor of the slot's atoms (see reclass below).
     - `0041eeb0` per-slot flags-dword (+0x294) bit arbiter — inputs set bits
       {0x40,0x80,0x800}, dominant flags {0x10,0x20,0x400} veto them. Caller
       `FUN_0040da50`.
     - `0041efe0` per-slot flags getter (bit 0x08). Caller `FUN_0040ca00`.

- **RECLASS RECOMMENDATIONS (gameplay -> render), 2 RVAs** — both are pure
  RW-material color writes with no game-state mutation:
  - `0041e4b0` — writes the state-indexed RGBA `DAT_0063d5f8[state]` into the
    `RpMaterial.color` (`atomic+0x18 -> +0x20 -> deref +4`) of 12 child atoms
    (offsets 0x108..0x12c then 0x178,0x174) on state change.
  - `0041ecc0` — sets `RpMaterial.color` (`FUN_005401d0`) on materials whose type
    probe `FUN_0053fea0`==4, two colors keyed by the {0..3,0x21..0x24} child subset.
  Both plates carry `subsystem_observed: render(material-recolor)` in frontmatter and
  a [UNCERTAIN] subsystem note. (Counter-argument for keeping gameplay: tight cohesion
  with their gameplay drivers — sweep's call.)

- **Named RW import resolved**: `0041ec00`'s raw `FUN_004e6e00` (C1 plate) is the
  master's **`RpClumpDestroy`** — recorded in the plate (call target, not a stub
  candidate).

- **C1 corrections folded in**:
  - Dispatch family is **two-level** (`*(DAT_0063d7e4)` then `+OFF`), not a single
    `[DAT+OFF]` indirection (verified via listing bytes).
  - `0041eeb0` 0x400-override clear mask `0xfffff73f` clears exactly {0x40,0x80,0x800}
    (complement 0x8c0) — the C1 plate wrongly added 0x100.
  - `0041e1e0` comparison operands are **swapped** between phase 4 and phase 5, so the
    {1,2} state-code polarity is context-dependent (noted [UNCERTAIN], not guessed).

- **Out-of-bucket callees referenced (NOT minted — sweep owns S-IDs)**: FUN_004a2c48
  (CRT ftol), FUN_00429a70/00429a90/00429a80 (saved-time slot accessors),
  FUN_004115c0/00411580 (record walkers), FUN_0040e350 (event poll), FUN_0053d090/
  0053d060 (RW destroy pair), FUN_004b6520 (zero-fill), FUN_00422a80 (notify),
  FUN_004c5930 (shared-resource free), FUN_0053fba0/0053fea0/005401d0/0053fbb0 (RW
  material/atomic API). Named RW import: RpClumpDestroy.

- **Uncertainties carried**: all data-semantic / object-identity (non-blocking for C2
  of these control-flow-complete reads). Filed as bare `[UNCERTAIN]` in the plates,
  NOT minted to U-IDs (author-only). Recurring open items for central minting:
  identity of `*(DAT_0063d7e4)` singleton + its method/field slots 0x1c..0x3c; the
  vehicle child-index subsets {0x3b..0x3e,0x40} and {0..3,0x21..0x24}; the per-slot
  flag-bit semantics (0x08 / 0x10 / 0x20 / 0x40 / 0x80 / 0x400 / 0x800); `DAT_0063d5f8`
  state-color table size/contents; `DAT_0063d7e0` semantic.

- **needs_function_create = NONE** (all 26 have function objects). **library_skip =
  NONE** — no bucket RVA decodes as a named library primitive (the slice is below the
  CRT / D3DX9-PSGP / qhull bands; `FUN_004a2c48` is an out-of-bucket CRT *callee*, not
  a bucket member).

- **No U-IDs / S-IDs / arg_types minted. No Frida, no build, no re-classify, no
  hooks.csv write.** Per author-only mission only the bucket dir, this fragment, and
  `.pool_slot_am_s1` (untracked session marker) were created/modified.
