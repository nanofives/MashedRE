// PORT-OT-HUD (L-111 (a) step 1): exe-only port body of rva 0x00413bb0 = the matched source re/match/s03/raw_00413bb0.c
// (BYTE-IDENTICAL 12 B): FUN_004768c0(&DAT_0063bd50), the teardown of the record 0x00413b80 builds. The matched 0x0040bd00
// (Match/PortTop_0040bd00.cpp) calls it on the MASHED_ORIG_TOP path (AppDestroy 0x00402a40 -> 0x0040bd00), before the four
// texture dictionaries are destroyed. Callee 0x004768c0 = Orig_004768c0 (Match/PortTop_004768c0.cpp).
// The raw source passes &DAT_0063bd50 (undefined1*) to an int parameter (a C conversion C++ refuses), so the callee name
// binds to a C++ overload taking the pointer. The other body of this RVA is the .asi-only Render/PromoLoop_round256.cpp
// Fwd4768c0_63bd50, so this TU is the ORIGTOP-ONLY class (CROSS-TARGET).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
extern "C" void Orig_004768c0(int p0);
static inline void PortTop_Call004768c0_00413bb0(void* p0) {
    Orig_004768c0(static_cast<int>(reinterpret_cast<unsigned long>(p0)));
}
#define FUN_00413bb0 Orig_00413bb0
#define FUN_004768c0 PortTop_Call004768c0_00413bb0
extern "C" {
#include "../../../../re/match/s03/raw_00413bb0.c"
}
#endif  // MASHED_STANDALONE
