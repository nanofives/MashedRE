// MATCH-S14: port body of 0x0048a830 = the byte-matched source re/match/s14/0048a830.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/PromoLoop_sessionB.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0048a830 ZeroFill48a830
#define FUN_004b6480 BitArrayClear
// FUN_004b6480 -> BitArrayClear (Input/DirectInput.cpp, installed at 0x004b6480 in the .asi). The old naked body
// called the raw address 0x004b6480, which the .asi redirects to that same port body.
#pragma comment(linker, "/EXPORT:ZeroFill48a830=_ZeroFill48a830")
extern "C" {
#include "../../../../re/match/s14/0048a830.c"
}
RH_ScopedInstall(ZeroFill48a830, 0x0048a830);
