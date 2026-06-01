// Mashed RE - Frontend menu sprite-dispatch triplets + chrome shell pair.
// c3-batch-m session 2.
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Included in this file:
//   0x0042f0c0  MenuSpriteDispatchA  — options 3-row list renderer (Difficulty/Steering/Camera)
//   0x0042fb70  MenuSpriteDispatchB  — mini 3-row settings (Track/Player Count/Music)
//   0x0042fe90  MenuSpriteDispatchC  — per-vehicle Y/N feature list renderer (14 entries)
//   0x0042e3a0  MenuChromeShellA     — menu chrome: top/bottom bands, scroll, tick marks
//   0x0042e5b0  MenuChromeShellB     — frontend BG + animated logo renderer
//
// Analysis notes:
//   re/analysis/promote_c2_frontend_menus/0x0042f0c0.md
//   re/analysis/promote_c2_frontend_menus/0x0042fb70.md
//   re/analysis/promote_c2_frontend_menus/0x0042fe90.md
//   re/analysis/promote_c2_frontend_menus/0x0042e3a0.md
//   re/analysis/promote_c2_frontend_menus/0x0042e5b0.md

#include "../Core/HookSystem.h"
#include <cstdint>

// ---------------------------------------------------------------------------
// Callee declarations — all C2+ in hooks.csv
// ---------------------------------------------------------------------------

// 0x0042b8b0  ScreenWidthGet  (C3)
static auto* const s_ScreenWidthGet =
    reinterpret_cast<std::uint32_t(__cdecl*)()>(0x0042b8b0);

// 0x0042b8c0  ScreenHeightGet  (C3)
static auto* const s_ScreenHeightGet =
    reinterpret_cast<std::uint32_t(__cdecl*)()>(0x0042b8c0);

// 0x0040bb50  SpriteLookupC  (C3) — "Arrow" object lookup / constructor
static auto* const s_SpriteLookupC =
    reinterpret_cast<void*(__cdecl*)(void*)>(0x0040bb50);

// 0x004739f0  TextSpriteScaled  (C2) — draw Arrow left (12-param textured quad with scale)
static auto* const s_TextSpriteScaled =
    reinterpret_cast<void(__cdecl*)(void*, float, float, float, float,
                                    std::uint32_t, float, float, float, float,
                                    int, int)>(0x004739f0);

// 0x00473870  TextSpriteUVExplicit  (C2) — draw Arrow right (7-param textured quad)
static auto* const s_TextSpriteUVExplicit =
    reinterpret_cast<void(__cdecl*)(void*, float, float, float, float,
                                    std::uint32_t, int)>(0x00473870);

// 0x00427e00  SpriteDraw  (C2) — sprite draw (6 params: ID/X/Y/ARGB/scale/flags)
static auto* const s_SpriteDraw =
    reinterpret_cast<void(__cdecl*)(int, float, float, std::uint32_t,
                                    float, int)>(0x00427e00);

// 0x004282a0  TextMeasureA  (C2) — text width measure variant A
static auto* const s_TextMeasureA =
    reinterpret_cast<float(__cdecl*)(int)>(0x004282a0);

// 0x004a2c48  FrameCounter  (C2) — FPU banker's rounding frame counter
// Called as: push/call/fstp pattern; returns value in ST0.
// We call through original VA directly.
static auto* const s_FrameCounter =
    reinterpret_cast<std::uint32_t(__cdecl*)()>(0x004a2c48);

// 0x0042ac00  MenuGroupCount  (C3) — __fastcall row Y position init
// ECX = unused int; EDX = int*
static auto* const s_MenuGroupCount =
    reinterpret_cast<int(__fastcall*)(int, int*)>(0x0042ac00);

// 0x0042ac50  MenuBaseY  (C2) — __fastcall returns base Y float
static auto* const s_MenuBaseY =
    reinterpret_cast<float(__fastcall*)(int, int)>(0x0042ac50);

// 0x0042d5a0  CreditsSpriteLine  (C2) — credits sprite-timeline renderer
static auto* const s_CreditsSpriteLine =
    reinterpret_cast<void(__cdecl*)(int)>(0x0042d5a0);

// 0x00472f40  TextGradientV0V1Override  (C2) — Im2D top gradient quad (x/y/w/h/argb)
static auto* const s_TextGradientV0V1 =
    reinterpret_cast<void(__cdecl*)(float, float, float, float,
                                    std::uint32_t)>(0x00472f40);

// 0x004730b0  TextGradientV2V3Override  (C2) — Im2D bottom gradient quad (x/y/w/h/argb)
static auto* const s_TextGradientV2V3 =
    reinterpret_cast<void(__cdecl*)(float, float, float, float,
                                    std::uint32_t)>(0x004730b0);

// 0x00472c60  ChromeBaseDraw  (C2) — Im2D filled-quad draw (x/y/w/h/ARGB)
static auto* const s_ChromeBaseDraw =
    reinterpret_cast<void(__cdecl*)(float, float, float, float,
                                    std::uint32_t)>(0x00472c60);

// 0x00473c20  DrawFullscreenBG  (C2) — draw fullscreen BG texture
static auto* const s_DrawFullscreenBG =
    reinterpret_cast<void(__cdecl*)(void*)>(0x00473c20);

// 0x0042e590  SpriteAnimFrameThunk  (C4) — sprite-slot resolver
static auto* const s_SpriteAnimFrameThunk =
    reinterpret_cast<void(__cdecl*)(int, float, float, float, float,
                                    std::uint32_t, int, int, int)>(0x0042e590);

// 0x00474890  SpriteDrawCommit  (C2) — sprite draw commit
static auto* const s_SpriteDrawCommit =
    reinterpret_cast<void(__cdecl*)(void*, float, float, float, float,
                                    std::uint32_t, int, int, int)>(0x00474890);

// 0x00473ee0  LogoOverlayDraw  (C2) — draw logo overlay with slide offset
static auto* const s_LogoOverlayDraw =
    reinterpret_cast<void(__cdecl*)(void*, int)>(0x00473ee0);

// RW vtable accessor: DAT_007d3ff8
static constexpr std::uintptr_t kVtableGlobal = 0x007d3ff8u;

// Render state setter type at vtable+0x20
typedef void(__cdecl* RwSetState_t)(int, int);

static inline void vtable_render_state(int a, int b) {
    std::uintptr_t vptr = *reinterpret_cast<std::uintptr_t*>(kVtableGlobal);
    (*reinterpret_cast<RwSetState_t*>(vptr + 0x20))(a, b);
}

// ---------------------------------------------------------------------------
// MenuSpriteDispatchA  --  0x0042f0c0
//
// Original: FUN_0042f0c0 (800 bytes, 0x0042f0c0..0x0042f3e0)
// Signature: void FUN_0042f0c0(void)
//
// Options 3-row list renderer: Difficulty / Steering / Camera.
// Reads globals for selected row, alpha, display mode.
// Guard: early return if DAT_0067e7b0==0 && DAT_0067e7b4 < 0x60.
//
// ref: re/analysis/promote_c2_frontend_menus/0x0042f0c0.md
// ---------------------------------------------------------------------------

// Globals (cited at 0x0042f0c0 body):
static constexpr std::uintptr_t kDispA_ModeFlag   = 0x0067e7b0u; // display mode flag
static constexpr std::uintptr_t kDispA_Alpha       = 0x0067e7b4u; // alpha value
static constexpr std::uintptr_t kDispA_ScreenIdx   = 0x0067e9f8u; // current screen index
static constexpr std::uintptr_t kDispA_Difficulty  = 0x0067ea8cu; // Difficulty setting (0-3)
static constexpr std::uintptr_t kDispA_Steering    = 0x005f76c4u; // Steering setting (0-1)
static constexpr std::uintptr_t kDispA_Camera      = 0x005f76c8u; // Camera setting (0-1)
static constexpr std::uintptr_t kDispA_SelTableA   = 0x0067ed40u; // selected row table (DAT_0067ed40, stride 0x40)
static constexpr std::uintptr_t kDispA_SelTableB   = 0x0067ed80u; // alt selected row (stride 0x10)
static constexpr std::uintptr_t kDispA_SelFlag     = 0x0067e844u; // alt-table subtract-2 flag
// Scale constants (cited at body):
static constexpr std::uintptr_t kDispA_ScaleX      = 0x005cd834u;
static constexpr std::uintptr_t kDispA_ScaleY      = 0x005cd830u;
static constexpr std::uintptr_t kDispA_ScaleW      = 0x005cc9a4u;
static constexpr std::uintptr_t kDispA_ScaleH      = 0x005cc324u;
static constexpr std::uintptr_t kDispA_ScaleSprite = 0x005cc72cu;

// Sprite IDs per row/value (cited at 0x0042f0c0 body):
static const int kDispA_DiffSprites[4] = { 0x3a, 0x154, 0x5f, 0x60 };
static const int kDispA_SteerSprites[2] = { 0x61, 0x69 };
static const int kDispA_CamSprites[2]   = { 0x62, 0x6a };

// 0x0042f0c0
extern "C" __declspec(dllexport) void __cdecl MenuSpriteDispatchA(void)
{
    // Early return guard: if mode==0 && alpha < 0x60 (96), skip rendering.
    // Cited at 0x0042f0c0 entry.
    std::uint32_t mode  = *reinterpret_cast<std::uint32_t*>(kDispA_ModeFlag);
    std::uint32_t alpha = *reinterpret_cast<std::uint32_t*>(kDispA_Alpha);
    if (mode == 0 && alpha < 0x60u) {
        return;
    }

    // Begin render: vtable+0x20 with (6,0)/(8,0) — cited at body entry.
    vtable_render_state(6, 0);
    vtable_render_state(8, 0);

    // Determine selected row index.
    int screen_idx = *reinterpret_cast<int*>(kDispA_ScreenIdx);
    int selected_row;
    if (mode == 0) {
        // Path A: selected row from table at DAT_0067ed40 + screen_idx*0x40 + 4
        // Cited at 0x0042f0c0 body.
        selected_row = *reinterpret_cast<int*>(kDispA_SelTableA + screen_idx * 0x40 + 4);
    } else {
        // Path B: from alternate table DAT_0067ed80 + screen_idx*0x10
        selected_row = *reinterpret_cast<int*>(
            reinterpret_cast<std::uintptr_t>(&*reinterpret_cast<int*>(kDispA_SelTableB))
            + screen_idx * 0x10);
        // If DAT_0067e844==0, subtract 2 from selected row. Cited at body.
        if (*reinterpret_cast<int*>(kDispA_SelFlag) == 0) {
            selected_row -= 2;
        }
    }

    // Scale helpers (cited at body; float values at fixed addresses).
    float scale_x = *reinterpret_cast<float*>(kDispA_ScaleX);
    float scale_y = *reinterpret_cast<float*>(kDispA_ScaleY);
    float scale_w = *reinterpret_cast<float*>(kDispA_ScaleW);
    float scale_h = *reinterpret_cast<float*>(kDispA_ScaleH);

    // Row 0: Difficulty (DAT_0067ea8c, values 0..3 → sprite IDs 0x3a/0x154/0x5f/0x60)
    {
        int val = *reinterpret_cast<int*>(kDispA_Difficulty);
        if (val >= 0 && val <= 3) {
            int sprite_id = kDispA_DiffSprites[val];
            // Draw sprite at scaled position (281.0f, 211.0f + row*spacing), scale 0.7f.
            // Text X: 0x438c0000 = 281.0f; Y start: 211.0f; scale: 0x3f333333 = 0.7f.
            // Exact coordinates cited at 0x0042f0c0 body.
            float x = 281.0f * scale_x;
            float y = 211.0f * scale_y;
            s_SpriteDraw(sprite_id, x, y, (alpha << 24u) | 0x00ffffffu, 0.7f, 0);
        }
    }

    // Row 1: Steering (DAT_005f76c4, values 0..1 → sprite IDs 0x61/0x69)
    {
        int val = *reinterpret_cast<int*>(kDispA_Steering);
        if (val >= 0 && val <= 1) {
            int sprite_id = kDispA_SteerSprites[val];
            float x = 281.0f * scale_x;
            float y = (211.0f + scale_h) * scale_y;
            s_SpriteDraw(sprite_id, x, y, (alpha << 24u) | 0x00ffffffu, 0.7f, 0);
        }
    }

    // Row 2: Camera (DAT_005f76c8, values 0..1 → sprite IDs 0x62/0x6a)
    {
        int val = *reinterpret_cast<int*>(kDispA_Camera);
        if (val >= 0 && val <= 1) {
            int sprite_id = kDispA_CamSprites[val];
            float x = 281.0f * scale_x;
            float y = (211.0f + scale_h * 2.0f) * scale_y;
            s_SpriteDraw(sprite_id, x, y, (alpha << 24u) | 0x00ffffffu, 0.7f, 0);
        }
    }

    // End render: vtable+0x20 with (6,1)/(8,1). Cited at body exit.
    vtable_render_state(6, 1);
    vtable_render_state(8, 1);
}

RH_ScopedInstall(MenuSpriteDispatchA, 0x0042f0c0);  // re-enabled 2026-05-24 c3-frontend-b


// ---------------------------------------------------------------------------
// MenuSpriteDispatchB  --  0x0042fb70
//
// Original: FUN_0042fb70 (700 bytes, 0x0042fb70..0x0042fe2c)
// Signature: void FUN_0042fb70(void)
//
// Mini 3-row settings renderer: Track / Player Count / Music.
// Guard: only renders if DAT_0067e7e8 != 0 || _DAT_0067e7ec > 0x5f.
//
// ref: re/analysis/promote_c2_frontend_menus/0x0042fb70.md
// ---------------------------------------------------------------------------

// Globals (cited at 0x0042fb70 body):
static constexpr std::uintptr_t kDispB_ModeFlag    = 0x0067e7e8u; // mode flag (guard)
static constexpr std::uintptr_t kDispB_Alpha        = 0x0067e7ecu; // alpha (guard threshold 0x60)
static constexpr std::uintptr_t kDispB_ScreenIdx    = 0x0067e9f8u; // current screen index
static constexpr std::uintptr_t kDispB_Track        = 0x0067ea84u; // Track setting (0-3)
static constexpr std::uintptr_t kDispB_PlayerCount  = 0x0067ea78u; // Player count (0=1p, 1=multi)
static constexpr std::uintptr_t kDispB_Music        = 0x0067ea80u; // Music setting (0-2)
// Scale constants (cited at body):
static constexpr std::uintptr_t kDispB_ScaleXBase   = 0x005cd8a0u;
static constexpr std::uintptr_t kDispB_ScaleYBase   = 0x005cc9f4u;
static constexpr std::uintptr_t kDispB_ScaleW2      = 0x005cc560u;
static constexpr std::uintptr_t kDispB_ScaleH2      = 0x005cc9a4u;
static constexpr std::uintptr_t kDispB_ScaleH3      = 0x005cc324u;
static constexpr std::uintptr_t kDispB_ScaleSprite  = 0x005cc72cu;

// Sprite IDs per row/value (cited at 0x0042fb70 body):
static const int kDispB_TrackSprites[4]  = { 0xe3, 0xd2, 0xd3, 0xd4 };
static const int kDispB_PCSprites[2]     = { 0x59, 0x5a };
static const int kDispB_MusicSprites[3]  = { 0xe0, 0xe1, 0xe2 };

// 0x0042fb70
extern "C" __declspec(dllexport) void __cdecl MenuSpriteDispatchB(void)
{
    // Guard: only render if mode != 0 OR alpha > 0x5f. Cited at 0x0042fb70 entry.
    std::uint32_t mode  = *reinterpret_cast<std::uint32_t*>(kDispB_ModeFlag);
    std::uint32_t alpha = *reinterpret_cast<std::uint32_t*>(kDispB_Alpha);
    if (mode == 0 && alpha <= 0x5fu) {
        return;
    }

    // Begin render state. Cited at body entry.
    vtable_render_state(6, 0);
    vtable_render_state(8, 0);

    // Row Y init via FUN_0042ac00 (MenuGroupCount) and base via FUN_0042ac50.
    // Cited at 0x0042fb70 body.
    int dummy = 0;
    s_MenuGroupCount(0, &dummy);
    float base_y = s_MenuBaseY(0, 0);

    // Scale helpers.
    float scale_x = *reinterpret_cast<float*>(kDispB_ScaleXBase);
    float scale_h  = *reinterpret_cast<float*>(kDispB_ScaleH2);

    // Text X: 311.0f (0x439b0000). Cited at 0x0042fb70 body.
    static constexpr float kTextX = 311.0f;

    // Row 0: Track (DAT_0067ea84, 0..3 → 0xe3/0xd2/0xd3/0xd4)
    {
        int val = *reinterpret_cast<int*>(kDispB_Track);
        if (val >= 0 && val <= 3) {
            s_SpriteDraw(kDispB_TrackSprites[val],
                         kTextX * scale_x, base_y,
                         (alpha << 24u) | 0x00ffffffu, 0.7f, 0);
        }
    }

    // Row 1: Player Count (DAT_0067ea78, 0..1 → 0x59/0x5a)
    {
        int val = *reinterpret_cast<int*>(kDispB_PlayerCount);
        if (val >= 0 && val <= 1) {
            s_SpriteDraw(kDispB_PCSprites[val],
                         kTextX * scale_x, base_y + scale_h,
                         (alpha << 24u) | 0x00ffffffu, 0.7f, 0);
        }
    }

    // Row 2: Music (DAT_0067ea80, 0..2 → 0xe0/0xe1/0xe2)
    {
        int val = *reinterpret_cast<int*>(kDispB_Music);
        if (val >= 0 && val <= 2) {
            s_SpriteDraw(kDispB_MusicSprites[val],
                         kTextX * scale_x, base_y + scale_h * 2.0f,
                         (alpha << 24u) | 0x00ffffffu, 0.7f, 0);
        }
    }

    // End render state. Cited at body exit.
    vtable_render_state(6, 1);
    vtable_render_state(8, 1);
}

RH_ScopedInstall(MenuSpriteDispatchB, 0x0042fb70);  // re-enabled 2026-05-24 c3-frontend-b


// ---------------------------------------------------------------------------
// MenuSpriteDispatchC  --  0x0042fe90
//
// Original: FUN_0042fe90 (650 bytes, 0x0042fe90..0x00430116)
// Signature: void FUN_0042fe90(void)
//
// Per-vehicle-type Y/N feature list renderer (14 entries).
// Guard: only renders if DAT_0067e810 != 2 &&
//        (DAT_0067e810 != 0 || _DAT_0067e814 > 0x5f).
//
// ref: re/analysis/promote_c2_frontend_menus/0x0042fe90.md
// ---------------------------------------------------------------------------

// Globals (cited at 0x0042fe90 body):
static constexpr std::uintptr_t kDispC_ModeFlag    = 0x0067e810u; // mode flag (guard)
static constexpr std::uintptr_t kDispC_Alpha        = 0x0067e814u; // alpha (guard threshold 0x60)
static constexpr std::uintptr_t kDispC_ScreenIdx    = 0x0067e9f8u; // screen index
static constexpr std::uintptr_t kDispC_DimTable     = 0x0067ed44u; // dim flag table (screen×0x10+row)
static constexpr std::uintptr_t kDispC_FeatureBase  = 0x00898b1cu; // feature entry array base
static constexpr std::uintptr_t kDispC_TypeIndexArr = 0x00898aa0u; // feature type index array
static constexpr std::uintptr_t kDispC_VehFeatTable = 0x007f105cu; // vehicle feature presence table (stride 0x13)
// Layout constants (cited at 0x0042fe90 body):
static constexpr int kDispC_EntryCount   = 14;    // 14 vehicle feature entries
static constexpr int kDispC_EntryStride  = 0xd;   // entry stride in dwords = 52 bytes → 13 dwords
static constexpr int kDispC_SkipSentinel = 0xc3;  // entry disabled/hidden sentinel
static constexpr int kDispC_VehStride    = 0x13;  // vehicle feature table stride
// Scale constants (cited at body):
static constexpr std::uintptr_t kDispC_ScaleX       = 0x005cd8a0u;
static constexpr std::uintptr_t kDispC_ScaleY       = 0x005cc9f4u;
static constexpr float           kDispC_TextX        = 351.0f;     // 0x43af0000

// 0x0042fe90
extern "C" __declspec(dllexport) void __cdecl MenuSpriteDispatchC(void)
{
    // Guard: skip if mode==2, or mode==0 && alpha <= 0x5f. Cited at 0x0042fe90 entry.
    std::uint32_t mode  = *reinterpret_cast<std::uint32_t*>(kDispC_ModeFlag);
    std::uint32_t alpha = *reinterpret_cast<std::uint32_t*>(kDispC_Alpha);
    if (mode == 2u) return;
    if (mode == 0u && alpha <= 0x5fu) return;

    // Begin render state. Cited at body entry.
    vtable_render_state(6, 0);
    vtable_render_state(8, 0);

    // Row Y init. Cited at 0x0042fe90 body.
    int dummy = 0;
    s_MenuGroupCount(0, &dummy);
    float base_y = s_MenuBaseY(0, 0);

    float scale_x = *reinterpret_cast<float*>(kDispC_ScaleX);
    float scale_y = *reinterpret_cast<float*>(kDispC_ScaleY);
    float scale_h = *reinterpret_cast<float*>(0x005cc9a4u);  // per-row step

    int screen_idx = *reinterpret_cast<int*>(kDispC_ScreenIdx);

    // Iterate 14 feature entries. Stride is 0xd dwords = 52 bytes. Cited at body.
    for (int i = 0; i < kDispC_EntryCount; ++i) {
        // Entry pointer: kDispC_FeatureBase + i * (kDispC_EntryStride * 4)
        const std::uintptr_t entry_base = kDispC_FeatureBase
            + static_cast<std::uintptr_t>(i) * static_cast<std::uintptr_t>(kDispC_EntryStride) * 4u;
        int entry_head = *reinterpret_cast<const int*>(entry_base);

        // Skip if entry[0] == 0xc3 (disabled). Cited at body.
        if (static_cast<unsigned char>(entry_head) == static_cast<unsigned char>(kDispC_SkipSentinel))
            continue;

        // Alpha: from DAT_0067e814; dim to bVar8>>2 if dim-table entry==0.
        // Cited at 0x0042fe90 body.
        std::uint32_t row_alpha = alpha;
        int dim_flag = *reinterpret_cast<int*>(
            kDispC_DimTable + static_cast<std::uintptr_t>(screen_idx) * 0x10u
            + static_cast<std::uintptr_t>(i) * 4u);
        if (dim_flag == 0) {
            row_alpha = row_alpha >> 2u;
        }

        // Feature present: lookup vehicle feature table.
        // cVar9 = (DAT_007f105c[DAT_00898aa0[i] * 0x13] != 0) + 'Y'.
        // 'Y'=89 present, 'Z'=90 absent. Cited at 0x0042fe90 body.
        int type_idx = *reinterpret_cast<const int*>(
            kDispC_TypeIndexArr + static_cast<std::uintptr_t>(i) * 4u);
        int feat_present = *reinterpret_cast<const std::int8_t*>(
            kDispC_VehFeatTable
            + static_cast<std::uintptr_t>(type_idx) * static_cast<std::uintptr_t>(kDispC_VehStride));
        int sprite_id = (feat_present != 0 ? 89 : 90);  // 'Y' or 'Z'

        // Draw sprite. Text X: 351.0f (0x43af0000). Cited at body.
        float x = kDispC_TextX * scale_x;
        float y = base_y + scale_h * static_cast<float>(i);
        s_SpriteDraw(sprite_id, x, y, (row_alpha << 24u) | 0x00ffffffu, 0.7f, 0);
    }

    // End render state. Cited at body exit.
    vtable_render_state(6, 1);
    vtable_render_state(8, 1);
}

RH_ScopedInstall(MenuSpriteDispatchC, 0x0042fe90);  // re-enabled 2026-05-24 c3-frontend-b


// ---------------------------------------------------------------------------
// MenuChromeShellA  --  0x0042e3a0
//
// Original: FUN_0042e3a0 (230 bytes, 0x0042e3a0..0x0042e586)
// Signature: void FUN_0042e3a0(void)
//
// Menu chrome drawer: top/bottom UI bands + scroll animation + tick marks.
// - If current screen == credits sentinel (DAT_005f6980): advance scroll,
//   call CreditsSpriteLine(scroll - 15000).
// - Else: reset scroll to 0.
// - Draw top amber band (0,0)–(640,64) via TextGradientV0V1.
// - Draw bottom dark band (0,416)–(640,64) via TextGradientV2V3.
// - Draw two white divider lines via ChromeBaseDraw.
// - If DAT_0067f17c > 9: draw 4 amber tick marks via ChromeBaseDraw.
//
// ref: re/analysis/promote_c2_frontend_menus/0x0042e3a0.md
// ---------------------------------------------------------------------------

// Globals (cited at 0x0042e3a0 body):
static constexpr std::uintptr_t kChrA_ScreenTable   = 0x0067ed38u; // screen-type table (stride 0x40; 0x0042e3a9 SHL EAX,0x6)
static constexpr std::uintptr_t kChrA_ScreenIdx     = 0x0067e9f8u; // current screen index
static constexpr std::uintptr_t kChrA_CreditsSent   = 0x005f6980u; // credits screen descriptor ADDRESS (compared, not deref'd)
static constexpr std::uintptr_t kChrA_ScrollCtr     = 0x0067ebc0u; // credits scroll counter
static constexpr std::uintptr_t kChrA_DeltaTime     = 0x007f1000u; // delta-time (scroll speed)
static constexpr std::uintptr_t kChrA_AnimFrame     = 0x0067f17cu; // animation frame counter

// 0x0042e3a0
extern "C" __declspec(dllexport) void __cdecl MenuChromeShellA(void)
{
    int screen_idx = *reinterpret_cast<int*>(kChrA_ScreenIdx);

    // Credits-screen check. Disassembly at 0x0042e3a0 (verified in Ghidra slot,
    // MASHED.exe @0x00400000):
    //   0042e3a0  MOV EAX,[0x0067e9f8]            ; screen_idx
    //   0042e3a9  SHL EAX,0x6                     ; screen_idx * 0x40   (NOT 0x10)
    //   0042e3ac  MOV ECX,[EAX + 0x0067ed38]      ; screen_type = table[screen_idx]
    //   0042e3b6  CMP ECX,0x5f6980                ; compare to the ADDRESS &DAT_005f6980
    //   0042e3bd  JNZ ...                          ; (immediate-address compare)
    // The earlier reimpl used stride 0x10 and *dereferenced* the sentinel
    // (*(void**)0x005f6980) — two divergences. Both are invisible at the quiescent
    // main menu (screen_idx==0, non-credits screen → else branch either way), so the
    // synthetic diff and the standalone's B16 no-op thunk masked them; but they are
    // real for the credits screen / screen_idx!=0 and affect the .asi too.
    void* screen_type = *reinterpret_cast<void**>(
        kChrA_ScreenTable + static_cast<std::uintptr_t>(screen_idx) * 0x40u);
    void* credits_sentinel = reinterpret_cast<void*>(kChrA_CreditsSent);

    if (screen_type == credits_sentinel) {
        // Advance scroll counter by delta-time. Cited at body.
        int delta = *reinterpret_cast<int*>(kChrA_DeltaTime);
        *reinterpret_cast<int*>(kChrA_ScrollCtr) += delta;
        int scroll = *reinterpret_cast<int*>(kChrA_ScrollCtr);
        // Draw credits at scroll offset - 15000. Cited at body.
        s_CreditsSpriteLine(scroll - 15000);
    } else {
        // Reset scroll counter. Cited at body.
        *reinterpret_cast<int*>(kChrA_ScrollCtr) = 0;
    }

    // Top amber band: (0,0)–(640,64). Cited at body.
    // FUN_00472f40(0, 0, 0x44200000, 0x42800000, 0xa0000000)
    // = TextGradientV0V1Override(0.0f, 0.0f, 640.0f, 64.0f, 0xa0000000)
    s_TextGradientV0V1(0.0f, 0.0f, 640.0f, 64.0f, 0xa0000000u);

    // Bottom dark band: (0,416)–(640,64). Cited at body.
    // FUN_004730b0(0, 0x43d00000, 0x44200000, 0x42800000, 0xa0000000)
    // = TextGradientV2V3Override(0.0f, 416.0f, 640.0f, 64.0f, 0xa0000000)
    s_TextGradientV2V3(0.0f, 416.0f, 640.0f, 64.0f, 0xa0000000u);

    // Top white divider line: (0, 416.0f, 640.0f, 1.0f, 0xffffffff). Cited at body.
    s_ChromeBaseDraw(0.0f, 416.0f, 640.0f, 1.0f, 0xffffffffu);
    // Bottom white divider line: (0, 64.0f, 640.0f, 1.0f, 0xffffffff). Cited at body.
    s_ChromeBaseDraw(0.0f, 64.0f, 640.0f, 1.0f, 0xffffffffu);

    // Amber tick marks: only drawn when DAT_0067f17c > 9. Cited at body.
    if (*reinterpret_cast<int*>(kChrA_AnimFrame) > 9) {
        s_ChromeBaseDraw(0.0f, 64.0f,    640.0f, 3.0f, 0x80f7941du);
        s_ChromeBaseDraw(0.0f, 65.5f,    640.0f, 3.0f, 0x40f7941du);
        s_ChromeBaseDraw(0.0f, 413.0f,   640.0f, 3.0f, 0x80f7941du);
        s_ChromeBaseDraw(0.0f, 410.0f,   640.0f, 3.0f, 0x40f7941du);
    }
}

RH_ScopedInstall(MenuChromeShellA, 0x0042e3a0);  // re-enabled 2026-05-24 c3-frontend-a


// ---------------------------------------------------------------------------
// MenuChromeShellB  --  0x0042e5b0
//
// Original: FUN_0042e5b0 (755 bytes, 0x0042e5b0..0x0042e8a2)
// Signature: void FUN_0042e5b0(void)
//
// Frontend background + animated logo renderer.
// - Draws fullscreen BG texture (DrawFullscreenBG, DAT_00898ab8).
// - 512-frame animation cycle via FrameCounter() & 0x1ff.
// - Two sprite cross-fades (sprite A / sprite B):
//     Phase 0x000..0x0df: both faded out (iVar6=0, iVar7=0)
//     Phase 0x0e0..0x0ef: iVar7 = (uVar2-0xe0)*0x10 (fade in B)
//     Phase 0x0f0..0x0ff: iVar7 = 0xff; iVar6 = (0x1f-delta)*0x10
//     Phase 0x100..0x1df: iVar7=0xff, iVar6=0 (B fully visible)
//     Phase 0x1e0..0x1ef: iVar6 = (uVar2-0x1e0)*0x10 (fade in A)
//     Phase 0x1f0..0x1ff: iVar7 = (0x1f-delta)*0x10 (fade out B)
//   At phase 0x1e0: advance sprite A counter (DAT_0067e848 += 2).
//   At phase 0x0e0: advance sprite B counter (DAT_0067f0b8 += 2).
//   DAT_0067f0b8 always forced odd.
// - Sprite cycle table local_40[16]: index = (counter & 0xf) → slot 0..5 repeating.
// - Slide offset DAT_008990e0: decremented each frame, clamped [0, 0x200].
//   Initial slide from DAT_005cd65c.
// - Draw sprites via SpriteAnimFrameThunk, commit via SpriteDrawCommit.
// - Draw logo overlay via LogoOverlayDraw(DAT_008990e0 slide offset).
//
// ref: re/analysis/promote_c2_frontend_menus/0x0042e5b0.md
// ---------------------------------------------------------------------------

// Globals (cited at 0x0042e5b0 body):
static constexpr std::uintptr_t kChrB_BGHandle      = 0x00898ab8u; // fullscreen BG sprite handle
static constexpr std::uintptr_t kChrB_BGHandleB     = 0x008991a0u; // BG texture handle B
static constexpr std::uintptr_t kChrB_SpriteACount  = 0x0067e848u; // sprite A cycle counter
static constexpr std::uintptr_t kChrB_SpriteBCount  = 0x0067f0b8u; // sprite B cycle counter (forced odd)
static constexpr std::uintptr_t kChrB_SlideOffset   = 0x008990e0u; // slide offset
static constexpr std::uintptr_t kChrB_SlideBase     = 0x005cd65cu; // slide base constant

// Sprite cycle table: index 0..15 → slot 0..5 repeating. Cited at body.
static const int kChrB_SlotCycle[16] = { 0,1,2,3,4,5, 0,1,2,3,4,5, 0,1,2,3 };

// 0x0042e5b0
extern "C" __declspec(dllexport) void __cdecl MenuChromeShellB(void)
{
    // Draw fullscreen BG texture. Cited at 0x0042e5b0 body entry.
    void* bg_handle = *reinterpret_cast<void**>(kChrB_BGHandle);
    s_DrawFullscreenBG(bg_handle);

    // 512-frame phase. Cited at body.
    std::uint32_t phase = s_FrameCounter() & 0x1ffu;

    // Advance sprite A counter at phase 0x1e0. Cited at body.
    if (phase == 0x1e0u) {
        *reinterpret_cast<int*>(kChrB_SpriteACount) += 2;
    }
    // Advance sprite B counter at phase 0x0e0. Cited at body.
    if (phase == 0x0e0u) {
        *reinterpret_cast<int*>(kChrB_SpriteBCount) += 2;
    }
    // Force sprite B counter odd. Cited at body.
    *reinterpret_cast<int*>(kChrB_SpriteBCount) |= 1;

    // Compute alpha values for sprite A (iVar6) and sprite B (iVar7).
    // Phase ranges cited at body.
    int iVar6 = 0, iVar7 = 0;
    if (phase < 0x0e0u) {
        // Phase 0x000..0x0df: both faded out.
        iVar6 = 0; iVar7 = 0;
    } else if (phase < 0x0f0u) {
        // Phase 0x0e0..0x0ef: fade in B.
        iVar7 = static_cast<int>((phase - 0x0e0u) * 0x10u);
        iVar6 = 0;
    } else if (phase < 0x100u) {
        // Phase 0x0f0..0x0ff: B=0xff; partial A fade.
        iVar7 = 0xff;
        int delta = static_cast<int>(phase - 0x0f0u);
        iVar6 = (0x1f - delta) * 0x10;
    } else if (phase < 0x1e0u) {
        // Phase 0x100..0x1df: B fully visible, A=0.
        iVar7 = 0xff; iVar6 = 0;
    } else if (phase < 0x1f0u) {
        // Phase 0x1e0..0x1ef: fade in A.
        iVar6 = static_cast<int>((phase - 0x1e0u) * 0x10u);
        iVar7 = 0xff;
    } else {
        // Phase 0x1f0..0x1ff: fade out B.
        int delta = static_cast<int>(phase - 0x1f0u);
        iVar7 = (0x1f - delta) * 0x10;
        iVar6 = 0xff;
    }

    // Sprite A: cycle lookup from DAT_0067e848. Cited at body.
    int a_count = *reinterpret_cast<int*>(kChrB_SpriteACount);
    int a_slot = kChrB_SlotCycle[a_count & 0xf];
    if (iVar6 > 0) {
        // Draw sprite A via SpriteAnimFrameThunk. Cited at body.
        s_SpriteAnimFrameThunk(a_slot,
            0.0f, 0.0f, 640.0f, 480.0f,
            static_cast<std::uint32_t>(iVar6) << 24u, 0, 0, 0);
    }

    // Sprite B: cycle lookup from DAT_0067f0b8. Cited at body.
    int b_count = *reinterpret_cast<int*>(kChrB_SpriteBCount);
    int b_slot = kChrB_SlotCycle[b_count & 0xf];
    if (iVar7 > 0) {
        s_SpriteAnimFrameThunk(b_slot,
            0.0f, 0.0f, 640.0f, 480.0f,
            static_cast<std::uint32_t>(iVar7) << 24u, 0, 0, 0);
    }

    // Update slide offset: decrement by 1 each frame, clamp to [0, 0x200].
    // Cited at body.
    int slide = *reinterpret_cast<int*>(kChrB_SlideOffset);
    int slide_base = *reinterpret_cast<int*>(kChrB_SlideBase);
    slide -= slide_base;
    if (slide < 0)        slide = 0;
    if (slide > 0x200)    slide = 0x200;
    *reinterpret_cast<int*>(kChrB_SlideOffset) = slide;

    // Draw logo overlay with slide offset. Cited at body.
    void* logo_bg = *reinterpret_cast<void**>(kChrB_BGHandleB);
    s_LogoOverlayDraw(logo_bg, slide);
}

// MASS-DISABLED 2026-05-24 reimpl-divergent-error: RH_ScopedInstall(MenuChromeShellB, 0x0042e5b0);
// Phase A1 audit 2026-05-24: synthetic diff shows orig AVs at 0x8 but reimpl
// throws "system error" (different failure mode). The two implementations
// take different code paths when given bare-int input — suggests a real
// reimpl-vs-original divergence beyond just missing state. Needs reimpl
// audit against the asm before re-enable.
