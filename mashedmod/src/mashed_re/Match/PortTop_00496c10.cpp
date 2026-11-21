// PORT-TOP-RENDERFRAME-492E90 (2026-11-21): exe-only port body of rva 0x00496c10 = re/match/s16/00496c10.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RENDERFRAME
// through the frame-render closure of 0x00492e90 (re/tools/bind/porttop_bind.py --profile renderframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RenderFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00496c10 PortTop_Bound_00496c10
#define FUN_004938e0 PortTop_Movie_004938e0   // exe body (Match/PortTop_MovieCore.cpp)
#define FUN_004c75e0 PortTop_Bound_004c75e0
#define FUN_004969a0 PortTop_Local_00496c10_004969a0   // file-local in this source
extern "C" {
#include "../../../../re/match/s16/00496c10.c"
}
#endif // MASHED_STANDALONE
