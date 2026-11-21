// PORT-TOP-RENDERFRAME-492E90 (2026-11-21): exe-only port body of rva 0x004cd2d0 = re/match/s20/004cd2d0.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RENDERFRAME
// through the frame-render closure of 0x00492e90 (re/tools/bind/porttop_bind.py --profile renderframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RenderFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004cd2d0 PortTop_Bound_004cd2d0
#define FUN_004d40c0 GetPipelinePtr   // exe body (Render/BatchAB_s1.cpp)
#define FUN_004d40d0 RwCore_004d40d0   // exe body (RwCore/RwCore_004d40d0.cpp)
#define FUN_004d7ff0 Orig_004d7ff0   // exe body (Match/PortTop_004d7ff0.cpp)
#define FUN_004d8480 RwCore_004d8480   // exe body (RwCore/RwCore_004d8480.cpp)
extern "C" {
#include "../../../../re/match/s20/004cd2d0.c"
}
#endif // MASHED_STANDALONE
