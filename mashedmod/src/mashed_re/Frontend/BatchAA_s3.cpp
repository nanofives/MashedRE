// Mashed RE - Frontend/HUD hooks batch AA session 3.
//
// Candidates authored:
//   0x00427c90  GetDat0067d84c       — pure global getter (4-byte), no callees
//   0x00424100  TeamBlockZeroGet     — team-aggregating block-0 getter (int_scalar)
//   0x00556cc0  SetDat00912a20       — void(uint32) pure setter to DAT_00912a20
//
// Deferred:
//   0x00426d20  FUN_00426d20         — callee 0x004c15c0 at C1 (anti-island)
//   0x004a2b60  FUN_004a2b60         — variadic vsprintf-style; Frida NativeFunction
//                                      cannot express variadic
//   0x00427680  FUN_00427680         — ESI register out-param; trig_text_draw
//                                      requires live D3D state; DEFER
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Analysis notes:
//   re/analysis/frontend_c1_to_c2_s5/FUN_00427c90.md
//   re/analysis/frontend_c1_to_c2_s3/FUN_00424100.md
//   re/analysis/font_atlas_promote_ae5/0x00556cc0.md

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// GetDat0067d84c  --  0x00427c90
//
// Original: FUN_00427c90 (0x00427c90, 5 bytes)
// Signature: undefined4 FUN_00427c90(void)
// Body:
//   return DAT_0067d84c;
//
// Pure leaf getter: reads 4-byte global at 0x0067d84c, returns it.
// No callees, no branches, no side-effects.
// Callers: FUN_00428a30 (0x00428a30), FUN_00428bf0 (0x00428bf0).
// ref: re/analysis/frontend_c1_to_c2_s5/FUN_00427c90.md
// ---------------------------------------------------------------------------

// 0x00427c90
extern "C" __declspec(dllexport) std::uint32_t __cdecl GetDat0067d84c() {
    // DAT_0067d84c — read at 0x00427c90 per decompiler. [ref 0x00427c90]
    return *reinterpret_cast<const std::uint32_t*>(0x0067d84cu);
}

RH_ScopedInstall(GetDat0067d84c, 0x00427c90);

// ---------------------------------------------------------------------------
// TeamBlockZeroGet  --  0x00424100
//
// Original: FUN_00424100 (0x00424100..0x004241ab)
// Signature: int FUN_00424100(int param_1)
//   param_1: player slot index 0..3
//   return: int — block-0 stat field for param_1, plus team partner contributions
//           when team mode is active (partners with active-slot != -1 only)
//
// Control flow (from analysis note):
//   1. Call FUN_0042f500(); if 0: return (&DAT_008999a0)[param_1 * 0x4e] directly.
//   2. Load team-ID of param_1: (&DAT_007f1a18)[param_1 * 4] [0x00424113]
//   3. Seed iVar2 = (&DAT_008999a0)[param_1 * 0x4e] [0x0042411c]
//   4. For each partner slot k in {0,1,2,3}:
//      - Only add if: (team-ID of k == iVar1) AND (k != param_1)
//                     AND (active-slot of k != -1)
//   5. Return iVar2.
//
// Constants (cited from analysis note):
//   0x008999a0 — block-0 root, stride 0x4e int32s per player [0x0042411c]
//   0x007f1a18 — team-ID array for player 0; stride 4 bytes per player [0x00424113]
//   0x007f1a14 — active-slot for player 0 [0x00424125]
//   0x007f1a24 — active-slot for player 1 [0x00424136]
//   0x007f1a34 — active-slot for player 2 [0x00424147]
//   0x007f1a44 — active-slot for player 3 [0x00424158]
//   0xFFFFFFFF (-1) — sentinel for inactive/absent player
//
// Callee: FUN_0042f500 (0x0042f500) — team-mode flag getter; C4.
// Caller: FUN_0042c960 (0x0042c960) — C2.
// ref: re/analysis/frontend_c1_to_c2_s3/FUN_00424100.md
// ---------------------------------------------------------------------------

// Team-mode flag getter 0x0042f500 is our ported C4 export GetDat0067ea64
// (GameState/StateAccessors.cpp) -- call the port directly rather than
// trampolining through the original image.
extern "C" std::uint32_t __cdecl GetDat0067ea64();   // 0x0042f500

// MATCH-S05: the body of 0x00424100 (TeamBlockZeroGet) moved to Match/MatchS05_00424100.cpp,
// which compiles the byte-matched source re/match/s05/00424100.c.

// ---------------------------------------------------------------------------
// SetDat00912a20  --  0x00556cc0
//
// Original: FUN_00556cc0 (0x00556cc0, 9 bytes)
// Signature: void FUN_00556cc0(undefined4 param_1)
// Body:
//   DAT_00912a20 = param_1;
//   return;
//
// Pure leaf setter: stores param_1 into 4-byte global at 0x00912a20.
// No callees, no branches. One caller: FUN_00427ca0 (0x00427ca0).
// ref: re/analysis/font_atlas_promote_ae5/0x00556cc0.md
// ---------------------------------------------------------------------------

// 0x00556cc0
extern "C" __declspec(dllexport) void __cdecl SetDat00912a20(std::uint32_t param_1)
{
    // DAT_00912a20 — global font/style pointer; written at 0x00556cc3. [0x00556cc0]
    *reinterpret_cast<std::uint32_t*>(0x00912a20u) = param_1;
}

RH_ScopedInstall(SetDat00912a20, 0x00556cc0);
