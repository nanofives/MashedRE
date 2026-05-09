// Mashed RE — Vec3Magnitude reimplementation.
// Original: 0x004c3ac0  FUN_004c3ac0  ai_update_d2-20260503  C1
// Reads RW3 fast-sqrt LUT through the same globals the original uses.
#include "../Core/HookSystem.h"

#include <cstdint>
#include <cstring>

// 0x007d3ff8: RenderWare global object base pointer (read at runtime from MASHED.exe).
// 0x007d3ffc: RW per-engine offset to the sqrt LUT root pointer.
static constexpr std::uintptr_t kRwGlobalsBase   = 0x007d3ff8;
static constexpr std::uintptr_t kRwSqrtTableSlot = 0x007d3ffc;

extern "C" __declspec(dllexport) float __cdecl Vec3Magnitude(const float* v) {
    const float sq = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
    if (sq == 0.0f) {
        return 0.0f;
    }

    // sq's IEEE754 bit pattern feeds two index lanes of a two-level LUT.
    std::int32_t sq_bits;
    static_assert(sizeof(sq_bits) == sizeof(sq), "float must be 32-bit");
    std::memcpy(&sq_bits, &sq, sizeof(sq_bits));

    // *(int*)(0x007d3ffc + 0x007d3ff8) — Ghidra's literal addition: read the
    // pointer stored at the offset that lives in the RW globals object.
    const auto rw_globals = *reinterpret_cast<std::int32_t*>(kRwGlobalsBase);
    const auto rw_offset  = *reinterpret_cast<std::int32_t*>(kRwSqrtTableSlot);
    auto* table_base = *reinterpret_cast<std::int32_t**>(rw_globals + rw_offset);

    const std::int32_t level1_idx = ((sq_bits + 0x800) >> 12) & 0xfff;
    auto* level1 = reinterpret_cast<std::int32_t*>(table_base[level1_idx]);

    const std::int32_t level2_off = ((sq_bits + 0x800) >> 1) & 0x3fc00000;
    auto* level2 = reinterpret_cast<float*>(
        reinterpret_cast<std::uint8_t*>(level1) + level2_off);

    return *level2;
}

RH_ScopedInstall(Vec3Magnitude, 0x004c3ac0);
