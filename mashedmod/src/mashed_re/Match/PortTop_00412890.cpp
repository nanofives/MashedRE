// PORT-OT-BOOT (L-111 (a) step 1): exe-only port body of rva 0x00412890 = the matched source re/match/s03/h2_00412890.c
// (BYTE-IDENTICAL 86 B): the 4096 six-short quad index entries at 0x0089d4a0, then DAT_0063bc54 =
// FUN_0040bb30("headlight"). The matched AppInitialiseOnBootup 0x00402750 (Match/PortTop_00402750.cpp) calls it on the
// MASHED_ORIG_TOP path, after 0x0040bbb0 loaded the FX dictionary 0x0063b8f8 that 0x0040bb30 searches.
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00412890 Orig_00412890
#define FUN_0040bb30 Search40bb30                           // matched, Match/MatchS02_0040bb30.cpp (both targets)
#define DAT_0089d4a0 ((Idx6*)0x0089d4a0u)
#define DAT_0063bc54 (*(int*)0x0063bc54u)
extern "C" {
int Search40bb30(const char* name);
#include "../../../../re/match/s03/h2_00412890.c"
}
#endif  // MASHED_STANDALONE
