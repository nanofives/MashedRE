// PORT-TOP-AUDIOTICK-STOPS (2026-11-13): exe-only port body of rva 0x004645c0 = re/match/pilot/a1_004645c0.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_AUDIOTICK
// through the audio-tick closure of 0x00466b50 / 0x0045d3f0 / 0x0045d430. Callees bind to exe bodies by name;
// a callee with no matched or admitted body is a named stop (Match/PortTop_AudioStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004645c0 PortTop_Bound_004645c0
#define FUN_0045daf0 PortTop_Bound_0045daf0
#define FUN_0045e0f0 PortTop_Bound_0045e0f0
#define FUN_0046cbb0 PortTop_AStop_0046cbb0   // named stop
#define FUN_005a66d0 PortTop_Bound_005a66d0
#define FUN_005a6dc0 PortTop_Bound_005a6dc0
// The source declares its globals extern under #ifndef MATCH_PORT; the port reads them at their original addresses.
#define DAT_00604a10 (*(int *)0x00604a10u)
#define DAT_0068f640 ((Slot *)0x0068f640u)
extern "C" {
#include "../../../../re/match/pilot/a1_004645c0.c"
}
#endif // MASHED_STANDALONE
