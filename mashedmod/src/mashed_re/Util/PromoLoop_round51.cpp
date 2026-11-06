// Mashed RE — promote-round round 51 (palette-array initializer).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// FUN_004723d0 (decomp-verified, self-contained, no calls): inits the 10-entry
// 0x10-byte palette array at 0x00691500. Each entry i (0..9):
//   [+0]=0xff00ffff|((i+1)<<16)  [+4]=0xff003cff|((i+1)<<16)  [+8]=0  [+0xc]=0
// The diff compares the final 0xa0-byte memory state (range_init), so an
// equivalent loop reimpl is bit-identical. Caller FUN_00402750 boot C2.

#include "../Core/HookSystem.h"

#include <cstdint>

// MATCH-S11: the body of 0x004723d0 (Init691500) moved to Match/MatchS11_004723d0.cpp,
// which compiles the byte-matched source re/match/s11/004723d0.c.
