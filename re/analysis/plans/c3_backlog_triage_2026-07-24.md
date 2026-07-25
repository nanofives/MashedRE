# C3 backlog triage (2026-07-24)

**Sources:** `re/analysis/plans/callee_gate_cascade.tsv` (1,416 CLEARED rows, sorted small-first), `re/analysis/plans/callee_gate_cascade_2026-07-24.md` (method + Tier 1 breakdown), `re/analysis/plans/promote_frontier.tsv` (28 leaf rows), `hooks.csv`, `re/frida/ARG_TYPES.md`.

---

## Summary counts

| Bucket | Count | Basis |
|---|---|---|
| **Total candidates** | **1,444** | 1,416 cascade CLEARED + 28 leaf frontier |
| **Ready-now (existing arg_type covers the shape)** | **~287** | breakdown below |
| ÔåÆ read_global_u32 (cascade) | 168 | ÔåÆ `read_global` (diff_template.js:219, 96 uses) |
| ÔåÆ arg_getter (cascade, excl. 2 confirmed ST0) | ~100 | ÔåÆ various existing handlers; per-row selection |
| ÔåÆ const_return (cascade) | 10 | ÔåÆ early-window `const_return` (5 uses, early_window_leaf_diff.py) |
| ÔåÆ field_get_cdecl | 1 | ÔåÆ `ptr_arg_int_get` (diff_template.js, 8 uses) |
| ÔåÆ field_get_thiscall | 1 | ÔåÆ `thiscall_field_get` (diff_template.js, 4 uses) |
| ÔåÆ const_setter (1 of 2) | 1 | ÔåÆ `abs_ranges_setter` or `void_setter_observe`; FontSys_InitSeq excluded |
| ÔåÆ leaf frontier arg_getter rows | 5 | ÔåÆ per-row handler; shapes tractable |
| **Blocked ÔÇö ST0 float handler** | **10** | 5 confirmed (3 fsin float10, 2 Replay FPU), 3 uncertain shape-hint collisions, 2 cascade read_global_f32 beyond first page; see appendix |
| **Blocked ÔÇö runtime scenario** | **1** | FontSys_InitSeq (0x00552b60): REFUSED ├ù2; 7 alloc callees deadlock past 60 s at quiescent menu |
| **Tier 2 / other shape** | **1,143** | Not structurally blocked; each needs per-row arg_type authoring; excluded from ready-now count; yield unconfirmed until per-row analysis |

**CASCADE HAS NO CALLEE GATE.** Per `callee_gate_cascade_2026-07-24.md`: 0 first-party rows exist below C2; all 797 C1 rows are third-party library (RenderWare/CRT). Every CLEARED row is already callee-ready to diff now ÔÇö the only practical blocker is arg_type authoring and scenario availability.

---

## Ready-now queue

Ordered by promotion ease within each group (smallest body / most mechanical shape first).

### read_global_u32 ÔÇö Boot (17 total; 7 from first page of cascade TSV)

All use `read_global` arg_type. Sizes 13ÔÇô54 B. All callers chain through `0x00402a40`.

| RVA | Name | File | arg_type | Notes |
|---|---|---|---|---|
| 0041a3d0 | FUN_0041a3d0 | re/analysis/promote_c2_boot_lowrva/0x0041a3d0.md | `read_global` | 13 B; single-call shutdown wrapper, FUN_004e6e00(DAT_0063c620) |
| 0041c2c0 | FUN_0041c2c0 | re/analysis/promote_c2_boot_lowrva/0x0041c2c0.md | `read_global` | 13 B; shutdown wrapper, FUN_004e6e00(DAT_0063cdb4) |
| 0041da80 | FUN_0041da80 | re/analysis/promote_c2_boot_lowrva/0x0041da80.md | `read_global` | 13 B; shutdown wrapper, FUN_004e6e00(DAT_0063d57c) |
| 0041de70 | FUN_0041de70 | re/analysis/promote_c2_boot_lowrva/0x0041de70.md | `read_global` | 13 B; shutdown wrapper, FUN_004e6e00(DAT_0063d5e0) |
| 00425ed0 | FUN_00425ed0 | re/analysis/promote_c2_boot_lowrva/0x00425ed0.md | `read_global` | 13 B; releases Copters.txd handle via FUN_004c5930 |
| 00499cc0 | sub_00499cc0 | ? | `read_global` | 18 B; 1 caller at 0x00492370 |
| 00496ce0 | FUN_00496ce0 | ? | `read_global` | 27 B; 1 caller at 0x00402a40 |
| (10 more boot read_global_u32 rows in cascade TSV rows 710ÔÇô1428) | ÔÇö | ÔÇö | `read_global` | ÔÇö |

### read_global_u32 ÔÇö HUD (17 total; confirmed named functions)

| RVA | Name | File | arg_type | Notes |
|---|---|---|---|---|
| 00404820 | FUN_00404820 | re/analysis/boot_hud_promote_ae1/0x00404820.md | `read_global` | 13 B; free scene at DAT_00636c00 via FUN_004e6e00 |
| 005571c0 | FontSys_ShutdownFontPool | ? | `read_global` | 25 B; counterpart to FontSys_InitFontPool; U-5704 |
| 00557220 | FontSys_ShutdownDataPools | ? | `read_global` | 39 B |
| 00556ce0 | FontSys_ShutdownBuffers | ? | `read_global` | 54 B |
| 005571e0 | FontSys_InitFontPool | re/analysis/font_pools_frontend_ae6/0x005571e0.md | `read_global` | 49 B; 6-arg thunk_FUN_004cc820 call; U-5704 |
| 00557250 | FontSys_InitDataPools | ? | `read_global` | 83 B |
| 00556d20 | FontSys_InitBuffers | ? | `read_global` | 73 B |
| 004c57a0 | FontCtxMatrix_AllocInit | re/analysis/font_text_d2/font_text_d2-20260503.md | `read_global` | 83 B; allocs identity-matrix via vtable+0x118; U-1493 |
| 00413f50 | FUN_00413f50 | ? | `read_global` | 64 B; 1 caller FUN_0040bde0 |
| 00427620 | FontText_HudShutdown | ? | `read_global` | 94 B; 5 callees |
| (7 more hud read_global_u32 rows) | ÔÇö | ÔÇö | `read_global` | ÔÇö |

### read_global_u32 ÔÇö Render (70 total; smallest-first sample)

| RVA | Name | File | arg_type | Notes |
|---|---|---|---|---|
| 00454f80 | FUN_00454f80 | re/analysis/bucket_gameplay_00454130_00455fe0/00454f80.md | `read_global` | 22 B; reclassed gameplayÔåÆrender |
| 00491780 | FUN_00491780 | re/analysis/track_loader_d2/00491780.md | `read_global` | 23 B; guard DAT_00771534==0ÔåÆreturn 0; else FUN_00491590(); return 1; U-0911 |
| 004c7600 | FUN_004c7600 | re/analysis/render_4_c1_to_c2_s4/FUN_004c7600.md | `read_global` | 28 B; RW Image/Texture/Raster; batch-x-s3 |
| 004c7620 | FUN_004c7620 | re/analysis/render_4_c1_to_c2_s4/FUN_004c7620.md | `read_global` | 36 B; RW Image/Texture/Raster; batch-x-s3 |
| 00421560 | FUN_00421560 | re/analysis/bucket_gameplay_00421100_004223f0/0x00421560.md | `read_global` | 39 B; reclassed gameplayÔåÆrender; batch-w-s1 |
| 004219c0 | FUN_004219c0 | re/analysis/promote_c2_render_lowrva/004219c0.md | `read_global` | 42 B; copy *DAT_007d3ff8ÔåÆDAT_006403b0; loop FUN_00421720 4├ù; S-3131 |
| 00474db0 | FUN_00474db0 | re/analysis/bucket_00474d80/0x00474db0.md | `read_global` | 42 B; gfx-particle-emitter/decal pool; batch-w-s2; U-4391 |
| 004c7730 | FUN_004c7730 | re/analysis/render_4_c1_to_c2_s4/FUN_004c7730.md | `read_global` | 47 B; RW Image/Texture/Raster; reads param_1+0x60; batch-x-s3 |
| 004cb0b0 | FUN_004cb0b0 | re/analysis/bucket_004c4270/0x004cb0b0.md | `read_global` | 51 B; RW related; batch-x-s3 |
| 004cb0f0 | FUN_004cb0f0 | re/analysis/bucket_004c4270/0x004cb0f0.md | `read_global` | 51 B; RW related; batch-x-s3 |
| 004d38b0 | rwD3D8CheckTextureFormat | re/analysis/render_5_c1_to_c2_s5/004d38b0.md | `read_global` | 51 B; pure D3D9 CheckDeviceFormat capability query; batch-render-5-s5 |
| 004d52d0 | FUN_004d52d0 | re/analysis/render_6_c1_to_c2_s1/004d52d0.md | `read_global` | 51 B; RW pipeline object create via vtable+0x60; batch-render-6-s1 |
| 004d7c60 | FUN_004d7c60 | re/analysis/render_6_c1_to_c2_s3/004d7c60.md | `read_global` | 61 B; plugin type table allocator; U-5355 |
| 004891f0 | FUN_004891f0 | ? | `read_global` | 72 B; 1 caller 0x00410b30 |
| 00558470 | sub_00558470 | re/analysis/skeleton_prep_high_leverage/00558470.md | `read_global` | 70 B; teardown: 2├ù vtable-free dispatch; U-0088 |
| (55+ more render read_global_u32) | ÔÇö | ÔÇö | `read_global` | ÔÇö |

### read_global_u32 ÔÇö Particle, Util, Input, AI (11+11+~5+~3 rows)

| RVA | Name | Subsystem | Size | arg_type | Notes |
|---|---|---|---|---|---|
| 0048bc10 | FUN_0048bc10 | particle | 13 B | `read_global` | teardown; calls FUN_004c5a60(DAT_00722134); S-4100 |
| 0048fce0 | FUN_0048fce0 | particle | 39 B | `read_global` | 1 caller 0x0040bde0 |
| 0048fd10 | FUN_0048fd10 | particle | 39 B | `read_global` | 1 caller 0x0040bde0 |
| 0048fd40 | FUN_0048fd40 | particle | 39 B | `read_global` | 1 caller 0x0040bde0 |
| 0045d430 | FUN_0045d430 | util | 39 B | `read_global` | timer_d2; dual guard; U-1137 U-1138 |
| 005ab040 | FUN_005ab040 | util | 41 B | `read_global` | 5 callers at 0x005a6ea0 |
| 0047b880 | FUN_0047b880 | input | 25 B | `read_global` | 1 caller 0x0047b9b0 |
| 00495fe0 | FUN_00495fe0 | input | 42 B | `read_global` | 1 caller 0x004967e0 |
| 00423670 | FUN_00423670 | ai | 22 B | `read_global` | spline editor input handler (reclassed ai); 1 caller 0x00423b00 |
| 0040acd0 | FUN_0040acd0 | frontend | 18 B | `read_global` | 1 caller 0x0043dfd0 |

### arg_getter ÔÇö Audio (40 total; representative sample)

No arg_types are pre-registered for any of these; promoter must select from ARG_TYPES.md per row. Likely candidates: `ptr_arg_int_get`, `int_scalar`, `thiscall_field_get`, `out3_idx`.

| RVA | Name | File | arg_type | Notes |
|---|---|---|---|---|
| 005aabe0 | FUN_005aabe0 | re/analysis/bucket_audio_005a7b60_005ab620/005aabe0.md | `ptr_arg_int_get` | 24 B; ref-count trampoline: FUN_005adef0(&DAT_007dccf0,ÔÇª,param_1); U-3187 |
| 005ae380 | FUN_005ae380 | re/analysis/bucket_audio_005ab710_005af040/0x005ae380.md | ? | 24 B; RWA path + sequencer; batch-w-s6 |
| 005a6c90 | FUN_005a6c90 | re/analysis/bucket_audio_00465c10_005a7b50/005a6c90.md | ? | 25 B; audio stream + codec pipeline; batch-y-s6 |
| 005ad8b0 | FUN_005ad8b0 | re/analysis/bucket_audio_005ab710_005af040/0x005ad8b0.md | ? | 25 B; per-channel state + mixer callbacks; batch-x-s5 |
| 005ae7e0 | FUN_005ae7e0 | re/analysis/bucket_audio_005ab710_005af040/0x005ae7e0.md | ? | 30 B; RWA path + sequencer; batch-w-s6 |
| 005af300 | FUN_005af300 | re/analysis/bucket_audio_005af070_005b2190/0x005af300.md | ? | 30 B; 1 caller 0x005af2d0; batch-w-s6 |
| (34 more audio arg_getter) | ÔÇö | ÔÇö | ? | ÔÇö |

### arg_getter ÔÇö Render (29 total; notable rows)

| RVA | Name | File | arg_type | Notes |
|---|---|---|---|---|
| 004cbb50 | FUN_004cbb50 | re/analysis/render_4_c1_to_c2_s6/FUN_004cbb50.md | `ptr_arg_int_get` | 11 B; called with global address as arg; teardown helper; batch-x-s3 |
| 004cc4f0 | FUN_004cc4f0 | re/analysis/render_5_c1_to_c2_s1/FUN_004cc4f0.md | `int_scalar` | 19 B; RW chunk type validator: switch on type code, returns 1/0; pure leaf |
| 004cc400 caller gated | ÔÇö | ÔÇö | ÔÇö | FUN_004cc4f0 caller is `RwStreamReadChunkHeader` |
| 004b6570 | FUN_004b6570 | re/analysis/render_3_c1_to_c2_s5/FUN_004b6570.md | `ptr_arg_int_get` | 20 B; bool wrapper for PizOpenAndParse; batch-x-s2 |
| 0044b000 | FUN_0044b000 | re/analysis/bucket_gameplay_00422440_0044e070/0x0044b000.md | ? | 27 B; reclassed gameplayÔåÆrender; powerup-bezier-adjacent; batch-y-s1 |
| 004770a0 | FUN_004770a0 | ? | ? | 30 B; 19 callers at 0x00413f50 |
| 004e6680 | RpClumpRender | ? | `ptr_arg_int_get` | 65 B; RpClumpRender stub/wrapper; 12 callers |
| 004d8770 | RwStringGetSizeAligned | ? | `int_scalar` | 34 B; 2 callers; string size utility |
| 004e66d0 | RpClumpForAllAtomics | ? | ? | 57 B; 12 callers; forEach over atomics |
| (21 more render arg_getter) | ÔÇö | ÔÇö | ? | ÔÇö |

### arg_getter ÔÇö Util, Gameplay, Vehicle (9+9+5 rows)

| RVA | Name | File | arg_type | Notes |
|---|---|---|---|---|
| 00407600 | Player::GetPositionPtr | re/analysis/profile_career_d4/REPORT.md | `ptr_arg_int_get` | 29 B; returns float* for player[param_1*0x3B]+0x30; D-6474 |
| 0052bf20 | FUN_0052bf20 | ? | ? | 20 B; 2 callers at 0x0052a9c0 |
| 004764e0 | FUN_004764e0 | ? | ? | 16 B; 1 caller at 0x004764f0 |
| 0041f1e0 | FUN_0041f1e0 | re/analysis/util_c0_promote/0x0041f1e0.md | ? | 51 B; reads event table +4 ptr; copies 16 floats to param_3; matrix source |
| 0041f220 | FUN_0041f220 | ? | `ptr_arg_int_get` | 51 B; 2 callers; arg_getter pattern |
| 00474d60 | FUN_00474d60 | ? | ? | 28 B; vehicle; 11 callers |
| 00481750 | FUN_00481750 | ? | ? | 39 B; vehicle; 4 callers |
| 004b5190 | FUN_004b5190 | ? | ? | 49 B; vehicle; 13 callers |

### const_return ÔÇö (10 total; early-window lane)

Driven by `early_window_leaf_diff.py`, NOT `diff_template.js`. Evidence tag: `green-earlywindow-rN`.

| RVA | Name | File | arg_type | Notes |
|---|---|---|---|---|
| 004952f0 | ClosePizFile | re/analysis/promote_c2_piz_loader/004952f0.md | `const_return` | 84 B; piz close wrapper; 10 callers |
| 00477730 | FUN_00477730 | ? | `const_return` | 48 B; render; 1 caller 0x0040bd80 |
| 004194f0 | FUN_004194f0 | ? | `const_return` | 51 B; gameplay; 1 caller 0x00418990 |
| (7 more const_return rows) | ÔÇö | ÔÇö | `const_return` | sizes 5ÔÇô84 B range |

### field_get + const_setter (ready subset)

| RVA | Name | Subsystem | Size | arg_type | Notes |
|---|---|---|---|---|---|
| 005a6c60 | FUN_005a6c60 | audio | 45 B | `ptr_arg_int_get` | field_get_cdecl; audio stream + codec pipeline; batch-y-s6; no arg registered |
| 0049f030 | FUN_0049f030 | particle | 67 B | `thiscall_field_get` | field_get_thiscall; E_NOTIMPL if +0x44==NULL; vtable dispatch at [+0xc]; U-4157 S-4279 |
| 005a60b0 | FUN_005a60b0 | audio | 36 B | `abs_ranges_setter` | const_setter shape; audio enable+start: DAT_7DC95C=1 + 4 call chain; D-10371; check D-10371 scope before scheduling |

### Leaf frontier arg_getter rows (5 potentially ready)

Large bodies (91ÔÇô247 B); predict lower yield than cascade Tier 1. Per-row arg_type authoring required.

| RVA | Name | Subsystem | Size | arg_type | Notes |
|---|---|---|---|---|---|
| 004c4270 | FUN_004c4270 | render | 91 B | ? | 2 callers at 0x004c4530 |
| 004cbbd0 | FUN_004cbbd0 | render | 130 B | ? | 1 caller at 0x004ec130 |
| 004c42d0 | FUN_004c42d0 | render | 137 B | ? | 2 callers at 0x004c4530 |
| 0045c550 | FUN_0045c550 | gameplay | 232 B | ? | 1 caller at 0x0045cb20 |
| 004b4550 | FUN_004b4550 | util | 247 B | ? | 4 callers at 0x0044e0a0 |

---

## Proposed promote-round batches

Each batch is one focused `/promote-round` session. Minimum predicted yield ÔëÑ 30% (all batches here are ÔëÑ 80% for Tier 1).

### Batch 1 ÔÇö Boot + HUD read_global_u32 (12 hooks)
**Rationale:** All use `read_global` arg_type, smallest bodies (13ÔÇô94 B), zero arg_type authoring ambiguity, two subsystems that actively run at quiescent menu.

Candidates: `0041a3d0`, `0041c2c0`, `0041da80`, `0041de70`, `00425ed0`, `00499cc0`, `00496ce0` (boot ├ù7) + `00404820`, `005571c0`, `00557220`, `005571e0`, `00556ce0` (hud ├ù5).

Predicted yield: ~95%.

### Batch 2 ÔÇö Render read_global_u32 ÔÇö RW glue (12 hooks)
**Rationale:** Named RW functions and smallest render `read_global_u32` bodies; all have mapped analysis files; RW reads are quiescent-menu stable.

Candidates: `00454f80`, `00491780`, `004c7600`, `004c7620`, `004c7730`, `004219c0`, `00474db0`, `004cb0b0`, `004cb0f0`, `004d38b0`, `004d52d0`, `004d7c60`.

Predicted yield: ~90%.

### Batch 3 ÔÇö Cross-subsystem read_global_u32 (12 hooks)
**Rationale:** Drains particle (4 of 11), util (2 of 11), input (2 of ~5), AI (1), frontend (1), render residue (2). Same arg_type throughout.

Candidates: `0048bc10`, `0048fce0`, `0048fd10`, `0048fd40` (particle) + `0045d430`, `005ab040` (util) + `0047b880`, `00495fe0` (input) + `00423670` (ai) + `0040acd0` (frontend) + `00421560`, `00558470` (render).

Predicted yield: ~90%.

### Batch 4 ÔÇö Audio arg_getter sweep (10 hooks)
**Rationale:** 40 audio arg_getter rows are the single largest arg_getter subsystem; they cluster in analysis buckets (batch-w-s6, batch-x-s5, batch-y-s6). Per-row handler selection needed but within a homogeneous subsystem so overhead is low.

Candidates: `005aabe0`, `005ae380`, `005a6c90`, `005ad8b0`, `005ae7e0`, `005af300`, `005a6c60` (field_get_cdecl), `005a60b0` (const_setter ÔÇö verify D-10371 first) + 2 more from the audio arg_getter list (to be picked by promoter from bucket-y-s6 or bucket-w-s6).

Predicted yield: ~75% (arg_type authoring variance; lower for const_setter and field_get).

### Batch 5 ÔÇö const_return (early-window lane) + particle/render field_get (12 hooks)
**Rationale:** Different harness (`early_window_leaf_diff.py`); all 10 const_return rows fit in one session. Add `0049f030` (thiscall_field_get) and `004952f0` (ClosePizFile, also const_return) to round out.

Candidates: All 10 const_return rows (RVAs to be pulled from cascade TSV rows 710ÔÇô1428; `004952f0` and `00477730` and `004194f0` confirmed from first page) + `0049f030` (thiscall_field_get).

Predicted yield: ~90%.

### Batch 6 ÔÇö Render + Util arg_getter (12 hooks)
**Rationale:** 29 render + 9 util arg_getter rows; pick smallest bodies with the most mechanical shapes. Higher variance session ÔÇö allocate extra time for per-row handler authoring.

Candidates: `004cbb50`, `004cc4f0`, `004b6570`, `0044b000`, `004770a0`, `004e6680`, `004d8770` (render) + `00407600`, `0052bf20`, `004764e0`, `0041f220`, `004b5190` (util/vehicle).

Predicted yield: ~70% (handler authoring ambiguity on the larger and less mechanical rows).

---

## Blocked appendix ÔÇö ST0-gated rows

These unblock together once either `fpu_st0_input` (for FPU input args) or `read_global_f32` (for float returns) is added to diff_template.js. The fsin float10 trio may additionally need the `float_2ptr_ret` early-window lane or a new `read_global_f32` early-window arg_type.

### Confirmed ST0-blocked (hooks.csv REFUSED annotations)

| RVA | Name | Subsystem | Shape | Blocker | Notes |
|---|---|---|---|---|---|
| 00411350 | Replay::TimeFormat | vehicle | arg_getter | `fpu_st0_input` missing | Prior REFUSED-promote: "param_1 raw_ticks pushed as float; decompiler extraout_ST0 confirms FPU read; unblock: add fpu_st0_input arg_type" |
| 00411530 | Replay::GetTimeAtIdx | vehicle | arg_getter | `fpu_st0_input` + missing multi-out-ptr shape | Prior REFUSED-promote: "5-arg shape (int int ptr ptr ptr) + implicit-ST0 float pass-through to TimeFormat; unblock: add three_out_ptr + resolve TimeFormat FPU" |
| 00431b20 | FUN_00431b20 | audio | read_global_f32 | float10 ST0 return | fsin(DAT_007f0f00 ├ù _DAT_005cd8f0) returned as x87 float10 (80-bit extended precision); U-2211 |
| 00431b50 | FUN_00431b50 | util | read_global_f32 | float10 ST0 return | fsin(DAT_007f0f04 ├ù _DAT_005cd8f0) as float10; U-1618 |
| 00431b60 | FUN_00431b60 | util | read_global_f32 | float10 ST0 return | fsin(DAT_007f0f08 ├ù _DAT_005cd8f0) as float10; sibling +4 of 00431b50 |

### Uncertain ÔÇö shape_hint `read_global_f32` but notes inconsistent

These need a Ghidra confirm before scheduling. The cascade generator may have flagged them on an intermediate FLD rather than a true float return.

| RVA | Name | Subsystem | Notes |
|---|---|---|---|
| 00417370 | FUN_00417370 | gameplay | Shape: read_global_f32 (44 B); hooks.csv notes describe a broad vehicle-effects bucket ÔÇö specific function purpose unconfirmed; may or may not return ST0 |
| 00442a20 | Camera::InitWithMatrix | util | Shape: read_global_f32 (51 B); hooks.csv notes: matrix copy DAT_0089650c ÔåÆ *(*(+0x84)+4)+0x10; multiple operations, no explicit ST0 return noted ÔÇö [UNCERTAIN] |
| 0048b650 | FUN_0048b650 | particle (leaf frontier) | Shape: read_global_f32 (355 B); hooks.csv notes: "shockwave-pool 2-arg slot alloc+write (64-slot scan+evict)" ÔÇö description contradicts read_global_f32 shape; [UNCERTAIN] |

### Blocked by runtime scenario (not ST0)

| RVA | Name | Subsystem | Blocker |
|---|---|---|---|
| 00552b60 | FontSys_InitSeq | hud | REFUSED ├ù2 (TIMEOUT); 7 alloc/init callees deadlock at quiescent menu within 60 s; impl already committed FontCtx.cpp:494; re-evaluate when FontSys_AllocObjects is reachable past stub state |

### Cascade `read_global_f32` rows not yet read (4 of 6 total)

Four cascade `read_global_f32` rows fall in TSV lines 710ÔÇô1,428 (not yet paged). All are candidate ST0-blocked rows. Pull them from the TSV before scheduling: `grep 'read_global_f32' re/analysis/plans/callee_gate_cascade.tsv` will list all 6 including the 2 confirmed above.

---

**Bottom line:** Schedule Batches 1ÔÇô3 first (all `read_global`, zero handler-authoring variance, ~95-90% predicted yield). Batch 5 (const_return early-window) can run in parallel on a second session since it uses a different harness. Batches 4 and 6 require per-row handler selection ÔÇö run them after the pure `read_global` rows are exhausted. Tier 2 (`other`-shape, 1,125+ rows) is the long tail; begin per-row triaging only after the ~288 Tier 1 rows are cleared or stalled.
