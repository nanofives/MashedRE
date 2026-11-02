// MATCH-S06-FU: port body of 0x004282a0 = the byte-matched source re/match/s06/004282a0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%, relocheck OK).
// The same source text is this RVA's only body in BOTH targets (replaces the naked body of Frontend/MenuMeasure.cpp,
// D4-DC-82a0's one shared body). x87 TU (mashedmod/x87_tus.txt). Compiled here as C++ with MATCH_PORT.
//
// Per-target binding (verify/matchs06fu_20261102/PREREG_MATCH-S06-FU.md). The standalone maps no original data: its
// own image covers 0x00400000..0x00A00000 (exe_main.cpp, WEDGE TRAP note), so the preamble's MGLOBAL (a reference to
// the original address) would read the exe's own .data. This TU therefore supplies its own MGLOBAL, and binds each of
// the body's globals and callees to what that target read before the swap:
//   source name     .asi                                      exe (exe_main.cpp)
//   DAT_0067d838    [0x0067d838]                              g_menuBa_FontCell (a zero dword; the exe's measure ignores it)
//   DAT_0067d830    [0x0067d830]                              g_menuBa_DivCell  (DeriveMenuMeasureDiv)
//   FUN_00427780    FontText_StringTableLookup (S06 body)     MenuBa_ExeLookup  (passes the id through)
//   FUN_004277a0    TextCtrlCodeRemapBody (S06 body)          MenuBa_ExeRemap   (GetMenuMessage over g_msg_dat)
//   FUN_005554d0    raw forwarder to the original             MenuBa_ExeMeasure (g_font glyph sum)
//   DAT_007d3ff8    [0x007d3ff8]                              a private zero (declared by the source, not read by the body)
// The exe caller (exe_main.cpp MeasureMenuMessage) scopes PC_24, the precision the original's game thread runs at.
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_004282a0 MenuMenusBA
#pragma comment(linker, "/EXPORT:MenuMenusBA=_MenuMenusBA")

#ifdef MASHED_STANDALONE
#define FUN_00427780 MenuBa_ExeLookup
#define FUN_004277a0 MenuBa_ExeRemap
#define FUN_005554d0 MenuBa_ExeMeasure
extern "C" const void*  g_menuBa_FontCell;
extern "C" const float* g_menuBa_DivCell;
static int s_menuBa_unused_7d3ff8 = 0;
#define MATCH_CELL_DAT_0067d838 (g_menuBa_FontCell)
#define MATCH_CELL_DAT_0067d830 (g_menuBa_DivCell)
#define MATCH_CELL_DAT_007d3ff8 (&s_menuBa_unused_7d3ff8)
#else
#define FUN_00427780 FontText_StringTableLookup
#define FUN_004277a0 TextCtrlCodeRemapBody
// FUN_005554d0: no port body (HUD/HudBatch.cpp FontText_StringWidthAccumulator is under #if 0), so a raw-address
// forwarder (S04 rule), the call the original makes.
extern "C" float MatchS06_004282a0_Call005554d0(int font, short *text, float size) {
    return reinterpret_cast<float (__cdecl*)(int, short *, float)>(0x005554d0u)(font, text, size);
}
#define FUN_005554d0 MatchS06_004282a0_Call005554d0
#define MATCH_CELL_DAT_0067d838 (0x0067d838u)
#define MATCH_CELL_DAT_0067d830 (0x0067d830u)
#define MATCH_CELL_DAT_007d3ff8 (0x007d3ff8u)
#endif

// This TU's preamble: the source's own preamble is skipped (MATCH_PREAMBLE), and MGLOBAL binds through the cells above.
// The source uses no other preamble name (only int / short / float).
#define MATCH_PREAMBLE 1
#define MGLOBAL(T, n, a) static T &n = *(T *)(MATCH_CELL_##n)
extern "C" {
#include "../../../../re/match/s06/004282a0.c"
}
RH_ScopedInstall(MenuMenusBA, 0x004282a0);  // installs in the .asi, no-op in the exe
