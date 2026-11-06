// MATCH-S16: port body of 0x00499690 = the byte-matched source re/match/s16/00499690.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Boot/Window.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00499690 WindowMsgPump
#pragma comment(linker, "/EXPORT:WindowMsgPump=_WindowMsgPump")
extern "C" {
#include "../../../../re/match/s16/00499690.c"
}
RH_ScopedInstall(WindowMsgPump, 0x00499690);
