// PORT-TOP-02: 0x00493900 command-line switch parser, compiled for the MASHED_ORIG_TOP knob path. EXE ONLY.
// DDKOBJ-RETARGET-493900 (2026-11-15): compiles re/match/topm01/00493900.c (MATCH-TOPM01, 100%, --bytes
// BYTE-IDENTICAL, relocheck OK; same MATCH-S15 preamble) instead of the s15 draft re/match/s15/00493900.c (66.9%,
// relocheck BAD), so the object build links the certified bytes (verify/ddkobj_20261114/RESULT_DDKOBJ-RETARGET-493710.md).
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
#include "../../../../re/match/topm01/00493900.c"
}
#endif // MASHED_STANDALONE
