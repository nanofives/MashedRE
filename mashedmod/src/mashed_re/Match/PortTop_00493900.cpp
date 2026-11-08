// PORT-TOP-02: 0x00493900 command-line switch parser (re/match/s15/00493900.c, MATCH-S15 h8, best 66.9%: the only
// source for this RVA, hooks.csv C2), compiled for the MASHED_ORIG_TOP knob path. EXE ONLY.
//   rva 0x00499730 -> matched, Match/MatchS16_00499730.cpp (BootPtr773818Get)
//   rva 0x004a3ac9 -> CRT strtok (library row, out of the P-DoD port count)
//   rva 0x004b302f -> CRT _stricmp (library row)
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00493900 Orig_00493900
#define FUN_00499730 BootPtr773818Get
#define FUN_004a3ac9 strtok
#define FUN_004b302f _stricmp
extern "C" {
#include "../../../../re/match/s15/00493900.c"
}
#endif // MASHED_STANDALONE
