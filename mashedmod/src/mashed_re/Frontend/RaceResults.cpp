// Mashed RE - Race-results slot-state accessors (both moved out; this TU now holds no body).
// Analysis notes:
//   re/analysis/race_results/0040e470.md
//   re/analysis/c0_promotion_frontend_a/0x0040e480.md
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// MOVED OUT 2026-11-01 (D4-DC-INC-ai, dual-copy burn-down): FUN_0040e470 CarSlotStateGet, the getter
// *(PTR_PTR_005f2770 + param_1*4 + 0x34), is now ONE body shared by both targets in Ai/CarSlotState.cpp.
// This TU is .asi-only, and the exe ran two private verbatim copies (AL_40e470 in AiLeafTargets_naked.inc,
// TG_40e470 in AiTargetClosest_naked.inc). The body, its install and its C3 evidence moved unchanged.
//
// MOVED OUT 2026-10-16 (fwd494fd0, dual-copy burn-down): FUN_0040e480 CarSlotStateSet, the setter
// *(PTR_PTR_005f2770 + param_1*4 + 0x34) = param_2, is now ONE body shared by both targets in
// Ai/CarSlotState.cpp. This TU is .asi-only and the exe ran a private copy in AiStandalone.cpp.
//
// NOT INCLUDED (caller-gate or sig failures):
//   0x0040b460  SlotSortByScoreWithModeOverride — REFUSED: callee 0x00417740 not C2
//   0x0040e3a0  PlayerColorTableGet — already in MenuScoreSort.cpp; sig unsupported for diff

#include "../Core/HookSystem.h"
