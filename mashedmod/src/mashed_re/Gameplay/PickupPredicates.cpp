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

// 0x004576b0  particle — byte-verified: 33 C0 8B 0C 85 00 A3 68 00 85 C9 75 25 40 83 F8 02 7C EF 83 F8 04 7D 17 C1 E0 04 05 54 A2 68 00 83 38 00 75 0D 83 C0 10 3D 94 A2 68 00 7C F1 33 C0 C3 B8 01 00 00 00 C3
//   "any active?" over 4 slots (0x68a300,0x68a304 then 0x68a274,0x68a284) -> return 1 if any nonzero
//   Moved unchanged from Util/PromoLoop_sessionB.cpp. The exe copy PK_4576b0 read the same absolute addresses
//   (the exe maps 0x00500000..0x009fffff), so both targets read what each one's naked copy read.
extern "C" __declspec(dllexport) std::uint32_t __cdecl AnyActive4576b0(void) {
    for (std::uint32_t i = 0; i < 2; i++)
        if (*reinterpret_cast<std::int32_t*>(0x0068a300u + i * 4u)) return 1u;
    for (std::uint32_t p = 0x0068a274u; p < 0x0068a294u; p += 0x10u)
        if (*reinterpret_cast<std::int32_t*>(p)) return 1u;
    return 0u;
}
RH_ScopedInstall(AnyActive4576b0, 0x004576b0);

// MATCH-S10: the body of 0x0045c330 (PredNot45c330) moved to Match/MatchS10_0045c330.cpp,
// which compiles the byte-matched source re/match/s10/0045c330.c.
