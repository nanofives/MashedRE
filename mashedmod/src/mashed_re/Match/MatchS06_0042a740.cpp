// MATCH-S06: port body of 0x0042a740 = the byte-matched source re/match/s06/0042a740.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (new body: the RVA had no port body).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
// Callees: 0x0042a530 = FUN_0042a530 (Match/MatchS06_0042a530.cpp); 0x004cc5e0 = Rwl_StreamFindChunk
// (D3d9Render/RwWorldStream.cpp) in the exe, the only target with a port body of it, and the original
// address in the .asi; 0x004cc160 / 0x004cc230 / 0x00558df0: no port body, called at the original address;
// _strchr (the original's CRT strchr) = the CRT strchr.

#define MATCH_PORT 1
#define FUN_004cc160 MatchS06_0042a740_Call004cc160
#define FUN_004cc230 MatchS06_0042a740_Call004cc230
#define FUN_00558df0 MatchS06_0042a740_Call00558df0
#define FUN_004cc5e0 MatchS06_0042a740_Call004cc5e0
#define _strchr MatchS06_0042a740_strchr
extern "C" bool MatchS06_0042a740_Call004cc160(int p0, int p1) {
    return reinterpret_cast<bool (__cdecl*)(int, int)>(0x004cc160u)(p0, p1);
}
extern "C" int MatchS06_0042a740_Call004cc230(unsigned int p0, int p1, void * p2) {
    return reinterpret_cast<int (__cdecl*)(unsigned int, int, void *)>(0x004cc230u)(p0, p1, p2);
}
extern "C" unsigned int MatchS06_0042a740_Call00558df0(void * p0, int p1) {
    return reinterpret_cast<unsigned int (__cdecl*)(void *, int)>(0x00558df0u)(p0, p1);
}
#ifdef MASHED_STANDALONE
namespace mashed_re { namespace D3d9Render {
int Rwl_StreamFindChunk(void* stream, int type, int* lenOut, unsigned* verOut);
} }
extern "C" unsigned int MatchS06_0042a740_Call004cc5e0(int p0, int p1, unsigned int p2, unsigned int p3) {
    return static_cast<unsigned int>(mashed_re::D3d9Render::Rwl_StreamFindChunk(
        reinterpret_cast<void*>(p0), p1, reinterpret_cast<int*>(p2), reinterpret_cast<unsigned*>(p3)));
}
#else
extern "C" unsigned int MatchS06_0042a740_Call004cc5e0(int p0, int p1, unsigned int p2, unsigned int p3) {
    return reinterpret_cast<unsigned int (__cdecl*)(int, int, unsigned int, unsigned int)>(0x004cc5e0u)(p0, p1, p2, p3);
}
#endif
extern "C" char *strchr(const char *, int);
extern "C" char *MatchS06_0042a740_strchr(char *p0, int p1) {
    return strchr(p0, p1);
}
#pragma comment(linker, "/EXPORT:FUN_0042a740=_FUN_0042a740")
extern "C" {
#include "../../../../re/match/s06/0042a740.c"
}
