// PORT-TOP-RENDERFRAME-492E90 (2026-11-21): exe-only port body of rva 0x00429bd0 = re/match/s06/00429bd0.c
// (L-126 admitted (re/match/ADMITTED.tsv, EQUIV)). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RENDERFRAME
// through the frame-render closure of 0x00492e90 (re/tools/bind/porttop_bind.py --profile renderframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RenderFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00429bd0 PortTop_Bound_00429bd0
#define FUN_00401340 CupSpinSpeedAndColor   // exe body (Match/MatchS01.cpp)
#define FUN_004013f0 PortTop_Bound_004013f0
#define FUN_0040dc80 PortTop_Bound_0040dc80
#define FUN_00427e00 RwCore_00427e00   // exe body (RwCore/RwCore_00427e00.cpp)
#define FUN_00429620 PortTop_Bound_00429620
#define FUN_00429b70 PortTop_Bound_00429b70
#define FUN_00472c60 RwCore_00472c60   // exe body (RwCore/RwCore_00472c60.cpp)
extern "C" {
#include "../../../../re/match/s06/00429bd0.c"
}
#endif // MASHED_STANDALONE
