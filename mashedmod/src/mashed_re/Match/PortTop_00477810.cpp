// PORT-TOP-RENDERFRAME-492E90 (2026-11-21): exe-only port body of rva 0x00477810 = re/match/s12/00477500.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RENDERFRAME
// through the frame-render closure of 0x00492e90 (re/tools/bind/porttop_bind.py --profile renderframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RenderFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00477810 PortTop_Bound_00477810
#define FUN_005492a0 PortTop_Bound_005492a0
#define FUN_005493d0 PortTop_Bound_005493d0
#define FUN_00477500 PortTop_Local_00477810_00477500   // file-local in this source
extern "C" {
#include "../../../../re/match/s12/00477500.c"
}
#endif // MASHED_STANDALONE
