// POWERUPINIT-A (L-111 step 1): exe-only port body of rva 0x00412010 = the matched source re/match/s02/00412010.c
// (MATCH-S02, 100%): RpClumpDestroy(*(DAT_0063bb38[i] + 0x3c)) for the six 0x2c-byte records at 0x0063bb38, then
// RpClumpDestroy(DAT_0063bc40) and DAT_0063bc40 = 0 when it is non-zero. Its only caller in the exe is the matched
// 0x0045b930 (Match/PortTop_0045b930.cpp, AppDestroy 0x00402a40 on the MASHED_ORIG_TOP path), through the call-site
// forwarder PortTop_0045b930_Call00412010, which runs it only when all six records are non-zero.
// The records and DAT_0063bc40 are built by the matched 0x00411f30 (Match/PortTop_00411f30.cpp) from 0x0045bae0:
// DAT_0063bb38[i] = 0x004b3f90(clump i), an RW-layout atomic view whose +0x3c is RpAtomic.clump, the clump i itself
// (LibRw/RwApiBind.cpp), and DAT_0063bc40 = 0x004781b0("LaserTower.dff", 0).
// Callees bind by name: RpClumpDestroy 0x004e6e00 = MashedRw_ClumpDestroy_004e6e00 (LibRw/RwApiBind.cpp,
// rw::Clump::destroy, null skipped).
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00412010 Orig_00412010
#define RpClumpDestroy MashedRw_ClumpDestroy_004e6e00
extern "C" {
#include "../../../../re/match/s02/00412010.c"
}
#endif  // MASHED_STANDALONE
