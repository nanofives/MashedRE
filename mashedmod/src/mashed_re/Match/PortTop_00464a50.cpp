// PORT-TOP-AUDIOTICK-STOPS (2026-11-13): exe-only port body of rva 0x00464a50 = re/match/s10/00464a50.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_AUDIOTICK
// through the audio-tick closure of 0x00466b50 / 0x0045d3f0 / 0x0045d430. Callees bind to exe bodies by name;
// a callee with no matched or admitted body is a named stop (Match/PortTop_AudioStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00464a50 PortTop_Bound_00464a50
#define FUN_00405890 Pred405890   // exe body (Match/MatchS01.cpp)
#define FUN_004075a0 PortTop_AStop_004075a0   // named stop
#define FUN_00407600 PortTop_Bound_00407600
#define FUN_00425ee0 SlotWordPtrGet   // exe body (Match/MatchS05_00425ee0.cpp)
#define FUN_00425ef0 ActiveSlotCount   // exe body (Match/MatchS05_00425ef0.cpp)
#define FUN_004417e0 PortTop_Bound_004417e0
#define FUN_0045e0f0 PortTop_Bound_0045e0f0
#define FUN_004c3ac0 PortTop_AStop_004c3ac0   // named stop
#define FUN_005a66d0 PortTop_Bound_005a66d0
#define FUN_005a6d60 PortTop_AStop_005a6d60   // named stop
#define FUN_005a6dc0 PortTop_Bound_005a6dc0
extern "C" {
#include "../../../../re/match/s10/00464a50.c"
}
#endif // MASHED_STANDALONE
