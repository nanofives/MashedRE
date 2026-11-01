// Mashed RE — Frontend per-player score accessors (s2 cluster).
// Five score-getter functions from the Frontend subsystem.
// All read from per-player data arrays at base 0x00899a40, stride 0x4e dwords.
//
// Array indexing note (confirmed against s3 analysis plate FUN_00423ee0.md):
//   - Per-player score arrays are typed as dword[] in Ghidra.
//     Ghidra shows (&DAT_base)[param_1 * 0x4e], meaning the C++ access is
//     reinterpret_cast<uint32_t*>(base)[param_1 * 0x4e]
//     i.e. byte offset = param_1 * 0x4e * 4 = param_1 * 0x138.
//   - Team id array at 0x007f1a18: stride 0x10 bytes per player (4 dword slots).
//     Ghidra shows (&DAT_007f1a18)[param_1 * 4], meaning
//     reinterpret_cast<int32_t*>(0x007f1a18)[param_1 * 4]
//     i.e. byte offset = param_1 * 4 * 4 = param_1 * 0x10.
//
// Address verification (player stride 0x4e dwords = 0x138 bytes):
//   player 0 @ 0x00899a9c + 0*0x138 = 0x00899a9c  (DAT_00899a9c confirmed)
//   player 1 @ 0x00899a9c + 1*0x138 = 0x00899bd4  (DAT_00899bd4 confirmed)
//   player 2 @ 0x00899a9c + 2*0x138 = 0x00899d0c  (DAT_00899d0c confirmed)
//   player 3 @ 0x00899a9c + 3*0x138 = 0x00899e44  (DAT_00899e44 confirmed)
//
// Callees:
//   GetRaceSubMode  (0x0042f6a0, C3) — returns DAT_0067e9fc; race sub-mode int
//   GetDat0067ea64  (0x0042f500, C4) — returns DAT_0067ea64; team mode flag
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Session: c3-batch-s-s2  2026-05-26
// Analysis: re/analysis/frontend_c1_to_c2_s2/0x00423b40.md
//           re/analysis/frontend_c1_to_c2_s2/0x00423b60.md
//           re/analysis/frontend_c1_to_c2_s2/0x00423ba0.md
//           re/analysis/frontend_c1_to_c2_s2/0x00423bc0.md
//           re/analysis/frontend_c1_to_c2_s2/0x00423c40.md

#include "../Core/HookSystem.h"

#include <cstdint>

// Forward declarations of callees (originals are still live; we call them by RVA).
// GetRaceSubMode  @ 0x0042f6a0  C3 — returns race sub-mode int (DAT_0067e9fc)
// GetDat0067ea64  @ 0x0042f500  C4 — returns team mode flag (DAT_0067ea64)

extern "C" std::int32_t __cdecl GetRaceSubMode();   // 0x0042f6a0
extern "C" std::int32_t __cdecl GetDat0067ea64();   // 0x0042f500

// MATCH-S05: the body of 0x00423b40 (PlayerScoreAccA) moved to Match/MatchS05_00423b40.cpp,
// which compiles the byte-matched source re/match/s05/00423b40.c.

// MATCH-S05: the body of 0x00423b60 (PlayerScoreAccB) moved to Match/MatchS05_00423b60.cpp,
// which compiles the byte-matched source re/match/s05/00423b60.c.

// MATCH-S05: the body of 0x00423ba0 (PlayerScoreAccD) moved to Match/MatchS05_00423ba0.cpp,
// which compiles the byte-matched source re/match/s05/00423ba0.c.

// MATCH-S05: the body of 0x00423bc0 (PlayerScoreTeamAccC) moved to Match/MatchS05_00423bc0.cpp,
// which compiles the byte-matched source re/match/s05/00423bc0.c.

// MATCH-S05: the body of 0x00423c40 (PlayerScoreTeamAccBase) moved to Match/MatchS05_00423c40.cpp,
// which compiles the byte-matched source re/match/s05/00423c40.c.
