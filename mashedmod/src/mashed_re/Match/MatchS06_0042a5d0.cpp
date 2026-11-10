// MATCH-S06: port body of 0x0042a5d0 = the byte-matched source re/match/s06/0042a5d0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (new body: the RVA had no port body).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
// Callees: 0x0042a530 = FUN_0042a530 (Match/MatchS06_0042a530.cpp); 0x004b3bf0: no port body, called at the original address.
// strchr = the CRT strchr (C linkage).

#define MATCH_PORT 1
#define FUN_004b3bf0 MatchS06_0042a5d0_Call004b3bf0
#ifdef MASHED_STANDALONE
// [POWERUPINIT-A] exe: rva 0x004b3bf0 -> the matched exe body Orig_004b3bf0 (Match/PortTop_004b3bf0.cpp, DFF read,
// chunk 0x10), the shape of MatchS06_0042a6b0.cpp's 0x004b3d80 binding. The exe maps no original code, so the old
// raw-address call could not run. This source passes one argument and the body takes three (its 2nd / 3rd go to
// RwStreamFindChunk as the length / version out pointers). In the original they are whatever the caller's stack holds
// there. Declared deviation: the forwarder passes 0 / 0, so the find-chunk binding writes neither. Reached on the
// MASHED_ORIG_TOP path only, from the matched 0x004039c0 ("Bomb.dff", Match/PortTop_004039c0.cpp). Before this binding
// any exe call faulted, so no default-path caller exists.
extern "C" unsigned int Orig_004b3bf0(unsigned int p0, unsigned int p1, unsigned int p2);
extern "C" int MatchS06_0042a5d0_Call004b3bf0(char * p0) {
    return static_cast<int>(Orig_004b3bf0(static_cast<unsigned int>(reinterpret_cast<unsigned long>(p0)), 0u, 0u));
}
#else
extern "C" int MatchS06_0042a5d0_Call004b3bf0(char * p0) {
    return reinterpret_cast<int (__cdecl*)(char *)>(0x004b3bf0u)(p0);
}
#endif
#pragma comment(linker, "/EXPORT:FUN_0042a5d0=_FUN_0042a5d0")
extern "C" {
#include "../../../../re/match/s06/0042a5d0.c"
}
