// Mashed RE — Frontend batch AA session 6 reimplementations.
// Analysis notes:
//   re/analysis/font_atlas_promote_ae5/0x00556cd0.md
//   re/analysis/frontend_c1_to_c2_s5/FUN_00426d00.md
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Functions in this file:
//   0x00556cd0  GetDat00912a20    — 5-byte getter; returns DAT_00912a20 verbatim
//   0x00426d00  FrontendArraySlotGet — 2D array slot accessor; &base + p2*0xc + p1*0x4c

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// GetDat00912a20  --  0x00556cd0
//
// Original: FUN_00556cd0 (5 bytes, 0x00556cd0..0x00556cd4)
// Signature: undefined4 FUN_00556cd0(void)
//   No parameters.
// Returns: DAT_00912a20 (undefined4) verbatim.
//
// Decompiler output (verbatim from note; pool Mashed_pool2 fallback slot):
//   undefined4 FUN_00556cd0(void) {
//       return DAT_00912a20;
//   }
//
// One caller: 0x00427620 FontText_HudShutdown (C2+).
// No callees.
//
// Constants (cited from body 0x00556cd0..0x00556cd4):
//   0x00912a20 — global read by this getter (data ref at 0x00556cd1).
//
// Uncertainty carried (non-blocking for C3):
//   U-5677: DAT_00912a20 type is undefined4; whether this is a font-object
//           pointer or integer is unconfirmed. Semantics are orthogonal to
//           bit-faithful authoring.
//
// Frida diff strategy: read_global — write sentinel to 0x00912a20, call
//   fn(), compare return value vs sentinel. Both orig and reimpl read the
//   same global and return it, so A/B is bit-identical by construction.
//
// ref: re/analysis/font_atlas_promote_ae5/0x00556cd0.md
// ---------------------------------------------------------------------------

// 0x00556cd0
extern "C" __declspec(dllexport) std::uint32_t __cdecl GetDat00912a20()
{
    // Return DAT_00912a20 verbatim. Address cited at 0x00556cd1.
    return *reinterpret_cast<std::uint32_t*>(0x00912a20u);
}

RH_ScopedInstall(GetDat00912a20, 0x00556cd0);

// ---------------------------------------------------------------------------
// FrontendArraySlotGet  --  0x00426d00
//
// Original: FUN_00426d00 (pure leaf, body at 0x00426d00)
// Signature: undefined* FUN_00426d00(int param_1, int param_2)
//   param_1: row index (stride 0x4c = 76 dword-bytes per row)
//   param_2: column index (stride 0xc = 12 bytes per column)
// Returns: interior pointer into 2D array at DAT_00663670.
//
// Decompiler output (verbatim from note):
//   undefined * FUN_00426d00(int param_1, int param_2)
//   {
//       return &DAT_00663670 + param_2 * 0xc + param_1 * 0x4c;
//   }
//
// No callees (pure leaf).
// Callers (depth-1): 0x004019d0, 0x00407e20, 0x00408b00, 0x00412190, 0x00441820.
//
// Constants (cited from body 0x00426d00):
//   0x00663670 — base of the 2D array (raw address of DAT_00663670)
//   0x4c (76)  — row stride in bytes; param_1 multiplier
//   0x0c (12)  — column stride in bytes; param_2 multiplier
//
// Uncertainty (non-blocking):
//   U-5772: array element type unknown; semantics of the 2D layout not
//           determined. The address-arithmetic is exact as shown.
//
// Frida diff strategy: int_pair — pass [param_1, param_2] directly;
//   return value is the pointer (uint32 on x86), compared as integers.
//   Both paths compute the same address arithmetic against the same base.
//
// ref: re/analysis/frontend_c1_to_c2_s5/FUN_00426d00.md
// ---------------------------------------------------------------------------

// 0x00426d00
extern "C" __declspec(dllexport) std::uint8_t* __cdecl FrontendArraySlotGet(
    int param_1, int param_2)
{
    // &DAT_00663670 + param_2 * 0xc + param_1 * 0x4c
    // Base 0x00663670 cited from body; strides 0x4c (76) and 0xc (12) cited
    // from decomp multipliers.
    return reinterpret_cast<std::uint8_t*>(0x00663670u)
           + param_2 * 0xc
           + param_1 * 0x4c;
}

RH_ScopedInstall(FrontendArraySlotGet, 0x00426d00);
