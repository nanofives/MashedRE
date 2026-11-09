// PORT-TOP-04J (family 1, cup world / lights): exe-only port body of rva 0x004e48e0 (RpLightSetRadius: +0x14 = radius, then RwFrameUpdateObjects on the frame
// at +0x04 when nonzero) = the matched source re/match/rwl02/004e48e0.c (match.py BYTE-IDENTICAL 32 B). It writes the
// RW-layout light record of LibRw/RwApiBind.cpp (MashedRwLight). Callee: RwFrameUpdateObjects 0x004c0e50 (bound,
// rw::Frame::updateObjects). Caller on the MASHED_ORIG_TOP path: 0x00401000.
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004e48e0 Orig_004e48e0
#define FUN_004c0e50 MashedRw_FrameUpdateObjects_004c0e50
extern "C" {
#include "../../../../re/match/rwl02/004e48e0.c"
}
#endif  // MASHED_STANDALONE
