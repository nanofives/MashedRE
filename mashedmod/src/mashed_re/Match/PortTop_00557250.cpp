// HUDINIT-A (L-111 step 1, chain 1 of 0x00427ca0 FontText_HudInit: the Rt2d open 0x00552b60): exe-only port body of
// rva 0x00557250 = the matched source re/match/s24/00557250.c (100%, 83 B BYTE-IDENTICAL, relocheck OK).
// Two freelists: 8-byte entries into the static header 0x007dc864 -> DAT_00912a34, 0x14-byte entries into 0x007dc840 -> DAT_00912a38. Returns 1.
// Reached only from the Rt2d open 0x00552b60 (Match/PortTop_00552b60.cpp), which runs only on the MASHED_ORIG_TOP path
// with MASHED_ORIG_TOP_RWMOD=1 (RwGlobals +0x118 freelist alloc and the +0xf4 string slot are bound only then).
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00557250 Orig_00557250
#define thunk_FUN_004cc820 Orig_004cc9e0   // matched thunk, Match/PortTop_004cc9e0.cpp
extern "C" {
#include "../../../../re/match/s24/00557250.c"
}
#endif  // MASHED_STANDALONE
