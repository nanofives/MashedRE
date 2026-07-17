// Mashed RE - RwV3dNormalize: 3D vector normalize (in-place or to a separate out).
//
// 0x004c39b0  FUN_004c39b0  WS-A2 (vehicle physics RW-math prereq)
//
// Verbatim from Ghidra (pool2, read-only, 2026-06-16). Body 0x004c39b0..0x004c3abe.
// This is the 3D sibling of Vec2Normalize (0x004c3c60) and shares the same RW3
// two-level fast-sqrt / fast-inv-sqrt LUT family as Vec3Magnitude (0x004c3ac0)
// and FastInvSqrt (0x004c3b90).
//
// Decompiler signature: float10 FUN_004c39b0(float *out, float *in)
//   mag2 = (in[0]*in[0] + in[1]*in[1]) + in[2]*in[2]   (x87, FST rounds to f32)
//   if (mag2 != 0):
//     magnitude = bit_cast<float>(LUT_sqrt   [(mag2_bits+0x800 >>0xc)&0xfff]
//                                 + ((mag2_bits+0x800 >>1) & 0x3fc00000))
//     scale     = bit_cast<float>(LUT_invsqrt[(mag2_bits+0x800 >>0xc)&0xfff]
//                                 + ((~(mag2_bits+0x800) >>1) & 0x3fc00000))
//   else:  magnitude = 0.0, scale = 0.0   (the `param_2` register stays 0)
//   out[0] = scale*in[0]; out[1] = in[1]*scale; out[2] = in[2]*scale;
//   if (magnitude <= *0x005d757c)   error code 0x19 via FUN_004d7ff0 + FUN_004d8480
//   return magnitude               (left in st0 as float10; value is the f32 magnitude)
//
// Error-path comparison (asm 0x004c3a86: FLD; FCOMP [0x5d757c]; FNSTSW; TEST AH,0x41;
// JP skip) resolves to: fire when magnitude <= threshold AND not unordered — i.e.
// exactly C's `magnitude <= threshold` (false for NaN). The error stubs do not touch
// the output or the return value; they only fire on a (near-)zero-length vector.
//
// LUT roots (same band as Vec3Magnitude / Vec2Normalize):
//   DAT_007d3ff8 = rw_globals; DAT_007d3ffc = rw_offset.
//   LUT_sqrt    root = *(rw_globals + rw_offset + 0)
//   LUT_invsqrt root = *(rw_globals + rw_offset + 4)
#include "../Core/HookSystem.h"

#include <cstdint>
#include <cstring>
#include <cmath>

static constexpr std::uintptr_t kRwGlobalsBase   = 0x007d3ff8u;
static constexpr std::uintptr_t kRwSqrtTableSlot  = 0x007d3ffcu;
static constexpr std::uintptr_t kMagThresholdAddr = 0x005d757cu;  // float, read at runtime

// Error stubs (C1) — called only when magnitude <= threshold (degenerate input).
typedef int  (__cdecl *RwErrPassFn)(int);
typedef void (__cdecl *RwErrRecordFn)(int*);
static constexpr std::uintptr_t kFUN_004d7ff0 = 0x004d7ff0u;
static constexpr std::uintptr_t kFUN_004d8480 = 0x004d8480u;

// WS-PHYS-CRASH-FIX (2026-06-17) + VECCAP-1 fix (2026-07-16): LUT root resolved
// and validated by the shared readability+sentinel guard (RwLutGuard.h); the old
// in-image range check silently rejected legitimate high-heap layouts and forced
// the non-bit-identical CPU fallback. nullptr -> CPU normalize fallback
// (standalone, no RwEngineOpen).
#include "RwLutGuard.h"
static inline const std::uint32_t* lut_root(std::uint32_t delta)
{
    return delta == 4u ? RwLutGuard::InvSqrtRoot() : RwLutGuard::SqrtRoot();
}

// 0x004c39b0
// Normalises in[3] into out[3]; returns the original magnitude (= |in|).
extern "C" __declspec(dllexport)
float __cdecl RwV3dNormalize(float* out, const float* in)
{
    // x87 keeps these products extended; the assignment to a float rounds to f32,
    // matching the original's FST. Association (x*x + y*y) + z*z mirrors the asm.
    const float mag2 = in[0] * in[0] + in[1] * in[1] + in[2] * in[2];

    std::uint32_t mag2_bits;
    std::memcpy(&mag2_bits, &mag2, sizeof(mag2_bits));

    float magnitude = 0.0f;
    float scale     = 0.0f;  // = (float)0 when mag2 == 0 (the original leaves it 0)

    const std::uint32_t* sqrtRoot = lut_root(0);
    const std::uint32_t* invRoot  = lut_root(4);
    if (!sqrtRoot || !invRoot) {
        // Standalone CPU fallback: plain normalize. Degenerate (zero-length) input
        // leaves out=0 and returns 0, like the original's zero-mag branch.
        if (mag2 > 0.0f) {
            magnitude = std::sqrt(mag2);
            scale     = 1.0f / magnitude;
        }
        out[0] = scale * in[0];
        out[1] = in[1] * scale;
        out[2] = in[2] * scale;
        return magnitude;
    }

    if (mag2_bits != 0u) {
        const std::uint32_t biased = mag2_bits + 0x800u;
        {
            const std::uint32_t mantissa = sqrtRoot[(biased >> 12) & 0xfffu];
            const std::uint32_t exponent = (biased >> 1) & 0x3fc00000u;
            const std::uint32_t bits     = mantissa + exponent;
            std::memcpy(&magnitude, &bits, sizeof(magnitude));
        }
        {
            const std::uint32_t mantissa = invRoot[(biased >> 12) & 0xfffu];
            const std::uint32_t exponent = (~biased >> 1) & 0x3fc00000u;
            const std::uint32_t bits     = mantissa + exponent;
            std::memcpy(&scale, &bits, sizeof(scale));
        }
    }

    out[0] = scale * in[0];
    out[1] = in[1] * scale;
    out[2] = in[2] * scale;

    const float threshold = *reinterpret_cast<const float*>(kMagThresholdAddr);
    if (magnitude <= threshold) {
        int local[2];
        local[0] = 1;  // [type=1, message_ptr] pair on stack (matches original layout)
        // Error-reporting plumbing, degenerate-path only (shared with Vec2Normalize):
        local[1] = reinterpret_cast<RwErrPassFn>(kFUN_004d7ff0)(0x19);  // STUB S-3748
        reinterpret_cast<RwErrRecordFn>(kFUN_004d8480)(local);          // STUB S-3749
    }

    return magnitude;
}

RH_ScopedInstall(RwV3dNormalize, 0x004c39b0);
