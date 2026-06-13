// Mashed RE — promote-round round 11 (L3 looser-curation leaves).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Included in this file (subsystem per hooks.csv noted per function):
//   0x004cae90  RwCapsBlockPtrGet        — render; constant-pointer getter
//   0x0042f520  ViewportBlockPtrGet      — render; constant-pointer getter
//   0x00485370  DynamicObjectListGetBase — vehicle; constant-pointer getter
//   0x00405420  ReplayCursorReset        — util; zero-store setter
//
// DROPPED from this round: 0x00402f40 (5B getter of DAT_00636ad8) — no
// identified caller at C2+ (plate says "xrefs via Ghidra"); the C3 caller
// gate cannot be satisfied without a Ghidra xref pass.
//
// All bodies byte-verified in original\MASHED.exe.unpatched 2026-06-12
// (file offset = RVA - 0x400000); cites in the per-function headers.
//
// Analysis:
//   re/analysis/render_4_c1_to_c2_s5/FUN_004cae90.md
//   re/analysis/render_c1_to_c2_s2/FUN_0042f520.md
//   re/analysis/bucket_vehicle_004820e0_00485420/00485370.md
//   re/analysis/bucket_util_00095280_0040e460/00405420.md

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// RwCapsBlockPtrGet  --  0x004cae90   (subsystem: render)
//
// Original: FUN_004cae90 (6 bytes, 0x004cae90..0x004cae95)
// Bytes: B8 20 82 61 00 / C3   (mov eax,0x00618220; ret)
// Signature: undefined4 * FUN_004cae90(void)
//
// POINTER-RETURN: constant ORIGINAL-image VA of the D3D9 caps block
// (0xe dwords, filled by FUN_004c7a70 case 0x4 per the note).
//
// Constants (cited from function body at 0x004cae90):
//   0x00618220 — caps-block address
//
// Caller: FUN_004c30b0 (C2; checks non-null to gate execution).
// ---------------------------------------------------------------------------

// 0x004cae90
extern "C" __declspec(dllexport) std::uint32_t __cdecl RwCapsBlockPtrGet(void) {
    // 0x00618220 cited at 0x004cae90 body (mov eax, imm32).
    return 0x00618220u;
}

RH_ScopedInstall(RwCapsBlockPtrGet, 0x004cae90);

// ---------------------------------------------------------------------------
// ViewportBlockPtrGet  --  0x0042f520   (subsystem: render)
//
// Original: FUN_0042f520 (6 bytes, 0x0042f520..0x0042f525)
// Bytes: B8 50 91 89 00 / C3   (mov eax,0x00899150; ret)
// Signature: undefined * FUN_0042f520(void)
//
// Constants (cited from function body at 0x0042f520):
//   0x00899150 — viewport/render data block address (paired setter 0x0042f530)
//
// Caller: FUN_004430c0 (frontend, C2).
//
// Uncertainties (non-blocking):
//   U-4344: block layout (data-semantic).
// ---------------------------------------------------------------------------

// 0x0042f520
extern "C" __declspec(dllexport) std::uint32_t __cdecl ViewportBlockPtrGet(void) {
    // 0x00899150 cited at 0x0042f520 body.
    return 0x00899150u;
}

RH_ScopedInstall(ViewportBlockPtrGet, 0x0042f520);

// ---------------------------------------------------------------------------
// DynamicObjectListGetBase  --  0x00485370   (subsystem: vehicle)
//
// Original: FUN_00485370 (6 bytes, 0x00485370..0x00485375)
// Bytes: B8 B8 87 6E 00 / C3   (mov eax,0x006e87b8; ret)
// Signature: undefined * FUN_00485370(void)
//
// Constants (cited from function body at 0x00485370):
//   0x006e87b8 — dynamic-object geometry list base (count getter sibling
//                0x00485360; caller iterates at stride 0x90)
//
// Caller: FUN_004694e0 VehicleObjectContactSolver (C2).
//
// Uncertainties (non-blocking):
//   U-7556: entry struct layout (data-semantic; prior U-3575).
// ---------------------------------------------------------------------------

// 0x00485370
extern "C" __declspec(dllexport) std::uint32_t __cdecl DynamicObjectListGetBase(void) {
    // 0x006e87b8 cited at 0x00485370 body.
    return 0x006e87b8u;
}

RH_ScopedInstall(DynamicObjectListGetBase, 0x00485370);

// ---------------------------------------------------------------------------
// ReplayCursorReset  --  0x00405420   (subsystem: util)
//
// Original: FUN_00405420 (11 bytes, 0x00405420..0x0040542a)
// Bytes: C7 05 74 9D 63 00 00 00 00 00 / C3
//   (mov dword ptr [0x00639d74],0; ret)
// Signature: void FUN_00405420(void)
//
// Constants (cited from function body at 0x00405420):
//   0x00639d74 — playback/replay cursor global (advanced by FUN_00405460)
//   0          — value written
//
// Caller: FUN_0040de10 (util, C2 — "reset the playback/replay cursor" per
// its plate).
// ---------------------------------------------------------------------------

// 0x00405420
extern "C" __declspec(dllexport) void __cdecl ReplayCursorReset(void) {
    // 0x00639d74 := 0 cited at 0x00405420 body.
    *reinterpret_cast<std::uint32_t*>(0x00639d74u) = 0u;
}

RH_ScopedInstall(ReplayCursorReset, 0x00405420);
