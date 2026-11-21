// PORT-TOP-RENDERFRAME-492E90 (2026-11-21): exe-only port body of rva 0x00427840 = re/match/s06/00427840.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RENDERFRAME
// through the frame-render closure of 0x00492e90 (re/tools/bind/porttop_bind.py --profile renderframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RenderFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00427840 PortTop_Bound_00427840
#define FUN_00427780 RwCore_00427780   // exe body (RwCore/RwCore_00427780.cpp)
#define FUN_00552d10 RwCore_00552d10   // exe body (RwCore/RwCore_00552d10.cpp)
#define FUN_00552d70 RwCore_00552d70   // exe body (RwCore/RwCore_00552d70.cpp)
#define FUN_005555b0 RwCore_005555b0   // exe body (RwCore/RwCore_005555b0.cpp)
#define FUN_00556e90 RwCore_00556e90   // exe body (RwCore/RwCore_00556e90.cpp)
extern "C" {
#include "../../../../re/match/s06/00427840.c"
}
#endif // MASHED_STANDALONE
