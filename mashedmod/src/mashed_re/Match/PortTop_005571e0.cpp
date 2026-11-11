// HUDINIT-A (L-111 step 1, chain 1 of 0x00427ca0 FontText_HudInit: the Rt2d open 0x00552b60): exe-only port body of
// rva 0x005571e0 = the matched source re/match/s24/005571e0.c (100%, 49 B BYTE-IDENTICAL, relocheck OK).
// One freelist: 0x100-byte entries, alignment 0x10, static header 0x007dc81c -> DAT_00912a2c. Returns 1.
// Reached only from the Rt2d open 0x00552b60 (Match/PortTop_00552b60.cpp), which runs only on the MASHED_ORIG_TOP path
// with MASHED_ORIG_TOP_RWMOD=1 (RwGlobals +0x118 freelist alloc and the +0xf4 string slot are bound only then).
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_005571e0 Orig_005571e0
#define thunk_FUN_004cc820 Orig_004cc9e0   // matched thunk, Match/PortTop_004cc9e0.cpp
extern "C" {
#include "../../../../re/match/s24/005571e0.c"
}
#endif  // MASHED_STANDALONE
