// MATCH-S10: port body of 0x0045c640 = the byte-matched source re/match/s10/0045c640.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/PromoLoop_round60.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
// REGISTER CONTRACT (from the old naked body, BUGFIX 2026-07-27): the original touches only EAX, and its
// unhooked caller FUN_0045c6b0 carries its loop index in EDX across the call (0x0045c6b1 xor edx,edx;
// 0x0045c6c6 mov [edx*4+0x0088f0c0],0), so this body must not clobber EDX. VS2022 /O2 compiles the
// source below to EAX+ECX only (checked 2026-11-02, cxxasm listing); ECX is dead after the call in all
// five callers (0x0045c6b5, 0x0045cb3f, 0x0045cbe7, 0x0045cfe6, 0x0045d086 each rewrite ECX before reading it).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0045c640 Sel88fbc4
#pragma comment(linker, "/EXPORT:Sel88fbc4=_Sel88fbc4")
extern "C" {
#include "../../../../re/match/s10/0045c640.c"
}
RH_ScopedInstall(Sel88fbc4, 0x0045c640);
