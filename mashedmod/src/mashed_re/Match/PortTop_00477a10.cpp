// PORT-TOP-RENDERFRAME-492E90 (2026-11-21): exe-only port body of rva 0x00477a10 = re/match/s12/00477a10.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RENDERFRAME
// through the frame-render closure of 0x00492e90 (re/tools/bind/porttop_bind.py --profile renderframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RenderFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00477a10 PortTop_Bound_00477a10
#define FUN_004b6520 Orig_004b6520   // exe body (Match/PortTop_004b6520.cpp)
#define FUN_004c1b40 PortTop_RRStop_004c1b40   // named stop
#define FUN_004cd070 RwCore_004cd070   // exe body (RwCore/RwCore_004cd070.cpp)
#define FUN_004cd140 RwCore_004cd140   // exe body (RwCore/RwCore_004cd140.cpp)
#define FUN_004cd2d0 PortTop_Bound_004cd2d0
#define FUN_004778e0 PortTop_Local_00477a10_004778e0   // file-local in this source
extern "C" {
#include "../../../../re/match/s12/00477a10.c"
}
#endif // MASHED_STANDALONE
