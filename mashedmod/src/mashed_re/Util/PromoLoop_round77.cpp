// Mashed RE — promote-round round 77 (strided -1.0f fill + trailing global zero; opportunistic, beyond 200).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// FUN_00421080 (0x00421080): __cdecl, self-contained (zero callees), byte-verified.
//   MOV EAX,0x63e5a4 ; loop: MOV [EAX],0xbf800000 ; ADD EAX,0x2c ; CMP 0x63fba4 ; JL
//   MOV [0x0063fb88],0
//   -> strided fill 0xbf800000 (-1.0f) over [0x0063e5a4, 0x0063fba4) step 0x2c; then *(u32*)0x0063fb88 = 0
//      (0x0063fb88 lies inside the filled range, so the trailing store overwrites that slot to 0)

#include "../Core/HookSystem.h"

#include <cstdint>

// MATCH-S05: the body of 0x00421080 (Fill63e5a4) moved to Match/MatchS05_00421080.cpp,
// which compiles the byte-matched source re/match/s05/00421080.c.
