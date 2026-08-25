// BodyOrientationIntegrate.cpp — the original's body-orientation integrator.
//
// 0x0046e9e0  FUN_0046e9e0 — per-tick writer of the vehicle body rotation.
//
// WHY THIS FILE EXISTS. The standalone had no body orientation at all: it
// synthesized a yaw matrix from io.yaw every frame (VehiclePhysicsRun.cpp
// BuildYawMatrix) and derived io.yaw itself from a first-order lag chasing the
// VELOCITY heading. That lag was justified in-source by citing FUN_0047eb30,
// which a Ghidra pass on 2026-08-25 showed is a separate DAT_006ce274-gated
// RWP-servo bridge that never touches +0x9bc/+0x9c0/+0x9c4 — i.e. the citation
// was misattributed and the real law was never ported. Evidence:
// re/analysis/data/A8_body_heading_law_20260825.md.
//
// THE ORIGINAL LAW. Register-passed: ESI = record, EDI = source matrix,
// EBX = destination matrix (the +0x928 block, selected by the double-buffer
// indices at +0x9a8/+0x9ac, 0x40 bytes per matrix).
//
//   omega  = (dt * _DAT_005cc32c) * (+0x9bc, +0x9c0, +0x9c4)     // 0x0046ea86 / 0x0046ea90
//   omega += dt * _DAT_005ce018  * (+0x144, +0x148, +0x14c)      // brake/handbrake gated
//   for each basis row r in {0, 1, 2}:
//       dst[r] = src[r] + (omega x src[r])                       // first-order dR = omega x R
//   thunk_FUN_004c4680()                                         // matrix cleanup / ortho
//
// The row form, verbatim from the decompilation:
//   *EBX   = (omega.y*EDI[2] - omega.z*EDI[1]) + EDI[0]
//   EBX[1] = (omega.z*EDI[0] - omega.x*EDI[2]) + EDI[1]
//   EBX[2] = (omega.x*EDI[1] - omega.y*EDI[0]) + EDI[2]
//
// STATUS — DELIBERATELY NOT WIRED YET. Nothing calls these functions, so this
// file cannot change behaviour. The omega SOURCE is still forked and I will not
// guess it: FUN_0046e9e0 gates on ESI[4] (record +0x10), keeping the
// +0x9bc/+0x9c0/+0x9c4 seed when it is nonzero and REBUILDING omega from wheel
// forces when it is zero. Which arm a normal driving car takes is recorded as
// [UNCERTAIN] in the notes file (its line 127) and is under a follow-up Ghidra
// query. Our own A6_DIAG measured record +0x10 == 0 on every sample, so the
// wheel-force arm may well be the live one — which is exactly why the omega
// vector is a PARAMETER here rather than being computed in this file. When the
// query lands, only the caller changes.

#include <cmath>
#include <cstddef>
#include <cstdint>

namespace mashed_re {
namespace Vehicle {

namespace {

// record field accessors (float / int by byte offset)
inline float&        Fb(void* v, std::size_t o) { return *reinterpret_cast<float*>(static_cast<char*>(v) + o); }
inline std::int32_t& Ib(void* v, std::size_t o) { return *reinterpret_cast<std::int32_t*>(static_cast<char*>(v) + o); }

constexpr std::size_t kBodyMatrices = 0x928;   // RwMatrix[] block, 0x40 bytes each
constexpr std::size_t kBufSelA      = 0x9a8;   // double-buffer index A (0x0047068e path)
constexpr std::size_t kBufSelB      = 0x9ac;   // double-buffer index B (FUN_0046f6c0 @0x0046f6d8)
constexpr std::size_t kAngVel       = 0x9bc;   // angular velocity triple (+0x9bc/+0x9c0/+0x9c4)

inline float* MatAt(void* rec, int sel) {
    return reinterpret_cast<float*>(static_cast<char*>(rec)
                                    + static_cast<std::size_t>(sel) * 0x40 + kBodyMatrices);
}

// thunk_FUN_004c4680 — called by FUN_0046e9e0 right after the row integration.
// [UNCERTAIN] the thunk's body is NOT decoded; only its call site is cited. A
// first-order dR = omega x R integration is not norm-preserving, so SOMETHING
// must renormalize or the basis drifts and scales without bound. Gram-Schmidt
// on (at, up) is the minimal correct stand-in and is marked as a substitute,
// not a transcription. Resolve by decompiling 0x004c4680 before promoting this
// function past C2.
void MatrixOrthoCleanup(float* m) {
    float* right = m + 0;
    float* up    = m + 4;
    float* at    = m + 8;
    auto norm3 = [](float* v) {
        const float n = std::sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
        if (n > 1e-20f) { v[0] /= n; v[1] /= n; v[2] /= n; }
    };
    norm3(at);
    // up := up - at*(up.at), renormalized
    const float d = up[0]*at[0] + up[1]*at[1] + up[2]*at[2];
    up[0] -= at[0]*d; up[1] -= at[1]*d; up[2] -= at[2]*d;
    norm3(up);
    // right := up x at (the convention BuildYawMatrix used: right = up x at)
    right[0] = up[1]*at[2] - up[2]*at[1];
    right[1] = up[2]*at[0] - up[0]*at[2];
    right[2] = up[0]*at[1] - up[1]*at[0];
    norm3(right);
}

} // namespace

// Seed BOTH buffers of the +0x928 block with a yaw-only basis. The original gets
// its starting matrix from the live RW scene graph; the standalone has to plant
// one at spawn, and the spawn yaw is the only orientation information it has.
// Axis convention matches VehiclePhysicsRun.cpp BuildYawMatrix exactly:
//   at = (cos, 0, sin) ; up = (0,1,0) ; right = up x at = (sin, 0, -cos)
void BodyOrient_Init(void* rec, float yaw) {
    const float c = std::cos(yaw), s = std::sin(yaw);
    for (int sel = 0; sel < 2; ++sel) {
        float* m = MatAt(rec, sel);
        m[0]  =  s;  m[1]  = 0.f; m[2]  = -c;  m[3]  = 0.f;   // right
        m[4]  = 0.f; m[5]  = 1.f; m[6]  = 0.f; m[7]  = 0.f;   // up
        m[8]  =  c;  m[9]  = 0.f; m[10] =  s;  m[11] = 0.f;   // at (forward)
        m[12] = 0.f; m[13] = 0.f; m[14] = 0.f; m[15] = 0.f;   // pos
    }
    Ib(rec, kBufSelA) = 0;
    Ib(rec, kBufSelB) = 1;
}

// One integration step: dst = src + (omega x src) per basis row, then cleanup,
// then swap the buffer selectors so dst becomes current.
//
// omega is passed in rather than derived here — see the STATUS note at the top.
// It is the CALLER's job to build it from the correct FUN_0046e9e0 arm.
//
// [UNCERTAIN] which selector the original treats as "current": the port reads
// the block through +0x9a8 at VehicleControl.cpp:100 / PhysicsChainHooks.cpp:521
// (citing 0x0047068e) but through +0x9ac at VehiclePhysicsRun.cpp:340, and
// FUN_0046f6c0 @0x0046f6d8 uses +0x9ac. Treating A as source and B as
// destination and swapping keeps both readers seeing a populated matrix; if the
// original's polarity is the other way this is a one-line correction.
void BodyOrient_IntegrateStep(void* rec, const float omega[3]) {
    const int selSrc = Ib(rec, kBufSelA);
    const int selDst = Ib(rec, kBufSelB);
    const float* src = MatAt(rec, selSrc);
    float*       dst = MatAt(rec, selDst);

    const float wx = omega[0], wy = omega[1], wz = omega[2];
    for (int row = 0; row < 3; ++row) {
        const std::size_t b = static_cast<std::size_t>(row) * 4;
        const float r0 = src[b + 0], r1 = src[b + 1], r2 = src[b + 2];
        dst[b + 0] = (wy * r2 - wz * r1) + r0;
        dst[b + 1] = (wz * r0 - wx * r2) + r1;
        dst[b + 2] = (wx * r1 - wy * r0) + r2;
        dst[b + 3] = src[b + 3];
    }
    dst[12] = src[12]; dst[13] = src[13]; dst[14] = src[14]; dst[15] = src[15];

    MatrixOrthoCleanup(dst);                       // thunk_FUN_004c4680 stand-in

    Ib(rec, kBufSelA) = selDst;                    // dst becomes current
    Ib(rec, kBufSelB) = selSrc;
}

// The omega seed the ESI[4]!=0 arm uses, exposed so the caller does not have to
// know the field offsets. scale is (dt * _DAT_005cc32c).
// [UNCERTAIN] _DAT_005cc32c's value is not confirmed here; Integrate2.cpp uses
// the same address as its 0.5 floor (kHalf), which is consistent with 0.5 but
// is not a decode of this site. The follow-up Ghidra query covers it.
void BodyOrient_OmegaFromAngVel(void* rec, float scale, float omegaOut[3]) {
    omegaOut[0] = scale * Fb(rec, kAngVel + 0);    // +0x9bc
    omegaOut[1] = scale * Fb(rec, kAngVel + 4);    // +0x9c0
    omegaOut[2] = scale * Fb(rec, kAngVel + 8);    // +0x9c4
}

// The ESI[4]==0 arm's omega rebuild — the arm a normal driving car takes.
// Verbatim from re/analysis/data/A8_body_heading_law_20260825.md sections Q3/Q4
// and "Follow-up 2" (constants read from the image, addresses in the comments).
//
// THE A8 MECHANISM LIVES HERE: grip = speed * (1/1500) clamped to 1.0, and both
// steer torques are multiplied by it, so the yaw torque is PROPORTIONAL TO SPEED
// and saturates at the clamp. A velocity-chasing lag cannot produce that shape.
//
// The rotation axis is record +0x9c8/+0x9cc/+0x9d0, which FUN_0046d700 returns
// (it takes a SLOT INDEX, not a matrix pointer — the earlier "body forward row"
// reading was wrong, corrected by the Follow-up 2 pass). Our port already writes
// that field as the averaged contact normal (VehiclePhysicsRun.cpp:349), i.e.
// roughly the ground up-axis, so torque about it IS yaw. [UNCERTAIN] whether the
// original keeps that vector unit-length; FUN_0046d700 does not normalize, so we
// use it as-is rather than imposing a normalization the original does not do.
void BodyOrient_OmegaFromSteer(void* rec, float dtMs, const std::uint8_t* in,
                               float omegaOut[3]) {
    constexpr float kGripPerSpeed = 6.6667e-4f;   // _DAT_005ce1e8 0x3a2ec33e (1/1500)
    constexpr float kOne          = 1.0f;         // _DAT_005cc320 0x3f800000
    constexpr float kTrimClamp    = 255.0f;       // _DAT_005cd04c 0x437f0000
    constexpr float kRevDot       = -0.1f;        // _DAT_005cd0fc 0xbdcccccd
    constexpr float kSpinScale    = 1.5f;         // _DAT_005cc348 0x3fc00000
    constexpr float kPct          = 0.01f;        // _DAT_005cc328 0x3c23d70a
    constexpr float kTorqueK      = 1.0000e-4f;   // _DAT_005cd03c 0x38d1b717
    constexpr float kDtK          = 3.334e-4f;    // _DAT_005cc948 0x39aec33e
    constexpr float kTorque100    = 100.0f;       // _DAT_00613108 0x42c80000
    constexpr float kSeedScale    = 0.5f;         // _DAT_005cc32c 0x3f000000

    const float* fwd = &Fb(rec, 0x9c8);           // FUN_0046d700 -> +0x9c8/+0x9cc/+0x9d0
    float w[3] = { 0.f, 0.f, 0.f };

    const bool bothPedals = (in[4] != 0 && in[5] != 0);

    float grip = Fb(rec, 0x9e4) * kGripPerSpeed;  // ESI[0x279] = linear speed magnitude
    if (grip > kOne) grip = kOne;
    Ib(rec, 0xcfc) = 0;                           // ESI[0x33f]
    if (bothPedals) { Ib(rec, 0xcfc) = 1; grip = kOne; }

    const std::int32_t c = Ib(rec, 0x30);         // ESI[0xc]
    const float cScale = c ? (kOne - (float)c * kPct) : kOne;

    if (Fb(rec, 0x9e0) != 0.0f) {                 // +0x9e0 != 0.0 — grounded/steerable
        const std::int32_t trimL = Ib(rec, 0x20); // ESI[8]
        const std::int32_t trimR = Ib(rec, 0x24); // ESI[9]
        // Byte -> float FIRST, then trim ADDED, then the 255 clamp (and the clamp
        // applies ONLY when the trim is nonzero — the raw byte is never clamped),
        // then the ESI[0xc] scale. Order matters; taken from Follow-up 2 Task C.
        float sL = (float)in[0];
        if (trimL) { sL = (float)in[0] + (float)trimL; if (sL > kTrimClamp) sL = kTrimClamp; }
        if (c) sL = cScale * sL;
        if (sL != 0.0f) {
            const float tL = sL * dtMs * kTorque100 * grip * kTorqueK * kDtK;
            w[0] = fwd[0] * tL; w[1] = fwd[1] * tL; w[2] = fwd[2] * tL;   // ASSIGN
        }
        float sR = (float)in[1];
        if (trimR) { sR = (float)in[1] + (float)trimR; if (sR > kTrimClamp) sR = kTrimClamp; }
        if (c) sR = cScale * sR;
        if (sR != 0.0f) {
            const float tR = sR * dtMs * kTorque100 * grip * kTorqueK * kDtK;
            w[0] -= fwd[0] * tR; w[1] -= fwd[1] * tR; w[2] -= fwd[2] * tR; // ADD, negated
        }
    }

    if (bothPedals) { w[0] *= kSpinScale; w[1] *= kSpinScale; w[2] *= kSpinScale; }

    // +0x9e0 != 4.0 (not all four wheels grounded): re-add the residual angular
    // velocity X and Z. NOTE the Y component (+0x9c0) is deliberately absent —
    // yaw on this arm comes from the steer differential, never from A6a.
    if (Ib(rec, 0x9e0) != 0x40800000) {
        const float k = dtMs * kDtK * kSeedScale;
        w[0] += k * Fb(rec, 0x9bc);
        w[2] += k * Fb(rec, 0x9c4);
    }

    // Reverse: forward(+0x9d4..) . velocity(+0x9b0..) below -0.1 flips the torque,
    // unless both pedals are down.
    const float dot = Fb(rec,0x9d4)*Fb(rec,0x9b0) + Fb(rec,0x9d8)*Fb(rec,0x9b4)
                    + Fb(rec,0x9dc)*Fb(rec,0x9b8);
    if (dot < kRevDot && !bothPedals) { w[0] = -w[0]; w[1] = -w[1]; w[2] = -w[2]; }

    // NOT PORTED, deliberately: the +0x144/+0x148/+0x14c accumulator term. Its damp
    // is `(_DAT_005ccd08 - dt*_DAT_005cc35c) * _DAT_005cc948` and _DAT_005cc35c has
    // no confirmed value yet, so porting it would mean inventing a constant. Its ADD
    // into omega is gated on BOTH steer bytes being zero (Q4), so it is a no-steer
    // settling term and does not affect the steering response A8 measures. Read
    // _DAT_005cc35c and finish this before promoting past C2.
    // Also NOT PORTED: the FUN_0040e350()==7 / +0x4==1 special case, which overrides
    // omega with a forward-axis spin needing _DAT_005ce268 (also unread).

    omegaOut[0] = w[0]; omegaOut[1] = w[1]; omegaOut[2] = w[2];
}

// Body heading from the integrated basis, inverse of BuildYawMatrix's
// at = (cos(yaw), 0, sin(yaw)).
float BodyOrient_Heading(void* rec) {
    const float* m = MatAt(rec, Ib(rec, kBufSelA));
    return std::atan2(m[10], m[8]);
}

} // namespace Vehicle
} // namespace mashed_re
