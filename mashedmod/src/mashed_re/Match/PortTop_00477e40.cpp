// PORT-OT-RENDER (L-111 (a) step 1): exe-only port body of rva 0x00477e40 = the matched source re/match/s12/00477e40.c
// (MATCH-S12, BYTE-IDENTICAL): DAT_00693190 = 0x0040bb30(&DAT_005ceb10), then 0x00477b40. The matched 0x0040bbb0
// (Match/PortTop_0040bbb0.cpp) calls it on the MASHED_ORIG_TOP path. 0x005ceb10 is seeded (OT_MEASURE.tsv seed OK).
// Callees bind by name, no raw-address call:
//  - 0x0040bb30 -> Search40bb30 (Match/MatchS02_0040bb30.cpp), through a forwarder: the source declares an undefined4
//    parameter and passes &DAT_005ceb10 (a C conversion C++ refuses), so the name binds to a C++ overload taking the
//    pointer (the Match/PortTop_00413bb0.cpp pattern).
//  - 0x00477b40 -> Orig_00477b40 (Match/PortTop_00477b40.cpp, exe-only).
// Shared state: no other exe TU reads or writes 0x00693190.
// No other body of this RVA in the source tree.
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
extern "C" int Search40bb30(const char* name);
static inline unsigned int PortTop_00477e40_Call0040bb30(void* p0) {
    return static_cast<unsigned int>(Search40bb30(static_cast<const char*>(p0)));
}
#define FUN_00477e40 Orig_00477e40
#define FUN_0040bb30 PortTop_00477e40_Call0040bb30
#define FUN_00477b40 Orig_00477b40                              // matched, Match/PortTop_00477b40.cpp
extern "C" {
#include "../../../../re/match/s12/00477e40.c"
}
#endif  // MASHED_STANDALONE
