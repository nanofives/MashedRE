// Mashed RE — Frontend small-leaf reimplementations (c3-batch-t session 1).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Functions in this file:
//   0x00408ad0  RaceScoreFloatGetBySlot    — 16B leaf; float getter from stride-0x30c array
//   0x00401570  EntryTableScanByKey        — 36B leaf; stride-0x68 entry-table scan, writes DAT_00636ac0
//   0x0040d250  DoubleDerefIndexedGetter   — 16B leaf; (**(PTR_PTR_005f2770 + DAT_0063ba7c*4))
//   0x0041e080  ScoreboardStateZeroInit    — 67B leaf; zero 11 globals + write 0x47 to one field
//   0x00414120  CopyTable005f2a70To0089a384 — 80B leaf; zero+copy 0x9c bytes
//
// Deferred from batch t s1 (documented in session output):
//   0x0040b290  FUN_0040b290  — circ-buffer side effects + global state writes
//   0x0040d590  FUN_0040d590  — calls FUN_00422fd0 / FUN_0040b290 (state mutators)
//   0x00401da0  FUN_00401da0  — RW transform pipeline side effects
//   0x004189f0  thunk_FUN_00419760 — 4-byte thunk; E9+rel32 install on 4B is unsafe
//   0x004224d0  FUN_004224d0  — slot init with ptr param + memset callee; non-trivial harness
//
// Analysis notes:
//   re/analysis/frontend_c1_to_c2_s1/00408ad0.md
//   re/analysis/frontend_c1_to_c2_s1/00401570.md
//   re/analysis/frontend_c1_to_c2_s1/0040d250.md
//   re/analysis/frontend_c1_to_c2_s1/0041e080.md
//   re/analysis/frontend_c1_to_c2_s1/00414120.md

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// RaceScoreFloatGetBySlot  --  0x00408ad0
//
// Original: FUN_00408ad0 (16 bytes). float10 FUN_00408ad0(int param_1).
// Body: return (float10)*(float *)(&DAT_008a96ec + param_1 * 0x30c)
//   0x008a96ec base [0x00408ad3]; stride 0x30c [0x00408ad8]
// U-4200 (semantic meaning) non-blocking.
// ---------------------------------------------------------------------------

// [moved to Match/MatchS01.cpp: C4-by-match port body] 0x00408ad0 RaceScoreFloatGetBySlot


// ---------------------------------------------------------------------------
// EntryTableScanByKey  --  0x00401570
//
// Original: FUN_00401570 (36 bytes). void FUN_00401570(int param_1).
//   DAT_00636ac0 = &DAT_00636578;                            // init to base
//   for (p = base; (int)p < 0x636ac0; p += 0x68)
//       if (*(int*)(p+0x38) == param_1) DAT_00636ac0 = p;    // last-match wins
//   0x00636578 base [0x00401573]; 0x00636ac0 out/bound [0x00401576/0x00401589];
//   index 0xe == +0x38 [0x0040157e]; stride 0x1a*4 = 0x68 [0x00401582]
// ---------------------------------------------------------------------------

// [moved to Match/MatchS01.cpp: C4-by-match port body] 0x00401570 EntryTableScanByKey


// ---------------------------------------------------------------------------
// DoubleDerefIndexedGetter  --  0x0040d250
//
// Original: FUN_0040d250 (16 bytes). undefined4 FUN_0040d250(void).
// Body: return **(undefined4 **)(PTR_PTR_005f2770 + DAT_0063ba7c * 4)
//   0x005f2770 table base [0x0040d254]; 0x0063ba7c index [0x0040d254]
// Pure read — no side effects. U-4203 non-blocking.
// ---------------------------------------------------------------------------

// MATCH-S02: the body of 0x0040d250 (DoubleDerefIndexedGetter) moved to Match/MatchS02_0040d250.cpp,
// which compiles the byte-matched source re/match/s02/0040d250.c.

// ---------------------------------------------------------------------------
// ScoreboardStateZeroInit  --  0x0041e080
//
// Original: FUN_0041e080 (67 bytes). void FUN_0041e080(void).
// Zeroes 11 globals in 0x0063d798..0x0063d7dc; writes 0x47 to DAT_0063d7cc.
//   each write cited at 0x0041e083..0x0041e0c0. U-4209 non-blocking.
// ---------------------------------------------------------------------------

// MATCH-S04: the body of 0x0041e080 (ScoreboardStateZeroInit) moved to Match/MatchS04_0041e080.cpp,
// which compiles the byte-matched source re/match/s04/0041e080.c.

// ---------------------------------------------------------------------------
// CopyTable005f2a70To0089a384  --  0x00414120
//
// Original: FUN_00414120 (80 bytes). void FUN_00414120(void).
//   _DAT_0089a37c=0 [0x00414125]; DAT_0089a378=0 [0x0041412b]; _DAT_0089a380=0 [0x00414131]
//   loop iVar1 0..0x9c step 4 (3-iter inner unroll):
//     *(0x0089a420 + iVar1) = 0;                  [0x0041413d]
//     *(0x0089a384 + iVar1) = *(0x005f2a70 + iVar1); [0x00414143/49]
//   0x9c (156) total bytes [0x00414153]. U-4206 non-blocking.
// ---------------------------------------------------------------------------

// MATCH-S03 (2026-10-31): the body is the matched source re/match/s03/h1_00414120.c (byte-identical
// under re/tools/match/match.py --bytes, cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1); MATCH_PORT maps
// its two names onto the same addresses as before.
#define MATCH_PORT 1
#define DAT_0089a378 (*reinterpret_cast<G0089a378*>(0x0089a378u))
#define DAT_005f2a70 (reinterpret_cast<const int(*)[3]>(0x005f2a70u))
#define FUN_00414120 CopyTable005f2a70To0089a384
extern "C" __declspec(dllexport) void __cdecl CopyTable005f2a70To0089a384(void);
extern "C" {
// 0x00414120  CopyTable005f2a70To0089a384
#include "../../../../re/match/s03/h1_00414120.c"
}
#undef FUN_00414120
#undef DAT_005f2a70
#undef DAT_0089a378
#undef MATCH_PORT

RH_ScopedInstall(CopyTable005f2a70To0089a384, 0x00414120);
