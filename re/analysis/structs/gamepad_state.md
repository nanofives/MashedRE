# Gamepad State — Initial Draft

**Session:** input_dinput_d3-20260511-1750  
**Evidence sources:** FUN_00495870 (d2), FUN_004957a0 (d2), FUN_00496100 (d2), FUN_004972b0 (d3), FUN_00495830 (d3)  
**Confidence:** C1 (mechanically observed; no Frida confirmation)

---

## Global joypad management block (input subsystem)

| Address | Type | Description |
|---|---|---|
| DAT_00772fac | int | Joypad device count (written by EnumDevices callback at 0x00495ee0 D-7660) |
| DAT_00771e80 | int | Max axes per device (= 0x10; set by FUN_004960a0) |
| DAT_00772fa8 | float | Axis deadzone threshold (= ~0.35f, 0x3eb33333; set by FUN_004960a0) |
| DAT_00771eb4 | struct[] | Joypad device struct array; stride **0x448** per slot; base of per-pad data |

## Per-joypad device struct (base: DAT_00771eb4 + slot × 0x448)

Observed from FUN_00495870 (unaff_ESI = struct ptr, register convention).

| Offset | Size | Description |
|---|---|---|
| +0x00 | 4 | IDirectInputDevice8\* (COM interface pointer) |
| +0x50? | 0x50 | Raw DIJOYSTATE buffer (80 bytes; written by GetDeviceState vtable+0x24) |
| +0x92 | varies | Axis history accumulator (8 frames; cleared in FUN_00495870) |
| +0xa0 | 4 | Processed axis X (float; after deadzone + unit-circle clamp + smoothing) |
| +0xa1 | 4 | Processed axis Y (float; negated relative to raw) |
| +0xb2 | 4 | Current-frame field (zeroed per frame) |
| +0xb3 | 4 | Current-frame field (zeroed per frame) |
| +0xd0 | varies | 16-frame axis history window (offsets 0xd0..0x111; slid each frame) |
| +0xd2 | 4 | Current-frame field (zeroed per frame) |
| +0xd3 | 4 | Current-frame field (zeroed per frame) |
| +0xf2 | 4 | Current-frame field (zeroed per frame) |
| +0x102 | 4 | Button bitmap — 32 bits (bit k = DIK button k pressed; 17 buttons processed in FUN_00495870) |

**[UNCERTAIN U-2589]** FUN_00495830 (strcpy from joypad struct slot) copies a string starting at struct +0x00, which conflicts with the IDirectInputDevice8\* COM pointer at +0x00 observed in FUN_00495870. Either: (a) the string field begins at a non-zero offset and the stride calculation is off; (b) the COM ptr is stored elsewhere (possibly at a negative offset, i.e. FUN_00495830 parameter already points +N into the struct). Needs listing inspection at DAT_00771eb4.

**[UNCERTAIN U-2589-note]** Struct size 0x448 = 1096 bytes. DIJOYSTATE2 is 0x2a0 bytes; DIJOYSTATE is 0x50 bytes. The remaining space in the 0x448-byte struct likely holds the processed axis arrays and history buffers.

---

## Per-frame snapshot buffers (written by FUN_004972b0 each frame)

| Address | Size | Stride | Description |
|---|---|---|---|
| DAT_007730d4 | count×4 | 4 per slot | Axis snapshot (4-byte; written by FUN_004957a0 param_5) |
| DAT_0077311c | count×8 | 8 per slot | Axis snapshot 2 (8-byte; written by FUN_004957a0 param_3) |
| DAT_0077313c | 0x20 | — | Keyboard state bitmap (32 bytes = 256 bits; zeroed then filled by FUN_00496100) |

FUN_004957a0 with param_2=0 writes axis calibration min/max (param_3=range-A ptr, param_5=flags ptr) — so DAT_0077311c and DAT_007730d4 hold the axis calibration outputs, not raw DirectInput data. The raw DI data is in the per-joypad struct's DIJOYSTATE buffer.

---

## Axis calibration arrays

| Address | Stride | Description |
|---|---|---|
| DAT_00772150 | 8 per (slot×0x89 + axis) | Calibration range A (2×int32) per device/axis pair |
| DAT_007721d0 | 8 per (slot×0x89 + axis) | Calibration range B (2×int32) per device/axis pair |
| DAT_00772290 | 4 per (slot×0x112 + axis) | Axis flags (int32) per device/axis pair |

**[UNCERTAIN U-2292]** Stride 0x89 = 137 axes-per-device in calibration arrays. DIJOYSTATE has 8 positional axes + 4 POV hats; DIJOYSTATE2 adds 128 buttons and 2 velocity axes. 137 may account for all DIJOYSTATE2 axes/channels. Confirm by checking stride against sizeof(DIJOYSTATE2).

---

## Controller config buffer

| Address | Stride | Description |
|---|---|---|
| DAT_007e95c0 | 0x80 per slot | Controller key mapping config (0x200 bytes total for 4 slots; fread from "contcfg%d.bin") |

---

## Keyboard device

| Address | Description |
|---|---|
| DAT_00772fb8 | IDirectInputDevice8\* for keyboard (used by FUN_00496100) |
