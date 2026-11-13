// MATCH-S18 (PORT-TOP-04K): port body of 0x004b52c0 = the byte-matched source re/match/s18/004b52c0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%, BYTE-IDENTICAL 35 B).
// The same source text is this RVA's only body in the targets that compile this TU. It replaces the hand body
// Or4b52c0 of Util/PromoLoop_sessionB.cpp (round 115), which had no clear arm: the matched source stores
// flags & ~mask when the third argument is 0, the hand body stored the flags unchanged.
// Compiled here as C++ with MATCH_PORT. Export name kept (re/frida/hooks_registry.py 'or_4b52c0').
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_004b52c0 Or4b52c0
#pragma comment(linker, "/EXPORT:Or4b52c0=_Or4b52c0")
extern "C" {
#include "../../../../re/match/s18/004b52c0.c"
}
RH_ScopedInstall(Or4b52c0, 0x004b52c0);
