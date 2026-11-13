// Mashed RE - Frontend/MenuMeasure.h
// The one body of FUN_004282a0 (menu message width), shared by both targets: since MATCH-S06-FU (2026-11-02) it is the
// byte-matched source re/match/s06/004282a0.c, compiled by Match/MatchS06_004282a0.cpp (x87 TU). See that file for the
// per-target binding of its globals and callees.
#pragma once
#include <cstdint>

// The body. __cdecl, result in ST0 (the original's float10 return).
extern "C" float __cdecl MenuMenusBA(int id, float scale);

// The exe's binding (defined in exe_main.cpp, read only by Match/MatchS06_004282a0.cpp under MASHED_STANDALONE):
//   MenuBa_ExeLookup   FUN_00427780's slot: the [u16 len][len * u16] record of the id in g_msg_dat (the shape
//                      FUN_00427780 returns), or a zero-length record when the id is outside the table
//   MenuBa_ExeMeasure  FUN_005554d0's slot: the g_font glyph sum times size; the font argument is unused
// FUN_004277a0 binds to the S06 body TextCtrlCodeRemapBody in both targets, and the globals DAT_0067d838 /
// DAT_0067d830 / DAT_007d3ff8 to their original addresses (DDKOBJ-CELLS, 2026-11-12).
extern "C" short* MenuBa_ExeLookup(int id);
extern "C" float  MenuBa_ExeMeasure(int font, short* text, float size);
