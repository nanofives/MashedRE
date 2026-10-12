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
//   0x0042fe90  MenuSpriteDispatchC  — 4-entry Arrow/glyph renderer (glyph 0x59/0x5a per player field)
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
#include <cstring>

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

// 0x004282a0 (MenuMenusBA) and 0x004a2c48 (_ftol of ST0) are reached through
// DispC_MeasureTrunc; 0x0042e590 / 0x00474890 through ChrB_ThunkCommit (FUN_00474890
// takes nine arguments, not one); the phase _ftol through ChrB_Phase.

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

// 0x00473c20  DrawFullscreenBG  (C2) — fullscreen BG quad via the Im2D path.
// 10 args (verified vs 0x0042e5b0 call site + 0x00473c20 body, Ghidra slot):
// (x0,y0,x1,y1 floats; a5; bg_handle; a7; logo_handle; tex_sel; alpha).
static auto* const s_DrawFullscreenBG =
    reinterpret_cast<void(__cdecl*)(float, float, float, float,
                                    int, int, int, int, int, int)>(0x00473c20);

// 0x00473ee0  LogoOverlayDraw — animated logo overlay. 10 args (verified vs
// 0x0042e5b0 call site): (a1,a2 ints; slide float; 512.0f; a5; bg_handle; a7;
// logo_handle; tex_sel; alpha). The earlier reimpl typed it (void*,int).
static auto* const s_LogoOverlayDraw =
    reinterpret_cast<void(__cdecl*)(int, int, float, float,
                                    int, int, int, int, int, int)>(0x00473ee0);

// RW vtable accessor: DAT_007d3ff8
static constexpr std::uintptr_t kVtableGlobal = 0x007d3ff8u;

// Render state setter type at vtable+0x20
typedef void(__cdecl* RwSetState_t)(int, int);

static inline void vtable_render_state(int a, int b) {
    std::uintptr_t vptr = *reinterpret_cast<std::uintptr_t*>(kVtableGlobal);
    (*reinterpret_cast<RwSetState_t*>(vptr + 0x20))(a, b);
}

// Shared by MenuSpriteDispatchA/B/C (bits of a float constant; call helpers whose
// register conventions no C declaration expresses).
static inline float DispC_F(std::uint32_t bits) { float f; std::memcpy(&f, &bits, 4); return f; }


// Call helper (not a port): FUN_0042ac50 takes the count in EAX and the spacing in ECX, returns EAX
// (mov ebx,eax @0x0042ac51; imul ebx,ecx @0x0042ac6b). No C convention fits.
static int DispC_MenuBaseY(int count, int spacing)
{
    int r;
    __asm {
        mov  eax, count
        mov  ecx, spacing
        mov  edx, 0x0042ac50
        call edx
        mov  r, eax
    }
    return r;
}

// Call helper (not a port) for the sequence at 0x00430039..0x0043004a: push 0.7f; push id; call 0x004282a0; add esp,8;
// call 0x004a2c48 (ST0 -> EAX). The ST0 result goes to the truncation helper
// unrounded, so the pair is called back to back here too.
static int DispC_MeasureTrunc(int id)
{
    int r;
    __asm {
        push 0x3f333333
        push id
        mov  eax, 0x004282a0
        call eax
        add  esp, 8
        mov  eax, 0x004a2c48
        call eax
        mov  r, eax
    }
    return r;
}

// ---------------------------------------------------------------------------
// MenuSpriteDispatchA  --  0x0042f0c0
//
// Original: FUN_0042f0c0 (0x0042f0c0..0x0042f3e0, jump table 0x0042f3e4), void(void),
// cdecl; saves ebx/ebp/esi/edi. Options 3-row renderer (Difficulty/Steering/Camera).
//
// Re-ported from the bytes 2026-10-12 (capstone on MASHED.exe.unpatched). The old body
// skipped the (6,0)/(8,0) render states the original issues BEFORE its guard
// (0x0042f0d9/0x0042f0e6) and the (6,1)/(8,1) pair on the early out (0x0042f14e/
// 0x0042f15b); read the selected row from 0x0067ed44 (bytes: 0x0067ed40 + screen*0x40,
// 0x0042f0ff) and from ed80 + screen*0x10 (bytes: ed80 + (screen-2)*0x40, 0x0042f110/
// 0x0042f116); drew no Arrow quads; drew every row white (bytes: the selected row in
// colour B = alpha<<24, RGB 0); scaled text x and y (bytes: x 281.0 raw, y 211.0 + 30.0
// per row); dropped steering/camera values other than 0/1 (bytes: (v == 1)*8 + 0x61/0x62)
// and used an unsigned alpha guard (bytes: signed jge @0x0042f142).
//
// ref: re/analysis/promote_c2_frontend_menus/0x0042f0c0.md
// ---------------------------------------------------------------------------

// Globals (each cited at its read):
static constexpr std::uintptr_t kDispA_Mode       = 0x0067e7b0u;  // dword @0x0042f0e9
static constexpr std::uintptr_t kDispA_Alpha      = 0x0067e7b4u;  // dword @0x0042f130
static constexpr std::uintptr_t kDispA_Screen     = 0x0067e9f8u;  // dword @0x0042f0ef
static constexpr std::uintptr_t kDispA_SelMode0   = 0x0067ed40u;  // + screen*0x40 @0x0042f0ff
static constexpr std::uintptr_t kDispA_SelOther   = 0x0067ed80u;  // + (screen[-2])*0x40 @0x0042f116
static constexpr std::uintptr_t kDispA_SelBias    = 0x0067e844u;  // == 0 -> screen - 2 @0x0042f107/0x0042f110
static constexpr std::uintptr_t kDispA_Difficulty = 0x0067ea8cu;  // dword @0x0042f257
static constexpr std::uintptr_t kDispA_Steering   = 0x005f76c4u;  // dword @0x0042f28d
static constexpr std::uintptr_t kDispA_Camera     = 0x005f76c8u;  // dword @0x0042f2a8
static constexpr std::uintptr_t kDispArrowName    = 0x005cd828u;  // "Arrow", push @0x0042f239/0x0042f34d

// Jump table 0x0042f3e4 -> 0x0042f268/26f/276/27d.
static const int kDispA_DiffSprites[4] = { 0x3a, 0x154, 0x5f, 0x60 };

// Float constants, exact bits at their .rdata address / immediate.
static const float kDispA_ArrowXk = DispC_F(0x3ed00000u);  // 0.40625    fmul [0x005cd834] @0x0042f194
static const float kDispA_ArrowYk = DispC_F(0x3ed88889u);  // 0.4229167  fmul [0x005cd830] @0x0042f1ae
static const float kDispA_WScale  = DispC_F(0x3ccccccdu);  // 0.025      fmul [0x005cc9a4] @0x0042f1c8
static const float kDispA_HScale  = DispC_F(0x3d088889u);  // 1/30       fmul [0x005cc324] @0x0042f1fc
static const float kDispA_TextY0  = DispC_F(0x43530000u);  // 211.0      mov [esp+0x20] @0x0042f1f0
static const float kDispA_TextX   = DispC_F(0x438c0000u);  // 281.0      push imm @0x0042f2dd
static const float kDispA_RowStep = DispC_F(0x41f00000u);  // 30.0       fadd [0x005cc72c] @0x0042f3a1
static const float kDispTextSc    = DispC_F(0x3f333333u);  // 0.7        push imm
static const float kDispUv        = DispC_F(0x3f7ff972u);  // 0.9999     push imm
static const float kDispOne       = DispC_F(0x3f800000u);  // 1.0        push imm

// 0x0042f0c0
extern "C" __declspec(dllexport) void __cdecl MenuSpriteDispatchA(void)
{
    vtable_render_state(6, 0);                                                 // @0x0042f0d9
    vtable_render_state(8, 0);                                                 // @0x0042f0e6

    const std::int32_t mode = *reinterpret_cast<const std::int32_t*>(kDispA_Mode);
    std::uint32_t screen = *reinterpret_cast<const std::uint32_t*>(kDispA_Screen);
    std::int32_t sel;
    if (mode == 0) {                                                           // jne @0x0042f0fa
        sel = *reinterpret_cast<const std::int32_t*>(kDispA_SelMode0 + (screen << 6));
    } else {
        if (*reinterpret_cast<const std::int32_t*>(kDispA_SelBias) == 0)
            screen -= 2;                                                       // add edx,-2 @0x0042f110
        sel = *reinterpret_cast<const std::int32_t*>(kDispA_SelOther + (screen << 6));
    }

    const std::int32_t alpha = *reinterpret_cast<const std::int32_t*>(kDispA_Alpha);
    const std::uint32_t a8 = static_cast<std::uint8_t>(alpha);
    const std::uint32_t colorA = (a8 << 24) | 0x00ffffffu;                     // bytes @0x0042f124..0x0042f135
    const std::uint32_t colorB = a8 << 24;                                     // bytes @0x0042f175..0x0042f17f, 0x0042f139

    if (mode == 0 && alpha < 0x60) {                                           // cmp/jge @0x0042f13f (signed)
        vtable_render_state(6, 1);                                             // @0x0042f14e
        vtable_render_state(8, 1);                                             // @0x0042f15b
        return;
    }

    const int w0 = static_cast<std::int16_t>(s_ScreenWidthGet());              // movsx @0x0042f189
    const float ax = static_cast<float>(static_cast<double>(w0) * kDispA_ArrowXk);
    const int h0 = static_cast<std::int16_t>(s_ScreenHeightGet());             // @0x0042f1a3
    float ay = static_cast<float>(static_cast<double>(h0) * kDispA_ArrowYk);
    const int w1 = static_cast<std::int16_t>(s_ScreenWidthGet());              // @0x0042f1bd
    const float cw = static_cast<float>(static_cast<double>(w1) * kDispA_WScale);
    const int h1 = static_cast<std::int16_t>(s_ScreenHeightGet());             // @0x0042f1df
    const float dh = static_cast<float>(static_cast<double>(h1) * kDispA_HScale);

    float y = kDispA_TextY0;
    // esi starts as the 211.0 bits (mov esi,[esp+0x20] @0x0042f1f8) and is only replaced
    // by a matching case, so an out-of-range difficulty draws id 0x43530000.
    int id = static_cast<int>(0x43530000u);
    for (int i = 0; i < 3; ++i) {                                              // inc/cmp 3 @0x0042f389
        if (i == sel) {                                                        // @0x0042f210
            s_TextSpriteScaled(s_SpriteLookupC(reinterpret_cast<void*>(kDispArrowName)),  // @0x0042f23e/0x0042f247
                               ax, ay, cw, dh, colorB, kDispUv, 0.0f, 0.0f, kDispOne, 0, 0);
        }
        if (i == 0) {
            const std::uint32_t v = *reinterpret_cast<const std::uint32_t*>(kDispA_Difficulty);
            if (v <= 3) id = kDispA_DiffSprites[v];                            // ja @0x0042f25f (unsigned)
        } else if (i == 1) {
            id = (*reinterpret_cast<const std::int32_t*>(kDispA_Steering) == 1 ? 8 : 0) + 0x61;  // @0x0042f297
        } else {
            id = (*reinterpret_cast<const std::int32_t*>(kDispA_Camera) == 1 ? 8 : 0) + 0x62;    // @0x0042f2b2
        }
        s_SpriteDraw(id, kDispA_TextX, y, i == sel ? colorB : colorA, kDispTextSc, 0);  // @0x0042f2e3
        if (i == sel) {                                                        // @0x0042f2f3
            const int width = DispC_MeasureTrunc(id);                          // @0x0042f2fd/0x0042f305
            const int w3 = static_cast<std::int16_t>(s_ScreenWidthGet());      // @0x0042f30c
            const int prod = static_cast<int>(static_cast<std::uint32_t>(w3) *
                                              static_cast<std::uint32_t>(width + 0x120));  // @0x0042f314/0x0042f31a
            const int rx = prod / 640;                                         // 0x66666667, sar 8 @0x0042f31d..0x0042f332
            s_TextSpriteUVExplicit(s_SpriteLookupC(reinterpret_cast<void*>(kDispArrowName)),  // @0x0042f352/0x0042f35b
                                   static_cast<float>(rx), ay, cw, dh, colorB, 1);
        }
        const int h2 = static_cast<std::int16_t>(s_ScreenHeightGet());         // @0x0042f363
        const int d = (h2 * 0x1e) / 480;                                       // imul 0x1e, 0x88888889, sar 8 @0x0042f36b..0x0042f383
        ay = static_cast<float>(static_cast<double>(d) + ay);                  // fild/fadd/fstp @0x0042f38d..0x0042f399
        y = y + kDispA_RowStep;                                                // @0x0042f39d..0x0042f3a7
    }

    vtable_render_state(6, 1);                                                 // @0x0042f3bb
    vtable_render_state(8, 1);                                                 // @0x0042f3c7
}

RH_ScopedInstall(MenuSpriteDispatchA, 0x0042f0c0);  // re-enabled 2026-05-24 c3-frontend-b


// ---------------------------------------------------------------------------
// MenuSpriteDispatchB  --  0x0042fb70
//
// Original: FUN_0042fb70 (0x0042fb70..0x0042fe2c), void(void), cdecl; saves
// ebx/ebp/esi/edi. Mini 3-row renderer (Track/Player Count/Music).
//
// Re-ported from the bytes 2026-10-12. The old body called FUN_0042ac00/FUN_0042ac50 with
// (0,&dummy)/(0,0) (bytes: EDX = descriptor 0x0067ed38 (mode 0) / 0x0067ed78 (else) +
// screen*0x40 @0x0042fb91/0x0042fba2, then EAX = count, ECX = 0x1e @0x0042fbb1); issued
// the render states before the guard (bytes: the early out @0x0042fb83 returns with none);
// drew no Arrow quads (bytes: a left AND a right Arrow on EVERY row, 0x0042fcea/0x0042fde9);
// drew every row white (bytes: the selected row, dword 0x0067ed40 / 0x0067ed80 + screen*0x40
// == i, in RGB 0 @0x0042fc1d); scaled x (bytes: 311.0 raw) and stepped y by a scale (bytes:
// y = FUN_0042ac50 result, + 30.0 per row) and used an unsigned alpha guard (bytes: jl).
//
// ref: re/analysis/promote_c2_frontend_menus/0x0042fb70.md
// ---------------------------------------------------------------------------

static constexpr std::uintptr_t kDispB_Mode       = 0x0067e7e8u;  // dword @0x0042fb70/0x0042fbf3
static constexpr std::uintptr_t kDispB_Alpha      = 0x0067e7ecu;  // dword @0x0042fb7c, byte @0x0042fbe0
static constexpr std::uintptr_t kDispB_Screen     = 0x0067e9f8u;  // dword @0x0042fb89/0x0042fb99/0x0042fbfc/0x0042fc0d
static constexpr std::uintptr_t kDispB_DescMode0  = 0x0067ed38u;  // + screen*0x40 @0x0042fb91
static constexpr std::uintptr_t kDispB_DescOther  = 0x0067ed78u;  // + screen*0x40 @0x0042fba2
static constexpr std::uintptr_t kDispB_SelMode0   = 0x0067ed40u;  // + screen*0x40 @0x0042fc05
static constexpr std::uintptr_t kDispB_SelOther   = 0x0067ed80u;  // + screen*0x40 @0x0042fc15
static constexpr std::uintptr_t kDispB_Track      = 0x0067ea84u;  // dword @0x0042fd3b
static constexpr std::uintptr_t kDispB_Players    = 0x0067ea78u;  // dword @0x0042fd27
static constexpr std::uintptr_t kDispB_Music      = 0x0067ea80u;  // dword @0x0042fcff

static const float kDispB_ArrowXk = DispC_F(0x3ee80000u);  // 0.453125   fmul [0x005cd8a0] @0x0042fc4d
static const float kDispB_YBias   = DispC_F(0x41000000u);  // 8.0        fsub [0x005cc9f4] @0x0042fc6b
static const float kDispB_YScale  = DispC_F(0x3b088889u);  // 1/480      fmul [0x005cc560] @0x0042fc73
static const float kDispB_WScale  = DispC_F(0x3ccccccdu);  // 0.025      fmul [0x005cc9a4] @0x0042fc8d
static const float kDispB_HScale  = DispC_F(0x3d088889u);  // 1/30       fmul [0x005cc324] @0x0042fcca
static const float kDispB_TextX   = DispC_F(0x439b0000u);  // 311.0      push imm @0x0042fd7a
static const float kDispB_RowStep = DispC_F(0x41f00000u);  // 30.0       fadd [0x005cc72c] @0x0042fdf2

// 0x0042fb70
extern "C" __declspec(dllexport) void __cdecl MenuSpriteDispatchB(void)
{
    std::uintptr_t desc_table;
    if (*reinterpret_cast<const std::int32_t*>(kDispB_Mode) != 0) {           // jne @0x0042fb7a
        desc_table = kDispB_DescOther;
    } else {
        if (*reinterpret_cast<const std::int32_t*>(kDispB_Alpha) < 0x60)      // cmp/jl @0x0042fb7c (signed)
            return;                                                            // no render states
        desc_table = kDispB_DescMode0;
    }
    const std::uint32_t desc = *reinterpret_cast<const std::uint32_t*>(
        desc_table + (*reinterpret_cast<const std::uint32_t*>(kDispB_Screen) << 6));
    const int count = s_MenuGroupCount(0, reinterpret_cast<int*>(desc));       // @0x0042fbac (EDX = desc)
    float y = static_cast<float>(DispC_MenuBaseY(count, 0x1e));                // @0x0042fbb6, fild/fstp @0x0042fbc5

    vtable_render_state(6, 0);                                                 // @0x0042fbd1
    vtable_render_state(8, 0);                                                 // @0x0042fbdd

    const std::uint32_t a8 = *reinterpret_cast<const std::uint8_t*>(kDispB_Alpha);  // mov cl,[0x67e7ec] @0x0042fbe0
    // esi = [esp+0x18] @0x0042fbe6, a stack slot nothing in this function writes first. A
    // track value outside 0..3 on row 0 draws that indeterminate id on the original; the
    // port draws 0. Rows 1 and 2 never depend on it (row 1 always assigns, row 2 keeps row 1's).
    int id = 0;
    for (int i = 0; i < 3; ++i) {                                              // inc/cmp 3 @0x0042fdfb
        const std::uint32_t scr = *reinterpret_cast<const std::uint32_t*>(kDispB_Screen);
        const std::uintptr_t sel_table =
            *reinterpret_cast<const std::int32_t*>(kDispB_Mode) == 0 ? kDispB_SelMode0 : kDispB_SelOther;  // @0x0042fbf3
        const bool selected = *reinterpret_cast<const std::int32_t*>(sel_table + (scr << 6)) == i;  // @0x0042fc05/0x0042fc15
        const std::uint32_t color = (a8 << 24) | (selected ? 0u : 0x00ffffffu);   // bytes @0x0042fc1d..0x0042fc38

        const int w0 = static_cast<std::int16_t>(s_ScreenWidthGet());          // @0x0042fc42
        const float ax = static_cast<float>(static_cast<double>(w0) * kDispB_ArrowXk);
        const int h0 = static_cast<std::int16_t>(s_ScreenHeightGet());         // @0x0042fc5c
        const float ay = static_cast<float>(
            static_cast<double>(h0) * (static_cast<double>(y) - kDispB_YBias) * kDispB_YScale);  // @0x0042fc63..0x0042fc79
        const int w1 = static_cast<std::int16_t>(s_ScreenWidthGet());          // @0x0042fc82
        const float cw = static_cast<float>(static_cast<double>(w1) * kDispB_WScale);
        const int h1 = static_cast<std::int16_t>(s_ScreenHeightGet());         // @0x0042fca8
        const float dh = static_cast<float>(static_cast<double>(h1) * kDispB_HScale);
        s_TextSpriteScaled(s_SpriteLookupC(reinterpret_cast<void*>(kDispArrowName)),  // @0x0042fce1/0x0042fcea
                           ax, ay, cw, dh, color, kDispUv, 0.0f, 0.0f, kDispOne, 0, 0);

        if (i == 0) {                                                          // je @0x0042fcf7
            const std::int32_t v = *reinterpret_cast<const std::int32_t*>(kDispB_Track);
            if (v == 0) id = 0xe3; else if (v == 1) id = 0xd2;                 // @0x0042fd40..0x0042fd68
            else if (v == 2) id = 0xd3; else if (v == 3) id = 0xd4;
        } else if (i == 1) {
            id = (*reinterpret_cast<const std::int32_t*>(kDispB_Players) != 0 ? 1 : 0) + 0x59;  // setne/add @0x0042fd31
        } else {
            const std::int32_t v = *reinterpret_cast<const std::int32_t*>(kDispB_Music);
            if (v == 0) id = 0xe0; else if (v == 1) id = 0xe1;                 // @0x0042fd04..0x0042fd20
            else if (v == 2) id = 0xe2;
        }
        s_SpriteDraw(id, kDispB_TextX, y, color, kDispTextSc, 0);              // @0x0042fd80

        const int width = DispC_MeasureTrunc(id);                              // @0x0042fd8b/0x0042fd93
        const int w3 = static_cast<std::int16_t>(s_ScreenWidthGet());          // @0x0042fd9a
        const int prod = static_cast<int>(static_cast<std::uint32_t>(w3) *
                                          static_cast<std::uint32_t>(width + 0x13e));  // @0x0042fda2/0x0042fda8
        const int rx = prod / 640;                                             // 0x66666667, sar 8 @0x0042fdab..0x0042fdc0
        s_TextSpriteUVExplicit(s_SpriteLookupC(reinterpret_cast<void*>(kDispArrowName)),  // @0x0042fde0/0x0042fde9
                               static_cast<float>(rx), ay, cw, dh, color, 1);
        y = y + kDispB_RowStep;                                                // @0x0042fdee..0x0042fdff
    }

    vtable_render_state(6, 1);                                                 // @0x0042fe13
    vtable_render_state(8, 1);                                                 // @0x0042fe1f
}

RH_ScopedInstall(MenuSpriteDispatchB, 0x0042fb70);  // re-enabled 2026-05-24 c3-frontend-b


// ---------------------------------------------------------------------------
// MenuSpriteDispatchC  --  0x0042fe90
//
// Original: FUN_0042fe90 (0x0042fe90..0x00430116), void(void), cdecl; saves
// ebx/ebp/esi/edi, clobbers eax/ecx/edx. Only caller: 0x0043bfd3 in
// FUN_0043bf30, which reloads EAX from DAT_0067e818 straight after the call.
//
// Re-ported from the bytes 2026-10-12 (capstone on MASHED.exe.unpatched).
// Ghidra's (&DAT_007f105c)[idx*0x13] and DAT_0067ed44[screen*0x10+i] are
// DWORD-indexed: the bytes are imul eax,eax,0x4c @0x00430010 +
// mov edi,dword [eax+0x7f105c] @0x00430013, and shl edx,4 @0x0042ff1b +
// mov esi,dword [edx*4+0x67ed44] @0x0042ff20 (byte offsets idx*0x4c and
// screen*0x40 + i*4). The old port read an int8 at idx*0x13 and a dword at
// screen*0x10 + i*4, walked 14 entries, inverted the 0x59/0x5a pick, drew no
// Arrow quads and passed (0,&dummy)/(0,0) to FUN_0042ac00/FUN_0042ac50.
//
// ref: re/analysis/promote_c2_frontend_menus/0x0042fe90.md
// ---------------------------------------------------------------------------

// Globals (each cited at its read):
static constexpr std::uintptr_t kDispC_Mode        = 0x0067e810u;  // dword @0x0042fe90
static constexpr std::uintptr_t kDispC_Alpha       = 0x0067e814u;  // dword @0x0042fea9, byte @0x0042fefd/0x004300a1
static constexpr std::uintptr_t kDispC_Screen      = 0x0067e9f8u;  // dword @0x0042feb6/0x0042ff15/0x00430104
static constexpr std::uintptr_t kDispC_DescMode1   = 0x0067ed38u;  // + screen*0x40, dword @0x0043010c (mode == 1)
static constexpr std::uintptr_t kDispC_DescOther   = 0x0067ed78u;  // + screen*0x40, dword @0x0042febf (mode != 1)
static constexpr std::uintptr_t kDispC_DimTable    = 0x0067ed44u;  // + screen*0x40 + i*4, dword @0x0042ff20
static constexpr std::uintptr_t kDispC_EntryFirst  = 0x00898b1cu;  // mov eax,0x898b1c @0x0042ff08
static constexpr std::uintptr_t kDispC_EntryEnd    = 0x00898becu;  // cmp eax,0x898bec @0x004300c0 (4 entries)
static constexpr std::uintptr_t kDispC_EntryStride = 0x34u;        // add eax,0x34 @0x004300b6
static constexpr std::uint32_t  kDispC_SkipHead    = 0xc3u;        // cmp dword [eax],0xc3 @0x0042ff38
static constexpr std::uintptr_t kDispC_TypeIdx     = 0x00898aa0u;  // + i*4, dword @0x00430005
static constexpr std::uintptr_t kDispC_PlayerField = 0x007f105cu;  // + type*0x4c, dword @0x00430013
static constexpr std::uintptr_t kDispC_PlayerStride = 0x4cu;       // imul eax,eax,0x4c @0x00430010
static constexpr std::uintptr_t kDispC_ArrowName   = 0x005cd828u;  // "Arrow", push @0x0042fff2/0x0043008e

// Float constants, written as the exact bits at their .rdata address / immediate.
static const float kDispC_ArrowXk  = DispC_F(0x3f040000u);  // 0.515625   fmul [0x005cd8a4] @0x0042ff63
static const float kDispC_YBias    = DispC_F(0x41000000u);  // 8.0        fsub [0x005cc9f4] @0x0042ff81
static const float kDispC_YScale   = DispC_F(0x3b088889u);  // 1/480      fmul [0x005cc560] @0x0042ff89
static const float kDispC_WScale   = DispC_F(0x3ccccccdu);  // 0.025      fmul [0x005cc9a4] @0x0042ffa3
static const float kDispC_HScale   = DispC_F(0x3d088889u);  // 1/30       fmul [0x005cc324] @0x0042ffe0
static const float kDispC_RowStep  = DispC_F(0x41f00000u);  // 30.0       fadd [0x005cc72c] @0x004300b9
static const float kDispC_TextX    = DispC_F(0x43af0000u);  // 350.0      push imm @0x00430029
static const float kDispC_TextSc   = DispC_F(0x3f333333u);  // 0.7        push imm @0x00430022/0x0043003c
static const float kDispC_Uv       = DispC_F(0x3f7ff972u);  // 0.9999      push imm @0x0042ffd6
static const float kDispC_One      = DispC_F(0x3f800000u);  // 1.0        push imm @0x0042ffc9

// 0x0042fe90
extern "C" __declspec(dllexport) void __cdecl MenuSpriteDispatchC(void)
{
    // Guard and descriptor pick.
    const std::int32_t mode = *reinterpret_cast<const std::int32_t*>(kDispC_Mode);
    if (mode == 2) return;                                                     // je @0x0042fe9b
    std::uintptr_t desc_table = kDispC_DescOther;                              // @0x0042feb6
    if (mode == 0) {
        if (*reinterpret_cast<const std::int32_t*>(kDispC_Alpha) < 0x60)      // cmp/jl @0x0042fea9 (signed)
            return;
    } else if (mode == 1) {
        desc_table = kDispC_DescMode1;                                         // @0x004300fb..0x0043010c
    }
    const std::uint32_t screen = *reinterpret_cast<const std::uint32_t*>(kDispC_Screen);
    const std::uint32_t desc =
        *reinterpret_cast<const std::uint32_t*>(desc_table + (screen << 6));  // shl 6 @0x0042febc/0x00430109

    // Row origin: FUN_0042ac00(EDX = desc) @0x0042fec9, FUN_0042ac50(EAX = count, ECX = 0x1e) @0x0042fece.
    const int count = s_MenuGroupCount(0, reinterpret_cast<int*>(desc));
    float y = static_cast<float>(DispC_MenuBaseY(count, 0x1e));                // fild/fstp @0x0042fee2/0x0042feea

    vtable_render_state(6, 0);                                                 // @0x0042feee
    vtable_render_state(8, 0);                                                 // @0x0042fefa

    std::uint8_t alpha8 = *reinterpret_cast<const std::uint8_t*>(kDispC_Alpha);  // mov cl,[0x67e814] @0x0042fefd
    int i = 0;
    for (std::uintptr_t entry = kDispC_EntryFirst; entry < kDispC_EntryEnd;
         entry += kDispC_EntryStride, ++i, y = y + kDispC_RowStep) {          // @0x004300b2..0x004300d1
        const std::uint32_t scr = *reinterpret_cast<const std::uint32_t*>(kDispC_Screen);
        const std::int32_t dim = *reinterpret_cast<const std::int32_t*>(
            kDispC_DimTable + ((scr << 4) + static_cast<std::uint32_t>(i)) * 4u);  // @0x0042ff15..0x0042ff20
        std::uint8_t a = alpha8;                                               // @0x0042ff29
        if (dim == 0) a = static_cast<std::uint8_t>(alpha8 >> 2);             // shr dl,2 @0x0042ff31
        if (*reinterpret_cast<const std::uint32_t*>(entry) == kDispC_SkipHead) // @0x0042ff38
            continue;
        const std::uint32_t color = static_cast<std::uint32_t>(a) << 24;      // bytes 0x10..0x12 = 0 @0x0042ff44

        // Left Arrow quad.
        const int w1 = static_cast<std::int16_t>(s_ScreenWidthGet());          // @0x0042ff53 movsx
        const float ax = static_cast<float>(static_cast<double>(w1) * kDispC_ArrowXk);
        const int h1 = static_cast<std::int16_t>(s_ScreenHeightGet());         // @0x0042ff6d
        const float by = static_cast<float>(
            static_cast<double>(h1) * (static_cast<double>(y) - kDispC_YBias) * kDispC_YScale);  // @0x0042ff79..0x0042ff8f
        const int w2 = static_cast<std::int16_t>(s_ScreenWidthGet());          // @0x0042ff93
        const float cw = static_cast<float>(static_cast<double>(w2) * kDispC_WScale);
        const int h2 = static_cast<std::int16_t>(s_ScreenHeightGet());         // @0x0042ffad
        const float dh = static_cast<float>(static_cast<double>(h2) * kDispC_HScale);
        s_TextSpriteScaled(s_SpriteLookupC(reinterpret_cast<void*>(kDispC_ArrowName)),  // @0x0042fff7/0x00430000
                           ax, by, cw, dh, color, kDispC_Uv, 0.0f, 0.0f, kDispC_One, 0, 0);

        // Glyph 0x59 + (player field != 0).
        const std::int32_t type = *reinterpret_cast<const std::int32_t*>(
            kDispC_TypeIdx + static_cast<std::uintptr_t>(i) * 4u);
        const std::uint32_t field = *reinterpret_cast<const std::uint32_t*>(
            kDispC_PlayerField + static_cast<std::uintptr_t>(type) * kDispC_PlayerStride);
        const int id = (field != 0 ? 1 : 0) + 0x59;                            // setne/add 0x59 @0x0043001f/0x0043002e
        s_SpriteDraw(id, kDispC_TextX, y, color, kDispC_TextSc, 0);            // @0x00430034

        // Right Arrow quad at x = (W * (width + 0x166)) / 640.
        const int width = DispC_MeasureTrunc(id);                              // @0x00430042/0x0043004a
        const int w3 = static_cast<std::int16_t>(s_ScreenWidthGet());          // @0x00430051
        const int prod = static_cast<int>(static_cast<std::uint32_t>(w3) *
                                          static_cast<std::uint32_t>(width + 0x166));  // @0x00430059/0x0043005f
        const int rx = prod / 640;                                             // 0x66666667, sar 8 @0x00430062..0x00430077
        s_TextSpriteUVExplicit(s_SpriteLookupC(reinterpret_cast<void*>(kDispC_ArrowName)),  // @0x00430093/0x0043009c
                               static_cast<float>(rx), by, cw, dh, color, 1);
        alpha8 = *reinterpret_cast<const std::uint8_t*>(kDispC_Alpha);         // @0x004300a1
    }

    vtable_render_state(6, 1);                                                 // @0x004300e0
    vtable_render_state(8, 1);                                                 // @0x004300ed
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
// Original: FUN_0042e5b0 (0x0042e5b0..0x0042e8a2), void(void), cdecl; saves
// ebx/ebp/esi/edi.
//
// Frontend background + track-preview cross-fade + logo overlay.
// - FUN_00473c20(0, 0, 512.0, 512.0, 0, DAT_00898ab8, 0, DAT_008991a0, 0, 0xff) @0x0042e655.
// - phase = _ftol(DAT_007f1010 * 60.0 [0x005cc728]) & 0x1ff (fld/fmul @0x0042e65a/0x0042e660,
//   FUN_004a2c48 @0x0042e698 truncates ST0, and 0x1ff @0x0042e69d).
// - phase 0x1e0: DAT_0067e848 += 2 (@0x0042e6ad); phase 0xe0: DAT_0067f0b8 += 2 (@0x0042e6df);
//   then DAT_0067f0b8 forced odd (@0x0042e6b9..0x0042e6bf).
// - Alphas, A = ecx, B = ebx (0x0042e6c5..0x0042e740):
//     phase < 0xe0:   A 0xff, B 0
//     0xe0..0xef:     A 0xff, B (phase-0xe0)<<4
//     0xf0..0xff:     A (0x1f-(phase-0xe0))<<4, B 0xff
//     0x100..0x1df:   A 0,    B 0xff
//     0x1e0..0x1ef:   A (phase-0x1e0)<<4, B 0xff
//     0x1f0..0x1ff:   A 0xff, B (0x1f-(phase-0x1e0))<<4
// - Slot table on the stack (0x0042e5c4..0x0042e64d): 0,1,2,3,4,5,0,1,2,3,4,5,0,1,2,3.
// - Each drawn sprite: h = FUN_0042e590(sel, 288.0, 64.0, 352.0, 352.0, colour, frame, slot, 1),
//   add esp,4, push h, FUN_00474890(h, <the same 8 slots>) (0x0042e7ac..0x0042e7ba,
//   0x0042e7fe..0x0042e80c). FUN_00474890 takes NINE arguments, the eight left on the stack.
// - Slide DAT_008990e0: f = float(slide + 512.0 [0x005cd65c]) (fild/fadd @0x0042e80f/0x0042e81c),
//   slide -= 0x10, then < 0 -> 0 (jns @0x0042e848), > 0x200 -> 0x200 (@0x0042e861).
// - FUN_00473ee0(0, 0, f, 512.0, 0, DAT_00898ab8, 0, DAT_008991a0, 0, 0xff) @0x0042e897, the two
//   handles RE-READ after the sprite draws (@0x0042e86a/0x0042e870).
//
// ref: re/analysis/promote_c2_frontend_menus/0x0042e5b0.md
// ---------------------------------------------------------------------------

// Globals (cited at 0x0042e5b0 body):
static constexpr std::uintptr_t kChrB_BGHandle      = 0x00898ab8u; // dword @0x0042e5e8 / 0x0042e870
static constexpr std::uintptr_t kChrB_BGHandleB     = 0x008991a0u; // dword @0x0042e5d0 / 0x0042e86a
static constexpr std::uintptr_t kChrB_SpriteACount  = 0x0067e848u; // @0x0042e6ad / 0x0042e745
static constexpr std::uintptr_t kChrB_SpriteBCount  = 0x0067f0b8u; // @0x0042e6b3 / 0x0042e6d7
static constexpr std::uintptr_t kChrB_SlideOffset   = 0x008990e0u; // @0x0042e80f..0x0042e865

// Sprite cycle table: index 0..15 → slot 0..5 repeating. Stack stores @0x0042e5c4..0x0042e64d.
static const int kChrB_SlotCycle[16] = { 0,1,2,3,4,5, 0,1,2,3,4,5, 0,1,2,3 };

static const float kChrB_SlideBase = DispC_F(0x44000000u);   // 512.0  fadd [0x005cd65c] @0x0042e81c

// Call helper (not a port) for 0x0042e65a..0x0042e698: the phase is FUN_004a2c48 (_ftol)
// of the UNROUNDED x87 product DAT_007f1010 * 60.0. The old body called FUN_004a2c48 with
// nothing loaded in ST0.
static int ChrB_Phase()
{
    int r;
    __asm {
        fld  dword ptr ds:[0x007f1010]
        fmul dword ptr ds:[0x005cc728]
        mov  eax, 0x004a2c48
        call eax
        mov  r, eax
    }
    return r;
}

// Call helper (not a port) for 0x0042e78f..0x0042e7ba / 0x0042e7c9..0x0042e80c: push nine
// dwords, call FUN_0042e590, drop the first (add esp,4), push the result in its place and
// call FUN_00474890 over the same eight slots, then add esp,0x24. FUN_0042e590 may rewrite
// its own argument slots (it overwrites arg1 before its tail call), so FUN_00474890 must
// see the very same stack slots, not a re-pushed copy.
static void ChrB_ThunkCommit(std::uint32_t sel, std::uint32_t colour, int frame, int slot)
{
    __asm {
        push 1
        push slot
        push frame
        push colour
        push 0x43b00000                         // 352.0
        push 0x43b00000                         // 352.0
        push 0x42800000                         // 64.0
        push 0x43900000                         // 288.0
        push sel
        mov  eax, 0x0042e590
        call eax
        add  esp, 4
        push eax
        mov  eax, 0x00474890
        call eax
        add  esp, 0x24
    }
}

// 0x0042e5b0
extern "C" __declspec(dllexport) void __cdecl MenuChromeShellB(void)
{
    s_DrawFullscreenBG(0.0f, 0.0f, 512.0f, 512.0f, 0,                          // @0x0042e655
                       *reinterpret_cast<const int*>(kChrB_BGHandle), 0,
                       *reinterpret_cast<const int*>(kChrB_BGHandleB), 0, 0xff);

    const int phase = ChrB_Phase() & 0x1ff;                                    // @0x0042e69d
    if (phase == 0x1e0) {
        *reinterpret_cast<int*>(kChrB_SpriteACount) += 2;                      // @0x0042e6ad
    } else if (phase == 0xe0) {
        *reinterpret_cast<int*>(kChrB_SpriteBCount) += 2;                      // @0x0042e6df
    }
    int b_count = *reinterpret_cast<const int*>(kChrB_SpriteBCount);
    if ((b_count & 1) == 0) {                                                  // test dl,1 @0x0042e6b9
        b_count += 1;
        *reinterpret_cast<int*>(kChrB_SpriteBCount) = b_count;                 // @0x0042e6bf
    }

    int alphaA, alphaB;                                                        // ecx, ebx
    if (phase < 0xe0) {
        alphaB = 0; alphaA = 0xff;                                             // @0x0042e6cc/0x0042e740
    } else if (phase < 0x100) {
        const int t = phase - 0xe0;
        if (t < 0x10) { alphaB = t << 4;          alphaA = 0xff; }             // @0x0042e6f8
        else          { alphaA = (0x1f - t) << 4; alphaB = 0xff; }             // @0x0042e6ff
    } else if (phase < 0x1e0) {
        alphaA = 0; alphaB = 0xff;                                             // @0x0042e717
    } else {
        const int t = phase - 0x1e0;
        if (t < 0x10) { alphaA = t << 4;          alphaB = 0xff; }             // @0x0042e72a
        else          { alphaB = (0x1f - t) << 4; alphaA = 0xff; }             // @0x0042e736
    }

    const int slotA = kChrB_SlotCycle[*reinterpret_cast<const int*>(kChrB_SpriteACount) & 0xf];  // @0x0042e751
    const int slotB = kChrB_SlotCycle[b_count & 0xf];                          // @0x0042e758
    std::uint32_t colour = 0x00ffffffu;                                        // bytes @0x0042e669..0x0042e673

    if (alphaA != 0) {                                                         // @0x0042e75c
        int f = phase + 0x20;
        if (f > 0x1ff) f -= 0x200;                                             // @0x0042e76f/0x0042e77a
        colour = (colour & 0x00ffffffu) | (static_cast<std::uint32_t>(alphaA & 0xff) << 24);  // @0x0042e774
        ChrB_ThunkCommit(0, colour, (f << 8) / 0x120, slotA);                  // 0x38e38e39, sar 6 @0x0042e784..0x0042e79a
    }
    if (alphaB != 0) {                                                         // @0x0042e7bd
        colour = (colour & 0x00ffffffu) | (static_cast<std::uint32_t>(alphaB & 0xff) << 24);  // @0x0042e7ef
        ChrB_ThunkCommit(1, colour, ((phase - 0xe0) << 8) / 0x120, slotB);    // @0x0042e7cc..0x0042e7e4
    }

    const int slide_old = *reinterpret_cast<const int*>(kChrB_SlideOffset);
    const float slide_f = static_cast<float>(static_cast<double>(slide_old) + kChrB_SlideBase);  // @0x0042e80f..0x0042e82e
    const std::int32_t slide = static_cast<std::int32_t>(static_cast<std::uint32_t>(slide_old) - 0x10u);  // @0x0042e823
    *reinterpret_cast<int*>(kChrB_SlideOffset) = slide;                        // @0x0042e843
    if (slide < 0)          *reinterpret_cast<int*>(kChrB_SlideOffset) = 0;    // jns @0x0042e848
    else if (slide > 0x200) *reinterpret_cast<int*>(kChrB_SlideOffset) = 0x200;  // jle @0x0042e863

    s_LogoOverlayDraw(0, 0, slide_f, 512.0f, 0,                                // @0x0042e897
                      *reinterpret_cast<const int*>(kChrB_BGHandle), 0,
                      *reinterpret_cast<const int*>(kChrB_BGHandleB), 0, 0xff);
}

RH_ScopedInstall(MenuChromeShellB, 0x0042e5b0);  // re-ported from the bytes 2026-10-12 (negctl re-earn)
