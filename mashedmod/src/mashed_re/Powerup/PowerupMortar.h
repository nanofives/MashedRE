// Powerup/PowerupMortar.h — MORTAR's own projectile chain (D3 criterion (c)).
//
// Two functions, both decompiled from a read-only Ghidra pool clone and
// cross-checked against disassembly of original/MASHED.exe.unpatched (SHA-256
// BDCAE093...3C0E, the pinned anchor):
//
//   0x00453730  FUN_00453730 — the DETONATION TEST. Probes `pos -> pos + delta`
//                and, on a hit the surface gate allows, blows up and returns 1.
//                Single caller, 0x004538fe, inside FUN_004538b0.
//   0x004538b0  FUN_004538b0 — the per-frame projectile UPDATE. Its whole body
//                sits inside `if (FUN_00453730() == 0)`, so a detonation leaves
//                the record completely untouched, age included.
//
// POOL, disassembled from the MORTAR tick's own second loop
// (0x00453c30..0x00453c51): `MOV ESI,0x684ea8` @0x00453c28, `ADD ESI,0x110`
// @0x00453c45, `CMP ESI,0x6870a8` @0x00453c4b — base 0x00684ea8, stride 0x110,
// (0x6870a8 - 0x684ea8) / 0x110 = 32 records. An entry is live when its +0x04 is
// non-zero.
//
// (The tick's FIRST loop is a different thing entirely: per-CAR blocks at
// 0x00684e3c stride 0x1c, 4 entries, and it is what calls the shared target
// acquisition — `PUSH 0x41a00000` @0x00453bca = 20.0 cone, `PUSH 0x41700000`
// @0x00453bcf = 15.0 range, `CALL 0x459620` @0x00453bd9. MORTAR's cone and range
// are literals at that call site. See PowerupAim.h.)
//
// THE ARC. Y is not integrated. `pos.y = h(age) * 1.5 + baseY` with h a half-sine
// in age for the first 1.625 s and a falling line after, so the velocity's Y
// component is computed and then never used. That is what makes a mortar lob.
//
// PORT BOUNDARY, stated plainly:
//   - The integration and the detonation test are verbatim.
//   - The homing target (`FUN_0046d4a0(owner)`) is an INPUT, not a call — the
//     standalone and the replay reach the car field differently.
//   - The three effect calls a detonation makes (FUN_00477760 the explosion,
//     FUN_00486610, FUN_00453210 the teardown) are NOT ported; they make no
//     contact call and cannot move a criterion-(c) count.
//   - FUN_004532f0 and FUN_00453100 (the trail's 15-segment ribbon, run from the
//     tick right after the update) are NOT ported: visual, no contact call.
//   This is C2-grade. Do NOT mark C4.
#pragma once

#include <cstdint>

namespace mashed_re {
namespace Powerup {
namespace Mortar {

// One pool record's live fields, by the dword index FUN_004538b0 uses.
struct Record {
    int   owner;        // [0]         car index, the homing target's owner
    float aim[3];       // [2..4]      offset added to the target's position
    float pos[3];       // [5..7]      FUN_00453730 reads this as segment point A
    float vel[3];       // [8..10]
    float prev[3];      // [0xb..0xd]
    float delta[3];     // [0xe..0x10] the frame's travel; segment B = A + delta
    int   homing;       // [0x11]
    float age;          // [0x13]      seconds
    float ageN;         // [0x14]      age / 1.3
    float baseY;        // [0x15]      the arc's base height
};

// The one live-subsystem read, hoisted. `valid` is 0 on a frame where the
// original never called FUN_0046d4a0 (i.e. `homing` was already clear).
struct Target {
    float pos[3];
    int   valid;
};

// 0x00453730. Returns 1 when the projectile detonated this frame.
int  Detonate(Record& r);

// 0x004538b0. Runs Detonate first; on a detonation it returns 1 and changes
// NOTHING. Returns 2 if the projectile aged out (the caller should retire it),
// 0 if it integrated normally.
int  Step(Record& r, const Target& t, float dt);

}  // namespace Mortar
}  // namespace Powerup
}  // namespace mashed_re
