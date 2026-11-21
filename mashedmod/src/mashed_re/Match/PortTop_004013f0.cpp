// PORT-TOP-RENDERFRAME-492E90 (2026-11-21): exe-only port body of rva 0x004013f0 = re/match/s01/004013f0.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RENDERFRAME
// through the frame-render closure of 0x00492e90 (re/tools/bind/porttop_bind.py --profile renderframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RenderFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004013f0 PortTop_Bound_004013f0
#define FUN_004011f0 PortTop_Bound_004011f0
#define FUN_00426060 PortTop_Bound_00426060   // exe body (Match/PortTop_00426060.cpp)
#define FUN_004c19f0 RwCore_004c19f0   // exe body (RwCore/RwCore_004c19f0.cpp)
#define FUN_004c1a00 RwCore_004c1a00   // exe body (RwCore/RwCore_004c1a00.cpp)
#define FUN_004e4320 PortTop_Bound_004e4320
#define FUN_004e4350 PortTop_Bound_004e4350
#define FUN_004e6680 PortTop_RRStop_004e6680   // named stop
extern "C" {
#include "../../../../re/match/s01/004013f0.c"
}
#endif // MASHED_STANDALONE
