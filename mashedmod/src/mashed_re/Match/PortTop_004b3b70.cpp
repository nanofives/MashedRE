// POWERUPINIT-A (L-111 step 1): exe-only port body of rva 0x004b3b70 = the matched source re/match/s18/004b3b70.c
// (MATCH-S18, 100%): RwStreamOpen(2, 1, path), RwStreamRead(stream, buf, size), RwStreamClose(stream, 0); returns the
// byte count read, 0 when the open fails. Callers on the MASHED_ORIG_TOP path: the matched builders 0x004587a0
// ("IconCube.dff" into a 0x800-byte buffer) and 0x00411f30 ("lazer.dff" into 0x400), both from 0x0045bae0, with the size
// 0x004b65b0 returns (the piz entry length). Callees bind by name, no raw-address call: the stream bindings of
// Match/PortTop_RwStream04F.cpp (open type 2 reads the name from the piz mount stack, then the disk under "original/").
// The exe's other body of this RVA, Save/FileWrappers.cpp FileReadWrapperCore (both targets), opens through a host
// thunk that has no exe stream open, so in the exe it returns 0 for every path. That body stays on the default path
// until the ORIG_TOP flip.
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004b3b70 Orig_004b3b70
#define FUN_004cc230 PortTop_RwStreamOpen_004cc230
#define FUN_004cbd30 PortTop_RwStreamRead_004cbd30
#define FUN_004cc160 PortTop_RwStreamClose_004cc160
extern "C" {
#include "../../../../re/match/s18/004b3b70.c"
}
#endif  // MASHED_STANDALONE
