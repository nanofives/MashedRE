// Mashed RE - Frontend menus_a mixed reimplementations.
// Analysis notes:
//   re/analysis/frontend_promote_menus_a/0x0042a940.md
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)

#include "../Core/HookSystem.h"

#include <cstdint>

// MATCH-S06: the callee pointer s_FUN_0040ce80 went with the body (the matched source binds
// FUN_0040ce80 -> PtrTable5f2770Get, Util/PromoLoop_round49.cpp).

// ---------------------------------------------------------------------------
// MenuTableSearch  --  0x0042a940
//
// Original: FUN_0042a940 (61 bytes, 0x0042a940..0x0042a97d)
// Signature: undefined4 FUN_0042a940(undefined4 param_1)
//   param_1: car slot index passed to FUN_0040ce80 as lookup key
// Returns: 0 if not found; value field (+8) from stride-3 table at 0x005f6748 on match.
// Side-effects: none (read-only; writes no globals, no memory)
//
// Algorithm (cited from 0x0042a940 body):
//   1. key = FUN_0040ce80(param_1)       [0x0042a944]
//   2. pos = 0
//   3. entry0 = *(int32_t*)(0x005f6748)  [0x0042a944]
//   4. Loop:
//      a. if entry0 == -1: return 0      [0x0042a94c — sentinel 0xffffffff]
//      b. if entry0 == key: break        [match found]
//      c. entry0 = *(int32_t*)(0x005f6754 + pos*4)  [0x0042a953]
//      d. pos += 3                        [0x0042a955 — stride 3 int32 units]
//   5. return *(undefined4*)(0x005f6750 + pos*4)  [0x0042a966 — value at +8 relative to key]
//
// Table layout (stride 3 int32, base 0x005f6748):
//   Entry N: key at 0x005f6748+N*12; +4 field (unknown, never read here);
//            value at 0x005f6750+N*12
//
// Uncertainties (do not block reimplementation):
//   U-3434: FUN_0040ce80 semantics unknown (pure mechanical double-deref).
//   U-3435: +4 field per stride-3 entry never accessed; role unknown.
//
// ref: re/analysis/frontend_promote_menus_a/0x0042a940.md
// ---------------------------------------------------------------------------

// MATCH-S06: the body of 0x0042a940 (MenuTableSearch) moved to Match/MatchS06_0042a940.cpp,
// which compiles the byte-matched source re/match/s06/0042a940.c.
