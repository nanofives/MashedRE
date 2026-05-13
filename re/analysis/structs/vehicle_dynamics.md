# Vehicle Struct Layout — Dynamics Fields

**Status:** Extended draft (struct_extract_phase5_pt2-20260513). Original draft vehicle_dynamics_d3-20260512; extended from vehicle_update_d3 and vehicle_dynamics_d3 sessions.

All offsets are **byte offsets** from the vehicle struct base pointer. Vehicle slot array base: `DAT_00881000`, stride 0xD04 bytes per vehicle slot.

**Notation note:** Decompiler plates using `ESI[N]` / `EDI[N]` denote int-array index N → byte offset N×4. Direct byte-offset notation (`ESI+0x...`) is used by float-pointer arithmetic plates. Both are listed as byte offsets in this table.

## Confirmed fields

| Byte offset | Type | Name / notes | Source RVA(s) |
|-------------|------|--------------|---------------|
| +0x00 | int | Vehicle ID / type discriminator; compared against player IDs and event types | 0x004694e0, 0x00469df0 |
| +0x20..+0x34 | int[6] | Slip/state charge counters (6 × int); decremented when the paired countdown timer[i] reaches 0 | 0x00467350 |
| +0x38..+0x4C | float[6] | Countdown timers (6 × float); decremented by dt each tick, clamped ≥ 0 | 0x00467350 |
| +0x3C | float | Boost/slip timer index 1 (= timers[1] = +0x3C); has a separate gate check | 0x00467350 |
| +0x50 | float | Mass-like multiplier (`[0x14]`×velocity); exact meaning [UNCERTAIN] | 0x004694e0 |
| +0x54..+0x5C | float[3] | Gear torque constants (3 floats); `ESI[0x54]*ESI[0x55]*ESI[0x56]` × throttle in drive-torque | 0x00467650 |
| +0x60..+0x68 | float[3] | Right-direction XYZ for wheel 0; used in steer-torque projection | 0x0046ddb0 |
| +0x144..+0x14C | float[3] | Angular impulse accumulator XYZ; written by VehicleWheelContactSolver | 0x0046ef70, 0x0046f6c0 |
| +0x15C..+0x164 | float[3] | Center-of-mass reference position (3-float); radius arm for contact impulse | 0x00468d80, 0x004694e0 |
| +0x170 | float | Angular inertia scale (gear inertia); `ESI[0x5C]` × dt in angular-velocity update | 0x00467650, 0x0046ddb0 |
| +0x194 | float | Wheel-0 spring load value; contact-state threshold input | 0x0046f6c0 |
| +0x198 | int | Wheel-0 contact state base; state machine per wheel (0=no contact, 1=light, 2=full); stride 0x31 ints (0xC4 bytes) per wheel slot | 0x004694e0, 0x0046f6c0 |
| +0x1F0 | int | Vehicle type enum (MSVC raw ints: `−0x5F7F80`=light, `−0x557F80`=B, `−0x69E1A6`=helicopter, `−0xE17F4C`=D, `−0x373738`/`−1`=powerup-types) | 0x00467650 |
| +0x20C | float | Steer value wheel 0 (`EDI[0x83]`); dot product of velocity-delta with wheel right-axis | 0x0046ddb0 |
| +0x244..+0x24C | float[3] | Right-direction XYZ for wheel 1 (`EDI[0x91..0x93]`) | 0x0046ddb0 |
| +0x2D0 | float | Steer value wheel 1 (`EDI[0xB4]`) | 0x0046ddb0 |
| +0x394 | float | Steer value wheel 2 (`EDI[0xE5]`) | 0x0046ddb0 |
| +0x458 | float | Steer value wheel 3 (`EDI[0x116]`) | 0x0046ddb0 |
| +0x478..+0x4C7 | float[20] | Gear-ratio multiplier curves; 5 ratios per wheel-set × 4 wheel-sets = 20 floats (0x50 bytes) | 0x00467650 |
| +0x490 | int | Current gear index | 0x00467650 |
| +0x494 | int | Gear-shift cooldown counter (3000 on upshift, −3000 on downshift) | 0x00467650 |
| +0x4C0 | float[16][18] | Contact record array; 18 slots × 16 floats (64 bytes) per slot | 0x00468d80, 0x0046ef70 |
| +0x4CC | int | Contact manifold history start; stride 0x90 ints for 3 history slots | 0x00469aa0 |
| +0x920..+0x927 | ? | Wheel-ring state block base; `EDI[0x9AC]*0x40 + 0x928 + EDI` indexes current block | 0x0046f6c0 |
| +0x92C | ? | Contact write buffer pointer (`param_1 + param_1[0x26b]*0x10 + 0x24a`) | 0x00468d80 |
| +0x9AC | int | Wheel ring index (0 or 1); double-buffer selector for contact write manifold | 0x00468d80, 0x004694e0, 0x0046f6c0 |
| +0x9B0..+0x9B8 | float[3] | Linear velocity XYZ | 0x004694e0, 0x0046ef70, 0x0046f6c0, 0x0046ddb0 |
| +0x9BC..+0x9C4 | float[3] | 3-float vector; used as rotation axis in `FUN_004c4d20(…, 90.0f)` for terrain contact tangential response [UNCERTAIN U-3577] | 0x00468d80 |
| +0x9C8..+0x9D4 | float[3] | Forward direction XYZ (two overlapping sources: [0x272] from 0x0046d700, [0x267] = +0x9D4 from 0x00467350) [UNCERTAIN U-3721: two forward-dir fields at ≈ same offset] | 0x0046d700, 0x00467350 |
| +0x9D4..+0x9DC | float[3] | Forward direction XYZ (`pfVar6[0x267..0x269]`); alignment debug at 0x00467350 | 0x00467350 |
| +0x9E0 | float | Grounded-wheel count (output of VehicleWheelContactSolver; 4.0f = all 4 grounded) | 0x0046f6c0, 0x00467650 |
| +0x9E4 | float | Speed magnitude (linear velocity; written by DrivetrainUpdate) [UNCERTAIN U-3722: ForceIntegrator reads same offset as "throttle" via `EDI[0x279]`] | 0x00467650, 0x0046ddb0 |
| +0x9E8 | float | Angular speed magnitude (written by DrivetrainUpdate) | 0x00467650 |
| +0x9F0 | int | No-contact-pending flag; 0 → lateral drift path active; used in VehicleObjectContactSolver debug gate | 0x004694e0, 0x0046f6c0 |
| +0x9FC..+0xA04 | float[3] | Wheel-0 world position; start of 18-entry position array, stride 3 floats (0xC = 12 bytes) | 0x00468d80, 0x004694e0 |
| +0xAD0 | int | Position history ping-pong ring index (0 or 1; `EDI[0x2B4]`) | 0x0046ddb0 |
| +0xAD4..+0xAE8 | float[6] | Position history buffer: 2 slots × XYZ float[3] (`EDI[0x2B5..0x2BA]`) | 0x0046ddb0 |
| +0xAF0 | int | Bounce timer; set to 0x1e0 (480) on vehicle-vehicle collision | 0x00469df0 |
| +0xB00..+0xB08 | float[3] | Random impulse per-wheel (set by `FUN_00472650`; `EDI[0x2C0..0x2C2]`) | 0x0046ddb0 |
| +0xB14..+0xB1C | float[3] | Accumulated boost force XYZ | 0x00467650 |
| +0xB18 | float | Airborne velocity damping counter; decremented `dt * ESI[0x14] * _DAT_005ccd08` while airborne (`EDI[0x2C6]`) | 0x0046ddb0 |
| +0xB20 | int | Airborne flag / steering-applied flag [UNCERTAIN U-3723: both DrivetrainUpdate (steering) and ForceIntegrator (`EDI[0x2C8]`=airborne) cite this offset] | 0x00467650, 0x0046ddb0 |
| +0xBF4 | int | Boost duration counter | 0x00467650 |
| +0xBF8 | int | Boost state (0=off, 1=ramping, 2=active) | 0x00467650 |
| +0xBFC | int[32+] | Contact history table; 32 entries; active-flags at +0x80 per slot; scanned by FUN_00468b40 | 0x00468b40 |
| +0xD00 | int | Hover mode flag (helicopter vehicle type; non-zero = hover active) | 0x00467650 |

## Vehicle array layout (global)

| Global | Role |
|--------|------|
| `DAT_00881000` | Base of vehicle slot array; stride 0xD04 bytes per slot |
| `DAT_008815A0` | Alternate cited base (= `DAT_00881000 + 0x5A0`?); contact buffer for vehicle 0 |
| `DAT_008815D8` | Timer block base (= vehicle[0] + 0x38); used in VehicleSlipTimerTick |
| `DAT_00881560` | Wheel contact position scratch (16 DWORDs) |
| `DAT_00881564/570/57c/588` | Steer-rate scalar per wheel (4 globals) |
| `DAT_00881F4C` / `DAT_00881F48` | Contact write double-buffer ptr B / A; flip-flop via `& 1` |

## Uncertainties

| ID | Offset | Question |
|----|--------|---------|
| U-3573 | n/a | `DAT_0088e60c` — terrain geometry entry count; update path unknown |
| U-3574 | n/a | Terrain geometry entry layout (0x90-byte entries; `pfVar7[0xb]` sentinel values) |
| U-3575 | n/a | `DAT_006e87b8`/`DAT_006fa0f8` — dynamic physics object list base/count; entry structure unknown |
| U-3576 | n/a | `DAT_006e71c4`/`cc` — physics object manager; state table stride 0x10 |
| U-3577 | +0x9BC..+0x9C4 | 3-float used as rotation axis at 90° in terrain contact; exact semantic unknown |
| U-3721 | +0x9C8..+0x9D4 | Two different forward-direction field sources at ≈ same byte range; need runtime or listing cross-check |
| U-3722 | +0x9E4 | ForceIntegrator reads `EDI[0x279]` as "throttle"; DrivetrainUpdate writes +0x9E4 as speed magnitude; same offset — one may overwrite the other or they're different fields |
| U-3723 | +0xB20 | DrivetrainUpdate (ESI+0xB20 = steering-applied) vs ForceIntegrator (`EDI[0x2C8]` = airborne) — both cite byte +0xB20 |

## Total confirmed size lower bound

Largest confirmed field: +0xD00 + 4 = 0xD04 bytes. Stride confirmed 0xD04, consistent.
