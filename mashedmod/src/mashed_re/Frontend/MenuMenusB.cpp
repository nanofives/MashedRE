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

// ---------------------------------------------------------------------------
// MenuMenusBA (0x004282a0): since MATCH-S06-FU the one body of both targets is the byte-matched source
// re/match/s06/004282a0.c, compiled by Match/MatchS06_004282a0.cpp, which binds the .asi's globals and callees
// itself ([0x0067d838], [0x0067d830], FontText_StringTableLookup, TextCtrlCodeRemapBody, the original's 0x005554d0).
// The cells this TU used to define for the naked body (D4-DC-82a0) are gone.

// ---------------------------------------------------------------------------
// MenuMenusBB (0x00427ad0): moved to Frontend/MenuMenusBB.cpp (drawstream 2026-10-12), an x87 TU. Under
// SSE2 its rect math rounded w*xs to float before the second multiply; see that file.
// ---------------------------------------------------------------------------

// MATCH-S07: the body of 0x0042f8d0 (MenuMenusBC) moved to Match/MatchS07_0042f8d0.cpp, which compiles the
// byte-matched source re/match/s07/0042f8d0.c. The export MenuMenusBC there is a naked adapter that takes the
// original's AL alpha argument (and carries the RH_ScopedInstall); g_menuBc_Quad went with the naked body.

// ---------------------------------------------------------------------------
// MenuMenusBD  --  0x0040b460
//
// Original: FUN_0040b460  (body 0x0040b460..0x0040b53c)
// Signature: void FUN_0040b460(undefined4 *param_1)
//   param_1: caller-provided int[4] output — receives sorted slot indices
//
// Algorithm:
//   1. Init: copy 4 keys from DAT_008a94f0 (DWORD stride, 0x008a94f0 + 4*i);
//      key = -1 if the dword at 0x007f1a14 + 0x10*i < 0.
//      Fill param_1[slot] = slot  (identity).
//   2. Bubble sort descending (signed), keys and param_1 swapped in parallel,
//      inner bound 3, 2, 1.
//   3. Game mode 4 or 7: override param_1[slot] = FUN_00417740(slot), then re-read the mode
//   4. Game mode 9: if DAT_007f0fcc != 0 → [0,1]; else [1,0]
//
// Globals:
//   DAT_008a94f0  per-slot keys    (+4 stride, 4 elements)     (0x0040b470)
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

// RE-PORTED FROM THE BYTES 2026-10-12 (negctl re-earn). The keys are DWORD-strided:
// mov edx,[eax*4+0x8a94f0] @0x0040b470 reads 0x008a94f0 + 4*i. The old body read
// score_base[i*4] (stride 0x10), so it sorted on the wrong keys. Only the sign words are
// 0x10-strided (ecx = 0x7f1a14, add ecx,0x10 @0x0040b48b, cmp ecx,0x7f1a54 @0x0040b48f).
// The sort sorts the key copy and the output in parallel (0x0040b4bc..0x0040b4d3) with a
// shrinking inner bound (edi = 3, 2, 1: mov edi,3 @0x0040b49a, dec edi @0x0040b4db), and
// DAT_007f0fd0 is RE-READ after the 4/7 override loop (mov eax,[0x7f0fd0] @0x0040b503)
// before the == 9 test @0x0040b508, so the old early return was not the bytes either.

// 0x0040b460
extern "C" __declspec(dllexport) void __cdecl MenuMenusBD(std::int32_t* param_1)
{
    std::int32_t keys[4];                                          // [esp+4..0x13] @0x0040b477
    for (int i = 0; i < 4; i++) {
        keys[i] = reinterpret_cast<const std::int32_t*>(kMenuBd_ScoreBase)[i];          // @0x0040b470
        if (*reinterpret_cast<const std::int32_t*>(kMenuBd_FlagBase + 0x10u * i) < 0)  // cmp/jge @0x0040b47b
            keys[i] = -1;                                                              // @0x0040b480
        param_1[i] = i;                                                                // @0x0040b488
    }

    for (int n = 3; n > 0; n--) {                                  // @0x0040b49a / 0x0040b4db
        for (int j = 0; j < n; j++) {                              // @0x0040b4a0 / 0x0040b4d6
            const std::int32_t lo = keys[j];                       // edx @0x0040b4b4
            const std::int32_t hi = keys[j + 1];                   // eax @0x0040b4b0
            if (lo < hi) {                                         // cmp edx,eax; jge @0x0040b4b8 (signed)
                keys[j] = hi;                                      // @0x0040b4bc
                keys[j + 1] = lo;                                  // @0x0040b4cb
                const std::int32_t o = param_1[j + 1];             // ebp @0x0040b4c3
                param_1[j + 1] = param_1[j];                       // @0x0040b4d3
                param_1[j] = o;                                    // @0x0040b4d1
            }
        }
    }

    // Step 3: game mode override (0x0040b4e0..0x0040b538)
    int game_mode = *reinterpret_cast<const int*>(kMenuBd_GameMode);   // @0x0040b4e0

    if (game_mode == 4 || game_mode == 7) {                        // @0x0040b4e5 / 0x0040b4ea
        for (int i = 0; i < 4; i++) {
            param_1[i] = s_FUN_00417740(i);                        // @0x0040b4f2 / 0x0040b4fa
        }
        game_mode = *reinterpret_cast<const int*>(kMenuBd_GameMode);   // re-read @0x0040b503
    }

    if (game_mode == 9) {                                          // @0x0040b508
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

// MATCH-S06: the body of 0x00429a30 (MenuMenusBE) moved to Match/MatchS06_00429a30.cpp,
// which compiles the byte-matched source re/match/s06/00429a30.c.
