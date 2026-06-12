// Mashed RE — promote-round round 2 (L0: c3_batch_race1 session-2 set).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Included in this file (subsystem per hooks.csv noted per function):
//   0x00442410  CameraSlotFieldPtrGet — camera; pointer-return field getter
//   0x00442420  CameraSlotFloatGet    — camera; float-field getter (same record array)
//   0x00423b20  CarSnapshotDwordGet   — vehicle; per-car snapshot dword getter
//   0x00426cc0  VehicleTable4cPtrGet  — vehicle; pointer-return table getter
//   0x00442df0  RaceFloat898980Get    — vehicle; collision-impact float getter
//
// All five bodies byte-verified in original\MASHED.exe.unpatched 2026-06-12
// (file offset = RVA - 0x400000); the two x87 returns are FLD (true float
// loads), not FILD — see per-function byte cites below.
//
// Verification lane: scenario:'race' diffs (re/analysis/scenario_attach_lane.md).
//
// Analysis:
//   re/analysis/bucket_powerups_camera_particle_0044d5e0_004b4140/0x00442410.md
//   re/analysis/bucket_powerups_camera_particle_0044d5e0_004b4140/0x00442420.md
//   re/analysis/promote_c2_vehicle_lowrva/0x00423b20.md
//   re/analysis/promote_c2_vehicle_lowrva/0x00426cc0.md
//   re/analysis/promote_c2_vehicle_lowrva/0x00442df0.md

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// CameraSlotFieldPtrGet  --  0x00442410   (subsystem: camera)
//
// Original: FUN_00442410 (15 bytes, 0x00442410..0x0044241e)
// Bytes: 8B 44 24 04 / 69 C0 D8 00 00 00 / 05 FC 64 89 00 / C3
//   mov eax,[esp+4]; imul eax,eax,0xd8; add eax,0x008964fc; ret
// Signature: undefined * FUN_00442410(int param_1)
//
// POINTER-RETURN getter: returns the ORIGINAL-image VA; never dereferences.
//
// Constants (cited from function body at 0x00442410):
//   0x008964fc — field base (= 0x00896460 + 0x9c) in the 5-slot camera/view
//                record array driven per-frame by FUN_00448dc0
//   0xd8       — per-slot record stride (216 bytes)
//
// Callers: FUN_00448dc0, FUN_0044cb00.
//
// Uncertainties (non-blocking): semantic of the +0x9c field (data-semantic,
// reported as raw offset in the note).
// ---------------------------------------------------------------------------

// 0x00442410
extern "C" __declspec(dllexport) std::uint32_t __cdecl CameraSlotFieldPtrGet(int param_1) {
    // 0x008964fc base + 0xd8 stride cited at 0x00442410 body.
    return 0x008964fcu + static_cast<std::uint32_t>(param_1) * 0xd8u;
}

RH_ScopedInstall(CameraSlotFieldPtrGet, 0x00442410);

// ---------------------------------------------------------------------------
// CameraSlotFloatGet  --  0x00442420   (subsystem: camera)
//
// Original: FUN_00442420 (16 bytes, 0x00442420..0x0044242f)
// Bytes: 8B 44 24 04 / 69 C0 D8 00 00 00 / D9 80 70 65 89 00 / C3
//   mov eax,[esp+4]; imul eax,eax,0xd8; FLD dword ptr [eax+0x00896570]; ret
//   (D9 80 = FLD m32fp — a TRUE float load, not FILD)
// Signature: float10 FUN_00442420(int param_1)
//
// DEREFERENCES: keep the index in 0..4 (5-slot record array per the note).
//
// Constants (cited from function body at 0x00442420):
//   0x00896570 — float-field base (= 0x00896460 + 0x110) of the same per-slot
//                camera/view record array as CameraSlotFieldPtrGet
//   0xd8       — per-slot record stride
//
// Callers: FUN_00448dc0, FUN_0044cb00.
//
// Uncertainties (non-blocking): semantic of the +0x110 float (data-semantic).
// ---------------------------------------------------------------------------

// 0x00442420
extern "C" __declspec(dllexport) float __cdecl CameraSlotFloatGet(int param_1) {
    // 0x00896570 base + 0xd8 stride cited at 0x00442420 body.
    return *reinterpret_cast<const float*>(
        0x00896570u + static_cast<std::uint32_t>(param_1) * 0xd8u);
}

RH_ScopedInstall(CameraSlotFloatGet, 0x00442420);

// ---------------------------------------------------------------------------
// CarSnapshotDwordGet  --  0x00423b20   (subsystem: vehicle)
//
// Original: FUN_00423b20 (16 bytes, 0x00423b20..0x00423b2f)
// Bytes: 8B 44 24 04 / 69 C0 38 01 00 00 / 8B 80 EC 95 89 00 / C3
//   mov eax,[esp+4]; imul eax,eax,0x138; mov eax,[eax+0x008995ec]; ret
// Signature: undefined4 FUN_00423b20(int param_1)
//
// DEREFERENCES: keep the index in 0..3 (4-car race; per-car snapshot stride
// 0x138 per STRUCTS.md §4 PerCarSnapshot).
//
// Constants (cited from function body at 0x00423b20):
//   0x008995ec — table base (dword at offset 0x12c within the per-car
//                snapshot whose base is 0x008994c0)
//   0x138      — per-car byte stride (decomp shows [param_1 * 0x4e] over a
//                4-byte element type; 0x4e * 4 = 0x138)
//
// Caller: FUN_00410d10 case-9 (checks per-car-1 dword).
//
// Uncertainties (non-blocking):
//   U-2175: meaning of the dword at +0x12c (data-semantic).
//   U-2176: writer not statically xref'd (indirected via stride alias).
// ---------------------------------------------------------------------------

// 0x00423b20
extern "C" __declspec(dllexport) std::uint32_t __cdecl CarSnapshotDwordGet(int param_1) {
    // 0x008995ec base + 0x138 byte stride cited at 0x00423b20 body.
    return *reinterpret_cast<const std::uint32_t*>(
        0x008995ecu + static_cast<std::uint32_t>(param_1) * 0x138u);
}

RH_ScopedInstall(CarSnapshotDwordGet, 0x00423b20);

// ---------------------------------------------------------------------------
// VehicleTable4cPtrGet  --  0x00426cc0   (subsystem: vehicle)
//
// Original: FUN_00426cc0 (13 bytes, 0x00426cc0..0x00426ccc)
// Bytes: 8B 44 24 04 / 6B C0 4C / 05 58 36 66 00 / C3
//   mov eax,[esp+4]; imul eax,eax,0x4c; add eax,0x00663658; ret
// Signature: undefined * FUN_00426cc0(int param_1)
//
// POINTER-RETURN getter: returns the ORIGINAL-image VA; never dereferences.
// Sibling of FUN_00426cb0 (base 0x00663664 = this base + 0xc, same 0x4c stride).
//
// Constants (cited from function body at 0x00426cc0):
//   0x00663658 — table base
//   0x4c       — per-entry stride (76 bytes)
// ---------------------------------------------------------------------------

// 0x00426cc0
extern "C" __declspec(dllexport) std::uint32_t __cdecl VehicleTable4cPtrGet(int param_1) {
    // 0x00663658 base + 0x4c stride cited at 0x00426cc0 body.
    return 0x00663658u + static_cast<std::uint32_t>(param_1) * 0x4cu;
}

RH_ScopedInstall(VehicleTable4cPtrGet, 0x00426cc0);

// ---------------------------------------------------------------------------
// RaceFloat898980Get  --  0x00442df0   (subsystem: vehicle)
//
// Original: FUN_00442df0 (7 bytes, 0x00442df0..0x00442df6)
// Bytes: D9 05 80 89 89 00 / C3
//   FLD dword ptr [0x00898980]; ret   (D9 05 = FLD m32fp — TRUE float load,
//   byte-verified after round 1's FILD lesson on 0x00429300)
// Signature: float10 FUN_00442df0(void)
//
// Constants (cited from function body at 0x00442df0):
//   0x00898980 — global float; caller FUN_00410d10 compares it against
//                _DAT_005cc55c (10.0f per existing notes) as the
//                collision-pair-resolution sentinel
//
// Uncertainties (non-blocking):
//   U-1854: writer of DAT_00898980 not statically xref'd (data-semantic).
// ---------------------------------------------------------------------------

// 0x00442df0
extern "C" __declspec(dllexport) float __cdecl RaceFloat898980Get(void) {
    // FLD [0x00898980] cited at 0x00442df0 (instruction bytes above).
    return *reinterpret_cast<const float*>(0x00898980u);
}

RH_ScopedInstall(RaceFloat898980Get, 0x00442df0);
