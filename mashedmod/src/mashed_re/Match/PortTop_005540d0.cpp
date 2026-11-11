// HUDINIT-A (L-111 step 1, chain 1 of 0x00427ca0 FontText_HudInit: the Rt2d open 0x00552b60): exe-only port body of
// rva 0x005540d0 = the matched source re/match/rwl04/005540d0.c (100%, 122 B BYTE-IDENTICAL, relocheck OK).
// A freelist of 0x1c-byte entries (static header 0x007dc77c) -> DAT_00912a24, one entry from it through RwGlobals +0x118 (hint
// 0x301a1) set to {0, 0, 0, 1, 0, 0, self} and passed to 0x00553f40, DAT_00912a28 = the entry, 0x00553f40 again. Returns 1.
// Reached only from the Rt2d open 0x00552b60 (Match/PortTop_00552b60.cpp), which runs only on the MASHED_ORIG_TOP path
// with MASHED_ORIG_TOP_RWMOD=1 (RwGlobals +0x118 freelist alloc and the +0xf4 string slot are bound only then).
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_005540d0 Orig_005540d0
#define thunk_FUN_004cc820 Orig_004cc9e0   // matched thunk, Match/PortTop_004cc9e0.cpp
#define FUN_00553f40 Orig_00553f40   // matched, Match/PortTop_00553f40.cpp
extern "C" {
#include "../../../../re/match/rwl04/005540d0.c"
}
#endif  // MASHED_STANDALONE
