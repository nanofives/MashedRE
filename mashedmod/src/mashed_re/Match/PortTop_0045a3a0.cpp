// POWERUPINIT-C: exe-only copy of rva 0x0045a3a0 (hooks.csv C2, re/match/s09/0045a3a0.c, best 81.0%, NOT a byte match).
// The source's loop and stores are the original's instruction for instruction; the residual is register allocation
// only (the original keeps param_1 in EBP from entry, the source reloads it into EAX after the loop, POWERUPINIT-B).
// What it does: zeroes fields 0, 1, 2, 3, 4, 5, 6, 8, 11, 12 of every 0x34-byte record of the 4 x 5 x 5 array at
// 0x0068bd00, then param_1[0] = param_1[5] = param_1[6] = 0 and param_1[3] = 0x3f800000. No callee.
// Caller on the MASHED_ORIG_TOP path: the matched init 0x0045a6b0 (Match/PortTop_0045a6b0.cpp), once per record.
// Not a matched body, so the BINDINGS row is "port", not "matched"; it moves to matched when the source reaches 100%.
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_0045a3a0 PortTop_0045a3a0_Copy
extern "C" {
#include "../../../../re/match/s09/0045a3a0.c"
}  // extern "C"
#endif  // MASHED_STANDALONE
