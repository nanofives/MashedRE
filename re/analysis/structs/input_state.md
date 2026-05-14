# Input State — Controller Config and Lua Binding Layer

**Session:** struct_extract_phase6_pt3 (2026-05-14, Session 96)
**Evidence sources:** re/analysis/input_lua_d2_cont1/ plates, re/analysis/settings_config_d2_cont1/ plates, re/analysis/input_dinput_d3/ plates
**Confidence:** C1 — fields mechanically observed from decompilation; no Frida confirmation
**Related struct:** re/analysis/structs/gamepad_state.md (covers per-joypad device struct, axis calibration, keyboard state)

---

## Controller Config Struct (DAT_007e95c0 cluster)

4-slot controller config block written to/from `contcfg%d.bin` (pattern from FUN_00497190, `DAT_007730e4`).

### Global config management

| Address | Type | Tentative name | Notes |
|---------|------|----------------|-------|
| `DAT_007e95c0` | byte[0x200] | `g_controllerCfgBase` | Config buffer base; 4 slots × 0x200 bytes each (total 0x800 bytes) |
| `DAT_007e96fc` | int[4] | `g_deviceTypeArray` | Device type per slot; indexed as `[param_1 × 0x80]`; 0=no device, 1=joypad, 2=keyboard; first cite settings_config_d2_cont1/004982a0.md |
| `DAT_007e96c8` | ptr | `g_joystickSlotEnumBase` | Joypad slot enumeration array base; first cite settings_config_d2_cont1/00498510.md |
| `DAT_007730b0` | int | `g_configCurrentSlot` | Current player slot being configured; first cite settings_config_d2_cont1/004982a0.md |
| `DAT_007e9580` | HINSTANCE | `g_hInstance` | HINSTANCE global; first cite input_dinput_d3/00499720.md |
| `DAT_007e9584` | HWND | `g_hwndInput` | HWND for DirectInput; first cite input_dinput_d3/00499720.md |

### Key-table mapping

| Address | Stride | Count | Tentative name | Notes |
|---------|--------|-------|----------------|-------|
| `DAT_00773098` | 4 | 6 | `g_keyTablePtrArray` | Key-table pointer array; 6 entries, one per supported language; first cite settings_config_d2_cont1/00498510.md |
| `DAT_00773118` | 4 | 1 | `g_keyTableActive` | Selected (current-language) key-table pointer; first cite settings_config_d2_cont1/00498510.md |

### String / display buffers

| Address | Size | Notes |
|---------|------|-------|
| `DAT_007730b4` | 32 bytes | Resource label string buffer (slot A); first cite settings_config_d2_cont1/00498510.md |
| `DAT_007730f8` | 32 bytes | Resource label string buffer (slot B); first cite settings_config_d2_cont1/00498510.md |

### Axis threshold

`DAT_005d757c` — float threshold for axis IDs 9..12 (joypad trigger/analog range boundary); cited by settings_config_d2_cont1/SESSION_END.md.

---

## Render Config Getters (settings_config_d2_cont1 evidence)

Three getter functions directly read these globals for display-mode queries:

| Address | Getter RVA | Tentative name | Notes |
|---------|-----------|----------------|-------|
| `DAT_00616028` | FUN_00498bc0 | `g_renderWidth` | Current render width |
| `DAT_0061602c` | FUN_00498bd0 | `g_renderHeight` | Current render height |
| `DAT_00773414` | FUN_00498be0 | `g_renderBitDepth` | Current render bit depth |

Viewport identity matrix (16 floats): `DAT_00773928..0x773964`. Positions 0, 5, 10, 15 = 1.0f (0x3f800000); all other positions = 0. Written in FUN_00499ce0.

TXD handles: `DAT_0077195c` (RW texture dictionary resource ID 0x194), `DAT_00771960` (LoadIcon texture handle).

---

## Lua_State Struct Layout (input_lua_d2_cont1 evidence)

Lua 5.x internal state struct (`lua_State`), passed as `param_1` to all binding functions.

| Offset | Width | Tentative name | Evidence |
|--------|-------|----------------|----------|
| +0x00 | 4 | `stack_top` | Stack top pointer; dereferenced directly in FUN_004b7140, FUN_004b7250 |
| +0x04 | 4 | `stack_base` | Stack base pointer; observed as param_1[1] in FUN_004b7570 |
| +0x08 | 4 | `stack_last` | Stack upper boundary; param_1[2] in FUN_004b7570 |
| +0x0c | 4 | `max_stack` | Max stack depth; param_1[3] in FUN_004b7570 |
| +0x10 | 4 | `errorJmp_field` | Error jump field; param_1[4] in FUN_004b7be0 |
| +0x14 | 4 | `errorJmp` | `jmp_buf` chain; param_1[5] in FUN_004b7be0 |
| +0x30 | 4 | `tagTypeCountUpperBound` | Upper bound for tag-type count; `param_1[0x4c]` in FUN_004b9600 |
| +0x44 | 4 | `globalTablePtr` | Global environment table pointer; param_1[0x11] in FUN_004b8340 |
| +0x48 | 4 | `tagMethodArrayBase` | Tag-method array base pointer; param_1[0x12] in FUN_004b9730 |
| +0x6c | 4 | [UNCERTAIN U-3835] | Saved/restored in FUN_004b7be0 across pcall; param_1[0x1b]; exact role unknown |

Default error handler: `DAT_00617380` (Lua chunk loader fallback).

---

## TObject Stack Slot Layout (16 bytes per slot)

Lua stack slots (TObject structs) as written by FUN_004b7140 (push number) and FUN_004b7200 (push string/bool).

| Offset | Width | Tentative name | Notes |
|--------|-------|----------------|-------|
| +0x00 | 4 | `type_tag` | Object type discriminant; FUN_004b7140 writes type=2 (number) |
| +0x08 | 4 | `value_low` | Value param_2; the low 32 bits of the payload |
| +0x0c | 4 | `value_high` | Value param_3; forms IEEE 754 double with value_low |

FUN_004b7200 writes uint16 `0x0001` at byte offset +12 (`puVar1+3` as uint16), confirming the type_tag field is 16 bits wide in that path.

---

## Lua Tag-method Data Tables

| Address | Content | Notes |
|---------|---------|-------|
| `DAT_005d8880` | Settability table | byte[event × 0xF]; signed bytes; row stride 0xF; indexed as [event + type×0xF]; first cite input_lua_d2_cont1/0x004b9540.md |
| `DAT_005d8830` | Event name array | Tag-method event name string array; 15+ entries |
| `DAT_005d8818` | Type name array | Tag type names |
| `DAT_005d8808` | Nil/absent-key sentinel | Returned by FUN_004b9bb0 when table key not found |
| `DAT_00617f34` | String key | Global name string used in Lua table registration |

---

## Open uncertainties

| U-ID | Gap |
|------|-----|
| U-2589 | Per-joypad struct +0x00 field conflict (IDirectInputDevice8* vs string ptr) — see gamepad_state.md |
| U-3835 | lua_State +0x6c — saved/restored across pcall but role not determined from observed plates |
