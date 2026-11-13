// HUDINIT-B (L-111 step 1, chain 2 of 0x00427ca0 FontText_HudInit: brushes + the 0x00427ca0 body): exe-only port body of
// rva 0x00556cc0 = the matched source re/match/s24/00556cc0.c (100%, 10 B BYTE-IDENTICAL, relocheck OK 1/1).
// DAT_00912a20 = param_1 (the font dictionary 0x00427ca0 loads; the matched 0x00427620 destroys it at AppDestroy).
// Reached only from the matched 0x00427ca0 (Match/MatchS06_00427ca0.cpp), which the exe runs only on the MASHED_ORIG_TOP
// path with MASHED_ORIG_TOP_RWMOD=1 (selector PortTop_00402750_Call00427ca0, Match/PortTop_00552b60.cpp).
// The exe also has the hand body SetDat00912a20 (Frontend/BatchAA_s3.cpp, both targets). This matched copy runs on
// the knob path instead, so no hand body runs there (L0 ruling, 2026-11-10).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00556cc0 Orig_00556cc0
extern "C" {
#include "../../../../re/match/s24/00556cc0.c"
}
#endif  // MASHED_STANDALONE
