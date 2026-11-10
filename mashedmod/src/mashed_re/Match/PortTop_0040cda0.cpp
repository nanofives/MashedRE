// ATOMIC-VIEW (L-111 step 1): exe-only port body of rva 0x0040cda0 = the matched source re/match/s02/0040cda0.c
// (BYTE-IDENTICAL 86 B, relocheck OK): when DAT_00636ad0 == 0, args = {coef, texture (param or 0x0040bb30("shine")),
// 0x004672d0(0)} and RpClumpForAllAtomics(clump, 0x0040bb10, &args). Caller on the MASHED_ORIG_TOP path: the matched
// init 0x004576f0 (type-table entry 7), through PortTop_PWI_Call0040cda0 (Match/PortTop_004576f0.cpp).
// Callees bind by name, no raw-address call:
//  - RpClumpForAllAtomics 0x004e66d0 = MashedRw_ClumpForAllAtomicsAsViews_004e66d0 (LibRw/RwApiBind.cpp): the callback
//    chain reads the RW 3.x atomic layout (0x0040bae0: geometry at +0x18), so each librw atomic goes over as its
//    RW-layout view (NewAtomicView, the 0x004b3f90 view).
//  - 0x0040bb10 = Orig_0040bb10 (Match/PortTop_0040bb10.cpp), 0x0040bb30 = Search40bb30 (Match/MatchS02_0040bb30.cpp),
//    0x004672d0 = Orig_004672d0 (Match/PortTop_004672d0.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_0040cda0 Orig_0040cda0
#define RpClumpForAllAtomics MashedRw_ClumpForAllAtomicsAsViews_004e66d0
#define FUN_0040bb10 Orig_0040bb10
#define FUN_0040bb30 Search40bb30
#define FUN_004672d0 Orig_004672d0
extern "C" {
#include "../../../../re/match/s02/0040cda0.c"
}  // extern "C"
#endif  // MASHED_STANDALONE
