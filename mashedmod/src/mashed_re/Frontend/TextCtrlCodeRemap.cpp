// TextCtrlCodeRemap -- 0x004277a0
//
// MATCH-S06 (2026-11-01): this TU holds no body. The body of 0x004277a0 moved to Match/MatchS06_004277a0.cpp,
// which compiles the byte-matched source re/match/s06/004277a0.c as the cdecl TextCtrlCodeRemapBody. The
// installed/exported entry TextCtrlCodeRemap is the EAX = src / EBX = dst register adapter in that wrapper, and it
// keeps the U-9065 dev-only EBX == 0 boot guard (#ifndef MASHED_STANDALONE) this file carried.
// History: the naked verbatim transcription that lived here, and its notes, are in git history
// (ref: re/analysis/promote_c1_low_ab1/0x004277a0.md, re/analysis/frontend_004277a0_c3_plan.md).

#include "../Core/HookSystem.h"
