// MATCH-S06: port body of 0x00429660 = the byte-matched source re/match/s06/00429660.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (new body, MATCH top-down rule L-72; no install).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
// Callees:
//   0x00427e00 -> FUN_00427e00 (symmap new-body-batch2 sw4)
//   0x00429620 -> FUN_00429620 (symmap new-body-batch2 sw5)
//   0x0042f500 -> GetDat0067ea64 (GameState/StateAccessors.cpp)
//   0x0040dc80 -> UtilFloat63b910Get in the .asi (Util/PromoLoop_round9.cpp, .asi-only TU); the exe makes the raw-address call
//   0x0040dc90 -> UtilSlotIndexCondGet in the .asi (Util/PromoLoop_round9.cpp, .asi-only TU); the exe makes the raw-address call
//   0x0042c7c0 -> raw-address forwarder: no port body

#define MATCH_PORT 1
#define FUN_00429660 FUN_00429660
#define FUN_00427e00 FUN_00427e00
#define FUN_00429620 FUN_00429620
#define FUN_0042f500 GetDat0067ea64
#ifdef MASHED_STANDALONE
#define FUN_0040dc80 MatchS06_00429660_Call0040dc80
#else
#define FUN_0040dc80 UtilFloat63b910Get
#endif
#ifdef MASHED_STANDALONE
#define FUN_0040dc90 MatchS06_00429660_Call0040dc90
#else
#define FUN_0040dc90 UtilSlotIndexCondGet
#endif
#define FUN_0042c7c0 MatchS06_00429660_Call0042c7c0
#pragma comment(linker, "/EXPORT:FUN_00429660=_FUN_00429660")
extern "C" {
#include "../../../../re/match/s06/00429660.c"
}

// Raw-address forwarders (call-throughs into the original at the fixed address, not bodies of the callees).
extern "C" {
void MatchS06_00429660_Call0042c7c0(int p0) {
    reinterpret_cast<void (__cdecl*)(int)>(0x0042c7c0u)(p0);
}
#ifdef MASHED_STANDALONE
float MatchS06_00429660_Call0040dc80(void) {
    return reinterpret_cast<float (__cdecl*)()>(0x0040dc80u)();
}
int MatchS06_00429660_Call0040dc90(void) {
    return reinterpret_cast<int (__cdecl*)()>(0x0040dc90u)();
}
#endif
}
