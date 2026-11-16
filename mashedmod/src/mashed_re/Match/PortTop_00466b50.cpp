// RO-AUDIOTICK-BODIES (RO-BIND-TIMER follow-up): exe-only port body of rva 0x00466b50 = the byte-matched source
// re/match/s10/00466b50.c (match.py BYTE-IDENTICAL 1208 B, relocheck OK checked=126; hooks.csv C4 c4=match, src sha256 13fee2f17d4125f5).
// Its exe caller is the matched 0x004926c0 AudioTickAndAvg (Match/MatchS15_004926c0.cpp), run under MASHED_ORIG_TOP with
// MASHED_OT_AUDIOTICK=1 (Match/PortTop_00492370.cpp). The .asi keeps its own binding (raw-address call or L2 body).
// The body returns at once while DAT_006904e8 == 0. That word is .bss (0 at load) and its only writer is 0x004669b0
// (re/match/s10/004669b0.c: DAT_006904e8 = 1), which the exe runs as the hand stand-in piece 3 (HandTop_InitAudio,
// Match/PortTop_00402750_Pieces.cpp) that never writes it. So on the exe path no callee below is reached. Each callee
// binds to a named stop (log + exit code = rva, PortTop_AudioCalleeStop in Match/PortTop_00492370.cpp): the exe has
// no body for them, and a stop makes any change of that gate visible instead of calling into unmapped code.
// PORT-TOP-AUDIOTICK-STOPS (2026-11-13): callees rebound to matched / admitted exe bodies; the rest stay named stops
// (PortTop_AStop_<rva>, Match/PortTop_AudioStops.cpp).
// PORT-TOP-AUDIOTICK-STOPS (2026-11-13): callees rebound to matched / admitted exe bodies; the rest stay named stops
// (PortTop_AStop_<rva>, Match/PortTop_AudioStops.cpp).
// PORT-TOP-AUDIOTICK-STOPS (2026-11-13): callees rebound to matched / admitted exe bodies; the rest stay named stops
// (PortTop_AStop_<rva>, Match/PortTop_AudioStops.cpp).
// PORT-TOP-AUDIOTICK-STOPS (2026-11-13): callees rebound to matched / admitted exe bodies; the rest stay named stops
// (PortTop_AStop_<rva>, Match/PortTop_AudioStops.cpp).
// PORT-TOP-AUDIOTICK-STOPS (2026-11-13): callees rebound to matched / admitted exe bodies; the rest stay named stops
// (PortTop_AStop_<rva>, Match/PortTop_AudioStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00466b50 PortTop_Bound_00466b50
#define FUN_0040e340 GetLiveCarCount   // exe body (Match/MatchS02_0040e340.cpp)
#define FUN_0040e370 IsCarSlotActive   // exe body (Match/MatchS02_0040e370.cpp)
#define FUN_00426c00 GameStateFlagGet   // exe body (Match/MatchS06_00426c00.cpp)
#define FUN_0042b8f0 StatePhaseIsFinal   // exe body (Match/MatchS07_0042b8f0.cpp)
#define FUN_0042b900 PortTop_Bound_0042b900
#define FUN_0042f6a0 GetRaceSubMode   // exe body (Match/MatchS07_0042f6a0.cpp)
#define FUN_00431b50 SinGlobalProduct431b50   // exe body (Match/MatchS07_00431b50.cpp)
#define FUN_00431b60 SinGlobalProduct431b60   // exe body (Match/MatchS07_00431b60.cpp)
#define FUN_00432290 PortTop_Bound_00432290
#define FUN_0045b350 RwInitNullStub   // exe body (Match/MatchS09_0045b350.cpp)
#define FUN_0045c480 PortTop_Bound_0045c480
#define FUN_0045d3a0 PortTop_Bound_0045d3a0
#define FUN_0045d7a0 PortTop_AStop_0045d7a0   // named stop
#define FUN_0045db50 FloatSliderStep   // exe body (Match/MatchS10_0045db50.cpp)
#define FUN_0045dbe0 PortTop_Bound_0045dbe0
#define FUN_0045dc80 PortTop_Bound_0045dc80
#define FUN_0045dd60 PortTop_AStop_0045dd60   // named stop
#define FUN_0045df70 PortTop_Bound_0045df70
#define FUN_0045dfc0 PortTop_Bound_0045dfc0
#define FUN_0045e460 PortTop_Bound_0045e460
#define FUN_0045e990 PortTop_AStop_0045e990   // named stop
#define FUN_0045efe0 PortTop_AStop_0045efe0   // named stop
#define FUN_0045f5f0 PortTop_Bound_0045f5f0
#define FUN_0045faa0 PortTop_Bound_0045faa0
#define FUN_0045ff50 PortTop_Bound_0045ff50
#define FUN_00460270 PortTop_Bound_00460270
#define FUN_00460350 PortTop_Bound_00460350
#define FUN_00460df0 PortTop_Bound_00460df0
#define FUN_00461650 PortTop_Bound_00461650
#define FUN_004623e0 PortTop_Bound_004623e0
#define FUN_00462520 PortTop_Bound_00462520
#define FUN_004627b0 PortTop_Bound_004627b0
#define FUN_00462dd0 PortTop_Bound_00462dd0
#define FUN_00462ec0 PortTop_Bound_00462ec0
#define FUN_00462f50 PortTop_Bound_00462f50
#define FUN_004631f0 PortTop_Bound_004631f0
#define FUN_00463590 PortTop_Bound_00463590
#define FUN_00463640 PortTop_AStop_00463640   // named stop
#define FUN_00463c80 PortTop_Bound_00463c80
#define FUN_00463f40 PortTop_Bound_00463f40
#define FUN_004642f0 PortTop_Bound_004642f0
#define FUN_004644a0 PortTop_Bound_004644a0
#define FUN_004645c0 PortTop_Bound_004645c0
#define FUN_004647f0 PortTop_Bound_004647f0
#define FUN_004648b0 PortTop_Bound_004648b0
#define FUN_00464a50 PortTop_Bound_00464a50
#define FUN_00464ca0 PortTop_AStop_00464ca0   // named stop
#define FUN_00464e10 PortTop_Bound_00464e10
#define FUN_004652b0 PortTop_Bound_004652b0
#define FUN_004661f0 PortTop_Bound_004661f0
#define FUN_00492d10 PortTop_Bound_00492d10
#define FUN_004d8560 UtilReturnOne4d8560   // exe body (Util/UtilLeaves_ab6.cpp)
#define FUN_005a89a0 VoiceFieldD6cGet   // exe body (Audio/AudioFieldGetters.cpp)
#define FUN_005a89b0 VoiceFieldD70Get   // exe body (Audio/AudioFieldGetters.cpp)
#define FUN_005a89c0 VoiceFieldD74Get   // exe body (Audio/AudioFieldGetters.cpp)
extern "C" {
#include "../../../../re/match/s10/00466b50.c"
}
#endif // MASHED_STANDALONE
