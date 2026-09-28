// Powerup/PowerupAim.cpp — FUN_00459620, target acquisition. See PowerupAim.h
// for the port boundary and for why this routine gates MORTAR, GUN and MISSILE.
//
// Anchor: original/MASHED.exe.unpatched (SHA-256 BDCAE093...3C0E).

#include "PowerupAim.h"
#include "PowerupContact.h"

#include <cmath>
#include <cstdlib>
#include <cstring>

namespace mashed_re {
namespace Powerup {
namespace Aim {

namespace {

// NON-DEGENERACY CONTROL (harness only; unset in every real run).
//   MASHED_AIM_FORCE=none  -> the candidate loop always finds nothing
//   MASHED_AIM_FORCE=lock  -> it always finds one
// A 187-of-348 match on the fallback query site means nothing unless breaking
// the loop breaks the match, so this exists to be run and reported alongside it.
int ForceMode() {
    static int m = -2;
    if (m == -2) {
        const char* e = std::getenv("MASHED_AIM_FORCE");
        m = !e ? -1 : (std::strcmp(e, "none") == 0 ? 0
                     : std::strcmp(e, "lock") == 0 ? 1 : -1);
    }
    return m;
}

// ---- constants, all read from the anchor with re/tools/disasm_va.py --------
// (read through disasm_va.py's OWN section mapper: a hand-rolled walk that gets
// the section-header field order wrong lands in .rdata's string pool and reports
// ASCII as floats.)
constexpr float kRad2Deg  = 57.2957802f;  // _DAT_005cc98c 0x42652ee1 = 180/pi
constexpr float kNormEps  = 1e-5f;        // _DAT_005cc990 0x3727c5ac
constexpr float kOne      = 1.0f;         // _DAT_005cc320 0x3f800000
constexpr float kZero     = 0.0f;         // DAT_005d757c  0x00000000
constexpr float kLift     = 0.02f;        // _DAT_005ce18c 0x3ca3d70a
constexpr float kBigAngle = 1000.0f;      // `MOV [ESP+0x54],0x447a0000` @0x0045969b

// The scratch FUN_00459620 zeroes at its top: `MOV ECX,0x1c` @0x0045963e +
// `REP STOSD` @0x00459647 -- 28 dwords, i.e. FOUR 7-dword candidate entries.
//
// NOTE, recorded not fixed: the candidate cursor keeps appending past entry 3
// (`piVar11 = local_a0 + local_104*7 - 1`). With at most 3 cars (the firing one
// is skipped) that cannot overflow, but the second list appends to the same
// array with no bound, so a non-empty list CAN walk off the 28-dword scratch in
// the original. That is one more reason `listCount` is asserted rather than
// modelled.
constexpr int kMaxCandidates = 4;

// Entry layout, read off the cursor arithmetic (`pfVar8 = local_b0 + 3`, then
// [-3],[-2],[-1],[0],[1],[2],[3] and `pfVar8 += 7`):
struct Candidate {
    float pos[3];   // [0..2]  the target's position when it was scored
    int   carIdx;   // [3]     car index, or -1 for a second-list entry
    int   objIdx;   // [4]     second-list index, or -1 for a car
    float dist;     // [5]
    float angle;    // [6]     degrees
};

int g_lastCount = 0;

// FUN_004c3ac0 (Vec3Magnitude) and FUN_004c39b0 (RwV3dNormalize) are both C4 in
// hooks.csv, but both read their result out of a RenderWare fast-sqrt LUT
// (`*(u32**)(*0x007d3ffc + *0x007d3ff8)`), and both already fall back to
// std::sqrt when that pointer is absent -- which it is on this path, in the
// standalone and in the replay alike. So this TU uses the CPU sqrt directly,
// the same convention PowerupContact.cpp's Normalize3 states. The difference is
// a possible last-ulp disagreement on `dist < range` and on the normalised
// direction; MEASURED not to flip any branch on the only capture that records
// the inputs (re/tools/aim_model.py: 348/348 with exact sqrt).
float Len3(const float v[3]) {
    const float sq = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
    return (sq > 0.0f) ? std::sqrt(sq) : 0.0f;
}

// FUN_004726f0 is NOT a bare dot product: it CLAMPS to
// [_DAT_005cc33c, _DAT_005cc320] = [-1, +1] (0x004726f0 body), which is what
// keeps the acos below in domain.
float DotClamped(const float a[3], const float b[3]) {
    float d = a[2] * b[2] + a[0] * b[0] + a[1] * b[1];
    if (d < -kOne) return -kOne;
    if (d > kOne) d = kOne;
    return d;
}

}  // namespace

int LastCandidateCount() { return g_lastCount; }

// 0x00459620
void Acquire(int slot, const Inputs& in, Record* rec) {
    Candidate cand[kMaxCandidates];
    std::memset(cand, 0, sizeof cand);
    int n = 0;

    // --- the header writes, 0x00459649..0x00459693 -------------------------
    rec->cone     = in.cone;        // [EBP+0x04] @0x0045966a
    rec->range    = in.range;       // [EBP+0x18] @0x0045966d
    rec->hit      = 0;              // [EBP+0x14] @0x00459673
    rec->objIndex = -1;             // [EBP+0x0c] @0x0045967a
    for (int i = 0; i < 3; ++i) rec->origin[i] = in.origin[i];

    // --- candidate loop over the cars, 0x004596c1.. ------------------------
    // `MOV EBX,4` @0x004596ab then DEC-then-test, so the scan order is
    // 3, 2, 1, 0 -- which fixes both the entry order and the tie-break below.
    for (int i = 3; i >= 0; --i) {
        if (i == slot) continue;                 // `CMP EAX,EBX` @0x004596cd
        if (!in.carActive[i]) continue;          // FUN_0040e370
        // d = target - origin with Y FORCED TO ZERO (`local_10c = 0.0`): the
        // cone test is a ground-plane bearing test, not a 3D one.
        float d[3] = { in.carPos[i][0] - in.origin[0],
                       kZero,
                       in.carPos[i][2] - in.origin[2] };
        const float dist = Len3(d);              // FUN_004c3ac0
        const float lsq  = d[0] * d[0] + d[1] * d[1] + d[2] * d[2];
        if (lsq > kNormEps) {                    // FUN_004c39b0, gated
            const float L = std::sqrt(lsq);
            d[0] /= L; d[1] /= L; d[2] /= L;
        }
        const float ang = std::acos(DotClamped(d, in.at)) * kRad2Deg;
        if (ang < rec->cone && dist < rec->range) {
            if (n < kMaxCandidates) {
                Candidate& c = cand[n];
                c.pos[0] = in.carPos[i][0];
                c.pos[1] = in.carPos[i][1];
                c.pos[2] = in.carPos[i][2];
                c.carIdx = i;
                c.objIdx = -1;
                c.dist   = dist;
                c.angle  = ang;
            }
            ++n;
        }
    }

    // --- the SECOND candidate list, 0x004597b2.. ---------------------------
    // Not ported. MEASURED empty (list_n == 0 on all 348 calls of
    // verify/d3_contact_20260928b/m1.msd). If a future capture disagrees this is
    // where the divergence will be, so it is left visible rather than silent.
    // [UNCERTAIN] the per-entry test is the same cone/range test with the
    // position from FUN_004075b0 (frame+0x40..0x48) and a `w` of 2.0; what the
    // list HOLDS is not established. Next command:
    //   py -3.12 re/tools/decomp_pc.py 0x004075b0 --callers
    (void)in.listCount;

    if (ForceMode() == 0) n = 0;
    else if (ForceMode() == 1 && n == 0) {          // seed one, so the lock arm runs
        n = 1; cand[0].carIdx = -1; cand[0].objIdx = -1;
        cand[0].dist = in.range; cand[0].angle = 0.f;
        for (int k = 0; k < 3; ++k) cand[0].pos[k] = in.origin[k] + in.at[k] * in.range;
    }
    g_lastCount = n;

    if (n == 0) {
        // --- the FALLBACK branch, 0x00459b9c..0x00459cd4 -------------------
        rec->dist      = rec->range;   // rec+0x1c = rec+0x18
        rec->targetCar = -1;
        // The probe is VERTICAL, at the point `range` straight ahead: A is one
        // unit above it and B one unit below. Note the Y term uses DAT_005d757c
        // (0.0), NOT at[1] -- so the probe point keeps the origin's height even
        // though the endpoint written below does not.
        const float aimZ  = in.at[2] * rec->range;
        const float baseY = rec->range * kZero + rec->origin[1];
        const float seg[6] = {
            in.at[0] * rec->range + rec->origin[0],   // A.x
            baseY + kOne,                             // A.y
            aimZ + rec->origin[2],                    // A.z
            in.at[0] * rec->range + rec->origin[0],   // B.x = A.x
            baseY - kOne,                             // B.y
            aimZ + rec->origin[2],                    // B.z = A.z
        };
        Contact::WorldHit hit;
        // CALL 0x4b4cd0 @0x00459c14; TEST EAX,EAX @0x00459c1c;
        // JE 0x459c9a @0x00459c1e -> the miss arm.
        if (Contact::SegmentQuery(seg, &hit, 0x00459c19u) == 0) {
            // miss: the endpoint is just `origin + at*range`, in FULL 3D this
            // time -- at[1] is used here where the probe ignored it.
            rec->endpoint[0] = in.at[0] * rec->range + rec->origin[0];
            rec->endpoint[1] = in.at[1] * rec->range + rec->origin[1];
            rec->endpoint[2] = in.at[2] * rec->range + rec->origin[2];
            rec->hit = 0;
        } else {
            float p[3];
            // CALL 0x4b4650 @0x00459c37
            Contact::Vec3Lerp(p, seg, seg + 3, hit.t, 0x00459c3cu);
            for (int c = 0; c < 3; ++c) p[c] += hit.normal[c] * kLift;
            rec->hit = 1;
            for (int c = 0; c < 3; ++c) rec->endpoint[c] = p[c];
        }
        for (int c = 0; c < 3; ++c) rec->aimPoint[c] = rec->endpoint[c];
    } else {
        // --- the LOCKED branch, 0x00459cdc.. -------------------------------
        // Selection: smallest angle, scanning entry n-1 DOWN to 0 with a STRICT
        // `<` against a 1000.0 seed. Scanning downward with a strict compare
        // means an exact tie keeps the HIGHER entry index -- i.e. the car found
        // EARLIER in the 3,2,1,0 scan.
        int   best = -1;
        float bestAngle = kBigAngle;
        for (int k = n - 1; k >= 0; --k) {
            if (cand[k].angle < bestAngle) { bestAngle = cand[k].angle; best = k; }
        }
        if (best >= 0) {
            const Candidate& c = cand[best];
            // [UNCERTAIN] The original may MOVE the stored position before
            // committing it: on a car target it calls FUN_0041f300(car) for a
            // sub-index, and if FUN_0041f1c0(car, sub) is non-zero it runs the
            // intercept prediction FUN_0041f2c0(car, sub, &origin, &buf) and, on
            // success, replaces cand[best].pos with
            //   origin + normalise(targetPt - origin) * (buf.t * |targetPt - origin|)
            // where targetPt is (tgt.x, tgt.y + tgt.w*0.2, tgt.z) and 0.2 is
            // _DAT_005cc9c0. On refusal it instead runs FUN_00558b40. NEITHER is
            // ported: the capture records only 3 of the 4 position dwords (tgt.w
            // is missing) and nothing at all about FUN_0041f1c0's gate. What is
            // missing to resolve it: a capture channel for tgt.w and for
            // FUN_0041f1c0's return. NEITHER path makes a contact call, so this
            // cannot move a criterion-(c) COUNT -- only the written endpoint.
            for (int k = 0; k < 3; ++k) {
                rec->aimPoint[k] = c.pos[k];
                rec->endpoint[k] = c.pos[k];
            }
            rec->dist      = c.dist;
            rec->targetCar = c.carIdx;
            rec->hit       = 1;
        }
    }

    // --- the LINE-OF-SIGHT probe, 0x00459d18..0x00459e2f -------------------
    // Runs on BOTH branches: origin -> endpoint. A hit here cancels the lock.
    {
        const float seg[6] = { rec->origin[0], rec->origin[1], rec->origin[2],
                               rec->endpoint[0], rec->endpoint[1], rec->endpoint[2] };
        Contact::WorldHit hit;
        // CALL 0x4b4cd0 @0x00459d4f; TEST EAX,EAX @0x00459d57;
        // JE 0x459e2f @0x00459d59.
        if (Contact::SegmentQuery(seg, &hit, 0x00459d54u) != 0) {
            const float d[3] = { seg[3] - seg[0], seg[4] - seg[1], seg[5] - seg[2] };
            const float len = Len3(d);            // CALL 0x4c3ac0 @0x00459d93
            float p[3];
            // CALL 0x4b4650 @0x00459db0
            Contact::Vec3Lerp(p, seg, seg + 3, hit.t, 0x00459db5u);
            for (int c = 0; c < 3; ++c) p[c] += hit.normal[c] * kLift;
            rec->hit       = 1;
            rec->targetCar = -1;                  // the wall cancels the lock
            for (int c = 0; c < 3; ++c) rec->endpoint[c] = p[c];
            rec->aimPoint[0] = p[0];
            rec->aimPoint[1] = p[1];
            rec->dist        = len * hit.t;
            rec->aimPoint[2] = p[2];
        }
    }

    // --- the THIRD loop, 0x00459f5e..0x0045a0dc ----------------------------
    // FUN_0047ce70 / FUN_0047d130 over RW atomics, with FUN_0055bde0 the actual
    // intersect. NOT ported. It makes NO contact call -- it can move the written
    // endpoint and rec+0x0c, never a criterion-(c) count.
}

}  // namespace Aim
}  // namespace Powerup
}  // namespace mashed_re
