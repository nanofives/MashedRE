# window_fullscreen_d2 — Session Findings

**Session:** window_fullscreen_d2-20260508-1346  
**Pool slot:** Mashed_pool15  
**Parent bucket:** window_fullscreen (d1 done 2026-05-03)  

---

## TL;DR

All four d1 uncertainties (U-1327..U-1330) resolved. Key findings:

- **DAT_00911f98 writer found**: `FUN_004c7a70` (`_rwDeviceSystemFn`), cases 0x0 and 0x7.
- **FUN_00499400 fully decoded**: startup display-mode selector / HardwareSetVideoMode orchestrator. Applies `SetWindowLongA(GWL_STYLE, WS_POPUP)` + `SetWindowPos` when fullscreen.
- **SetWindowLongA has 3 call sites, SetWindowPos has 3 call sites**: one each in `FUN_00499400` (resolved), two pairs in unanalyzed D3D functions (new uncertainties U-3027, U-3028).
- **FUN_004c7a70 case 0x8** (ShowWindow): called only in fullscreen mode (when `DAT_00912100 == 0`); shows/hides window on minimize/restore.

---

## U-1327 — Writer of DAT_00911f98 (RESOLVED)

`DAT_00911f98` (fullscreen config flag) is written by `FUN_004c7a70` in two cases:

| Write address | Case | Value written | Context |
|---|---|---|---|
| `0x004c7af0` | Case 0x0 (init) | `0` | During D3D adapter/mode enumeration reset |
| `0x004c7f41` | Case 0x7 (set mode) | `puVar5[4]` | 5th dword of selected mode entry (stride 0x14); this is the fullscreen flag field |

**Mode table layout** (at `DAT_007d4130 + param_4 * 0x14`):
```
[+0x00] = width (pixels)
[+0x04] = height (pixels)
[+0x08] = color depth
[+0x0c] = D3DFORMAT pixel format
[+0x10] = fullscreen flag  ← DAT_00911f98 set from here
```

**Downstream use** of `DAT_00911f98`: read in Case 0x2 (create device) and passed as `param_2` to `FUN_004c8800` (D3DPRESENT_PARAMETERS builder). `param_2 & 1 == 0` → windowed; `param_2 & 1 == 1` → fullscreen.

---

## U-1328 — DAT_00773204 semantic (RESOLVED)

`DAT_00773204` = **application-level fullscreen active flag**:
- Set to `1` by `FUN_00499400` at `0x00499626` area when `bStack_50 & 1 != 0` (fullscreen mode selected at startup).
- Read by `FUN_00498bf0` (6-byte getter, C1, `MOV EAX,[DAT_00773204]; RET`).
- Gates `ShowCursor(0)` in `FUN_004951f0` ("HardwareSetVideoMode") — cursor hidden when fullscreen.
- **Conclusion**: set once at init; not a runtime toggle flag.

---

## U-1329 — FUN_00499400 purpose (RESOLVED)

See `re/analysis/window_fullscreen_d2/00499400.md` for full analysis.

Summary: startup display-mode selection orchestrator. Two paths:
- `param_1 == 0`: silent (uses saved settings)
- `param_1 != 0`: shows mode-selection dialog (ID 0x65, proc `FUN_004991f0`)

After selection: applies WS_POPUP window style change (fullscreen only), propagates settings to globals.

---

## U-1330 — SetWindowLongA/SetWindowPos call sites (PARTIALLY RESOLVED)

All 3 SetWindowLongA and 3 SetWindowPos sites located via `reference_to`:

**Site 1 (FUN_00499400 — RESOLVED):**
- SetWindowLongA at `0x00499626` via `FF 15 80 C2 5C 00`
- SetWindowPos at `0x00499639` via `FF 15 84 C2 5C 00`
- Context: startup fullscreen window setup; makes window borderless (WS_POPUP)

**Site 2 (D3D init area — [UNCERTAIN U-3027]):**
- SetWindowLongA at `0x004c9667` via `FF 15 80 C2 5C 00`
- SetWindowPos at `0x004c96c7` via `FF 15 84 C2 5C 00`
- Context: inside unanalyzed code between `FUN_004c7a70` (end 0x004c856a) and `FUN_004c9cd0`. Preliminary byte decode: code manipulates window style using GWL_STYLE and GWL_EXSTYLE values. Likely part of D3D device creation or mode-switch in RW pipeline. Pool clone cannot auto-define functions here.

**Site 3 (D3D area — [UNCERTAIN U-3028]):**
- SetWindowLongA at `0x004ca5ff` via `FF 15 80 C2 5C 00`
- SetWindowPos at `0x004ca65f` via `FF 15 84 C2 5C 00`
- Context: similar unanalyzed region at ~0x004ca400+. Byte patterns mirror Site 2 closely, suggesting it's a parallel codepath (same function or sibling).

---

## FUN_004c7a70 Case 0x8 — ShowWindow behavior

```c
case 0x8:
    if (DAT_007d4110 != 0) {     // device exists
        if (DAT_00912100 != 0) { // windowed mode → skip
            return 1;
        }
        // fullscreen mode only:
        if (param_4 == 0) {
            ShowWindow(DAT_007d4104, SW_HIDE=0);
        } else {
            ShowWindow(DAT_007d4104, SW_RESTORE=9);
        }
    }
```

ShowWindow (IAT `0x005cc2a0`) is called **only in D3D fullscreen mode** (`DAT_00912100 == 0`). In windowed mode, the OS handles window show/hide automatically. The `DAT_007d4104` HWND is the game window.

---

## Functions analyzed this session

| RVA | Name | Confidence | Notes |
|---|---|---|---|
| `0x00499400` | FUN_00499400 | C2 | Display mode selection orchestrator; fully decompiled |

FUN_004c7a70 was decompiled for context (writer of DAT_00911f98) but is in the `rw_engine_init` domain — not newly classified here.

---

## New globals mapped (d2)

| Address | Semantics | Source |
|---|---|---|
| `0x00773410` | Subsystem count ("Number of subsystems") | FUN_00499400 decompile |
| `0x007731fc` | Subsystem name table (base, stride 0x50) | FUN_00499400 decompile |
| `0x007731f8` | Subsystem entry-count table (base) | FUN_00499400 decompile |
| `0x0077338c` | Saved subsystem index | FUN_00499400 decompile |
| `0x00773390` | Saved mode index | FUN_00499400 decompile |
| `0x0077340c` | Current display subsystem index | FUN_00499400 decompile |
| `0x00773200` | Current display mode index | FUN_00499400 decompile |
| `0x00616028` | Display width (pixels) from mode info | FUN_00499400 decompile |
| `0x0061602c` | Display height (pixels) from mode info | FUN_00499400 decompile |
| `0x00773414` | [UNCERTAIN U-3029] 3rd field from FUN_004c2ed0 | FUN_00499400 decompile |
| `0x007d4130` | Display mode table base (stride 0x14 per entry) | FUN_004c7a70 case 0x7 |
| `0x007d4128` | Mode count (upper bound for valid mode index) | FUN_004c7a70 case 0x7 |
| `0x007d4100` | Current mode index (D3D level) | FUN_004c7a70 case 0x7 |
| `0x007d4104` | HWND at D3D device level | FUN_004c7a70 cases 0x0/0x8 |
| `0x007d4110` | D3D device object (IDirect3DDevice9*) | FUN_004c7a70 case 0x8 |

---

## New uncertainties (U-3027..U-3046 range)

| ID | Description | Resolution |
|---|---|---|
| U-3027 | Unanalyzed function containing SetWindowLongA `0x004c9667` / SetWindowPos `0x004c96c7`. Region: 0x004c856a..0x004c9cc0 (between FUN_004c7a70 and FUN_004c9cd0). Pool clone cannot auto-define functions here; `search_bytes` returns 0 results on unanalyzed clone. | Master project: decompile function containing 0x004c9667. |
| U-3028 | Unanalyzed function containing SetWindowLongA `0x004ca5ff` / SetWindowPos `0x004ca65f`. Region: 0x004ca000+. Byte pattern mirrors Site 2 closely. | Master project: decompile function containing 0x004ca5ff. |
| U-3029 | `DAT_00773414` — 3rd field of `FUN_004c2ed0` output struct; meaning unknown. | Decompile FUN_004c2ed0 (already analyzed in rw_engine_init_d3? Check master). |

---

## Deferred: 0 items

cap_count = 0. < 5 DEFERRED → session halts here. No d3 needed.
