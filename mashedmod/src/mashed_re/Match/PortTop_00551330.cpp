// PORT-TOP-AUDIO-HWINIT-4627F0 (2026-11-13): exe-only port body of rva 0x00551330 = re/match/rwl04/00551330.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_AUDIOINIT
// through the closure of 0x004627f0 (sound engine HW init, called by 0x004669b0). Callees bind to exe bodies by name;
// a callee with no matched or admitted body is a named stop (Match/PortTop_AudioStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00551330 PortTop_Bound_00551330
#define FUN_00550400 PortTop_Bound_00550400
#define FUN_00550520 PortTop_Bound_00550520
#define FUN_00550750 PortTop_Bound_00550750
extern "C" {
#include "../../../../re/match/rwl04/00551330.c"
}
#endif // MASHED_STANDALONE
