// MATCH-S13: port body of 0x0047c160 = the byte-matched source re/match/s13/0047c160.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of HUD/ScenarioLeaves_sa2.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).


#define MATCH_PORT 1
#define FUN_0047c160 CameraPathNodeContainTest
#define FUN_004c0ed0 MatchS13_0047c160_Call004c0ed0
#define FUN_004c1b40 MatchS13_0047c160_Call004c1b40
#define FUN_0047bb10 MatchS13_0047c160_Call0047bb10
#pragma comment(linker, "/EXPORT:CameraPathNodeContainTest=_CameraPathNodeContainTest")
extern "C" {
#include "../../../../re/match/s13/0047c160.c"
// Callees of the matched source: the old HUD/ScenarioLeaves_sa2.cpp body called 0x004c0ed0, 0x004c1b40 and 0x0047bb10
// through their original addresses (s_F004c0ed0 / s_F004c1b40 / s_F0047bb10, __cdecl function-pointer casts; no port
// body exists for any of them), so these forwarders make the same calls. Defined after the include so the .c's types are in scope.
int MatchS13_0047c160_Call004c0ed0(int p) {
    return reinterpret_cast<int (__cdecl*)(int)>(0x004c0ed0u)(p);
}
int MatchS13_0047c160_Call004c1b40(int q, void *box) {
    return reinterpret_cast<int (__cdecl*)(int, void *)>(0x004c1b40u)(q, box);
}
void MatchS13_0047c160_Call0047bb10(SectRec *rec, int pos, SectBuf *buf, int *flag) {
    reinterpret_cast<void (__cdecl*)(SectRec *, int, SectBuf *, int *)>(0x0047bb10u)(rec, pos, buf, flag);
}
}
