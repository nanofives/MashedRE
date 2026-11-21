// PORT-TOP-06 (2026-11-21): exe-only port body of rva 0x0040ce00 = re/match/s02/0040ce00.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACEFRAME
// through the race-frame closure of 0x004111c0 (re/tools/bind/porttop_bind.py --profile raceframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_0040ce00 PortTop_Bound_0040ce00
#define FUN_0040c5b0 PortTop_Bound_0040c5b0
#define FUN_0040ca00 PortTop_Bound_0040ca00
#define FUN_00412620 PortTop_RFStop_00412620   // named stop
#define FUN_00413cb0 PortTop_RFStop_00413cb0   // named stop
#define FUN_004212b0 PortTop_Bound_004212b0
#define FUN_00425ef0 ActiveSlotCount   // exe body (Match/MatchS05_00425ef0.cpp)
#define FUN_00475c10 PortTop_Bound_00475c10
#define FUN_00476440 PortTop_Bound_00476440
#define FUN_004777d0 PortTop_Bound_004777d0
#define FUN_00481a30 PortTop_Bound_00481a30
#define FUN_00485ef0 PortTop_Bound_00485ef0
#define FUN_00486830 PortTop_Bound_00486830
#define FUN_00487df0 PortTop_RFStop_00487df0   // named stop
#define FUN_0048a780 PortTop_RFStop_0048a780   // named stop
#define FUN_0048ab70 PortTop_Bound_0048ab70
#define FUN_0048b960 PortTop_Bound_0048b960
#define FUN_0048e5f0 PortTop_Bound_0048e5f0
#define FUN_0048f8a0 PortTop_Bound_0048f8a0
#define FUN_0048f9f0 PortTop_Bound_0048f9f0
#define FUN_0048fb10 PortTop_Bound_0048fb10
#define FUN_00490380 PortTop_Bound_00490380
#define FUN_00490500 PortTop_Bound_00490500
#define thunk_FUN_00425ef0 ActiveSlotCount   // Ghidra thunk name of the jump to 0x00425ef0: the port calls its exe body
extern "C" {
#include "../../../../re/match/s02/0040ce00.c"
}
#endif // MASHED_STANDALONE
