// Mashed RE — Frontend per-player score accessor (c3-batch-aa session 2).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Functions in this file:
//   0x00423b80  PlayerScoreAccC  — per-player score accessor #3; base 0x00899f74
//
// Analysis note:
//   re/analysis/frontend_c1_to_c2_s2/0x00423b80.md
//
// Session: c3-batch-aa-s2  2026-05-29
//
// Deferred this session (see BATCH-AA-s2 result block):
//   0x00427880  FUN_00427880  — callee FUN_00554390 (0x00554390) is C1; anti-island rule
//   0x00426cb0  SlotIndexToPtr — already implemented in MenuNearLeaves_s6.cpp; skipped
//   0x00555f20  FontCtx_BuildExtTable — DEFERRED per prompt; live font-ctx ptr, no synthetic ctx
//   0x00495080  FUN_00495080  — void return, no arg_type for (float, uint32) pair; no observable
//   0x004c1be0  sub_004c1be0  — needs live render-target ptr as param_1; no synthetic vectors

#include "../Core/HookSystem.h"

#include <cstdint>

// Forward declarations of callees (originals are still live; we call them by RVA).
// GetRaceSubMode  @ 0x0042f6a0  C3 — returns race sub-mode int (DAT_0067e9fc)

extern "C" std::int32_t __cdecl GetRaceSubMode();   // 0x0042f6a0

// MATCH-S05: the body of 0x00423b80 (PlayerScoreAccC) moved to Match/MatchS05_00423b80.cpp,
// which compiles the byte-matched source re/match/s05/00423b80.c.
