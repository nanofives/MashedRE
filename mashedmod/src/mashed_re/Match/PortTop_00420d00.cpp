// PORT-OT-BOOT (L-111 (a) step 1): exe-only port body of rva 0x00420d00 PanelSortInit = the matched source
// re/match/s05/00420d00.c (MATCH-S05): FUN_004b6520(&DAT_0063e4b8, 0x90), then 0 / 1 / 2 / 3 at 0x0063e4d0 / 0x0063e4f4 /
// 0x0063e518 / 0x0063e53c. The matched AppInitialiseOnBootup 0x00402750 (Match/PortTop_00402750.cpp) calls it on the
// MASHED_ORIG_TOP path. The other body of this RVA is the .asi-only hand transcription Boot/PanelSortInit.cpp.
// The raw source passes a pointer to an undefined4 parameter (a C conversion C++ refuses), so the callee name binds to
// a C++ overload taking the pointer, which forwards to the matched 0x004b6520 (Match/PortTop_004b6520.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
extern "C" void Orig_004b6520(unsigned int dst, unsigned int size);
static inline void PortTop_Call004b6520_00420d00(void* dst, unsigned int size) {
    Orig_004b6520(static_cast<unsigned int>(reinterpret_cast<unsigned long>(dst)), size);
}
#define FUN_00420d00 Orig_00420d00
#define FUN_004b6520 PortTop_Call004b6520_00420d00
extern "C" {
#include "../../../../re/match/s05/00420d00.c"
}
#endif  // MASHED_STANDALONE
