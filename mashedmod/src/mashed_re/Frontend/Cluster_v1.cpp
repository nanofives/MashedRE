// Mashed RE — Frontend cluster, c3-batch-v session 1.
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Included in this file:
//   0x00474e60  DegToRad          — float deg-to-rad leaf: param_1 * PI/180
//   0x00556cd0  FontAtlasSlotGet  — getter: returns DAT_00912a20 as uint32
//   0x00556cc0  FontAtlasSlotSet  — setter: DAT_00912a20 = param_1
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
// FontAtlasSlotGet  --  0x00556cd0
//
// Original: FUN_00556cd0 (5 bytes, 0x00556cd0..0x00556cd4)
// Signature: undefined4 FUN_00556cd0(void)
// Returns DAT_00912a20 directly (no transformation).
//
// Decompiler output (complete):
//   undefined4 FUN_00556cd0(void) { return DAT_00912a20; }
//
// Disasm at 0x00556cd0 (5 bytes):
//   MOV EAX, [0x00912a20]        ; load DAT_00912a20 (data ref at 0x00556cd1)
//   RET
//
// Constants (cited from function body at 0x00556cd0):
//   0x00912a20 — global read by this getter (data ref at 0x00556cd1)
//
// Pure leaf, no callees. Caller: FontText_HudShutdown (0x00427620, C2).
// [UNCERTAIN U-5677] — see ## Uncertainties in analysis note; non-blocking.
//
// ref: re/analysis/font_atlas_promote_ae5/0x00556cd0.md
// ---------------------------------------------------------------------------

// 0x00556cd0
extern "C" __declspec(dllexport) std::uint32_t __cdecl FontAtlasSlotGet() {
    // Read DAT_00912a20 (data ref at 0x00556cd1).
    return *reinterpret_cast<const std::uint32_t*>(0x00912a20u);
}

RH_ScopedInstall(FontAtlasSlotGet, 0x00556cd0);

// ---------------------------------------------------------------------------
// FontAtlasSlotSet  --  0x00556cc0
//
// Original: FUN_00556cc0 (9 bytes, 0x00556cc0..0x00556cc8)
// Signature: void FUN_00556cc0(undefined4 param_1)
// Writes param_1 into DAT_00912a20.
//
// Decompiler output (complete):
//   void FUN_00556cc0(undefined4 param_1) { DAT_00912a20 = param_1; return; }
//
// Disasm at 0x00556cc0 (9 bytes):
//   MOV EAX, [ESP+4]             ; load param_1
//   MOV [0x00912a20], EAX        ; store to DAT_00912a20 (data ref at 0x00556cc3)
//   RET
//
// Constants (cited from function body at 0x00556cc0):
//   0x00912a20 — global written by this setter (data ref at 0x00556cc3)
//
// Pure leaf, no callees. Caller: FUN_00427ca0 (0x00427ca0, C2).
// [UNCERTAIN U-5676] — see ## Uncertainties in analysis note; non-blocking.
//
// ref: re/analysis/font_atlas_promote_ae5/0x00556cc0.md
// ---------------------------------------------------------------------------

// 0x00556cc0
extern "C" __declspec(dllexport) void __cdecl FontAtlasSlotSet(std::uint32_t param_1) {
    // Write param_1 to DAT_00912a20 (data ref at 0x00556cc3).
    *reinterpret_cast<std::uint32_t*>(0x00912a20u) = param_1;
}

RH_ScopedInstall(FontAtlasSlotSet, 0x00556cc0);
