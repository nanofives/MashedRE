// HUDINIT-A (L-111 step 1, chain 1 of 0x00427ca0 FontText_HudInit: the Rt2d open 0x00552b60): exe-only port body of
// rva 0x00553f40 = the matched source re/match/rwl04/00553f40.c (100%, 205 B BYTE-IDENTICAL, relocheck OK).
// For a non-zero node whose +0x4 is not -1: a list from 0x005c4ad0(0x18, 0x301a1); when +0x4 is non-zero, 0x005c4c60 and a copy
// of +0x4 * 0x18 bytes, then RwGlobals +0x10c on the old +0x0; +0x4 = -1, +0x0 = the list, then the same for the node at +0x14.
// Reached only from the Rt2d open 0x00552b60 (Match/PortTop_00552b60.cpp), which runs only on the MASHED_ORIG_TOP path
// with MASHED_ORIG_TOP_RWMOD=1 (RwGlobals +0x118 freelist alloc and the +0xf4 string slot are bound only then).
// On this path both calls come from 0x005540d0 with a node whose +0x4 = 0 and +0x14 = 0. With 0x005c4ad0 a stand-in (returns
// 0) each call takes the `return 0` branch and leaves the node unchanged; with the matched 0x005c4ad0 the first would make
// one list, set +0x4 = -1, and the second would return the node. The +0x4 == -1 reads inside the outer test (0x005c4d30 / 0x0055deb0) cannot run: the outer test needs
// +0x4 != -1.
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
// FUN_00553f40 also names the recursion on +0x14.
#define FUN_00553f40 Orig_00553f40
#define FUN_005c4ad0 PortTop_StandInHIA_005c4ad0   // stand-in, Match/PortTop_StandInsHIA.cpp (96.7% source, L-115 class not yet ruled)
#define FUN_005c4c60 PortTop_StandInHIA_005c4c60   // stand-in, Match/PortTop_StandInsHIA.cpp: runs only for a non-zero +0x4
#define FUN_005c4d30 Orig_005c4d30   // matched, Match/PortTop_005c4d30.cpp (PORT-OT-HUD-2)
#define FUN_0055deb0 PortTop_StandInHIA_0055deb0   // stand-in, Match/PortTop_StandInsHIA.cpp: runs only for +0x4 == -1
extern "C" {
#include "../../../../re/match/rwl04/00553f40.c"
}
#endif  // MASHED_STANDALONE
