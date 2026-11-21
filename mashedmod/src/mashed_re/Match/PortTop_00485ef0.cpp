// PORT-TOP-06 (2026-11-21): exe-only port body of rva 0x00485ef0 = re/match/s14/00485ef0.c
// (hooks.csv C4 c4=frida-replay (L-128)). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACEFRAME
// through the race-frame closure of 0x004111c0 (re/tools/bind/porttop_bind.py --profile raceframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00485ef0 PortTop_Bound_00485ef0
#define FUN_0041f1e0 PortTop_Bound_0041f1e0
#define FUN_00474de0 PortTop_Bound_00474de0
#define FUN_004893b0 PortTop_Bound_004893b0
#define FUN_004893d0 PortTop_Bound_004893d0
#define FUN_00489450 PortTop_Bound_00489450
#define FUN_00489480 PortTop_Bound_00489480
#define FUN_004894a0 PortTop_Bound_004894a0
#define FUN_004894f0 PortTop_Bound_004894f0
#define FUN_00489890 PortTop_Bound_00489890
#define FUN_00485c20 PortTop_Local_00485ef0_00485c20   // file-local in this source
extern "C" {
#include "../../../../re/match/s14/00485ef0.c"
}
#endif // MASHED_STANDALONE
