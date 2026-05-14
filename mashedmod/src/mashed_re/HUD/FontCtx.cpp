// Mashed RE — Font context / font render-state reimplementations.
// Analysis notes: re/analysis/hud_promote_c2_b/
// Subsystem: HUD (font matrix stack + render-state init)
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Callee drift-promotes (C1 with analysis, not yet in hooks.csv at C2+):
//   0x004c57a0  FontCtxMatrix_AllocInit  — allocs identity-matrix slot via vtable+0x118
//   0x004c4dc0  RwMatrixInvert           — 3-path invert (identity/orthonormal/general cofactor)

#include "../Core/HookSystem.h"

#include <cstdint>
#include <cstring>

// ---------------------------------------------------------------------------
// Forward declarations for RW callees that are C2+ or drift-promoted.
// These RVAs are confirmed from analysis notes (see cited .md files).
// ---------------------------------------------------------------------------

// 0x004c5010  RwMatrixScale  (C3 — RwMatrixScale.cpp)
// 0x004c51a0  RwMatrixTranslate  (C2, re/analysis/font_text_d3/)
// 0x004c57a0  FontCtxMatrix_AllocInit  (C1 drift-promote, re/analysis/font_text_d2/)
// 0x004c4600  RwMatrixMultiply  (C1 drift-promote, re/analysis/render_pipeline_d3/)
// 0x004c4dc0  RwMatrixInvert  (C1 drift-promote, re/analysis/render_pipeline_d3/)
// 0x004c0ed0  FUN_004c0ed0 camera matrix accessor  (C1, re/analysis/render_lighting_alt/)
// 0x00552750  FontCtx_ResetTransform  (C2, re/analysis/hud_promote_c2_b/0x00552750.md — s5)

typedef void* (__cdecl *PFN_AllocInit)();
typedef void  (__cdecl *PFN_RwMatrixScale)(float* mat, const float* scale, int mode);
typedef void  (__cdecl *PFN_RwMatrixTranslate)(float* mat, const float* vec, int mode);
typedef void* (__cdecl *PFN_RwMatrixMultiply)(void* dst, const void* a, const void* b);
typedef void* (__cdecl *PFN_RwMatrixInvert)(float* dst, const float* src);
typedef void* (__cdecl *PFN_CameraMatrix)(void* cam_node);
typedef void  (__cdecl *PFN_ResetTransform)();

static const PFN_AllocInit        g_AllocInit       = reinterpret_cast<PFN_AllocInit>      (0x004c57a0u); // drift-promote C1→C2 (font_text_d2-20260503.md)
static const PFN_RwMatrixScale    g_RwMatrixScale   = reinterpret_cast<PFN_RwMatrixScale>  (0x004c5010u);
static const PFN_RwMatrixTranslate g_RwMatrixTranslate = reinterpret_cast<PFN_RwMatrixTranslate>(0x004c51a0u);
static const PFN_RwMatrixMultiply g_RwMatrixMultiply= reinterpret_cast<PFN_RwMatrixMultiply>(0x004c4600u);
static const PFN_RwMatrixInvert   g_RwMatrixInvert  = reinterpret_cast<PFN_RwMatrixInvert> (0x004c4dc0u);
static const PFN_CameraMatrix     g_CameraMatrix    = reinterpret_cast<PFN_CameraMatrix>   (0x004c0ed0u); // STUB S-2126
static const PFN_ResetTransform   g_ResetTransform  = reinterpret_cast<PFN_ResetTransform> (0x00552750u);

// ---------------------------------------------------------------------------
// Font context global layout  (from analysis notes, all addresses Ghidra-cited)
// ---------------------------------------------------------------------------

// Font context pointer array: float*[32]  at 0x00912a84..0x00912b03
//   [0] = DAT_00912a84 (slot 0 ptr)
//   [1..30] = DAT_00912a88..DAT_00912b00 (slots 1..30 ptrs)
// DAT_00912a80 = one dword BEFORE slot[0] — used as array-base-1 in Push
// DAT_00912b04 = uint32 stack depth index
static std::uint32_t* const g_FontCtxDepth =
    reinterpret_cast<std::uint32_t*>(0x00912b04u);
// Note: the ptr array is indexed as (&DAT_00912a84)[depth]; base ptr below:
static void**          const g_FontCtxPtrs =
    reinterpret_cast<void**>(0x00912a84u);
// The dword immediately before slot[0] — for source-ptr calc in Push:
static void**          const g_FontCtxPtrBase1 =
    reinterpret_cast<void**>(0x00912a80u);

// Char-glyph identity table: short[256]  at 0x00912500
static std::int16_t*   const g_CharGlyphTable =
    reinterpret_cast<std::int16_t*>(0x00912500u);

// Render-state flags and float globals  (all addresses Ghidra-cited in note)
static std::uint32_t*  const g_FlagsA50  = reinterpret_cast<std::uint32_t*>(0x00912a50u);
static float*          const g_Scale_be0 = reinterpret_cast<float*>(0x00912be0u);
static std::uint32_t*  const g_DirtyBd8  = reinterpret_cast<std::uint32_t*>(0x00912bd8u);
static std::uint32_t*  const g_DirtyBec  = reinterpret_cast<std::uint32_t*>(0x00912becu);
static float*          const g_Be4       = reinterpret_cast<float*>(0x00912be4u);
static std::uint32_t*  const g_FlagBf0   = reinterpret_cast<std::uint32_t*>(0x00912bf0u);

// Global 3x3 render-state region (diagonals = 1.0f, rest = 0.0f)
// Addresses cited from note: +0x44, +0x58, +0x6c = diagonal; rest zeros
// Base struct starts at 0x00912a00 effectively; offsets are absolute:
static float* const g_A44 = reinterpret_cast<float*>(0x00912a44u);
static float* const g_A48 = reinterpret_cast<float*>(0x00912a48u);
static float* const g_A4c = reinterpret_cast<float*>(0x00912a4cu);
static float* const g_A54 = reinterpret_cast<float*>(0x00912a54u);
static float* const g_A58 = reinterpret_cast<float*>(0x00912a58u);
static float* const g_A5c = reinterpret_cast<float*>(0x00912a5cu);
static float* const g_A64 = reinterpret_cast<float*>(0x00912a64u);
static float* const g_A68 = reinterpret_cast<float*>(0x00912a68u);
static float* const g_A6c = reinterpret_cast<float*>(0x00912a6cu);
static float* const g_A74 = reinterpret_cast<float*>(0x00912a74u);
static float* const g_A78 = reinterpret_cast<float*>(0x00912a78u);
static float* const g_A7c = reinterpret_cast<float*>(0x00912a7cu);

// Camera + aspect ratio globals
static void**         const g_ActiveCamera = reinterpret_cast<void**>(0x00912c0cu);
static float*         const g_AspBf8       = reinterpret_cast<float*>(0x00912bf8u);
static float*         const g_AspC00       = reinterpret_cast<float*>(0x00912c00u);
static float*         const g_AspC04       = reinterpret_cast<float*>(0x00912c04u);
static std::uint32_t* const g_Unk3204      = reinterpret_cast<std::uint32_t*>(0x00913204u);
static std::uint32_t* const g_Unk3208      = reinterpret_cast<std::uint32_t*>(0x00913208u);

// FlushMatrix globals
static float*         const g_FontScale    = reinterpret_cast<float*>(0x00912be0u);  // same as g_Scale_be0
static float*         const g_ExtraXform   = reinterpret_cast<float*>(0x00912a44u);  // same as g_A44
static float*         const g_ViewW        = reinterpret_cast<float*>(0x00912b08u);
static float*         const g_ViewH        = reinterpret_cast<float*>(0x00912b0cu);
static const float*   const g_HalfScale    = reinterpret_cast<const float*>(0x005cc32cu);
static float*         const g_CachedMat    = reinterpret_cast<float*>(0x00912b18u); // 64-byte cached compose
static float*         const g_InvMat       = reinterpret_cast<float*>(0x00912b58u); // 64-byte inverted
static float*         const g_OutputMat    = reinterpret_cast<float*>(0x00912b98u); // 64-byte per-frame output

// ---------------------------------------------------------------------------
// FontCtx_SetScale  --  0x00552da0
//
// Original: FUN_00552da0 (72 bytes, 0x00552da0–0x00552de8)
// Stack {sx, sy, 1.0f}; call RwMatrixScale(ctx, vec, 1=preconcat); zero flags.
// Returns 1 always.
// ref: re/analysis/hud_promote_c2_b/0x00552da0.md
// ---------------------------------------------------------------------------

// 0x00552da0
extern "C" __declspec(dllexport)
std::uint32_t __cdecl FontCtx_SetScale(float sx, float sy)
{
    // 0x00552da0: build 3-float scale vector on stack
    float scale[3];
    scale[0] = sx;                  // local_c = param_1 (sx)
    scale[1] = sy;                  // local_8 = param_2 (sy)
    scale[2] = 1.0f;                // local_4 = 0x3f800000 (sz = identity in Z)

    // 0x00552db8: RwMatrixScale(ctx, {sx,sy,1.0f}, rwCOMBINEPRECONCAT=1)
    float* ctx = reinterpret_cast<float*>(g_FontCtxPtrs[*g_FontCtxDepth]);
    g_RwMatrixScale(ctx, scale, 1);

    // 0x00552dce: DAT_00912bd8 = 0  (dirty flag reset)
    *g_DirtyBd8 = 0u;
    // 0x00552dd8: DAT_00912bec = 0
    *g_DirtyBec = 0u;

    // 0x00552de4: return 1
    return 1u;
}

RH_ScopedInstall(FontCtx_SetScale, 0x00552da0);

// ---------------------------------------------------------------------------
// FontCtx_SetTranslation  --  0x00552df0
//
// Original: FUN_00552df0 (72 bytes, 0x00552df0–0x00552e38)
// Stack {x, y, 0.0f}; call RwMatrixTranslate(ctx, vec, 1=preconcat); zero flags.
// Returns 1 always.
// ref: re/analysis/hud_promote_c2_b/0x00552df0.md
// ---------------------------------------------------------------------------

// 0x00552df0
extern "C" __declspec(dllexport)
std::uint32_t __cdecl FontCtx_SetTranslation(float x, float y)
{
    // 0x00552df0: build 3-float translation vector on stack
    float vec[3];
    vec[0] = x;                     // local_c = param_1 (x)
    vec[1] = y;                     // local_8 = param_2 (y)
    vec[2] = 0.0f;                  // local_4 = 0 (z = 0.0f — 2D, no Z offset)

    // 0x00552e08: RwMatrixTranslate(ctx, {x,y,0.0f}, rwCOMBINEPRECONCAT=1)
    float* ctx = reinterpret_cast<float*>(g_FontCtxPtrs[*g_FontCtxDepth]);
    g_RwMatrixTranslate(ctx, vec, 1);

    // 0x00552e1e: DAT_00912bd8 = 0  (dirty flag reset)
    *g_DirtyBd8 = 0u;
    // 0x00552e28: DAT_00912bec = 0
    *g_DirtyBec = 0u;

    // 0x00552e34: return 1
    return 1u;
}

RH_ScopedInstall(FontCtx_SetTranslation, 0x00552df0);

// ---------------------------------------------------------------------------
// FontSys_InitRenderState  --  0x00552c10
//
// Original: FUN_00552c10 (253 bytes, 0x00552c10–0x00552d0d)
// Master font render-state initialiser. Sets up the font context stack,
// char-glyph identity table, all render-state float globals.
// Callees:
//   0x004c57a0  FontCtxMatrix_AllocInit  (C1 drift-promote)
//   0x00552750  FontCtx_ResetTransform   (C2)
// Returns 1 always.
// ref: re/analysis/hud_promote_c2_b/0x00552c10.md
// ---------------------------------------------------------------------------

// 0x00552c10
extern "C" __declspec(dllexport)
std::uint32_t __cdecl FontSys_InitRenderState()
{
    // 0x00552c10: stack depth = 0
    *g_FontCtxDepth = 0u;

    // 0x00552c19: allocate slot-0 font context; store at DAT_00912a84
    g_FontCtxPtrs[0] = g_AllocInit();

    // 0x00552c22: FontCtx_ResetTransform() — identity matrix for slot 0
    g_ResetTransform();

    // 0x00552c2b: DAT_00912be4 = 0x3f000000 (0.5f)
    *g_Be4 = 0.5f;

    // 0x00552c34: DAT_00912a50 |= 0x20003
    *g_FlagsA50 |= 0x20003u;

    // 0x00552c3e: DAT_00912bec = 0
    *g_DirtyBec = 0u;

    // 0x00552c47: DAT_00912be0 = 1.0f (global font scale)
    *g_Scale_be0 = 1.0f;

    // 0x00552c50: DAT_00912bd8 = 0 (dirty flag = 0)
    *g_DirtyBd8 = 0u;

    // 0x00552c59: DAT_00912bf0 = 7
    *g_FlagBf0 = 7u;

    // 0x00552c62: DAT_00912a6c = 1.0f
    *g_A6c = 1.0f;
    // 0x00552c6b: DAT_00912a58 = 1.0f
    *g_A58 = 1.0f;
    // 0x00552c74: DAT_00912a44 = 1.0f  (diagonal [0,0] of global matrix region)
    *g_A44 = 1.0f;
    // 0x00552c7d: DAT_00912a54 = 0
    *g_A54 = 0.0f;
    // 0x00552c86: DAT_00912a4c = 0
    *g_A4c = 0.0f;
    // 0x00552c8f: DAT_00912a48 = 0
    *g_A48 = 0.0f;
    // 0x00552c98: DAT_00912a68 = 0
    *g_A68 = 0.0f;
    // 0x00552ca1: DAT_00912a64 = 0
    *g_A64 = 0.0f;
    // 0x00552caa: DAT_00912a5c = 0
    *g_A5c = 0.0f;
    // 0x00552cb3: DAT_00912a7c = 0
    *g_A7c = 0.0f;
    // 0x00552cbc: DAT_00912a78 = 0
    *g_A78 = 0.0f;
    // 0x00552cc5: DAT_00912a74 = 0
    *g_A74 = 0.0f;

    // 0x00552cce–0x00552ce7: identity char-glyph table init
    // Loop: (&DAT_00912500)[i] = (short)i; for i=0..255
    for (std::int32_t i = 0; i < 256; ++i) {
        g_CharGlyphTable[i] = static_cast<std::int16_t>(i);
    }

    // 0x00552ceb–0x00552d03: font context ptr array clear (31 slots, 1..31)
    // Loop: (&DAT_00912a88)[i] = 0; for i = 0x1f..1 (downward)
    // DAT_00912a88 = g_FontCtxPtrs[1] (first slot after slot[0] at 0x00912a84)
    for (std::int32_t i = 0x1f; i >= 1; --i) {
        g_FontCtxPtrs[i] = nullptr;
    }

    // 0x00552d04: DAT_00912c0c = 0 (active camera = null)
    *g_ActiveCamera = nullptr;

    // 0x00552d0b: DAT_00912bf8 = 0
    *g_AspBf8 = 0.0f;

    // Aspect ratio defaults (from note end section; 0x00912c00 and 0x00912c04 = 1.0f)
    *g_AspC00 = 1.0f;
    *g_AspC04 = 1.0f;

    // Unknown trailing zero-writes (0x00913204, 0x00913208 = 0)
    *g_Unk3204 = 0u;
    *g_Unk3208 = 0u;

    return 1u;
}

RH_ScopedInstall(FontSys_InitRenderState, 0x00552c10);

// ---------------------------------------------------------------------------
// FontMatrix_Push  --  0x00552d10
//
// Original: FUN_00552d10 (86 bytes, 0x00552d10–0x00552d66)
// Overflow check (max 31); increment depth; lazy-alloc new slot;
// copy 64 bytes (16 dwords) from previous ctx to new ctx.
// Returns true on success, false on overflow.
// Callee: 0x004c57a0 FontCtxMatrix_AllocInit (C1 drift-promote)
// ref: re/analysis/hud_promote_c2_b/0x00552d10.md
// ---------------------------------------------------------------------------

// 0x00552d10
extern "C" __declspec(dllexport)
bool __cdecl FontMatrix_Push()
{
    // 0x00552d10: overflow check — max 31 levels (0x1f)
    bool bVar1 = (*g_FontCtxDepth < 0x1fu);

    // 0x00552d16: if overflow, return false immediately
    if (!bVar1)
        return false;

    // 0x00552d1c: increment stack depth
    *g_FontCtxDepth = *g_FontCtxDepth + 1u;
    std::uint32_t new_idx = *g_FontCtxDepth;

    // 0x00552d25: lazy-allocate new slot if not yet created
    if (g_FontCtxPtrs[new_idx] == nullptr) {
        // 0x00552d2e: g_FontCtxPtrs[new_idx] = FontCtxMatrix_AllocInit()
        g_FontCtxPtrs[new_idx] = g_AllocInit();
    }

    // 0x00552d41–0x00552d5e: copy 64 bytes (16 dwords) from prev ctx to new ctx
    // Source: (&DAT_00912a80 + new_idx * 4) dereferences slot[new_idx - 1]
    //   = g_FontCtxPtrBase1[new_idx] = g_FontCtxPtrs[new_idx - 1]
    void* prev_ctx = g_FontCtxPtrBase1[new_idx]; // == g_FontCtxPtrs[new_idx - 1]
    void* new_ctx  = g_FontCtxPtrs[new_idx];
    std::memcpy(new_ctx, prev_ctx, 64u);

    // 0x00552d63: return bVar1 (true)
    return bVar1;
}

RH_ScopedInstall(FontMatrix_Push, 0x00552d10);

// ---------------------------------------------------------------------------
// FontCtx_FlushMatrix  --  0x00552e40
//
// Original: FUN_00552e40 (349 bytes, 0x00552e40–0x00552f9d)
// Dirty-gated matrix composer. On dirty (DAT_00912bd8==0): recomputes compose,
// scales by viewport, translates, caches result + inverted copy, marks clean.
// On every call: composes cached matrix with camera view, returns &DAT_00912b98.
// Callees:
//   0x004c4600 RwMatrixMultiply  (C1 drift-promote)
//   0x004c5010 RwMatrixScale     (C3)
//   0x004c51a0 RwMatrixTranslate (C2)
//   0x004c4dc0 RwMatrixInvert    (C1 drift-promote)
//   0x004c0ed0 camera matrix accessor (C1 drift-promote)
// ref: re/analysis/hud_promote_c2_b/0x00552e40.md
// Note: [UNCERTAIN U-2132] dirty flag convention: 0=dirty/recompute, 1=clean/skip.
// ---------------------------------------------------------------------------

// 0x00552e40
extern "C" __declspec(dllexport)
float* __cdecl FontCtx_FlushMatrix()
{
    // Path A — recompute only when dirty (DAT_00912bd8 == 0)
    if (*g_DirtyBd8 == 0u) {
        // 0x00552e52: load global font scale
        float fVar7 = *g_FontScale;

        // 0x00552e5b: camera-type override — if camera[+0x14] == 2, scale = 1.0f
        {
            std::int32_t cam_type =
                *reinterpret_cast<std::int32_t*>(
                    reinterpret_cast<std::uint8_t*>(*g_ActiveCamera) + 0x14u);
            if (cam_type == 2)
                fVar7 = 1.0f;
        }

        // 0x00552e76: compose ctx matrix with g_ExtraXform into local_40
        // FUN_004c4600(local_40, (&DAT_00912a84)[depth], &DAT_00912a44, fVar7)
        // Note: 4th arg fVar7 passed — but analysis shows FUN_004c4600 is a 3-param
        // matrix multiply (dst, a, b). The fVar7 appears in analysis at 0x00552e76
        // as a separate line before the call. From decomp context this is:
        // compose(local_40, ctx, g_ExtraXform)
        float local_40[16];
        float* ctx = reinterpret_cast<float*>(g_FontCtxPtrs[*g_FontCtxDepth]);
        g_RwMatrixMultiply(local_40, ctx, g_ExtraXform);

        // 0x00552e8f–0x00552ea5: build {fVar7, fVar7, 1.0f} scale vector
        // 0x00552ea8: RwMatrixScale(local_40, {fVar7,fVar7,1.0f}, 2=postconcat)
        {
            float sv[3] = { fVar7, fVar7, 1.0f };
            g_RwMatrixScale(local_40, sv, 2);
        }

        // 0x00552ebc–0x00552ece: min_dim = min(DAT_00912b08, DAT_00912b0c)
        float min_dim = (*g_ViewW < *g_ViewH) ? *g_ViewW : *g_ViewH;

        // 0x00552ed1–0x00552ee7: build {-min_dim, -min_dim, 1.0f}
        // 0x00552eea: RwMatrixScale(local_40, {-min_dim,-min_dim,1.0f}, 2=postconcat)
        {
            float sv[3] = { -min_dim, -min_dim, 1.0f };
            g_RwMatrixScale(local_40, sv, 2);
        }

        // 0x00552f00–0x00552f21: compute tx, ty
        // tx = DAT_00912b08 * fVar7 * DAT_005cc32c
        // ty = -(DAT_00912b0c * fVar7 * DAT_005cc32c)
        // tz = 0
        float local_4c = (*g_ViewW) * fVar7 * (*g_HalfScale);
        float local_48 = -((*g_ViewH) * fVar7 * (*g_HalfScale));
        float local_44 = 0.0f;

        // 0x00552f2a: RwMatrixTranslate(local_40, {tx,ty,0}, 2=postconcat)
        {
            float tv[3] = { local_4c, local_48, local_44 };
            g_RwMatrixTranslate(local_40, tv, 2);
        }

        // 0x00552f3a–0x00552f55: cache local_40 (64 bytes) → DAT_00912b18
        std::memcpy(g_CachedMat, local_40, 64u);

        // 0x00552f59: RwMatrixInvert(&DAT_00912b58, local_40) — store inverted
        g_RwMatrixInvert(g_InvMat, local_40);

        // 0x00552f65: DAT_00912bd8 = 1 (mark clean)
        *g_DirtyBd8 = 1u;
    }

    // Final composition — every call:
    // 0x00552f6c: uVar3 = FUN_004c0ed0(*(cam + 4)) — get camera view matrix ptr
    void* cam_node_ptr =
        *reinterpret_cast<void**>(
            reinterpret_cast<std::uint8_t*>(*g_ActiveCamera) + 4u);
    void* view_mat = g_CameraMatrix(cam_node_ptr);

    // 0x00552f7c: RwMatrixMultiply(&DAT_00912b98, &DAT_00912b18, view_mat)
    g_RwMatrixMultiply(g_OutputMat, g_CachedMat, reinterpret_cast<float*>(view_mat));

    // return &DAT_00912b98
    return g_OutputMat;
}

RH_ScopedInstall(FontCtx_FlushMatrix, 0x00552e40);
