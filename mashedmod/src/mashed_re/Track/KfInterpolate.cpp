// Mashed RE — Track/KfInterpolate.cpp
// 0x00532b80 RpHAnimStdKeyFrameInterpolate, the one body (lane kfdot, 2026-10-22; moved from Track/WarzonePontoon.cpp,
// where verify/pontoon_20261016/disasm_slerp.txt transcribed it). In both rsp files: the exe calls it from the boat and
// (MASHED_KF_INTERP) the Dump train, the .asi installs it over 0x00532b80 with the in-process A/B below.
//
// Anchor: MASHED.exe SHA-256 BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E (on .unpatched)
#include "KfInterpolate.h"
#include "../Math/RwLutTables.h"
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <cmath>
#ifndef MASHED_STANDALONE
#include <windows.h>
#include "../Core/HookSystem.h"
#endif

namespace mashed_re {
namespace Track {
namespace {

inline float FB(std::uint32_t u) { float f; std::memcpy(&f, &u, 4); return f; }
inline std::uint32_t BF(float f) { std::uint32_t u; std::memcpy(&u, &f, 4); return u; }

// .rdata constants, from the unpatched image (memory reads 2026-10-16, pontoon).
const float k1     = FB(0x3f800000u);   // _DAT_005cc320
const float kHalf  = FB(0x3f000000u);   // _DAT_005cc32c
const float k0     = FB(0x00000000u);   // DAT_005d757c
const float kDot   = FB(0x3f7fbe77u);   // _DAT_005cea88  0.999 slerp threshold
const float kPi    = FB(0x40490fdbu);   // _DAT_005ce2f4
const float kPio2  = FB(0x3fc90fdbu);   // _DAT_005cd108
const float kC0    = FB(0x40490fdau);   // _DAT_005d8dc0
const float kC4    = FB(0x3fc90fdau);   // _DAT_005d8dc4
const float kC8    = FB(0x33a22168u);   // _DAT_005d8dc8
const float kQ1    = FB(0x4019d139u);   // _DAT_005d8dcc
const float kQ2    = FB(0x4001572du);   // _DAT_005d8dd0
const float kQ3    = FB(0x3f303361u);   // _DAT_005d8dd4
const float kQ4    = FB(0x3d9dc62eu);   // _DAT_005d8dd8
const float kP1    = FB(0x3ea6b090u);   // _DAT_005d8ddc
const float kP2    = FB(0x3e4e0aa8u);   // _DAT_005d8de0
const float kP3    = FB(0x3d241146u);   // _DAT_005d8de4
const float kP4    = FB(0x3a4f7f04u);   // _DAT_005d8de8
const float kP5    = FB(0x3811ef08u);   // _DAT_005d8dec
const float kS0    = FB(0x3e2aaaabu);   // _DAT_005cc8f4 (also the acos P0)
const float kS1    = FB(0x3c088889u);   // _DAT_005cc8f8
const float kS2    = FB(0x39500d01u);   // _DAT_005cc8fc
const float kS3    = FB(0x3638ef1bu);   // _DAT_005cc900
const float kS4    = FB(0x32d72f34u);   // _DAT_005cc904
const float kS5    = FB(0x2f2ec9d3u);   // _DAT_005cc908

}  // namespace

// 0x00532b80 RpHAnimStdKeyFrameInterpolate(out, a, b, time, custom): disasm_slerp.txt. b's quaternion is negated IN PLACE
// when the dot is negative (0x00532bfd..0x00532c1c). Order of the dot: ((x + y) + z) + w, 0x00532b8f..0x00532bab.
// The acos branch for dot > 0.5 takes FUN_004c3b30's LUT sqrt (0x00532dc9), here Math::LutSqrt.
void KfInterpolateFields(float* oq, float* ot, const float* aq, const float* at, float atime, float* bq, const float* bt,
                         float btime, float time) {
    float dot = ((bq[0] * aq[0] + aq[1] * bq[1]) + bq[2] * aq[2]) + aq[3] * bq[3];                   // 0x00532b8f..ab
    const float u = (time - atime) / (btime - atime);                                                // 0x00532bad..bbc
    ot[0] = (bt[0] - at[0]) * u + at[0];                                                             // 0x00532bc0..cd
    ot[1] = (bt[1] - at[1]) * u + at[1];
    ot[2] = (bt[2] - at[2]) * u + at[2];
    if (dot < k0) {                                                                                  // 0x00532bf0 jp skip
        dot = -dot;
        bq[0] = -bq[0]; bq[1] = -bq[1]; bq[2] = -bq[2]; bq[3] = -bq[3];
    }
    float wa = k1 - u;                                                                               // 0x00532c1f..c29
    float wb = u;
    if (dot < kDot) {                                                                                // 0x00532c2d..c3a
        const std::uint32_t bits = BF(dot), ab = bits & 0x7fffffffu;
        float th;
        if (ab >= 0x3f800000u) {
            th = (static_cast<std::int32_t>(bits) > 0) ? k0 : kPi;                                   // 0x00532c56..c6d
        } else if (ab < 0x3f000000u) {
            if (ab <= 0x23000000u) th = kPio2;                                                       // 0x00532c84
            else {                                                                                   // 0x00532c91..d01
                const float z = dot * dot;
                const float p = ((((((kP5 * z + kP4) * z - kP3) * z + kP2) * z - kP1) * z + kS0) * z);
                const float q = (((z * kQ4 - kQ3) * z + kQ2) * z - kQ1) * z + k1;
                const float r = (p / q) * dot;
                const float w = dot - (kC8 - r);
                th = kC4 - w;
            }
        } else if (static_cast<std::int32_t>(bits) < 0) {                                            // 0x00532d10..dab
            const float zz = (dot + k1) * kHalf;
            const float s = Math::LutSqrt(zz);
            const float p = ((((((zz * kP5 + kP4) * zz - kP3) * zz + kP2) * zz - kP1) * zz + kS0) * zz);
            const float q = (((zz * kQ4 - kQ3) * zz + kQ2) * zz - kQ1) * zz + k1;
            const float r = (p / q) * s;
            const float w = s + (r - kC8);
            th = kC0 - (w + w);
        } else {                                                                                     // 0x00532db0..e6f
            const float zz = (k1 - dot) * kHalf;
            const float s = Math::LutSqrt(zz);
            const float df = FB(BF(s) & 0xfffff000u);
            const float p = ((((((zz * kP5 + kP4) * zz - kP3) * zz + kP2) * zz - kP1) * zz + kS0) * zz);
            const float q = (((zz * kQ4 - kQ3) * zz + kQ2) * zz - kQ1) * zz + k1;
            const float r = (p / q) * s;
            const float c = (zz - df * df) / (df + s);
            const float w = (r + c) + df;
            th = w + w;
        }
        auto sinp = [](float x) {                                                                    // 0x00532e71..ea9
            const float x2 = x * x;
            const float poly = ((((kS5 * x2 - kS4) * x2 + kS3) * x2 - kS2) * x2 + kS1) * x2 - kS0;
            return poly * (x2 * x) + x;
        };
        const float inv = k1 / sinp(th);                                                             // 0x00532eab fdivr
        wa = sinp(th * wa) * inv;                                                                    // 0x00532eb1..ef3
        wb = sinp(th * u) * inv;                                                                     // 0x00532ef9..f3b
    }
    oq[0] = wb * bq[0] + wa * aq[0];                                                                 // 0x00532f43..53
    oq[1] = wa * aq[1] + wb * bq[1];
    oq[2] = wb * bq[2] + wa * aq[2];
    oq[3] = wa * aq[3] + wb * bq[3];
}

}  // namespace Track
}  // namespace mashed_re

#ifndef MASHED_STANDALONE
// =====================================================================================================================
// .asi install + in-process A/B. MASHED_LEAF_SELFTEST=1: per call, out's and b's 36 bytes are snapshotted, the original
// runs twice through a trampoline (noo), then the shared body (ndiff); compared: out's q / t (7 dwords) and b's 9 dwords
// (b's quaternion is negated in place). The game keeps the shared body's output. MASHED_LEAF_SELFTEST_SWEEP=1 adds, once,
// synthetic key pairs over every branch (dot >= 0.999, the |dot| < 0.5 / tiny / > 0.5 acos forms, a negative dot) at
// four times. Log: leaf_selftest.log in the CWD (shared with the other leaf self-tests).
// Original layout: out (interpolated frame) q +0x08, t +0x18; key: time +0x04, q +0x08, t +0x18 (0x00532b8f..0x00532bcd,
// 0x00532f43..0x00532f8e). void, cdecl (out, a, b, time).
// =====================================================================================================================
namespace {
using mashed_re::Track::KfInterpolateFields;
inline std::uint32_t KfBits(float f) { std::uint32_t u; std::memcpy(&u, &f, 4); return u; }

// Trampoline: the 5-byte E9 covers PUSH EBX + PUSH ESI + the first 3 bytes of MOV ESI,[ESP+0x10]; re-run all three,
// resume at 0x00532b86 (PUSH EDI).
void* g_kfResume = reinterpret_cast<void*>(0x00532b86);
__declspec(naked) void __cdecl KfOrig(void*, void*, void*, float) {
    __asm {
        push ebx
        push esi
        mov  esi, dword ptr [esp + 10h]
        jmp  dword ptr [g_kfResume]
    }
}
void KfShared(unsigned char* out, unsigned char* a, unsigned char* b, float time) {
    float at, bt;
    std::memcpy(&at, a + 4, 4); std::memcpy(&bt, b + 4, 4);
    KfInterpolateFields(reinterpret_cast<float*>(out + 8), reinterpret_cast<float*>(out + 0x18),
                        reinterpret_cast<const float*>(a + 8), reinterpret_cast<const float*>(a + 0x18), at,
                        reinterpret_cast<float*>(b + 8), reinterpret_cast<const float*>(b + 0x18), bt, time);
}
int KfSelfTestOn() {
    static int v = -1;
    if (v < 0) { const char* s = std::getenv("MASHED_LEAF_SELFTEST"); v = (s && s[0] && s[0] != '0') ? 1 : 0; }
    return v;
}
void KfLog(const char* s) {
    HANDLE h = CreateFileA("leaf_selftest.log", FILE_APPEND_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE,
                           nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) return;
    DWORD wrote; WriteFile(h, s, (DWORD)std::strlen(s), &wrote, nullptr); CloseHandle(h);
}
// One A/B on (out, a, b, time); leaves the shared body's output in place.
void KfAB(unsigned char* out, unsigned char* a, unsigned char* b, float time, int* noo, int* nd, int* first) {
    unsigned char so[36], sb[36], o1[36], b1[36];
    std::memcpy(so, out, 36); std::memcpy(sb, b, 36);
    KfOrig(out, a, b, time);
    std::memcpy(o1, out, 36); std::memcpy(b1, b, 36);
    std::memcpy(out, so, 36); std::memcpy(b, sb, 36);
    KfOrig(out, a, b, time);
    *noo = (std::memcmp(out + 8, o1 + 8, 28) != 0) + (std::memcmp(b, b1, 36) != 0);
    std::memcpy(out, so, 36); std::memcpy(b, sb, 36);
    KfShared(out, a, b, time);
    *nd = 0; *first = -1;
    for (int i = 2; i < 9; ++i) if (std::memcmp(out + 4 * i, o1 + 4 * i, 4) != 0) { ++*nd; if (*first < 0) *first = 4 * i; }
    for (int i = 0; i < 9; ++i) if (std::memcmp(b + 4 * i, b1 + 4 * i, 4) != 0) { ++*nd; if (*first < 0) *first = 0x100 + 4 * i; }
}
long g_kfCalls = 0, g_kfNoo = 0, g_kfNdiff = 0, g_kfNeg = 0, g_kfLerp = 0;
void KfSweep() {
    const char* e = std::getenv("MASHED_LEAF_SELFTEST_SWEEP");
    if (!e || !e[0] || e[0] == '0') return;
    // a = (0, 0, 0, 1); b = (0.6 s, 0, 0.8 s, d) with s = sqrt(1 - d^2), so the dot is d.
    const float dots[] = { 0.99995f, 0.9995f, 0.99f, 0.8f, 0.6f, 0.5f, 0.45f, 0.2f, 1e-20f, 0.0f, -0.3f, -0.95f };
    const float times[] = { 0.0f, 0.25f, 0.5f, 0.9f };
    long cases = 0, noo = 0, ndf = 0;
    for (float d : dots) {
        const float s = (d * d < 1.0f) ? std::sqrt(1.0f - d * d) : 0.0f;
        for (float t : times) {
            unsigned char out[36] = {}, a[36] = {}, b[36] = {};
            const float aq[4] = { 0.0f, 0.0f, 0.0f, 1.0f }, bq[4] = { s * 0.6f, 0.0f, s * 0.8f, d };
            const float at3[3] = { 1.0f, -2.0f, 3.5f }, bt3[3] = { -4.0f, 0.5f, 7.25f };
            const float ta = 2.0f, tb = 3.0f, tm = ta + t;
            std::memcpy(a + 4, &ta, 4); std::memcpy(a + 8, aq, 16); std::memcpy(a + 0x18, at3, 12);
            std::memcpy(b + 4, &tb, 4); std::memcpy(b + 8, bq, 16); std::memcpy(b + 0x18, bt3, 12);
            int n1, n2, f;
            KfAB(out, a, b, tm, &n1, &n2, &f);
            ++cases; noo += (n1 != 0); ndf += (n2 != 0);
            if (n1 || n2) {
                char line[200];
                wsprintfA(line, "[sw%ld] fn=532b80 dot=%08x t=%08x noo=%d ndiff=%d firstOff=0x%x\r\n", cases, KfBits(d),
                          KfBits(tm), n1, n2, f < 0 ? 0 : f);
                KfLog(line);
            }
        }
    }
    char line[160];
    wsprintfA(line, "SWEEP fn=532b80 cases=%ld noo=%ld ndiff=%ld\r\n", cases, noo, ndf);
    KfLog(line);
}
__declspec(noinline) void __cdecl KfDispatch(unsigned char* out, unsigned char* a, unsigned char* b, float time) {
    if (!KfSelfTestOn() || !out || !a || !b) { KfShared(out, a, b, time); return; }
    static LONG s_swept = 0;
    if (InterlockedExchange(&s_swept, 1) == 0) KfSweep();
    float aq[4], bq[4];
    std::memcpy(aq, a + 8, 16); std::memcpy(bq, b + 8, 16);
    const float dot0 = ((bq[0] * aq[0] + aq[1] * bq[1]) + bq[2] * aq[2]) + aq[3] * bq[3];
    int noo, nd, first;
    KfAB(out, a, b, time, &noo, &nd, &first);
    const long c = ++g_kfCalls;
    g_kfNoo += (noo != 0); g_kfNdiff += (nd != 0);
    const float ad = dot0 < 0.0f ? -dot0 : dot0;
    if (dot0 < 0.0f) ++g_kfNeg;
    if (!(ad < 0.999f)) ++g_kfLerp;
    char line[260];
    if (noo || nd || c <= 4) {
        wsprintfA(line, "[%ld] fn=532b80 dot=%08x t=%08x noo=%d ndiff=%d firstOff=0x%x\r\n", c, KfBits(dot0), KfBits(time),
                  noo, nd, first < 0 ? 0 : first);
        KfLog(line);
    }
    if (c <= 16 || (c & 0xff) == 0) {
        wsprintfA(line, "SUM fn=532b80 calls=%ld noo=%ld ndiff=%ld negdot=%ld lerp=%ld\r\n", c, g_kfNoo, g_kfNdiff, g_kfNeg,
                  g_kfLerp);
        KfLog(line);
    }
}
}  // namespace

// The installed entry and the path1 export.
extern "C" __declspec(dllexport) void __cdecl KfInterpolate532b80(unsigned char* out, unsigned char* a, unsigned char* b,
                                                                  float time) {
    KfDispatch(out, a, b, time);
}
RH_ScopedInstall(KfInterpolate532b80, 0x00532b80);
#endif
