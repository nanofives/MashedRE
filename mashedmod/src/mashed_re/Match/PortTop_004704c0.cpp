// PORT-TOP-06 (2026-11-21): exe-only port body of rva 0x004704c0 = re/match/s11/004704c0.c
// (hooks.csv C4 c4=frida-replay (L-128)). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACEFRAME
// through the race-frame closure of 0x004111c0 (re/tools/bind/porttop_bind.py --profile raceframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004704c0 PortTop_Bound_004704c0
#define FUN_0041eda0 TimerBitFieldSet   // exe body (Match/MatchS04_0041eda0.cpp)
#define FUN_0046baa0 VehiclePhysicsStateReset   // exe body (Race/SubStateCallees.cpp)
#define FUN_0046d400 PortTop_Bound_0046d400
#define FUN_0046f6c0 PortTop_RFStop_0046f6c0   // named stop
#define FUN_0047ea60 PortTop_Bound_0047ea60
#define FUN_004c4d20 RwCore_004c4d20   // exe body (RwCore/RwCore_004c4d20.cpp)
#define FUN_004c51a0 RwCore_004c51a0   // exe body (RwCore/RwCore_004c51a0.cpp)
extern "C" {
#include "../../../../re/match/s11/004704c0.c"
}
#endif // MASHED_STANDALONE
