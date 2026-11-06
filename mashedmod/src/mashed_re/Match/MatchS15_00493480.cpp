// MATCH-S15: port body of 0x00493480 = the byte-matched source re/match/s15/00493480.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Boot/FrameDispatch.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00493480 FpsDiscretise
#define FUN_00493390 MatchS15_00493480_Call00493390
// Callees of the matched source: the old Boot/FrameDispatch.cpp body called each one through its original address, so these
// forwarders make that same raw-address call (in the .asi an installed callee hook is reached through its JMP).
extern "C" void MatchS15_00493480_Call00493390(void) {
    reinterpret_cast<void (__cdecl*)()>(0x00493390u)();
}
#pragma comment(linker, "/EXPORT:FpsDiscretise=_FpsDiscretise")
extern "C" {
#include "../../../../re/match/s15/00493480.c"
}
RH_ScopedInstall(FpsDiscretise, 0x00493480);
