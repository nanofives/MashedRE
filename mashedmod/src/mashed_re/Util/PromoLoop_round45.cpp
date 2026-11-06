// Mashed RE — promote-round round 45 (pool-manager INSERT; self-contained list op).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Faithful port of FUN_00485a70 (decomp-verified). Manager (byte offsets):
//   +0 count(u16), +2 cap(u16), +4 head(ptr), +8 tail(ptr), +0xc pool(ptr),
//   +0x10 slots(int[] ptr). Node (pool + slot*0x10): +0 key, +4 link-a, +8 link-b,
//   +0xc slot-id(u16). No function calls (self-contained). Diffed via
//   early_window_leaf_diff pool_insert_snapshot (reset+call+full-state snapshot).
//   Caller FUN_004850e0 (smplfzx) C2.

#include "../Core/HookSystem.h"

#include <cstdint>

// MATCH-S14: the body of 0x00485a70 (PoolInsert485a70) moved to Match/MatchS14_00485a70.cpp,
// which compiles the byte-matched source re/match/s14/00485a70.c.
