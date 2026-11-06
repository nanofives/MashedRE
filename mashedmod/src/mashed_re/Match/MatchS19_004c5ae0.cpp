// MATCH-S19: port body of 0x004c5ae0 = the byte-matched source re/match/s19/004c5ae0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Render/TextureLoaderCluster.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#include <cstdarg>

#define MATCH_PORT 1
#define FUN_004c5ae0 RwTextureSetName
#define FUN_004d7ff0 MatchS19_004c5ae0_Call004d7ff0
#define FUN_004d8480 MatchS19_004c5ae0_Call004d8480
#pragma comment(linker, "/EXPORT:RwTextureSetName=_RwTextureSetName")
extern "C" {
#include "../../../../re/match/s19/004c5ae0.c"
}
// Raw-address forwarder: the old body called 0x004d8480 through its raw address (no port body on that path).
extern "C" RwError * MatchS19_004c5ae0_Call004d8480(RwError *p0) {
    return reinterpret_cast<RwError *(__cdecl*)(RwError *)>(0x004d8480u)(p0);
}
// Raw-address forwarder: the old body called 0x004d7ff0 through its raw address. The matched source declares it
// variadic and always passes 4 ints after the code (0x8000001e, name, 0x20, 0x1f, name[0x1f]); forward exactly those.
extern "C" int MatchS19_004c5ae0_Call004d7ff0(int code, ...) {
    va_list ap;
    va_start(ap, code);
    const int a = va_arg(ap, int), b = va_arg(ap, int), c = va_arg(ap, int), d = va_arg(ap, int);
    va_end(ap);
    return reinterpret_cast<int(__cdecl*)(int, ...)>(0x004d7ff0u)(code, a, b, c, d);
}
RH_ScopedInstall(RwTextureSetName, 0x004c5ae0);
