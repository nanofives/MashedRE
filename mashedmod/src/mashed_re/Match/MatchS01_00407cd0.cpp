// MATCH-TOPDOWN-T13: port body of 0x00407cd0 = the byte-matched source re/match/s01/00407cd0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%, relocheck OK).
// It replaces the naked verbatim transcription PP_407cd0 (Race/PathProgStep_naked.inc), so the exe's PathProg
// chain (PP_407e20 calls it at 0x00407f9f / 0x00407faf / 0x004080e6 / 0x004080f3) runs the matched C body.
// Exe only: PathProgStep.cpp is exe-only and the .asi has no body at this RVA. No install (the naked body had none).
// Callee 0x00426090 binds to GlobalDat0066ce58Get (returns &DAT_0066ce58, written in the exe by Race/PathNodes.cpp).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00407cd0 PP_407cd0
#define FUN_00426090 GlobalDat0066ce58Get
extern "C" {
// 0x00407cd0
#include "../../../../re/match/s01/00407cd0.c"
}
