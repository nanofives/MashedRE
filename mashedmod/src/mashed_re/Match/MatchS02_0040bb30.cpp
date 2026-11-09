// PORT-TOP-04G: port body of 0x0040bb30 = the byte-matched source re/match/s02/0040bb30.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py --bytes BYTE-IDENTICAL 21 B,
// relocheck OK checked=2). Forwarder: FUN_004c5c00(*DAT_0063b8f8 (FX.TXD), name).
// The same source text is this RVA's only body in both targets (the .asi body moved out of Util/PromoLoop_sessionB.cpp).
//  - .asi: FUN_004c5c00 -> the original address 0x004c5c00 (installed JMP into Util/PromoLoop_sessionB.cpp Search4c5c00).
//  - exe:  FUN_004c5c00 -> the RW binding MashedRw_TexDictionaryFindNamedTexture_004c5c00 (LibRw/RwApiBind.cpp,
//          rw::TexDictionary::find). The matched 0x00402750 calls this body on the MASHED_ORIG_TOP path.
// Compiled as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0040bb30 Search40bb30
#define FUN_004c5c00 MatchS02_0040bb30_Call004c5c00
#ifdef MASHED_STANDALONE
extern "C" int MashedRw_TexDictionaryFindNamedTexture_004c5c00(int dict, const char* name);
extern "C" unsigned int* MatchS02_0040bb30_Call004c5c00(int p0, char* p1) {
    return reinterpret_cast<unsigned int*>(static_cast<unsigned>(MashedRw_TexDictionaryFindNamedTexture_004c5c00(p0, p1)));
}
#else
// .asi: the call the original makes, through the original address (it carries the installed JMP into Search4c5c00,
// which is naked with no C signature), as the old body and the sibling Match/MatchS02_0040bb90.cpp do.
extern "C" unsigned int* MatchS02_0040bb30_Call004c5c00(int p0, char* p1) {
    return reinterpret_cast<unsigned int* (__cdecl*)(int, char*)>(0x004c5c00u)(p0, p1);
}
#pragma comment(linker, "/EXPORT:Search40bb30=_Search40bb30")
#endif
extern "C" {
#include "../../../../re/match/s02/0040bb30.c"
}
#ifndef MASHED_STANDALONE
RH_ScopedInstall(Search40bb30, 0x0040bb30);
#endif
