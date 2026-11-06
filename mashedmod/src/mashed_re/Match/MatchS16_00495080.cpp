// MATCH-S16: port body of 0x00495080 = the byte-matched source re/match/s16/00495080.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Render/PromoLoop_round256.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
// The matched file also holds 0x00494f40 and 0x00494fd0 (100%, static, no port body anywhere): this TU is their only
// port body too. Only 0x00495080 is installed; its in-port FUN_00494fd0 takes the device as a plain argument.
#define FUN_00495080 Fwd494fd0_DoublePlusHalf
#define FUN_004a2d30 MatchS16_00495080_Call004a2d30
// Callee of the matched source: FUN_004a2d30 (CRT pow, stack entry) has no port body (rva_body_scan). The old body
// reached it only through the original FUN_00494fd0 -> FUN_00494f40; this forwarder calls it at its original address.
extern "C" double MatchS16_00495080_Call004a2d30(double x, double y) {
    return reinterpret_cast<double (__cdecl*)(double, double)>(0x004a2d30u)(x, y);
}
#pragma comment(linker, "/EXPORT:Fwd494fd0_DoublePlusHalf=_Fwd494fd0_DoublePlusHalf")
extern "C" {
#include "../../../../re/match/s16/00495080.c"
}
RH_ScopedInstall(Fwd494fd0_DoublePlusHalf, 0x00495080);
