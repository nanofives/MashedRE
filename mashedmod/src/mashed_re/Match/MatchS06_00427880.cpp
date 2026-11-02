// MATCH-S06: port body of 0x00427880 = the byte-matched source re/match/s06/00427880.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (new body: the port had no body of this RVA).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).

#define MATCH_PORT 1
#define FUN_004cc160 MatchS06_00427880_Call004cc160
#define FUN_004cc230 MatchS06_00427880_Call004cc230
#define FUN_00554390 MatchS06_00427880_Call00554390
// Callees with no port body in either target: raw-address forwarders (S04 rule), the call the original makes.
extern "C" int MatchS06_00427880_Call004cc160(void *stream, void *a) {
    return reinterpret_cast<int (__cdecl*)(void *, void *)>(0x004cc160u)(stream, a);
}
extern "C" void * MatchS06_00427880_Call004cc230(int type, int mode, const char *name) {
    return reinterpret_cast<void * (__cdecl*)(int, int, const char *)>(0x004cc230u)(type, mode, name);
}
extern "C" int MatchS06_00427880_Call00554390(void *stream) {
    return reinterpret_cast<int (__cdecl*)(void *)>(0x00554390u)(stream);
}
// FUN_004cc5e0: the port body mashed_re::D3d9Render::Rwl_StreamFindChunk (D3d9Render/RwWorldStream.cpp) is exe-only, C++ linkage.
// The .asi has no body of 0x004cc5e0, so there it is a raw-address forwarder (S04 rule).
#ifdef MASHED_STANDALONE
namespace mashed_re { namespace D3d9Render {
int Rwl_StreamFindChunk(void* stream, int type, int* lenOut, unsigned* verOut);
} }
extern "C" int MatchS06_00427880_Call004cc5e0(void *stream, int id, void *a, void *b) {
    return mashed_re::D3d9Render::Rwl_StreamFindChunk((void*)(stream), (int)(id), (int*)(a), (unsigned*)(b));
}
#else
extern "C" int MatchS06_00427880_Call004cc5e0(void *stream, int id, void *a, void *b) {
    return reinterpret_cast<int (__cdecl*)(void *, int, void *, void *)>(0x004cc5e0u)(stream, id, a, b);
}
#endif
#define FUN_004cc5e0 MatchS06_00427880_Call004cc5e0
#pragma comment(linker, "/EXPORT:FUN_00427880=_FUN_00427880")
extern "C" {
#include "../../../../re/match/s06/00427880.c"
}
