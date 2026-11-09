// MATCH-S06: port body of 0x0042a6b0 = the byte-matched source re/match/s06/0042a6b0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (new body: the RVA had no port body).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
// Callees: 0x0042a530 = FUN_0042a530 (Match/MatchS06_0042a530.cpp); 0x004b3d20 / 0x004b3d80: no port body, called at the original address.
// strchr = the CRT strchr (C linkage).

#define MATCH_PORT 1
#define FUN_004b3d20 MatchS06_0042a6b0_Call004b3d20
#define FUN_004b3d80 MatchS06_0042a6b0_Call004b3d80
#ifdef MASHED_STANDALONE
// [PORT-TOP-04G] exe, reached on the MASHED_ORIG_TOP path from the matched 0x0040bbb0 (Match/PortTop_0040bbb0.cpp):
//  - rva 0x004b3d80 -> the matched exe body Orig_004b3d80 (Match/PortTop_004b3d80.cpp, TXD read, chunk 0x23). This source
//    passes one argument and the body takes three (its 2nd / 3rd go to RwStreamFindChunk as the length / version out
//    pointers). In the original they are whatever the caller's stack holds there. Declared deviation: the forwarder
//    passes 0 / 0, so the find-chunk binding writes neither (D3d9Render/RwWorldStream.cpp skips a null out pointer).
//  - rva 0x004b3d20 -> stand-in (Match/PortTop_0040bbb0.cpp): its chunk-0x16 reader 0x004cf7d0 has no matched source
//    (C2). The sfx.piz names 0x0040bbb0 asks for are found at the root (search kind 1), which takes 0x004b3d80.
extern "C" unsigned int Orig_004b3d80(unsigned int p0, unsigned int p1, unsigned int p2);
extern "C" int PortTop_StandIn04G_004b3d20(char * p0);
extern "C" int MatchS06_0042a6b0_Call004b3d20(char * p0) {
    return PortTop_StandIn04G_004b3d20(p0);
}
extern "C" int MatchS06_0042a6b0_Call004b3d80(char * p0) {
    return static_cast<int>(Orig_004b3d80(static_cast<unsigned int>(reinterpret_cast<unsigned long>(p0)), 0u, 0u));
}
#else
extern "C" int MatchS06_0042a6b0_Call004b3d20(char * p0) {
    return reinterpret_cast<int (__cdecl*)(char *)>(0x004b3d20u)(p0);
}
extern "C" int MatchS06_0042a6b0_Call004b3d80(char * p0) {
    return reinterpret_cast<int (__cdecl*)(char *)>(0x004b3d80u)(p0);
}
#endif
#pragma comment(linker, "/EXPORT:FUN_0042a6b0=_FUN_0042a6b0")
extern "C" {
#include "../../../../re/match/s06/0042a6b0.c"
}
