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

}  // namespace Math
}  // namespace mashed_re
