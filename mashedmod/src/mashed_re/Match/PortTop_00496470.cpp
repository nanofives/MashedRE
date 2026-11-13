// RVA-LINT: ORIGTOP-ONLY
// BIND-CRT-STDIO-CALLERS (roadmap run 41): rva 0x00496470 log close (WM_DESTROY) = re/match/s16/00496470.c (MATCH-S16, 100%), compiled verbatim
// for the MASHED_ORIG_TOP knob path. EXE ONLY. fclose 0x00772fbc and store 0, if 0x00772fbc != 0. 0x00772fbc is the original address (MGLOBAL under
// MATCH_PORT). Callee adapters and the log-cluster notes: Match/PortTop_00496490.cpp.
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00496470 Orig_00496470
#define fclose PortTop_LogFclose
extern "C" {
#include "../../../../re/match/s16/00496470.c"
}  // extern "C"
#undef fclose
#endif  // MASHED_STANDALONE
