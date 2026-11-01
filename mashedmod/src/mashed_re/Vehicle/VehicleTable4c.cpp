// Mashed RE — FUN_00426cc0 table-pointer getter: ONE body for both build targets (D4-DC-INC-pathprog, 2026-11-01).
//
// Before this file the RVA had two bodies, the dual-copy class (re/analysis/DUAL_COPY_AUDIT_2026-09-29.md,
// ROADMAP D4):
//   Vehicle/PromoLoop_round2.cpp               .asi-only, installed, the body hooks.csv's C4 evidence covers
//   PP_426cc0 (Race/PathProgStep_naked.inc)    exe-only, private verbatim naked copy, called by FUN_00408610's
//                                              transcription and by Race/PathProgDot.cpp's exe binding
// This TU is in BOTH exe_sources.rsp and asi_sources.rsp. The body moved here unchanged; the exe's naked caller
// now `call`s it (re/tools/naked_specs/pathprog_p2.json) and Dot408590's exe binding points at it.
// RH_ScopedInstall installs it in the .asi and is a no-op in the exe.
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Analysis: re/analysis/promote_c2_vehicle_lowrva/0x00426cc0.md

#include "../Core/HookSystem.h"

#include <cstdint>

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
