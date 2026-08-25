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

// Body heading from the integrated basis, inverse of BuildYawMatrix's
// at = (cos(yaw), 0, sin(yaw)).
float BodyOrient_Heading(void* rec) {
    const float* m = MatAt(rec, Ib(rec, kBufSelA));
    return std::atan2(m[10], m[8]);
}

} // namespace Vehicle
