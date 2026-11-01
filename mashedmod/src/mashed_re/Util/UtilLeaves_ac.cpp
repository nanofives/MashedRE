// Mashed RE — Util leaf functions, c3_batch_ac session 1 (s5-redo, 8 util leaves).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched — verified this session)
//
// All eight reimplementations below are 1:1 transcriptions of the C1 analysis
// notes cited per-function. NO-GUESSING: every constant/offset is cited to the
// analysis note (and, transitively, to the Ghidra address in that note).
//
// Candidates (c3_batch_ac.txt session 1 — all pure leaves, callees_depth1==[]):
//   0x00407a60  FUN_00407a60          — void(void): init record table @0x008a9620
//   0x0040ad30  FUN_0040ad30          — void(uint32): *0x008a95ac = arg
//   0x0040b250  FUN_0040b250          — void(void): write -1000 to 8 globals
//   0x0040b430  FUN_0040b430          — void(int,int): indexed field += delta
//   0x0040b6d0  FUN_0040b6d0          — int(int): return (&0x008a94e0)[idx]
//   0x004292c0  sub_004292c0          — void(uint32*,uint32): *p = v
//   0x0042aab0  sub_0042aab0          — void(void): *0x00898a90=0x180; *0x00898ab0=0
//   0x00430b00  TimeDisplay::SetEntry — void(int,int,u32,u32,u32): 2D-indexed 3-field write
//
// Source analysis notes:
//   re/analysis/timer_d3_cont1_a/0x00407a60.md
//   re/analysis/timer_d3_cont1_a/0x0040ad30.md
//   re/analysis/game_state_d5_cont2/0x0040b250.md
//   re/analysis/game_state/0x0040b430.md
//   re/analysis/game_state/0x0040b6d0.md
//   re/analysis/timer_d3_cont1_b/0x004292c0.md
//   re/analysis/timer_d3_cont1_b/0x0042aab0.md
//   re/analysis/leaderboard_d2/0x00430b00.md

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// FUN_00407a60  --  0x00407a60   (void(void) record-table initializer)
//
// Per note (re/analysis/timer_d3_cont1_a/0x00407a60.md):
//   puVar2 = (uint*)0x008a9648                    ; record-0 anchor (+0x28 from base)
//   do {
//     fill = puVar2 - 10;                          ; record base = puVar2 - 0x28 bytes
//     for (i = 0xc3; i--; ) *fill++ = 0;           ; zero 0xc3 (195) dwords = 780 bytes
//     *puVar2 = 0xffffffff;                        ; sentinel at record base + 0x28
//     puVar2 += 0xc3;                              ; stride 0xc3 dwords = 0x30c bytes
//   } while ((uint)puVar2 < 0x008aa278);           ; 4 full records
//
// 39-byte leaf. No params, no callees. Re-init of the table at 0x008a9620.
// U-3637 (record semantics) is data-semantic only — non-blocking for a
// bit-identical leaf.
// ---------------------------------------------------------------------------

// [moved to Match/MatchS01.cpp: C4-by-match port body] 0x00407a60 UtilTableInit8a9620


// ---------------------------------------------------------------------------
// FUN_0040ad30  --  0x0040ad30   (void(uint32) global setter)
//
// Per note (re/analysis/timer_d3_cont1_a/0x0040ad30.md):
//   DAT_008a95ac = param_1 ; ret    — 9-byte leaf setter.
// ---------------------------------------------------------------------------

// MATCH-S02: the body of 0x0040ad30 (UtilSet8a95ac) moved to Match/MatchS02_0040ad30.cpp,
// which compiles the byte-matched source re/match/s02/0040ad30.c.

// ---------------------------------------------------------------------------
// FUN_0040b250  --  0x0040b250   (void(void) score/timer reset)
//
// Per note (re/analysis/game_state_d5_cont2/0x0040b250.md):
//   Writes 0xfffffc18 (-1000) to 8 globals in 0x008a9500..0x008a952c, in the
//   order: 9500, 9504, 9508, 9520, 9524, 9528, 950c, 952c. 51-byte leaf.
// ---------------------------------------------------------------------------

// MATCH-S02: the body of 0x0040b250 (UtilReset8a9500) moved to Match/MatchS02_0040b250.cpp,
// which compiles the byte-matched source re/match/s02/0040b250.c.

// ---------------------------------------------------------------------------
// FUN_0040b430  --  0x0040b430   (void(int idx, int delta) indexed accumulator)
//
// Per note (re/analysis/game_state/0x0040b430.md):
//   if (idx > 3)  *(int*)0x008a94fc        += delta;
//   else          (&DAT_008a94f0)[idx]     += delta;   // int array, indices 0..3
//   46-byte leaf. (0x008a94fc == (&0x008a94f0)[3].)  U-0489 data-semantic only.
// ---------------------------------------------------------------------------

// MATCH-S02: the body of 0x0040b430 (UtilFieldAdd8a94f0) moved to Match/MatchS02_0040b430.cpp,
// which compiles the byte-matched source re/match/s02/0040b430.c.

// ---------------------------------------------------------------------------
// FUN_0040b6d0  --  0x0040b6d0   (int(int idx) array getter)
//
// Per note (re/analysis/game_state/0x0040b6d0.md):
//   return (&DAT_008a94e0)[param_1];    — 11-byte pure getter, 4-entry int array.
//   U-0491 data-semantic only.
// ---------------------------------------------------------------------------

// MATCH-S02: the body of 0x0040b6d0 (UtilArrayGet8a94e0) moved to Match/MatchS02_0040b6d0.cpp,
// which compiles the byte-matched source re/match/s02/0040b6d0.c.

// ---------------------------------------------------------------------------
// sub_004292c0  --  0x004292c0   (void(uint32* p, uint32 v) indirect store)
//
// Per note (re/analysis/timer_d3_cont1_b/0x004292c0.md):
//   *param_1 = param_2 ; ret     — 10-byte trivial indirect write (cdecl stack args).
// ---------------------------------------------------------------------------

// MATCH-S06: the body of 0x004292c0 (UtilWritePtr) moved to Match/MatchS06_004292c0.cpp,
// which compiles the byte-matched source re/match/s06/004292c0.c.

// ---------------------------------------------------------------------------
// sub_0042aab0  --  0x0042aab0   (void(void) two-constant initializer)
//
// Per note (re/analysis/timer_d3_cont1_b/0x0042aab0.md):
//   DAT_00898a90 = 0x180 (384) ; DAT_00898ab0 = 0 ; ret    — 20-byte leaf.
// ---------------------------------------------------------------------------

// MATCH-S06: the body of 0x0042aab0 (UtilInit898a90) moved to Match/MatchS06_0042aab0.cpp,
// which compiles the byte-matched source re/match/s06/0042aab0.c.

// ---------------------------------------------------------------------------
// FUN_00430b00  --  0x00430b00   (TimeDisplay::SetEntry — 2D-indexed 3-field write)
//
// Per note (re/analysis/leaderboard_d2/0x00430b00.md):
//   iVar1 = (param_2 + param_1 * 2) * 0xc;                 // byte offset, element=[p1][p2]
//   *(undefined4*)(&DAT_008989e0 + iVar1) = param_3;       // field +0
//   *(undefined4*)(&DAT_008989e4 + iVar1) = param_4;       // field +4
//   *(undefined4*)(&DAT_008989e8 + iVar1) = param_5;       // field +8
//   47-byte leaf. Buffer base 0x008989e0; 12-byte elements; p1,p2 in {0,1}.
// ---------------------------------------------------------------------------

// 0x00430b00
extern "C" __declspec(dllexport) void __cdecl TimeDisplaySetEntry(
        int param_1, int param_2, std::uint32_t v3, std::uint32_t v4, std::uint32_t v5) {
    int off = (param_2 + param_1 * 2) * 0xc;
    *reinterpret_cast<std::uint32_t*>(0x008989e0u + off) = v3;
    *reinterpret_cast<std::uint32_t*>(0x008989e4u + off) = v4;
    *reinterpret_cast<std::uint32_t*>(0x008989e8u + off) = v5;
}

RH_ScopedInstall(TimeDisplaySetEntry, 0x00430b00);
