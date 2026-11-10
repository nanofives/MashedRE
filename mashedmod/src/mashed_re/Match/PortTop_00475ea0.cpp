// PORT-OT-RENDER (L-111 (a) step 1): exe-only port body of rva 0x00475ea0 = the matched source re/match/s12/00475ea0.c
// (MATCH-S12, BYTE-IDENTICAL): 0x004768c0 on the two 0x40-byte records at 0x00691618, the two at 0x00692418, then on
// 0x00692498. The matched 0x0040bd00 (Match/PortTop_0040bd00.cpp) calls it on the MASHED_ORIG_TOP path (AppDestroy
// 0x00402a40 -> 0x0040bd00). The five records are the ones the matched 0x00476390 (Match/PortTop_00476390.cpp) builds from
// 0x0040bbb0 on the same path. 0x004768c0 has no null check on record +4, so this body and 0x00476390 are swapped together.
// Callee binds by name, no raw-address call: 0x004768c0 -> Orig_004768c0 (Match/PortTop_004768c0.cpp), through a forwarder
// with the source's prototype.
// No other body of this RVA in the source tree.
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00475ea0 Orig_00475ea0
#define FUN_004768c0 PortTop_00475ea0_Call004768c0
extern "C" {
void Orig_004768c0(int p0);
#include "../../../../re/match/s12/00475ea0.c"
void PortTop_00475ea0_Call004768c0(undefined* p0) { Orig_004768c0(static_cast<int>(reinterpret_cast<unsigned long>(p0))); }
}
#endif  // MASHED_STANDALONE
