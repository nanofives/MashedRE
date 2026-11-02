// Mashed RE — promote-round round 76 (global-field getter; loop opportunistic, past the 200 goal).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// FUN_0044c370 (0x0044c370): __cdecl, self-contained (zero callees), byte-verified.
//   MOV EAX,[0x00896000] ; MOV EAX,[EAX+0x4] ; ADD EAX,0x40
//   -> return *(int*)(*(int*)0x00896000 + 4) + 0x40

#include "../Core/HookSystem.h"

#include <cstdint>

// MATCH-S08: the body of 0x0044c370 (GlobalFieldGet896000) moved to Match/MatchS08_0044c370.cpp,
// which compiles the byte-matched source re/match/s08/0044c370.c.
