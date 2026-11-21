// PORT-TOP-RENDERFRAME-492E90 (2026-11-21): exe-only port body of rva 0x00410b30 = re/match/s02/00410b30.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RENDERFRAME
// through the frame-render closure of 0x00492e90 (re/tools/bind/porttop_bind.py --profile renderframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RenderFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00410b30 PortTop_Bound_00410b30
#define FUN_00403910 PortTop_Bound_00403910
#define FUN_00404650 PortTop_Bound_00404650
#define FUN_004072e0 PortTop_Bound_004072e0
#define FUN_0040bde0 PortTop_Bound_0040bde0
#define FUN_0040e350 GetRenderSubMode   // exe body (Match/MatchS02_0040e350.cpp)
#define FUN_00411ce0 PortTop_Bound_00411ce0
#define FUN_00413a00 PortTop_RRStop_00413a00   // named stop
#define FUN_00413a40 PortTop_RRStop_00413a40   // named stop
#define FUN_004189c0 PortTop_RRStop_004189c0   // named stop
#define FUN_0041ebb0 PortTop_RRStop_0041ebb0   // named stop
#define FUN_00420050 PortTop_Bound_00420050
#define FUN_004219c0 PortTop_Bound_004219c0
#define FUN_00425e40 PortTop_Bound_00425e40
#define FUN_00426640 PortTop_Bound_00426640
#define FUN_00426ba0 HudDrawEnabled   // exe body (Match/MatchS06_00426ba0.cpp)
#define FUN_004270f0 PortTop_Bound_004270f0
#define FUN_00429e10 PortTop_Bound_00429e10
#define FUN_00448730 PortTop_Bound_00448730
#define FUN_0045b350 RwInitNullStub   // exe body (Match/MatchS09_0045b350.cpp)
#define FUN_0045b990 PortTop_RRStop_0045b990   // named stop
#define FUN_004671a0 RwCore_004671a0   // exe body (RwCore/RwCore_004671a0.cpp)
#define FUN_004725c0 PortTop_RRStop_004725c0   // named stop
#define FUN_00475d30 PortTop_Bound_00475d30
#define FUN_00475e50 PortTop_RRStop_00475e50   // named stop
#define FUN_00477810 PortTop_Bound_00477810
#define FUN_00477a10 PortTop_Bound_00477a10
#define FUN_0047b9e0 PortTop_Bound_0047b9e0
#define FUN_004891f0 PortTop_RRStop_004891f0   // named stop
#define FUN_0048fd70 PortTop_RRStop_0048fd70   // named stop
#define FUN_00490490 PortTop_RRStop_00490490   // named stop
extern "C" {
#include "../../../../re/match/s02/00410b30.c"
}
#endif // MASHED_STANDALONE
