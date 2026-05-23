// Mashed RE — Render Wave 3 Session 3 cluster.
//
// Binary anchor: MASHED.exe SHA-256 (unpatched):
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Functions in this file (promoted):
//   0x004cc9f0  RwFreeListDestroy         — 136B; free-list pool destructor; vtbl+0x10c/+0x11c raw free; global list unlink
//   0x004ccba0  RwFreeListFree            — 168B; free element back to pool; bitmap bit-clear; auto-release empty blocks
//   0x004cfe40  rwD3D9VBPoolDestroy       — 148B; vertex/index buffer pool teardown; drains linked list + free-lists
//   0x004dcdc0  rwD3D9SkinVBPoolDestroy   — 455B; geometry/skin VB pool cluster teardown (4-slot + VB cache + geometry pool)
//   0x004cc5e0  RwStreamFindChunk         — 243B; scan RW stream for chunk type; validate version range [0x35000,0x37002]
//   0x004260e0  TrackPathBuild            — 524B; pure leaf; constructs ToastArt/tracks/<name> path into static buffer
//   0x00403db0  HudLabelDrawTriple        — 277B; 3×shadow+fill label pair draw; callee FUN_00427ad0 C3
//   0x0040de30  MinimapCameraOrthoSetup   — 228B; save+reset camera to ortho; WorldRenderDispatch_Begin
//   0x0040df20  MinimapCameraRestore      — 56B; restore camera from save slots; WorldRenderDispatch_End
//
// Already hooked in Math/ (skip to avoid redefinition):
//   0x004c3730  RwV3dTransformPoint   — in Math/RwV3dTransform.cpp
//   0x004c3880  RwV3dTransformVector  — in Math/RwV3dTransform.cpp
//   0x004c3bf0  Vec2Magnitude         — in Math/RwV3dTransform.cpp
//   0x004c3c60  Vec2Normalise         — in Math/RwV3dTransform.cpp
//
// Deferred (live-state / anti-island gate failures in this candidate set):
//   0x004c8690  rwD3D9ConstantBufferPoolInit  — callee 0x004ccc50 still C1 (anti-island)
//   0x004c8740  rwD3D9EnumerateMaxMSAALevel   — walks live IDirect3D9* object (live D3D state)
//   0x004c8800  rwD3D9SelectPresentParameters — live D3D9 present-parameter selection; calls CreateDevice-adjacent funcs
//   0x004c8c70  rwD3D9DeviceStop              — live D3D9 device teardown; mutates device state
//   0x004dcaa0  rwD3D9VBAlloc                 — calls IDirect3DDevice9::CreateVertexBuffer (live D3D state)
//   0x004c8e50  rwD3D9RasterSystemInit        — 9 functional callees all C1 (anti-island)
//
// Analysis refs:
//   re/analysis/promote_c2_render_d3d9/0x004cc9f0.md
//   re/analysis/promote_c2_render_d3d9/0x004ccba0.md
//   re/analysis/promote_c2_render_d3d9/0x004cfe40.md
//   re/analysis/promote_c2_render_d3d9/0x004dcdc0.md
//   re/analysis/texture_loader_d2/0x004cc5e0.md
//   re/analysis/render_promote_c2_track_loader/0x004260e0.md
//   re/analysis/promote_c2_render_lowrva/00403db0.md
//   re/analysis/promote_c2_render_lowrva/0040de30.md
//   re/analysis/promote_c2_render_lowrva/0040df20.md

#include "../Core/HookSystem.h"

#include <cstdint>
#include <cstring>   // memset

// RW device vtable base pointer. [0x007d3ff8]
// Dereferenced as: const std::uint32_t vtbl = *s_RwVtableBase;
static const std::uint32_t* const s_RwVtableBase =
    reinterpret_cast<const std::uint32_t*>(0x007d3ff8u);

// ---------------------------------------------------------------------------
// TrackPathBuild  --  0x004260e0
//
// Original: FUN_004260e0 (524 bytes, 0x004260e0..0x004262e8)
// Signature:
//   undefined4* FUN_004260e0(int param_1, char* param_2, char* param_3, char* param_4)
//   param_1 = separator mode: 0 = forward-slash "/" ; non-0 = backslash "\" + prefix
//   param_2 = track name string
//   param_3 = sub-path or "" (appended with separator if non-empty)
//   param_4 = extension or "" (appended directly if non-empty)
//   returns: &DAT_008991e0 (static output buffer)
//
// Constructs a path of the form ToastArt/tracks/<param_2>[/<param_3>][param_4]
// into a static buffer at 0x008991e0 using inline walk-to-null + 4-byte block copy
// (no CRT call). param_3 appears twice if non-empty (quirk matching decompilation).
//
// Steps (cited from 0x004260e0 body):
//   1. Clears first byte of DAT_008991e0 to '\0'.                    [0x004260e0 body]
//   2. param_1 == 0: separator = "/".                               [0x004260e0 body]
//      param_1 != 0: separator = "\\"; prepend DAT_005cd4ac + DAT_005cc4dc. [0x004260e0 body]
//   3. Appends "ToastArt" (s_ToastArt_005cd4a0).                    [0x004260e0 body]
//   4. Appends separator.                                            [0x004260e0 body]
//   5. Appends "tracks" (s_tracks_005cd498).                        [0x004260e0 body]
//   6. Appends separator.                                            [0x004260e0 body]
//   7. Appends param_2.                                              [0x004260e0 body]
//   8. If *param_3 != '\0': appends separator then param_3.         [0x004260e0 body]
//   9. Appends param_3 unconditionally.                              [0x004260e0 body: quirk — may appear twice]
//   10. If *param_4 != '\0': appends param_4.                       [0x004260e0 body]
//   11. Returns &DAT_008991e0.                                       [0x004260e0 body]
//
// Key globals (cited from 0x004260e0 body):
//   0x008991e0 — static output path buffer (first byte cleared at entry)
//   0x005cd4a0 — literal "ToastArt"
//   0x005cd498 — literal "tracks"
//   0x005cd4ac — prefix string for backslash mode [UNCERTAIN U-3218]
//   0x005cc4dc — 2-byte char appended in backslash mode [UNCERTAIN U-3218]
//
// No callees — pure leaf (inline string ops only).
// Caller: FUN_00426e10 passes (0, trackName, "COURSE.LUA", 0x5cc4de).
//
// ref: re/analysis/render_promote_c2_track_loader/0x004260e0.md
// ---------------------------------------------------------------------------

// Static path buffer. [0x008991e0]
static char* const s_TrackPathBuf =
    reinterpret_cast<char*>(0x008991e0u);
// String constants. [cited from 0x004260e0 body]
static const char* const s_ToastArt =
    reinterpret_cast<const char*>(0x005cd4a0u);  // "ToastArt"
static const char* const s_tracks =
    reinterpret_cast<const char*>(0x005cd498u);  // "tracks"
// Backslash-mode prefix [UNCERTAIN U-3218]. [0x005cd4ac]
static const char* const s_BsPrefix =
    reinterpret_cast<const char*>(0x005cd4acu);
// Backslash-mode 2-byte char. [0x005cc4dc] [UNCERTAIN U-3218]
static const char* const s_BsChar2 =
    reinterpret_cast<const char*>(0x005cc4dcu);

// Helper: inline walk-to-null append (replicates original 4-byte block copy loop).
static char* s_AppendStr(char* dst, const char* src) {
    while (*src) {
        *dst++ = *src++;
    }
    *dst = '\0';
    return dst;
}

// 0x004260e0
extern "C" __declspec(dllexport) char* __cdecl TrackPathBuild(
    std::int32_t  param_1,   // separator mode (0=forward-slash, else backslash)
    const char*   param_2,   // track name
    const char*   param_3,   // sub-path (may be "")
    const char*   param_4)   // extension (may be "")
{
    // Step 1: clear output buffer. [0x004260e0 body]
    s_TrackPathBuf[0] = '\0';
    char* p = s_TrackPathBuf;

    // Step 2: separator selection and optional prefix. [0x004260e0 body]
    const char* sep;
    if (param_1 == 0) {
        sep = "/";   // forward-slash mode [0x004260e0 body]
    } else {
        sep = "\\";  // backslash mode [0x004260e0 body]
        // Prepend DAT_005cd4ac and 2-byte DAT_005cc4dc. [0x004260e0 body: backslash branch]
        p = s_AppendStr(p, s_BsPrefix);
        // Append the 2-byte char (may be \0-terminated 1-char or 2-char seq). [0x004260e0 body]
        *p++ = s_BsChar2[0];
        *p++ = s_BsChar2[1];
        *p   = '\0';
    }

    // Step 3: append "ToastArt". [0x004260e0 body]
    p = s_AppendStr(p, s_ToastArt);
    // Step 4: separator. [0x004260e0 body]
    p = s_AppendStr(p, sep);
    // Step 5: "tracks". [0x004260e0 body]
    p = s_AppendStr(p, s_tracks);
    // Step 6: separator. [0x004260e0 body]
    p = s_AppendStr(p, sep);
    // Step 7: track name. [0x004260e0 body]
    p = s_AppendStr(p, param_2);
    // Step 8: conditional separator + param_3. [0x004260e0 body]
    if (*param_3 != '\0') {
        p = s_AppendStr(p, sep);
        p = s_AppendStr(p, param_3);
    }
    // Step 9: param_3 unconditionally (decompiler quirk — may appear twice). [0x004260e0 body]
    p = s_AppendStr(p, param_3);
    // Step 10: param_4 if non-empty. [0x004260e0 body]
    if (*param_4 != '\0') {
        p = s_AppendStr(p, param_4);
    }

    return s_TrackPathBuf;   // Step 11: return static buffer. [0x004260e0 body]
}

RH_ScopedInstall(TrackPathBuild, 0x004260e0);

// ---------------------------------------------------------------------------
// HudLabelDrawTriple  --  0x00403db0
//
// Original: FUN_00403db0 (277 bytes, 0x00403db0..0x00403ec5)
// Signature:
//   void FUN_00403db0(void)
//
// Draws three HUD labels at fixed positions using the standard shadow+fill
// pair pattern (Δx=+3, Δy=+3 drop-shadow).
//
// Each label pair calls FUN_00427ad0 (SpriteTextDraw — 7 args: id/x/y/w/h/color/scale).
//
// Three label pairs (cited from 0x00403db0 body):
//   Pair 1 — id 0xef  (239): y_shadow=83.0, y_fill=80.0    [0x00403db0 body]
//   Pair 2 — id 0x22d (557): y_shadow=113.0, y_fill=110.0  [0x00403db0 body]
//   Pair 3 — id 0x22e (558): y_shadow=143.0, y_fill=140.0  [0x00403db0 body]
//   Y stride between pairs = 30.0 (three stacked rows).
//
// All 6 calls share (cited from 0x00403db0 body):
//   x_shadow = 123.0  (0x42f60000)
//   x_fill   = 120.0  (0x42f00000)
//   w        = 400.0  (0x43c80000)
//   h        =  80.0  (0x42a00000)
//   scale    =   0.8  (0x3f4ccccd)
//   color_shadow = 0xff000000 (opaque black)
//   color_fill   = 0xc8ffffff (white α=0xC8)
//
// Callee: FUN_00427ad0 (SpriteTextDraw — C3, already hooked).
//
// ref: re/analysis/promote_c2_render_lowrva/00403db0.md
// ---------------------------------------------------------------------------

// SpriteTextDraw (7-arg sprite/text dispatcher). [0x00427ad0] C3.
typedef void (__cdecl *SpriteTextDraw_t)(std::uint32_t id,
                                          float x, float y,
                                          float w, float h,
                                          std::uint32_t color,
                                          float scale);
static const SpriteTextDraw_t s_FUN_00427ad0 =
    reinterpret_cast<SpriteTextDraw_t>(0x00427ad0u);

// 0x00403db0
extern "C" __declspec(dllexport) void __cdecl HudLabelDrawTriple(void)
{
    // All pairs: w=400.0, h=80.0, scale=0.8, x_shadow=123.0, x_fill=120.0. [0x00403db0 body]
    // Pair 1 — id 0xef, y row 0 (80/83). [0x00403db0 body]
    s_FUN_00427ad0(0xefu,  123.0f, 83.0f, 400.0f, 80.0f, 0xff000000u, 0.8f);  // shadow [0x00403db0 body]
    s_FUN_00427ad0(0xefu,  120.0f, 80.0f, 400.0f, 80.0f, 0xc8ffffffu, 0.8f);  // fill   [0x00403db0 body]
    // Pair 2 — id 0x22d, y row 1 (110/113). [0x00403db0 body]
    s_FUN_00427ad0(0x22du, 123.0f, 113.0f, 400.0f, 80.0f, 0xff000000u, 0.8f); // shadow [0x00403db0 body]
    s_FUN_00427ad0(0x22du, 120.0f, 110.0f, 400.0f, 80.0f, 0xc8ffffffu, 0.8f); // fill   [0x00403db0 body]
    // Pair 3 — id 0x22e, y row 2 (140/143). [0x00403db0 body]
    s_FUN_00427ad0(0x22eu, 123.0f, 143.0f, 400.0f, 80.0f, 0xff000000u, 0.8f); // shadow [0x00403db0 body]
    s_FUN_00427ad0(0x22eu, 120.0f, 140.0f, 400.0f, 80.0f, 0xc8ffffffu, 0.8f); // fill   [0x00403db0 body]
}

RH_ScopedInstall(HudLabelDrawTriple, 0x00403db0);

// ---------------------------------------------------------------------------
// MinimapCameraOrthoSetup  --  0x0040de30
//
// Original: FUN_0040de30 (228 bytes, 0x0040de30..0x0040df14)
// Signature:
//   void FUN_0040de30(int param_1)   — param_1 = RwCamera*
//
// Saves the current camera state into global slots (frame matrix, view-window,
// projection-type) and resets to an orthographic (parallel) projection at
// view-window (0.6, 0.45). Then begins the minimap render pass via
// FUN_00426670 (WorldRenderDispatch_Begin).
//
// Steps (cited from 0x0040de30 body):
//   1. iVar1 = *(int*)(param_1 + 4) — frame ptr at camera+0x04.    [0x0040de30 body]
//   2. local_48 = 0x3f19999a (≈ 0.6f); local_44 = 0x3ee66666 (≈ 0.45f). [0x0040de30 body]
//   3. Copy 16 dwords (64 bytes) from iVar1+0x10 → DAT_0063b928.   [0x0040de30 body: save matrix]
//   4. DAT_0063b9ac = *(camera+0x68) — save view-window x.          [0x0040de30 body]
//   5. DAT_0063b9b0 = *(camera+0x6c) — save view-window y.          [0x0040de30 body]
//   6. DAT_0063b9a8 = *(camera+0x14) — save projection slot.        [0x0040de30 body]
//   7. Build identity 3×3 matrix on stack; flags local_34 |= 0x20003. [0x0040de30 body]
//   8. FUN_004c1480(iVar1, &local_40, 0) — apply identity frame.   [0x0040de30 body → 0x004c1480]
//   9. FUN_004c1c80(param_1, &local_48) — set view-window (0.6, 0.45). [0x0040de30 body → 0x004c1c80]
//   10. FUN_004c1c10(param_1, 2) — set projection rwPARALLEL (=2). [0x0040de30 body → 0x004c1c10]
//   11. FUN_00426670(param_1) — WorldRenderDispatch_Begin.          [0x0040de30 body → 0x00426670]
//
// Key globals (cited from 0x0040de30 body):
//   0x0063b928 — save slot: 64-byte frame modelling matrix (16 DWORDs)
//   0x0063b9a8 — save slot: projection-type (camera+0x14)
//   0x0063b9ac — save slot: view-window x (camera+0x68)
//   0x0063b9b0 — save slot: view-window y (camera+0x6c)
//
// Struct offsets on RwCamera (param_1):
//   +0x04 = frame pointer
//   +0x14 = projection-type slot [UNCERTAIN U-1709]
//   +0x68 = view-window x
//   +0x6c = view-window y
//
// Callees:
//   0x004c1480 FUN_004c1480 — RwFrameTransform (C2)
//   0x004c1c80 FUN_004c1c80 — RwCameraSetViewWindow (C2)
//   0x004c1c10 Camera::SetProjection (C2)
//   0x00426670 WorldRenderDispatch_Begin (C3, in FrameWorldPasses.cpp)
//
// Paired with: MinimapCameraRestore (0x0040df20) — call it to undo.
//
// ref: re/analysis/promote_c2_render_lowrva/0040de30.md
// ---------------------------------------------------------------------------

// Callee typedefs for minimap camera functions.
// RwFrameTransform (C2). [0x004c1480]
typedef void (__cdecl *RwFrameTransform_t)(std::uint32_t frame,
                                            const void* matrix,
                                            std::int32_t mode);
static const RwFrameTransform_t s_FUN_004c1480 =
    reinterpret_cast<RwFrameTransform_t>(0x004c1480u);
// RwCameraSetViewWindow (C2). [0x004c1c80]
typedef void (__cdecl *RwCameraSetVW_t)(std::uint32_t camera, const float* vw);
static const RwCameraSetVW_t s_FUN_004c1c80 =
    reinterpret_cast<RwCameraSetVW_t>(0x004c1c80u);
// Camera::SetProjection (C2). [0x004c1c10]
typedef void (__cdecl *CameraSetProj_t)(std::uint32_t camera, std::int32_t projType);
static const CameraSetProj_t s_FUN_004c1c10 =
    reinterpret_cast<CameraSetProj_t>(0x004c1c10u);
// WorldRenderDispatch_Begin (C3). [0x00426670]
typedef void (__cdecl *WorldRenderBegin_t)(std::uint32_t camera);
static const WorldRenderBegin_t s_FUN_00426670 =
    reinterpret_cast<WorldRenderBegin_t>(0x00426670u);

// Save slots for minimap camera state. [cited from 0x0040de30 body]
static volatile std::uint32_t* const s_SaveMatrix  =
    reinterpret_cast<volatile std::uint32_t*>(0x0063b928u);   // 64 bytes / 16 DWORDs
static volatile std::uint32_t* const s_SaveProjSlot =
    reinterpret_cast<volatile std::uint32_t*>(0x0063b9a8u);
static volatile std::uint32_t* const s_SaveVWX      =
    reinterpret_cast<volatile std::uint32_t*>(0x0063b9acu);
static volatile std::uint32_t* const s_SaveVWY      =
    reinterpret_cast<volatile std::uint32_t*>(0x0063b9b0u);

// 0x0040de30
extern "C" __declspec(dllexport) void __cdecl MinimapCameraOrthoSetup(
    std::uint32_t param_1)   // RwCamera*
{
    // Step 1: read frame pointer at camera+0x04. [0x0040de30 body]
    std::uint32_t iVar1 = *reinterpret_cast<std::uint32_t*>(param_1 + 4u);

    // Step 2: ortho view-window (0.6, 0.45). [0x0040de30 body]
    float local_48 = 0.6f;   // 0x3f19999a
    float local_44 = 0.45f;  // 0x3ee66666
    float vw[2] = { local_48, local_44 };

    // Step 3: save frame modelling matrix (16 DWORDs from frame+0x10). [0x0040de30 body]
    const std::uint32_t* srcMatrix =
        reinterpret_cast<const std::uint32_t*>(iVar1 + 0x10u);
    for (int i = 0; i < 16; ++i) {
        const_cast<std::uint32_t*>(s_SaveMatrix)[i] = srcMatrix[i];
    }

    // Step 4+5: save view-window from camera+0x68/0x6c. [0x0040de30 body]
    *s_SaveVWX = *reinterpret_cast<std::uint32_t*>(param_1 + 0x68u);
    *s_SaveVWY = *reinterpret_cast<std::uint32_t*>(param_1 + 0x6cu);

    // Step 6: save projection slot from camera+0x14. [0x0040de30 body]
    *s_SaveProjSlot = *reinterpret_cast<std::uint32_t*>(param_1 + 0x14u);

    // Step 7: build identity 3×3 + zero translation on stack.
    // local_34 flags = 0x20003 (rwMATRIXTYPENORMAL | optimization bits). [0x0040de30 body]
    // Layout matches RwMatrix 9-float block: [0x0040de30 body]
    //   right(x,y,z), up(x,y,z), at(x,y,z) — each 1.0 on diagonal, 0 off.
    float identMatrix[13];   // 13 DWORDs to cover local_40..local_8 as in Ghidra layout
    memset(identMatrix, 0, sizeof(identMatrix));
    identMatrix[0]  = 1.0f;  // local_40 — right.x  [0x0040de30 body: diagonal 1.0]
    identMatrix[4]  = 1.0f;  // local_2c — up.y      [0x0040de30 body]
    identMatrix[8]  = 1.0f;  // local_18 — at.z      [0x0040de30 body]
    // Flags word overlaid at local_34 position (DWORD at index 3 of right field group).
    std::uint32_t matrixFlags = 0x20003u;   // [0x0040de30 body: local_34 |= 0x20003]

    // Build a 16-float RwMatrix (identity 3×3, zero translation, flags).
    float rwMat[16];
    memset(rwMat, 0, sizeof(rwMat));
    // right = (1,0,0) at offsets [0..2]; flags at [3]
    rwMat[0] = 1.0f;
    memcpy(&rwMat[3], &matrixFlags, sizeof(matrixFlags));
    // up = (0,1,0) at offsets [4..6]; zero at [7]
    rwMat[5] = 1.0f;
    // at = (0,0,1) at offsets [8..10]; zero at [11]
    rwMat[10] = 1.0f;
    // translation = (0,0,0) at offsets [12..14]; zero at [15]

    // Step 8: apply identity to frame (mode 0 = overwrite). [0x0040de30 body → 0x004c1480]
    s_FUN_004c1480(iVar1, rwMat, 0);

    // Step 9: set ortho view-window. [0x0040de30 body → 0x004c1c80]
    s_FUN_004c1c80(param_1, vw);

    // Step 10: set projection rwPARALLEL = 2. [0x0040de30 body → 0x004c1c10]
    s_FUN_004c1c10(param_1, 2);

    // Step 11: begin minimap world render. [0x0040de30 body → 0x00426670]
    s_FUN_00426670(param_1);
}

RH_ScopedInstall(MinimapCameraOrthoSetup, 0x0040de30);

// ---------------------------------------------------------------------------
// MinimapCameraRestore  --  0x0040df20
//
// Original: FUN_0040df20 (56 bytes, 0x0040df20..0x0040df58)
// Signature:
//   void FUN_0040df20(int param_1)   — param_1 = RwCamera*
//
// Ends the minimap render pass and restores camera state saved by
// MinimapCameraOrthoSetup (FUN_0040de30). Restore order is reverse of save:
// projection → view-window → matrix.
//
// Steps (cited from 0x0040df20 body):
//   1. FUN_004266b0(param_1) — WorldRenderDispatch_End.            [0x0040df20 body → 0x004266b0]
//   2. uVar1 = *(uint*)(param_1 + 4) — frame ptr.                  [0x0040df20 body]
//   3. FUN_004c1c10(param_1, DAT_0063b9a8) — restore projection.   [0x0040df20 body → 0x004c1c10]
//   4. FUN_004c1c80(param_1, &DAT_0063b9ac) — restore view-window. [0x0040df20 body → 0x004c1c80]
//   5. FUN_004c1480(uVar1, &DAT_0063b928, 0) — restore frame matrix. [0x0040df20 body → 0x004c1480]
//
// Save slots (read back from 0x0040de30 writes):
//   0x0063b9a8 — saved projection type
//   0x0063b9ac — saved view-window {x, y}
//   0x0063b928 — saved frame modelling matrix (64 bytes)
//
// Callees:
//   0x004266b0 WorldRenderDispatch_End (C3, in FrameWorldPasses.cpp)
//   0x004c1c10 Camera::SetProjection (C2)
//   0x004c1c80 FUN_004c1c80 — RwCameraSetViewWindow (C2)
//   0x004c1480 FUN_004c1480 — RwFrameTransform (C2)
//
// Paired with: MinimapCameraOrthoSetup (0x0040de30).
//
// ref: re/analysis/promote_c2_render_lowrva/0040df20.md
// ---------------------------------------------------------------------------

// WorldRenderDispatch_End (C3). [0x004266b0]
typedef void (__cdecl *WorldRenderEnd_t)(std::uint32_t camera);
static const WorldRenderEnd_t s_FUN_004266b0 =
    reinterpret_cast<WorldRenderEnd_t>(0x004266b0u);

// 0x0040df20
extern "C" __declspec(dllexport) void __cdecl MinimapCameraRestore(
    std::uint32_t param_1)   // RwCamera*
{
    // Step 1: end minimap render pass. [0x0040df20 body → 0x004266b0]
    s_FUN_004266b0(param_1);

    // Step 2: read frame pointer. [0x0040df20 body]
    std::uint32_t uVar1 = *reinterpret_cast<std::uint32_t*>(param_1 + 4u);

    // Step 3: restore projection. [0x0040df20 body → 0x004c1c10]
    s_FUN_004c1c10(param_1, static_cast<std::int32_t>(*s_SaveProjSlot));

    // Step 4: restore view-window from save slots {0x0063b9ac, 0x0063b9b0}. [0x0040df20 body → 0x004c1c80]
    // Pass &DAT_0063b9ac — consecutive two DWORDs = {saved_vw_x, saved_vw_y}.
    s_FUN_004c1c80(param_1, reinterpret_cast<const float*>(
        const_cast<const std::uint32_t*>(s_SaveVWX)));

    // Step 5: restore frame matrix from save slot at 0x0063b928 (mode 0 = overwrite). [0x0040df20 body → 0x004c1480]
    s_FUN_004c1480(uVar1, const_cast<const std::uint32_t*>(s_SaveMatrix), 0);
}

RH_ScopedInstall(MinimapCameraRestore, 0x0040df20);

// ---------------------------------------------------------------------------
// RwFreeListDestroy  --  0x004cc9f0
//
// Original: FUN_004cc9f0 (136 bytes, 0x004cc9f0..0x004cca77)
// Signature:
//   undefined4 FUN_004cc9f0(int param_1)  // param_1 = RwFreeList*; returns 1
//
// Destroys a free-list pool created by RwFreeListCreate (FUN_004cc820).
//
// Steps (cited from 0x004cc9f0 body):
//   1. piVar1 = param_1 + 0x10 (sentinel node — free-block list head).  [0x004cc9f0 body]
//   2. Unlink param_1 from global pool chain at DAT_007d45cc:
//      *(param_1+0x20) = *(param_1+0x1c)                               [0x004cc9f0 body]
//      *(*(param_1+0x1c) + 4) = *(param_1+0x20)                        [0x004cc9f0 body]
//   3. Walk block chain at *piVar1 until node == piVar1 (sentinel):
//      - Unlink each block.                                              [0x004cc9f0 body]
//      - Free block via vtbl +0x10c.                                    [0x004cc9f0 body → DAT_007d3ff8+0x10c]
//   4. Free header param_1:
//      - If (*(byte*)(param_1+0x18) & 1): caller-supplied → skip free. [0x004cc9f0 body: bit 0 flag]
//      - If DAT_007d45fc == param_1 or DAT_007d45fc == NULL:
//          vtbl +0x10c (raw free).                                       [0x004cc9f0 body]
//      - Else: vtbl +0x11c (DAT_007d45fc, param_1) — pool free.         [0x004cc9f0 body]
//   5. Return 1.
//
// Key globals (cited from 0x004cc9f0 body):
//   0x007d3ff8 — RW device vtable base
//   0x007d45fc — secondary pool allocator handle
//   vtbl +0x10c — raw memory free
//   vtbl +0x11c — pool memory free into secondary allocator
//
// No callees — leaf-function exemption applies per CONFIDENCE.md.
//
// ref: re/analysis/promote_c2_render_d3d9/0x004cc9f0.md
// ---------------------------------------------------------------------------

// Secondary pool allocator handle. [0x007d45fc]
static volatile std::uint32_t* const s_RwSecondaryPool =
    reinterpret_cast<volatile std::uint32_t*>(0x007d45fcu);

// 0x004cc9f0
extern "C" __declspec(dllexport) std::uint32_t __cdecl RwFreeListDestroy(
    std::uint32_t* param_1)   // RwFreeList* to destroy
{
    // Step 1: sentinel = param_1 + 0x10. [0x004cc9f0 body]
    std::uint32_t* piVar1 = param_1 + 4;   // +0x10 bytes = +4 DWORDs

    // Step 2: unlink from global pool chain. [0x004cc9f0 body]
    std::uint32_t* backLink  = reinterpret_cast<std::uint32_t*>(param_1[7]);  // param_1+0x1c
    std::uint32_t* fwdLink   = reinterpret_cast<std::uint32_t*>(param_1[8]);  // param_1+0x20
    *fwdLink   = param_1[7];   // predecessor's next = our next  [0x004cc9f0 body]
    backLink[1] = param_1[8];  // successor's prev  = our prev  [0x004cc9f0 body]

    // Step 3: walk and free all blocks in the chain. [0x004cc9f0 body]
    typedef void (__cdecl *RawFreeFn)(void*);
    typedef void (__cdecl *PoolFreeFn)(std::uint32_t pool, void* ptr);

    std::uint32_t vtbl = *s_RwVtableBase;
    RawFreeFn  rawFree  = *reinterpret_cast<RawFreeFn*> (vtbl + 0x10cu);
    PoolFreeFn poolFree = *reinterpret_cast<PoolFreeFn*>(vtbl + 0x11cu);

    std::uint32_t* piVar2 = reinterpret_cast<std::uint32_t*>(*piVar1);
    while (piVar2 != piVar1) {
        std::uint32_t* next  = reinterpret_cast<std::uint32_t*>(piVar2[0]);  // link at node+0
        std::uint32_t* prev  = reinterpret_cast<std::uint32_t*>(piVar2[1]);  // link at node+4
        // Unlink block. [0x004cc9f0 body]
        *prev = piVar2[0];
        *reinterpret_cast<std::uint32_t*>(piVar2[0] + 4u) = reinterpret_cast<std::uint32_t>(piVar2) + 4u;
        rawFree(piVar2);   // [0x004cc9f0 body → vtbl+0x10c]
        piVar2 = next;
    }

    // Step 4: free the header. [0x004cc9f0 body]
    std::uint8_t flags = *reinterpret_cast<std::uint8_t*>(
        reinterpret_cast<std::uint8_t*>(param_1) + 0x18u);
    if ((flags & 1u) == 0u) {
        // Not caller-supplied — free it. [0x004cc9f0 body]
        std::uint32_t secondary = *s_RwSecondaryPool;
        if (secondary == reinterpret_cast<std::uint32_t>(param_1) || secondary == 0u) {
            rawFree(param_1);   // raw free [0x004cc9f0 body → vtbl+0x10c]
        } else {
            poolFree(secondary, param_1);   // pool free [0x004cc9f0 body → vtbl+0x11c]
        }
    }

    return 1u;   // [0x004cc9f0 body]
}

RH_ScopedInstall(RwFreeListDestroy, 0x004cc9f0);

// ---------------------------------------------------------------------------
// RwFreeListFree  --  0x004ccba0
//
// Original: FUN_004ccba0 (168 bytes, 0x004ccba0..0x004ccc48)
// Signature:
//   uint * FUN_004ccba0(uint *param_1, uint param_2)
//   param_1 = RwFreeList*
//   param_2 = address of allocated element to free
//   returns: param_1 on success, NULL if element address not found
//
// Frees an element back to a RenderWare free-list pool.
//
// Steps (cited from 0x004ccba0 body):
//   1. puVar1 = (uint *)param_1[4] — block list head (at +0x10).  [0x004ccba0 body]
//   2. uVar2 = param_1[2] — bitmap row stride.                    [0x004ccba0 body]
//   3. Walk block chain while puVar1 != &param_1[4] (sentinel):
//      uVar3 = (int)puVar1 + uVar2 + 8  (block data start)       [0x004ccba0 body]
//      If uVar3 <= param_2 <= param_1[1]*param_1[0] + uVar3 → found, break.
//      Else: puVar1 = *puVar1 (advance).                          [0x004ccba0 body]
//      If sentinel reached → return NULL.                         [0x004ccba0 body]
//   4. Element index = (param_2 - uVar3) / param_1[0].            [0x004ccba0 body]
//   5. Clear bit in bitmap:
//      *(byte*)((int)puVar1 + (index>>3) + 8) &= ~(0x80 >> (index & 7)).  [0x004ccba0 body]
//   6. If param_1[6] & 2 (auto-release empty blocks):
//      Count set bytes in bitmap [0..uVar2); if sum != 0 → return param_1.
//      Else: unlink+free block via vtbl+0x10c.                    [0x004ccba0 body → DAT_007d3ff8+0x10c]
//   7. return param_1.                                             [0x004ccba0 body]
//
// Key globals (cited from 0x004ccba0 body):
//   0x007d3ff8 — RW device vtable base
//   vtbl +0x10c — raw memory free
//
// No callees — leaf-function exemption applies per CONFIDENCE.md.
//
// ref: re/analysis/promote_c2_render_d3d9/0x004ccba0.md
// ---------------------------------------------------------------------------

// 0x004ccba0
extern "C" __declspec(dllexport) std::uint32_t* __cdecl RwFreeListFree(
    std::uint32_t* param_1,   // RwFreeList*
    std::uint32_t  param_2)   // address of element to free
{
    // Step 1+2: block list head + bitmap row stride. [0x004ccba0 body]
    std::uint32_t* sentinel = param_1 + 4;    // +0x10 = sentinel
    std::uint32_t  uVar2    = param_1[2];      // bitmap row stride [0x004ccba0 body]
    std::uint32_t* puVar1   = reinterpret_cast<std::uint32_t*>(param_1[4]);  // first block

    // Step 3: walk chain to find containing block. [0x004ccba0 body]
    while (puVar1 != sentinel) {
        std::uint32_t blockData = reinterpret_cast<std::uint32_t>(puVar1) + uVar2 + 8u;
        if (blockData <= param_2 &&
            param_2 <= param_1[1] * param_1[0] + blockData) {
            break;   // found block
        }
        puVar1 = reinterpret_cast<std::uint32_t*>(*puVar1);
    }
    if (puVar1 == sentinel) {
        return nullptr;   // not found [0x004ccba0 body: sentinel check]
    }

    // Step 4: element index within block. [0x004ccba0 body]
    std::uint32_t blockData = reinterpret_cast<std::uint32_t>(puVar1) + uVar2 + 8u;
    std::uint32_t elemIdx   = (param_2 - blockData) / param_1[0];

    // Step 5: clear bit in allocation bitmap. [0x004ccba0 body]
    std::uint8_t* bitmap = reinterpret_cast<std::uint8_t*>(puVar1) + 8u;
    std::uint32_t byteIdx = elemIdx >> 3;
    bitmap[byteIdx] &= static_cast<std::uint8_t>(~(0x80u >> (elemIdx & 7u)));

    // Step 6: auto-release empty blocks if flag set. [0x004ccba0 body: param_1[6] & 2]
    if ((param_1[6] & 2u) != 0u) {
        std::int32_t sum = 0;
        for (std::uint32_t i = 0u; i < uVar2; ++i) {
            sum += bitmap[i];
        }
        if (sum != 0) {
            return param_1;   // block not fully free [0x004ccba0 body]
        }
        // Unlink and free the block. [0x004ccba0 body]
        *reinterpret_cast<std::uint32_t*>(puVar1[1]) = puVar1[0];
        *reinterpret_cast<std::uint32_t*>(puVar1[0] + 4u) = puVar1[1];

        typedef void (__cdecl *RawFreeFn)(void*);
        std::uint32_t vtbl = *s_RwVtableBase;
        RawFreeFn rawFree  = *reinterpret_cast<RawFreeFn*>(vtbl + 0x10cu);
        rawFree(puVar1);   // [0x004ccba0 body → vtbl+0x10c]
    }

    return param_1;   // [0x004ccba0 body]
}

RH_ScopedInstall(RwFreeListFree, 0x004ccba0);

// ---------------------------------------------------------------------------
// rwD3D9VBPoolDestroy  --  0x004cfe40
//
// Original: FUN_004cfe40 (148 bytes, 0x004cfe40..0x004cfed4)
// Signature:
//   void FUN_004cfe40(void)
//
// Destroys the vertex-buffer / index-buffer pool used by the D3D9 backend.
// Called exclusively from rwD3D9DeviceStop (FUN_004c8c70) at device teardown.
//
// Steps (cited from 0x004cfe40 body):
//   Phase 1 — drain linked list at DAT_007d46e0:
//     While DAT_007d46e0 != 0:
//       iVar1 = *(int*)(DAT_007d46e0 + 4)   (link at node+4)    [0x004cfe40 body]
//       vtbl+0x11c (DAT_007d46e4, DAT_007d46e0) — free into pool [0x004cfe40 body]
//       DAT_007d46e0 = iVar1
//     DAT_007d46e0 = 0                                           [0x004cfe40 body]
//
//   Phase 2 — destroy backing free-list DAT_007d46e4:
//     FUN_004cc9f0(DAT_007d46e4) — RwFreeListDestroy             [0x004cfe40 body → 0x004cc9f0]
//     DAT_007d46e4 = 0                                           [0x004cfe40 body]
//
//   Phase 3 — clear VB pool counters:
//     DAT_007d46cc = 0, DAT_007d46d0 = 0, DAT_007d46d4 = 0      [0x004cfe40 body]
//
//   Phase 4 — free raw allocation at DAT_007d46d8:
//     vtbl+0x10c (raw free); zero                                [0x004cfe40 body → vtbl+0x10c]
//
//   Phase 5 — destroy secondary free-list DAT_007d46dc:
//     FUN_004cc9f0(DAT_007d46dc); DAT_007d46dc = 0               [0x004cfe40 body → 0x004cc9f0]
//
// Key globals (cited from 0x004cfe40 body):
//   0x007d46e0 — VB node linked-list head
//   0x007d46e4 — primary free-list for VB nodes
//   0x007d46cc, 0x007d46d0, 0x007d46d4 — VB pool counters
//   0x007d46d8 — raw allocation (large buffer)
//   0x007d46dc — secondary free-list
//   0x007d3ff8 — RW device vtable base
//
// Callee: RwFreeListDestroy (0x004cc9f0) at C2 (see above in this file).
//
// ref: re/analysis/promote_c2_render_d3d9/0x004cfe40.md
// ---------------------------------------------------------------------------

// VB pool globals. [cited from 0x004cfe40 body]
static volatile std::uint32_t* const s_VbListHead    = reinterpret_cast<volatile std::uint32_t*>(0x007d46e0u);
static volatile std::uint32_t* const s_VbFreelist    = reinterpret_cast<volatile std::uint32_t*>(0x007d46e4u);
static volatile std::uint32_t* const s_VbCounterA    = reinterpret_cast<volatile std::uint32_t*>(0x007d46ccu);
static volatile std::uint32_t* const s_VbCounterB    = reinterpret_cast<volatile std::uint32_t*>(0x007d46d0u);
static volatile std::uint32_t* const s_VbCounterC    = reinterpret_cast<volatile std::uint32_t*>(0x007d46d4u);
static volatile std::uint32_t* const s_VbRawAlloc    = reinterpret_cast<volatile std::uint32_t*>(0x007d46d8u);
static volatile std::uint32_t* const s_VbFreelist2   = reinterpret_cast<volatile std::uint32_t*>(0x007d46dcu);

// Forward ref to callee (defined in this file above at 0x004cc9f0).
extern "C" __declspec(dllexport) std::uint32_t __cdecl RwFreeListDestroy(std::uint32_t*);

// 0x004cfe40
extern "C" __declspec(dllexport) void __cdecl rwD3D9VBPoolDestroy(void)
{
    typedef void (__cdecl *RawFreeFn)(void*);
    typedef void (__cdecl *PoolFreeFn)(std::uint32_t pool, void* ptr);

    // Phase 1: drain VB node linked list. [0x004cfe40 body]
    if (*s_VbListHead != 0u) {
        do {
            std::uint32_t node = *s_VbListHead;
            std::uint32_t next = *reinterpret_cast<std::uint32_t*>(node + 4u);

            std::uint32_t vtbl = *s_RwVtableBase;
            PoolFreeFn poolFree = *reinterpret_cast<PoolFreeFn*>(vtbl + 0x11cu);
            poolFree(*s_VbFreelist, reinterpret_cast<void*>(node));   // [0x004cfe40 body → vtbl+0x11c]

            *s_VbListHead = next;
        } while (*s_VbListHead != 0u);
        *s_VbListHead = 0u;
    }

    // Phase 2: destroy primary free-list. [0x004cfe40 body → 0x004cc9f0]
    if (*s_VbFreelist != 0u) {
        RwFreeListDestroy(reinterpret_cast<std::uint32_t*>(*s_VbFreelist));
        *s_VbFreelist = 0u;
    }

    // Phase 3: clear counters. [0x004cfe40 body]
    *s_VbCounterA = 0u;
    *s_VbCounterB = 0u;
    *s_VbCounterC = 0u;

    // Phase 4: free raw allocation. [0x004cfe40 body → vtbl+0x10c]
    if (*s_VbRawAlloc != 0u) {
        std::uint32_t vtbl = *s_RwVtableBase;
        RawFreeFn rawFree = *reinterpret_cast<RawFreeFn*>(vtbl + 0x10cu);
        rawFree(reinterpret_cast<void*>(*s_VbRawAlloc));
        *s_VbRawAlloc = 0u;
    }

    // Phase 5: destroy secondary free-list. [0x004cfe40 body → 0x004cc9f0]
    if (*s_VbFreelist2 != 0u) {
        RwFreeListDestroy(reinterpret_cast<std::uint32_t*>(*s_VbFreelist2));
        *s_VbFreelist2 = 0u;
    }
}

RH_ScopedInstall(rwD3D9VBPoolDestroy, 0x004cfe40);

// ---------------------------------------------------------------------------
// rwD3D9SkinVBPoolDestroy  --  0x004dcdc0
//
// Original: FUN_004dcdc0 (455 bytes, 0x004dcdc0..0x004dcf87)
// Signature:
//   void FUN_004dcdc0(void)
//
// Destroys the geometry/skin vertex-buffer pool cluster.
// Called from rwD3D9DeviceStop (0x004c8c70) and FUN_004dcd50 (4-pool init).
//
// Steps (cited from 0x004dcdc0 body):
//
//   Outer loop (uVar5 = 0, 4, 8, 0xC — 4 iterations × stride 4):
//     DAT_007d70cc[uVar5] = 0                                    [0x004dcdc0 body]
//     DAT_007d70dc[uVar5] = 0                                    [0x004dcdc0 body]
//     If DAT_007d70ec[uVar5] != 0 (live slot):
//       Walk VB cache list (DAT_007d70c0) for node where node+8 == DAT_007d70ec[uVar5].
//       Clear node+4 and node+0xc.                               [0x004dcdc0 body]
//       DAT_007d70ec[uVar5] = 0.                                 [0x004dcdc0 body]
//
//   VB cache node teardown (list DAT_007d70c0):
//     For each node:
//       Release() via vtbl+8 on the VB ptr at node+8.           [0x004dcdc0 body → vtbl+0x8]
//       Clear node+8, node+0xc.                                  [0x004dcdc0 body]
//       Free node via vtbl+0x11c into DAT_007d70c4.              [0x004dcdc0 body → vtbl+0x11c]
//       Advance via node+0x10.                                   [0x004dcdc0 body]
//     FUN_004cc9f0(DAT_007d70c4) — destroy VB-node free-list.   [0x004dcdc0 body → 0x004cc9f0]
//     DAT_007d70c4 = 0; DAT_007d70c0 = 0.                        [0x004dcdc0 body]
//
//   Geometry pool teardown (list DAT_007d70b0):
//     For each geometry node:
//       Drain node+4 linked list: free each element via vtbl+0x11c into DAT_007d70b8.
//       Drain node+8 linked list: Release() each ptr, free via vtbl+0x11c into DAT_007d70bc.
//       Free geometry node itself via vtbl+0x11c into DAT_007d70b4.
//       DAT_007d70b0 = node+0xc (advance).                       [0x004dcdc0 body]
//     FUN_004cc9f0(DAT_007d70bc);
//     FUN_004cc9f0(DAT_007d70b8);
//     FUN_004cc9f0(DAT_007d70b4).                                [0x004dcdc0 body → 0x004cc9f0 ×3]
//
// Key globals (cited from 0x004dcdc0 body):
//   0x007d70c0 — VB cache list head
//   0x007d70c4 — VB node free-list
//   0x007d70b0 — geometry pool list head
//   0x007d70b4 — geometry node free-list
//   0x007d70b8 — geometry elem-A free-list
//   0x007d70bc — geometry elem-B free-list
//   0x007d70c8 — zeroed at entry
//   0x007d70cc..0x007d70d8 — per-slot counter A (4 entries stride 4)
//   0x007d70dc..0x007d70e8 — per-slot counter B (4 entries stride 4)
//   0x007d70ec..0x007d70f8 — per-slot live-VB ptr (4 entries stride 4)
//   0x007d3ff8 — RW device vtable base
//
// Callee: RwFreeListDestroy (0x004cc9f0) — in this file.
//
// ref: re/analysis/promote_c2_render_d3d9/0x004dcdc0.md
// ---------------------------------------------------------------------------

// Geometry/skin VB pool globals. [cited from 0x004dcdc0 body]
static volatile std::uint32_t* const s_VbCacheHead    = reinterpret_cast<volatile std::uint32_t*>(0x007d70c0u);
static volatile std::uint32_t* const s_VbCacheFl      = reinterpret_cast<volatile std::uint32_t*>(0x007d70c4u);
static volatile std::uint32_t* const s_GeoPoolHead     = reinterpret_cast<volatile std::uint32_t*>(0x007d70b0u);
static volatile std::uint32_t* const s_GeoNodeFl       = reinterpret_cast<volatile std::uint32_t*>(0x007d70b4u);
static volatile std::uint32_t* const s_GeoElemAFl      = reinterpret_cast<volatile std::uint32_t*>(0x007d70b8u);
static volatile std::uint32_t* const s_GeoElemBFl      = reinterpret_cast<volatile std::uint32_t*>(0x007d70bcu);
// Per-slot arrays (4 entries, stride 4 bytes):
static volatile std::uint32_t* const s_SlotCounterA    = reinterpret_cast<volatile std::uint32_t*>(0x007d70ccu);  // [0..3]
static volatile std::uint32_t* const s_SlotCounterB    = reinterpret_cast<volatile std::uint32_t*>(0x007d70dcu);  // [0..3]
static volatile std::uint32_t* const s_SlotLiveVB      = reinterpret_cast<volatile std::uint32_t*>(0x007d70ecu);  // [0..3]

// 0x004dcdc0
extern "C" __declspec(dllexport) void __cdecl rwD3D9SkinVBPoolDestroy(void)
{
    typedef void     (__cdecl *RawFreeFn)  (void*);
    typedef void     (__cdecl *PoolFreeFn) (std::uint32_t pool, void* ptr);
    typedef std::uint32_t (__cdecl *ReleaseFn)(void*);

    // Outer loop: 4 slots (stride 4 bytes → slot indices 0,1,2,3). [0x004dcdc0 body]
    for (std::uint32_t uVar5 = 0u; uVar5 <= 0xCu; uVar5 += 4u) {
        s_SlotCounterA[uVar5 / 4u] = 0u;   // [0x004dcdc0 body]
        s_SlotCounterB[uVar5 / 4u] = 0u;   // [0x004dcdc0 body]

        std::uint32_t liveVB = s_SlotLiveVB[uVar5 / 4u];
        if (liveVB != 0u) {
            // Walk VB cache list, find node where node+8 == liveVB. [0x004dcdc0 body]
            std::uint32_t node = *s_VbCacheHead;
            while (node != 0u) {
                std::uint32_t nodeVb = *reinterpret_cast<std::uint32_t*>(node + 8u);
                if (nodeVb == liveVB) {
                    *reinterpret_cast<std::uint32_t*>(node + 4u)  = 0u;   // [0x004dcdc0 body]
                    *reinterpret_cast<std::uint32_t*>(node + 0xcu) = 0u;  // [0x004dcdc0 body]
                    break;
                }
                node = *reinterpret_cast<std::uint32_t*>(node + 0x10u);
            }
            s_SlotLiveVB[uVar5 / 4u] = 0u;   // [0x004dcdc0 body]
        }
    }

    // VB cache node teardown. [0x004dcdc0 body]
    {
        std::uint32_t vtbl = *s_RwVtableBase;
        RawFreeFn  rawFree  = *reinterpret_cast<RawFreeFn*> (vtbl + 0x10cu);
        PoolFreeFn poolFree = *reinterpret_cast<PoolFreeFn*>(vtbl + 0x11cu);
        ReleaseFn  release  = *reinterpret_cast<ReleaseFn*> (vtbl + 0x8u);

        std::uint32_t node = *s_VbCacheHead;
        while (node != 0u) {
            std::uint32_t nextNode = *reinterpret_cast<std::uint32_t*>(node + 0x10u);
            std::uint32_t vbPtr    = *reinterpret_cast<std::uint32_t*>(node + 8u);
            if (vbPtr != 0u) {
                release(reinterpret_cast<void*>(vbPtr));   // Release() vtbl+0x8 [0x004dcdc0 body]
                *reinterpret_cast<std::uint32_t*>(node + 8u)  = 0u;
                *reinterpret_cast<std::uint32_t*>(node + 0xcu) = 0u;
            }
            poolFree(*s_VbCacheFl, reinterpret_cast<void*>(node));  // vtbl+0x11c [0x004dcdc0 body]
            node = nextNode;
        }
        // Destroy VB-node free-list. [0x004dcdc0 body → 0x004cc9f0]
        if (*s_VbCacheFl != 0u) {
            RwFreeListDestroy(reinterpret_cast<std::uint32_t*>(*s_VbCacheFl));
        }
        *s_VbCacheFl  = 0u;
        *s_VbCacheHead = 0u;

        // Geometry pool teardown. [0x004dcdc0 body]
        while (*s_GeoPoolHead != 0u) {
            std::uint32_t gnode = *s_GeoPoolHead;

            // Drain node+4 element-A list. [0x004dcdc0 body]
            std::uint32_t elemA = *reinterpret_cast<std::uint32_t*>(gnode + 4u);
            while (elemA != 0u) {
                std::uint32_t nextA = *reinterpret_cast<std::uint32_t*>(elemA);
                poolFree(*s_GeoElemAFl, reinterpret_cast<void*>(elemA));  // [0x004dcdc0 body → vtbl+0x11c]
                elemA = nextA;
            }

            // Drain node+8 element-B list (Release() each). [0x004dcdc0 body]
            std::uint32_t elemB = *reinterpret_cast<std::uint32_t*>(gnode + 8u);
            while (elemB != 0u) {
                std::uint32_t nextB = *reinterpret_cast<std::uint32_t*>(elemB);
                release(reinterpret_cast<void*>(elemB));                  // vtbl+0x8 [0x004dcdc0 body]
                poolFree(*s_GeoElemBFl, reinterpret_cast<void*>(elemB)); // vtbl+0x11c [0x004dcdc0 body]
                elemB = nextB;
            }

            // Free geometry node. [0x004dcdc0 body → vtbl+0x11c]
            std::uint32_t nextGnode = *reinterpret_cast<std::uint32_t*>(gnode + 0xcu);
            poolFree(*s_GeoNodeFl, reinterpret_cast<void*>(gnode));
            *s_GeoPoolHead = nextGnode;
        }

        // Destroy three geometry free-lists. [0x004dcdc0 body → 0x004cc9f0 ×3]
        if (*s_GeoElemBFl != 0u) {
            RwFreeListDestroy(reinterpret_cast<std::uint32_t*>(*s_GeoElemBFl));
            *s_GeoElemBFl = 0u;
        }
        if (*s_GeoElemAFl != 0u) {
            RwFreeListDestroy(reinterpret_cast<std::uint32_t*>(*s_GeoElemAFl));
            *s_GeoElemAFl = 0u;
        }
        if (*s_GeoNodeFl != 0u) {
            RwFreeListDestroy(reinterpret_cast<std::uint32_t*>(*s_GeoNodeFl));
            *s_GeoNodeFl = 0u;
        }
    }
}

RH_ScopedInstall(rwD3D9SkinVBPoolDestroy, 0x004dcdc0);

// ---------------------------------------------------------------------------
// RwStreamFindChunk  --  0x004cc5e0
//
// Original: FUN_004cc5e0 (243 bytes, 0x004cc5e0..0x004cc6cf)
// Signature:
//   undefined4 FUN_004cc5e0(undefined4 param_1, int param_2,
//                           undefined4 *param_3, uint *param_4)
//   param_1 = RwStream* (opaque stream context)
//   param_2 = target chunk type ID to locate
//   param_3 = optional output — chunk size (written at 0x004cc664)
//   param_4 = optional output — chunk version (written at 0x004cc66e)
//   returns: 1 on success, 0 on read failure or type not found
//
// Scans an RW stream for a specific chunk type ID, skipping intervening chunks.
// Validates the RW version is in range [0x35000, 0x37002] (= RenderWare 3.x).
// Mechanically identical to RwStreamFindChunk() in the RW3 SDK.
//
// Steps (cited from 0x004cc5e0 body):
//   1. FUN_004cc400(stream, &local_c, &param_1, &local_8, 0) — read first chunk header.
//      local_c = chunk type; param_1 (reused) = size; local_8 = version.   [0x004cc5e0 body]
//   2. If read fails (iVar3 == 0) → return 0.                               [0x004cc5e0 body]
//   3. While local_c != param_2:
//      FUN_004cc050(stream, size) — skip body.                              [0x004cc5e0 body → 0x004cc050]
//      FUN_004cc400(stream, ...) — read next header.                        [0x004cc5e0 body → 0x004cc400]
//      Return 0 if either fails.
//   4. Version check: if local_8 <= 0x34fff or local_8 > 0x37002:
//      FUN_004d7ff0(0x80000004) + return 0.                                 [0x004cc5e0 body: version gate]
//   5. If param_3 != NULL: *param_3 = size.                                 [0x004cc5e0 body: 0x004cc664]
//   6. If param_4 != NULL: *param_4 = version.                              [0x004cc5e0 body: 0x004cc66e]
//   7. Return 1.
//
// Version range 0x35000..0x37002 = RW 3.3.0.0 to ~3.7.0.2 (matches librw/gta-reversed).
//
// Callees:
//   0x004cc400 FUN_004cc400 — RwStreamReadChunkHeader (C2)
//   0x004cc050 FUN_004cc050 — RwStreamSkip (C3, in TextureLoader_q6.cpp)
//   0x004d7ff0 FUN_004d7ff0 — RwIdentityPassthrough (C3, in HighAB3Helpers_p6.cpp)
//
// ref: re/analysis/texture_loader_d2/0x004cc5e0.md
// ---------------------------------------------------------------------------

// Forward declarations for callees.
// RwIdentityPassthrough (C3; error-code relay). [0x004d7ff0]
typedef std::uint32_t (__cdecl *RwErrPassthrough_t)(std::uint32_t errCode);
static const RwErrPassthrough_t s_FUN_004d7ff0 =
    reinterpret_cast<RwErrPassthrough_t>(0x004d7ff0u);
// RwStreamReadChunkHeader (C2; reads 12-byte RW chunk header). [0x004cc400]
typedef std::uint32_t (__cdecl *RwReadChunkHdr_t)(std::uint32_t stream,
                                                   std::uint32_t* outType,
                                                   std::uint32_t* outSize,
                                                   std::uint32_t* outVersion,
                                                   std::uint32_t reserved);
static const RwReadChunkHdr_t s_FUN_004cc400 =
    reinterpret_cast<RwReadChunkHdr_t>(0x004cc400u);
// RwStreamSkip (C3). [0x004cc050]
typedef std::uint32_t (__cdecl *RwStreamSkip_t)(std::uint32_t stream, std::uint32_t size);
static const RwStreamSkip_t s_FUN_004cc050 =
    reinterpret_cast<RwStreamSkip_t>(0x004cc050u);

// 0x004cc5e0
extern "C" __declspec(dllexport) std::uint32_t __cdecl RwStreamFindChunk(
    std::uint32_t  param_1,   // RwStream*
    std::int32_t   param_2,   // target chunk type ID
    std::uint32_t* param_3,   // optional output: chunk size
    std::uint32_t* param_4)   // optional output: chunk version
{
    std::uint32_t local_c;    // chunk type
    std::uint32_t chunkSize;  // chunk data size (reuses param_1 address in original)
    std::uint32_t local_8;    // chunk version

    // Step 1: read first chunk header. [0x004cc5e0 body]
    // Note: original reuses &param_1 as the &size output — we use a separate local.
    std::int32_t iVar3 = static_cast<std::int32_t>(
        s_FUN_004cc400(param_1, &local_c, &chunkSize, &local_8, 0u));
    if (iVar3 == 0) {
        return 0u;   // read failure [0x004cc5e0 body]
    }

    // Step 3: scan until type matches. [0x004cc5e0 body]
    while (static_cast<std::int32_t>(local_c) != param_2) {
        // Skip this chunk body. [0x004cc5e0 body → 0x004cc050]
        if (s_FUN_004cc050(param_1, chunkSize) == 0u) {
            return 0u;
        }
        // Read next header. [0x004cc5e0 body → 0x004cc400]
        iVar3 = static_cast<std::int32_t>(
            s_FUN_004cc400(param_1, &local_c, &chunkSize, &local_8, 0u));
        if (iVar3 == 0) {
            return 0u;
        }
    }

    // Step 4: version range check [0x35000, 0x37002]. [0x004cc5e0 body: 0x004cc637, 0x004cc645]
    if (local_8 <= 0x34fffu || local_8 > 0x37002u) {
        s_FUN_004d7ff0(0x80000004u);   // error passthrough [0x004cc5e0 body → 0x004d7ff0]
        return 0u;
    }

    // Step 5+6: write optional outputs. [0x004cc5e0 body: 0x004cc664, 0x004cc66e]
    if (param_3 != nullptr) {
        *param_3 = chunkSize;
    }
    if (param_4 != nullptr) {
        *param_4 = local_8;
    }

    return 1u;   // success [0x004cc5e0 body]
}

RH_ScopedInstall(RwStreamFindChunk, 0x004cc5e0);
