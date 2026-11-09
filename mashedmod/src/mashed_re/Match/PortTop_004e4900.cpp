// PORT-TOP-04J (family 1, cup world / lights): exe-only port body of rva 0x004e4900 (RpLightSetColor: colour at +0x18..+0x24, privateFlags +0x03 = 1 when
// red == green == blue, else 0) = the matched source re/match/rwl02/004e4900.c (match.py BYTE-IDENTICAL 77 B). It writes the
// RW-layout light record of LibRw/RwApiBind.cpp (MashedRwLight); RpWorldAddLight's binding copies the fields to librw.
// Caller on the MASHED_ORIG_TOP path: 0x00401000 (Match/PortTop_00401000.cpp).
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004e4900 Orig_004e4900
extern "C" {
#include "../../../../re/match/rwl02/004e4900.c"
}
#endif  // MASHED_STANDALONE
