# window_fullscreen — Globals Map

Identified during session QQQ (window_fullscreen-20260503-XXXX).

| Global address | Type | Value semantics | Source |
|---|---|---|---|
| `0x00911f98` | `uint` | Fullscreen config flag: 0 = windowed, non-zero = fullscreen. Read by `_rwDeviceSystemFn` caseD_2 (0x004c8185) and passed as `param_2` to `FUN_004c8800`. | Inferred from `MOV ECX, [0x00911f98]` at 0x004c81c4-c9 (raw bytes `8b 0d 98 1f 91 00`) |
| `0x00912100` | `uint` | `D3DPRESENT_PARAMETERS.Windowed` (output of `FUN_004c8800`): 1 = windowed, 0 = fullscreen. | `FUN_004c8800` decompilation (session render_d3d9_device) |
| `0x009120e0` | struct | `D3DPRESENT_PARAMETERS` base address (14 dwords zeroed at caseD_2 entry). | `_rwDeviceSystemFn` bytes at 0x004c819b–0x004c81a0 (`MOV EDI, 0x009120e0; REP STOSD`) |
| `0x009120fc` | HWND | `D3DPRESENT_PARAMETERS.hDeviceWindow`: game window HWND. Written at 0x004c81c8 area. | Bytes `89 15 fc 20 91 00` decoded from caseD_2 |
| `0x007d4104` | HWND | Game window HWND (RW device level). Used by `FUN_004c8800` for `GetWindowRect`. | `FUN_004c8800` decompilation |
| `0x007e9584` | HWND | CreateWindowExA result stored here by `FUN_00499ba0` at 0x00499c97. | Bytes `a3 84 95 7e 00` in FUN_00499ba0 |
| `0x00773204` | `uint` | Secondary fullscreen flag (cursor-hide gate): non-zero gates `ShowCursor(0)` in `FUN_004951f0`. Returned by `FUN_00498bf0`. | `FUN_00498bf0` = `MOV EAX, [0x00773204]; RET` at 0x00498bf0–0x00498bf5 |

## Key IAT addresses (USER32.DLL — fullscreen-relevant)

All confirmed from prior analysis + WNDPROC note (PTR_DefWindowProcA_005cc268).

| Import | IAT address | Call pattern |
|---|---|---|
| `DefWindowProcA` | `0x005cc268` | `FF 15 68 C2 5C 00` |
| `SetWindowLongA` | `0x005cc280` | `FF 15 80 C2 5C 00` |
| `SetWindowPos` | `0x005cc284` | `FF 15 84 C2 5C 00` |
| `ShowWindow` | `0x005cc2a0` | `FF 15 A0 C2 5C 00` |
| `GetWindowRect` | `0x005cc2a4` | `FF 15 A4 C2 5C 00` |
| `AdjustWindowRect` | `0x005cc260` | `FF 15 60 C2 5C 00` |
| `CreateWindowExA` | `0x005cc264` | `FF 15 64 C2 5C 00` |
| `RegisterClassA` | `0x005cc25c` | `FF 15 5C C2 5C 00` |

Note: `search_bytes` / `search_instructions` are non-functional on unanalyzed pool clones.
Call sites for `SetWindowLongA` / `SetWindowPos` / `ShowWindow` not located — [UNCERTAIN U-1327].
