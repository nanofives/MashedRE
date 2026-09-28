// Powerup/PowerupMissile.cpp — the contact half of FUN_00455c90. See
// PowerupMissile.h for the pool geometry and the port boundary.
//
// Anchor: original/MASHED.exe.unpatched (SHA-256 BDCAE093...3C0E).

#include "PowerupMissile.h"
#include "PowerupContact.h"

#include <cstdlib>
#include <cstring>

namespace mashed_re {
namespace Powerup {
namespace Missile {

namespace {

// Constants read from the anchor with re/tools/disasm_va.py's own section mapper.
constexpr float kAgeLimit = 3.0f;           // _DAT_005cc31c 0x40400000
constexpr float kRadK     = 1.5f;           // _DAT_005cc348 0x3fc00000
constexpr float kRadBase  = 0.05f;          // `MOV [ESP+0x40],0x3d4ccccd` @0x00455d5a
constexpr float kGroundA  = 0.400000006f;   // _DAT_005ccac0 0x3ecccccd
constexpr float kGroundB  = 2.5f;           // _DAT_005cd088 0x40200000
constexpr float kGroundC  = -0.5f;          // _DAT_005cd50c 0xbf000000
constexpr float kBiasFloor= -0.0500000007f; // _DAT_005cd054 0xbd4ccccd, and the
                                            // value written on the clamp
                                            // (`MOV [EBP-0x2c],0xbd4ccccd` @0x00455e8f)
constexpr float kBiasMiss = -0.000500000024f; // 0xba03126f @0x00455e98

// NON-DEGENERACY CONTROL (harness only; unset in every real run).
//   MASHED_MISSILE_FORCE=noparity  -> the sphere query runs EVERY frame
//   MASHED_MISSILE_FORCE=nolife    -> the age gate never expires a projectile
//   MASHED_MISSILE_FORCE=flatbias  -> the ground bias is always the miss sentinel
// The parity gate is the single most load-bearing thing in this TU (it halves one
// site's call count), so it gets a control that breaks exactly it.
int ForceMode() {
    static int m = -2;
    if (m == -2) {
        const char* e = std::getenv("MASHED_MISSILE_FORCE");
        m = !e ? -1 : (std::strcmp(e, "noparity") == 0 ? 0
                     : std::strcmp(e, "nolife") == 0 ? 1
                     : std::strcmp(e, "flatbias") == 0 ? 2 : -1);
    }
    return m;
}

}  // namespace

// 0x00455c90, per-record body
int Step(Record& r, float dt, int frameCtr) {
    if (!r.live) return 0;

    // The flight step (FUN_00455610 / FUN_004556f0) is NOT ported — see the
    // header. `pos` and `delta` arrive already stepped.

    r.age += dt;                                   // DAT_007f100c @0x00455d17
    // `FCOMP [0x5cc31c]` @0x00455d23 then `TEST AH,0x41` / `JNE 0x455d3c`: the
    // loop continues while age <= 3.0, so strictly-greater is what expires it.
    if (r.age > kAgeLimit && ForceMode() != 1) {
        // CALL 0x455910 @0x00455d32
        Contact::WorldHit dummy;
        (void)dummy;
        r.live = 0;
        return 2;
    }

    // ---- the frame-parity gate, 0x00455d83..0x00455d99 --------------------
    // `MOV ECX,[0x7f101c]` / `AND ECX,0x80000001` / the sign fixup / `JNE
    // 0x455e07`. The AND-with-sign-fixup is MSVC's idiom for a signed `% 2`, so
    // the block runs on EVEN values of the counter only.
    int detonated = 0;
    const bool even = (frameCtr & 1) == 0;
    if (even || ForceMode() == 0) {
        // radius = |delta|^2 * 1.5 + 0.05. Note it is the SQUARED length, not the
        // length: 0x00455d9b..0x00455dc1 multiplies each component by itself,
        // sums, and does a single FMUL by _DAT_005cc348 — there is no FSQRT.
        const float l2 = r.delta[0] * r.delta[0]
                       + r.delta[1] * r.delta[1]
                       + r.delta[2] * r.delta[2];
        const float sphere[4] = { r.pos[0], r.pos[1], r.pos[2], l2 * kRadK + kRadBase };
        Contact::WorldHit hit;
        // CALL 0x4b4d10 @0x00455ddb; TEST EAX,EAX @0x00455de3; JE 0x455e07.
        if (Contact::SphereQueryAt(sphere, &hit, 0x00455de0u) != 0) {
            // CALL 0x45c350 @0x00455df4; TEST EAX,EAX; JNE 0x455e07 -> NON-ZERO
            // REFUSES, the same polarity as MORTAR's gate.
            if (Contact::ConfirmGateAt(&hit, 0x00455df9u) == 0) {
                // CALL 0x455910 @0x00455e02 -- and then execution FALLS THROUGH to
                // 0x00455e07, the ground probe. There is no jump here. Only the AGE
                // path skips the probe, via `JMP 0x455f2a` @0x00455d37.
                //
                // Returning early here instead cost exactly ONE ground query on
                // verify/d3_contact_20260928b/s1 (155 vs 154), which is how the
                // difference was found rather than reasoned about.
                r.live = 0;
                detonated = 1;
            }
        }
    }

    // ---- the ground probe, 0x00455e07..0x00455e9f -------------------------
    // Straight down by 3.0 from the position. The Y drop reuses _DAT_005cc31c,
    // the same constant as the age limit — recorded, not explained.
    const float seg[6] = { r.pos[0], r.pos[1], r.pos[2],
                           r.pos[0], r.pos[1] - kAgeLimit, r.pos[2] };
    Contact::WorldHit hit;
    // CALL 0x4b4cd0 @0x00455e54; TEST EAX,EAX @0x00455e5c; JE 0x455e98.
    if (Contact::SegmentQuery(seg, &hit, 0x00455e59u) == 0) {
        r.bias = kBiasMiss;                        // @0x00455e98
    } else if (ForceMode() == 2) {
        r.bias = kBiasMiss;
    } else {
        // (t*3.0 - 0.4) * 2.5 * -0.5, i.e. 0.5 - 3.75*t, floored at -0.05.
        const float b = (hit.t * kAgeLimit - kGroundA) * kGroundB * kGroundC;
        r.bias = b;                                // FST @0x00455e7f
        if (b < kBiasFloor) r.bias = kBiasFloor;   // FCOMP @0x00455e82, @0x00455e8f
    }
    return detonated;
}

}  // namespace Missile
}  // namespace Powerup
}  // namespace mashed_re
