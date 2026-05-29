// Mashed RE — Batch AA s1: mixed frontend/HUD hooks.
//
// Functions authored here:
//   0x00426cf0  FUN_00426cf0     (frontend) — pure address-of-global accessor
//   0x0041ded0  FUN_0041ded0     (hud)      — guarded HUD draw dispatcher
//   0x00552840  FontCtx_SetRotation (hud)   — Z-axis rotation on font context matrix
//   0x0042bde0  FUN_0042bde0     (frontend) — 5-call HUD-rect emitter
//   0x00426dc0  FUN_00426dc0     (frontend) — 3-arg forwarder to FUN_00479100
//
// Analysis notes:
//   re/analysis/frontend_c1_to_c2_s5/FUN_00426cf0.md
//   re/analysis/hud_ingame_promote_c2/0x0041ded0.md
//   re/analysis/hud_promote_c2_b/0x00552840.md
//   re/analysis/frontend_c1_to_c2_s6/FUN_0042bde0.md
//   re/analysis/frontend_c1_to_c2_s5/FUN_00426dc0.md
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Session: c3-batch-aa-s1  2026-05-29

#include "../Core/HookSystem.h"

#include <cstdint>
#include <cstring>  // for memcpy (float-bit casts)

// ---------------------------------------------------------------------------
// Forward declarations of callees (by RVA; originals are still live)
// ---------------------------------------------------------------------------

// 0x0041de80  FUN_0041de80  (C2) — HUD mode dispatcher; ESI=this; vtable[0x48] dispatches
typedef void (__cdecl *PFN_HudModeDispatch)(std::uint32_t param_1);
static const PFN_HudModeDispatch g_FUN_0041de80 =
    reinterpret_cast<PFN_HudModeDispatch>(0x0041de80u);

// 0x004c4d20  RwMatrix_SetRotAxisAngle  (C2) — normalizes axis; sin/cos(deg*pi/180);
//             calls FUN_004c4a50 (Rodrigues rotation); preconcat mode 1.
typedef void (__cdecl *PFN_RwMatSetRotAxisAngle)(void* mat, float* axis, float angle, int mode);
static const PFN_RwMatSetRotAxisAngle g_RwMatrix_SetRotAxisAngle =
    reinterpret_cast<PFN_RwMatSetRotAxisAngle>(0x004c4d20u);

// 0x00472c60  ChromeBaseDraw  (C3) — RwIm2D filled-quad draw; args: (float x, float y, float w, float h, uint32 argb)
// Declared extern "C" in DrawQuadPrimitives.cpp — call by pointer to avoid duplicate export.
typedef void (__cdecl *PFN_ChromeBaseDraw)(float x, float y, float w, float h, std::uint32_t argb);
static const PFN_ChromeBaseDraw g_ChromeBaseDraw =
    reinterpret_cast<PFN_ChromeBaseDraw>(0x00472c60u);

// 0x00479100  FUN_00479100  (C2) — raycast + vertex-color sample on collision mesh
typedef void (__cdecl *PFN_FUN_00479100)(void* self, std::uint32_t p1, std::uint32_t p2, std::uint32_t p3);
static const PFN_FUN_00479100 g_FUN_00479100 =
    reinterpret_cast<PFN_FUN_00479100>(0x00479100u);

// ---------------------------------------------------------------------------
// Font context globals  (cited from re/analysis/hud_promote_c2_b/0x00552840.md)
// ---------------------------------------------------------------------------

// DAT_00912b04: current font context stack depth index (uint32, cited 0x00552859)
static std::uint32_t* const g_FontCtxDepth =
    reinterpret_cast<std::uint32_t*>(0x00912b04u);

// DAT_00912a84: font context pointer array base; indexed by [*g_FontCtxDepth]
static void** const g_FontCtxPtrs =
    reinterpret_cast<void**>(0x00912a84u);

// DAT_00912bd8: dirty flag A (zeroed at 0x00552872)
static std::uint32_t* const g_DirtyBd8 =
    reinterpret_cast<std::uint32_t*>(0x00912bd8u);

// DAT_00912bec: dirty flag B (zeroed at 0x0055287c)
static std::uint32_t* const g_DirtyBec =
    reinterpret_cast<std::uint32_t*>(0x00912becu);

// ---------------------------------------------------------------------------
// FUN_0042bde0 globals  (cited from re/analysis/frontend_c1_to_c2_s6/FUN_0042bde0.md)
// ---------------------------------------------------------------------------

// _DAT_005cc35c: global float; addend used in call 3: _DAT_005cc35c + 534.0f
static float* const g_Dat005cc35c = reinterpret_cast<float*>(0x005cc35cu);

// _DAT_005cc574: global float; subtracted in call 4: 26.0f - _DAT_005cc574
static float* const g_Dat005cc574 = reinterpret_cast<float*>(0x005cc574u);

// _DAT_005cd784: global float; used in call 5: _DAT_005cd784 + 534.0f  (x arg)
static float* const g_Dat005cd784 = reinterpret_cast<float*>(0x005cd784u);

// ---------------------------------------------------------------------------
// Helper: reinterpret uint32 float-bits as float (avoids UB vs *(float*)&i)
// ---------------------------------------------------------------------------
static inline float u32_to_float(std::uint32_t bits) {
    float f;
    std::memcpy(&f, &bits, sizeof f);
    return f;
}

// ---------------------------------------------------------------------------
// 0x00426cf0  FUN_00426cf0
// undefined4 * FUN_00426cf0(void)
// Pure leaf: returns address of global DAT_0066d6e4.
// No branches. No callees. No side effects.
// Decomp (verbatim): return &DAT_0066d6e4;
// Source: re/analysis/frontend_c1_to_c2_s5/FUN_00426cf0.md
// ---------------------------------------------------------------------------
// 0x00426cf0
extern "C" __declspec(dllexport)
std::uint32_t* __cdecl GetDat0066d6e4(void) {
    // 0x00426cf0: return &DAT_0066d6e4
    return reinterpret_cast<std::uint32_t*>(0x0066d6e4u);
}
RH_ScopedInstall(GetDat0066d6e4, 0x00426cf0);

// ---------------------------------------------------------------------------
// 0x0041ded0  FUN_0041ded0
// void FUN_0041ded0(undefined4 param_1)
// Guard: if (DAT_0063d5e8 != 0) call FUN_0041de80(param_1); else return.
// param_1: passed through unchanged to FUN_0041de80.
//   Callers pass 1 (game-modes 4/7/9) or 0 (game-mode 8).
// Decomp (verbatim): if (DAT_0063d5e8 != 0) { FUN_0041de80(param_1); }
// Source: re/analysis/hud_ingame_promote_c2/0x0041ded0.md
// ---------------------------------------------------------------------------
// DAT_0063d5e8: enable-flag for game-modes 4/7/8/9 HUD draw path (int32)
static std::int32_t* const g_HudEnableFlag =
    reinterpret_cast<std::int32_t*>(0x0063d5e8u);

// 0x0041ded0
extern "C" __declspec(dllexport)
void __cdecl HudModeGuardDispatch(std::uint32_t param_1) {
    // 0x0041ded0: if (DAT_0063d5e8 != 0) call FUN_0041de80(param_1)
    if (*g_HudEnableFlag != 0) {    // DAT_0063d5e8 @ 0x0063d5e8
        g_FUN_0041de80(param_1);    // FUN_0041de80 @ 0x0041de80
    }
}
RH_ScopedInstall(HudModeGuardDispatch, 0x0041ded0);

// ---------------------------------------------------------------------------
// 0x00552840  FontCtx_SetRotation
// undefined4 FUN_00552840(undefined4 param_1)
// Applies a Z-axis rotation to the current font context matrix.
// Steps (addresses cited from re/analysis/hud_promote_c2_b/0x00552840.md):
//   0x00552840: stack-allocate axis vector {0, 0, 1.0f} (Z axis)
//     local_c = 0 (axis.x = 0.0f)
//     local_8 = 0 (axis.y = 0.0f)
//     local_4 = 0x3f800000 (axis.z = 1.0f)
//   0x00552859: FUN_004c4d20(ctx, axis, param_1, 1) — preconcat mode
//     ctx = (&DAT_00912a84)[DAT_00912b04]
//   0x00552872: DAT_00912bd8 = 0
//   0x0055287c: DAT_00912bec = 0
//   0x00552885: return 1
// ---------------------------------------------------------------------------
// 0x00552840
extern "C" __declspec(dllexport)
std::uint32_t __cdecl FontCtx_SetRotation(float param_1) {
    // 0x00552840: Z-axis unit vector on stack
    float axis[3];
    axis[0] = 0.0f;                     // local_c = 0x00000000
    axis[1] = 0.0f;                     // local_8 = 0x00000000
    axis[2] = 1.0f;                     // local_4 = 0x3f800000

    // 0x00552859: RwMatrix_SetRotAxisAngle(ctx, {0,0,1}, param_1, preconcat=1)
    void* ctx = g_FontCtxPtrs[*g_FontCtxDepth];  // (&DAT_00912a84)[DAT_00912b04]
    g_RwMatrix_SetRotAxisAngle(ctx, axis, param_1, 1);

    // 0x00552872: dirty flag reset
    *g_DirtyBd8 = 0u;                   // DAT_00912bd8 = 0

    // 0x0055287c: secondary dirty flag reset
    *g_DirtyBec = 0u;                   // DAT_00912bec = 0

    // 0x00552885: return 1
    return 1u;
}
RH_ScopedInstall(FontCtx_SetRotation, 0x00552840);

// ---------------------------------------------------------------------------
// 0x0042bde0  FUN_0042bde0
// void FUN_0042bde0(int param_1, byte param_2)
// 5-call HUD-rect emitter via ChromeBaseDraw (FUN_00472c60).
// param_1: base row index; fVar1 = (float)(param_1 - 13) used as base-Y.
// param_2: colour/alpha value; used to build packed ARGB arguments.
//
// Constants (IEEE-754):
//   0x42400000 = 48.0f    0x44058000 = 534.0f   0x41d00000 = 26.0f
//   0x42380000 = 46.0f    0x40000000 = 2.0f
//   CONCAT13(a, b): low 3 bytes = b, high byte = a.
//     argb1 = CONCAT13(param_2 >> 1, 0x146ef0)  = (uint32)(param_2>>1)<<24 | 0x146ef0
//     argb2 = CONCAT13(param_2,      0x1050b4)  = (uint32)(param_2)<<24    | 0x1050b4
//
// Decomp verbatim (shape-decomp-injected 2026-05-29):
//   fVar1 = (float)(param_1 + -0xd);
//   param_1 = CONCAT13(param_2 >> 1, 0x146ef0);
//   FUN_00472c60(0x42400000, fVar1, 0x44058000, 0x41d00000, param_1);
//   param_1 = CONCAT13(param_2, 0x1050b4);
//   FUN_00472c60(0x42380000, fVar1, 0x40000000, 0x41d00000, param_1);
//   fVar2 = _DAT_005cc35c + 534.0;
//   FUN_00472c60(0x42380000, fVar1, fVar2, 0x40000000, param_1);
//   fVar3 = 26.0 - _DAT_005cc574;
//   fVar1 = fVar3 + fVar1;
//   FUN_00472c60(0x42380000, fVar1, fVar2, 0x40000000, param_1);
//   FUN_00472c60(_DAT_005cd784 + 534.0, fVar1 - fVar3, 0x40000000, 0x41d00000, param_1);
//
// NOTE on first args: Ghidra shows them as hex uint32 constants (0x42400000 etc.)
// because the undefined4 parameter type is used. ChromeBaseDraw (0x00472c60)
// takes (float, float, float, float, uint32). The constants are IEEE-754 float
// bit patterns; at the call site they are pushed as dwords and interpreted as
// floats by the callee. u32_to_float() is used to form the correct float value.
// Source: re/analysis/frontend_c1_to_c2_s6/FUN_0042bde0.md
// ---------------------------------------------------------------------------
// 0x0042bde0
extern "C" __declspec(dllexport)
void __cdecl HudRectEmitter5(std::int32_t param_1, std::uint8_t param_2) {
    float fVar1, fVar2, fVar3;

    // fVar1 = (float)(param_1 + -0xd)  i.e. (float)(param_1 - 13)
    fVar1 = static_cast<float>(param_1 + (-0xd));

    // argb1 = CONCAT13(param_2 >> 1, 0x146ef0)
    std::uint32_t argb1 = (static_cast<std::uint32_t>(param_2 >> 1u) << 24u) | 0x00146ef0u;

    // Call 1: FUN_00472c60(0x42400000=48.0f, fVar1, 0x44058000=534.0f, 0x41d00000=26.0f, argb1)
    g_ChromeBaseDraw(u32_to_float(0x42400000u), fVar1,
                     u32_to_float(0x44058000u), u32_to_float(0x41d00000u), argb1);

    // argb2 = CONCAT13(param_2, 0x1050b4)
    std::uint32_t argb2 = (static_cast<std::uint32_t>(param_2) << 24u) | 0x001050b4u;

    // Call 2: FUN_00472c60(0x42380000=46.0f, fVar1, 0x40000000=2.0f, 0x41d00000=26.0f, argb2)
    g_ChromeBaseDraw(u32_to_float(0x42380000u), fVar1,
                     u32_to_float(0x40000000u), u32_to_float(0x41d00000u), argb2);

    // fVar2 = _DAT_005cc35c + 534.0f
    fVar2 = *g_Dat005cc35c + 534.0f;

    // Call 3: FUN_00472c60(0x42380000=46.0f, fVar1, fVar2, 0x40000000=2.0f, argb2)
    g_ChromeBaseDraw(u32_to_float(0x42380000u), fVar1, fVar2,
                     u32_to_float(0x40000000u), argb2);

    // fVar3 = 26.0f - _DAT_005cc574
    fVar3 = 26.0f - *g_Dat005cc574;

    // fVar1 = fVar3 + fVar1
    fVar1 = fVar3 + fVar1;

    // Call 4: FUN_00472c60(0x42380000=46.0f, fVar1, fVar2, 0x40000000=2.0f, argb2)
    g_ChromeBaseDraw(u32_to_float(0x42380000u), fVar1, fVar2,
                     u32_to_float(0x40000000u), argb2);

    // Call 5: FUN_00472c60(_DAT_005cd784 + 534.0f, fVar1 - fVar3, 0x40000000=2.0f, 0x41d00000=26.0f, argb2)
    g_ChromeBaseDraw(*g_Dat005cd784 + 534.0f, fVar1 - fVar3,
                     u32_to_float(0x40000000u), u32_to_float(0x41d00000u), argb2);
}
RH_ScopedInstall(HudRectEmitter5, 0x0042bde0);

// ---------------------------------------------------------------------------
// 0x00426dc0  FUN_00426dc0
// void FUN_00426dc0(undefined4 param_1, undefined4 param_2, undefined4 param_3)
// 3-arg forwarder: prepends &DAT_00646e58 and calls FUN_00479100.
// DAT_00646e58 (0x00646e58): global struct/object; passed by address as arg 0.
// Decomp (verbatim): FUN_00479100(&DAT_00646e58, param_1, param_2, param_3); return;
// Source: re/analysis/frontend_c1_to_c2_s5/FUN_00426dc0.md
// ---------------------------------------------------------------------------
// 0x00426dc0
extern "C" __declspec(dllexport)
void __cdecl FrontendRaycastForward(std::uint32_t param_1, std::uint32_t param_2, std::uint32_t param_3) {
    // 0x00426dc0: FUN_00479100(&DAT_00646e58, param_1, param_2, param_3)
    void* self = reinterpret_cast<void*>(0x00646e58u);  // &DAT_00646e58 @ 0x00646e58
    g_FUN_00479100(self, param_1, param_2, param_3);
}
RH_ScopedInstall(FrontendRaycastForward, 0x00426dc0);
