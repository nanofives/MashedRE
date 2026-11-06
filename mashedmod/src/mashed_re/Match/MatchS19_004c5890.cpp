// MATCH-S19: port body of 0x004c5890 = the byte-matched source re/match/s19/004c5890.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Render/TextureLoader_q6.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_004c5890 RwTexDictionaryCreate
#define FUN_004d8000 MatchS19_004c5890_Call004d8000
#pragma comment(linker, "/EXPORT:RwTexDictionaryCreate=_RwTexDictionaryCreate")
extern "C" {
#include "../../../../re/match/s19/004c5890.c"
}
// Raw-address forwarder: the old body called 0x004d8000 through its raw address (no port body on that path).
extern "C" int MatchS19_004c5890_Call004d8000(void *p0, void *p1) {
    return reinterpret_cast<int(__cdecl*)(void *, void *)>(0x004d8000u)(p0, p1);
}
RH_ScopedInstall(RwTexDictionaryCreate, 0x004c5890);
