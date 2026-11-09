// PORT-TOP-04I (PTank family): exe-only port body of rva 0x004e7e30 (RpAtomicSetFrame) = the matched source
// re/match/rwl02/004e7e30.c: _rwObjectHasFrameSetFrame(atomic, frame) (0x004c0740), then atomic privateFlags (+0x03)
// |= 1; returns the atomic. Callers on the MASHED_ORIG_TOP path: 0x004881d0 / 0x004880a0 (Match/MatchS14_004881d0.cpp,
// Match/MatchS14_004880a0.cpp) on the RW-layout PTank records. 0x004c0740 binds to LibRw/RwApiBind.cpp, which stores
// the frame in the record.
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004e7e30 Orig_004e7e30
#define FUN_004c0740 MashedRw_ObjectHasFrameSetFrame_004c0740
extern "C" {
#include "../../../../re/match/rwl02/004e7e30.c"
}
#endif  // MASHED_STANDALONE
