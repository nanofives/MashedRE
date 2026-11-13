// PORT-TOP-AUDIOTICK-STOPS (2026-11-13): exe-only port body of rva 0x004661f0 = re/match/s10/004661f0.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_AUDIOTICK
// through the audio-tick closure of 0x00466b50 / 0x0045d3f0 / 0x0045d430. Callees bind to exe bodies by name;
// a callee with no matched or admitted body is a named stop (Match/PortTop_AudioStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004661f0 PortTop_Bound_004661f0
#define FUN_004417e0 PortTop_Bound_004417e0
#define FUN_00452160 PortTop_Bound_00452160
#define FUN_0045e0f0 PortTop_Bound_0045e0f0
#define FUN_00465a30 PortTop_Bound_00465a30
#define FUN_00465b20 PortTop_Bound_00465b20
#define FUN_00472650 PortTop_Bound_00472650
#define FUN_004b44f0 PortTop_Bound_004b44f0
#define FUN_005a66d0 PortTop_Bound_005a66d0
#define FUN_005a6d60 PortTop_Bound_005a6d60
#define FUN_005a6dc0 PortTop_Bound_005a6dc0
extern "C" {
#include "../../../../re/match/s10/004661f0.c"
}
#endif // MASHED_STANDALONE
