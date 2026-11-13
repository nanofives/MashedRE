// RVA-LINT: ORIGTOP-ONLY
// BIND-CRT-498510 (roadmap run 43, top-down row): rva 0x00495830 = the byte-matched source re/match/s16/00495830.c (MATCH-S16, 100%),
// compiled verbatim for the MASHED_ORIG_TOP knob path as a callee of 0x00498510 (Match/PortTop_00498510.cpp). EXE ONLY.
// Its asi body is Match/MatchS16_00495830.cpp (ORIGTOP-ONLY CROSS-TARGET pair). Globals are the original addresses.
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00495830 Orig_00495830
extern "C" {
#include "../../../../re/match/s16/00495830.c"
}  // extern "C"
#endif  // MASHED_STANDALONE
