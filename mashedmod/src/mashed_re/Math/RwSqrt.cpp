// Mashed RE - Fast reciprocal-sqrt (1/sqrt) primitive.
// Original: 0x004c3b90  FUN_004c3b90  vehicle_dynamics-20260506-expand  C1 -> C3
//
// Sibling of Vec3Magnitude (0x004c3ac0). Same RW3 two-level LUT family,
// two structural differences:
//   - reads the LUT root at offset +4 (different second-level table)
//   - applies NOT before the right-shift, inverting the exponent contribution
//
// Disasm at 0x004c3b90..0x004c3be6:
//   MOV EAX, [ESP+4]                         ; param_1 bits
//   TEST EAX,EAX; JZ ret                     ; zero guard
//   MOV ECX, [0x007d3ff8]                    ; rw_globals
//   MOV EDX, [0x007d3ffc]                    ; rw_offset
//   MOV EAX, [EDX + ECX*1 + 4]               ; LUT_root_invsqrt = *(rw_offset+rw_globals+4)
//                                              -- +4 picks the inv-sqrt table, distinct
//                                              from the sqrt table that Vec3Magnitude reads
//   ADD ECX, 0x800                           ; biased = bits + 0x800
//   SHR EDX, 0xc; AND EDX, 0xfff             ; level1_idx
//   MOV EAX, [EAX + EDX*4]                   ; mantissa = LUT_root_invsqrt[idx]
//   NOT ECX                                  ; ~biased     <-- bit-flip
//   SHR ECX, 1; AND ECX, 0x3fc00000          ; exp_correction (inverted)
//   ADD EAX, ECX                             ; result_bits = mantissa + exp_correction
//   MOV [ESP+4], EAX
//   FLD [ESP+4]                              ; reinterpret bits AS float
//   RET
//
// Same caveat as Vec3Magnitude: Ghidra renders the final step as
// `(float)(lut_val + mantissa_correction)` which reads as an int->float
// numeric cast, but the asm is FLD on the stored int bits — bit-cast,
// not value-conversion.
#include "../Core/HookSystem.h"

#include <cstdint>
#include <cstring>

// Same RW3 globals as Vec3Magnitude. The LUT root for inv-sqrt sits at +4
// inside the RW globals struct, vs +0 for the sqrt LUT.
static constexpr std::uintptr_t kRwGlobalsBase    = 0x007d3ff8;
static constexpr std::uintptr_t kRwSqrtTableSlot  = 0x007d3ffc;
static constexpr std::uint32_t  kInvSqrtRootDelta = 4;

extern "C" __declspec(dllexport) float __cdecl FastInvSqrt(float x) {
    std::uint32_t bits;
    static_assert(sizeof(bits) == sizeof(x), "float must be 32-bit");
    std::memcpy(&bits, &x, sizeof(bits));

    if (bits == 0u) {
        return 0.0f;
    }

    const std::uint32_t biased     = bits + 0x800u;
    const std::uint32_t rw_globals = *reinterpret_cast<std::uint32_t*>(kRwGlobalsBase);
    const std::uint32_t rw_offset  = *reinterpret_cast<std::uint32_t*>(kRwSqrtTableSlot);
    const auto*  lut_root          = *reinterpret_cast<std::uint32_t**>(
                                        rw_globals + rw_offset + kInvSqrtRootDelta);
    const std::uint32_t mantissa   = lut_root[(biased >> 12) & 0xfffu];
    const std::uint32_t exp_corr   = (~biased >> 1) & 0x3fc00000u;
    const std::uint32_t result_bits = mantissa + exp_corr;

    float result;
    std::memcpy(&result, &result_bits, sizeof(result));
    return result;
}

RH_ScopedInstall(FastInvSqrt, 0x004c3b90);

// Mashed RE - Fast sqrt (single float) primitive.
// Original: 0x004c3b30  FUN_004c3b30  ai_update_d4  C1 -> C3
//
// Same LUT family. Disasm at 0x004c3b30..0x004c3b83 mirrors Vec3Magnitude's
// inner step exactly — LUT root at offset +0 inside RW globals, no NOT
// before the right-shift. Difference from Vec3Magnitude is that this
// function takes the squared value as a parameter (caller does the
// sum-of-squares); FastSqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]) would behave
// identically to Vec3Magnitude(v).
extern "C" __declspec(dllexport) float __cdecl FastSqrt(float x) {
    std::uint32_t bits;
    static_assert(sizeof(bits) == sizeof(x), "float must be 32-bit");
    std::memcpy(&bits, &x, sizeof(bits));

    if (bits == 0u) {
        return 0.0f;
    }

    const std::uint32_t biased     = bits + 0x800u;
    const std::uint32_t rw_globals = *reinterpret_cast<std::uint32_t*>(kRwGlobalsBase);
    const std::uint32_t rw_offset  = *reinterpret_cast<std::uint32_t*>(kRwSqrtTableSlot);
    const auto*  lut_root          = *reinterpret_cast<std::uint32_t**>(rw_globals + rw_offset);
    const std::uint32_t mantissa   = lut_root[(biased >> 12) & 0xfffu];
    const std::uint32_t exp_corr   = (biased >> 1) & 0x3fc00000u;
    const std::uint32_t result_bits = mantissa + exp_corr;

    float result;
    std::memcpy(&result, &result_bits, sizeof(result));
    return result;
}

RH_ScopedInstall(FastSqrt, 0x004c3b30);
