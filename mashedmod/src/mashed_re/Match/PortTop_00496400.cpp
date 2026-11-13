// RVA-LINT: ORIGTOP-ONLY
// BIND-CRT-STDIO-CALLERS (roadmap run 41): rva 0x00496400 ConfigLogDebug = re/match/s16/00496400.c (MATCH-S16, 100%), compiled verbatim
// for the MASHED_ORIG_TOP knob path. EXE ONLY. vsprintf 0x004a42c5 into a 512-byte local, then fputs, if 0x00772fbc != 0. 0x00772fbc is the original address (MGLOBAL under
// MATCH_PORT). Callee adapters and the log-cluster notes: Match/PortTop_00496490.cpp.
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00496400 Orig_00496400
#define fputs PortTop_LogFputs
#define FUN_004a42c5 PortTop_Crt_004a42c5
extern "C" {
#include "../../../../re/match/s16/00496400.c"
}  // extern "C"
#undef fputs
#endif  // MASHED_STANDALONE
