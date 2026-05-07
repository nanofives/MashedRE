# window_fullscreen — Session QQQ Findings

## TL;DR

Mashed uses a **fixed display mode configured at startup only**.  
No runtime FS_TOGGLE_FN exists. No RESIZE_FN exists.  
`SetWindowLongA` / `SetWindowPos` are imported and present but call sites were not locatable.

---

## Evidence for "no runtime toggle"

| Evidence | Address / source |
|---|---|
| `ChangeDisplaySettingsA` **not imported** | `external_imports_list` (177 entries) |
| WNDPROC handles exactly 4 messages (WM_CREATE, WM_DESTROY, WM_ACTIVATE, WM_KEYDOWN) | `re/analysis/window_msgpump/00499820.md` |
| No WM_SIZE, WM_ENTERSIZEMOVE, or WM_MOVE handler in WNDPROC | `re/analysis/window_msgpump/00499820.md` |
| No string anchors for "Fullscreen", "windowed", "WS_OVERLAPPED", "WS_POPUP" | `search_defined_strings` — 0 hits all queries |
| D3DPRESENT_PARAMETERS.Windowed value (`DAT_00912100`) is set once by `FUN_004c8800` during device init | `re/analysis/render_d3d9_device/0x004c8800.md` |
| The Reset path in `FUN_004c9cd0` reuses existing `DAT_009120e0` struct ��� does NOT re-invoke `FUN_004c8800` | `re/analysis/render_d3d_reset/004c9cd0.md` |

---

## Fullscreen configuration pipeline (init-only)

```
FUN_00493710 (RW_INIT_FN)
  └── "Calling HardwareSetVideoMode\n"
  └── FUN_004951f0 ("HardwareSetVideoMode")
        ├── FUN_00499400(DAT_006147bc)  → gates entry
        ├── FUN_00498bf0()              → returns DAT_00773204 (secondary FS flag)
        ├── ShowCursor(0)               → hides cursor if fullscreen (DAT_00773204 non-zero)
        └── FUN_004cbb60()             → returns device caps pointer → DAT_00771e58

  └── "Calling RwEngineOpen\n"
  └── FUN_004c30b0(param_1)            → RwEngineOpen; param_1 = HWND-bearing struct

  └── "Calling RwEngineStart\n"
  └── FUN_004c2fb0()                   → RwEngineStart
        └── FUN_004d8000(&DAT_00617fe0, DAT_007d3ff8)  → triggers _rwDeviceSystemFn dispatch

  └── _rwDeviceSystemFn (0x004c7a70, caseD_2 at 0x004c8185)
        ├── IDirect3D9::GetAdapterDisplayMode → result at 0x00911f84
        ├── zeroes 14 dwords at 0x009120e0 (D3DPRESENT_PARAMETERS)
        ├── reads DAT_00911f98  [fullscreen config flag — writer UNKNOWN, see U-1327]
        └── CALL FUN_004c8800(0x00911f84, DAT_00911f98, [ESP+0x2C])
```

`FUN_004c8800` (D3DPRESENT_PARAMETERS builder):
- `param_2 & 1 == 0` → windowed: `DAT_00912100 = 1`, `GetWindowRect` for dimensions
- `param_2 & 1 == 1` → fullscreen: `DAT_00912100 = 0`, reads width/height/refresh from param_1

---

## FS_TOGGLE_FN identification

**Not found. No candidate function identified.**

The display mode is fully determined by `DAT_00911f98` at startup. There is no mechanism to change it at runtime without restarting the RW engine chain.

---

## RESIZE_FN identification

**Not found.** The WNDPROC does not handle WM_SIZE. No discrete window-resize function discovered. The D3D9 reset path (`FUN_004c9cd0`) handles device loss, which is distinct from a user-driven resize.

---

## Functions analyzed this session

| RVA | Name | Size | Confidence | Notes |
|---|---|---|---|---|
| `0x00498bf0` | FUN_00498bf0 | 6 bytes | C1 | Trivial getter: `MOV EAX, [0x00773204]; RET`. Returns secondary fullscreen flag. |

`FUN_004c8800` was already C1 (render_d3d9_device bucket). No new C1+ functions added beyond `FUN_00498bf0`.

---

## Uncertainties

| ID | Description | Resolution path |
|---|---|---|
| U-1327 | Writer of `DAT_00911f98` not found. Pool clone search tools (search_bytes, search_instructions, search_constants) all return 0 results on unanalyzed clones. Call sites for SetWindowLongA (0x005cc280) / SetWindowPos (0x005cc284) / ShowWindow (0x005cc2a0) not located. | Re-run with master Ghidra project (Mashed.gpr) where analysis is complete; use search_constants on 0x00911f98. Or: raw memory scan for pattern `98 1f 91` in .text (0x00401000���0x005cafff). |
| U-1328 | `DAT_00773204` semantic meaning: is it a runtime "fullscreen currently active" flag, or just set at init? | Frida: hook FUN_004951f0, log DAT_00773204 value at entry; trace who writes it. |
| U-1329 | `FUN_00499400(DAT_006147bc)` — gates HardwareSetVideoMode. Return value 0 blocks the entire fullscreen setup. Purpose of DAT_006147bc and FUN_00499400 unknown. | Decompile FUN_00499400 from master project; examine DAT_006147bc. |
| U-1330 | Window created with WS_OVERLAPPEDWINDOW (0x00CF0000) in FUN_00499ba0. For D3D9 fullscreen the device takes over regardless of window style, but SetWindowLongA/SetWindowPos are imported — may be used to make window borderless. Their call sites not found. | See U-1327. |

---

## D3D9 reset path (nearest to RESIZE_FN)

`FUN_004c9cd0` (render_d3d_reset bucket, already C1):
- Calls `TestCooperativeLevel` → if `D3DERR_DEVICENOTRESET`: releases resources, calls `IDirect3DDevice9::Reset` with existing `&DAT_009120e0`
- Does NOT re-invoke `FUN_004c8800` → fullscreen/windowed flag is preserved across resets
- This is device-loss recovery, NOT a user-driven resize or toggle
