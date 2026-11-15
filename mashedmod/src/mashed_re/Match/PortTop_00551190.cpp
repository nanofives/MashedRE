// PORT-TOP-AUDIO-HWINIT-4627F0 (2026-11-13): exe-only port body of rva 0x00551190 = re/match/rwl04/00551190.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_AUDIOINIT
// through the closure of 0x004627f0 (sound engine HW init, called by 0x004669b0). Callees bind to exe bodies by name;
// a callee with no matched or admitted body is a named stop (Match/PortTop_AudioStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00551190 PortTop_Bound_00551190
#define FUN_005504d0 PortTop_Bound_005504d0
#define FUN_00550c30 PortTop_Bound_00550c30
#define FUN_00550c50 PortTop_Bound_00550c50
#define FUN_00550d80 PortTop_Bound_00550d80
#define FUN_00550da0 PortTop_Bound_00550da0
#define FUN_00550f20 PortTop_AStop_00550f20   // named stop
#define FUN_00551050 PortTop_Bound_00551050
#define FUN_00551090 PortTop_Bound_00551090
#define FUN_00551180 PortTop_Bound_00551180
#define FUN_00551290 PortTop_Bound_00551290
#define FUN_005512b0 PortTop_Bound_005512b0
#define FUN_005512d0 PortTop_Bound_005512d0
#define FUN_00551330 PortTop_Bound_00551330
#define FUN_005514e0 PortTop_Bound_005514e0
extern "C" {
#include "../../../../re/match/rwl04/00551190.c"
}
#endif // MASHED_STANDALONE
