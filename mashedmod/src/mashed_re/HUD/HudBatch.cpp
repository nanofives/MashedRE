// Mashed RE - HUD batch h4 cluster (C2->C3 promotions).
// Subsystem: HUD (in-game dispatchers, font/text helpers, viewport handler).
//
// Binary anchor: MASHED.exe size 2,846,720
//   SHA-256 BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Hooks in this file:
//   0x0041db80  Sub0041db80_HudThresholdDispatch   leaf (vtable+0x48 only)
//   0x005554d0  FontText_StringWidthAccumulator    pure leaf
//   0x00403160  Sub00403160_SubMode0BViewport      MULTI (C2+ callees)
//   0x0041bc50  Sub0041bc50_HudRender29Dispatcher  EAX-thiscall 29-slot vtable dispatcher
//   0x00427780  FontText_StringTableLookup         pure leaf (packed string lookup)
//   0x00427840  FontText_UTF16WidenCopy            __fastcall EAX=src, ECX=dst
//   0x004c57a0  FontCtxMatrix_AllocInit            implicit-EAX-return; vtable+0x118 alloc
//
// Refusals (kept in PROMOTION_QUEUE.md note):
//   0x0041d870  callee FUN_0041d410 still C1 (no C2+ callee)
//   0x0041ded0  callee FUN_0041de80 still C1 (no C2+ callee)
//   0x00427620  all callees < C2 (FUN_00555830, FUN_00556e40 not in csv;
//               00556cd0/004c5930/00552b90 still C1)
//   0x00427680  non-standard ESI implicit output ptr + U-2127 EDI artifact;
//               would require new harness arg_type
//   0x0041c2d0  already hooked in HudDispatch.cpp at 0x0041c2d0
//
// Analysis notes:
//   re/analysis/hud_ingame_promote_c2/0x0041db80.md
//   re/analysis/promote_c2_hud_ingame/0x005554d0.md
//   re/analysis/hud_ingame_promote_c2/0x00403160.md
//   re/analysis/hud_ingame_d2/0x0041bc50.md
//   re/analysis/hud_promote_c2_b/0x00427780.md
//   re/analysis/hud_promote_c2_b/0x00427840.md
//   re/analysis/font_text_d2/font_text_d2-20260503.md   (for 0x004c57a0)

#include "../Core/HookSystem.h"
#include <cstdint>

// ===========================================================================
// 0x0041db80  Sub0041db80_HudThresholdDispatch
//
// Original: FUN_0041db80 (9-byte Ghidra body bounds artifact; decomp expands
// beyond the 9-byte boundary -- see U-0579 in 0x0041db80.md).
// void(void). Reads int32_t DAT_0063d588 and compares against 5 .rdata
// thresholds at 0x005cc35c, 0x005cd238, 0x005cd234, 0x005cd230, 0x005ccdf4.
// Dispatches up to 6 HUD render objects via vtable[0x48] (pointers stored at
// 0x0063d55c..0x0063d570 consecutively).
//
// Leaf for the C2->C3 promotion rule: the only callees are vtable indirections
// (no named direct callees). Bit-identity is observable at the main menu by
// reading DAT_0063d588 and the threshold table, since the LUT is at quiescent
// state (all guards/objects null) -> both orig and reimpl take same branches
// and return without side-effects.
// ===========================================================================

// 0x0041db80
//
// SHORT-THUNK STRUCTURE (verified from binary at 0x0041db80):
//   0x0041db80: b8 5c d5 63 00     MOV EAX, 0x0063d55c   ; pass obj-array base
//   0x0041db85: e9 26 ff ff ff     JMP 0x0041dab0        ; shared body
//
// The shared body lives at 0x0041dab0; the thunk just passes the base ptr
// to the obj-ptr array (here 0x0063d55c) via EAX, and the body reads
// counter/objs as ESI-relative (ESI = EAX inside the body).
//
// Counter is a FLOAT at [ESI+0x2c] (= *(float*)0x0063d588). Thresholds in
// .rdata are also floats. Comparisons use FLD/FCOMP/FNSTSW/TEST AH; we
// reproduce that behavior with C `<` on float (which compiles to UCOMISS
// in MSVC32; functionally equivalent for non-NaN inputs which is the
// quiescent-state expectation).
//
// Each dispatch: load obj from [base+slot]; call *(obj+0x48)(obj).
// Direct field call (NOT vtable indirection): the fn ptr lives at
// obj+0x48 itself. When obj=0, original AVs at 0x48 loading the fn ptr.
extern "C" __declspec(dllexport)
void __cdecl Sub0041db80_HudThresholdDispatch()
{
    // ESI base in the body = 0x0063d55c (from the thunk's MOV EAX,imm).
    const std::uintptr_t base = 0x0063d55cu;

    // Counter is FLOAT at base+0x2c (= 0x0063d588).
    const float counter = *reinterpret_cast<const float*>(base + 0x2cu);

    // Float thresholds in .rdata.
    const float thr_upper     = *reinterpret_cast<const float*>(0x005cc35cu);
    const float thr_lower_mid = *reinterpret_cast<const float*>(0x005ccdf4u);
    const float thr_mid3      = *reinterpret_cast<const float*>(0x005cd230u);
    const float thr_mid2      = *reinterpret_cast<const float*>(0x005cd234u);
    const float thr_mid1      = *reinterpret_cast<const float*>(0x005cd238u);

    // Dispatch helper: load obj from [base+slot]; call *(obj+0x48)(obj).
    // Direct fn ptr at obj+0x48 (no vtable interposition).
    typedef void (__cdecl* DispatchFn)(std::int32_t);
    auto dispatch_at = [](std::uintptr_t obj_ptr_addr) {
        std::int32_t obj = *reinterpret_cast<const std::int32_t*>(obj_ptr_addr);
        DispatchFn fn = *reinterpret_cast<DispatchFn*>(
            static_cast<std::uintptr_t>(obj) + 0x48u);
        fn(obj);
    };

    // Block A: if (counter < thr_upper) -> dispatch obj1 + obj6
    if (counter < thr_upper) {
        dispatch_at(base + 0x00u);   // obj1 (= *0x0063d55c)
        dispatch_at(base + 0x14u);   // obj6 (= *0x0063d570)
    }

    // Block B: if (counter < thr_mid1) ...
    if (counter < thr_mid1) {
        if (counter < thr_mid2) {
            // counter < thr_mid2: dispatch obj2 only
            dispatch_at(base + 0x04u);   // obj2
        } else if (counter < thr_mid3) {
            // counter in [thr_mid2, thr_mid3): dispatch obj2 + obj3
            dispatch_at(base + 0x04u);   // obj2
            dispatch_at(base + 0x08u);   // obj3
        } else if (counter < thr_lower_mid) {
            // counter in [thr_mid3, thr_lower_mid): dispatch obj2 + obj3 + obj4
            dispatch_at(base + 0x04u);   // obj2
            dispatch_at(base + 0x08u);   // obj3
            dispatch_at(base + 0x0cu);   // obj4
        }
        // (counter >= thr_lower_mid && counter < thr_mid1): tail-check below
        if ((counter >= thr_lower_mid) && (counter < thr_upper)) {
            dispatch_at(base + 0x10u);   // obj5
        }
    }
}

RH_ScopedInstall(Sub0041db80_HudThresholdDispatch, 0x0041db80);  // re-enabled 2026-05-24 batch-mixed


// ===========================================================================
// 0x005554d0  REFUSED (post-diff verification)
//
// Initial diff timed out — reimpl ran without crashing while orig was
// reachable. Raw disasm at 0x005554d0 shows the function's FIRST memory
// read is `[ESI+0x134]` followed by a CALL (the analysis note claims it
// starts by reading ctx+0x0c kerning + a `while (*str)` loop). The note's
// signature and body summary are inconsistent with the binary.
// Faithful reimpl requires re-decoding the 120-byte body from scratch.
//
// Refusal logged in PROMOTION_QUEUE.md note for this session.
// ===========================================================================

#if 0
// ===========================================================================
// 0x005554d0  FontText_StringWidthAccumulator  (DISABLED — see refusal note)
//
// Original: FUN_005554d0 (120 bytes). Pure leaf.
// float __cdecl FUN_005554d0(void* font_ctx, byte* str, float scale)
//
// Computes total pixel-advance width of a null-terminated byte string.
// ASCII path:    advance = *(short*)(ctx+0x24 + ch*2)
// Extended path: idx = ch - *(int*)(ctx+0x124); bounds vs *(int*)(ctx+0x128);
//                advance = *(short*)(*(void**)(ctx+0x12c) + idx*2) or 0 OOB.
// Per-char: total += (float)advance + *(float*)(ctx+0x0c) [kerning constant]
// Return: total * scale.
// ===========================================================================

// 0x005554d0
extern "C" __declspec(dllexport)
float __cdecl FontText_StringWidthAccumulator(const void* font_ctx,
                                              const unsigned char* str,
                                              float scale)
{
    const std::uint8_t* ctx = static_cast<const std::uint8_t*>(font_ctx);
    const float kerning = *reinterpret_cast<const float*>(ctx + 0x0c);

    float total = 0.0f;
    while (*str) {
        unsigned char ch = *str++;
        short advance;
        if (ch < 0x80) {
            // ASCII: short[128] at ctx+0x24
            advance = *reinterpret_cast<const short*>(ctx + 0x24 + ch * 2);
        } else {
            // Extended path: base index at ctx+0x124, count at ctx+0x128,
            // table ptr at ctx+0x12c.
            int base  = *reinterpret_cast<const int*>(ctx + 0x124);
            int count = *reinterpret_cast<const int*>(ctx + 0x128);
            int idx   = static_cast<int>(ch) - base;
            if (idx >= 0 && idx < count) {
                const short* tab = *reinterpret_cast<const short* const*>(ctx + 0x12c);
                advance = tab[idx];
            } else {
                advance = 0;
            }
        }
        total += static_cast<float>(advance) + kerning;
    }
    return total * scale;
}

// MASS-DISABLED 2026-05-24 phase-a2-no-registry-deferred: RH_ScopedInstall(FontText_StringWidthAccumulator, 0x005554d0);
#endif


// ===========================================================================
// 0x00403160  Sub00403160_SubMode0BViewport
//
// Original: FUN_00403160 (229 bytes). MULTI; callees:
//   0x004671a0 (C2) get camera handle
//   0x004c19f0 (C3) camera lock / pre-update
//   0x004c1c80 (C2) set viewport (float[2])
//   0x004c1a00 (C1 — BUT) camera begin-update [callee strict-fail; see below]
//   0x00402fb0 (C1) HUD draw body for sub-mode 0xb
//   0x00428760 (C1) conditional draw on DAT_00771964 != 0
//
// NOTE: 0x004c1a00 is C1. The C2->C3 promotion rule requires "at least one
// caller and one callee at C2+". 0x004c19f0 is C3 and 0x004671a0 / 0x004c1c80
// are C2. Caller is HudIngameDispatch at 0x0040dfc0 (C3). Gate satisfied.
//
// void(void). Sets 0.8x0.8 viewport on cam, saves/restores cam+0x68/+0x6c,
// disables render states 6+8 via vtable[+0x20], calls HUD draw body, then
// conditional FUN_00428760 with float args (50,30,240,120).
// Render-state vtable on object at DAT_007d3ff8 (slot +0x20).
// Stack cookie at 0x00616038.
// ===========================================================================

// 0x00403160
extern "C" __declspec(dllexport) void __cdecl Sub00403160_SubMode0BViewport() {
    typedef int   (__cdecl* GetCamFn)(int);
    typedef void  (__cdecl* CamLockFn)(int);
    typedef void  (__cdecl* SetViewportFn)(int cam, void* viewport2f);
    typedef int   (__cdecl* CamBeginFn)(int cam);
    typedef void  (__cdecl* HudDrawBodyFn)();
    typedef void  (__cdecl* CondDrawFn)(std::int32_t, float, float, float, float, std::int32_t);
    typedef void  (__cdecl* RenderStateSetFn)(int state_id, int value);

    int cam = reinterpret_cast<GetCamFn>(0x004671a0)(0);
    reinterpret_cast<CamLockFn>(0x004c19f0)(cam);

    // Save camera+0x68/+0x6c (4 bytes each).
    std::uint32_t saved_w = *reinterpret_cast<std::uint32_t*>(static_cast<std::uintptr_t>(cam) + 0x68);
    std::uint32_t saved_h = *reinterpret_cast<std::uint32_t*>(static_cast<std::uintptr_t>(cam) + 0x6c);

    // Build {0.8f, 0.8f} viewport struct on stack.
    std::uint32_t vp[2] = { 0x3f4ccccdu, 0x3f4ccccdu };
    reinterpret_cast<SetViewportFn>(0x004c1c80)(cam, vp);

    int begin_result = reinterpret_cast<CamBeginFn>(0x004c1a00)(cam);
    if (begin_result != 0) {
        // Render-state vtable on DAT_007d3ff8, slot +0x20.
        std::int32_t rs_obj    = *reinterpret_cast<const std::int32_t*>(0x007d3ff8u);
        std::int32_t rs_vtable = *reinterpret_cast<const std::int32_t*>(static_cast<std::uintptr_t>(rs_obj));
        RenderStateSetFn rs_set = *reinterpret_cast<RenderStateSetFn*>(static_cast<std::uintptr_t>(rs_vtable) + 0x20u);

        rs_set(6, 0);
        rs_set(8, 0);
        reinterpret_cast<HudDrawBodyFn>(0x00402fb0)();

        // Conditional call: DAT_00771964 != 0
        std::int32_t flag = *reinterpret_cast<const std::int32_t*>(0x00771964u);
        if (flag != 0) {
            // Args: float32 50.0f / 30.0f / 240.0f / 120.0f; trailing int 0.
            reinterpret_cast<CondDrawFn>(0x00428760)(
                flag, 50.0f, 30.0f, 240.0f, 120.0f, 0);
        }

        rs_set(6, 1);
        rs_set(8, 1);
        reinterpret_cast<CamLockFn>(0x004c19f0)(cam);
    }

    // Restore camera viewport from saved values.
    std::uint32_t restore_vp[2] = { saved_w, saved_h };
    reinterpret_cast<SetViewportFn>(0x004c1c80)(cam, restore_vp);
    reinterpret_cast<CamBeginFn>(0x004c1a00)(cam);
}

RH_ScopedInstall(Sub00403160_SubMode0BViewport, 0x00403160);  // re-enabled 2026-05-24 batch-mixed


// ===========================================================================
// 0x0041bc50  REFUSED (post-diff verification)
//
// Initial diff at quiescent main menu showed orig AV at 0x1c (first guard
// load) while reimpl AV at 0x10 — i.e., dispatch order in the analysis
// note (0x10, 0x14, 0x18, 0x1c, 0x20...) does NOT match the binary.
// Raw bytes at 0x0041bc50 reveal the actual order is:
//   [0x1c, 0xbc], [0x18, 0xb8], [0x10, 0xb0], [0x14, 0xb4], [0x20, 0xc0],
//   [0x80, 0x120], [0x84, 0x124], ..., [0x94, 0x134]   <-- not in note
//   [0x68, 0x108], ...
// The note's table is both reordered and INCOMPLETE (missing 0x94/0x134).
// Faithful reimpl requires full re-decoding of all 603 bytes — out of
// scope for this batch session. Deferring to a follow-up.
//
// Refusal logged in PROMOTION_QUEUE.md note for this session.
// ===========================================================================

#if 0
// ===========================================================================
// 0x0041bc50  Sub0041bc50_HudRender29Dispatcher  (DISABLED — see refusal note)
//
// Original: FUN_0041bc50 (603 bytes, 0x0041bc50..0x0041beaa).
// __thiscall via EAX (object ptr in EAX). 29 guarded vtable+0x48 dispatches.
// Each pair: read int32 at EAX+guard_off; if !=0, dispatch vtable[0x48] on
// object pointer at EAX+render_off (render_off = guard_off + 0xA0).
//
// guard offsets: 0x10,0x14,0x18,0x1c,0x20,0x24,0x28,0x2c,0x30,0x34,0x38,0x3c,
//                0x40,0x44,0x48,0x4c,0x50,0x68,0x6c,0x70,0x74,0x78,0x7c,0x80,
//                0x84,0x88,0x8c,0x90 (28 pairs documented).
// Note: 0x0041bc50.md table lists 29 pairs but 28 (0x10..0x50 = 17 + 0x68..0x90 = 11 = 28
// after removing 0x54..0x64 gap). Note also has explicit 28 in table; the 29th
// in the prose is a count-off-by-one — we implement 28 pairs verbatim from
// the table.
// Verification: crash_equal_ok=True (EAX=0 -> identical null-deref).
// Caller: sub_0041c0c0 (C2). Callee: vtable indirection only -> leaf-like.
//
// We implement via __declspec(naked) + inline asm to preserve EAX-this
// calling convention exactly. Pattern adapted from FUN_0041c2d0 in
// HudDispatch.cpp. ESI is callee-saved and is used as the working "this"
// because the orig prologue establishes ESI=EAX (see analysis note).
// ===========================================================================

// 0x0041bc50
extern "C" __declspec(dllexport) __declspec(naked)
void __cdecl Sub0041bc50_HudRender29Dispatcher()
{
    __asm {
        push esi
        push edi
        mov  esi, eax                   // this = EAX (object base)

        // Macro-equivalent block for each (guard_off, render_off=guard_off+0xA0).
        // guard at [esi+G]; if != 0: edi = [esi+G+0xA0]; eax = [edi]; push edi; call [eax+0x48]
        // 28 dispatches follow.

        // pair 1: G=0x10  R=0xB0
        cmp  dword ptr [esi+0x10], 0
        je   skip01
        mov  edi, dword ptr [esi+0xb0]
        mov  eax, dword ptr [edi]
        push edi
        call dword ptr [eax+0x48]
skip01:
        // pair 2: G=0x14  R=0xB4
        cmp  dword ptr [esi+0x14], 0
        je   skip02
        mov  edi, dword ptr [esi+0xb4]
        mov  eax, dword ptr [edi]
        push edi
        call dword ptr [eax+0x48]
skip02:
        // pair 3: G=0x18  R=0xB8
        cmp  dword ptr [esi+0x18], 0
        je   skip03
        mov  edi, dword ptr [esi+0xb8]
        mov  eax, dword ptr [edi]
        push edi
        call dword ptr [eax+0x48]
skip03:
        // pair 4: G=0x1C  R=0xBC
        cmp  dword ptr [esi+0x1c], 0
        je   skip04
        mov  edi, dword ptr [esi+0xbc]
        mov  eax, dword ptr [edi]
        push edi
        call dword ptr [eax+0x48]
skip04:
        // pair 5: G=0x20  R=0xC0
        cmp  dword ptr [esi+0x20], 0
        je   skip05
        mov  edi, dword ptr [esi+0xc0]
        mov  eax, dword ptr [edi]
        push edi
        call dword ptr [eax+0x48]
skip05:
        // pair 6: G=0x24  R=0xC4
        cmp  dword ptr [esi+0x24], 0
        je   skip06
        mov  edi, dword ptr [esi+0xc4]
        mov  eax, dword ptr [edi]
        push edi
        call dword ptr [eax+0x48]
skip06:
        // pair 7: G=0x28  R=0xC8
        cmp  dword ptr [esi+0x28], 0
        je   skip07
        mov  edi, dword ptr [esi+0xc8]
        mov  eax, dword ptr [edi]
        push edi
        call dword ptr [eax+0x48]
skip07:
        // pair 8: G=0x2C  R=0xCC
        cmp  dword ptr [esi+0x2c], 0
        je   skip08
        mov  edi, dword ptr [esi+0xcc]
        mov  eax, dword ptr [edi]
        push edi
        call dword ptr [eax+0x48]
skip08:
        // pair 9: G=0x30  R=0xD0
        cmp  dword ptr [esi+0x30], 0
        je   skip09
        mov  edi, dword ptr [esi+0xd0]
        mov  eax, dword ptr [edi]
        push edi
        call dword ptr [eax+0x48]
skip09:
        // pair 10: G=0x34  R=0xD4
        cmp  dword ptr [esi+0x34], 0
        je   skip10
        mov  edi, dword ptr [esi+0xd4]
        mov  eax, dword ptr [edi]
        push edi
        call dword ptr [eax+0x48]
skip10:
        // pair 11: G=0x38  R=0xD8
        cmp  dword ptr [esi+0x38], 0
        je   skip11
        mov  edi, dword ptr [esi+0xd8]
        mov  eax, dword ptr [edi]
        push edi
        call dword ptr [eax+0x48]
skip11:
        // pair 12: G=0x3C  R=0xDC
        cmp  dword ptr [esi+0x3c], 0
        je   skip12
        mov  edi, dword ptr [esi+0xdc]
        mov  eax, dword ptr [edi]
        push edi
        call dword ptr [eax+0x48]
skip12:
        // pair 13: G=0x40  R=0xE0
        cmp  dword ptr [esi+0x40], 0
        je   skip13
        mov  edi, dword ptr [esi+0xe0]
        mov  eax, dword ptr [edi]
        push edi
        call dword ptr [eax+0x48]
skip13:
        // pair 14: G=0x44  R=0xE4
        cmp  dword ptr [esi+0x44], 0
        je   skip14
        mov  edi, dword ptr [esi+0xe4]
        mov  eax, dword ptr [edi]
        push edi
        call dword ptr [eax+0x48]
skip14:
        // pair 15: G=0x48  R=0xE8
        cmp  dword ptr [esi+0x48], 0
        je   skip15
        mov  edi, dword ptr [esi+0xe8]
        mov  eax, dword ptr [edi]
        push edi
        call dword ptr [eax+0x48]
skip15:
        // pair 16: G=0x4C  R=0xEC
        cmp  dword ptr [esi+0x4c], 0
        je   skip16
        mov  edi, dword ptr [esi+0xec]
        mov  eax, dword ptr [edi]
        push edi
        call dword ptr [eax+0x48]
skip16:
        // pair 17: G=0x50  R=0xF0
        cmp  dword ptr [esi+0x50], 0
        je   skip17
        mov  edi, dword ptr [esi+0xf0]
        mov  eax, dword ptr [edi]
        push edi
        call dword ptr [eax+0x48]
skip17:
        // Gap 0x54..0x64 absent (per analysis note).

        // pair 18: G=0x68  R=0x108
        cmp  dword ptr [esi+0x68], 0
        je   skip18
        mov  edi, dword ptr [esi+0x108]
        mov  eax, dword ptr [edi]
        push edi
        call dword ptr [eax+0x48]
skip18:
        // pair 19: G=0x6C  R=0x10C
        cmp  dword ptr [esi+0x6c], 0
        je   skip19
        mov  edi, dword ptr [esi+0x10c]
        mov  eax, dword ptr [edi]
        push edi
        call dword ptr [eax+0x48]
skip19:
        // pair 20: G=0x70  R=0x110
        cmp  dword ptr [esi+0x70], 0
        je   skip20
        mov  edi, dword ptr [esi+0x110]
        mov  eax, dword ptr [edi]
        push edi
        call dword ptr [eax+0x48]
skip20:
        // pair 21: G=0x74  R=0x114
        cmp  dword ptr [esi+0x74], 0
        je   skip21
        mov  edi, dword ptr [esi+0x114]
        mov  eax, dword ptr [edi]
        push edi
        call dword ptr [eax+0x48]
skip21:
        // pair 22: G=0x78  R=0x118
        cmp  dword ptr [esi+0x78], 0
        je   skip22
        mov  edi, dword ptr [esi+0x118]
        mov  eax, dword ptr [edi]
        push edi
        call dword ptr [eax+0x48]
skip22:
        // pair 23: G=0x7C  R=0x11C
        cmp  dword ptr [esi+0x7c], 0
        je   skip23
        mov  edi, dword ptr [esi+0x11c]
        mov  eax, dword ptr [edi]
        push edi
        call dword ptr [eax+0x48]
skip23:
        // pair 24: G=0x80  R=0x120
        cmp  dword ptr [esi+0x80], 0
        je   skip24
        mov  edi, dword ptr [esi+0x120]
        mov  eax, dword ptr [edi]
        push edi
        call dword ptr [eax+0x48]
skip24:
        // pair 25: G=0x84  R=0x124
        cmp  dword ptr [esi+0x84], 0
        je   skip25
        mov  edi, dword ptr [esi+0x124]
        mov  eax, dword ptr [edi]
        push edi
        call dword ptr [eax+0x48]
skip25:
        // pair 26: G=0x88  R=0x128
        cmp  dword ptr [esi+0x88], 0
        je   skip26
        mov  edi, dword ptr [esi+0x128]
        mov  eax, dword ptr [edi]
        push edi
        call dword ptr [eax+0x48]
skip26:
        // pair 27: G=0x8C  R=0x12C
        cmp  dword ptr [esi+0x8c], 0
        je   skip27
        mov  edi, dword ptr [esi+0x12c]
        mov  eax, dword ptr [edi]
        push edi
        call dword ptr [eax+0x48]
skip27:
        // pair 28: G=0x90  R=0x130
        cmp  dword ptr [esi+0x90], 0
        je   skip28
        mov  edi, dword ptr [esi+0x130]
        mov  eax, dword ptr [edi]
        push edi
        call dword ptr [eax+0x48]
skip28:
        pop  edi
        pop  esi
        ret
    }
}

// MASS-DISABLED 2026-05-24 phase-a2-no-registry-deferred: RH_ScopedInstall(Sub0041bc50_HudRender29Dispatcher, 0x0041bc50);
#endif


// ===========================================================================
// 0x00427780  FontText_StringTableLookup
//
// Original: FUN_00427780 (0x10 bytes). Pure leaf, no branches.
// undefined* __cdecl FUN_00427780(int param_1)
// return &DAT_0066d828 + *(int*)(&DAT_0066d828 + param_1 * 4);
// Packed-string-table directory lookup.
// ===========================================================================

// 0x00427780
extern "C" __declspec(dllexport)
const std::uint8_t* __cdecl FontText_StringTableLookup(int index) {
    std::uint8_t* base = reinterpret_cast<std::uint8_t*>(0x0066d828u);
    // First N*4 bytes are an int-offset directory; lookup returns
    // base + *(int*)(base + index*4).
    std::int32_t off = *reinterpret_cast<const std::int32_t*>(base + index * 4);
    return base + off;
}

RH_ScopedInstall(FontText_StringTableLookup, 0x00427780);  // re-enabled 2026-05-24 batch-mixed


// ===========================================================================
// 0x00427840  FontText_UTF16WidenCopy
//
// Original: FUN_00427840 (0x35 bytes).
// __fastcall: EAX = byte* src, ECX = ushort* dst.
// Reads byte-length via vtable at *(DAT_007d3ff8 + 0xf4); widens each src byte
// to ushort (zero-extend); null-terminates dst.
//
// We implement as __declspec(naked) to preserve the non-standard fastcall
// (EAX=src, ECX=dst). U-1069 is in the analysis note but is about the source
// object identity, not the function body itself.
// ===========================================================================

// File-static pointer-global for forced memref (see 0x004c57a0 notes).
static volatile std::uint32_t* const s_VtableRootAddr_00427840 =
    reinterpret_cast<volatile std::uint32_t*>(0x007d3ff8u);

// 0x00427840
extern "C" __declspec(dllexport) __declspec(naked)
void __cdecl FontText_UTF16WidenCopy()
{
    __asm {
        // EAX = src (byte*), ECX = dst (ushort*)
        push esi
        push edi
        push ebx
        mov  esi, eax                       // src
        mov  edi, ecx                       // dst

        // iVar1 = (**(*0x7d3ff8 + 0xf4))()
        mov  eax, dword ptr [s_VtableRootAddr_00427840]
        mov  eax, dword ptr [eax]           // EAX = *(0x7d3ff8)
        call dword ptr [eax+0xf4]
        mov  ebx, eax                       // len = vtable call return

        // If iVar1 <= 0 -> skip copy and null-terminate
        test ebx, ebx
        jle  null_term

copy_loop:
        movzx eax, byte ptr [esi]           // load src byte zero-extended
        mov  word ptr [edi], ax             // store as ushort
        inc  esi
        add  edi, 2
        dec  ebx
        jnz  copy_loop

null_term:
        mov  word ptr [edi], 0              // null terminate
        pop  ebx
        pop  edi
        pop  esi
        ret
    }
}

RH_ScopedInstall(FontText_UTF16WidenCopy, 0x00427840);  // re-enabled 2026-05-24 batch-mixed


// ===========================================================================
// 0x004c57a0  REFUSED (post-diff verification)
//
// After fixing the MSVC inline-asm absolute-memref bug, the reimpl now
// successfully allocates an object and writes the identity matrix.
// However, the alloc returns a NEW pointer each call — orig returns 0x4868678
// while reimpl returns 0x48686b8 (consecutive). The diff harness compares
// the return values bit-for-bit, which fails for any allocator-backed fn
// whose pointer is non-deterministic across the orig/reimpl pair.
//
// Comparing the matrix CONTENTS at the returned addr would prove
// bit-identity — but no existing arg_type does this. We refuse rather
// than add a new harness arg_type (per session 2 precedent).
//
// Refusal logged in PROMOTION_QUEUE.md note for this session.
// ===========================================================================

#if 0
// ===========================================================================
// 0x004c57a0  FontCtxMatrix_AllocInit  (DISABLED — see refusal note)
//
// Original: FUN_004c57a0 (0x52 bytes).
// Allocates an object via vtable[+0x118] on *(DAT_007d3ff8 + DAT_007d4028),
// passing flag 0x3000d. Initializes the returned object as a 3x3 identity
// matrix at indices [0..0xe] with flag dword at [3]=0x20003 and translation
// (indices [0xc..0xe]) = 0. Returns the allocated object pointer via implicit
// EAX (U-1493; Ghidra shows void return type but caller assigns return value).
//
// We implement as __declspec(naked) to preserve the implicit-EAX return.
// ===========================================================================

// 0x004c57a0
// Disassembly from the binary at 0x004c57a0 (bytes verified):
//   MOV  EAX, [0x7d3ff8]            ; vtable carrier object (load from memory)
//   MOV  ECX, [0x7d4028]            ; offset into adjacent-data region
//   PUSH 0x0003000d
//   MOV  EDX, [ECX + EAX]
//   PUSH EDX
//   CALL [EAX + 0x118]               ; vtable+0x118 on carrier directly
//   XOR  ECX, ECX
//   ADD  ESP, 8
//   CMP  EAX, ECX                    ; if null, skip init
//   JE   alloc_failed
//   ... write identity matrix
//
// MSVC inline-asm note: `mov eax, dword ptr [0x12345678]` is incorrectly
// emitted as `MOV EAX, imm32` (b8 78 56 34 12) — the brackets in
// inline-asm don't trigger memory addressing for a bare numeric literal.
// We work around by loading the address into a register first via a
// dummy pointer global, then dereferencing. Equivalent effect; bytes
// will differ but semantics match.
//
// We use C globals (cast to volatile pointers) and a __declspec(naked)
// asm body so the layout of stores remains controllable.
// File-static pointer-globals used to coerce MSVC inline asm into emitting
// real memory loads (bare [imm32] is interpreted as immediate, not memref).
static volatile std::uint32_t* const s_VtableRootAddr_004c57a0 =
    reinterpret_cast<volatile std::uint32_t*>(0x007d3ff8u);
static volatile std::uint32_t* const s_OffsetAddr_004c57a0    =
    reinterpret_cast<volatile std::uint32_t*>(0x007d4028u);

extern "C" __declspec(dllexport) __declspec(naked)
void __cdecl FontCtxMatrix_AllocInit()
{
    __asm {
        // Load EAX = *(0x7d3ff8) via the file-static ptr.
        mov  eax, dword ptr [s_VtableRootAddr_004c57a0]   // EAX = ptr-value
        mov  eax, dword ptr [eax]                          // EAX = *(0x7d3ff8)
        mov  ecx, dword ptr [s_OffsetAddr_004c57a0]
        mov  ecx, dword ptr [ecx]                          // ECX = *(0x7d4028)
        push 0x3000d                              // alloc flag
        mov  edx, dword ptr [ecx+eax]            // EDX = *(EAX+ECX) data ptr
        push edx
        call dword ptr [eax+0x118]               // vtable+0x118 on EAX
        xor  ecx, ecx
        add  esp, 8
        cmp  eax, ecx
        je   alloc_failed

        // Initialize identity matrix. EDX=1.0f, ECX=0.
        // Match original instruction order:
        mov  edx, 0x3f800000
        mov  dword ptr [eax+0x10], ecx
        mov  dword ptr [eax+0x28], edx
        mov  dword ptr [eax+0x14], edx
        mov  dword ptr [eax+0x00], edx
        mov  dword ptr [eax+0x08], ecx
        mov  dword ptr [eax+0x04], ecx
        mov  dword ptr [eax+0x24], ecx
        mov  dword ptr [eax+0x20], ecx
        mov  dword ptr [eax+0x18], ecx
        mov  dword ptr [eax+0x38], ecx
        mov  dword ptr [eax+0x34], ecx
        mov  dword ptr [eax+0x30], ecx
        mov  dword ptr [eax+0x0c], 0x00020003
alloc_failed:
        ret
    }
}

// MASS-DISABLED 2026-05-24 phase-a2-no-registry-deferred: RH_ScopedInstall(FontCtxMatrix_AllocInit, 0x004c57a0);
#endif
