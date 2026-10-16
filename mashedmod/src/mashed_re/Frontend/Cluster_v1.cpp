// Mashed RE — Frontend cluster, c3-batch-v session 1.
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Included in this file:
//   0x00474e60  DegToRad          — float deg-to-rad leaf: param_1 * PI/180
//   (FontAtlasSlotGet 0x00556cd0 / FontAtlasSlotSet 0x00556cc0 removed 2026-10-16, see below)
//
// Analysis notes:
//   re/analysis/font_pools_frontend_ae6/0x00474e60.md
//   re/analysis/font_atlas_promote_ae5/0x00556cd0.md
//   re/analysis/font_atlas_promote_ae5/0x00556cc0.md

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// DegToRad  --  0x00474e60
//
// Original: FUN_00474e60 (10 bytes, 0x00474e60..0x00474e69)
// Signature: float10 FUN_00474e60(float param_1)
//   param_1: angle in degrees (float32)
// Returns param_1 * _DAT_005cd7a8 as x87 float10 extended precision.
//
// Disasm at 0x00474e60 (10 bytes):
//   FLDS  [ESP+4]                ; push param_1 onto x87 stack
//   FMULS [0x005cd7a8]           ; multiply by _DAT_005cd7a8 (PI/180)
//   RET
//
// Constants (cited from function body at 0x00474e60):
//   0x005cd7a8 — address of PI/180 constant (read at 0x00474e62)
//   _DAT_005cd7a8 = 0x3C8EFA35 = 0.0174533f (confirmed; U-4309 RESOLVED in hooks.csv)
//
// Pure leaf, no callees. Caller: FUN_00434720 (0x00434720, C2).
//
// ref: re/analysis/font_pools_frontend_ae6/0x00474e60.md
// ---------------------------------------------------------------------------

// Constant read at 0x00474e62: _DAT_005cd7a8 = 0x3C8EFA35 = PI/180.
static constexpr std::uintptr_t kDegToRadConstAddr = 0x005cd7a8;

// 0x00474e60
extern "C" __declspec(dllexport) float __cdecl DegToRad(float param_1) {
    // Read PI/180 directly from the game's data address so the constant is
    // identical to what the original x87 FMULS instruction reads.
    const float kPiOver180 = *reinterpret_cast<const float*>(kDegToRadConstAddr);
    return param_1 * kPiOver180;
}

RH_ScopedInstall(DegToRad, 0x00474e60);

// ---------------------------------------------------------------------------
// FUN_00556cd0 (getter) and FUN_00556cc0 (setter) of DAT_00912a20 were ported here first as
// FontAtlasSlotGet / FontAtlasSlotSet. Their one-body-per-RVA copies are GetDat00912a20
// (Frontend/BatchAA_s6.cpp) and SetDat00912a20 (Frontend/BatchAA_s3.cpp), the bodies
// hooks.csv's evidence cites (U-5677 / U-5676 stay filed against those rows). The duplicates
// that sat here were removed 2026-10-16 (dupinstall).
// ref: re/analysis/font_atlas_promote_ae5/0x00556cd0.md, 0x00556cc0.md
// ---------------------------------------------------------------------------
