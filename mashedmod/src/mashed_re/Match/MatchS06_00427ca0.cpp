// MATCH-S06: port body of 0x00427ca0 = the byte-matched source re/match/s06/00427ca0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (new body: the port had no body of this RVA).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).

#define MATCH_PORT 1
#ifdef MASHED_STANDALONE
// [HUDINIT-B] exe callees bind by name, no raw-address call. The exe runs this body only on the MASHED_ORIG_TOP path with
// MASHED_ORIG_TOP_RWMOD=1, from the selector PortTop_00402750_Call00427ca0 (Match/PortTop_00552b60.cpp).
//  - 0x00552b60: the Rt2d open, PortTop_FontSysInit_00552b60(camera) (Match/PortTop_00552b60.cpp, HUDINIT-A)
//  - 0x00556cc0: the matched Orig_00556cc0 (Match/PortTop_00556cc0.cpp), not the hand SetDat00912a20
//  - 0x00556d70: the matched brush create Orig_00556d70 (Match/PortTop_00556d70.cpp)
//  - 0x00556e90: the stand-in PortTop_StandInHIB_00556e90 (97.5% source, residual outside L-115)
//  - 0x00427880: the stand-in PortTop_StandInHIB_00427880 (the font-file loader stays a stand-in until HUDINIT-C)
//  - 0x0042a6b0: the exe body FUN_0042a6b0 (Match/MatchS06_0042a6b0.cpp, PORT-TOP-04G)
#define FUN_00552b60 PortTop_FontSysInit_00552b60
#define FUN_00556cc0 Orig_00556cc0
#define FUN_00556d70 Orig_00556d70
#define FUN_00556e90 PortTop_StandInHIB_00556e90
#define FUN_00427880 PortTop_StandInHIB_00427880
#else
#define FUN_00556cc0 SetDat00912a20
#define FUN_00556d70 MatchS06_00427ca0_Call00556d70
#define FUN_00556e90 MatchS06_00427ca0_Call00556e90
// Callees with no port body in the .asi: raw-address forwarders (S04 rule), the call the original makes.
extern "C" int MatchS06_00427ca0_Call00556d70(void) {
    return reinterpret_cast<int (__cdecl*)()>(0x00556d70u)();
}
extern "C" int MatchS06_00427ca0_Call00556e90(int font, void *c0, void *c1, void *c2, void *c3) {
    return reinterpret_cast<int (__cdecl*)(int, void *, void *, void *, void *)>(0x00556e90u)(font, c0, c1, c2, c3);
}
#define FUN_00552b60 FontSys_InitSeq
#endif
#pragma comment(linker, "/EXPORT:FUN_00427ca0=_FUN_00427ca0")
extern "C" {
#include "../../../../re/match/s06/00427ca0.c"
}
