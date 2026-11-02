// Mashed RE — promote-round round 256 (five forwarders).
//
// NAMING: these are MECHANICAL names, deliberately. Fwd<callee>_<literal> says what
// the function demonstrably does -- forwards to that callee binding that literal --
// and claims nothing about why. Earlier drafts called them ParticlePool703068Reset,
// SpriteBatch63bd50Reset and InputAxisScaleBiasApply; none of those roles is
// supported by anything read here, and a wrong name in hooks.csv is worse than a
// dull one. Where a name IS grounded it is used: 0x00428610 is the C4 row
// ViewportScaledRectDraw, and 0x004e66d0 is RpClumpForAllAtomics.
//
// Functions in this file:
//   0x00487140  Fwd4768c0_703068  — FUN_004768c0(&DAT_00703068)
//   0x00413bb0  Fwd4768c0_63bd50   — FUN_004768c0(&DAT_0063bd50)   [sibling]
//   0x00428760  ViewportScaledRectDraw_Arg7Zero            — FUN_00428610(a1..a6, 0)
//   0x004b5580  ClumpForAllAtomics_Cb4b5560   — RpClumpForAllAtomics(a1, FUN_004b5560, a2)
//   0x00495080  Fwd494fd0_DoublePlusHalf  — FUN_00494fd0(a1 + a1 + 0.5f, a2)
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// Fwd4768c0_703068  --  0x00487140   and
// Fwd4768c0_63bd50   --  0x00413bb0
//
//   0x00487140  push 0x703068 / call 0x4768c0 / pop ecx / ret
//   0x00413bb0  push 0x63bd50 / call 0x4768c0 / pop ecx / ret
//
// A SIBLING PAIR: identical four-instruction bodies calling the SAME callee,
// differing ONLY in the pushed literal. That literal is the entire content of
// each function, so each one's A/B asserts its own address and would fail if the
// two were swapped.
//
// `pop ecx` is the one-argument cleanup (equivalent to `add esp,4`) and does NOT
// touch EAX, so the callee's return falls through in both. Ghidra types both
// `void`; that is the same mistyping corrected on 0x004c2d90 in r249, so both
// are transcribed as returning the callee's value.
// ---------------------------------------------------------------------------

typedef int(__cdecl* PoolReset_t)(std::uint32_t);

// 0x00487140
extern "C" __declspec(dllexport) int __cdecl Fwd4768c0_703068()
{
    return reinterpret_cast<PoolReset_t>(0x004768c0u)(0x00703068u);
}

RH_ScopedInstall(Fwd4768c0_703068, 0x00487140);

// 0x00413bb0
extern "C" __declspec(dllexport) int __cdecl Fwd4768c0_63bd50()
{
    return reinterpret_cast<PoolReset_t>(0x004768c0u)(0x0063bd50u);
}

RH_ScopedInstall(Fwd4768c0_63bd50, 0x00413bb0);

// MATCH-S06: the body of 0x00428760 (ViewportScaledRectDraw_Arg7Zero) moved to Match/MatchS06_00428760.cpp,
// which compiles the byte-matched source re/match/s06/00428760.c.

// ---------------------------------------------------------------------------
// ClumpForAllAtomics_Cb4b5560  --  0x004b5580
//
//   push eax(a2) / push 0x4b5560 / push ecx(a1) / call 0x4e66d0 / add esp,0xc
//
// RpClumpForAllAtomics(a1, FUN_004b5560, a2) — the middle argument is a LITERAL
// CALLBACK ADDRESS, and it is the whole point of the function. The A/B records
// it, so a port that bound the wrong callback fails even though the call shape
// and argument count would be identical.
//
// The callee at 0x004e66d0 is RpClumpForAllAtomics; U-4416 (resolved 2026-09-11)
// records its mechanics: a circular-list walk invoking
// `(*param_2)(puVar2 + -0x10, param_3)` per node and early-outing when the
// callback returns 0. It is stubbed here, so none of that runs.
// ---------------------------------------------------------------------------

typedef int(__cdecl* RpClumpForAllAtomics_t)(std::uint32_t, std::uint32_t, std::uint32_t);

// 0x004b5580
extern "C" __declspec(dllexport) int __cdecl ClumpForAllAtomics_Cb4b5560(
    std::uint32_t clump, std::uint32_t user)
{
    return reinterpret_cast<RpClumpForAllAtomics_t>(0x004e66d0u)(
        clump, 0x004b5560u, user);
}

RH_ScopedInstall(ClumpForAllAtomics_Cb4b5560, 0x004b5580);

// MATCH-S16: the body of 0x00495080 (Fwd494fd0_DoublePlusHalf) moved to Match/MatchS16_00495080.cpp,
// which compiles the byte-matched source re/match/s16/00495080.c.
