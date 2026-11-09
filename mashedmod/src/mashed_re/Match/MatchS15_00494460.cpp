// MATCH-S15: port body of 0x00494460 = the byte-matched source re/match/s15/00494460.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Frontend/IntroSplash.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).


// [PORT-TOP-04C] exe: compiled in mashed_re.exe too (one body in both targets), reached from AppDestroy 0x00402a40 through
// the thunk 0x00494f20 when 0x00494ef0 reads DAT_00771a04 != 0. Its callees bind by name there: 0x00494320 to its matched
// source (Match/PortTop_00494320.cpp), RwRasterDestroy 0x004c7650 to LibRw/RwApiBind.cpp. The .asi keeps the forwarders.
#ifdef MASHED_STANDALONE
#define FUN_00494320 Orig_00494320                       // matched, Match/PortTop_00494320.cpp
#define FUN_004c7650 MashedRw_RasterDestroy_004c7650     // RW: RwRasterDestroy -> rw::Raster::destroy
#else
// Raw-address forwarders: the old body was a call into the original 0x00494460 itself, which calls
// these two through their addresses (no port body of either on this branch).
extern "C" void MatchS15_00494460_Call00494320(void) {
    typedef void (__cdecl* Fn)(void);
    reinterpret_cast<Fn>(0x00494320u)();
}
extern "C" unsigned int MatchS15_00494460_Call004c7650(unsigned int p0) {
    typedef unsigned int (__cdecl* Fn)(unsigned int);
    return reinterpret_cast<Fn>(0x004c7650u)(p0);
}
#define FUN_00494320 MatchS15_00494460_Call00494320
#define FUN_004c7650 MatchS15_00494460_Call004c7650
#endif
#define MATCH_PORT 1
#define FUN_00494460 IntroVideoClose
#pragma comment(linker, "/EXPORT:IntroVideoClose=_IntroVideoClose")
extern "C" {
#include "../../../../re/match/s15/00494460.c"
}
