// PORT-TOP-RENDERFRAME-492E90 (2026-11-21): exe-only port body of rva 0x0040dfc0 = re/match/s02/0040dfc0.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RENDERFRAME
// through the frame-render closure of 0x00492e90 (re/tools/bind/porttop_bind.py --profile renderframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RenderFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_0040dfc0 PortTop_Bound_0040dfc0
#define FUN_00403160 RwCore_00403160   // exe body (RwCore/RwCore_00403160.cpp)
#define FUN_0041a3e0 PortTop_RRStop_0041a3e0   // named stop
#define FUN_0041b630 PortTop_RRStop_0041b630   // named stop
#define FUN_0041c0c0 HudDispatchSlot2   // exe body (Match/MatchS04_0041bc50.cpp)
#define FUN_0041c300 HudDispatchMode5   // exe body (Match/MatchS04_0041c2d0.cpp)
#define FUN_0041ccc0 HudSlotLoopCcc0   // exe body (Match/MatchS04_0041c9a0.cpp)
#define FUN_0041d870 PortTop_Bound_0041d870
#define FUN_0041db80 PortTop_RRStop_0041db80   // named stop
#define FUN_0041ded0 PortTop_RRStop_0041ded0   // named stop
#define FUN_0041e850 PortBatch_0041e850   // exe body (Match/PortBatch_HUD_0041e630.cpp)
#define FUN_00426ba0 HudDrawEnabled   // exe body (Match/MatchS06_00426ba0.cpp)
#define FUN_0042f500 GetDat0067ea64   // exe body (Match/MatchS07_0042f500.cpp)
#define FUN_0042f6a0 GetRaceSubMode   // exe body (Match/MatchS07_0042f6a0.cpp)
extern "C" {
#include "../../../../re/match/s02/0040dfc0.c"
}
#endif // MASHED_STANDALONE
