// PORT-TOP-RENDERFRAME-492E90 (2026-11-21): exe-only port body of rva 0x0040bde0 = re/match/s02/0040bde0.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RENDERFRAME
// through the frame-render closure of 0x00492e90 (re/tools/bind/porttop_bind.py --profile renderframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RenderFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_0040bde0 PortTop_Bound_0040bde0
#define FUN_00412100 PortTop_RRStop_00412100   // named stop
#define FUN_00413f50 PortTop_RRStop_00413f50   // named stop
#define FUN_00421560 PortTop_RRStop_00421560   // named stop
#define FUN_00457610 PortTop_RRStop_00457610   // named stop
#define FUN_00475d30 PortTop_Bound_00475d30
#define FUN_00475e50 PortTop_RRStop_00475e50   // named stop
#define FUN_00477810 PortTop_Bound_00477810
#define FUN_00486220 PortTop_Bound_00486220
#define FUN_00486f50 PortTop_RRStop_00486f50   // named stop
#define FUN_00487de0 PortTop_RRStop_00487de0   // named stop
#define FUN_0048ad50 PortTop_RRStop_0048ad50   // named stop
#define FUN_0048baf0 PortTop_RRStop_0048baf0   // named stop
#define FUN_0048e7b0 PortTop_Bound_0048e7b0
#define FUN_0048fce0 PortTop_RRStop_0048fce0   // named stop
#define FUN_0048fd10 PortTop_RRStop_0048fd10   // named stop
#define FUN_0048fd40 PortTop_RRStop_0048fd40   // named stop
#define FUN_0048fd70 PortTop_RRStop_0048fd70   // named stop
#define FUN_00490490 PortTop_RRStop_00490490   // named stop
extern "C" {
#include "../../../../re/match/s02/0040bde0.c"
}
#endif // MASHED_STANDALONE
