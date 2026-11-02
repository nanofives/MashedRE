// MATCH-S06: port body of 0x00429a30 = the byte-matched source re/match/s06/00429a30.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Frontend/MenuMenusB.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
// Callees:
//   0x00430790 -> GetDat0067f17c (Util/SmallLeaves_o6.cpp)
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00429a30 MenuMenusBE
#define FUN_00430790 GetDat0067f17c
#pragma comment(linker, "/EXPORT:MenuMenusBE=_MenuMenusBE")
extern "C" {
#include "../../../../re/match/s06/00429a30.c"
}
RH_ScopedInstall(MenuMenusBE, 0x00429a30);
