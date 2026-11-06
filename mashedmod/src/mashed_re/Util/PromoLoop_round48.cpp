// Mashed RE — promote-round round 48 (SmplFzx array-stack push/pop pair).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Array-stack {top@0, cap@4, buf-ptr@8}. Diffed via early_window_leaf_diff
// stack_pop_snapshot / stack_push_snapshot. Callers smplfzx C2.
//   0x00485bd0  StackPop485bd0  — top--; <0?clamp,0:buf[top]  (caller 004850e0 smplfzx C2)
//   0x00485bf0  StackPush485bf0 — full?0:buf[top]=val,top++,1 (caller 004852e0 smplfzx C2)

#include "../Core/HookSystem.h"

#include <cstdint>

// MATCH-S14: the body of 0x00485bd0 (StackPop485bd0) moved to Match/MatchS14_00485bd0.cpp,
// which compiles the byte-matched source re/match/s14/00485bd0.c.

// MATCH-S14: the body of 0x00485bf0 (StackPush485bf0) moved to Match/MatchS14_00485bf0.cpp,
// which compiles the byte-matched source re/match/s14/00485bf0.c.
