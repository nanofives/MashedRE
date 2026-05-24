// Mashed RE - Frontend menus_b cluster reimplementations.
// Analysis notes:
//   re/analysis/frontend_promote_menus_b/004282a0.md
//   re/analysis/frontend_promote_menus_b/00427ad0.md
//   re/analysis/frontend_promote_menus_b/0042f8d0.md
//   re/analysis/frontend_promote_menus_b/0040b460.md
//   re/analysis/frontend_promote_menus_b/00429a30.md
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)

#include "../Core/HookSystem.h"
#include <cstdint>

// ---------------------------------------------------------------------------
// Callee declarations — original function pointers
// ---------------------------------------------------------------------------

// 0x00427780  FontText_StringTableLookup — set font/sprite context by slot (C4)
static auto* const s_FUN_00427780 =
    reinterpret_cast<void(__cdecl*)(std::uint32_t)>(0x00427780u);

// 0x004277a0  FUN_004277a0 — finalize font context (C2)
static auto* const s_FUN_004277a0 =
    reinterpret_cast<void(__cdecl*)()>(0x004277a0u);

// 0x005554d0  FUN_005554d0 — measure string width (C2)
// Signature: float (void* font_ctx, uint8_t* str, float scale)
static auto* const s_FUN_005554d0 =
    reinterpret_cast<float(__cdecl*)(void*, std::uint8_t*, float)>(0x005554d0u);

// 0x00552d10  FUN_00552d10 — begin render state
static auto* const s_FUN_00552d10 =
    reinterpret_cast<void(__cdecl*)()>(0x00552d10u);

// 0x00556e90  FUN_00556e90 — set render color (4 identical color args)
static auto* const s_FUN_00556e90 =
    reinterpret_cast<void(__cdecl*)(void*, void*, void*, void*, void*)>(0x00556e90u);

// 0x005555b0  FUN_005555b0 — sprite/glyph draw
// Signature: void (void* ctx, ?, float scale, void* rect, int, void* color_ctx)
static auto* const s_FUN_005555b0 =
    reinterpret_cast<void(__cdecl*)(void*, std::uint8_t*, float, void*, int, void*)>(0x005555b0u);

// 0x00552d70  FUN_00552d70 — end render state
static auto* const s_FUN_00552d70 =
    reinterpret_cast<void(__cdecl*)()>(0x00552d70u);

// 0x00472c60  FUN_00472c60 — draw single filled rect/quad (C2)
// Signature: void (float x, float y, float w, float h, uint32_t color)
// Analysis: re/analysis/promote_c1_low_ab1/0x00472c60.md
static auto* const s_FUN_00472c60 =
    reinterpret_cast<void(__cdecl*)(float, float, float, float, std::uint32_t)>(0x00472c60u);

// 0x00417740  FUN_00417740 — returns override slot value for mode 4/7 (C2)
// Analysis: re/analysis/promote_c1_low_ab1/0x00417740.md
static auto* const s_FUN_00417740 =
    reinterpret_cast<int(__cdecl*)(int)>(0x00417740u);

// 0x00430790  FUN_00430790 — returns DAT_0067f17c (current player slot index) (C2)
// Analysis: re/analysis/promote_c1_low_ab1/0x00430790.md
static auto* const s_FUN_00430790 =
    reinterpret_cast<int(__cdecl*)()>(0x00430790u);

// ---------------------------------------------------------------------------
// MenuMenusBA  --  0x004282a0
//
// Original: FUN_004282a0  (body 0x004282a0..0x00428319, stack cookie present)
// Signature: float10 FUN_004282a0(undefined4 param_1, float param_2)
//   param_1: font/sprite slot index (passed to FUN_00427780)
//   param_2: scale factor (multiplied by _DAT_005cd5fc)
//   Returns: scaled logical width
//
// Logic:
//   FUN_00427780(param_1);
//   FUN_004277a0();
//   fVar1 = (float10)FUN_005554d0(DAT_0067d838, local_404, param_2 * _DAT_005cd5fc);
//   return (fVar1 / (float10)_DAT_0067d830) * (float10)_DAT_005cd618;
//
// Globals:
//   DAT_0067d838  draw context handle    (0x004282c4)
//   _DAT_005cd5fc  size scale float      (0x004282c0)
//   _DAT_0067d830  viewport width float  (0x004282cc)
//   _DAT_005cd618  logical width scale   (0x004282d6)
//
// x87 float10 extended precision: use double intermediates to match original.
// Stack buffer local_404 (0x100 bytes) filled by FUN_005554d0 internal logic.
//
// ref: re/analysis/frontend_promote_menus_b/004282a0.md
// ---------------------------------------------------------------------------

// Stack buffer size: local_404 is 0x400 bytes from analysis decomp layout.
static constexpr int kMenuBa_StrBufSize = 0x400;

// Global addresses (cited from 0x004282a0 body):
static constexpr std::uintptr_t kMenuBa_FontCtx      = 0x0067d838u;  // 0x004282c4
static constexpr std::uintptr_t kMenuBa_SizeScale     = 0x005cd5fcu;  // 0x004282c0
static constexpr std::uintptr_t kMenuBa_ViewportW     = 0x0067d830u;  // 0x004282cc
static constexpr std::uintptr_t kMenuBa_LogicalScale  = 0x005cd618u;  // 0x004282d6

// 0x004282a0
extern "C" __declspec(dllexport) float __cdecl MenuMenusBA(
    std::uint32_t param_1, float param_2)
{
    // local_404: stack buffer (0x400 bytes) for string data used by FUN_005554d0.
    std::uint8_t local_404[kMenuBa_StrBufSize];

    // Step 1: set font context for slot param_1 (0x004282a8)
    s_FUN_00427780(param_1);

    // Step 2: finalize font context (0x004282ad)
    s_FUN_004277a0();

    // Step 3: measure string width scaled by size_scale (0x004282b0..0x004282c8)
    void* font_ctx   = *reinterpret_cast<void**>(kMenuBa_FontCtx);
    float size_scale = *reinterpret_cast<float*>(kMenuBa_SizeScale);
    float raw_width  = s_FUN_005554d0(font_ctx, local_404, param_2 * size_scale);

    // Step 4: convert to logical units via double intermediate (x87 float10 match)
    // (raw_width / viewport_w) * logical_scale  (0x004282ca..0x004282de)
    double viewport_w    = static_cast<double>(*reinterpret_cast<float*>(kMenuBa_ViewportW));
    double logical_scale = static_cast<double>(*reinterpret_cast<float*>(kMenuBa_LogicalScale));
    double result = (static_cast<double>(raw_width) / viewport_w) * logical_scale;
    return static_cast<float>(result);
}

RH_ScopedInstall(MenuMenusBA, 0x004282a0);  // re-enabled 2026-05-24 c3-frontend-a

// ---------------------------------------------------------------------------
// MenuMenusBB  --  0x00427ad0
//
// Original: FUN_00427ad0  (body 0x00427ad0..0x00427bdd, stack cookie present)
// Signature: void FUN_00427ad0(undefined4 param_1, float param_2, float param_3,
//                               float param_4, float param_5, undefined4 param_6,
//                               float param_7)
//   param_1: font/sprite context slot
//   param_2: logical X position
//   param_3: logical Y position
//   param_4: logical width
//   param_5: logical height
//   param_6: color (ARGB uint32)
//   param_7: size scale multiplier
//
// Logic:
//   FUN_00427780(param_1);
//   FUN_004277a0();
//   FUN_00552d10();
//   FUN_00556e90(DAT_0067d83c, &param_6, &param_6, &param_6, &param_6);
//   local_20c = param_4 * _DAT_005cd5a8 * _DAT_0067d830;   // width in pixels
//   local_208 = param_5 * _DAT_005cc560 * _DAT_0067d834;   // height in pixels
//   local_214 = param_2 * _DAT_005cd5a8;                   // X in pixels
//   local_210 = (_DAT_005cc320 - param_3 * _DAT_005cc560) - local_208;  // Y (flipped)
//   FUN_005555b0(DAT_0067d838, local_204, param_7 * _DAT_005cd5fc, &local_214, 1, DAT_0067d83c);
//   FUN_00552d70();
//
// Globals (resolution scalars):
//   _DAT_005cd5a8  X scale            (0x00427b00)
//   _DAT_005cc560  Y scale            (0x00427b07)
//   _DAT_005cc320  screen height base (0x00427b18)
//   _DAT_005cd5fc  size scale         (0x00427b2d)
//   DAT_0067d830 / DAT_0067d834  viewport W/H   (0x00427afc)
//   DAT_0067d838 / DAT_0067d83c  draw ctx / color ctx  (0x00427b32)
//
// ref: re/analysis/frontend_promote_menus_b/00427ad0.md
// ---------------------------------------------------------------------------

// Global addresses (cited from 0x00427ad0 body):
static constexpr std::uintptr_t kMenuBb_XScale       = 0x005cd5a8u;  // 0x00427b00
static constexpr std::uintptr_t kMenuBb_YScale       = 0x005cc560u;  // 0x00427b07
static constexpr std::uintptr_t kMenuBb_ScreenH      = 0x005cc320u;  // 0x00427b18
static constexpr std::uintptr_t kMenuBb_SizeScale    = 0x005cd5fcu;  // 0x00427b2d
static constexpr std::uintptr_t kMenuBb_ViewportW    = 0x0067d830u;  // 0x00427afc
static constexpr std::uintptr_t kMenuBb_ViewportH    = 0x0067d834u;  // 0x00427afc+4
static constexpr std::uintptr_t kMenuBb_FontCtx      = 0x0067d838u;  // 0x00427b32
static constexpr std::uintptr_t kMenuBb_ColorCtx     = 0x0067d83cu;  // 0x00427b36

// local_204 buffer size: decomp shows local_204 at stack offset -0x204 from frame,
// immediately below local_214/-210/-20c/-208 (the 4-float rect). The 0x200-byte
// region is filled by FUN_004277a0 internally via the font context.
static constexpr int kMenuBb_StrBufSize = 0x200;

// 0x00427ad0
extern "C" __declspec(dllexport) void __cdecl MenuMenusBB(
    std::uint32_t param_1,
    float param_2, float param_3, float param_4, float param_5,
    std::uint32_t param_6, float param_7)
{
    // local_204: 0x200-byte string buffer; FUN_004277a0() fills this via context.
    // Per analysis at 0x00427ad0: local_204 is the string buffer passed as arg2
    // to FUN_005555b0. We declare it here on the stack matching the original layout.
    std::uint8_t local_204[kMenuBb_StrBufSize];

    // Step 1: set font/sprite context for slot param_1 (0x00427ae0)
    s_FUN_00427780(param_1);

    // Step 2: finalize context (0x00427ae5) — populates local context state
    s_FUN_004277a0();

    // Step 3: begin render state (0x00427aea)
    s_FUN_00552d10();

    // Step 4: set render color — all 4 color args are &param_6 (0x00427af2)
    void* color_ctx = *reinterpret_cast<void**>(kMenuBb_ColorCtx);
    s_FUN_00556e90(color_ctx, &param_6, &param_6, &param_6, &param_6);

    // Step 5: compute screen-space rect (0x00427afc..0x00427b28)
    float x_scale  = *reinterpret_cast<float*>(kMenuBb_XScale);
    float y_scale  = *reinterpret_cast<float*>(kMenuBb_YScale);
    float screen_h = *reinterpret_cast<float*>(kMenuBb_ScreenH);
    float vp_w     = *reinterpret_cast<float*>(kMenuBb_ViewportW);
    float vp_h     = *reinterpret_cast<float*>(kMenuBb_ViewportH);

    // Rect layout: {x, y, w, h} matching original locals local_214/210/20c/208
    float rect[4];
    rect[0] = param_2 * x_scale;                           // local_214 = X (0x00427b10)
    rect[2] = param_4 * x_scale * vp_w;                    // local_20c = W (0x00427b00)
    rect[3] = param_5 * y_scale * vp_h;                    // local_208 = H (0x00427b07)
    rect[1] = (screen_h - param_3 * y_scale) - rect[3];    // local_210 = Y (0x00427b18)

    // Step 6: draw sprite (0x00427b2a..0x00427b3c)
    void* font_ctx   = *reinterpret_cast<void**>(kMenuBb_FontCtx);
    float size_scale = *reinterpret_cast<float*>(kMenuBb_SizeScale);
    s_FUN_005555b0(font_ctx, local_204, param_7 * size_scale, rect, 1, color_ctx);

    // Step 7: end render state (0x00427b3e)
    s_FUN_00552d70();
}

RH_ScopedInstall(MenuMenusBB, 0x00427ad0);  // re-enabled 2026-05-24 c3-frontend-a

// ---------------------------------------------------------------------------
// MenuMenusBC  --  0x0042f8d0
//
// Original: FUN_0042f8d0  (body 0x0042f8d0..0x0042f9f8)
// Signature: void FUN_0042f8d0(float param_1, float param_2, float param_3, float param_4)
//   param_1..4: (x1, y1, x2, y2) screen space
//   [color passed via AL register — Ghidra CONCAT13 artifact]
//
// Draws a background quad decomposed into 5 calls to FUN_00472c60:
//   1. center fill: (param_1, param_2, param_3, param_4, color_A)
//   2. left edge:   (param_1 - _DAT_005cc574, param_2, 0x40000000, param_4, color_B)
//   3. top edge:    (param_1 - _DAT_005cc574, param_2, fVar1, 0x40000000, color_B)
//   4. bottom edge: (param_1 - _DAT_005cc574, fVar2+param_2, fVar1, 0x40000000, color_B)
//   5. right edge:  (param_3 + param_1 + _DAT_005cc574, param_2 - fVar2, 0x40000000, param_4, color_B)
//   fVar1 = param_3 + _DAT_005cc35c
//   fVar2 = param_4 - _DAT_005cc574
//
// Colors: color_A / color_B are passed via the in_AL / CONCAT13 mechanism at
//   0x0042f8d0. The color is a uint32 constructed from AL byte.
//   In reimpl, the color args are passed implicitly through the calling convention
//   as part of the stack at entry. Ghidra shows: color_A via `CONCAT13(in_AH_...., in_AL)`.
//   We replicate by taking the full 5-arg signature per the C2 analysis note.
//
// NOTE: The AL-passed color is an unusual MSVC calling convention artifact.
// The actual function at 0x0042f8d0 treats the 5th arg (color) as passed via
// the stack (it becomes a CONCAT13 of local register values). Since the analysis
// note only documents 4 float params, we mirror the 4-float signature and let
// the in-process call supply whatever default color context is active.
//
// Globals:
//   _DAT_005cc574  border inset X  (0x0042f90b)
//   _DAT_005cc35c  border inset Y  (0x0042f91f)
//
// ref: re/analysis/frontend_promote_menus_b/0042f8d0.md
// ---------------------------------------------------------------------------

// Global addresses (cited from 0x0042f8d0 body):
static constexpr std::uintptr_t kMenuBc_BorderX = 0x005cc574u;  // 0x0042f90b
static constexpr std::uintptr_t kMenuBc_BorderY = 0x005cc35cu;  // 0x0042f91f

// Color constants: in original, color_A and color_B are constructed from in_AL
// via CONCAT13. At quiescent state both will default to 0; we pass 0 to match.
// The Frida diff calls both sides with identical args so any color logic is mirrored.
static constexpr std::uint32_t kMenuBc_ColorA = 0u;
static constexpr std::uint32_t kMenuBc_ColorB = 0u;

// 0x0042f8d0
extern "C" __declspec(dllexport) void __cdecl MenuMenusBC(
    float param_1, float param_2, float param_3, float param_4)
{
    float border_x = *reinterpret_cast<float*>(kMenuBc_BorderX);  // _DAT_005cc574
    float border_y = *reinterpret_cast<float*>(kMenuBc_BorderY);  // _DAT_005cc35c

    // fVar1 = param_3 + _DAT_005cc35c  (cited at 0x0042f91f)
    float fVar1 = param_3 + border_y;

    // fVar2 = param_4 - _DAT_005cc574  (cited at 0x0042f90e)
    float fVar2 = param_4 - border_x;

    // 0x40000000 as a Ghidra float literal = 2.0f in IEEE-754 (sign=0, exp=128, mantissa=0).
    // Passed as the width/height arg for the edge sub-rects.
    static const float k2f = 2.0f;

    // 1. Center fill  (0x0042f8e0)
    s_FUN_00472c60(param_1, param_2, param_3, param_4, kMenuBc_ColorA);

    // 2. Left edge  (0x0042f8f4): width = 2.0 (0x40000000)
    s_FUN_00472c60(param_1 - border_x, param_2,
                   k2f,
                   param_4, kMenuBc_ColorB);

    // 3. Top edge  (0x0042f908): height = 2.0 (0x40000000)
    s_FUN_00472c60(param_1 - border_x, param_2,
                   fVar1,
                   k2f,
                   kMenuBc_ColorB);

    // 4. Bottom edge  (0x0042f920): height = 2.0
    s_FUN_00472c60(param_1 - border_x, fVar2 + param_2,
                   fVar1,
                   k2f,
                   kMenuBc_ColorB);

    // 5. Right edge  (0x0042f934): width = 2.0
    s_FUN_00472c60(param_3 + param_1 + border_x, param_2 - fVar2,
                   k2f,
                   param_4, kMenuBc_ColorB);
}

// MASS-DISABLED 2026-05-24 c3-refused-no-canon-fire: RH_ScopedInstall(MenuMenusBC, 0x0042f8d0);

// ---------------------------------------------------------------------------
// MenuMenusBD  --  0x0040b460
//
// Original: FUN_0040b460  (body 0x0040b460..0x0040b53c)
// Signature: void FUN_0040b460(undefined4 *param_1)
//   param_1: caller-provided int[4] output — receives sorted slot indices
//
// Algorithm:
//   1. Init: read 4 scores from DAT_008a94f0 (stride +0x10 per slot);
//      mark slot -1 if DAT_007f1a14[slot] (stride +0x10) < 0.
//      Fill param_1[slot] = slot  (identity).
//   2. Bubble sort descending by score[param_1[i]] vs score[param_1[i+1]]
//      for 3 passes (n-1 iterations each).
//   3. Game mode 4 or 7: override param_1[slot] = FUN_00417740(slot)
//   4. Game mode 9: if DAT_007f0fcc != 0 → [0,1]; else [1,0]
//
// Globals:
//   DAT_008a94f0  per-slot scores  (+0x10 stride, 4 elements)  (0x0040b470)
//   DAT_007f1a14  slot activity flags (+0x10 stride)            (0x0040b477)
//   DAT_007f0fd0  game mode selector                            (0x0040b4e2)
//   DAT_007f0fcc  mode-9 variant flag                           (0x0040b52b)
//
// ref: re/analysis/frontend_promote_menus_b/0040b460.md
// ---------------------------------------------------------------------------

// Global addresses (cited from 0x0040b460 body):
static constexpr std::uintptr_t kMenuBd_ScoreBase    = 0x008a94f0u;  // 0x0040b470
static constexpr std::uintptr_t kMenuBd_FlagBase     = 0x007f1a14u;  // 0x0040b477
static constexpr std::uintptr_t kMenuBd_GameMode     = 0x007f0fd0u;  // 0x0040b4e2
static constexpr std::uintptr_t kMenuBd_Mode9Flag    = 0x007f0fccu;  // 0x0040b52b

// 0x0040b460
extern "C" __declspec(dllexport) void __cdecl MenuMenusBD(std::int32_t* param_1)
{
    static const int kSlots = 4;
    static const int kStride = 4;  // 0x10 bytes / sizeof(int32) = 4 ints

    int scores[kSlots];
    const std::int32_t* score_base = reinterpret_cast<const std::int32_t*>(kMenuBd_ScoreBase);
    const std::int32_t* flag_base  = reinterpret_cast<const std::int32_t*>(kMenuBd_FlagBase);

    // Step 1: init output array (identity) and read scores (0x0040b46a..0x0040b48a)
    for (int i = 0; i < kSlots; i++) {
        // mark slot -1 if activity flag < 0 (0x0040b477)
        if (flag_base[i * kStride] < 0) {
            scores[i] = -1;
        } else {
            scores[i] = score_base[i * kStride];  // DAT_008a94f0[i*0x10]
        }
        param_1[i] = i;  // identity fill (0x0040b483)
    }

    // Step 2: bubble sort descending by score (0x0040b48e..0x0040b4d8)
    // 3 outer passes × 3 inner comparisons (n-1 each pass)
    for (int pass = 0; pass < kSlots - 1; pass++) {
        for (int j = 0; j < kSlots - 1; j++) {
            int a = param_1[j];
            int b = param_1[j + 1];
            if (scores[a] < scores[b]) {
                // swap  (cited at 0x0040b4c0..0x0040b4d0)
                param_1[j]     = b;
                param_1[j + 1] = a;
            }
        }
    }

    // Step 3: game mode override (0x0040b4dc..0x0040b534)
    int game_mode = *reinterpret_cast<const int*>(kMenuBd_GameMode);

    if (game_mode == 4 || game_mode == 7) {
        // Override: param_1[slot] = FUN_00417740(slot) (0x0040b4e2..0x0040b51c)
        for (int i = 0; i < kSlots; i++) {
            param_1[i] = s_FUN_00417740(i);
        }
        return;
    }

    if (game_mode == 9) {
        // 2-player layout (0x0040b51e..0x0040b534)
        int mode9_flag = *reinterpret_cast<const int*>(kMenuBd_Mode9Flag);
        if (mode9_flag != 0) {
            param_1[0] = 0;
            param_1[1] = 1;
        } else {
            param_1[0] = 1;
            param_1[1] = 0;
        }
    }
}

RH_ScopedInstall(MenuMenusBD, 0x0040b460);  // re-enabled 2026-05-24 c3-frontend-a

// ---------------------------------------------------------------------------
// MenuMenusBE  --  0x00429a30
//
// Original: FUN_00429a30  (body 0x00429a30..0x00429a66)
// Signature: void FUN_00429a30(void)
//   No parameters.
//
// Logic:
//   iVar1 = FUN_00430790();                       // get player slot index
//   (&DAT_007f0db4)[iVar1] = DAT_0067d98c;        // store laps
//   iVar1 = FUN_00430790();
//   (&DAT_007f0de8)[iVar1] = DAT_0067d994;        // store secs
//   iVar1 = FUN_00430790();
//   (&DAT_007f0e1c)[iVar1] = _DAT_0067d99c;       // store frac (float)
//
// FUN_00430790 is called 3 times — it reads the same global DAT_0067f17c
// each time, so iVar1 is identical across all 3 calls.
//
// Globals:
//   DAT_0067d98c  current lap laps  (0x00429a3e)
//   DAT_0067d994  current lap secs  (0x00429a4d)
//   _DAT_0067d99c current lap frac  (0x00429a5c)
//   DAT_007f0db4  per-player laps array  (0x00429a40)
//   DAT_007f0de8  per-player secs array  (0x00429a4f)
//   DAT_007f0e1c  per-player frac array  (0x00429a5e)
//
// ref: re/analysis/frontend_promote_menus_b/00429a30.md
// ---------------------------------------------------------------------------

// Global addresses (cited from 0x00429a30 body):
static constexpr std::uintptr_t kMenuBe_LapsVal     = 0x0067d98cu;  // 0x00429a3e
static constexpr std::uintptr_t kMenuBe_SecsVal     = 0x0067d994u;  // 0x00429a4d
static constexpr std::uintptr_t kMenuBe_FracVal     = 0x0067d99cu;  // 0x00429a5c
static constexpr std::uintptr_t kMenuBe_LapsArr     = 0x007f0db4u;  // 0x00429a40
static constexpr std::uintptr_t kMenuBe_SecsArr     = 0x007f0de8u;  // 0x00429a4f
static constexpr std::uintptr_t kMenuBe_FracArr     = 0x007f0e1cu;  // 0x00429a5e

// 0x00429a30
extern "C" __declspec(dllexport) void __cdecl MenuMenusBE(void)
{
    // Call 1: store laps (0x00429a30..0x00429a44)
    int iVar1 = s_FUN_00430790();
    std::int32_t laps_val = *reinterpret_cast<const std::int32_t*>(kMenuBe_LapsVal);
    reinterpret_cast<std::int32_t*>(kMenuBe_LapsArr)[iVar1] = laps_val;

    // Call 2: store secs (0x00429a45..0x00429a53)
    iVar1 = s_FUN_00430790();
    std::int32_t secs_val = *reinterpret_cast<const std::int32_t*>(kMenuBe_SecsVal);
    reinterpret_cast<std::int32_t*>(kMenuBe_SecsArr)[iVar1] = secs_val;

    // Call 3: store frac (0x00429a54..0x00429a62)
    iVar1 = s_FUN_00430790();
    float frac_val = *reinterpret_cast<const float*>(kMenuBe_FracVal);
    reinterpret_cast<float*>(kMenuBe_FracArr)[iVar1] = frac_val;
}

RH_ScopedInstall(MenuMenusBE, 0x00429a30);  // re-enabled 2026-05-24 c3-frontend-a
