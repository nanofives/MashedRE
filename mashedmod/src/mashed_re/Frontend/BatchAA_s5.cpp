// Mashed RE - Frontend batch-aa-s5 reimplementations.
// Analysis notes:
//   re/analysis/frontend_c1_to_c2_s6/FUN_0042a980.md
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)

#include "../Core/HookSystem.h"

#include <cstdint>

// MATCH-S06: the callee pointer s_FUN_0040ce80 went with the body (the matched source binds
// FUN_0040ce80 -> PtrTable5f2770Get, Util/PromoLoop_round49.cpp).

// ---------------------------------------------------------------------------
// MenuTableSearchAlt  --  0x0042a980
//
// Original: FUN_0042a980 (61 bytes, 0x0042a980..0x0042a9bd)
// Signature: undefined4 FUN_0042a980(undefined4 param_1)
//   param_1: argument forwarded to FUN_0040ce80 to derive the search key.
// Returns: 0 if not found; value field at stride-3 entry offset +4 on match.
//
// Algorithm (cited from 0x0042a980 body):
//   1. iVar2 = FUN_0040ce80(param_1)       [0x0042a984 — derive search key]
//   2. iVar3 = 0                           [word-index accumulator]
//   3. iVar1 = DAT_005f6748                [0x0042a98b — load first key]
//   4. Loop:
//      a. if iVar1 == -1: return 0         [0x0042a990 — sentinel 0xffffffff]
//      b. if iVar1 == iVar2: break         [0x0042a996 — key match]
//      c. iVar1 = (&DAT_005f6754)[iVar3]  [0x0042a999 — next key at 0x5f6754 + iVar3*4]
//      d. iVar3 += 3                       [0x0042a99b — stride 3 int32 units = 12 bytes]
//   5. return *(undefined4*)(iVar3*4 + 0x5f674c)
//                                          [0x0042a9aa — value field at +4 from base 0x5f6748]
//
// Table layout (stride 3 int32 words, base 0x005f6748):
//   Entry N key:    *(int32_t*)(0x005f6748 + N*12)
//   Entry N +4:     *(int32_t*)(0x005f674c + N*12)  <-- THIS FUNCTION returns this field
//   Entry N +8:     *(int32_t*)(0x005f6750 + N*12)  <-- FUN_0042a940 returns this field
//
// Relationship to MenuTableSearch (0x0042a940, C3):
//   Same table (0x005f6748), same walk, same sentinel, same callee FUN_0040ce80.
//   Differs only in which value field is returned: +4 here vs +8 in 0x0042a940.
//
// Uncertainties (non-blocking):
//   U-4199: exact field semantics at +4 per stride-3 entry unknown.
//
// Callees (all C2+):
//   0x0040ce80  FUN_0040ce80  C2
//
// ref: re/analysis/frontend_c1_to_c2_s6/FUN_0042a980.md
// ---------------------------------------------------------------------------

// MATCH-S06: the body of 0x0042a980 (MenuTableSearchAlt) moved to Match/MatchS06_0042a980.cpp,
// which compiles the byte-matched source re/match/s06/0042a980.c.
