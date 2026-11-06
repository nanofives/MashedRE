// Mashed RE — two pickup-lane predicates: ONE body each for both build targets (D4-DC-INC-batch3, 2026-11-01).
//
// Before this file each RVA had two bodies, the dual-copy class (ROADMAP D4, re/tools/dual_copy_allowlist.txt):
//   Util/PromoLoop_sessionB.cpp                          .asi-only, installed, the body hooks.csv's evidence covers
//   PK_4576b0 (Gameplay/PickupTick_naked.inc)            exe-only, private verbatim naked copy called by PK_459000
//   PK_45c330 (Gameplay/PickupCollect_naked.inc)         exe-only, private verbatim naked copy called by PK_458920
// This TU is in BOTH exe_sources.rsp and asi_sources.rsp. The bodies moved here. RH_ScopedInstall installs them in
// the .asi and is a no-op in the exe. The exe's naked callers now `call` them (re/tools/naked_specs/pickups_m2a.json,
// pickups_m3.json).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)

#include "../Core/HookSystem.h"
#include "PickupPredicates.h"

#include <cstdint>

// MATCH-S09: the body of 0x004576b0 (AnyActive4576b0) moved to Match/MatchS09_004576b0.cpp,
// which compiles the byte-matched source re/match/s09/004576b0.c.

// MATCH-S10: the body of 0x0045c330 (PredNot45c330) moved to Match/MatchS10_0045c330.cpp,
// which compiles the byte-matched source re/match/s10/0045c330.c.
