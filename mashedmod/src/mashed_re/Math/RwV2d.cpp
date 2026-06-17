// Mashed RE - RW 2D vector math: length and normalize.
//
// 0x004c3bf0  FUN_004c3bf0  promote_c2_render_d3d9-20260513  C2 -> C3
// Vec2Length: sqrt(v[0]^2 + v[1]^2) via RW fast-sqrt LUT.
//
// 0x004c3c60  FUN_004c3c60  promote_c2_render_d3d9-20260513  C2 -> C3
// Vec2Normalize: normalise in[2] into out[2]; return original magnitude.
//
// Both use the same RW3 two-level LUT family as FastSqrt/FastInvSqrt.
// RW device state:
//   DAT_007d3ff8 = rw_globals (pointer to RW device struct)
//   DAT_007d3ffc = rw_offset  (offset within that struct for LUT root slot)
//   LUT root sqrt:    *(rw_globals + rw_offset)
//   LUT root inv-sqrt: *(rw_globals + rw_offset + 4)
//
// Error handling:
//   DAT_005d757c = magnitude epsilon (float) — below this triggers error code 0x19
//   FUN_004d7ff0 = error passthrough (C1 stub, called with error code)
//   FUN_004d8480 = error recorder    (C1 stub, called with &local_min_val)
#include "../Core/HookSystem.h"

#include <cstdint>
#include <cstring>
#include <cmath>

static constexpr std::uintptr_t kRwGlobalsBase    = 0x007d3ff8u;
static constexpr std::uintptr_t kRwSqrtTableSlot  = 0x007d3ffcu;
static constexpr std::uintptr_t kMagEpsilonAddr   = 0x005d757cu;

// Error stubs (C1) — called only when magnitude < epsilon.
typedef int   (__cdecl *RwErrPassFn)(int);
typedef void  (__cdecl *RwErrRecordFn)(float*);
static constexpr std::uintptr_t kFUN_004d7ff0 = 0x004d7ff0u;
static constexpr std::uintptr_t kFUN_004d8480 = 0x004d8480u;

// WS-PHYS-CRASH-FIX (2026-06-17): same null RW-LUT failure mode as Math/RwSqrt.cpp
// (standalone has no RwEngineOpen -> unbuilt LUT -> null table deref -> AV). Resolve
// + validate the root; nullptr -> std::sqrt fallback (dev .asi keeps the LUT path).
static constexpr std::uintptr_t kImgLo = 0x00010000u;
static constexpr std::uintptr_t kImgHi = 0x00b40000u;
static inline const std::uint32_t* lut_root(std::uint32_t delta)
{
    const std::uint32_t globals = *reinterpret_cast<const std::uint32_t*>(kRwGlobalsBase);
    const std::uint32_t offset  = *reinterpret_cast<const std::uint32_t*>(kRwSqrtTableSlot);
    const std::uintptr_t slotAddr = (std::uintptr_t)globals + offset + delta;
    if (slotAddr < kImgLo || slotAddr + 4u > kImgHi) return nullptr;
    const std::uint32_t root = *reinterpret_cast<std::uint32_t*>(slotAddr);
    if (root < kImgLo || (std::uintptr_t)root + 0x1000u * 4u > kImgHi) return nullptr;
    return reinterpret_cast<const std::uint32_t*>(root);
}

// 0x004c3bf0
// Returns sqrt(v[0]^2 + v[1]^2).  Zero input returns 0.0f.
extern "C" __declspec(dllexport)
float __cdecl Vec2Length(const float* v)
{
    const float sq = v[1] * v[1] + v[0] * v[0];

    std::uint32_t sq_bits;
    std::memcpy(&sq_bits, &sq, sizeof(sq_bits));

    if (sq_bits == 0u)
        return 0.0f;

    const std::uint32_t* root0 = lut_root(0);
    if (!root0) return (sq > 0.0f) ? std::sqrt(sq) : 0.0f;   // standalone fallback

    const std::uint32_t biased   = sq_bits + 0x800u;
    const std::uint32_t mantissa = root0[(biased >> 12) & 0xfffu];
    const std::uint32_t exponent = (biased >> 1) & 0x3fc00000u;
    const std::uint32_t result_bits = mantissa + exponent;

    float result;
    std::memcpy(&result, &result_bits, sizeof(result));
    return result;
}

RH_ScopedInstall(Vec2Length, 0x004c3bf0);  // re-enabled 2026-05-24 batch-mixed

// 0x004c3c60
// Normalises in[2] into out[2] using RW inv-sqrt LUT.
// Returns the original magnitude (= sqrt(x^2+y^2)).
// If magnitude < epsilon: sets out to near-zero, records error code 0x19.
extern "C" __declspec(dllexport)
float __cdecl Vec2Normalize(float* out, const float* in)
{
    const float sq = in[1] * in[1] + in[0] * in[0];

    std::uint32_t sq_bits;
    std::memcpy(&sq_bits, &sq, sizeof(sq_bits));

    float mag = 0.0f, inv_mag = 0.0f;

    const std::uint32_t* root0 = lut_root(0);
    const std::uint32_t* root4 = lut_root(4);
    if (!root0 || !root4) {
        // standalone CPU fallback (no engine LUT)
        if (sq > 0.0f) { mag = std::sqrt(sq); inv_mag = 1.0f / mag; }
        out[0] = inv_mag * in[0];
        out[1] = in[1]  * inv_mag;
        return mag;
    }

    if (sq_bits != 0u) {
        const std::uint32_t biased = sq_bits + 0x800u;
        // sqrt for return value
        {
            const std::uint32_t mantissa = root0[(biased >> 12) & 0xfffu];
            const std::uint32_t exponent = (biased >> 1) & 0x3fc00000u;
            const std::uint32_t bits     = mantissa + exponent;
            std::memcpy(&mag, &bits, sizeof(mag));
        }
        // inv-sqrt for normalisation (uses same biased bits, different LUT, NOT before shift)
        {
            const std::uint32_t mantissa = root4[(biased >> 12) & 0xfffu];
            const std::uint32_t exponent = (~biased >> 1) & 0x3fc00000u;
            const std::uint32_t bits     = mantissa + exponent;
            std::memcpy(&inv_mag, &bits, sizeof(inv_mag));
        }
    }

    out[0] = inv_mag * in[0];
    out[1] = in[1]  * inv_mag;

    const float epsilon = *reinterpret_cast<const float*>(kMagEpsilonAddr);
    if (mag < epsilon) {
        float denorm = 1.4013e-45f;  // raw bits 0x00000001
        reinterpret_cast<RwErrPassFn>(kFUN_004d7ff0)(0x19);       // STUB S-3705
        reinterpret_cast<RwErrRecordFn>(kFUN_004d8480)(&denorm);  // STUB S-3706
    }

    return mag;
}

RH_ScopedInstall(Vec2Normalize, 0x004c3c60);  // re-enabled 2026-05-24 batch-mixed
