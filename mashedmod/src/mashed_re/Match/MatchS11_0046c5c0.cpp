// MATCH-FIX-46C5C0: port body of 0x0046c5c0 = the byte-matched source re/match/s11/0046c5c0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%, relocheck OK).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of
// Frontend/SmallLeaves_n2.cpp, whose old body stored a BYTE 0 at 0x008815a4 + i*0x341; the original stores a DWORD 0 at
// 0x008815a4 + i*0xd04 (imul eax,eax,0xd04 at 0x0046c5d2, mov dword [eax+4],0 at 0x0046c5dd)).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL). Leaf: no callees.
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0046c5c0 VehicleSlotInit
#pragma comment(linker, "/EXPORT:VehicleSlotInit=_VehicleSlotInit")
extern "C" {
#include "../../../../re/match/s11/0046c5c0.c"
}
RH_ScopedInstall(VehicleSlotInit, 0x0046c5c0);
