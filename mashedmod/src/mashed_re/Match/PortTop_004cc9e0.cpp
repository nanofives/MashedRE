// HUDINIT-A (L-111 step 1, chain 1 of 0x00427ca0 FontText_HudInit: the Rt2d open 0x00552b60): exe-only port body of
// rva 0x004cc9e0 = the matched source re/match/rwl01/004cc9e0.c (100%, 5 B BYTE-IDENTICAL, relocheck OK).
// jmp to 0x004cc820 (RwFreeListCreate) with the same six arguments.
// Reached only from the Rt2d open 0x00552b60 (Match/PortTop_00552b60.cpp), which runs only on the MASHED_ORIG_TOP path
// with MASHED_ORIG_TOP_RWMOD=1 (RwGlobals +0x118 freelist alloc and the +0xf4 string slot are bound only then).
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define thunk_FUN_004cc820 Orig_004cc9e0
#define FUN_004cc820 PortTop_RwFreeListCreate_004cc820   // port body, Match/PortTop_004cc820.cpp
extern "C" {
#include "../../../../re/match/rwl01/004cc9e0.c"
}
#endif  // MASHED_STANDALONE
