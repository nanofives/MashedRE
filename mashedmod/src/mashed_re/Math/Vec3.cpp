// Mashed RE - Vec3Magnitude reimplementation.
// Original: 0x004c3ac0  FUN_004c3ac0  ai_update_d2-20260503  C1 -> C4 candidate
//
// RW3 fast-sqrt trick. Disasm at 0x004c3ac0..0x004c3b2f:
//   sq = v[0]*v[0] + v[1]*v[1] + v[2]*v[2]   (FPU; FSTP rounds to f32 in [ESP+4])
//   sq_bits = bit_cast<u32>(sq)
//   if sq_bits == 0: return 0.0f             (FLD on the stored 0)
//   else:
//     biased         = sq_bits + 0x800
//     LUT_root       = *(u32**)(*0x007d3ffc + *0x007d3ff8)
//     level1_idx     = (biased >> 12) & 0xfff
//     mantissa_bits  = LUT_root[level1_idx]                  // 32-bit value, NOT a ptr
//     exponent_bits  = (biased >> 1) & 0x3fc00000
//     return bit_cast<float>(mantissa_bits + exponent_bits)
//
// Ghidra's pseudocode renders the final step as `*(float *)(level1 + offset)`
// because the bytes get round-tripped through the stack slot ([ESP+4]) and
// the closing FLD looks like a pointer deref. It is actually a reinterpret
// of an integer sum — see asm at 0x004c3b17..0x004c3b2f.
#include "../Core/HookSystem.h"

#include <cstdint>
#include <cstring>

// Globals from MASHED.exe, read at runtime.
// 0x007d3ff8: RenderWare globals base pointer (RwEngineInstance or similar).
// 0x007d3ffc: offset of fast-sqrt LUT root pointer within that struct.
static constexpr std::uintptr_t kRwGlobalsBase   = 0x007d3ff8;
static constexpr std::uintptr_t kRwSqrtTableSlot = 0x007d3ffc;

extern "C" __declspec(dllexport) float __cdecl Vec3Magnitude(const float* v) {
    const float sq = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];

    std::uint32_t sq_bits;
    static_assert(sizeof(sq_bits) == sizeof(sq), "float must be 32-bit");
    std::memcpy(&sq_bits, &sq, sizeof(sq_bits));

    if (sq_bits == 0u) {
        return 0.0f;
    }

    const std::uint32_t biased     = sq_bits + 0x800u;
    const std::uint32_t rw_globals = *reinterpret_cast<std::uint32_t*>(kRwGlobalsBase);
    const std::uint32_t rw_offset  = *reinterpret_cast<std::uint32_t*>(kRwSqrtTableSlot);
    const auto*  lut_root          = *reinterpret_cast<std::uint32_t**>(rw_globals + rw_offset);
    const std::uint32_t mantissa   = lut_root[(biased >> 12) & 0xfffu];
    const std::uint32_t exponent   = (biased >> 1) & 0x3fc00000u;
    const std::uint32_t result_bits = mantissa + exponent;

    float result;
    std::memcpy(&result, &result_bits, sizeof(result));
    return result;
}

// MASS-DISABLED 2026-05-24 loader-broken-9d: RH_ScopedInstall(Vec3Magnitude, 0x004c3ac0);
