# Phase 6 Struct Extraction Report

**Session:** struct_extract_phase6_pt3 (2026-05-14, Session 96)
**Pool slot:** Mashed_pool7 (pre-assigned, read-only; no master Ghidra writes)
**Model:** Sonnet 4.6
**S-DoD requirement addressed:** #3 — struct docs consolidated across batches M/N/O evidence (30 Frida-verified C3 hooks + ~240 C2 plates)

---

## Summary

5 struct files produced (2 new, 3 amended). All 5 targets from the session prompt reached. 1 pre-existing U-ID resolved (U-3728). 8 new U-IDs filed.

---

## Files produced

| File | DAT_ root(s) | New content | Status |
|------|------------|-------------|--------|
| [camera_path_node.md](camera_path_node.md) | DAT_007d4104..7d458c / various | RwCamera +0x60/+0x64, D3D9 device globals cluster (30+ globals), D3D vtable slots (+0x68..+0x1ac), RwFreeList struct (9 fields) | AMENDED |
| [race_state_globals.md](race_state_globals.md) | 0x0063e4b8 / 0x0063a5e4 / 0x0076xxxx | Leaderboard entry array (stride 0x24), grid placement state (8 globals), 5 per-track reset arrays, race timer pair, per-slot activity flags | AMENDED |
| [boot_crt_globals.md](boot_crt_globals.md) | DAT_008ab6d0 / DAT_008aa688 / DAT_008aa580 | atexit table (U-3728 resolved), SBH region table (stride 0x14 + 5-field header), file handle blocks (stride 0x480/0x24), CRT security handler | AMENDED |
| [input_state.md](input_state.md) | DAT_007e95c0 / Lua_State | Controller config struct, device-type array, key-table ptrs, render config getters, Lua_State layout (11 offsets), TObject slot layout, tag-method data tables | NEW |
| [frontend_menu_state.md](frontend_menu_state.md) | DAT_0067ed40 / DAT_007f0a48 / DAT_00898ac0 | Menu cursor/limit arrays, entry validity, player slot struct (stride 0x1e0), menu entry array (52-byte stride, 30 entries), score arrays | NEW |

---

## Target-by-target outcome

### 1. camera_state (extend camera_path_node.md)

- **New batch_m evidence (render_frame + render_d3d9):**
  - RwCamera +0x60/+0x64: frame-buffer and z-buffer RwRaster ptrs (FUN_0042f530)
  - D3D9 device globals cluster: 30+ addresses at DAT_007d4104..7d458c (device ptr, D3D ptr, HWND, render-target table, constant-buffer pool, MSAA capability globals, D3DPRESENT staging)
  - D3D vtable slots: 11 new offset mappings (+0x68..+0x1ac) added to existing sub-table
  - RwFreeList struct: 9-field layout (+0x00..+0x20) with global chain head at DAT_007d45cc
- **S-DoD unblocked:** render_frame subsystem S-DoD #3 (substantially met; U-2027/2028/1247/1248 remain open)
- **U-IDs:** no new U-IDs; existing U-2027/U-2028/U-1247/U-1248 unchanged

### 2. race_state (extend race_state_globals.md)

- **New batch_n/o evidence:**
  - Leaderboard entry array: DAT_0063e4b8, stride 0x24, 4 entries (internal layout deferred as U-3829)
  - Race timer state pair: DAT_008991b0/b4 (zeroed at race init)
  - Grid placement state: 8 globals (rolling counter, frame-ID, position handle, modulo-12 counter, player count, per-player slot index array)
  - Per-track reset arrays: 5 arrays in 0x0076xxxx range (stride 0x38/0x24/0x20/0x24/0x30)
  - Per-slot activity flag bits (0x3c into split-screen viewport entry): 6 bit values documented
- **S-DoD unblocked:** race_state S-DoD #3 (substantially met; leaderboard entry internals and per-track array internals still open)
- **U-IDs filed:** U-3829 (leaderboard entry layout), U-3830..U-3834 (per-track reset array layouts)

### 3. input_state (new)

- **New input_lua_d2_cont1 + settings_config_d2_cont1 evidence:**
  - Controller config struct: 4-slot × 0x200-byte buffer at DAT_007e95c0; device-type array at DAT_007e96fc (stride 0x80)
  - Management globals: current slot (DAT_007730b0), HINSTANCE (DAT_007e9580), HWND (DAT_007e9584)
  - Key-table mapping: 6-entry pointer array (DAT_00773098), active table pointer (DAT_00773118)
  - Render config getters: DAT_00616028 (width), DAT_0061602c (height), DAT_00773414 (bit depth)
  - Lua_State struct: 11 documented offsets (+0x00..+0x6c) from input_lua_d2_cont1 plates
  - TObject stack slot: 3-field layout (type_tag +0, value_low +8, value_high +0xc), 16 bytes per slot
  - Tag-method data tables: 5 global arrays (settability table, event names, type names, nil-sentinel)
- **S-DoD unblocked:** input subsystem S-DoD #3 (substantially met; U-2589 joypad +0x00 conflict and U-3835 lua_State +0x6c still open)
- **U-IDs filed:** U-3835 (lua_State +0x6c role)

### 4. frontend_menu_state (new)

- **New batch_n Session 79 evidence (frontend_promote_menus_a/b + promote_c2_frontend_menus):**
  - Menu cursor/limit arrays: DAT_0067ed40/ed74, stride 0x40 per slot
  - Menu entry validity: DAT_0067ed84, stride 0x10 per slot (int8; 1 = valid)
  - Menu animation state: frame counter (DAT_0067f17c), race-end gate (DAT_0067ea90)
  - Im2D vertex render blocks: 4 blocks at DAT_0067ec30..ec84
  - Player slot struct: DAT_007f0a48, stride 0x1e0 (480 bytes); 3 dispatch-access patterns documented
  - Menu entry array: DAT_00898ac0, 52-byte stride, 30 entries; field-10 (+0x1c) preserved across bulk-clear
  - Score/rank arrays: group A (DAT_008a9500..950c, sentinel −1000), group B (DAT_008a9520..952c)
- **S-DoD unblocked:** frontend subsystem S-DoD #3 (substantially met; menu entry internals and player slot struct lower fields still open)
- **U-IDs filed:** U-3836 (menu entry 14-field layout), U-3837 (DAT_008a9510..951c gap), U-3838 (player slot struct lower fields)

### 5. boot_crt_globals (extend boot_crt_globals.md)

- **New promote_c2_boot_crt + boot_crt_exit_d3_cont1 evidence:**
  - **U-3728 RESOLVED:** atexit table at DAT_008ab6d0, write cursor at DAT_008ab6cc; growth strategy documented (doubles +0x800, fallback +0x10)
  - SBH extended: region table at DAT_008aa688 (stride 0x14); 5-field region header layout; 6 new SBH globals (DAT_008aa680/684/694/698 + per-group constants)
  - File handle blocks: DAT_008aa580 block-array; stride 0x480 per block = 32 slots × 0x24; per-slot CritSec at +0x0c
  - CRT security handler: DAT_007739f4 (non-NULL = custom handler)
- **S-DoD unblocked:** boot_crt_exit S-DoD #3 now **substantially met**
- **U-IDs resolved:** U-3728. U-3726 and U-3727 remain open.

---

## U-IDs filed this session

| ID | Type | Struct | Unknown |
|----|------|--------|---------|
| U-3829 | structural | race_state_globals | Leaderboard entry (DAT_0063e4b8, stride 0x24) — 36-byte entry internal field layout not characterized |
| U-3830 | structural | race_state_globals | Per-track reset array A (DAT_0076a100, stride 0x38, 256 entries) — internal layout not characterized |
| U-3831 | structural | race_state_globals | Per-track reset array B (DAT_00769f50, stride 0x24, 9 entries) — internal layout not characterized |
| U-3832 | structural | race_state_globals | Per-track reset array C (DAT_00766a00, stride 0x20, 40 entries) — internal layout not characterized |
| U-3833 | structural | race_state_globals | Per-track reset array D (DAT_00770718, stride 0x24, 60 entries) — internal layout not characterized |
| U-3834 | structural | race_state_globals | Per-track reset array E (DAT_00766f40, stride 0x30, 256 entries) — internal layout not characterized |
| U-3835 | structural | input_state (Lua_State) | +0x6c field — saved/restored across pcall boundary; exact role not determined from observed plates |
| U-3836 | structural | frontend_menu_state | Menu entry struct (DAT_00898ac0, 52-byte stride) — 14-field layout; only bulk-clear pattern observed; semantics unknown |
| U-3837 | structural | frontend_menu_state | DAT_008a9510..008a951c gap block (4 DWORDs) — not written in observed plates; relationship to adjacent score arrays unknown |
| U-3838 | structural | frontend_menu_state | Player slot struct (DAT_007f0a48, 0x1e0 stride) — lower-offset fields +0x00..+0x0b not characterized |

---

## U-IDs resolved this session

| ID | Resolution |
|----|-----------|
| U-3728 | atexit table at DAT_008ab6d0; write cursor at DAT_008ab6cc; confirmed by __onexit_lk at 0x004a407e |

---

## S-DoD impact summary (cumulative, Sessions 48 + 72 + 96)

| Subsystem | S-DoD #3 status |
|-----------|-----------------|
| race_state | **Substantially met** — trigger/timer/leaderboard/grid-placement globals; per-car 0x138 struct (U-1510/1511) and 5 reset-array internals still open |
| camera_follow | Substantially met (unchanged from session 72) |
| audio | Substantially met (unchanged from session 72) |
| rw_engine_init | Partially met (U-3029 still open) |
| input | **Substantially met** — controller config, Lua_State, TObject all documented; U-2589/U-3835 open |
| vehicle | Substantially met (unchanged from session 72) |
| hud/font | Substantially met (unchanged from session 72) |
| boot_crt_env | Substantially met (SBH region table extended) |
| boot_crt_exit | **Substantially met** — atexit table located (U-3728 resolved), file handle blocks and security handler documented |
| render_frame | **Substantially met** (new: D3D9 device globals, RwFreeList, 2 RwCamera fields added) |
| frontend/menus | **Substantially met** (new: menu cursor/limit/validity arrays, player slot struct, menu entry array) |

---

## No tracker mutations

Per session protocol: no hooks.csv mutations, no SCRIBE_QUEUE entry, no STUBS.md / UNCERTAINTIES.md / DEFERRED.md changes. This session is documentation-only.
