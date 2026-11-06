// Mashed RE — promote-round round 47 (stride-0x18 table getter + 2 clearers).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// All byte-verified self-contained (no calls). Callers C2:
//   0x00454a30  Table688304Get  — return *(0x00688304 + i*0x18) (caller 004177b0 ai       C2)
//   0x0045c850  Table88f09cClear— *(0x0088f09c + i*4) = 0        (caller 004657b0 util     C2)
//   0x004894f0  Clear894f0      — p->[0x54]=p->[8]=p->[0xc]=0    (caller 00486270 particle C2)

#include "../Core/HookSystem.h"

#include <cstdint>

// MATCH-S09: the body of 0x00454a30 (Table688304Get) moved to Match/MatchS09_00454a30.cpp,
// which compiles the byte-matched source re/match/s09/00454a30.c.

// MATCH-S10: the body of 0x0045c850 (Table88f09cClear) moved to Match/MatchS10_0045c850.cpp,
// which compiles the byte-matched source re/match/s10/0045c850.c.

// MATCH-S14: the body of 0x004894f0 (Clear894f0) moved to Match/MatchS14_004894f0.cpp,
// which compiles the byte-matched source re/match/s14/004894f0.c.
