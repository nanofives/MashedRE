// Mashed RE — the RenderWare fast-sqrt LUTs, rebuilt in-process.
//
// The standalone never runs RwEngineOpen, so the LUT chain *(0x007d3ff8)+*(0x007d3ffc) is
// garbage and Math/RwSqrt.cpp / RwV3dNormalize.cpp / Vec3.cpp take an exact-sqrt CPU fallback
// (RwLutGuard.h). That fallback is NOT the original's arithmetic.
//
// Generator, fitted 4096/4096 on BOTH tables against the live original's tables dumped
// 2026-10-09 (verify/d3_modes37_20261009/o_rwlut.{sqrt,invsqrt}.bin, scenario_launch
// --dump-rwlut; sentinels equal RwLutGuard.h's captured values). For table index i:
//   X       = float with exponent (i & 0x800 ? 0x7f : 0x80), mantissa (i & 0x7ff) << 12
//   sqrt[i] = bits(sqrtf(X))      - (((bits(X) + 0x800) >> 1) & 0x3fc00000)
//   inv[i]  = bits(1.0f/sqrtf(X)) - ((~(bits(X) + 0x800) >> 1) & 0x3fc00000)
// sqrtf and the divide are correctly rounded float operations: this TU is SSE2 (NOT in
// x87_tus.txt) so both round to float.
#include "RwLutTables.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace mashed_re {
namespace Math {

namespace {

struct Tables {
    std::uint32_t sqrt_[0x1000];
    std::uint32_t inv_[0x1000];
    Tables() {
        for (std::uint32_t i = 0; i < 0x1000u; ++i) {
            const std::uint32_t xb = ((i & 0x800u) ? 0x7fu : 0x80u) << 23 | (i & 0x7ffu) << 12;
            float x; std::memcpy(&x, &xb, 4);
            const float s = std::sqrt(x);
            const float r = 1.0f / s;
            std::uint32_t sb, rb; std::memcpy(&sb, &s, 4); std::memcpy(&rb, &r, 4);
            sqrt_[i] = sb - (((xb + 0x800u) >> 1) & 0x3fc00000u);
            inv_[i]  = rb - ((~(xb + 0x800u) >> 1) & 0x3fc00000u);
        }
    }
};

const Tables& T() { static const Tables t; return t; }

inline float FromBits(std::uint32_t b) { float f; std::memcpy(&f, &b, 4); return f; }
inline std::uint32_t Bits(float f) { std::uint32_t b; std::memcpy(&b, &f, 4); return b; }

}  // namespace

const std::uint32_t* RwLutSqrtTable()    { return T().sqrt_; }
const std::uint32_t* RwLutInvSqrtTable() { return T().inv_; }

// 0x004c3b30: zero guard, then table[(b+0x800)>>12 & 0xfff] + ((b+0x800)>>1 & 0x3fc00000).
float LutSqrt(float x) {
    const std::uint32_t b = Bits(x);
    if (b == 0u) return 0.0f;
    const std::uint32_t biased = b + 0x800u;
    return FromBits(T().sqrt_[(biased >> 12) & 0xfffu] + ((biased >> 1) & 0x3fc00000u));
}

// [pontoon 2026-10-16] FUN_004c3b90's arithmetic with the real LUT (0x004c3b9e: 0 -> 0; 0x004c3bb4 +0x800; index
// (>> 12) & 0xfff at 0x004c3bc2 into the second table [devBase+4]; 0x004c3bd2..dc + ((~biased >> 1) & 0x3fc00000)).
float LutInvSqrt(float x) {
    const std::uint32_t b = Bits(x);
    if (b == 0u) return x;
    const std::uint32_t biased = b + 0x800u;
    return FromBits(T().inv_[(biased >> 12) & 0xfffu] + ((~biased >> 1) & 0x3fc00000u));
}

// Math::LutNormalize (the real-LUT form of FUN_004c39b0) lives in Math/RwV3dNormalize.cpp since D4-DC-MATH-39b0
// (2026-10-25): one body for that RVA, called with these tables.

// LUT form of RwV3dLength (the body is Race/RaceCamera.cpp's Vec3Mag, which keeps the exact
// fallback; this helper is the arithmetic the original runs at 0x004c3ac0): 0x004c3ac4..0x004c3ada
// form (x*x + y*y) + z*z in ST and store it
// as a dword (0x004c3adc); zero returns 0 (0x004c3aec); otherwise the same LUT sqrt as 0x004c3b30
// (0x004c3af2..0x004c3b17). PC_24 caller (the race loop), so float arithmetic is the original's.
// [d2lateral 2026-10-16] bit-exact against the original's +0x9e4 refresh on two captured rows
// (verify/d2lateral_20261016/PREREG_d2lateral.md A2.2).
float LutLength(const float* v) {
    const float mag2 = (v[0] * v[0] + v[1] * v[1]) + v[2] * v[2];
    return LutSqrt(mag2);
}

// The cdecl call-throughs the naked lanes call (RwLutTables.h). D4-DC-INC-batch3 deleted the private naked copies
// TG_4c3b30, BW_4c3b90 / PP_4c3b90 and AL_ / TG_ / PK_4c3ac0. Each copy read the same rebuilt tables (its spec
// patched [0x007d3ff8] to a per-lane pointer pair set from RwLutSqrtTable / RwLutInvSqrtTable) and ran the same
// integer index arithmetic, then FLD of the dword: the result bits are those of LutSqrt / LutInvSqrt. LutLength
// rounds each product and sum to float (SSE2 TU), which is what the x87 copies did at the lanes' default PC_24.
extern "C" float __cdecl RwLutSqrt_cd(float x) { return LutSqrt(x); }
extern "C" float __cdecl RwLutInvSqrt_cd(float x) { return LutInvSqrt(x); }
extern "C" float __cdecl RwLutLength_cd(const float* v) { return LutLength(v); }

// The 2-D normalize the original runs at 0x004c3c60 (decomp: local_8 = p[1]*p[1] + p[0]*p[0] stored as a
// float; both LUTs indexed by (bits + 0x800) >> 12; out = (scale * p[0], p[1] * scale)); the degenerate-length
// error report (FUN_004d7ff0 / FUN_004d8480) changes no output and is omitted. Returns the length.
// [gridyaw 2026-10-16] FUN_00408b00's grid yaw runs its record direction through this: on Training's node 0 the
// LUT inverse sqrt of 1.0001527 is exactly 1.0, which is what lets the dot clamp to -1 and the yaw be 180.0
// (verify/gridyaw_20261016/PREREG_gridyaw.md A1.2).
float LutNormalize2D(float* out, const float* in) {
    const float mag2 = in[1] * in[1] + in[0] * in[0];
    float len = 0.0f, scale = 0.0f;
    const std::uint32_t b = Bits(mag2);
    if (b != 0u) {
        const std::uint32_t biased = b + 0x800u;
        len   = FromBits(T().sqrt_[(biased >> 12) & 0xfffu] + ((biased >> 1) & 0x3fc00000u));
        scale = FromBits(T().inv_[(biased >> 12) & 0xfffu] + ((~biased >> 1) & 0x3fc00000u));
    }
    out[0] = scale * in[0];
    out[1] = in[1] * scale;
    return len;
}

// [D4-DC-INC-SQRT-FOLD 2026-11-01] the knob and its log (RwLutTables.h).
bool SqrtFoldOn() {
    static const bool s = [] { const char* e = std::getenv("MASHED_SQRT_FOLD"); return e && e[0] && e[0] != '0'; }();
    return s;
}

static const char* SqrtFoldLogPath() {
    static const char* p = [] { const char* e = std::getenv("MASHED_SQRT_FOLD_LOG"); return (e && e[0]) ? e : nullptr; }();
    return p;
}

bool SqrtFoldLogOn() { return SqrtFoldLogPath() != nullptr; }

// One line per hook each time its call count reaches a power of two: calls so far, and how many of them returned
// bits that differ from the pre-fold form (the std::sqrt fallback in the exe, the live table in the .asi).
void SqrtFoldNote(int hook, float folded, float twoForm) {
    static const char* const kName[3] = { "FastSqrt", "FastInvSqrt", "Vec3Magnitude" };
    static unsigned long long s_calls[3], s_differ[3];
    if (hook < 0 || hook > 2) return;
    const unsigned long long n = ++s_calls[hook];
    if (Bits(folded) != Bits(twoForm)) ++s_differ[hook];
    if ((n & (n - 1)) != 0) return;
    if (FILE* f = std::fopen(SqrtFoldLogPath(), "a")) {
        std::fprintf(f, "%s calls=%llu differ=%llu\n", kName[hook], n, s_differ[hook]);
        std::fclose(f);
    }
}

}  // namespace Math
}  // namespace mashed_re
