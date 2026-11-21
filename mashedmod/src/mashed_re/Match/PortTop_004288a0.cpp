// PORT-TOP-RENDERFRAME-492E90 (2026-11-21): exe-only port body of rva 0x004288a0 = re/match/s06/004288a0.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RENDERFRAME
// through the frame-render closure of 0x00492e90 (re/tools/bind/porttop_bind.py --profile renderframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RenderFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004288a0 PortTop_Bound_004288a0
#define FUN_00427e00 RwCore_00427e00   // exe body (RwCore/RwCore_00427e00.cpp)
#define FUN_004282a0 RwCore_004282a0   // exe body (RwCore/RwCore_004282a0.cpp)
#define FUN_00428760 RwCore_00428760   // exe body (RwCore/RwCore_00428760.cpp)
extern "C" {
#include "../../../../re/match/s06/004288a0.c"
}
#endif // MASHED_STANDALONE
