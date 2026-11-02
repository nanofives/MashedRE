// MATCH-S06: port body of 0x00429290 = the byte-matched source re/match/s06/00429290.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (new body, MATCH top-down rule L-72; no install).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
// Also the only body of 0x00429240 (FUN_00429240): the file holds it (canonical, MATCH_STATIC), no install.
// Callees:
//   0x00428a30 -> FUN_00428a30 (symmap new-body-batch2 sw5)
//   0x00428bf0 -> TitleBuildInfoRender (symmap swap-batch2 sw5)
//   0x00428d30 -> FUN_00428d30 (symmap new-body-batch2 sw5)
//   0x0045d3f0 -> L2_FUN_0045d3f0 in the .asi (Lane2/L2_0045d3f0.cpp, .asi-only TU); the exe makes the raw-address call
//   0x0045d430 -> raw-address forwarder: no port body
//   0x00466b50 -> raw-address forwarder: no port body
//   0x004967e0 -> raw-address forwarder: no port body (IntroSplash.cpp OrigInputPipeline is a raw call-through)

#define MATCH_PORT 1
#define FUN_00429290 FUN_00429290
#define FUN_00429240 FUN_00429240
#define FUN_00428a30 FUN_00428a30
#define FUN_00428bf0 TitleBuildInfoRender
#define FUN_00428d30 FUN_00428d30
#ifdef MASHED_STANDALONE
#define FUN_0045d3f0 MatchS06_00429290_Call0045d3f0
#else
#define FUN_0045d3f0 L2_FUN_0045d3f0
#endif
#define FUN_0045d430 MatchS06_00429290_Call0045d430
#define FUN_00466b50 MatchS06_00429290_Call00466b50
#define FUN_004967e0 MatchS06_00429290_Call004967e0
#pragma comment(linker, "/EXPORT:FUN_00429290=_FUN_00429290")
extern "C" {
#include "../../../../re/match/s06/00429290.c"
}

// Raw-address forwarders (call-throughs into the original at the fixed address, not bodies of the callees).
extern "C" {
void MatchS06_00429290_Call0045d430(void) {
    reinterpret_cast<void (__cdecl*)()>(0x0045d430u)();
}
void MatchS06_00429290_Call00466b50(int p0) {
    reinterpret_cast<void (__cdecl*)(int)>(0x00466b50u)(p0);
}
void MatchS06_00429290_Call004967e0(void) {
    reinterpret_cast<void (__cdecl*)()>(0x004967e0u)();
}
#ifdef MASHED_STANDALONE
void MatchS06_00429290_Call0045d3f0(int p0) {
    reinterpret_cast<void (__cdecl*)(int)>(0x0045d3f0u)(p0);
}
#endif
}
