// PORT-TOP-06 (2026-11-21): exe-only port body of rva 0x0040d8f0 = re/match/racepath/0040d8f0.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACEFRAME
// through the race-frame closure of 0x004111c0 (re/tools/bind/porttop_bind.py --profile raceframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_0040d8f0 PortTop_Bound_0040d8f0
#define FUN_0041f260 PortTop_Bound_0041f260
#define FUN_0041f290 PortTop_Bound_0041f290
#define FUN_00426c00 GameStateFlagGet   // exe body (Match/MatchS06_00426c00.cpp)
#define FUN_0046cc10 VehicleFloatFieldAsInt   // exe body (Match/MatchS11_0046cc10.cpp)
#define FUN_0046d2e0 PortTop_Bound_0046d2e0   // exe body (Match/PortTop_0046d2e0.cpp)
#define FUN_0046d6a0 PortTop_Bound_0046d6a0   // exe body (Match/PortTop_0046d6a0.cpp)
#define FUN_00477b60 PortTop_Bound_00477b60
#define FUN_00477be0 PortTop_RFStop_00477be0   // named stop
#define FUN_00477e00 PortTop_Bound_00477e00
extern "C" {
#include "../../../../re/match/racepath/0040d8f0.c"
}
#endif // MASHED_STANDALONE
