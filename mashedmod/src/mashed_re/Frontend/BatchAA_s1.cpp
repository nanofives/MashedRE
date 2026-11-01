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

// 0x0041de80  FUN_0041de80 — record in ESI, one stack arg; called from inline asm
//             in HudModeGuardDispatch (C cannot load ESI).

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

// 0x00479100  FUN_00479100  (C2) — raycast + vertex-color sample on collision mesh; returns 0/1 in EAX
// (0x00479150 xor eax,eax / 0x004792ec, 0x00479304, 0x0047931a mov eax,1).
typedef std::uint32_t (__cdecl *PFN_FUN_00479100)(void* self, std::uint32_t p1, std::uint32_t p2, std::uint32_t p3);
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
// Bytes (re-ported 2026-10-12):
//   0x0041ded0 mov eax,[0x0063d5e8]; test eax,eax; je ret
//   0x0041ded9 mov eax,[esp+4]; push esi; push eax
//   0x0041dedf mov esi,0x0063d5a0          <- FUN_0041de80 reads its record via ESI
//   0x0041dee4 call 0x0041de80; add esp,4; pop esi; ret
// FUN_0041de80 takes the record in ESI and param_1 on the stack
// (0x0041dea8 mov eax,[esp+0x18] after five pushes). The old port called it as
// a plain cdecl and never loaded ESI.
extern "C" __declspec(dllexport)
void __cdecl HudModeGuardDispatch(std::uint32_t param_1) {
    if (*g_HudEnableFlag != 0) {    // DAT_0063d5e8 @ 0x0041ded0
        __asm {
            push esi
            push param_1
            mov  esi, 0x0063d5a0
            mov  eax, 0x0041de80
            call eax
            add  esp, 4
            pop  esi
        }
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

// MATCH-S07: the body of 0x0042bde0 (HudRectEmitter5) moved to Match/MatchS07_0042bde0.cpp,
// which compiles the byte-matched source re/match/s07/0042bde0.c.

// ---------------------------------------------------------------------------
// 0x00426dc0  FUN_00426dc0
// void FUN_00426dc0(undefined4 param_1, undefined4 param_2, undefined4 param_3)
// 3-arg forwarder: prepends &DAT_00646e58 and calls FUN_00479100. Listing 0x00426dc0..0x00426ddc:
// push [esp+0xc]; push [esp+8]; push [esp+4]; push 0x646e58; call 0x479100; add esp,0x10; ret -- EAX is
// FUN_00479100's return, which the sole caller tests (0x0041f630 test eax,eax). bworld 2026-10-12: the export
// returned void and EAX survived only by codegen; it now returns the callee's value.
// DAT_00646e58 (0x00646e58): global struct/object; passed by address as arg 0.
// Decomp (verbatim): FUN_00479100(&DAT_00646e58, param_1, param_2, param_3); return;
// Source: re/analysis/frontend_c1_to_c2_s5/FUN_00426dc0.md
// ---------------------------------------------------------------------------
// 0x00426dc0
extern "C" __declspec(dllexport)
std::uint32_t __cdecl FrontendRaycastForward(std::uint32_t param_1, std::uint32_t param_2, std::uint32_t param_3) {
    // 0x00426dc0: return FUN_00479100(&DAT_00646e58, param_1, param_2, param_3)
    void* self = reinterpret_cast<void*>(0x00646e58u);  // &DAT_00646e58 @ 0x00646e58
    return g_FUN_00479100(self, param_1, param_2, param_3);
}
RH_ScopedInstall(FrontendRaycastForward, 0x00426dc0);
