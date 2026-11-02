// MATCH-S13: port body of 0x0047c270 = the byte-matched source re/match/s13/0047c270.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Camera/CameraPathPredicates.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0047c270 CameraPathAllNodesEq2
#define FUN_004780c0 MatchS13_0047c270_Call004780c0
#pragma comment(linker, "/EXPORT:CameraPathAllNodesEq2=_CameraPathAllNodesEq2")
extern "C" {
#include "../../../../re/match/s13/0047c270.c"
// Callee of the matched source's in-file inner predicate (0x0047c230, which the old Camera/CameraPathPredicates.cpp scan
// called through its original address, so it reached 0x004780c0 inside the original image): no port body exists for
// 0x004780c0, so this forwarder calls it at its original address. Defined after the include so SectItem is in scope.
int MatchS13_0047c270_Call004780c0(SectItem *item, int q) {
    return reinterpret_cast<int (__cdecl*)(SectItem *, int)>(0x004780c0u)(item, q);
}
}
RH_ScopedInstall(CameraPathAllNodesEq2, 0x0047c270);
