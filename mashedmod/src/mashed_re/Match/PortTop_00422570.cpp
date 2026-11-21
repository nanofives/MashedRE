// PORT-TOP-RENDERFRAME-492E90 (2026-11-21): exe-only port body of rva 0x00422570 = re/match/s05/00422570.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RENDERFRAME
// through the frame-render closure of 0x00492e90 (re/tools/bind/porttop_bind.py --profile renderframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RenderFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00422570 PortTop_Bound_00422570
#define FUN_00472500 PortTop_Bound_00472500
#define FUN_00477e60 PortTop_Bound_00477e60
#define FUN_00477f50 PortTop_Bound_00477f50
#define FUN_004b4ad0 PortTop_Bound_004b4ad0
#define FUN_004c3d90 PortTop_RRStop_004c3d90   // named stop
extern "C" {
#include "../../../../re/match/s05/00422570.c"
}
#endif // MASHED_STANDALONE
