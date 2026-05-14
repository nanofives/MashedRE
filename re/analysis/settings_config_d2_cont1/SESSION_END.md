# SESSION_END — settings_config_d2_cont1
**Date:** 2026-05-13
**Session ID:** settings_config_d2_cont1-20260513
**Pool slot used:** Mashed_pool10
**Bucket:** re/analysis/settings_config_d2_cont1/

## Pre-flight

- SHA-256 MASHED.exe: `bdcae093a30fbf226bdd852b9c36798a987aee33b3ae82bf7404b0336efd3c0e` ✓ (via pool identity)
- No active lock conflicts

## DEFERRED resolution note

D-3580..D-3585 were present in `DEFERRED.md` tagged `settings_config_d2-cont1`. All 6 were already analyzed by prior sessions (rw_engine_init_d3 and render_promote_c2_rw_plugin) before this session ran. Marked RESOLVED in DEFERRED.md; no new per-RVA plates needed for them.

| D-ID | RVA | Resolution |
|------|-----|-----------|
| D-3580 | 0x004a504f | C1 in re/analysis/rw_engine_init_d2/004a504f.md |
| D-3581 | 0x004c2e40 | C2 in re/analysis/render_promote_c2_rw_plugin/0x004c2e40.md |
| D-3582 | 0x004c2f00 | C2 in re/analysis/render_promote_c2_rw_plugin/0x004c2f00.md |
| D-3583 | 0x004c2de0 | C2 in re/analysis/render_promote_c2_rw_plugin/0x004c2de0.md |
| D-3584 | 0x004c2e10 | C2 in re/analysis/render_promote_c2_rw_plugin/0x004c2e10.md |
| D-3585 | 0x004c2ea0 | C2 in re/analysis/render_promote_c2_rw_plugin/0x004c2ea0.md |

## Functions processed (14 new plates)

| RVA | Proposed name | Subsystem | Classification | Notes file |
|-----|---------------|-----------|---------------|-----------|
| 0x004982a0 | ControllerPlayerDialog | input | C1 | 004982a0.md |
| 0x00498510 | ControllerDialogInit | input | C1 | 00498510.md |
| 0x00498bc0 | GetRenderWidth | render | C1 | 00498bc0.md |
| 0x00498bd0 | GetRenderHeight | render | C1 | 00498bd0.md |
| 0x00498be0 | GetRenderDepth | render | C1 | 00498be0.md |
| 0x00499730 | GetSomeBufferPtr | boot | C1 | 00499730.md |
| 0x004997b0 | LoadAndLockResource | util | C1 | 004997b0.md |
| 0x00499890 | LogOSVersion | boot | C1 | 00499890.md |
| 0x00499ce0 | InitViewportMatrix | render | C1 | 00499ce0.md |
| 0x00497230 | SaveControllerConfig | input | C1 | 00497230.md |
| 0x004921d0 | DisplayModeInit | render | C1 | 004921d0.md |
| 0x00497000 | BuildProjMatrix | render | C1 | 00497000.md |
| 0x00497060 | BuildCameraMatrices | render | C1 | 00497060.md |
| 0x00497310 | ReadInputForAction | input | C1 | 00497310.md |

## Key findings

**Controller config subsystem (input):**
- `FUN_004982a0` (ControllerPlayerDialog): shows dialog 0x68 per player slot; guards on device-type; backs up and restores 0x200-byte slot config on cancel; calls FUN_00497230 on confirm. `DAT_007e96fc + slot*0x80` = device type, `DAT_007e95c0 + slot*0x80` = 0x200-byte binding struct.
- `FUN_00498510` (ControllerDialogInit): builds 6-pointer key-table at `DAT_00773098..007730ac` (pointing to code-section tables at stride 0x400); enumerates joypad slots, populates binding structs, optionally shows dialog 0x67. Language selection via `param_2` indexes the key-table.
- `FUN_00497230` (SaveControllerConfig): fwrite 0x200 bytes of `DAT_007e95c0+EAX*0x80` to `"contcfg%d.bin"`; EAX = implicit slot index.
- `FUN_00497310` (ReadInputForAction): two device paths (joypad/keyboard); joypad dispatches axis IDs 9-12 vs threshold `DAT_005d757c`, default path reads button bitmap `DAT_007730d4`; keyboard reads `DAT_0077313c` bitmap.

**Render dimension getters (render):**
Three 5-byte getters for the globals written by `FUN_00499400` on mode apply:
- `FUN_00498bc0` → `DAT_00616028` (width)
- `FUN_00498bd0` → `DAT_0061602c` (height)
- `FUN_00498be0` → `DAT_00773414` (depth)

**Display init and resource loading (render/util):**
- `FUN_004921d0` (DisplayModeInit): called after mode selection; gets dimensions via the three getters; calls FUN_00467110/0042b890/0042b8a0/0042d560/0042f660 for render system setup; loads embedded RWTEXDICTIONARY resource ID 0x194 → TXD handle at `DAT_0077195c`; finds "LoadIcon" texture → `DAT_00771960`.
- `FUN_004997b0` (LoadAndLockResource): generic Win32 FindResourceA+LoadResource+SizeofResource+LockResource wrapper.

**Projection matrix builder (render):**
- `FUN_00497000` (BuildProjMatrix): fills 16-entry output from 4 fields in an input struct (offsets +0x70, +0x74, +0x80, +0x84); entry [14]=1.0f; entries [10] and [11] use near/far division formula.
- `FUN_00497060` (BuildCameraMatrices): calls FUN_00496ec0+FUN_00497000+thunk_FUN_004ed6ad; copies 4×16-entry result blocks to output params.

**Boot logging (boot):**
- `FUN_00499890` (LogOSVersion): detects Win9x/NT/2000/XP; writes OS type code 1–7 to `DAT_00773920`; logs to mashed.log.
- `FUN_00499ce0` (InitViewportMatrix): zeros 12 floats + sets 4 to 1.0f at `DAT_00773928..00773964` (0x40 bytes); called from main boot function FUN_00402750.

## New DEFERRED

None. No callee recursion warranted — all unanalyzed callees are either Win32, CRT, or in other subsystems already tracked.

## New UNCERTAINTIES

U-3010..U-3024 filed. See UNCERTAINTIES.md.

Notable:
- U-3014: 0x40-byte block at DAT_00773928 (16 floats, pattern with 4 ones at positions 0/5/10/15) — semantic role and consumers not identified.
- U-3020: Input struct fields at param_2+0x70/+0x74/+0x80/+0x84 for BuildProjMatrix — semantic labels unconfirmed.

## New STUBS

None.

## Cross-subsystem notes

- `FUN_004921d0` bridges render (mode query, dimension setters) and util (resource loader). Does NOT overlap with `videocfg.bin` parser or `gamesave.bin` serialization.
- Controller config uses `"contcfg%d.bin"` files (separate from `videocfg.bin`). No overlap with save subsystem.

## Cap usage

14 functions processed. Target was 20; stopped at 14 due to natural boundary (all settings_config callees covered; fill exhausted adjacent unanalyzed area).

## SCRIBE_QUEUE entry

Queued in re/SCRIBE_QUEUE.md.
