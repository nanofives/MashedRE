// MATCH-S06: port body of 0x00427880 = the byte-matched source re/match/s06/00427880.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (new body: the port had no body of this RVA).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).

#define MATCH_PORT 1
#define FUN_004cc160 MatchS06_00427880_Call004cc160
#define FUN_004cc230 MatchS06_00427880_Call004cc230
#define FUN_00554390 MatchS06_00427880_Call00554390
#ifdef MASHED_STANDALONE
// [RWC-FRAME-CLUMP] exe: the only caller is MatchS06_00427ca0_Sel00427880 (Match/MatchS06_00427ca0.cpp) under
// MASHED_RWC_FRAME (on the knob path without it 0x00427880 stays the stand-in PortTop_StandInHIB_00427880). The stream
// open / close go through the PORT-TOP-04F bindings (Match/PortTop_RwStream04F.cpp: the exe has no RtFSManager, a type-2
// open is served from the piz mounts as a memory stream), the font read is the RW core body RwCore_00554390
// (re/match/rwl05/00554390.c, verify/rwccorerun_20261111/RwCore_Bodies.tsv).
extern "C" {
void* PortTop_RwStreamOpen_004cc230(int type, int access, const void* data);
int PortTop_RwStreamClose_004cc160(void* stream, void* memOut);
void RwCore_00554390(void);
}
extern "C" int MatchS06_00427880_Call004cc160(void *stream, void *a) {
    return PortTop_RwStreamClose_004cc160(stream, a);
}
extern "C" void * MatchS06_00427880_Call004cc230(int type, int mode, const char *name) {
    return PortTop_RwStreamOpen_004cc230(type, mode, name);
}
extern "C" int MatchS06_00427880_Call00554390(void *stream) {
    return reinterpret_cast<int (__cdecl*)(void *)>(&RwCore_00554390)(stream);
}
#else
// Callees with no port body in the .asi: raw-address forwarders (S04 rule), the call the original makes.
extern "C" int MatchS06_00427880_Call004cc160(void *stream, void *a) {
    return reinterpret_cast<int (__cdecl*)(void *, void *)>(0x004cc160u)(stream, a);
}
extern "C" void * MatchS06_00427880_Call004cc230(int type, int mode, const char *name) {
    return reinterpret_cast<void * (__cdecl*)(int, int, const char *)>(0x004cc230u)(type, mode, name);
}
extern "C" int MatchS06_00427880_Call00554390(void *stream) {
    return reinterpret_cast<int (__cdecl*)(void *)>(0x00554390u)(stream);
}
#endif
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
