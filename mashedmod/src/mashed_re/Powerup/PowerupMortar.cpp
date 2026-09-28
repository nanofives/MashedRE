// Powerup/PowerupMortar.cpp — FUN_00453730 + FUN_004538b0. See PowerupMortar.h
// for the pool geometry and the port boundary.
//
// Anchor: original/MASHED.exe.unpatched (SHA-256 BDCAE093...3C0E).

#include "PowerupMortar.h"
#include "PowerupContact.h"

#include <cmath>
#include <cstdlib>
#include <cstring>

namespace mashed_re {
namespace Powerup {
namespace Mortar {

namespace {

// NON-DEGENERACY CONTROL (harness only; unset in every real run).
//   MASHED_MORTAR_FORCE=noarc   -> Y integrates by velocity instead of arcing
//   MASHED_MORTAR_FORCE=nohome  -> the homing branch never runs
//   MASHED_MORTAR_FORCE=allow   -> the detonation gate's polarity is inverted
// The replay's headline number for this TU is a max-drift, and a drift of ~1 ULP
// means nothing unless breaking the integrator moves it. These exist to be run.
int ForceMode() {
    static int m = -2;
    if (m == -2) {
        const char* e = std::getenv("MASHED_MORTAR_FORCE");
        m = !e ? -1 : (std::strcmp(e, "noarc") == 0 ? 0
                     : std::strcmp(e, "nohome") == 0 ? 1
                     : std::strcmp(e, "allow") == 0 ? 2 : -1);
    }
    return m;
}

// Constants read from the anchor with re/tools/disasm_va.py's own section mapper.
constexpr float kAgeRate  = 0.769230783f;  // _DAT_005ce424 0x3f44ec4f  = 1/1.3
constexpr float kExpire   = 2.0f;          // _DAT_005cc574 0x40000000
constexpr float kHomeS    = 0.379999995f;  // _DAT_005ce420 0x3ec28f5c
constexpr float kHomeOff  = 0.579999983f;  // _DAT_005ce41c 0x3f147ae1
constexpr float kArcSplit = 1.25f;         // _DAT_005cd074 0x3fa00000
constexpr float kArcT0    = 1.29999995f;   // _DAT_005cd230 0x3fa66666
constexpr float kArcSlope = -2.20000005f;  // _DAT_005ce418 0xc00ccccd
constexpr float kPi       = 3.14159274f;   // _DAT_005ce2f4 0x40490fdb
constexpr float kArcH     = 1.5f;          // _DAT_005cc348 0x3fc00000
constexpr float kLift     = 0.0500000007f; // _DAT_005cc9a0 0x3d4ccccd

}  // namespace

// 0x00453730
int Detonate(Record& r) {
    // A = rec+0x14 (pos), B = A + rec+0x38 (the frame's delta). The probe is the
    // segment the projectile is ABOUT to travel, which is why the update writes
    // `delta` before the next frame's test rather than after this one's.
    const float seg[6] = { r.pos[0], r.pos[1], r.pos[2],
                           r.pos[0] + r.delta[0],
                           r.pos[1] + r.delta[1],
                           r.pos[2] + r.delta[2] };
    Contact::WorldHit hit;
    // CALL 0x4b4cd0 @0x00453784; TEST EAX,EAX; JE -> return 0.
    if (Contact::SegmentQuery(seg, &hit, 0x00453789u) == 0) return 0;
    // CALL 0x45c350 @0x004537b6 -- NOTE this is the sweep's confirm leaf, not the
    // 0x0045c110 surface gate the OIL/P_MINE drops use. NON-zero REFUSES: the
    // original falls through to `return 0` and the projectile keeps flying.
    const int gate = Contact::ConfirmGateAt(&hit, 0x004537bbu);
    if (ForceMode() == 2 ? (gate == 0) : (gate != 0)) return 0;
    float p[3];
    // CALL 0x4b4650 @0x004537da
    Contact::Vec3Lerp(p, seg, seg + 3, hit.t, 0x004537dfu);
    for (int c = 0; c < 3; ++c) p[c] += hit.normal[c] * kLift;
    float m[16] = {0};
    // CALL 0x4b5080 @0x00453827
    Contact::BasisFromTri(m, hit.vert, p, 0x0045382cu);
    // The three effect calls that follow (FUN_004c5010, FUN_00477760,
    // FUN_00486610, FUN_00453210) are not ported -- no contact call among them.
    return 1;
}

// 0x004538b0
int Step(Record& r, const Target& t, float dt) {
    // CALL 0x453730 @0x004538fe. The ENTIRE rest of the body is inside
    // `if (iVar4 == 0)`, so a detonation leaves the record untouched -- age
    // included. That is load-bearing: a replay that advanced the age anyway
    // would drift by one frame from the detonation on.
    if (Detonate(r) != 0) return 1;

    r.ageN = r.age * kAgeRate;
    if (r.ageN > kExpire) return 2;              // FUN_00453210, then return

    if (r.homing != 0 && ForceMode() != 1) {
        // The bearing is taken on the GROUND PLANE: the Y component is written
        // as 0.0 and never recomputed, so the velocity's Y stays 0 too -- which
        // is consistent with Y being an absolute arc below rather than integrated.
        float d[3] = { (r.aim[0] + t.pos[0]) - r.pos[0],
                       0.0f,
                       (r.aim[2] + t.pos[2]) - r.pos[2] };
        const float L = std::sqrt(d[0] * d[0] + d[1] * d[1] + d[2] * d[2]);
        if (L != 0.0f) { d[0] /= L; d[1] /= L; d[2] /= L; }
        const float s = L * r.ageN * kHomeS;
        r.vel[0] = d[0] * s;
        r.vel[1] = d[1] * s;
        r.vel[2] = d[2] * s;
        if (r.ageN > kHomeOff) r.homing = 0;
    }

    // The arc height. A half-sine in normalised age for the first 1.625 s, then
    // a falling line. Both take the age BEFORE this frame's increment.
    const float h = (r.ageN >= kArcSplit) ? ((r.age - kArcT0) * kArcSlope)
                                          : std::sin(r.ageN * kPi);

    const float prev[3] = { r.pos[0], r.pos[1], r.pos[2] };
    r.age += dt;                                  // DAT_007f100c
    for (int c = 0; c < 3; ++c) r.prev[c] = prev[c];
    r.pos[0] = r.vel[0] + prev[0];
    r.pos[2] = r.vel[2] + prev[2];
    r.pos[1] = (ForceMode() == 0) ? (r.vel[1] + prev[1])
                                  : (h * kArcH + r.baseY);   // ABSOLUTE, not integrated
    for (int c = 0; c < 3; ++c) r.delta[c] = r.pos[c] - prev[c];
    return 0;
}

}  // namespace Mortar
}  // namespace Powerup
}  // namespace mashed_re
