# Vehicle Struct Layout — Dynamics Fields

**Status:** Initial draft (vehicle_dynamics_d3-20260512). Byte offsets confirmed from decompilation across multiple sessions.

All offsets are byte offsets from the vehicle struct base pointer. RVA citations are the Ghidra functions that reveal each field.

## Confirmed fields

| Byte offset | Field (int[] index) | Type | Confirmed value / notes | Source RVA |
|-------------|---------------------|------|-------------------------|------------|
| +0x00 | [0] | int | Vehicle ID / type discriminator; compared against player IDs and event types | 0x004694e0, 0x00469df0 |
| +0x50 | [0x14] | float | Mass-like multiplier (used as `param_1[0x14] * velocity`); exact meaning [UNCERTAIN] | 0x004694e0 |
| +0x15C..+0x164 | [0x57..0x59] | float[3] | Center-of-mass reference position (3-float); used to compute radius arm for contact impulse | 0x00468d80, 0x004694e0 |
| +0x144..+0x14C | [0x51..0x53] | float[3] | Angular velocity (3-float) | 0x0046ef70 (SESSION_END vehicle_dynamics) |
| +0x198 | [0x66] | int | Wheel active-flag array base; 18 entries, stride 0x31 ints (0xC4 bytes) per entry | 0x00468d80, 0x004694e0 |
| +0x4C0 | [0x130] | float[16][18] | Contact record array; 18 slots × 16 floats (64 bytes) per slot | 0x00468d80, 0x0046ef70 |
| +0x4CC | [0x133] | int | Contact manifold history start; stride 0x90 ints for 3 history slots | 0x00469aa0 |
| +0x92C | [0x24B] | ? | Base of contact write buffer (`param_1 + param_1[0x26b]*0x10 + 0x24a`) — write pointer starts here | 0x00468d80 |
| +0x9AC | [0x26b] | int/uint | Contact write slot index (counts active contacts) | 0x00468d80, 0x004694e0 |
| +0x9B0..+0x9B8 | [0x26c..0x26e] | float[3] | Linear velocity (3-float) | 0x004694e0, 0x0046ef70 (SESSION_END) |
| +0x9BC..+0x9C4 | [0x26f..0x271] | float[3] | 3-float vector; used as rotation axis in FUN_004c4d20(…, 90.0f); exact role [UNCERTAIN U-3577] | 0x00468d80 |
| +0x9C8..+0x9D0 | [0x272..0x274] | float[3] | Forward direction (3-float) | 0x0046d700 (SESSION_END vehicle_dynamics) |
| +0x9F0 | [0x27c] | int | Flag: gates debug event enqueue; values 0, 1 seen | 0x004694e0 |
| +0x9FC..+0xA04 | [0x27f..0x281] | float[3] | First wheel position (3-float); start of 18-entry position array stride 3 floats | 0x00468d80, 0x004694e0 |
| +0xAF0 | [0x2BC] | int | Bounce timer; set to 0x1e0 (480) on vehicle-vehicle collision | 0x00469df0 |
| +0xA60 | [0x298?] | ? | Linear velocity written via FUN_004c3d90 dispatch (from 0x0046d510 analysis) | 0x0046d510 |
| +0xBFC | n/a | int[32+] | Contact history table; 32 entries scanned by FUN_00468b40; active flags at +0x80 offset | 0x00468b40 |

## Vehicle array layout (global)

| Global | Byte offset formula | Notes |
|--------|---------------------|-------|
| DAT_00881000 | base of vehicle slot array | Confirmed 0xD04 stride per vehicle |
| DAT_008815A0 | contact buffer base | per vehicle at +v*0xD04 + 0x24A |
| DAT_00881F48 | contact write double-buffer ptr A | flip-flop with 0x00881F4C via `& 1` |
| DAT_00881F4C | contact read double-buffer ptr B | |

Source: vehicle_dynamics-20260506-expand SESSION_END.

## Uncertainties

- U-3573: `DAT_0088e60c` terrain geometry entry count
- U-3574: terrain geometry entry layout (10 floats per 0x90-byte entry; normal, tri-verts semantics)
- U-3575: `DAT_006e87b8` / `DAT_006fa0f8` — dynamic physics object list; entry structure unknown
- U-3576: `DAT_006e71c4` / `DAT_006e71cc` — physics object table max index and manager base; stride 0x10 in state block table
- U-3577: `param_1[0x26f..0x271]` (+0x9BC..+0x9C4): 3-float; used as rotation axis at 90 degrees in terrain contact; could be slip direction or wheel axis

## Total confirmed size lower bound

0xBFC + contact-history-table ≈ at least 0xC7C bytes. Vehicle slot stride confirmed as 0xD04 bytes.
