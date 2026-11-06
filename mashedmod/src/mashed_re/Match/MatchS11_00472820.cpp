// MATCH-S11: port body of 0x00472820 = the byte-matched source re/match/s11/00472820.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/Vec3ValidateMixed_cfd622.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00472820 Vec3IsValidNonTiny
#define FUN_004a3907 MatchS11_00472820_Call004a3907
// Raw-address forwarder: no port body of 0x004a3907 exists; call the original image's routine (cdecl, double arg).
extern "C" int MatchS11_00472820_Call004a3907(double x) {
    return reinterpret_cast<int(__cdecl*)(double)>(0x004a3907u)(x);
}
#pragma comment(linker, "/EXPORT:Vec3IsValidNonTiny=_Vec3IsValidNonTiny")
extern "C" {
#include "../../../../re/match/s11/00472820.c"
}
RH_ScopedInstall(Vec3IsValidNonTiny, 0x00472820);
