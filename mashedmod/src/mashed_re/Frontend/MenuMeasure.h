// Mashed RE - Frontend/MenuMeasure.h
// The one body of FUN_004282a0 (menu message width), shared by both targets: since MATCH-S06-FU (2026-11-02) it is the
// byte-matched source re/match/s06/004282a0.c, compiled by Match/MatchS06_004282a0.cpp (x87 TU). See that file for the
// per-target binding of its globals and callees.
#pragma once
#include <cstdint>

// The body. __cdecl, result in ST0 (the original's float10 return).
extern "C" float __cdecl MenuMenusBA(int id, float scale);

// The exe's binding (defined in exe_main.cpp, read only by Match/MatchS06_004282a0.cpp under MASHED_STANDALONE):
//   MenuBa_ExeLookup   FUN_00427780's slot: returns the id itself as the "string pointer"
//   MenuBa_ExeRemap    FUN_004277a0's slot: GetMenuMessage(id, dst, 0x200) over g_msg_dat (applies the remap)
//   MenuBa_ExeMeasure  FUN_005554d0's slot: the g_font glyph sum times size; the font argument is unused
//   g_menuBa_FontCell  address of the dword the body reads as DAT_0067d838 (a zero dword)
//   g_menuBa_DivCell   address of the float the body reads as DAT_0067d830 (DeriveMenuMeasureDiv)
extern "C" short* MenuBa_ExeLookup(int id);
extern "C" void   MenuBa_ExeRemap(short* src, short* dst);
extern "C" float  MenuBa_ExeMeasure(int font, short* text, float size);
extern "C" const void*  g_menuBa_FontCell;
extern "C" const float* g_menuBa_DivCell;
