// PORT-TOP-RENDERFRAME-492E90 (2026-11-21): exe-only port body of rva 0x00472ad0 = re/match/s11/00472ad0.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RENDERFRAME
// through the frame-render closure of 0x00492e90 (re/tools/bind/porttop_bind.py --profile renderframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RenderFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00472ad0 PortTop_Bound_00472ad0
#define FUN_004729b0 PortTop_Bound_004729b0
extern "C" {
#include "../../../../re/match/s11/00472ad0.c"
}
#endif // MASHED_STANDALONE
