// Mashed RE — promote-round round 74 (strided fill + 2 index->ptr-array getters + 2 multi-bit flag setters).
// This round brings the promote-round total to 200 C2->C3.
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// All five are __cdecl, self-contained (zero callees), byte-verified from the listing.
//
// FUN_00453f30 (0x00453f30): strided fill of 0xffffffff.
//   MOV EAX,0x6870b4 ; loop: MOV [EAX],0xffffffff ; ADD EAX,0x24 ; CMP 0x687ec4 ; JL
//
// FUN_00404e00 (0x00404e00): index->ptr-array getter (1 index).
//   idx=*(int*)(0x639cc8 + p1*4) ; if(idx==-1) return 0 ; return *(u32*)(0x5ea0b8 + idx*4)
//
// FUN_00404e20 (0x00404e20): index->ptr-array getter (2 indices, comp=p1*0x4e+p2).
//   idx=*(int*)(0x636c08 + (p1*0x4e+p2)*4) ; if(idx==-1) return 0 ; return *(u32*)(0x5ea0c8 + idx*4)
//
// FUN_0041ede0 (0x0041ede0): per-player flag multi-bit setter (3 args; bits 0x10/0x20 + cascades 0x1000/0x2000).
// FUN_0041eeb0 (0x0041eeb0): per-player flag multi-bit setter (4 args; bits 0x40/0x80/0x800 + 3 clear-cascades).

#include "../Core/HookSystem.h"

#include <cstdint>

// 0x00453f30
extern "C" __declspec(dllexport) void __cdecl Fill6870b4(void) {
    for (std::uint32_t p = 0x006870b4u; static_cast<std::int32_t>(p) < 0x00687ec4; p += 0x24u)
        *reinterpret_cast<std::uint32_t*>(p) = 0xffffffffu;
}
RH_ScopedInstall(Fill6870b4, 0x00453f30);

// [rtex 2026-10-16] IdxPtr404e00 (FUN_00404e00) and IdxPtr404e20 (FUN_00404e20) moved,
// unchanged for the .asi, to Vehicle/CarMaterialTables.cpp, which both targets compile: the
// standalone's car dirt set-up calls them, and an RVA keeps one body (build.bat rva-lint).

// MATCH-S04: the body of 0x0041ede0 (Flag0041ede0) moved to Match/MatchS04_0041ede0.cpp,
// which compiles the byte-matched source re/match/s04/0041ede0.c.

// MATCH-S04: the body of 0x0041eeb0 (Flag0041eeb0) moved to Match/MatchS04_0041eeb0.cpp,
// which compiles the byte-matched source re/match/s04/0041eeb0.c.
