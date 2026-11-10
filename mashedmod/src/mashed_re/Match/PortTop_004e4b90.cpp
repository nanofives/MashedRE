// PORT-TOP-04J (family 1, cup world / lights): exe-only port body of rva 0x004e4b90 (RpLightSetConeAngle: when DAT_005d757c <= angle <= DAT_005cd108,
// +0x28 = -cos(angle) and returns the light, else returns 0) = the matched source re/match/rwl02/004e4b90.c (match.py
// BYTE-IDENTICAL 55 B). DAT_005d757c (0.0) and DAT_005cd108 (1.5707964) are .rdata floats: the knob path's rdata seed
// (Match/PortTop_RdataSeed.inc, gen_rdata_seed.py) writes them back under MASHED_ORIG_TOP. It writes the RW-layout light
// record of LibRw/RwApiBind.cpp (MashedRwLight). Caller on the MASHED_ORIG_TOP path: 0x00401000.
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define RpLightSetConeAngle Orig_004e4b90
extern "C" {
#include "../../../../re/match/rwl02/004e4b90.c"
}
#endif  // MASHED_STANDALONE
