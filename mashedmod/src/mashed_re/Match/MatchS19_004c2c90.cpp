// MATCH-S19: port body of 0x004c2c90 = the byte-matched source re/match/s19/004c2c90.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Render/PromoLoop_round250.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#include <cstdarg>

#define MATCH_PORT 1
#define FUN_004c2c90 DriverSystemDispatch
#define FUN_004d7ff0 MatchS19_004c2c90_Call004d7ff0
#define FUN_004d8480 MatchS19_004c2c90_Call004d8480
#pragma comment(linker, "/EXPORT:DriverSystemDispatch=_DriverSystemDispatch")
extern "C" {
#include "../../../../re/match/s19/004c2c90.c"
}
// Raw-address forwarder: the old body called 0x004d8480 through its raw address (no port body on that path).
extern "C" RwError_t * MatchS19_004c2c90_Call004d8480(RwError_t *p0) {
    return reinterpret_cast<RwError_t *(__cdecl*)(RwError_t *)>(0x004d8480u)(p0);
}
// Raw-address forwarder: the old body called 0x004d7ff0 through its raw address. The matched source declares it
// variadic and passes one int after the code (0x18, req); forward exactly that.
extern "C" int MatchS19_004c2c90_Call004d7ff0(int p0, ...) {
    va_list ap;
    va_start(ap, p0);
    const int a = va_arg(ap, int);
    va_end(ap);
    return reinterpret_cast<int(__cdecl*)(int, ...)>(0x004d7ff0u)(p0, a);
}
RH_ScopedInstall(DriverSystemDispatch, 0x004c2c90);
