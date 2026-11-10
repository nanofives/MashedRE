// PORT-OT-PICKUPPOOL (L-111 (a) step 1): exe-only port body of rva 0x004c5770 = the matched source re/match/s19/004c5770.c
// (MATCH-S19 raw g2s): calls RwGlobals +0x11c (freelist free) with *(RwGlobals + [0x007d4028]) and param_1, returns 1.
// Its only caller in the exe is the matched 0x004593b0 (Match/PortTop_004593b0.cpp), which runs only with
// MASHED_ORIG_TOP_RWMOD=1: +0x11c is null without it, and with it the slot is RwGlobalsFreeListFree (LibRw/RwApiBind.cpp,
// PORT-OT-RWG-MODULES), which returns 0 and frees nothing for an entry the freelist did not hand out.
// No other body of this RVA exists in either target.
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004c5770 Orig_004c5770
extern "C" {
#include "../../../../re/match/s19/004c5770.c"
}
#endif  // MASHED_STANDALONE
