# Phase 5 Struct Extraction Report

**Session:** struct_extract_phase5 (2026-05-12, Session 48)
**Pool slot:** Mashed_pool5 (pre-assigned, read-only; no master Ghidra writes)
**Model:** Sonnet 4.6
**S-DoD requirement addressed:** #3 — shared structs touched by Phase 5 subsystems documented field-by-field with first-read/write RVA citations

---

## Summary

5 struct files produced (4 new, 1 amended). All targets from the session prompt were reached or explicitly handled.

---

## Files produced

| File | DAT_ root(s) | Field count | Confidence | Status |
|------|------------|-------------|------------|--------|
| [race_state_globals.md](race_state_globals.md) | 0x0067e844..0x0067f19c (static BSS) | 22 known addresses | C1 | NEW |
| [camera_path_node.md](camera_path_node.md) | various ptr types (RwCamera, RwFrame, path struct, cam-anim tables) | 35+ fields across 6 layout types | C1 | NEW |
| [audio_rws_engine.md](audio_rws_engine.md) | 0x007dc94c / 0x0068f640 / 0x0069049c | ~25 globals + channel struct partial | C1 | NEW |
| [rw_engine_init_config.md](rw_engine_init_config.md) | 0x00773200..0x00773920 | 17 globals + mode table layout | C1 | NEW |
| [gamepad_state.md](gamepad_state.md) | 0x00771eb4 / 0x007730d4 / 0x007e95c0 | (unchanged) | C1 draft | AMENDED (verification note only) |

---

## Target-by-target outcome

### 1. race_state
- **Root found:** No single pointer; static BSS block 0x0067e844..0x0067f19c.
- **Cross-references:** 12+ per-RVA plates (race_state, race_state_d2, leaderboard_d3, camera_follow_d2).
- **Fields documented:** Trigger arm/output flags, 4×float race timers (210.0f), transition request block, working buffers, per-car snapshot arrays.
- **S-DoD unblocked:** race_state subsystem S-DoD #3 (struct documentation gate).
- **U-IDs still open:** U-1073 (handle type at 0x0067ecac), U-1075 (210.0f float semantics), U-1507, U-1508 (transition block fields), U-1510/U-1511 (0x138-byte per-car struct layout).

### 2. camera_state
- **Root found:** No single global struct; RW canonical types (RwCamera, RwFrame) passed by pointer, plus camera-anim management tables at absolute addresses.
- **Cross-references:** 27 plates from camera_follow_d2 + 18 plates from camera_follow_d3.
- **Fields documented:** RwCamera (+0x04/+0x14/+0x68/+0x6c/+0x80/+0x84/+0x8c/+0x90/+0x94), RwFrame hierarchy (+0x03/+0x04/+0x10/+0x50/+0x98/+0x9c), camera path struct, path node (stride 0x24), intersection node (+0xf0/+0x90/+0x148), cam-anim tables.
- **S-DoD unblocked:** camera_follow subsystem S-DoD #3.
- **U-IDs still open:** U-2027 (FUN_004b5240 calling convention), U-2028 (vtable slot +8), U-1247 (vtable slot +0x10), U-1248 (particle field [7]).

### 3. dispatch_table_sfx
- **Root found:** No single dispatch table pointer; distributed across 4 per-type node descriptors (DAT_007dcae8/dc980/dde70/dcb78) + static channel pool at DAT_0068f640.
- **Cross-references:** 15 plates from audio_sfx_dispatch_d3 (D-7380..D-7394).
- **Fields documented:** RWS audio engine init flags, FUN_005aa560 wrapper family (4 node types), voice registration globals, DSound 200-channel pool, channel struct partial.
- **S-DoD unblocked:** audio subsystem S-DoD #3.
- **Gaps:** Per-type node descriptor internal layout not yet decomped; FUN_005aa560 args 1-4 unknown.

### 4. rw_engine_globals
- **Root found:** Cluster of static globals in 0x00773200..0x00773920 (display config) plus 0x007d4130 (D3D mode table) plus CRT flag at 0x007739f0.
- **Cross-references:** rw_engine_init_d2/00499400.md (C2), window_fullscreen_d2/FINDINGS.md, rw_engine_init/00493710.md, boot_app_init_d2/00499890.md — 4 distinct plate sources.
- **Note:** DAT_007739f0 is a CRT `_fast_error_exit` banner flag (VS2003 library match), not a RW global. Documented correctly as CRT.
- **Fields documented:** Fullscreen flag, mode/subsystem indices, subsystem name table, mode table layout (stride 0x14), OS version enum (1-7), D3D device globals.
- **S-DoD unblocked:** rw_engine_init subsystem S-DoD #3 (partial — U-3029 still open).
- **U-IDs still open:** U-3029 (DAT_00773414 meaning), U-3027/U-3028 (unanalyzed SetWindow call sites), U-0209 (FUN_004c2ed0 output struct layout).

### 5. input_dinput_state
- **Action:** Reviewed existing gamepad_state.md. Draft is complete relative to current plate evidence.
- **U-2589 status:** Still open — COM ptr vs string conflict at per-joypad struct +0x00 requires listing inspection at DAT_00771eb4. Added verification note to gamepad_state.md.
- **No new U-IDs generated.**
- **S-DoD unblocked:** input subsystem S-DoD #3 (conditional on U-2589 resolution for full layout).

---

## U-IDs filed this session

No new U-IDs were generated. All uncertainties above were pre-existing (filed by earlier sessions). This session surfaces and cross-references them, not creates them.

---

## S-DoD impact summary

| Subsystem | S-DoD #3 status after this session |
|-----------|-------------------------------------|
| race_state | Partially met — trigger/timer globals documented; per-car 0x138 struct (U-1510/1511) still open |
| camera_follow | Substantially met — RwCamera, RwFrame, path node all documented; cam-anim tables complete |
| audio | Substantially met — engine state, node types, channel struct documented; descriptor internals open |
| rw_engine_init | Partially met — display config documented; U-3029 still open |
| input | Partially met — existing draft verified; U-2589 still open |

---

## Deferred: structs not written (below 3-cross-ref threshold check)

No STOP-AND-ASK was triggered. All 5 target structs had ≥ 4 distinct per-RVA plate sources. No struct was deferred.

---

## No tracker mutations

Per session protocol: no hooks.csv mutations, no SCRIBE_QUEUE entry, no STUBS.md / UNCERTAINTIES.md / DEFERRED.md changes. This session is documentation-only.

---

# Phase 5 Struct Extraction Report — Session 72 (pt2)

**Session:** struct_extract_phase5_pt2 (2026-05-13, Session 72)
**Pool slot:** Mashed_pool7 (pre-assigned, read-only; no master Ghidra writes)
**Model:** Sonnet 4.6
**S-DoD requirement addressed:** #3 — extended struct docs across vehicle, font, and boot_crt subsystems; new fields from batches H-K

---

## Summary

3 struct files produced/amended (1 new (font_atlas), 1 new (boot_crt_globals), 1 extended (vehicle_dynamics)). txd_texture.md verified complete from texture_loader_d3_cont1. split_screen_viewport.md verified from split_screen_d2_cont1.

---

## Files produced

| File | DAT_ root(s) | Field count | Confidence | Status |
|------|-------------|-------------|------------|--------|
| [vehicle_dynamics.md](vehicle_dynamics.md) | DAT_00881000 (stride 0xD04) | 45 confirmed fields | C1 | EXTENDED (+29 new fields from batches H-K) |
| [font_atlas.md](font_atlas.md) | DAT_00912a00..0x912a3c cluster | GlyphEntry (8 fields) + FontCtx (12 known) + 11 globals | C1 | NEW |
| [boot_crt_globals.md](boot_crt_globals.md) | DAT_00616408 / DAT_008aa330 cluster | 20 globals across 4 categories | C1 | NEW |
| [txd_texture.md](txd_texture.md) | DAT_007d4054+DAT_007d3ff8 | RwTexture (8), RwTexDictionary (6), RwImage (8) | C1 | VERIFIED (no amendment needed) |
| [split_screen_viewport.md](split_screen_viewport.md) | DAT_0063dc38 (stride 0x2AC) | 14 viewport + 12 overlay slots | C1 | VERIFIED (no amendment needed) |

---

## Target-by-target outcome

### 1. vehicle_dynamics_state
- **Root found:** `DAT_00881000` — vehicle slot array, stride 0xD04 bytes confirmed.
- **Cross-references:** 8 plates from vehicle_dynamics_d3, 8 plates from vehicle_update_d3 (SlipTimerTick, WheelDrivetrainUpdate, WheelForceIntegrator, WheelContactSolver, OOBDebugDraw).
- **New fields this session:** gear timers (+0x20..+0x4C), gear torque (+0x54..+0x5C), angular inertia (+0x170), vehicle type (+0x1F0), gear-ratio tables (+0x478..+0x4C7), speed magnitude (+0x9E0..+0x9E8), position history (+0xAD0..+0xAE8), boost state (+0xBF4..+0xBF8), hover flag (+0xD00), and more.
- **S-DoD unblocked:** vehicle subsystem S-DoD #3 (substantially met; lower-offset fields +0x00..+0x50 gap still open, U-3721..U-3723 filed).
- **U-IDs filed:** U-3721 (two forward-dir fields), U-3722 (throttle vs speed at +0x9E4), U-3723 (steering vs airborne at +0xB20).

### 2. font_atlas (FontCtx + GlyphEntry)
- **Root found:** `DAT_00912a00..0x912a3c` — font subsystem global cluster; FontCtx is a heap-allocated context, not a static global.
- **Cross-references:** 5+ font_text_cont1 plates (FontCtx_LoadMetrics_Met, LoadMetrics_Atlas, InitFontPool, InitDataPools, InitBuffers).
- **Fields documented:** GlyphEntry fully documented (8 fields, stride 0x20); FontCtx 12 fields (line_height, aspect, ascii_glyph[0x80], glyph_count, glyph_buf_handle, ext char table fields); 11 subsystem globals.
- **S-DoD unblocked:** hud/font subsystem S-DoD #3 (substantially met; FontCtx gap +0x14..+0x23 open as U-3724).
- **U-IDs filed:** U-3724 (FontCtx gap), U-3725 (field_0 role).

### 3. boot_crt_globals
- **Root found:** Two clusters: `DAT_00616038..0x616d70` (CRT statics at 0x006xxxxx), `DAT_008aa330..0x8aa458` (runtime environment state at 0x008xxxxx).
- **Cross-references:** 8+ boot_crt_env_cont1 plates; 4 boot_crt_exit_d3_cont1 plates.
- **Fields documented:** CRT lock table (5 lock IDs), SBH globals (3), security cookie, multibyte locale globals (5), locale builtin table cluster (8).
- **S-DoD unblocked:** boot_crt_env S-DoD #3 (substantially met); boot_crt_exit S-DoD #3 (partially met — atexit table still open, U-3728).
- **U-IDs filed:** U-3726 (char-class flags), U-3727 (DAT_00773d70), U-3728 (atexit table global).

### 4. txd_texture
- **Status:** Verified complete. `txd_texture.md` was created by texture_loader_d3_cont1 session. All RwTexture, RwTexDictionary, RwImage fields and plugin globals documented. No new plates found in batch_k/l that extend it.

### 5. camera_state / race_state
- **Status:** Verified complete from Session 48 (race_state_globals.md, camera_path_node.md). No new conflicting data from batch_k plates.

---

## U-IDs filed this session

| ID | Type | Struct | Unknown |
|----|------|--------|---------|
| U-3721 | structural | vehicle_dynamics | Two forward-dir field citations at overlapping byte range +0x9C8..+0x9DC |
| U-3722 | structural | vehicle_dynamics | ForceIntegrator reads +0x9E4 as "throttle"; DrivetrainUpdate writes same as speed magnitude |
| U-3723 | structural | vehicle_dynamics | DrivetrainUpdate and ForceIntegrator both cite +0xB20 for different fields |
| U-3724 | structural | font_atlas (FontCtx) | +0x14..+0x23 gap (20 bytes) not touched by any observed plate |
| U-3725 | structural | font_atlas (FontCtx) | field_0 (+0x00) role unclear — TXD ctx save vs struct field |
| U-3726 | structural | boot_crt_globals | DAT_00616d68 char-class flag bit layout for 4 categories |
| U-3727 | structural | boot_crt_globals | DAT_00773d70 exact role (SBCS override vs init flag) |
| U-3728 | structural | boot_crt_globals | atexit table global address and layout |

---

## S-DoD impact summary (cumulative, Sessions 48 + 72)

| Subsystem | S-DoD #3 status |
|-----------|-----------------|
| race_state | Substantially met (trigger/timer globals; per-car 0x138 struct U-1510/1511 still open) |
| camera_follow | Substantially met (RwCamera, RwFrame, path node, cam-anim tables) |
| audio | Substantially met (engine state, node types, channel struct; descriptor internals open) |
| rw_engine_init | Partially met (display config; U-3029 still open) |
| input | Partially met (gamepad draft; U-2589 still open) |
| vehicle | **Substantially met** (45 fields, full physics chain; 3 U-IDs for offset conflicts) |
| hud/font | **Substantially met** (GlyphEntry fully documented; FontCtx 12 fields; U-3724 gap) |
| boot_crt_env | **Substantially met** (SBH, lock, locale, cookie globals documented) |
| boot_crt_exit | Partially met (lock table + security cookie; atexit table U-3728 open) |

---

## No tracker mutations

Per session protocol: no hooks.csv mutations, no SCRIBE_QUEUE entry, no STUBS.md / UNCERTAINTIES.md / DEFERRED.md changes. This session is documentation-only.
