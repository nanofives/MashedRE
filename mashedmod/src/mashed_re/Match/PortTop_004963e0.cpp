// RVA-LINT: ORIGTOP-ONLY
// BIND-CRT-STDIO-CALLERS (roadmap run 41): rva 0x004963e0 ConfigLogError = re/match/s16/004963e0.c (MATCH-S16, 100%), compiled verbatim
// for the MASHED_ORIG_TOP knob path. EXE ONLY. fputs if 0x00772fbc != 0. 0x00772fbc is the original address (MGLOBAL under
// MATCH_PORT). Callee adapters and the log-cluster notes: Match/PortTop_00496490.cpp.
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004963e0 Orig_004963e0
#define fputs PortTop_LogFputs
extern "C" {
#include "../../../../re/match/s16/004963e0.c"
}  // extern "C"
#undef fputs
#endif  // MASHED_STANDALONE
