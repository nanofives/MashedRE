// HUDINIT-A (L-111 step 1, chain 1 of 0x00427ca0 FontText_HudInit: the Rt2d open 0x00552b60): exe-only port body of
// rva 0x00552750 = the matched source re/match/s24/00552750.c (100%, 238 B BYTE-IDENTICAL, relocheck OK).
// Sets the matrix at (&DAT_00912a84)[DAT_00912b04] to identity (1.0 at +0x0 / +0x14 / +0x28, 0 elsewhere, flags +0xc |
// 0x20003), DAT_00912bd8 = DAT_00912bec = 0. Returns 1.
// Reached only from the Rt2d open 0x00552b60 (Match/PortTop_00552b60.cpp), which runs only on the MASHED_ORIG_TOP path
// with MASHED_ORIG_TOP_RWMOD=1 (RwGlobals +0x118 freelist alloc and the +0xf4 string slot are bound only then).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00552750 Orig_00552750
extern "C" {
#include "../../../../re/match/s24/00552750.c"
}
#endif  // MASHED_STANDALONE
