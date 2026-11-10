// PORT-OT-OTHER (L-111 (a) step 1, subsystem particle): exe-only port body of rva 0x0048eac0 = the matched source
// re/match/s15/0048eac0.c (BYTE-IDENTICAL): nine 0x40-byte records built by 0x004770c0: 0x007706d8 (0x27, 10, "animFire"),
// 0x00766f00 (0x27, 0x50, "exp_cloud"), 0x0076a0b8 (0x27, 10, "exp_flash"), 0x007668f8 (0x27, 100), 0x00766978 (0x27, 0x28)
// and 0x007669b8 (0x27, 0x100) ("exp_cloud"), 0x00766938 (0x807, 0x100, "smoke"), 0x0076d900 (0x817, 0x50, "smoke"),
// 0x0076d940 (0x817, 0x50, "fireball"). The matched 0x0040bbb0 (Match/PortTop_0040bbb0.cpp) calls it on the MASHED_ORIG_TOP
// path. Its teardown 0x0048fdd0 (Match/PortTop_0048fdd0.cpp, from the matched 0x0040bd00) runs 0x004768c0 on the nine.
// Callees bind by name, no raw-address call, through C++ overloads taking pointers (the source declares undefined4 / uint*
// parameters and passes string literals and undefined1 addresses): 0x0040bb30 -> Search40bb30 (Match/MatchS02_0040bb30.cpp),
// 0x004770c0 -> Orig_004770c0 (Match/PortTop_004770c0.cpp; flag 0x800 calls 0x00476c10 Batch476c10).
// Shared state: no exe TU reads or writes the nine records (the .asi-only Lane2/L2_0048fce0.cpp, Lane2/L2_0048fd10.cpp,
// Lane2/L2_0048fd40.cpp and Util/PromoLoop_round53.cpp are the only other sources that name them).
// No other body of this RVA in the source tree.
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
extern "C" int Search40bb30(const char* name);
extern "C" void Orig_004770c0(unsigned int* p0, unsigned int p1, unsigned int p2, unsigned int p3);
static inline unsigned int PortTop_0048eac0_Call0040bb30(const char* p0) {
    return static_cast<unsigned int>(Search40bb30(p0));
}
static inline void PortTop_0048eac0_Call004770c0(void* p0, unsigned int p1, unsigned int p2, unsigned int p3) {
    Orig_004770c0(static_cast<unsigned int*>(p0), p1, p2, p3);
}
#define FUN_0048eac0 Orig_0048eac0
#define FUN_0040bb30 PortTop_0048eac0_Call0040bb30
#define FUN_004770c0 PortTop_0048eac0_Call004770c0
extern "C" {
#include "../../../../re/match/s15/0048eac0.c"
}
#endif  // MASHED_STANDALONE
