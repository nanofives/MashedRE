// PORT-TOP-04I (family 5): exe-only port body of rva 0x00489250 = the matched source re/match/s14/00489250.c
// (match.py BYTE-IDENTICAL 56 B): RwFrameDestroy(+0x04) and RpAtomicDestroy for DAT_007030a8, then DAT_007030ac, the two
// PTank atomics 0x004881d0 / 0x004880a0 create. Reached only on the MASHED_ORIG_TOP path, from the matched AppDestroy
// 0x00402a40 (Match/PortTop_00402a40.cpp), which calls it before the dictionary teardown 0x0040bd00. Callees are RW
// bindings (LibRw/RwApiBind.cpp): RwFrameDestroy 0x004c0c20 -> rw::Frame::destroy, RpAtomicDestroy 0x004e6920 -> the
// PTank record's release.
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00489250 Orig_00489250
#define FUN_004c0c20 MashedRw_FrameDestroy_004c0c20
#define FUN_004e6920 MashedRw_AtomicDestroy_004e6920
extern "C" {
#include "../../../../re/match/s14/00489250.c"
}
#endif  // MASHED_STANDALONE
