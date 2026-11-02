// MATCH-S13: port body of 0x0047c2d0 = the byte-matched source re/match/s13/0047c2d0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Camera/CameraPathPredicates.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0047c2d0 CameraPathAnyNodeNonzero
#define FUN_00478030 MatchS13_0047c2d0_Call00478030
#pragma comment(linker, "/EXPORT:CameraPathAnyNodeNonzero=_CameraPathAnyNodeNonzero")
extern "C" {
#include "../../../../re/match/s13/0047c2d0.c"
// Callee of the matched source's in-file inner predicate (0x0047c1f0, which the old Camera/CameraPathPredicates.cpp scan
// called through its original address, so it reached 0x00478030 inside the original image): no port body exists for
// 0x00478030, so this forwarder calls it at its original address. Defined after the include so SectItem is in scope.
int MatchS13_0047c2d0_Call00478030(SectItem *item, int q) {
    return reinterpret_cast<int (__cdecl*)(SectItem *, int)>(0x00478030u)(item, q);
}
}
RH_ScopedInstall(CameraPathAnyNodeNonzero, 0x0047c2d0);
