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
#include "MenuMeasure.h"
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
// MenuMenusBA: the body is Frontend/MenuMeasure.cpp, shared with the exe (D4-DC-82a0, 2026-10-25).
// This TU supplies the .asi's cells: the original's callees at their absolute addresses and the
// addresses of the two globals the body reads (font context at 0x004282dd, divisor at 0x004282f1).
// ---------------------------------------------------------------------------
extern "C" void*        g_menuBa_Lookup   = reinterpret_cast<void*>(0x00427780u);
extern "C" void*        g_menuBa_Remap    = reinterpret_cast<void*>(0x004277a0u);
extern "C" void*        g_menuBa_Measure  = reinterpret_cast<void*>(0x005554d0u);
extern "C" const void*  g_menuBa_FontCell = reinterpret_cast<const void*>(0x0067d838u);
extern "C" const float* g_menuBa_DivCell  = reinterpret_cast<const float*>(0x0067d830u);

// ---------------------------------------------------------------------------
// MenuMenusBB (0x00427ad0): moved to Frontend/MenuMenusBB.cpp (drawstream 2026-10-12), an x87 TU. Under
// SSE2 its rect math rounded w*xs to float before the second multiply; see that file.
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// MenuMenusBC  --  0x0042f8d0
//
// Original: FUN_0042f8d0  (body 0x0042f8d0..0x0042f9f8, 95 instructions)
// Signature: void __cdecl FUN_0042f8d0(float x, float y, float w, float h)
//            PLUS a fifth argument, the panel ALPHA, passed in AL.
//
// THE ALPHA IS A REGISTER ARGUMENT (0x0042f8d9 `mov bl, al`, before any stack
// slot is touched). Ghidra does surface it, as `byte in_AL` feeding two
// CONCAT13s; the 2026-05-11 transcription replaced below read that as a
// decompiler artifact and hardcoded both colours to 0. Every call site reloads
// AL first: 0x0043a67a `mov al,[0x67e7dc]` for the first ability plate, then
// 0x0043a70a / 0x0043a733 / 0x0043a75b / 0x0043a7fc from stack copies, and
// 0x0043ab53 / 0x0043ab8a / 0x0043ac91 `mov al, bl` on the team side.
// See re/analysis/race_hud_capture_20260902.md Finding 30.
//
// Ghidra (headless against a pool slot, 2026-09-05):
//   local_c = CONCAT13(in_AL >> 1, 0x146ef0);
//   FUN_00472c60(param_1, param_2, param_3, param_4, local_c);
//   param_1 = param_1 - _DAT_005cc574;                    <-- ARG SLOT REWRITTEN
//   local_c = CONCAT13(in_AL, 0x1050b4);
//   FUN_00472c60(param_1, param_2, 0x40000000, param_4, local_c);
//   fVar1 = param_3 + _DAT_005cc35c;
//   FUN_00472c60(param_1, param_2, fVar1, 0x40000000, local_c);
//   fVar2 = param_4 - _DAT_005cc574;
//   param_2 = fVar2 + param_2;                            <-- ARG SLOT REWRITTEN
//   FUN_00472c60(param_1, param_2, fVar1, 0x40000000, local_c);
//   FUN_00472c60(param_3 + param_1 + _DAT_005cc574, param_2 - fVar2,
//                0x40000000, param_4, local_c);
//
// Calls 1-4 were transcribed correctly in 2026-05. Call 5 was not: it reads the
// ALREADY-REWRITTEN param_1 and param_2, so its x is (w + (x - BX)) + BX and
// its y is (y + (h - BX)) - (h - BX), NOT `w + x + BX` and `y - (h - BX)`,
// which is what the old body computed. The two rewrites land in the CALLER's
// argument slots, at 0x0042f933 (`fstp [esp+0x34]`, the x slot) and 0x0042f996
// (`fstp [esp+0x6c]`, the y slot).
//
// WHY NAKED VERBATIM RATHER THAN C. One store in the middle is `fst`, not
// `fstp`:
//   0x0042f97d  fld  dword ptr [esp+0x54]     ; h (local copy)
//   0x0042f981  fsub dword ptr [0x5cc574]     ; ST0 = h - BX
//   0x0042f98d  fst  dword ptr [esp+0x54]     ; rounds to float IN MEMORY only
//   0x0042f992  fadd dword ptr [esp+0x6c]     ; ST0 (UNROUNDED) + y
// so fVar2 exists at two precisions at once: the rounded float that call 5's
// `fsub` reads back, and the register value call 4's `fadd` consumes. No
// `float` local holds both, and reproducing it with `double` would silently
// depend on whichever x87 precision-control bits MASHED's CRT has set. The
// transcription below is the instruction sequence, so the question does not
// arise. Only the five `E8 rel32` calls change form, to a memory-indirect call
// through g_menuBc_Quad: same stack effect, and it clobbers no register.
//
// Colours, built byte-by-byte on the 12-byte local block:
//   A (fill)   0x0042f8e9..0x0042f8f7  ->  0x{alpha>>1}146ef0
//   B (border) 0x0042f920..0x0042f92f  ->  0x{alpha}1050b4
//
// Globals:
//   _DAT_005cc574  border inset X  (read at 0x0042f91a, 0x0042f981, 0x0042f9b5)
//   _DAT_005cc35c  border inset Y  (read at 0x0042f959)
//
// ref: re/analysis/frontend_promote_menus_b/0042f8d0.md
//      re/analysis/race_hud_capture_20260902.md Finding 30
// ---------------------------------------------------------------------------

// Indirection cell for the five `call 0x00472c60` sites. A naked body cannot
// emit the original's `E8 rel32` (it would resolve against our own address), so
// each becomes `call dword ptr [g_menuBc_Quad]`: 6 bytes instead of 5, same
// push-return-address stack effect, no scratch register consumed. Deliberately
// non-const so the storage is guaranteed to exist for the asm to reference.
void* g_menuBc_Quad = reinterpret_cast<void*>(0x00472c60u);

// 0x0042f8d0
// Verbatim transcription. Every line carries the address of the instruction it
// reproduces; the stack displacements are the original's and are only valid
// against this exact prologue, so do not "tidy" the pushes.
extern "C" __declspec(dllexport) __declspec(naked) void __cdecl MenuMenusBC(
    float /*x*/, float /*y*/, float /*w*/, float /*h*/)
{
    __asm {
        sub  esp, 0xc                           // 0x0042f8d0
        push ebx                                // 0x0042f8d3
        push ebp                                // 0x0042f8d4
        mov  ebp, dword ptr [esp + 0x1c]        // 0x0042f8d5  ebp = y
        mov  bl, al                             // 0x0042f8d9  bl  = ALPHA (AL arg)
        mov  eax, dword ptr [esp + 0x20]        // 0x0042f8db  eax = w
        push esi                                // 0x0042f8df
        mov  cl, bl                             // 0x0042f8e0
        push edi                                // 0x0042f8e2
        mov  edi, dword ptr [esp + 0x2c]        // 0x0042f8e3  edi = h
        shr  cl, 1                              // 0x0042f8e7  fill alpha = a >> 1
        mov  byte ptr [esp + 0x13], cl          // 0x0042f8e9
        mov  byte ptr [esp + 0x10], 0xf0        // 0x0042f8ed  colour A = 0x{a>>1}146ef0
        mov  byte ptr [esp + 0x11], 0x6e        // 0x0042f8f2
        mov  byte ptr [esp + 0x12], 0x14        // 0x0042f8f7
        mov  edx, dword ptr [esp + 0x10]        // 0x0042f8fc
        push edx                                // 0x0042f900  arg5 colour A
        push edi                                // 0x0042f901  arg4 h
        push eax                                // 0x0042f902  arg3 w
        mov  dword ptr [esp + 0x20], eax        // 0x0042f903  local copy of w
        mov  eax, dword ptr [esp + 0x2c]        // 0x0042f907  eax = x
        push ebp                                // 0x0042f90b  arg2 y
        push eax                                // 0x0042f90c  arg1 x
        mov  dword ptr [esp + 0x2c], edi        // 0x0042f90d  local copy of h
        call dword ptr [g_menuBc_Quad]          // 0x0042f911  call 1: centre fill
        fld  dword ptr [esp + 0x34]             // 0x0042f916  x
        fsub dword ptr ds:[0x005cc574]          // 0x0042f91a  x - BX
        mov  byte ptr [esp + 0x24], 0xb4        // 0x0042f920  colour B = 0x{a}1050b4
        mov  byte ptr [esp + 0x25], 0x50        // 0x0042f925
        mov  byte ptr [esp + 0x26], 0x10        // 0x0042f92a
        mov  byte ptr [esp + 0x27], bl          // 0x0042f92f  full alpha, not halved
        fstp dword ptr [esp + 0x34]             // 0x0042f933  REWRITE caller's x slot
        mov  esi, dword ptr [esp + 0x24]        // 0x0042f937  esi = colour B
        push esi                                // 0x0042f93b  arg5
        push edi                                // 0x0042f93c  arg4 h
        mov  edi, dword ptr [esp + 0x3c]        // 0x0042f93d  edi = x - BX
        mov  dword ptr [esp + 0x44], 0x40000000 // 0x0042f941  2.0f
        mov  ecx, dword ptr [esp + 0x44]        // 0x0042f949
        push ecx                                // 0x0042f94d  arg3 2.0
        push ebp                                // 0x0042f94e  arg2 y
        push edi                                // 0x0042f94f  arg1 x - BX
        call dword ptr [g_menuBc_Quad]          // 0x0042f950  call 2: left edge
        fld  dword ptr [esp + 0x3c]             // 0x0042f955  w
        fadd dword ptr ds:[0x005cc35c]          // 0x0042f959  w + BY
        push esi                                // 0x0042f95f  arg5
        mov  dword ptr [esp + 0x58], 0x40000000 // 0x0042f960  2.0f
        mov  ebx, dword ptr [esp + 0x58]        // 0x0042f968  ebx: alpha is spent here
        push ebx                                // 0x0042f96c  arg4 2.0
        fstp dword ptr [esp + 0x58]             // 0x0042f96d
        mov  edx, dword ptr [esp + 0x58]        // 0x0042f971
        push edx                                // 0x0042f975  arg3 w + BY
        push ebp                                // 0x0042f976  arg2 y
        push edi                                // 0x0042f977  arg1 x - BX
        call dword ptr [g_menuBc_Quad]          // 0x0042f978  call 3: top edge
        fld  dword ptr [esp + 0x54]             // 0x0042f97d  h
        fsub dword ptr ds:[0x005cc574]          // 0x0042f981  ST0 = h - BX
        mov  eax, dword ptr [esp + 0x64]        // 0x0042f987  eax = w + BY
        push esi                                // 0x0042f98b  arg5
        push ebx                                // 0x0042f98c  arg4 2.0
        fst  dword ptr [esp + 0x54]             // 0x0042f98d  NOT fstp: ST0 survives
        push eax                                // 0x0042f991  arg3 w + BY
        fadd dword ptr [esp + 0x6c]             // 0x0042f992  unrounded (h-BX) + y
        fstp dword ptr [esp + 0x6c]             // 0x0042f996  REWRITE caller's y slot
        mov  ecx, dword ptr [esp + 0x6c]        // 0x0042f99a
        push ecx                                // 0x0042f99e  arg2 y + (h - BX)
        push edi                                // 0x0042f99f  arg1 x - BX
        call dword ptr [g_menuBc_Quad]          // 0x0042f9a0  call 4: bottom edge
        fld  dword ptr [esp + 0x64]             // 0x0042f9a5  w
        fadd dword ptr [esp + 0x70]             // 0x0042f9a9  + REWRITTEN x (x - BX)
        mov  edx, dword ptr [esp + 0x68]        // 0x0042f9ad  edx = h
        add  esp, 0x50                          // 0x0042f9b1  drop calls 1-4 args
        push esi                                // 0x0042f9b4  arg5
        fadd dword ptr ds:[0x005cc574]          // 0x0042f9b5  + BX
        mov  eax, edx                           // 0x0042f9bb
        mov  dword ptr [esp + 0x2c], 0x40000000 // 0x0042f9bd  2.0f
        mov  ecx, dword ptr [esp + 0x2c]        // 0x0042f9c5
        fstp dword ptr [esp + 0x24]             // 0x0042f9c9
        mov  dword ptr [esp + 0x30], edx        // 0x0042f9cd  restore h into its slot
        fld  dword ptr [esp + 0x28]             // 0x0042f9d1  REWRITTEN y
        push eax                                // 0x0042f9d5  arg4 h
        fsub dword ptr [esp + 0x18]             // 0x0042f9d6  - rounded (h - BX)
        fstp dword ptr [esp + 0x2c]             // 0x0042f9da
        mov  edx, dword ptr [esp + 0x2c]        // 0x0042f9de
        mov  eax, dword ptr [esp + 0x28]        // 0x0042f9e2
        push ecx                                // 0x0042f9e6  arg3 2.0
        push edx                                // 0x0042f9e7  arg2
        push eax                                // 0x0042f9e8  arg1
        call dword ptr [g_menuBc_Quad]          // 0x0042f9e9  call 5: right edge
        add  esp, 0x14                          // 0x0042f9ee
        pop  edi                                // 0x0042f9f1
        pop  esi                                // 0x0042f9f2
        pop  ebp                                // 0x0042f9f3
        pop  ebx                                // 0x0042f9f4
        add  esp, 0xc                           // 0x0042f9f5
        ret                                     // 0x0042f9f8  __cdecl, caller cleans
    }
}

// RE-ENABLED 2026-09-05 (was MASS-DISABLED 2026-05-24 hangs-harness).
// The 2026-05-24 note recorded the right symptom and the wrong conclusion: the
// synthetic path1 diff does not "hang", it drives a renderer with no live RW
// device and no live vertex buffer. arg_type='none' also called this with a
// zero-length argument list, so all four floats and the AL alpha were whatever
// happened to be left in the frame. The registry entry is retired (see
// re/frida/hooks_registry.py 'menu_menus_bc'); acceptance for this function is
// the hook-on vs hook-off draw-stream A/B with an off-vs-off control, which is
// the channel that detected the missing AL argument in the first place.
// 2026-10-12 (a2-dialog): path1 is live again. re/frida/negctl_fixtures/z_fix_a2_dialog.py drives AL through
// `eax_arg` with a decoy in the 5th stack slot, and replaces 0x00472c60 with a recorder, so x/y/w/h and both
// colours of all five quads are compared. Witnesses: AL ignored (`mov bl,0`) RED 8/9, alpha read from the
// 5th stack slot RED 9/9 (re/analysis/negctl/followup_20261012/a2-dialog_RESULT.md).
RH_ScopedInstall(MenuMenusBC, 0x0042f8d0);

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
