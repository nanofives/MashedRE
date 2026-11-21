// PORT-TOP-06 (2026-11-21): exe-only port body of rva 0x0040ca00 = re/match/racepath/0040ca00.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACEFRAME
// through the race-frame closure of 0x004111c0 (re/tools/bind/porttop_bind.py --profile raceframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_0040ca00 PortTop_Bound_0040ca00
#define FUN_0040c690 PortTop_Bound_0040c690
#define FUN_0041efe0 PortTop_Bound_0041efe0
#define FUN_0041f290 PortTop_Bound_0041f290
#define FUN_0045dd50 PortTop_Bound_0045dd50
#define FUN_0046cb30 PortTop_Bound_0046cb30
#define FUN_0046cc10 VehicleFloatFieldAsInt   // exe body (Match/MatchS11_0046cc10.cpp)
#define FUN_0046d320 PortTop_Bound_0046d320
#define FUN_0046d570 PortTop_Bound_0046d570
#define FUN_0046d6a0 PortTop_Bound_0046d6a0   // exe body (Match/PortTop_0046d6a0.cpp)
#define FUN_00472650 PortTop_Bound_00472650   // exe body (Match/PortTop_00472650.cpp)
#define FUN_00485e50 PortTop_Bound_00485e50
#define FUN_004c3ac0 RwCore_004c3ac0   // exe body (RwCore/RwCore_004c3ac0.cpp)
extern "C" {
#include "../../../../re/match/racepath/0040ca00.c"
}
#endif // MASHED_STANDALONE
