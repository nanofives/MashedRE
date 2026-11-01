// Mashed RE — Frontend menu-leaf reimplementations (c3-batch-s session 3).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Functions in this file:
//   0x00423ee0  PlayerBlock2Field00Get    — team-aggregating getter, block-2+0x00, no gate
//   0x00423f60  PlayerBlock2Field04Get    — team-aggregating getter, block-2+0x04, RaceSubMode gate
//   0x00423ff0  PlayerBlock2Field50Get    — team-aggregating getter, block-2+0x50, no gate
//   0x00424070  PlayerBlock2Field08Get    — team-aggregating getter, block-2+0x08, RaceSubMode gate
//   0x004241b0  GetDat008994c0            — 5-byte leaf; returns DAT_008994c0
//
// Analysis notes:
//   re/analysis/frontend_c1_to_c2_s3/FUN_00423ee0.md
//   re/analysis/frontend_c1_to_c2_s3/FUN_00423f60.md
//   re/analysis/frontend_c1_to_c2_s3/FUN_00423ff0.md
//   re/analysis/frontend_c1_to_c2_s3/FUN_00424070.md
//   re/analysis/frontend_c1_to_c2_s3/FUN_004241b0.md

#include "../Core/HookSystem.h"

#include <cstdint>

// MATCH-S05: the callee declarations, constants and the TeamAggregate / Block2Field helpers that
// served the five bodies below were removed with them (no other user in this TU).

// ---------------------------------------------------------------------------
// PlayerBlock2Field00Get  --  0x00423ee0
//
// Original: FUN_00423ee0 (0x00423ee0..0x00423f5e)
// Signature: int FUN_00423ee0(int param_1)
//   param_1: player index 0..3
// Returns: sum of block-2 field +0x00 for param_1's slot and all team-mates
//          when teams are enabled; raw field value otherwise.
//
// Algorithm (cited from 0x00423ee0 body):
//   1. Call GetDat0067ea64(); if 0 → return (&DAT_00899f20)[param_1*0x4e]. [0x00423ef8/04]
//   2. Load iVar1 = team-ID of param_1 from *(0x007f1a18 + param_1*0x10). [0x00423eef/f2]
//   3. Seed iVar2 = (&DAT_00899f20)[param_1*0x4e]. [0x00423f28 area]
//   4. For k in {0,1,2,3}: if team-ID[k]==iVar1 and k!=param_1:
//        iVar2 += (&DAT_00899f20)[k*0x4e]. [0x00423f06/18/2b/3e]
//   5. Return iVar2.
//
// Constants cited:
//   0x00899f20 — block-2 array root                        [0x00423f04]
//   0x4e       — dword stride per slot                     [0x00423f04]
//   0x007f1a18 — team-ID array root, 0x10-byte stride     [0x00423ef2]
//
// ref: re/analysis/frontend_c1_to_c2_s3/FUN_00423ee0.md
// ---------------------------------------------------------------------------

// MATCH-S05: the body of 0x00423ee0 (PlayerBlock2Field00Get) moved to Match/MatchS05_00423ee0.cpp,
// which compiles the byte-matched source re/match/s05/00423ee0.c.

// ---------------------------------------------------------------------------
// PlayerBlock2Field04Get  --  0x00423f60
//
// Original: FUN_00423f60 (0x00423f60..0x00423feb)
// Signature: int FUN_00423f60(int param_1)
//   param_1: player index 0..3
// Returns: aggregated block-2+0x04 field; 0 if RaceSubMode==4.
//
// Algorithm (cited from 0x00423f60 body):
//   1. Call GetRaceSubMode(); if == 4 → return 0. [0x00423f6c]
//   2. Call GetDat0067ea64(); if 0 → return (&DAT_00899f24)[param_1*0x4e]. [0x00423f78/85]
//   3. Team-aggregate as per PlayerBlock2Field00Get over field dword offset 1 (+0x04). [0x00423f85..]
//
// Constants cited:
//   4          — RaceSubMode early-out sentinel          [0x00423f6c]
//   0x00899f24 — block-2+0x04 root (DAT_00899f20+4)     [0x00423f85]
//   0x4e       — dword stride per slot                   [0x00423f85]
//
// ref: re/analysis/frontend_c1_to_c2_s3/FUN_00423f60.md
// ---------------------------------------------------------------------------

// MATCH-S05: the body of 0x00423f60 (PlayerBlock2Field04Get) moved to Match/MatchS05_00423f60.cpp,
// which compiles the byte-matched source re/match/s05/00423f60.c.

// ---------------------------------------------------------------------------
// PlayerBlock2Field50Get  --  0x00423ff0
//
// Original: FUN_00423ff0 (0x00423ff0..0x0042406e)
// Signature: int FUN_00423ff0(int param_1)
//   param_1: player index 0..3
// Returns: aggregated block-2+0x50 field; no RaceSubMode gate.
//
// Algorithm (cited from 0x00423ff0 body):
//   1. Call GetDat0067ea64(); if 0 → return (&DAT_00899f70)[param_1*0x4e]. [0x00424002]
//   2. Team-aggregate over field dword offset 0x14 (+0x50 from block-2 root). [0x00424002..]
//
// Constants cited:
//   0x00899f70 — block-2+0x50 root (DAT_00899f20 + 0x50)  [0x00424002]
//     field dword offset = 0x50 / 4 = 0x14 = 20
//   0x4e       — dword stride per slot                     [0x00424002]
//
// ref: re/analysis/frontend_c1_to_c2_s3/FUN_00423ff0.md
// ---------------------------------------------------------------------------

// MATCH-S05: the body of 0x00423ff0 (PlayerBlock2Field50Get) moved to Match/MatchS05_00423ff0.cpp,
// which compiles the byte-matched source re/match/s05/00423ff0.c.

// ---------------------------------------------------------------------------
// PlayerBlock2Field08Get  --  0x00424070
//
// Original: FUN_00424070 (0x00424070..0x004240fb)
// Signature: int FUN_00424070(int param_1)
//   param_1: player index 0..3
// Returns: aggregated block-2+0x08 field; 0 if RaceSubMode==4.
//
// Algorithm (cited from 0x00424070 body):
//   1. Call GetRaceSubMode(); if == 4 → return 0. [gate at 0x00424070 area]
//   2. Call GetDat0067ea64(); if 0 → return (&DAT_00899f28)[param_1*0x4e].
//   3. Team-aggregate over field dword offset 2 (+0x08 from block-2 root).
//
// Constants cited:
//   4          — RaceSubMode early-out sentinel
//   0x00899f28 — block-2+0x08 root (DAT_00899f20 + 0x08)
//     field dword offset = 0x08 / 4 = 2
//   0x4e       — dword stride per slot
//
// ref: re/analysis/frontend_c1_to_c2_s3/FUN_00424070.md
// ---------------------------------------------------------------------------

// MATCH-S05: the body of 0x00424070 (PlayerBlock2Field08Get) moved to Match/MatchS05_00424070.cpp,
// which compiles the byte-matched source re/match/s05/00424070.c.

// ---------------------------------------------------------------------------
// GetDat008994c0  --  0x004241b0
//
// Original: FUN_004241b0 (0x004241b0..0x004241b5; 6 bytes including RET)
// Signature: undefined4 FUN_004241b0(void)
//   no parameters
// Returns: *(uint32*)0x008994c0  [cited at 0x004241b1]
//
// Algorithm:
//   Single instruction: MOV EAX, [0x008994c0]; RET.
//   No branches, no calls, no writes.
//
// Constants cited:
//   0x008994c0 — sole memory access address  [0x004241b1]
//
// ref: re/analysis/frontend_c1_to_c2_s3/FUN_004241b0.md
// ---------------------------------------------------------------------------

// MATCH-S05: the body of 0x004241b0 (GetDat008994c0) moved to Match/MatchS05_004241b0.cpp,
// which compiles the byte-matched source re/match/s05/004241b0.c.
