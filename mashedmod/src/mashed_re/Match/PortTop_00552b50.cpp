// HUDINIT-A (L-111 step 1, chain 1 of 0x00427ca0 FontText_HudInit: the Rt2d open 0x00552b60): exe-only port body of
// rva 0x00552b50 = the matched source re/match/s24/00552b50.c (100%, 6 B BYTE-IDENTICAL, relocheck OK 1/1).
// jmp dword ptr [0x00912c58]: the camera render callback 0x00552a60 installs, which calls the one it replaced.
// Reached only from the Rt2d open 0x00552b60 (Match/PortTop_00552b60.cpp), which runs only on the MASHED_ORIG_TOP path
// with MASHED_ORIG_TOP_RWMOD=1 (RwGlobals +0x118 freelist alloc and the +0xf4 string slot are bound only then).
// Not called on this path (installed only for a non-zero camera). New source in HUDINIT-A.
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00552b50 Orig_00552b50
extern "C" {
#include "../../../../re/match/s24/00552b50.c"
}
#endif  // MASHED_STANDALONE
