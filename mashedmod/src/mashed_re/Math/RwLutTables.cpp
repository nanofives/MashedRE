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

// [pontoon 2026-10-16] FUN_004c3b90's arithmetic with the real LUT (0x004c3b9e: 0 -> 0; 0x004c3bb4 +0x800;
// 0x004c3bc2 index (>> 12) & 0xfff into the second table [devBase+4]; 0x004c3bd2..dc + ((~biased >> 1) & 0x3fc00000)).
float LutInvSqrt(float x) {
    const std::uint32_t b = Bits(x);
    if (b == 0u) return x;
    const std::uint32_t biased = b + 0x800u;
    return FromBits(T().inv_[(biased >> 12) & 0xfffu] + ((~biased >> 1) & 0x3fc00000u));
}

// 0x004c39b0: mag2 = (x*x + y*y) + z*z in ST (0x004c39c4..0x004c39e2 FST dword);
// length and 1/length from the two tables; out = in * scale. The degenerate-length error
// report (FUN_004d7ff0 / FUN_004d8480) changes no output and is omitted.
float LutNormalize(float* out, const float* in) {
    // PC_24 form: the original's ST products and sum round to float when the caller runs at
    // 24-bit precision (MEASURED for the track-load caller, D-11072 G-P1 2026-10-09), which is
    // exactly SSE float arithmetic. A 53-bit caller would need the double form instead.
    const float mag2 = (in[0] * in[0] + in[1] * in[1]) + in[2] * in[2];
    float len = 0.0f, scale = 0.0f;
    const std::uint32_t b = Bits(mag2);
    if (b != 0u) {
        const std::uint32_t biased = b + 0x800u;
        len   = FromBits(T().sqrt_[(biased >> 12) & 0xfffu] + ((biased >> 1) & 0x3fc00000u));
        scale = FromBits(T().inv_[(biased >> 12) & 0xfffu] + ((~biased >> 1) & 0x3fc00000u));
    }
    out[0] = scale * in[0];
    out[1] = in[1] * scale;
    out[2] = in[2] * scale;
    return len;
}

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

}  // namespace Math
}  // namespace mashed_re
