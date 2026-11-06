// MATCH-FIX-46C5C0: port body of 0x0046c790 = the byte-matched source re/match/s11/0046c790.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%, relocheck OK).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of
// Frontend/SmallLeaves_n2.cpp, whose old body stored at 0x008815b0 + i*0x341; the original stores at 0x008815b0 + i*0xd04
// (imul eax,eax,0xd04 at 0x0046c7a1, mov [eax+0x8815b0],ecx at 0x0046c7a7)).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL). Leaf: no callees.
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0046c790 VehicleSlotFieldSet
#pragma comment(linker, "/EXPORT:VehicleSlotFieldSet=_VehicleSlotFieldSet")
extern "C" {
#include "../../../../re/match/s11/0046c790.c"
}
RH_ScopedInstall(VehicleSlotFieldSet, 0x0046c790);
