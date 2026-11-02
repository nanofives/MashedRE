// MATCH-S05: port body of 0x004235b0 = the byte-matched source re/match/s05/004235b0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Render/LowRvaSetters_o2.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).


#define MATCH_PORT 1
#define FUN_004235b0 AiPizLoad
#define FUN_00423480 AiFilenameBuild
#define FUN_00495280 MatchS05_004235b0_Call00495280
#define FUN_004952f0 MatchS05_004235b0_Call004952f0
#define FUN_004cbd30 MatchS05_004235b0_Call004cbd30
#define FUN_004cc160 MatchS05_004235b0_Call004cc160
#define FUN_004cc230 MatchS05_004235b0_Call004cc230
#define FUN_004cc5e0 MatchS05_004235b0_Call004cc5e0
// Callees of the matched source: the old Render/LowRvaSetters_o2.cpp body called these six through their original addresses (__cdecl
// function-pointer casts), so each forwarder makes that same call. 0x00423480 binds to AiFilenameBuild.
extern "C" void MatchS05_004235b0_Call00495280(const char * p0) {
    reinterpret_cast<void (__cdecl*)(const char *)>(0x00495280u)(p0);
}
extern "C" void MatchS05_004235b0_Call004952f0(void) {
    reinterpret_cast<void (__cdecl*)()>(0x004952f0u)();
}
extern "C" unsigned int MatchS05_004235b0_Call004cbd30(int p0, void * p1, unsigned int p2) {
    return reinterpret_cast<unsigned int (__cdecl*)(int, void *, unsigned int)>(0x004cbd30u)(p0, p1, p2);
}
extern "C" bool MatchS05_004235b0_Call004cc160(int p0, int p1) {
    return reinterpret_cast<bool (__cdecl*)(int, int)>(0x004cc160u)(p0, p1);
}
extern "C" int MatchS05_004235b0_Call004cc230(unsigned int p0, int p1, void * p2) {
    return reinterpret_cast<int (__cdecl*)(unsigned int, int, void *)>(0x004cc230u)(p0, p1, p2);
}
extern "C" unsigned int MatchS05_004235b0_Call004cc5e0(int p0, int p1, unsigned int * p2, void * p3) {
    return reinterpret_cast<unsigned int (__cdecl*)(int, int, unsigned int *, void *)>(0x004cc5e0u)(p0, p1, p2, p3);
}
#pragma comment(linker, "/EXPORT:AiPizLoad=_AiPizLoad")
extern "C" {
#include "../../../../re/match/s05/004235b0.c"
}
