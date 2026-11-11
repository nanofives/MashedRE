// HUDINIT-B (L-111 step 1, chain 2 of 0x00427ca0 FontText_HudInit: brushes + the 0x00427ca0 body): exe-only port body of
// rva 0x00557110 = the matched source re/match/s24/00557110.c (100%, 169 B BYTE-IDENTICAL, relocheck OK 4/4).
// Brush UVs: p0 = *param_5, d0 = param_4 - param_5, p1 = *param_2, d1 = param_3 - param_2 (V2d at +0x10 / +0x28 /
// +0x40 / +0x58); +0x64 bit 1 set unless all four deltas are 0. Returns param_1. No callees.
// Reached only from the matched 0x00427ca0 (Match/MatchS06_00427ca0.cpp), which the exe runs only on the MASHED_ORIG_TOP
// path with MASHED_ORIG_TOP_RWMOD=1 (selector PortTop_00402750_Call00427ca0, Match/PortTop_00552b60.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00557110 Orig_00557110
extern "C" {
#include "../../../../re/match/s24/00557110.c"
}
#endif  // MASHED_STANDALONE
