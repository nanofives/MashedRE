// PORT-OT-OTHER (L-111 (a) step 1, subsystem particle): exe-only port body of rva 0x0048fdd0 = the matched source
// re/match/s15/0048fdd0.c (BYTE-IDENTICAL): 0x004768c0 on the nine records 0x0076d900, 0x00766f00, 0x007706d8, 0x0076d940,
// 0x007668f8, 0x00766978, 0x00766938, 0x0076a0b8, 0x007669b8. The matched 0x0040bd00 (Match/PortTop_0040bd00.cpp) calls it on
// the MASHED_ORIG_TOP path. The nine records are the ones the matched 0x0048eac0 (Match/PortTop_0048eac0.cpp) builds from
// 0x0040bbb0 on the same path, so the two are swapped together.
// Callee binds by name, no raw-address call: 0x004768c0 -> Orig_004768c0 (Match/PortTop_004768c0.cpp), through a C++
// overload taking the pointer.
// No other body of this RVA in the source tree.
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
extern "C" void Orig_004768c0(int p0);
static inline void PortTop_0048fdd0_Call004768c0(void* p0) {
    Orig_004768c0(static_cast<int>(reinterpret_cast<unsigned long>(p0)));
}
#define FUN_0048fdd0 Orig_0048fdd0
#define FUN_004768c0 PortTop_0048fdd0_Call004768c0
extern "C" {
#include "../../../../re/match/s15/0048fdd0.c"
}
#endif  // MASHED_STANDALONE
