// RO-AUDIOTICK-BODIES (RO-BIND-TIMER follow-up): exe-only port body of rva 0x00466b50 = the byte-matched source
// re/match/s10/00466b50.c (match.py BYTE-IDENTICAL 1208 B, relocheck OK checked=126; hooks.csv C4 c4=match, src sha256 13fee2f17d4125f5).
// Its exe caller is the matched 0x004926c0 AudioTickAndAvg (Match/MatchS15_004926c0.cpp), run under MASHED_ORIG_TOP with
// MASHED_OT_AUDIOTICK=1 (Match/PortTop_00492370.cpp). The .asi keeps its own binding (raw-address call or L2 body).
// The body returns at once while DAT_006904e8 == 0. That word is .bss (0 at load) and its only writer is 0x004669b0
// (re/match/s10/004669b0.c: DAT_006904e8 = 1), which the exe runs as the hand stand-in piece 3 (HandTop_InitAudio,
// Match/PortTop_00402750_Pieces.cpp) that never writes it. So on the exe path no callee below is reached. Each callee
// binds to a named stop (log + exit code = rva, PortTop_AudioCalleeStop in Match/PortTop_00492370.cpp): the exe has
// no body for them, and a stop makes any change of that gate visible instead of calling into unmapped code.
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#include <intrin.h>
#define MATCH_PORT 1
#define FUN_00466b50 PortTop_Bound_00466b50
#define FUN_0040e340 PortTop_AStop_0040e340
#define FUN_0040e370 PortTop_AStop_0040e370
#define FUN_00426c00 GameStateFlagGet   // matched exe body (Match/MatchS06_00426c00.cpp)
#define FUN_0042b8f0 PortTop_AStop_0042b8f0
#define FUN_0042b900 PortTop_AStop_0042b900
#define FUN_0042f6a0 PortTop_AStop_0042f6a0
#define FUN_00431b50 PortTop_AStop_00431b50
#define FUN_00431b60 PortTop_AStop_00431b60
#define FUN_00432290 PortTop_AStop_00432290
#define FUN_0045b350 RwInitNullStub   // matched exe body (Match/MatchS09_0045b350.cpp)
#define FUN_0045c480 PortTop_AStop_0045c480
#define FUN_0045d3a0 PortTop_AStop_0045d3a0
#define FUN_0045d7a0 PortTop_AStop_0045d7a0
#define FUN_0045db50 PortTop_AStop_0045db50
#define FUN_0045dbe0 PortTop_AStop_0045dbe0
#define FUN_0045dc80 PortTop_AStop_0045dc80
#define FUN_0045dd60 PortTop_AStop_0045dd60
#define FUN_0045df70 PortTop_AStop_0045df70
#define FUN_0045dfc0 PortTop_AStop_0045dfc0
#define FUN_0045e460 PortTop_AStop_0045e460
#define FUN_0045e990 PortTop_AStop_0045e990
#define FUN_0045efe0 PortTop_AStop_0045efe0
#define FUN_0045f5f0 PortTop_AStop_0045f5f0
#define FUN_0045faa0 PortTop_AStop_0045faa0
#define FUN_0045ff50 PortTop_AStop_0045ff50
#define FUN_00460270 PortTop_AStop_00460270
#define FUN_00460350 PortTop_AStop_00460350
#define FUN_00460df0 PortTop_AStop_00460df0
#define FUN_00461650 PortTop_AStop_00461650
#define FUN_004623e0 PortTop_AStop_004623e0
#define FUN_00462520 PortTop_AStop_00462520
#define FUN_004627b0 PortTop_AStop_004627b0
#define FUN_00462dd0 PortTop_AStop_00462dd0
#define FUN_00462ec0 PortTop_AStop_00462ec0
#define FUN_00462f50 PortTop_AStop_00462f50
#define FUN_004631f0 PortTop_AStop_004631f0
#define FUN_00463590 PortTop_AStop_00463590
#define FUN_00463640 PortTop_AStop_00463640
#define FUN_00463c80 PortTop_AStop_00463c80
#define FUN_00463f40 PortTop_AStop_00463f40
#define FUN_004642f0 PortTop_AStop_004642f0
#define FUN_004644a0 PortTop_AStop_004644a0
#define FUN_004645c0 PortTop_AStop_004645c0
#define FUN_004647f0 PortTop_AStop_004647f0
#define FUN_004648b0 PortTop_AStop_004648b0
#define FUN_00464a50 PortTop_AStop_00464a50
#define FUN_00464ca0 PortTop_AStop_00464ca0
#define FUN_00464e10 PortTop_AStop_00464e10
#define FUN_004652b0 PortTop_AStop_004652b0
#define FUN_004661f0 PortTop_AStop_004661f0
#define FUN_00492d10 PortTop_AStop_00492d10
#define FUN_004d8560 UtilReturnOne4d8560   // exe body, Util/UtilLeaves_ab6.cpp (returns 1)
#define FUN_005a89a0 PortTop_AStop_005a89a0
#define FUN_005a89b0 PortTop_AStop_005a89b0
#define FUN_005a89c0 PortTop_AStop_005a89c0
extern "C" {
#include "../../../../re/match/s10/00466b50.c"
void PortTop_AudioCalleeStop(unsigned rva, unsigned caller, void* ret);   // Match/PortTop_00492370.cpp
int PortTop_AStop_0040e340(void) { PortTop_AudioCalleeStop(0x0040e340u, 0x00466b50u, _ReturnAddress()); return 0; }
int PortTop_AStop_0040e370(int) { PortTop_AudioCalleeStop(0x0040e370u, 0x00466b50u, _ReturnAddress()); return 0; }
int PortTop_AStop_0042b8f0(void) { PortTop_AudioCalleeStop(0x0042b8f0u, 0x00466b50u, _ReturnAddress()); return 0; }
int PortTop_AStop_0042b900(void) { PortTop_AudioCalleeStop(0x0042b900u, 0x00466b50u, _ReturnAddress()); return 0; }
int PortTop_AStop_0042f6a0(void) { PortTop_AudioCalleeStop(0x0042f6a0u, 0x00466b50u, _ReturnAddress()); return 0; }
float PortTop_AStop_00431b50(void) { PortTop_AudioCalleeStop(0x00431b50u, 0x00466b50u, _ReturnAddress()); return 0.0f; }
float PortTop_AStop_00431b60(void) { PortTop_AudioCalleeStop(0x00431b60u, 0x00466b50u, _ReturnAddress()); return 0.0f; }
int PortTop_AStop_00432290(void) { PortTop_AudioCalleeStop(0x00432290u, 0x00466b50u, _ReturnAddress()); return 0; }
void PortTop_AStop_0045c480(void) { PortTop_AudioCalleeStop(0x0045c480u, 0x00466b50u, _ReturnAddress()); }
void PortTop_AStop_0045d3a0(void) { PortTop_AudioCalleeStop(0x0045d3a0u, 0x00466b50u, _ReturnAddress()); }
void PortTop_AStop_0045d7a0(void) { PortTop_AudioCalleeStop(0x0045d7a0u, 0x00466b50u, _ReturnAddress()); }
void PortTop_AStop_0045db50(int) { PortTop_AudioCalleeStop(0x0045db50u, 0x00466b50u, _ReturnAddress()); }
void PortTop_AStop_0045dbe0(int) { PortTop_AudioCalleeStop(0x0045dbe0u, 0x00466b50u, _ReturnAddress()); }
int PortTop_AStop_0045dc80(void) { PortTop_AudioCalleeStop(0x0045dc80u, 0x00466b50u, _ReturnAddress()); return 0; }
void PortTop_AStop_0045dd60(void) { PortTop_AudioCalleeStop(0x0045dd60u, 0x00466b50u, _ReturnAddress()); }
void PortTop_AStop_0045df70(float) { PortTop_AudioCalleeStop(0x0045df70u, 0x00466b50u, _ReturnAddress()); }
void PortTop_AStop_0045dfc0(void) { PortTop_AudioCalleeStop(0x0045dfc0u, 0x00466b50u, _ReturnAddress()); }
void PortTop_AStop_0045e460(void) { PortTop_AudioCalleeStop(0x0045e460u, 0x00466b50u, _ReturnAddress()); }
void PortTop_AStop_0045e990(void) { PortTop_AudioCalleeStop(0x0045e990u, 0x00466b50u, _ReturnAddress()); }
void PortTop_AStop_0045efe0(void) { PortTop_AudioCalleeStop(0x0045efe0u, 0x00466b50u, _ReturnAddress()); }
void PortTop_AStop_0045f5f0(void) { PortTop_AudioCalleeStop(0x0045f5f0u, 0x00466b50u, _ReturnAddress()); }
void PortTop_AStop_0045faa0(void) { PortTop_AudioCalleeStop(0x0045faa0u, 0x00466b50u, _ReturnAddress()); }
void PortTop_AStop_0045ff50(void) { PortTop_AudioCalleeStop(0x0045ff50u, 0x00466b50u, _ReturnAddress()); }
void PortTop_AStop_00460270(void) { PortTop_AudioCalleeStop(0x00460270u, 0x00466b50u, _ReturnAddress()); }
void PortTop_AStop_00460350(void) { PortTop_AudioCalleeStop(0x00460350u, 0x00466b50u, _ReturnAddress()); }
void PortTop_AStop_00460df0(void) { PortTop_AudioCalleeStop(0x00460df0u, 0x00466b50u, _ReturnAddress()); }
void PortTop_AStop_00461650(void) { PortTop_AudioCalleeStop(0x00461650u, 0x00466b50u, _ReturnAddress()); }
void PortTop_AStop_004623e0(void) { PortTop_AudioCalleeStop(0x004623e0u, 0x00466b50u, _ReturnAddress()); }
int PortTop_AStop_00462520(void) { PortTop_AudioCalleeStop(0x00462520u, 0x00466b50u, _ReturnAddress()); return 0; }
int PortTop_AStop_004627b0(void) { PortTop_AudioCalleeStop(0x004627b0u, 0x00466b50u, _ReturnAddress()); return 0; }
void PortTop_AStop_00462dd0(void) { PortTop_AudioCalleeStop(0x00462dd0u, 0x00466b50u, _ReturnAddress()); }
void PortTop_AStop_00462ec0(void) { PortTop_AudioCalleeStop(0x00462ec0u, 0x00466b50u, _ReturnAddress()); }
void PortTop_AStop_00462f50(void) { PortTop_AudioCalleeStop(0x00462f50u, 0x00466b50u, _ReturnAddress()); }
void PortTop_AStop_004631f0(void) { PortTop_AudioCalleeStop(0x004631f0u, 0x00466b50u, _ReturnAddress()); }
void PortTop_AStop_00463590(void) { PortTop_AudioCalleeStop(0x00463590u, 0x00466b50u, _ReturnAddress()); }
void PortTop_AStop_00463640(int) { PortTop_AudioCalleeStop(0x00463640u, 0x00466b50u, _ReturnAddress()); }
void PortTop_AStop_00463c80(int,  int) { PortTop_AudioCalleeStop(0x00463c80u, 0x00466b50u, _ReturnAddress()); }
void PortTop_AStop_00463f40(int) { PortTop_AudioCalleeStop(0x00463f40u, 0x00466b50u, _ReturnAddress()); }
void PortTop_AStop_004642f0(int,  int) { PortTop_AudioCalleeStop(0x004642f0u, 0x00466b50u, _ReturnAddress()); }
void PortTop_AStop_004644a0(int) { PortTop_AudioCalleeStop(0x004644a0u, 0x00466b50u, _ReturnAddress()); }
void PortTop_AStop_004645c0(int) { PortTop_AudioCalleeStop(0x004645c0u, 0x00466b50u, _ReturnAddress()); }
void PortTop_AStop_004647f0(void) { PortTop_AudioCalleeStop(0x004647f0u, 0x00466b50u, _ReturnAddress()); }
void PortTop_AStop_004648b0(void) { PortTop_AudioCalleeStop(0x004648b0u, 0x00466b50u, _ReturnAddress()); }
void PortTop_AStop_00464a50(void) { PortTop_AudioCalleeStop(0x00464a50u, 0x00466b50u, _ReturnAddress()); }
void PortTop_AStop_00464ca0(void) { PortTop_AudioCalleeStop(0x00464ca0u, 0x00466b50u, _ReturnAddress()); }
void PortTop_AStop_00464e10(void) { PortTop_AudioCalleeStop(0x00464e10u, 0x00466b50u, _ReturnAddress()); }
void PortTop_AStop_004652b0(void) { PortTop_AudioCalleeStop(0x004652b0u, 0x00466b50u, _ReturnAddress()); }
void PortTop_AStop_004661f0(void) { PortTop_AudioCalleeStop(0x004661f0u, 0x00466b50u, _ReturnAddress()); }
int PortTop_AStop_00492d10(void) { PortTop_AudioCalleeStop(0x00492d10u, 0x00466b50u, _ReturnAddress()); return 0; }
int PortTop_AStop_005a89a0(int) { PortTop_AudioCalleeStop(0x005a89a0u, 0x00466b50u, _ReturnAddress()); return 0; }
int PortTop_AStop_005a89b0(int) { PortTop_AudioCalleeStop(0x005a89b0u, 0x00466b50u, _ReturnAddress()); return 0; }
int PortTop_AStop_005a89c0(int) { PortTop_AudioCalleeStop(0x005a89c0u, 0x00466b50u, _ReturnAddress()); return 0; }
}
#endif // MASHED_STANDALONE
