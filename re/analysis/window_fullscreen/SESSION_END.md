# SESSION_END — window_fullscreen-20260503

## Slot held
Mashed_pool10 (pre-assigned, no lock file, opened read-only)

## FS_TOGGLE_FN identified?
**No.** Mashed uses a fixed display mode configured at startup only.
- No `ChangeDisplaySettingsA` import
- WNDPROC (0x00499820) handles only 4 messages; no WM_SIZE or toggle path
- No string anchors for "Fullscreen" / "windowed" / WS_* style constants
- D3DPRESENT_PARAMETERS.Windowed is set once at init via `DAT_00911f98`; not modified at runtime

## RESIZE_FN identified?
**No.** No WM_SIZE handler in WNDPROC. No discrete resize function found.
D3D9 reset path (`FUN_004c9cd0`) is device-loss recovery only, not user-driven resize.

## Anchor hits
- `SetWindowLongA` IAT confirmed at `0x005cc280`
- `SetWindowPos` IAT confirmed at `0x005cc284`
- `ShowWindow` IAT confirmed at `0x005cc2a0`
- `GetWindowRect` IAT confirmed at `0x005cc2a4`
- `AdjustWindowRect` IAT confirmed at `0x005cc260`
- `CreateWindowExA` IAT confirmed at `0x005cc264`
- **No** `ChangeDisplaySettingsA` import (checked exhaustively via external_imports_list)
- `D3D9.DLL::Direct3DCreate9` IAT at `0x005cc2e8` (only D3D9 import; device vtable calls are indirect)

## Functions covered

| RVA | Name | Confidence | Notes |
|---|---|---|---|
| `0x00498bf0` | FUN_00498bf0 | C1 | 6-byte getter: `return DAT_00773204` |
| `0x004c8800` | FUN_004c8800 | C1 (prior) | D3DPRESENT_PARAMETERS builder; re-cited as FS params source |

## New globals mapped

| Address | Semantics |
|---|---|
| `0x00911f98` | Fullscreen config flag (0=windowed, non-zero=fullscreen); read at `_rwDeviceSystemFn` caseD_2 |
| `0x00912100` | D3DPRESENT_PARAMETERS.Windowed (1=windowed, 0=fullscreen) |
| `0x009120e0` | D3DPRESENT_PARAMETERS struct base |
| `0x009120fc` | D3DPRESENT_PARAMETERS.hDeviceWindow |
| `0x007e9584` | HWND stored by `FUN_00499ba0` after `CreateWindowExA` |
| `0x00773204` | Secondary fullscreen flag (cursor-hide gate) |

## Tracker counts

- **New C1 functions**: 1 (`FUN_00498bf0`)
- **New uncertainties**: U-1327, U-1328, U-1329, U-1330
- **New stubs**: 0
- **DEFERRED (cap_count=0)**: 0 (no depth-2 functions reached)

## IDs used
- U-1327..1330 (4 of 20 slots consumed)
- D range: unused
- S range: unused

## cap_count
0 (standard loop; no deferral needed — FS_TOGGLE_FN not found, session halts by conclusion)

## MCP failures / observations
- `search_bytes`, `search_instructions`, `search_constants`, `function_list`, `decomp_function` all return 0 results on unanalyzed pool clones — confirmed as a systemic limitation of read-only pool clones without analysis imported from master
- `memory_read` works correctly; raw byte decoding used throughout
- `listing_code_units_list` returns raw `??` DataDB entries only (no instructions) in unanalyzed pool

## External user32/d3d9 imports noted
- USER32: SetWindowLongA, SetWindowPos, ShowWindow, GetWindowRect, AdjustWindowRect, CreateWindowExA, RegisterClassA, ShowCursor all confirmed present
- D3D9: only Direct3DCreate9 imported; IDirect3DDevice9 methods called via vtable
- **ChangeDisplaySettingsA: NOT imported** ← definitive

## Scribe outcome
Queued for sweep (window_fullscreen-20260503). Files written:
- `re/analysis/window_fullscreen/GLOBALS.md`
- `re/analysis/window_fullscreen/FINDINGS.md`
- `re/analysis/window_fullscreen/00498bf0.md`
- `re/analysis/window_fullscreen/SESSION_END.md`
