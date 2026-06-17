// Mashed RE — WS-A6a: velocity / angular-velocity integration step FUN_00467650.
//
// STATUS: verbatim port of the determinable SPINE, PENDING diff-original C4. This is the
// heaviest physics callee (~280-line x87 body). The genuinely ambiguous parts are marked
// [U-A6A...] and MUST be resolved by an asm-level register pass + diff-original before C4:
//   * float10 (80-bit x87) intermediates rendered as `double`/`float` — results may differ.
//   * wheel-block int* slots p[0x1c..0x21] hold FLOAT bit-patterns in force context
//     (ported via Rp/Wp reinterpret). [U-A6A-SLOT]
//   * FUN_004a2c48 (round-of-ST0) calls take an IMPLICIT ST0 input not in the decomp.
//     [U-A6A-ST0] — the gearbox/boost timers that depend on it are transcribed as the
//     clamp SHAPE only (the ST0 value is the prior-timer minus a dt term).
//   * the full per-wheel contact-FRAME block and the trailing speed-limit grip-clamp are
//     [U-A6A-CONTACT] — the dominant integrate spine (suspension force -> linear velocity)
//     IS transcribed; those two blocks need the asm. Track-id literals kept verbatim.
//
// Operates on the vehicle record (original unaff_ESI). Math callees = the C4'd Math/ ports.
// Constants memory_read (Ghidra pool11, read_only, 2026-06-16). Decomp: FUN_00467650.
// Anchored MASHED.exe SHA-256 BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E.
#include "ForceIntegrator.h"
#include <cstdint>
#include <cstring>
#include <cmath>

namespace mashed_re {
namespace Vehicle {

// Vec3 length / normalize via the ForceIntegrator.h inline helpers (Vec3Mag3 = 0x004c3ac0
// family, Vec3Norm3 = 0x004c39b0) — header-inline, no extra link surface.
int  Fi_GameMode();        // FUN_0040e350
int  Vc_RoundST0();        // FUN_004a2c48 (round ST0 -> long) — input implicit [U-A6A-ST0]

static inline float Rf(void* b, int off) { float v; std::memcpy(&v, (char*)b + off, 4); return v; }
static inline void  Wf(void* b, int off, float v) { std::memcpy((char*)b + off, &v, 4); }
static inline int   Ri(void* b, int off) { int v; std::memcpy(&v, (char*)b + off, 4); return v; }
static inline void  Wi(void* b, int off, int v) { std::memcpy((char*)b + off, &v, 4); }
static inline float Rp(int* p, int k) { float v; std::memcpy(&v, p + k, 4); return v; }

namespace a6 {
constexpr float kZero=0.0f, kHalf=0.5f, k0p25=0.25f, k0p01=0.01f, k10=10.0f, k0p99=0.98999f;
constexpr float kDt=3.33268e-4f;        // 005cc948
constexpr float kSpeedMin=9.99999e-5f;  // 005cd03c
constexpr float k1500=1500.0f;          // 005cd0ac
constexpr float k500=500.0f;            // 005ccd04
constexpr float k0p0167=0.0166667f;     // 005cd694
constexpr float k6p667e4=6.6667e-4f;    // 005ce1e8
constexpr float k149744=149744.0f;      // 005cea34
constexpr float k200000=200000.0f;      // 005cea30
constexpr float k349872=349872.0f;      // 005cea38
constexpr float k299488=299488.0f;      // 005cea2c
constexpr float k160=160.0f;            // 005cea3c
constexpr float k250=250.0f;            // 005cea40
} // namespace a6

// 0x00467650 — Vehicle_Integrate2(self, param_1, dt, wheelBlock(unused by body), input)
void Vehicle_Integrate2(int* self, int param_1, float dt, void* /*wheelBlock*/, std::uint8_t* input)
{
    using namespace a6;
    void* v = self;
    int mode = Fi_GameMode();

    Wf(v, 0x9e4, Vec3Mag3((const float*)((char*)v + 0x9b0)));   // linear speed -> +0x9e4
    Wf(v, 0x9e8, Vec3Mag3((const float*)((char*)v + 0x9bc)));   // angular speed -> +0x9e8

    float local_54[6] = {0,0,0,0,0,0};
    float l_b0=0,l_b4=0,l_b8=0, l_64=0,l_68=0,l_6c=0, l_70=0,l_74=0,l_78=0;

    float fVar5 = k1500 - Rf(v, 0xb0c);
    if (fVar5 < k500) fVar5 = k500;
    int iVar11 = 0;
    float fVar4 = Rf(v, 0x498) * kHalf * fVar5 * k6p667e4;
    fVar5 = fVar5 * k6p667e4 * Rf(v, 0x49c) * kHalf;
    for (int j = 0; j < 5; ++j) {
        float g = Rf(v, 0x478 + j * 4);
        if (g != kZero) {
            float t = (j == 0) ? ((Rf(v, 0x9e4) + k250) * Rf(v, 0x478)) : (g * Rf(v, 0x9e4));
            if (fVar5 < t) { t = fVar5; if (j != 4) iVar11++; }
            local_54[j] = ((fVar4 / g) / fVar5) * t + Rf(v, 0x9e4);
        }
    }
    // gearbox state machine (+0x490 gear int, +0x494 timer int 3000 / -3000)
    int gear = Ri(v, 0x490);
    if (iVar11 != gear) {
        if (Ri(v, 0x494) == 0 && gear < iVar11) { Wi(v, 0x494, 3000); Wi(v, 0x490, gear + 1); }
        if (iVar11 < Ri(v, 0x490)) { Wi(v, 0x494, (int)0xfffff448); Wi(v, 0x490, iVar11); }
    }
    gear = Ri(v, 0x490);
    float local_cc = local_54[(gear >= 0 && gear < 6) ? gear : 0];
    // [U-A6A-ST0] timer wind-down via round-of-ST0; clamp shape preserved
    if (Ri(v, 0x494) > 0) { int r = Vc_RoundST0(); if (r < 0) r = 0; Wi(v, 0x494, r); }
    if (Ri(v, 0x494) < 0) { int r = Vc_RoundST0(); if (r > 0) r = 0; Wi(v, 0x494, r); }

    // surface/grip selection from the +0x1f0 track-id literal (verbatim ints)
    float gripIn = (float)(unsigned)input[4];
    if (mode != 7) { if (k160 < gripIn) gripIn = k160; } else gripIn = kZero;
    int trackId = Ri(v, 0x1f0);
    float fVar4b = Rf(v, 0x478 + ((gear >= 0 && gear < 6) ? gear : 0) * 4);
    float fVar6 = Rf(v, 0x9e4) * k0p01 + k10;
    float fVar3sel;
    if (trackId == -0x5f7f80)      fVar3sel = k149744;
    else if (trackId == -0x557f80) fVar3sel = k200000;
    else {
        fVar3sel = k349872;
        if ((trackId == -0x69e1a6 || trackId == -0xe17f4c) && Ri(v, 0xd00) == 0) fVar3sel = k299488;
        if (trackId == -1 || trackId == -0x373738) {
            float f7 = (1.0f - dt * k0p0167 * k0p01) * k0p99;
            Wf(v, 0x9b0, f7 * Rf(v, 0x9b0)); Wf(v, 0x9b4, f7 * Rf(v, 0x9b4)); Wf(v, 0x9b8, f7 * Rf(v, 0x9b8));
        }
    }
    bool bGrip = fVar3sel < ((local_cc * fVar4b) / fVar6) * gripIn;
    if (bGrip) local_cc = local_cc * kHalf;
    unsigned local_58 = bGrip ? 1u : 0u;

    // per-wheel loop (4 wheels, piVar12 = self + 0x1a4, stride 0x31 ints)
    int* p = self + (0x1a4 / 4);
    for (int wheel = 0; wheel < 4; ++wheel, p += 0x31) {
        if (p[-0xf] == 2 && p[-3] != 0) {
            float drive = (float)(unsigned)input[4];
            if (mode == 7) drive = 0.0f;
            if (Ri(v, 0x28) != 0) drive = (float)(100 - Ri(v, 0x28)) * drive * k0p01;
            if ((trackId == -0x69e1a6 || trackId == -0xe17f4c) && kZero < Rf(v, 0x9e4)) {
                float n[3]; { float src[3] = { Rf(v,0x9b0), Rf(v,0x9b4), Rf(v,0x9b8) }; Vec3Norm3(n, src); }
                float fwd = Rf(v,0x9dc)*n[2] + Rf(v,0x9d4)*n[0] + Rf(v,0x9d8)*n[1];
                fwd = fwd*fwd; fwd = fwd*fwd; fwd = fwd*fwd;
                if (fwd < 0.0f) fwd = -fwd;
                if (fwd < k0p25) fwd = k0p25;
                if (Ri(v,0xd00) == 0 && fwd < kHalf) fwd = kHalf;
                drive = fwd * drive;
            }
            if (mode != 7 && drive != kZero) {
                Wi(v, 0xb20, 1);
                if (k160 < drive) drive = 160.0f;
                drive = drive * local_cc;
                Wf(v, 0xb14, Rp(p,0x1f) * drive + Rf(v,0xb14));
                Wf(v, 0xb18, Rp(p,0x20) * drive + Rf(v,0xb18));
                Wf(v, 0xb1c, Rp(p,0x21) * drive + Rf(v,0xb1c));
            }
            if (Fi_GameMode() == 6) { int r = Vc_RoundST0(); if (r > 3000) r = 3000; if (r < 0) r = 0; Wi(v, 0xbf4, r); }
            // [U-A6A-ST0] boost-state machine (+0xbf8 == 1 / == 2) omitted — gated on Vc_RoundST0.
        }
        { int r = Vc_RoundST0(); p[0] = r; }                  // *piVar12 = round(ST0) [U-A6A-ST0]
        if (local_58 == 0 || p[-0xf] != 2) { p[-1] = 0; }
        else { int p1 = p[-1] + 1; p[-1] = p1; if (p1 < 0x200) p[-1] = 0x200; p[0] = p[0] + (p[-1] - 0x200) * 0x40; }

        // [U-A6A-CONTACT] full contact-frame friction block here in the original
        // (+0x9e0 all-grounded fast path). The dominant suspension-force accumulation:
        if (kSpeedMin < Vec3Mag3((const float*)(p + 0x1c))) {
            float inv = 1.0f / Rp(p, -0xb);
            float d0 = Rp(p,-9)*inv, d1 = Rp(p,-8)*inv, d2 = Rp(p,-7)*inv;
            float dot = d2*Rp(p,0x1e) + d1*Rp(p,0x1d) + d0*Rp(p,0x1c);
            l_b8 += d0*dot; l_b4 += d1*dot; l_b0 += d2*dot;
            l_6c += Rp(p,0x1c); l_68 += Rp(p,0x1d); l_64 += Rp(p,0x1e);
        }
    }

    // integrate angular velocity (+0x9bc..) when state +0x10 == 0
    float angTerm = dt * Rf(v, 0x5c) * k6p667e4;
    if (Ri(v, 0x10) == 0) {
        Wf(v, 0x9bc, l_78 * angTerm + Rf(v,0x9bc));
        Wf(v, 0x9c0, l_74 * angTerm + Rf(v,0x9c0));
        Wf(v, 0x9c4, l_70 * angTerm + Rf(v,0x9c4));
    }
    // integrate linear velocity (+0x9b0..) by mass*dt over the accumulated force
    float linTerm = dt * Rf(v, 0x54) * kDt;
    Wf(v, 0x9b0, linTerm * (Rf(v,0xb14) + l_b8) + Rf(v,0x9b0));
    Wf(v, 0x9b4, linTerm * (Rf(v,0xb18) + l_b4) + Rf(v,0x9b4));
    Wf(v, 0x9b8, linTerm * (Rf(v,0xb1c) + l_b0) + Rf(v,0x9b8));
    Wf(v, 0x9e4, Vec3Mag3((const float*)((char*)v + 0x9b0)));
    (void)param_1; (void)l_64; (void)l_68; (void)l_6c; (void)fVar3sel;
    // [U-A6A-CONTACT] trailing speed-limit grip-clamp (005ce9fc/f8/f4/f0, 005cc750) deferred.
}

} // namespace Vehicle
} // namespace mashed_re
