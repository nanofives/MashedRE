# RW Engine Init Config Globals — 0x00773xxx / 0x00616xxx Range

**Session:** struct_extract_phase5 (2026-05-12)
**Evidence sources:** rw_engine_init_d2/00499400.md (C2), window_fullscreen_d2/FINDINGS.md, rw_engine_init/00493710.md (C1), boot_app_init_d2/00499890.md (C1), entry_callees/004a4b93.md (C1)
**Confidence:** C1 for most fields; [UNCERTAIN] noted per field

---

## Display mode / video config globals

Written during video mode selection (`FUN_00499400` — HardwareSetVideoMode orchestrator, C2).

| Address | Width | Type | Writer RVA | Tentative name |
|---------|-------|------|-----------|----------------|
| `0x00773200` | 4 | int32 | FUN_00499400 (LAB_00499590 path) | `g_currentDisplayModeIndex` — current video mode index; passed to `FUN_004c2f30` (RwEngineSetVideoMode) |
| `0x00773204` | 4 | int32 | FUN_00499400 `0x00499626` area | `g_fullscreenActive` — application-level fullscreen flag; set to 1 if fullscreen mode selected at startup; gates `ShowCursor(0)` in FUN_004951f0; read by `FUN_00498bf0` (6-byte getter) |
| `0x0077338c` | 4 | int32 | FUN_00499400 (saved on mode-select) | `g_savedSubsystemIndex` — saved subsystem selection (written before display dialog) |
| `0x00773390` | 4 | int32 | FUN_00499400 (saved on mode-select) | `g_savedModeIndex` — saved mode selection |
| `0x0077340c` | 4 | int32 | FUN_00499400 (from saved or dialog result) | `g_currentSubsystemIndex` — current display subsystem index; passed to `FUN_004c2e70` (RwEngineSetSubSystem) |
| `0x00773410` | 4 | int32 | Written before FUN_00499400 is called | `g_subsystemCount` — total subsystem count; logged as "Number of subsystems: %d" |
| `0x00773414` | 4 | int32 | FUN_004c2ed0 (GetModeInfo) writes 3rd field of output struct | [UNCERTAIN U-3029] — 3rd field of `FUN_004c2ed0` output struct; exact meaning unknown (possibly mode depth or refresh rate field) |

### Display mode table (from FUN_004c7a70 case 0x7 — `_rwDeviceSystemFn`)

| Address | Width | Notes |
|---------|-------|-------|
| `0x007d4130` | N×0x14 | Display mode table base; stride 0x14 (20 bytes) per entry |
| `0x007d4128` | 4 | Mode count (upper bound for valid mode index) |
| `0x007d4100` | 4 | Current mode index at D3D device level |

Per-entry layout (stride 0x14):

| Offset | Size | Notes |
|--------|------|-------|
| +0x00 | 4 | Width (pixels) |
| +0x04 | 4 | Height (pixels) |
| +0x08 | 4 | Color depth |
| +0x0c | 4 | D3DFORMAT pixel format |
| +0x10 | 4 | Fullscreen flag → written to `DAT_00911f98` |

---

## Subsystem name/count tables

| Address | Stride | Notes |
|---------|--------|-------|
| `0x007731fc` | 0x50 per entry | Subsystem name table; each entry is 80-byte string (logged by FUN_00499400) |
| `0x007731f8` | 4 per slot | Subsystem entry-count table; `*(0x007731f8 + i*4)` = mode count for subsystem i |

---

## Display resolution globals

Written by FUN_00499400 when `bStack_50 & 1 != 0` (fullscreen path), sourced from `FUN_004c2ed0` output:

| Address | Width | Notes |
|---------|-------|-------|
| `0x00616028` | 4 | Display width (pixels from mode info) |
| `0x0061602c` | 4 | Display height (pixels from mode info) |

---

## Video mode selection state (FUN_00499400 dialog path)

Written by both param_1==0 (silent) and param_1!=0 (dialog) paths:

| Address | Width | Notes |
|---------|-------|-------|
| `0x007733a4` | 4 | Written from DAT_007733a4; propagated to DAT_00636ad4 and DAT_00636ad0 |
| `0x007733a8` | 4 | Propagated to DAT_00636acc |
| `0x007733ac` | 4 | Propagated to DAT_005ea098 |
| `0x007733b0` | 4 | Propagated to DAT_00636ae0 and DAT_00636adc |

---

## Fullscreen / window config (from FUN_004c7a70 cases 0 / 7 / 8)

| Address | Width | Notes |
|---------|-------|-------|
| `0x00911f98` | 4 | Fullscreen config flag; written by `_rwDeviceSystemFn` case 0x7 (from mode table +0x10 field); read in case 0x2 (D3D device create), gates windowed vs fullscreen |
| `0x00912100` | 4 | Windowed mode flag at D3D level; `!= 0` means windowed; used in case 0x8 ShowWindow |
| `0x007d4104` | 4 | HWND — game window handle at D3D device level; used by ShowWindow in case 0x8 |
| `0x007d4110` | 4 | IDirect3DDevice9* or D3D device object; non-zero = device exists (case 0x8 guard) |

---

## RW engine state globals (from rw_engine_init/00493710.md)

Written by the RW engine init sequence in FUN_00493710:

| Address | Width | Notes |
|---------|-------|-------|
| `0x007719d8` | 4 | Return value of FUN_004cbc90 (identity/role unknown; written on RW engine start success path) |
| `0x007719dc` | 4 | Return value of FUN_004cbc70 (identity/role unknown; written on RW engine start success path) |

`FUN_004cbc90` and `FUN_004cbc70` are stubs S-0063 / S-0061; their return semantics are unknown.

---

## CRT runtime globals (0x00773xxx range, non-RW)

| Address | Notes |
|---------|-------|
| `0x007739f0` | CRT `_fast_error_exit` banner flag — if == 1, `_fast_error_exit` calls `__FF_MSGBANNER()` before exit. Ghidra library match: VS2003 Release `_fast_error_exit`. Not a RW global. |

---

## Cross-references

- [[rw_plugin_registry]] — covers `DAT_007d3ff8` (g_rwDevice engine state pointer, offsets +0x10/+0x18/+0x20/+0xCC/+0x124) and `DAT_00617fe0` (g_rwEnginePlugins registry)
- [[rwim2d_vertex_buffer]] — covers `DAT_007d3ff8` offsets +0x18/+0x20/+0x28/+0x30 for Im2D draw path

---

## Open uncertainties

| U-ID | Gap |
|------|-----|
| U-3029 | `DAT_00773414` — 3rd field of FUN_004c2ed0 output struct; exact meaning not determined. Resolution: decompile FUN_004c2ed0 (rw_engine_init bucket) |
| U-0209 | bStack_50 (byte at uStack_5c+0x50, from FUN_004c2ed0): bit 0 gates fullscreen window changes + DAT_00773204=1; full struct layout at uStack_5c not determined |
| — | DAT_007719d8 / DAT_007719dc: return values from FUN_004cbc90/cbc70 (stubs S-0063/S-0061); identity unknown |
| U-3027 | SetWindowLongA/SetWindowPos unanalyzed site at 0x004c9667 (D3D init area) |
| U-3028 | SetWindowLongA/SetWindowPos unanalyzed site at 0x004ca5ff (D3D area) |
