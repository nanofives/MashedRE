// PORT-OT-OTHER (L-111 (a) step 1, subsystem particle): exe-only port body of rva 0x00486350 = the matched source
// re/match/s14/00486350.c (BYTE-IDENTICAL): 0x004768c0 on 0x006fe700 and 0x006fe740. The matched 0x0040bd00
// (Match/PortTop_0040bd00.cpp) calls it on the MASHED_ORIG_TOP path. The two records are the ones the matched 0x004862d0
// (Match/PortTop_004862d0.cpp) builds from 0x0040bbb0 on the same path, so the two are swapped together.
// Callee binds by name, no raw-address call: 0x004768c0 -> Orig_004768c0 (Match/PortTop_004768c0.cpp), through a C++
// overload taking the pointer (the source declares an int parameter and passes the address).
// No other body of this RVA in the source tree.
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
extern "C" void Orig_004768c0(int p0);
static inline void PortTop_00486350_Call004768c0(void* p0) {
    Orig_004768c0(static_cast<int>(reinterpret_cast<unsigned long>(p0)));
}
#define FUN_00486350 Orig_00486350
#define FUN_004768c0 PortTop_00486350_Call004768c0
extern "C" {
#include "../../../../re/match/s14/00486350.c"
}
#endif  // MASHED_STANDALONE
