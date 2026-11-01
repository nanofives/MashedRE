// Mashed RE — Frontend race-end gate flag reimplementations.
// Analysis notes: re/analysis/promote_c2_frontend_menus/
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Callee 0x0042f6a0 (GetRaceSubMode) is C3 — gate satisfied.

#include "../Core/HookSystem.h"

#include <cstdint>

// 0x0042f6a0 — GetRaceSubMode; returns DAT_0067e9fc (race sub-mode).
// C3, impl in mashedmod/src/mashed_re/Util/GameStateGetters.cpp.
extern "C" std::uint32_t __cdecl GetRaceSubMode();

// ---------------------------------------------------------------------------
// RaceEndFlagIfEndMode  --  0x0042fe30
//
// Original: FUN_0042fe30 (20 bytes, 0x0042fe30..0x0042fe44)
// Signature: undefined4 FUN_0042fe30(void)
//
// Returns 1 when GetRaceSubMode() == 0xb (race end condition).
// Otherwise returns DAT_0067ea74.
//
// Logic (cited from 0x0042fe30..0x0042fe44):
//   iVar1 = FUN_0042f6a0();   // 0x0042fe33 — call GetRaceSubMode
//   uVar2 = 1;
//   if (iVar1 != 0xb) uVar2 = DAT_0067ea74;   // 0x0042fe3c
//   return uVar2;
//
// ref: re/analysis/promote_c2_frontend_menus/0x0042fe30.md
// ---------------------------------------------------------------------------

// MATCH-S07: the body of 0x0042fe30 (RaceEndFlagIfEndMode) moved to Match/MatchS07_0042fe30.cpp,
// which compiles the byte-matched source re/match/s07/0042fe30.c.

// ---------------------------------------------------------------------------
// RaceEndAltFlagIfEndMode  --  0x0042fe50
//
// Original: FUN_0042fe50 (20 bytes, 0x0042fe50..0x0042fe64)
// Signature: uint FUN_0042fe50(void)
//
// Complement of RaceEndFlagIfEndMode (0x0042fe30):
//   that function returns 1 when mode==0xb;
//   this one returns DAT_0067ea78 when mode!=0xb, else 0.
//
// Logic (cited from 0x0042fe50..0x0042fe64):
//   iVar1 = FUN_0042f6a0();
//   return -(uint)(iVar1 != 0xb) & DAT_0067ea78;
//
// The mask -(uint)(cond) evaluates to 0xFFFFFFFF when cond is true (mode!=0xb),
// and 0x00000000 when cond is false (mode==0xb). This reproduces the original
// MSVC branchless AND pattern exactly.
//
// ref: re/analysis/promote_c2_frontend_menus/0x0042fe50.md
// ---------------------------------------------------------------------------

// MATCH-S07: the body of 0x0042fe50 (RaceEndAltFlagIfEndMode) moved to Match/MatchS07_0042fe50.cpp,
// which compiles the byte-matched source re/match/s07/0042fe50.c.
