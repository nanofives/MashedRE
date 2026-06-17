// Mashed RE — WS-A6a: velocity / angular-velocity integration step FUN_00467650.
//
// STATUS: verbatim port — spine + per-wheel CONTACT-FRAME suspension block (#4) + the
// cross-product friction block (#5) + the angular/linear integration tail + the trailing
// speed-limit grip-clamp (#6) are now all transcribed (WS-A6-COMPLETE). PENDING diff-original
// C4. Residual uncertainties (resolve at A-VERIFY-2):
//   [U-A6A-FLOAT10] RESOLVED (WS-A-VERIFY-3, 2026-06-17, asm 0x004682c0.. pool11):
//     the grip (l_60) and normal-load (l_d0) accumulators — and every per-vector
//     magnitude feeding them — are stored back to the stack as 32-bit `FSTP float
//     ptr [ESP+..]` EACH iteration (not qword/tbyte); the x87 80-bit width is purely
//     a TRANSIENT of one `a*b+c` step in ST0 before it rounds to float32 on store.
//     So `double` intermediates round to the same float32 (closer than the 80-bit
//     transient, differ by <=1 ULP in rare cases). For the standalone body this is
//     moot (the fn is bit-distinct by construction — it calls the std::sqrt LUT
//     fallback, not the RW fast-sqrt LUT). For an .asi-only verbatim hook the TU must
//     compile x87 (build.bat does — no /arch:SSE2) so the transient stays 80-bit.
//   [U-A6A-ST0] FUN_004a2c48 (=round-of-ST0) calls take an implicit ST0 input not in the
//     decomp; the gearbox/boost timers + brake quantizer use Vc_RoundST0() (stub=0).
//   [U-A6A-GLOB] g_suspScale (_DAT_0088e5f0) is 0 in the standalone stub -> the suspension
//     force magnitude is inert until A8 binds the real per-frame value; the .asi reads it live.
//   The transform (FUN_004c3df0) is now the CPU device-transform (WS-A-DEVXFORM); the
//   FUN_004c4d20 matrix build is mode 0 (standalone-correct).
//
// Operates on the vehicle record (original unaff_ESI). Constants memory_read (Ghidra pool11).
// Anchored MASHED.exe SHA-256 BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E.
#include "ForceIntegrator.h"
#include <cstdint>
#include <cstring>
#include <cmath>

namespace mashed_re {
namespace Vehicle {

int  Fi_GameMode();        // FUN_0040e350
int  Vc_RoundST0();        // FUN_004a2c48 (round ST0 -> long) — input implicit [U-A6A-ST0]

static inline float Rf(void* b, int off) { float v; std::memcpy(&v, (char*)b + off, 4); return v; }
static inline void  Wf(void* b, int off, float v) { std::memcpy((char*)b + off, &v, 4); }
static inline int   Ri(void* b, int off) { int v; std::memcpy(&v, (char*)b + off, 4); return v; }
static inline void  Wi(void* b, int off, int v) { std::memcpy((char*)b + off, &v, 4); }
static inline float Rp(int* p, int k) { float v; std::memcpy(&v, p + k, 4); return v; }
static inline void  Wp(int* p, int k, float v) { std::memcpy(p + k, &v, 4); }
static inline float Mag3(float a, float b, float c) { float t[3] = {a,b,c}; return Vec3Mag3(t); }

namespace a6 {
constexpr float kZero=0.0f, kHalf=0.5f, k0p25=0.25f, k0p01=0.01f, k0p1=0.1f, k10=10.0f, k0p99=0.98999f;
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
constexpr float kBrake=0.9f;            // 005cc9c8
// contact-frame block (#4)
constexpr float kAngLo=-9.9954e-6f;     // 005cea1c
constexpr float kAngHi=9.98e-6f;        // 005cc990
constexpr float k0p008=0.008f;          // 005cea14
constexpr float k8=8.0f;                // 005cc9f4
constexpr float k0p019877=0.019877f;    // 005cea18
constexpr float k360=360.0f;            // 005ccac4
constexpr float k270=270.0f;            // 0x43870000
constexpr float k1024=1024.0f;          // 005cea10
constexpr float k0p0009766=0.0009766f;  // 005cea0c
constexpr float k50=50.0f;              // 005cd120
constexpr float k0p02=0.02f;            // 005ce18c
constexpr float k0p85=0.85f;            // 005ce264
constexpr float k3=3.0f;                // 005cc31c
constexpr float k0p0019531=0.0019531f;  // 005cea08
constexpr float k1p1=1.1f;              // 005ccabc
// tail
constexpr float kNormAccum=50.24f;      // 005cea04
constexpr float kAngMul=9.2549e-7f;     // 005cea00
constexpr float k1p5=1.5f;              // 005cc348
constexpr float k32768=32768.0f;        // 005ce9fc
constexpr float k1e7=1.0e7f;            // 005ce9f8
constexpr float k9p9998e8=9.9998e-8f;   // 005ce9f4
constexpr float k3p0518e5=3.0518e-5f;   // 005ce9f0
constexpr float k16=16.0f;              // 005cc750
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
    double l_d0=0.0, l_60=0.0;

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
    // gearbox state machine (+0x490 gear int, +0x494 timer int)
    int gear = Ri(v, 0x490);
    if (iVar11 != gear) {
        if (Ri(v, 0x494) == 0 && gear < iVar11) { Wi(v, 0x494, 3000); Wi(v, 0x490, gear + 1); }
        if (iVar11 < Ri(v, 0x490)) { Wi(v, 0x494, (int)0xfffff448); Wi(v, 0x490, iVar11); }
    }
    gear = Ri(v, 0x490);
    float local_cc = local_54[(gear >= 0 && gear < 6) ? gear : 0];
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

    // per-wheel loop (4 wheels, piVar12 = self + 0x1a4 ints, stride 0x31 ints)
    int* p = self + (0x1a4 / 4);
    for (int wheel = 0; wheel < 4; ++wheel, p += 0x31) {
        // drive-force block (committed mode 2 + active)
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
            // [U-A6A-ST0] boost-state machine (+0xbf8 == 1 / == 2) gated on Vc_RoundST0 — shape only.
        }
        { int r = Vc_RoundST0(); p[0] = r; }                  // *piVar12 = round(ST0) [U-A6A-ST0]
        if (local_58 == 0 || p[-0xf] != 2) { p[-1] = 0; }
        else { int p1 = p[-1] + 1; p[-1] = p1; if (p1 < 0x200) p[-1] = 0x200; p[0] = p[0] + (p[-1] - 0x200) * 0x40; }

        // ===== contact-frame block #4 (if wheel active p[-3] != 0) =====
        if (p[-3] != 0) {
            // brake handling: input[5] (brake) set, input[4] (accel) clear
            if (input[5] != 0 && input[4] == 0) {
                Wi(v, 0xb20, 1);
                if (p[0] < 1) {
                    if (p[-0xf] == 2) {
                        float bf = (float)(unsigned)input[5];
                        if (Ri(v, 0x28) != 0) bf = (float)(100 - Ri(v, 0x28)) * bf * k0p01;
                        if (k160 < bf) bf = k160;
                        bf = -(local_cc * kBrake * bf);
                        Wp(p, 0x1c, bf * Rp(p,0x1f) + Rp(p,0x1c));
                        Wp(p, 0x1d, bf * Rp(p,0x20) + Rp(p,0x1d));
                        Wp(p, 0x1e, bf * Rp(p,0x21) + Rp(p,0x1e));
                    }
                } else if (wheel > 1) {
                    p[-1] = Vc_RoundST0();
                }
            }
            // orientation/spin check: any ang-vel component outside [kAngLo, kAngHi]
            float le0, ldc, ld8, le4;
            float avx = Rf(v,0x9bc), avy = Rf(v,0x9c0), avz = Rf(v,0x9c4);
            if (avx < kAngLo || kAngHi < avx || avy < kAngLo || kAngHi < avy || avz < kAngLo || kAngHi < avz) {
                float m40[16]; Rw_MatrixFromAxisAngle(m40, (const float*)((char*)v + 0x9bc), k270, 0);  // FUN_004c4d20 mode 0
                float src[3] = { Rp(p,-9), Rp(p,-8), Rp(p,-7) }, dst[3];
                Rw_TransformPoints(dst, src, 1, m40);          // FUN_004c3df0 (CPU device-transform)
                le0 = dst[0]; ldc = dst[1]; ld8 = dst[2];
                float f = Rf(v,0x9e4) * k0p008;
                if (f < k8) f = k8;
                f = f * Rf(v,0x9e8) * k0p019877 * k360;
                le0 = le0*f - Rf(v,0x9b0); ldc = ldc*f - Rf(v,0x9b4); ld8 = ld8*f - Rf(v,0x9b8);
                le4 = Mag3(le0, ldc, ld8);
            } else {
                le4 = Rf(v,0x9e4); le0 = -Rf(v,0x9b0); ldc = -Rf(v,0x9b4); ld8 = -Rf(v,0x9b8);
            }
            // all-grounded suspension force (writes per-wheel force p[0x1c..0x1e])
            if (kSpeedMin < le4 && Ri(v, 0x9e0) == 0x40800000) {
                float inv = 1.0f / le4;
                float s0 = le0 * inv, s1 = ldc * inv;
                if (k1024 < le4) le4 = k1024;
                float l98 = 0, l9c = 0, la0 = 0;
                float lbc = Rp(p,0x15) * Rp(p,0x1b) * g_suspScale * le4 * k0p0009766;
                float lc0 = Rf(v,0x9dc)*inv*ld8 + Rf(v,0x9d4)*s0 + Rf(v,0x9d8)*s1;
                float lc8 = lc0 * Rp(p,0x1f), lc4 = lc0 * Rp(p,0x20); lc0 = lc0 * Rp(p,0x21);
                float lac = s0 - lc8, la8 = s1 - lc4, la4 = inv*ld8 - lc0;
                float ld4 = Mag3(lac, la8, la4);
                unsigned l94 = (unsigned)p[-1];
                l_60 = (double)ld4 * (double)le4 + l_60;             // [U-A6A-FLOAT10]
                if ((l94 & 0x100) == 0) {
                    float f5 = lbc;
                    if (l94 == 0) {
                        l98 = Rp(p,0x16) * Rp(p,0x1b) * g_suspScale;
                        la0 = lc8 * l98; l9c = lc4 * l98; l98 = l98 * lc0;
                        float f4 = ld4 * Rf(v,0x9e4);
                        if (f4 < k50) f5 = f4 * k0p02 * lbc;
                    }
                    la0 = lac * f5 + la0; l9c = la8 * f5 + l9c; f5 = f5 * la4 + l98;
                    Wp(p, 0x1c, la0 + Rp(p,0x1c)); Wp(p, 0x1d, l9c + Rp(p,0x1d)); Wp(p, 0x1e, f5 + Rp(p,0x1e));
                } else {
                    float f5 = lbc * k0p85;
                    float f4 = Rp(p,0x1b) * k3 * g_suspScale * (float)(int)l94 * k0p0019531 * k1p1;
                    if (bGrip) f4 = f4 * kHalf;
                    la0 = lc8 * f4 + lac * f5; l9c = lc4 * f4 + la8 * f5; f5 = lc0 * f4 + f5 * la4;
                    Wp(p, 0x1c, la0 + Rp(p,0x1c)); Wp(p, 0x1d, l9c + Rp(p,0x1d)); Wp(p, 0x1e, f5 + Rp(p,0x1e));
                }
            }
        }

        // ===== cross-product friction block #5 (per-wheel force -> torque accum) =====
        if (kSpeedMin < Mag3(Rp(p,0x1c), Rp(p,0x1d), Rp(p,0x1e))) {
            float inv = 1.0f / Rp(p,-0xb);
            float d0 = Rp(p,-9)*inv, d1 = Rp(p,-8)*inv, d2 = Rp(p,-7)*inv;
            float f0 = Rp(p,0x1c), f1 = Rp(p,0x1d), f2 = Rp(p,0x1e);
            float dot = d2*f2 + d1*f1 + d0*f0;
            float c8 = d0*dot, c4 = d1*dot, c0 = d2*dot;
            float ac = f0 - c8, a8 = f1 - c4, a4 = f2 - c0;
            float x84 = a4*c4 - a8*c0, x80 = ac*c0 - a4*c8, x7c = a8*c8 - ac*c4;
            if (0.0f < dot) { x84 = -x84; x80 = -x80; x7c = -x7c; }
            l_b8 += c8; l_b4 += c4; l_b0 += c0;
            l_6c += f0; l_68 += f1; l_64 += f2;
            float wm = Rp(p,-0xb);
            double m15 = (double)Mag3(ac*wm, a8*wm, a4*wm);
            l_d0 = m15 * (double)kNormAccum + l_d0;                  // [U-A6A-FLOAT10]
            double m16 = (double)Mag3(x84, x80, x7c);
            if ((double)kSpeedMin < m16) {
                double r = (m15 * (double)kNormAccum) / m16;
                l_78 = (float)((double)l_78 - (double)x84 * r);
                l_74 = (float)((double)l_74 - (double)x80 * r);
                l_70 = (float)((double)l_70 - (double)x7c * r);
            }
        }
    }

    // angular-velocity integration (+0x9bc..) when state +0x10 == 0
    float adt = dt * Rf(v, 0x5c) * kAngMul;
    if (Ri(v, 0x10) == 0) {
        Wf(v, 0x9bc, l_78 * adt + Rf(v,0x9bc));
        Wf(v, 0x9c0, l_74 * adt + Rf(v,0x9c0));
        Wf(v, 0x9c4, l_70 * adt + Rf(v,0x9c4));
    }
    // redistribute the normal-load torque by (l_d0 - |l_78,l_74,l_70|)/l_d0
    double m78 = (double)Mag3(l_78, l_74, l_70);
    double lin_b0;
    if (l_d0 - m78 == 0.0) {
        lin_b0 = (double)l_b0;
    } else {
        double f = (l_d0 - m78) / l_d0;
        float e0 = l_6c - l_b8, dc = l_68 - l_b4, d8 = l_64 - l_b0;
        l_b8 = (float)((double)e0 * f + (double)l_b8);
        l_b4 = (float)((double)dc * f + (double)l_b4);
        lin_b0 = (double)d8 * f + (double)l_b0;
    }
    float lb0f = (float)((double)Rf(v,0xb1c) + lin_b0);
    // linear-velocity integration (+0x9b0..) by mass*dt over (control force + accum)
    float linTerm = dt * Rf(v, 0x54) * kDt;
    Wf(v, 0x9b0, linTerm * (Rf(v,0xb14) + l_b8) + Rf(v,0x9b0));
    Wf(v, 0x9b4, linTerm * (Rf(v,0xb18) + l_b4) + Rf(v,0x9b4));
    Wf(v, 0x9b8, linTerm * lb0f + Rf(v,0x9b8));
    float speed = Vec3Mag3((const float*)((char*)v + 0x9b0));
    int tId = Ri(v, 0x1f0);
    float grip = (float)(l_60 / (double)Rf(v, 0x18c));
    Wf(v, 0x9e4, speed);
    // track-id grip scaling
    if (tId == -0x5f7f80) {
        grip = grip * k1p5;
    } else {
        bool doDouble = (tId == -0x557f80);
        if (!doDouble) {
            if (tId == -0x69e1a6 || tId == -0xe17f4c) grip = grip * (Ri(v,0xd00) == 0 ? k3 : k1p5);
            if (tId == -0x37e1a6) doDouble = true;
        }
        if (doDouble) grip = grip + grip;
    }
    if (Ri(v, 0x2c) != 0) grip = ((float)Ri(v,0x2c) * k0p01 + 1.0f) * grip;   // 005cc328
    if (Ri(v, 0x34) != 0) grip = ((float)Ri(v,0x34) * k0p1  + 1.0f) * grip;   // 005cc56c

    // ===== trailing speed-limit grip-clamp #6 (all-grounded) =====
    if (speed != 0.0f && Ri(v, 0x9e0) == 0x40800000) {
        float fwdDot = Rf(v,0x9dc)*Rf(v,0x9b8) + Rf(v,0x9d4)*Rf(v,0x9b0) + Rf(v,0x9d8)*Rf(v,0x9b4);
        float fx = Rf(v,0x9b0) - fwdDot*Rf(v,0x9d4);   // lateral velocity
        float fy = Rf(v,0x9b4) - fwdDot*Rf(v,0x9d8);
        float fz = Rf(v,0x9b8) - fwdDot*Rf(v,0x9dc);
        grip = grip * speed;
        if (k32768 < grip) {
            float k = (k1e7 - grip) * k9p9998e8;
            if (k < 0.0f) k = 0.0f;
            k = k * k0p1 + k * k0p1;                    // *0.2
            Wf(v,0x9b0, Rf(v,0x9b0) - fx*k); Wf(v,0x9b4, Rf(v,0x9b4) - fy*k); Wf(v,0x9b8, Rf(v,0x9b8) - fz*k);
            k = 1.0f - k;
            Wf(v,0x9bc, k*Rf(v,0x9bc)); Wf(v,0x9c0, k*Rf(v,0x9c0)); Wf(v,0x9c4, k*Rf(v,0x9c4));
            return;
        }
        float k = (k32768 - grip) * k3p0518e5;
        if (k < k0p1) k = k0p1;
        Wf(v,0x9b0, Rf(v,0x9b0) - fx*k); Wf(v,0x9b4, Rf(v,0x9b4) - fy*k); Wf(v,0x9b8, Rf(v,0x9b8) - fz*k);
        if (k < kHalf) k = kHalf;
        k = 1.0f - k;
        Wf(v,0x9bc, k*Rf(v,0x9bc)); Wf(v,0x9c0, k*Rf(v,0x9c0)); Wf(v,0x9c4, k*Rf(v,0x9c4));
        if (Ri(v,0xb20) == 0 && speed < k16) {         // parked at low speed -> full stop
            Wf(v,0x9b0,0.0f); Wf(v,0x9b4,0.0f); Wf(v,0x9b8,0.0f);
            Wf(v,0x9bc,0.0f); Wf(v,0x9c0,0.0f); Wf(v,0x9c4,0.0f);
            return;
        }
    }
    (void)param_1; (void)fVar3sel;
}

} // namespace Vehicle
} // namespace mashed_re
