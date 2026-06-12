// Mashed RE — promote-round round 3 (L1 race-lane pair + L2 cheap re-earns).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Included in this file (subsystem per hooks.csv noted per function):
//   0x0042fe70  VehicleDword67ea80Get — vehicle; timeout-selector global getter (L1)
//   0x0041ea80  TrackDescField40Get   — ai; track-descriptor +0x40 getter (L1)
//   0x004cbc60  RwGlobal7d4598Set     — render; 9B setter (L2 re-earn)
//   0x004cbc70  RwGlobal7d4598Get     — render; 5B getter (L2 re-earn)
//   0x004cbc80  RwGlobal7d459cSet     — render; 9B setter (L2 re-earn)
//
// All five bodies byte-verified in original\MASHED.exe.unpatched 2026-06-12
// (file offset = RVA - 0x400000); cites in the per-function headers.
//
// L2 context: the render trio were C3/C4 under loader-broken-era evidence and
// demoted to C2 in re-validation BECAUSE no installed reimpl existed
// (C2_GATE_AUDIT demoted-needs-reimpl). This file is the missing reimpl;
// their diffs are standard menu-attach (state-independent: the harness
// seeds/saves/restores the globals).
//
// Analysis:
//   re/analysis/promote_c2_vehicle_lowrva/0x0042fe70.md
//   re/analysis/ai_update_d6/0x0041ea80.md
//   re/analysis/skeleton_prep_render/004cbc60.md
//   re/analysis/skeleton_prep_render/004cbc70.md
//   re/analysis/skeleton_prep_render/004cbc80.md

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// VehicleDword67ea80Get  --  0x0042fe70   (subsystem: vehicle)
//
// Original: FUN_0042fe70 (6 bytes, 0x0042fe70..0x0042fe75)
// Bytes: A1 80 EA 67 00 / C3   (mov eax,[0x0067ea80]; ret)
// Signature: undefined4 FUN_0042fe70(void)
//
// Constants (cited from function body at 0x0042fe70):
//   0x0067ea80 — global dword; existing notes: selects timeout values
//                (900k/1800k/3600k) for DAT_007f1018
//
// Caller: FUN_00470c70 (VEHICLE_UPDATE_FN, C2).
//
// Uncertainties (non-blocking):
//   U-0397: writer/meaning of DAT_0067ea80 (data-semantic).
// ---------------------------------------------------------------------------

// 0x0042fe70
extern "C" __declspec(dllexport) std::uint32_t __cdecl VehicleDword67ea80Get(void) {
    // 0x0067ea80 cited at 0x0042fe70 body.
    return *reinterpret_cast<const std::uint32_t*>(0x0067ea80u);
}

RH_ScopedInstall(VehicleDword67ea80Get, 0x0042fe70);

// ---------------------------------------------------------------------------
// TrackDescField40Get  --  0x0041ea80   (subsystem: ai)
//
// Original: FUN_0041ea80 (9 bytes, 0x0041ea80..0x0041ea88)
// Bytes: A1 E4 D7 63 00 / 8B 40 40 / C3
//   (mov eax,[0x0063d7e4]; mov eax,[eax+0x40]; ret)
// Signature: undefined4 FUN_0041ea80(void)
//
// DEREFERENCES DAT_0063d7e4 (track-descriptor dispatch object) — NULL until
// a track is loaded, so this diff MUST run on the race lane.
//
// Constants (cited from function body at 0x0041ea80):
//   0x0063d7e4 — track-descriptor dispatch object pointer
//   0x40       — field offset checked as lap-line gate by caller FUN_00426c90
//
// Caller: FUN_00426c90 (lap-line conditional, C2).
// ---------------------------------------------------------------------------

// 0x0041ea80
extern "C" __declspec(dllexport) std::uint32_t __cdecl TrackDescField40Get(void) {
    // 0x0063d7e4 base + 0x40 offset cited at 0x0041ea80 body.
    const std::uint8_t* obj = *reinterpret_cast<std::uint8_t* const*>(0x0063d7e4u);
    return *reinterpret_cast<const std::uint32_t*>(obj + 0x40u);
}

RH_ScopedInstall(TrackDescField40Get, 0x0041ea80);

// ---------------------------------------------------------------------------
// RwGlobal7d4598Set  --  0x004cbc60   (subsystem: render)
//
// Original: FUN_004cbc60 (10 bytes, 0x004cbc60..0x004cbc69)
// Bytes: 8B 44 24 04 / A3 98 45 7D 00 / C3
//   (mov eax,[esp+4]; mov [0x007d4598],eax; ret)
// Signature: void FUN_004cbc60(undefined4 param_1)
//
// Constants (cited from function body at 0x004cbc60):
//   0x007d4598 — destination global dword
//
// Caller: FUN_00493710 (RW_INIT_FN, C2). No uncertainties (trivial leaf).
// ---------------------------------------------------------------------------

// 0x004cbc60
extern "C" __declspec(dllexport) void __cdecl RwGlobal7d4598Set(std::uint32_t param_1) {
    // 0x007d4598 cited at 0x004cbc60 body.
    *reinterpret_cast<std::uint32_t*>(0x007d4598u) = param_1;
}

RH_ScopedInstall(RwGlobal7d4598Set, 0x004cbc60);

// ---------------------------------------------------------------------------
// RwGlobal7d4598Get  --  0x004cbc70   (subsystem: render)
//
// Original: FUN_004cbc70 (6 bytes, 0x004cbc70..0x004cbc75)
// Bytes: A1 98 45 7D 00 / C3   (mov eax,[0x007d4598]; ret)
// Signature: undefined4 FUN_004cbc70(void)
//
// Getter counterpart of RwGlobal7d4598Set (same global).
//
// Constants (cited from function body at 0x004cbc70):
//   0x007d4598 — source global dword
//
// Caller: FUN_00493710 (RW_INIT_FN, C2). No uncertainties (trivial leaf).
// ---------------------------------------------------------------------------

// 0x004cbc70
extern "C" __declspec(dllexport) std::uint32_t __cdecl RwGlobal7d4598Get(void) {
    // 0x007d4598 cited at 0x004cbc70 body.
    return *reinterpret_cast<const std::uint32_t*>(0x007d4598u);
}

RH_ScopedInstall(RwGlobal7d4598Get, 0x004cbc70);

// ---------------------------------------------------------------------------
// RwGlobal7d459cSet  --  0x004cbc80   (subsystem: render)
//
// Original: FUN_004cbc80 (10 bytes, 0x004cbc80..0x004cbc89)
// Bytes: 8B 44 24 04 / A3 9C 45 7D 00 / C3
//   (mov eax,[esp+4]; mov [0x007d459c],eax; ret)
// Signature: void FUN_004cbc80(undefined4 param_1)
//
// Constants (cited from function body at 0x004cbc80):
//   0x007d459c — destination global dword (= 0x007d4598 + 4, adjacent to the
//                RwGlobal7d4598Set target)
//
// Caller: FUN_00493710 (RW_INIT_FN, C2). No uncertainties (trivial leaf).
// ---------------------------------------------------------------------------

// 0x004cbc80
extern "C" __declspec(dllexport) void __cdecl RwGlobal7d459cSet(std::uint32_t param_1) {
    // 0x007d459c cited at 0x004cbc80 body.
    *reinterpret_cast<std::uint32_t*>(0x007d459cu) = param_1;
}

RH_ScopedInstall(RwGlobal7d459cSet, 0x004cbc80);
