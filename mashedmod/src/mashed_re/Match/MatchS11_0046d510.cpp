// MATCH-S11: port body of 0x0046d510 = the byte-matched source re/match/s11/0046d510.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Ai/VehicleVelocityWorldGet.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0046d510 VehicleVelocityWorldGet
#define FUN_004c3df0 MatchS11_0046d510_Call004c3df0
#pragma comment(linker, "/EXPORT:VehicleVelocityWorldGet=_VehicleVelocityWorldGet")
extern "C" {
#include "../../../../re/match/s11/0046d510.c"
}

// Callee of the matched source: the old Ai/VehicleVelocityWorldGet.cpp body called 0x004c3df0 through its original
// address (a __cdecl function-pointer cast), not the port's Math/RwV3dTransformPoints.cpp body, so this forwarder
// makes that same call. Defined after the include: its parameter types are the matched source's own structs.
extern "C" int MatchS11_0046d510_Call004c3df0(V3_0046d510 *out, V3_0046d510 *in, int n, Mat_0046d510 *m) {
    return reinterpret_cast<int (__cdecl*)(V3_0046d510*, V3_0046d510*, int, Mat_0046d510*)>(0x004c3df0u)(out, in, n, m);
}
RH_ScopedInstall(VehicleVelocityWorldGet, 0x0046d510);
