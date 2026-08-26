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
#include <cstring>

namespace mashed_re {
namespace Vehicle {

int Fi_GameMode();        // FUN_0040e350

namespace {

// record field accessors (float / int by byte offset)
// Bit-exact float from the image's .rdata pattern — the same Cf() idiom
// Integrate2.cpp uses. AUDITED 2026-08-26 against the PE: five of the plain
// decimal literals below had the WRONG BITS (the hex in each comment was right,
// the decimal gloss was not — the documented "plate hex gloss" trap). Errors were
// 0.0005-0.02%, so this is a bit-exactness fix, NOT a behavioural one.
inline float Cf(std::uint32_t bits) { float f; std::memcpy(&f, &bits, 4); return f; }
inline float&        Fb(void* v, std::size_t o) { return *reinterpret_cast<float*>(static_cast<char*>(v) + o); }
inline std::int32_t& Ib(void* v, std::size_t o) { return *reinterpret_cast<std::int32_t*>(static_cast<char*>(v) + o); }

constexpr std::size_t kAngVel = 0x9bc;   // angular velocity triple (+0x9bc/+0x9c0/+0x9c4)

// 0x004c4680  thunk_FUN_004c4680 — the matrix re-orthonormalize FUN_0046e9e0 calls
// immediately after the row integration (a first-order dR = omega x R is not
// norm-preserving, so without this the basis drifts and scales without bound).
//
// Decoded 2026-08-25 (notes "Follow-up 2", Task D): normalize all three axis rows
// via FUN_004c3b90 (reciprocal length), select the most-orthogonal axis PAIR by
// comparing absolute pairwise dots, rebuild the remaining two axes with two
// normalized cross products, preserve the translation row, and set the RwMatrix
// flags to (flags & 0xfffdffff) | 3 — clearing INTERNAL-IDENTITY 0x20000 and
// setting ORTHOGONAL|ORTHONORMAL. Behaviour matches RwMatrixOrthoNormalize (the
// export name itself is [UNCERTAIN]; the image carries no symbol for it).
//
// [UNCERTAIN] the exact tie-break in the axis selection. The decode describes
// "selects the dominant/most-orthogonal axis by abs pairwise dots"; I keep the
// pair with the SMALLEST |dot| (the most nearly perpendicular pair) and derive
// the third from it, which is the standard formulation. If the original instead
// keys off the largest dot the visible difference is confined to already-degenerate
// bases. Confirm against 0x004c4680's comparison order before promoting past C2.
void MatrixOrthoNormalize(float* m) {
    auto norm3 = [](float* v) {
        const float n = std::sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
        if (n > 1e-20f) { const float r = 1.0f / n; v[0] *= r; v[1] *= r; v[2] *= r; }
    };
    auto dot3 = [](const float* a, const float* b) {
        return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
    };
    auto cross3 = [](float* d, const float* a, const float* b) {
        d[0] = a[1]*b[2] - a[2]*b[1];
        d[1] = a[2]*b[0] - a[0]*b[2];
        d[2] = a[0]*b[1] - a[1]*b[0];
    };

    float* right = m + 0;
    float* up    = m + 4;
    float* at    = m + 8;

    norm3(right); norm3(up); norm3(at);                    // FUN_004c3b90 on each row

    const float dRU = std::fabs(dot3(right, up));
    const float dUA = std::fabs(dot3(up,    at));
    const float dAR = std::fabs(dot3(at,    right));

    if (dRU <= dUA && dRU <= dAR) {
        // keep (right, up) -> at = right x up, then up = at x right
        cross3(at, right, up);   norm3(at);
        cross3(up, at, right);   norm3(up);
    } else if (dUA <= dAR) {
        // keep (up, at) -> right = up x at, then at = right x up
        cross3(right, up, at);   norm3(right);
        cross3(at, right, up);   norm3(at);
    } else {
        // keep (at, right) -> up = at x right, then right = up x at
        cross3(up, at, right);   norm3(up);
        cross3(right, up, at);   norm3(right);
    }

    // flags word at m[3]: clear INTERNAL-IDENTITY (0x20000), set ORTHOGONAL|ORTHONORMAL
    std::uint32_t* flags = reinterpret_cast<std::uint32_t*>(m + 3);
    *flags = (*flags & 0xfffdffffu) | 3u;
    // translation row m[12..14] is preserved (never touched here).
}

} // namespace

// Seed BOTH buffers of the +0x928 block with a yaw-only basis. The original gets
// its starting matrix from the live RW scene graph; the standalone has to plant
// one at spawn, and the spawn yaw is the only orientation information it has.
// Axis convention matches VehiclePhysicsRun.cpp BuildYawMatrix exactly:
//   at = (cos, 0, sin) ; up = (0,1,0) ; right = up x at = (sin, 0, -cos)
// DEVIATION FROM THE ORIGINAL, STATED PLAINLY: the original keeps this basis in
// the record's +0x928 double buffer (indices +0x9a8/+0x9ac). We keep it in
// caller-owned storage instead, because our port ALREADY uses that same +0x928
// block as the contact ring — SolveWheelContacts derives a centre pointer from
// `sel = I(r,0x9ac)` and writes through it (VehiclePhysicsRun.cpp:340-341), and
// VehicleControl.cpp:100 / PhysicsChainHooks.cpp:521 read it through +0x9a8. An
// earlier attempt that swapped those indices per substep regressed the drive
// (median speed 378 -> 7), so the basis is deliberately kept out of that block.
// The arithmetic below is unaffected; only the storage location differs. Moving
// it into +0x928 is a follow-up that requires reconciling the contact-ring
// buffer semantics first.
void BodyOrient_Init(float* m, float yaw) {
    const float c = std::cos(yaw), s = std::sin(yaw);
    m[0]  =  s;  m[1]  = 0.f; m[2]  = -c;  m[3]  = 0.f;   // right
    m[4]  = 0.f; m[5]  = 1.f; m[6]  = 0.f; m[7]  = 0.f;   // up
    m[8]  =  c;  m[9]  = 0.f; m[10] =  s;  m[11] = 0.f;   // at (forward)
    m[12] = 0.f; m[13] = 0.f; m[14] = 0.f; m[15] = 0.f;   // pos
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
void BodyOrient_IntegrateStep(float* m, const float omega[3]) {
    const float wx = omega[0], wy = omega[1], wz = omega[2];
    // Read the whole source basis BEFORE writing any of it: the original reads
    // from EDI and writes to a DISTINCT EBX buffer, so no row may observe a
    // partially-updated predecessor. Integrating in place without this snapshot
    // would silently change the law.
    float src[12];
    for (int i = 0; i < 12; ++i) src[i] = m[i];
    for (int row = 0; row < 3; ++row) {
        const std::size_t b = static_cast<std::size_t>(row) * 4;
        const float r0 = src[b + 0], r1 = src[b + 1], r2 = src[b + 2];
        m[b + 0] = (wy * r2 - wz * r1) + r0;
        m[b + 1] = (wz * r0 - wx * r2) + r1;
        m[b + 2] = (wx * r1 - wy * r0) + r2;
    }
    MatrixOrthoNormalize(m);                       // thunk_FUN_004c4680
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
    const     float kGripPerSpeed = Cf(0x3a2ec33e); // _DAT_005ce1e8 = 1/1500 (EXACT)
    constexpr float kOne          = 1.0f;         // _DAT_005cc320 0x3f800000
    constexpr float kTrimClamp    = 255.0f;       // _DAT_005cd04c 0x437f0000
    constexpr float kRevDot       = -0.1f;        // _DAT_005cd0fc 0xbdcccccd
    constexpr float kSpinScale    = 1.5f;         // _DAT_005cc348 0x3fc00000
    constexpr float kPct          = 0.01f;        // _DAT_005cc328 0x3c23d70a
    constexpr float kTorqueK      = 1.0000e-4f;   // _DAT_005cd03c 0x38d1b717
    const     float kDtK          = Cf(0x39aec33e); // _DAT_005cc948 = 1/3000 (EXACT)
    constexpr float kTorque100    = 100.0f;       // _DAT_00613108 0x42c80000
    constexpr float kSeedScale    = 0.5f;         // _DAT_005cc32c 0x3f000000
    constexpr float kDampNum      = 3000.0f;      // _DAT_005ccd08 0x453b8000 (same global
                                                  //   as the suspension numerator; confirmed
                                                  //   at this site, damp @0x0046edfb)
    constexpr float kDampK        = 4.0f;         // _DAT_005cc35c 0x40800000
    const     float kAccumK       = Cf(0x3b03126f); // _DAT_005ce018 (EXACT)
    const     float kSpinTerm     = Cf(0x3a03126f); // _DAT_005ce268 (EXACT)

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

    // FUN_0040e350()==7 && record+0x4==1 special case (CMP dword [ESI+4],1 @0x0046edaf,
    // scalar multiply @0x0046edc7). It REPLACES the local torque triple with a spin
    // term (it does NOT zero the accumulator below, which still adds on top).
    //
    // The vector is CONFIRMED from the instruction stream (notes "Follow-up 4"): the
    // scalar goes to a temp at 0x0046edcd, then each component of the +0x9c8 vector
    // is loaded and multiplied by it — 0x0046edd1/edd5 (x), 0x0046edd9/eddd/ede1 (y),
    // 0x0046ede5/ede9 (z). All three assigned, none preserved, no FADD of the prior
    // omega in the branch. It is NOT a basis row of the matrix, NOT the forward vec3
    // at +0x9d4, and NOT the world up axis — all three were checked and ruled out.
    if (Fi_GameMode() == 7 && Ib(rec, 0x4) == 1) {
        const float spin = (2.0f * dtMs) * kDtK * Fb(rec, 0x9e4) * kSpinTerm;
        w[0] = fwd[0] * spin; w[1] = fwd[1] * spin; w[2] = fwd[2] * spin;
    }

    // +0x144/+0x148/+0x14c persistent accumulator (ESI[0x51..0x53]), present in BOTH
    // arms because it sits after the ESI[4] gate. Transcribed from Q4:
    //   if (+0x9e0 == 0.0) accum += local;
    //   damp  = (3000.0 - dt*4.0) * kDtK;                     // multiply @0x0046edfb
    //   accum = (local + accum) * damp;                       // FMUL ST1 @0x0046ee15
    //   if (in[0]==0 && in[1]==0) omega += dt*0.0020001*accum;
    // NOTE the apparent double-add of `local`: it is added conditionally on the first
    // line and then again unconditionally inside the damp line. That is what the
    // decompilation shows, so it is transcribed as-is rather than "corrected" — a
    // tidier reading would be a behavioural change, not a fix. Flag if it ever looks
    // wrong in a diff.
    float* accum = &Fb(rec, 0x144);
    if (Fb(rec, 0x9e0) == 0.0f) {
        accum[0] += w[0]; accum[1] += w[1]; accum[2] += w[2];
    }
    const float damp = (kDampNum - dtMs * kDampK) * kDtK;
    accum[0] = (w[0] + accum[0]) * damp;
    accum[1] = (w[1] + accum[1]) * damp;
    accum[2] = (w[2] + accum[2]) * damp;
    if (in[0] == 0 && in[1] == 0) {                // add ONLY when not steering
        const float k2 = dtMs * kAccumK;
        w[0] += k2 * accum[0]; w[1] += k2 * accum[1]; w[2] += k2 * accum[2];
    }

    omegaOut[0] = w[0]; omegaOut[1] = w[1]; omegaOut[2] = w[2];
}

// Body heading from the integrated basis, inverse of BuildYawMatrix's
// at = (cos(yaw), 0, sin(yaw)).
float BodyOrient_Heading(const float* m) {
    return std::atan2(m[10], m[8]);
}

} // namespace Vehicle
} // namespace mashed_re
