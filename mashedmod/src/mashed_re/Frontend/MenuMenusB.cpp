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

// String-table base used by FUN_00427780 (Ghidra 0x00427784/0x0042778b: [param_1*4 + 0x66d828]
// then + 0x66d828). FUN_00427780 is a PURE function (no side effects) -> safe to inline.
static constexpr std::uintptr_t kStrTableBase = 0x0066d828u;

// FIX (2026-06-07 navigate-C4): the previous reimpl called s_FUN_00427780(param_1) then
// s_FUN_004277a0() as plain __cdecl(void) calls. But FUN_004277a0 is a REGISTER-ARG function:
// it reads its source from in_EAX (the pointer FUN_00427780 returns) and writes to unaff_EBX
// (the caller's output buffer = local_404; Ghidra 0x004282c7 LEA EBX,[ESP+8]). Calling them as
// void __cdecl left EAX clobbered between the two -> FUN_004277a0 derefs garbage -> AV at
// 0x00427813 when installed. (The old crash_equal_ok C3 missed this: the synthetic force-call
// never set EAX/EBX either, so orig+reimpl crashed identically = false GREEN.) FUN_00427780 and
// FUN_004277a0 are both side-effect-free except for filling local_404, so we reimplement their
// logic inline and keep the (correct, __cdecl-stack) FUN_005554d0 call.

// 0x004282a0
extern "C" __declspec(dllexport) float __cdecl MenuMenusBA(
    std::uint32_t param_1, float param_2)
{
    // local_404: 0x400-byte stack buffer that holds the transcoded string for FUN_005554d0.
    std::uint8_t local_404[kMenuBa_StrBufSize];

    // inline FUN_00427780(param_1): src = *(u32*)(0x66d828 + param_1*4) + 0x66d828
    const std::uint16_t* src = reinterpret_cast<const std::uint16_t*>(
        *reinterpret_cast<std::uint32_t*>(kStrTableBase + param_1 * 4u) + kStrTableBase);

    // inline FUN_004277a0(): src is a u16 length-prefixed string; transcode into local_404,
    // remapping the control codes 8/9/10/0xb/0xc/0xd/0xe (Ghidra 0x004277a0 body).
    short slen = static_cast<short>(src[0]);
    short* dst = reinterpret_cast<short*>(local_404);
    for (short i = 0; i < slen; ++i) {
        short c = static_cast<short>(src[1 + i]);
        switch (c) {
            case 8:    c = 0x81; break;
            case 9:    c = 0x7f; break;
            case 10:   c = 0x81; break;
            case 0x0b: c = 0x8d; break;
            case 0x0c: c = 0x80; break;
            case 0x0d: c = 0x87; break;
            case 0x0e: c = 0x8f; break;
        }
        dst[i] = c;
    }
    dst[slen] = 0;   // matches original's unaff_EBX[(short)len] = 0

    // FUN_005554d0(font_ctx, local_404, param_2 * size_scale) — __cdecl stack args (0x004282ec)
    void* font_ctx   = *reinterpret_cast<void**>(kMenuBa_FontCtx);
    float size_scale = *reinterpret_cast<float*>(kMenuBa_SizeScale);
    float raw_width  = s_FUN_005554d0(font_ctx, local_404, param_2 * size_scale);

    // (raw_width / viewport_w) * logical_scale  (0x004282f1..0x00428309)
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

    // Step 1+2: inline FUN_00427780(param_1) + FUN_004277a0() — same register-ABI fix as
    // MenuMenusBA (FUN_004277a0 reads in_EAX = FUN_00427780's returned ptr, writes to
    // unaff_EBX = local_204). Calling them as void __cdecl clobbers EAX between -> AV. Both are
    // side-effect-free except filling local_204, so reimplement their logic inline.
    {
        const std::uint16_t* src = reinterpret_cast<const std::uint16_t*>(
            *reinterpret_cast<std::uint32_t*>(kStrTableBase + param_1 * 4u) + kStrTableBase);
        short slen = static_cast<short>(src[0]);
        short* dst = reinterpret_cast<short*>(local_204);
        for (short i = 0; i < slen; ++i) {
            short c = static_cast<short>(src[1 + i]);
            switch (c) {
                case 8:    c = 0x81; break;
                case 9:    c = 0x7f; break;
                case 10:   c = 0x81; break;
                case 0x0b: c = 0x8d; break;
                case 0x0c: c = 0x80; break;
                case 0x0d: c = 0x87; break;
                case 0x0e: c = 0x8f; break;
            }
            dst[i] = c;
        }
        dst[slen] = 0;
    }

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
// Original: FUN_0042f8d0  (body 0x0042f8d0..0x0042f9f8, 102 instructions)
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
RH_ScopedInstall(MenuMenusBC, 0x0042f8d0);

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
