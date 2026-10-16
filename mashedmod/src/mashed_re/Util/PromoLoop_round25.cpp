// Mashed RE — promote-round round 25 (L5 int2out: two-out-ptr getter).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Included in this file (subsystem per hooks.csv noted per function):
//   (0x0046cbb0 CarStatePairGet removed 2026-10-16, see below; the body is VehicleCarStateRead)
//
// Body byte-verified in original\MASHED.exe.unpatched 2026-06-13. Diffed via
// the new int2out handler.
//
// Analysis: re/analysis/vehicle_damage_d2/0x0046cbb0.md

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// FUN_0046cbb0 (CarStatePairGet, per-car state + secondary getter) was ported here first.
// Its one-body-per-RVA copy is VehicleCarStateRead in Vehicle/VehicleState.cpp (also compiled
// into mashed_re.exe, so exe_file == file). That is the body that installed (link order) when
// the C4 in-race racediff ran (re/analysis/phys_c4_evidence/c4_racediff_result_2026-08-15.json,
// installed by RVA), so the C4 evidence covers it. The duplicate that sat here (CarStatePairGet,
// RH_ScopedInstall 0x0046cbb0) was removed 2026-10-16 (dupinstall2).
// Analysis: re/analysis/vehicle_damage_d2/0x0046cbb0.md
// ---------------------------------------------------------------------------
