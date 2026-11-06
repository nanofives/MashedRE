// Mashed RE: stand-in promotion round 1 (plan item #3 step 2, 2026-10-11).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Rows that allocate through the RwGlobals memory slot *(0x007d3ff8) + 0x108 (rwmalloc(size, hint), 2 cdecl args;
// re/analysis/rwglobals_layout_20260911.md). A/B-tested under the heap stand-in (re/frida/js/standins.js), which
// carves both arms' blocks from one arena, so the slot must be read through *(0x007d3ff8) as the original does.

#include "../Core/HookSystem.h"

#include <cstdint>

namespace {
using RwMallocFn = void*(__cdecl*)(std::uint32_t size, std::uint32_t hint);
using ZeroFillFn = void(__cdecl*)(void* p, std::uint32_t n);   // 0x004b6520 ZeroFillWrapper (C3)

inline RwMallocFn RwMallocSlot() {
    const std::uintptr_t rwg = *reinterpret_cast<const std::uintptr_t*>(0x007d3ff8u);
    return *reinterpret_cast<RwMallocFn*>(rwg + 0x108u);
}
}  // namespace

// MATCH-S11: the body of 0x00474db0 (RwMallocArrayZeroFill) moved to Match/MatchS11_00474db0.cpp,
// which compiles the byte-matched source re/match/s11/00474db0.c.

// MATCH-S12: the body of 0x004759b0 (Arrays691600Alloc) moved to Match/MatchS12_004759b0.cpp,
// which compiles the byte-matched source re/match/s12/004759b0.c.

// MATCH-S14: the body of 0x00484170 (Rec18Create) moved to Match/MatchS14_00484170.cpp,
// which compiles the byte-matched source re/match/s14/00484170.c.
