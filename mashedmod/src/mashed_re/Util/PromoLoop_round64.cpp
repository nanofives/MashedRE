// Mashed RE — promote-round round 64 (strided 2-dword clear).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// __cdecl, self-contained (zero callees), byte-verified from the listing.
// (0x004840d0 is already C3 = JointPtr6ce81cGet — excluded.)
//
// FUN_0048a460 (0x0048a460): strided 2-dword clear.
//   b838927000    MOV EAX,0x709238 ; XOR ECX,ECX
//   8908 894804   MOV [EAX],ECX ; MOV [EAX+4],ECX     ; loop body (zero p[0], p[1])
//   0530030000    ADD EAX,0x330
//   3d98317100 7cef CMP EAX,0x713198 ; JL loop
//   -> for (p = 0x709238; p < 0x713198; p += 0x330) { *(u32*)p = 0; *(u32*)(p+4) = 0; }

#include "../Core/HookSystem.h"

#include <cstdint>

// MATCH-S14: the body of 0x0048a460 (StridedClear2_709238) moved to Match/MatchS14_0048a460.cpp,
// which compiles the byte-matched source re/match/s14/0048a460.c.
