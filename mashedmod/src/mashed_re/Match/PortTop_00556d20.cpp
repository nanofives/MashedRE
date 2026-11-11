// HUDINIT-A (L-111 step 1, chain 1 of 0x00427ca0 FontText_HudInit: the Rt2d open 0x00552b60): exe-only port body of
// rva 0x00556d20 = the matched source re/match/rwl05/00556d20.c (100%, 73 B BYTE-IDENTICAL, relocheck OK).
// RwGlobals +0x108 malloc(0x2400, 0x40190) -> DAT_00912a04, then a freelist of 0x78-byte entries, alignment 0x10, static header
// 0x007dc7e8 -> DAT_00912a00 (the list 0x00556d70 allocates from). Returns 1.
// Reached only from the Rt2d open 0x00552b60 (Match/PortTop_00552b60.cpp), which runs only on the MASHED_ORIG_TOP path
// with MASHED_ORIG_TOP_RWMOD=1 (RwGlobals +0x118 freelist alloc and the +0xf4 string slot are bound only then).
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00556d20 Orig_00556d20
#define thunk_FUN_004cc820 Orig_004cc9e0   // matched thunk, Match/PortTop_004cc9e0.cpp
extern "C" {
#include "../../../../re/match/rwl05/00556d20.c"
}
#endif  // MASHED_STANDALONE
