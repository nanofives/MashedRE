// Mashed RE - Frontend menu initialisation reimplementations.
// Analysis notes:
//   re/analysis/promote_c2_frontend_menus/0x0042fe80.md  (GetRaceEndFlag)
//   re/analysis/promote_c2_frontend_menus/0x0042f0b0.md  (GetFrameCounterPlus73)
//   re/analysis/promote_c2_frontend_menus/0x0042d3e0.md  (MenuEntryArrayInit)
//
// All functions in this file are pure leaves (callees_depth1: []).
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)

#include "../Core/HookSystem.h"
#include "MenuEntryZero.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// GetRaceEndFlag  --  0x0042fe80
//
// Original: FUN_0042fe80 (5 bytes, 0x0042fe80..0x0042fe85)
// Body: MOV EAX, [0x0067ea90]; RET
// Returns: DAT_0067ea90 (race-end flag or slot gate).
// No callees, no branches, no side-effects.
// ref: re/analysis/promote_c2_frontend_menus/0x0042fe80.md
// ---------------------------------------------------------------------------

// 0x0042fe80
extern "C" __declspec(dllexport) std::uint32_t __cdecl GetRaceEndFlag() {
    return *reinterpret_cast<std::uint32_t*>(0x0067ea90u);
}

RH_ScopedInstall(GetRaceEndFlag, 0x0042fe80);  // re-enabled 2026-05-24 batch-frontend

// ---------------------------------------------------------------------------
// GetFrameCounterPlus73  --  0x0042f0b0
//
// Original: FUN_0042f0b0 (8 bytes, 0x0042f0b0..0x0042f0b8)
// Body: MOV EAX, [0x0067f17c]; ADD EAX, 0x49; RET
// Returns: DAT_0067f17c (animation frame counter) + 0x49 (73, cited at 0x0042f0b4).
// No callees, no branches, no side-effects.
// ref: re/analysis/promote_c2_frontend_menus/0x0042f0b0.md
// ---------------------------------------------------------------------------

// 0x0042f0b0
extern "C" __declspec(dllexport) int __cdecl GetFrameCounterPlus73() {
    return *reinterpret_cast<std::int32_t*>(0x0067f17cu) + 0x49;
}

RH_ScopedInstall(GetFrameCounterPlus73, 0x0042f0b0);  // re-enabled 2026-05-24 c3-frontend-b

// ---------------------------------------------------------------------------
// MenuEntryArrayInit  --  0x0042d3e0
//
// Original: FUN_0042d3e0 (59 bytes, 0x0042d3e0..0x0042d41a). Clears 14 stores per entry over 30 entries of a 52-byte struct array at 0x00898ac0..0x008990dc.
// The body is Frontend/MenuEntryZero.h:MenuEntryZeroBody, written from the disassembly (verify/dualcopy4_20261016/disasm_42d3e0.txt): dwords at
// entry +0, +4, +8, bytes +12..+15, dwords +16, +20, +24, +28, +36, +40, +44; entry +32 and entry 30 onwards are NOT written.
//
// dualcopy4 (2026-10-16) CORRECTION. The body that stood here (C3 at 9b0248128) did not match those bytes: it wrote the dword before the entry,
// bytes +9..+12 instead of +12..+15, +32 instead of +28 and +44 (host test verify/dualcopy4_20261016/maskcheck_42d3e0.txt). Its registry key
// observed only the first dword of entry 0, which both bodies clear, so it could not see that. The exe's Frontend/MenuNavSM.cpp:RecordsZero
// (memset of its own g_records) is now a call into the same body.
// ref: re/analysis/promote_c2_frontend_menus/0x0042d3e0.md
// ---------------------------------------------------------------------------

// 0x0042d3e0
extern "C" __declspec(dllexport) void __cdecl MenuEntryArrayInit() {
    mashed_re::Frontend::MenuEntryZeroBody(reinterpret_cast<std::uint8_t*>(0x00898ac0u), 30);
}

RH_ScopedInstall(MenuEntryArrayInit, 0x0042d3e0);  // re-enabled 2026-05-24 c3-frontend-a; body corrected 2026-10-16 (dualcopy4)
