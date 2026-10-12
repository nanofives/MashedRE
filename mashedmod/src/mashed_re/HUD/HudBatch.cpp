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
//   0x0041bc50  FUN_0041bc50                       EAX-in record; 29 flag-gated [o+0x48](o) field calls
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
// Body 0x0041dab0 (ESI = EAX = 0x0063d55c), re-ported from the bytes 2026-10-12.
// Every compare is `fld [esi+0x2c]; fcomp [thr]; fnstsw ax; test ah,5; jp/jnp`.
// test ah,5 has odd parity only when C0 alone is set, i.e. c < thr (ordered);
// an unordered compare sets C0 and C2 and takes the jp. So "jp skips" <=> !(c < thr),
// which the C `<` on floats reproduces (false for NaN).
// The counter [esi+0x2c] (= 0x0063d588) and every object slot are re-read at
// each use: the dispatched functions can write them.
//
//   0x0041dab3..0x0041dac1  c < [0x005cc35c]: call obj[+0x00], obj[+0x14]
//   0x0041dad3..0x0041dae1  c < [0x005cd238]: return (jnp 0x0041db78)
//   0x0041dae7..0x0041daf5  c < [0x005cd234]: call obj[+0x04]; goto tail
//   0x0041db03..0x0041db11  c < [0x005cd230]: call obj[+0x04], obj[+0x08]; goto tail
//   0x0041db26..0x0041db34  c < [0x005ccdf4]: call obj[+0x04], obj[+0x08], obj[+0x0c]
//   tail 0x0041db4e..0x0041db5c  fcomp [0x005ccdf4]; test ah,1; jne return
//                                (C0 set: c < thr OR unordered)
//   0x0041db5e..0x0041db6c  c < [0x005cc35c]: call obj[+0x10]
//
// Each call is `push obj; call [obj+0x48]` with the caller cleaning the stack
// (add esp at 0x0041dad0/0x0041dafe/0x0041db21/0x0041db4b/0x0041db75). The
// function pointer lives at obj+0x48 itself (no vtable hop).
//
// Old port (pre 2026-10-12): ran the second block when c < [0x005cd238]
// (inverted; the bytes run it when !(c < [0x005cd238])) and cached the counter once.
extern "C" __declspec(dllexport)
void __cdecl Sub0041db80_HudThresholdDispatch()
{
    const std::uintptr_t base = 0x0063d55cu;   // 0x0041db80 mov eax,0x63d55c
    auto c = [base]() -> float {
        return *reinterpret_cast<volatile const float*>(base + 0x2cu);
    };
    auto thr = [](std::uintptr_t a) -> float {
        return *reinterpret_cast<volatile const float*>(a);
    };

    typedef void (__cdecl* DispatchFn)(std::uint32_t);
    auto dispatch_at = [base](std::uintptr_t off) {
        const std::uint32_t obj = *reinterpret_cast<volatile const std::uint32_t*>(base + off);
        const DispatchFn fn = *reinterpret_cast<const DispatchFn*>(
            static_cast<std::uintptr_t>(obj) + 0x48u);
        fn(obj);
    };

    // 0x0041dab3: fcomp [0x005cc35c]; jp 0x0041dad3
    if (c() < thr(0x005cc35cu)) {
        dispatch_at(0x00u);   // 0x0041dac3
        dispatch_at(0x14u);   // 0x0041dac9
    }
    // 0x0041dad3: fcomp [0x005cd238]; jnp 0x0041db78 (return)
    if (c() < thr(0x005cd238u))
        return;
    // 0x0041dae7: fcomp [0x005cd234]; jp 0x0041db03
    if (c() < thr(0x005cd234u)) {
        dispatch_at(0x04u);   // 0x0041daf7
    } else if (c() < thr(0x005cd230u)) {   // 0x0041db03; jp 0x0041db26
        dispatch_at(0x04u);   // 0x0041db13
        dispatch_at(0x08u);   // 0x0041db1a
    } else if (c() < thr(0x005ccdf4u)) {   // 0x0041db26; jp 0x0041db4e
        dispatch_at(0x04u);   // 0x0041db36
        dispatch_at(0x08u);   // 0x0041db3d
        dispatch_at(0x0cu);   // 0x0041db44
    }
    // 0x0041db4e: fcomp [0x005ccdf4]; test ah,1; jne return (C0: c < thr or unordered)
    if (!(c() >= thr(0x005ccdf4u)))
        return;
    // 0x0041db5e: fcomp [0x005cc35c]; jp return
    if (c() < thr(0x005cc35cu))
        dispatch_at(0x10u);   // 0x0041db6e
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
// Returns the EAX of the final FUN_004c1a00 call (0x0040322d): the epilogue
// (0x00403232 cookie xor, jmp 0x004a2be9) does not touch EAX.
extern "C" __declspec(dllexport) int __cdecl Sub00403160_SubMode0BViewport() {
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
        // 0x004031b6: mov eax,[0x007d3ff8]; call [eax+0x20]. ONE dereference: the
        // function pointer is the dword at (*0x007d3ff8)+0x20, re-read before each
        // call (0x004031c2 / 0x004031ff / 0x0040320c reload the global).
        // Old port (pre 2026-10-12) read *(*(*0x007d3ff8)+0x20), one hop too many.
        auto rs_set = [](int state_id, int value) {
            const std::uintptr_t rs_obj = *reinterpret_cast<volatile const std::uint32_t*>(0x007d3ff8u);
            reinterpret_cast<RenderStateSetFn>(
                *reinterpret_cast<const std::uint32_t*>(rs_obj + 0x20u))(state_id, value);
        };

        rs_set(6, 0);   // 0x004031bb push 0; push 6
        rs_set(8, 0);   // 0x004031c8 push 0; push 8
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
    reinterpret_cast<SetViewportFn>(0x004c1c80)(cam, restore_vp);   // 0x00403227
    return reinterpret_cast<CamBeginFn>(0x004c1a00)(cam);           // 0x0040322d
}

RH_ScopedInstall(Sub00403160_SubMode0BViewport, 0x00403160);  // re-enabled 2026-05-24 batch-mixed


// ===========================================================================
// 0x0041bc50  FUN_0041bc50   (603 bytes, 0x0041bc50..0x0041beaa)
//
// Re-ported from the bytes of original\MASHED.exe.unpatched on 2026-10-12
// (U-9675). The record pointer arrives in EAX:
//   0x0041bc50 push esi
//   0x0041bc51 mov  esi,eax                ; esi = rec
// then 29 pairs, each of this shape (first pair shown):
//   0x0041bc53 mov  eax,[esi+0x1c]         ; flag dword at rec+X
//   0x0041bc56 test eax,eax
//   0x0041bc58 je   0x0041bc67             ; skip when the dword is 0
//   0x0041bc5a mov  eax,[esi+0xbc]         ; o = [rec+X+0xa0]
//   0x0041bc60 push eax
//   0x0041bc61 call [eax+0x48]             ; o->[+0x48](o)
//   0x0041bc64 add  esp,4                  ; caller cleans the one arg
// and the last pair loads o into ESI instead of EAX:
//   0x0041be9c mov  esi,[esi+0xd8] / 0x0041bea2 push esi /
//   0x0041bea3 call [esi+0x48] / 0x0041bea6 add esp,4
//   0x0041bea9 pop  esi
//   0x0041beaa ret                         ; EAX = last pair's EAX
// [o+0x48] is a function-pointer FIELD of the object, called with one
// dereference. It is not a vtable slot: there is no `mov reg,[o]` before the
// call. The flag is re-read after every call (each pair loads it fresh).
// EAX at RET is the 0x38 flag when it is 0 (0x0041be98 je), else the
// 0x0041bea3 call's EAX. ESI preserved. No other register is written.
//
// The 29 flag offsets in byte order (object at X+0xa0 in every pair):
//   0x1c 0x18 0x10 0x14 0x20, 0x80 0x84 0x88 0x8c 0x90 0x94,
//   0x68 0x6c 0x70 0x74 0x78 0x7c, 0x3c 0x40 0x44 0x48 0x4c 0x50,
//   0x24 0x28 0x2c 0x30 0x34 0x38.
// 0x54..0x64 are not read.
//
// The previous port here (MASS-DISABLED 2026-05-24, never installed) did not
// match the bytes (U-9675): it read a vtable (`mov eax,[edi]; call
// [eax+0x48]`), never popped the pushed argument, walked the flags in
// ascending order from 0x10, and had 28 pairs (0x94 missing).
//
// Caller: 0x0041c0c0 HudDispatchSlot2 (0x0041c0c8 call, EAX = record
// 0x0063cab8 / 0x0063cc24). The port is a naked EAX shim around a cdecl body,
// the same shape as FUN_0041c2d0 in HudDispatch.cpp (U-9671). ECX/EDX at
// return are not part of the contract: 0x0041c0c0 reads neither after
// 0x0041c0c8 (0x0041c0cd add esi,0x16c; 0x0041c0d3 cmp; 0x0041c0db pop esi;
// ret), per HudDispatch.cpp's 0x0041c0c0 block.
// ===========================================================================

typedef std::uint32_t (__cdecl *HudObjFieldFn)(void* obj);

static const std::uint8_t s_bc50FlagOffsets[29] = {
    0x1c,   // 0x0041bc53 (object 0xbc,  call 0x0041bc61)
    0x18,   // 0x0041bc67 (object 0xb8,  call 0x0041bc75)
    0x10,   // 0x0041bc7b (object 0xb0,  call 0x0041bc89)
    0x14,   // 0x0041bc8f (object 0xb4,  call 0x0041bc9d)
    0x20,   // 0x0041bca3 (object 0xc0,  call 0x0041bcb1)
    0x80,   // 0x0041bcb7 (object 0x120, call 0x0041bcc8)
    0x84,   // 0x0041bcce (object 0x124, call 0x0041bcdf)
    0x88,   // 0x0041bce5 (object 0x128, call 0x0041bcf6)
    0x8c,   // 0x0041bcfc (object 0x12c, call 0x0041bd0d)
    0x90,   // 0x0041bd13 (object 0x130, call 0x0041bd24)
    0x94,   // 0x0041bd2a (object 0x134, call 0x0041bd3b)
    0x68,   // 0x0041bd41 (object 0x108, call 0x0041bd4f)
    0x6c,   // 0x0041bd55 (object 0x10c, call 0x0041bd63)
    0x70,   // 0x0041bd69 (object 0x110, call 0x0041bd77)
    0x74,   // 0x0041bd7d (object 0x114, call 0x0041bd8b)
    0x78,   // 0x0041bd91 (object 0x118, call 0x0041bd9f)
    0x7c,   // 0x0041bda5 (object 0x11c, call 0x0041bdb3)
    0x3c,   // 0x0041bdb9 (object 0xdc,  call 0x0041bdc7)
    0x40,   // 0x0041bdcd (object 0xe0,  call 0x0041bddb)
    0x44,   // 0x0041bde1 (object 0xe4,  call 0x0041bdef)
    0x48,   // 0x0041bdf5 (object 0xe8,  call 0x0041be03)
    0x4c,   // 0x0041be09 (object 0xec,  call 0x0041be17)
    0x50,   // 0x0041be1d (object 0xf0,  call 0x0041be2b)
    0x24,   // 0x0041be31 (object 0xc4,  call 0x0041be3f)
    0x28,   // 0x0041be45 (object 0xc8,  call 0x0041be53)
    0x2c,   // 0x0041be59 (object 0xcc,  call 0x0041be67)
    0x30,   // 0x0041be6d (object 0xd0,  call 0x0041be7b)
    0x34,   // 0x0041be81 (object 0xd4,  call 0x0041be8f)
    0x38,   // 0x0041be95 (object 0xd8,  call 0x0041bea3)
};

static std::uint32_t __cdecl HudRec29DispatchBody(std::uint8_t* rec) {
    std::uint32_t r = 0;
    for (int i = 0; i < 29; ++i) {
        const std::uint32_t x = s_bc50FlagOffsets[i];
        r = *reinterpret_cast<volatile const std::uint32_t*>(rec + x);          // mov eax,[esi+X]
        if (r != 0) {                                                            // test eax,eax / je
            void* o = *reinterpret_cast<void* const*>(rec + x + 0xa0);           // mov eax,[esi+X+0xa0]
            r = (*reinterpret_cast<HudObjFieldFn*>(static_cast<std::uint8_t*>(o) + 0x48))(o);  // push; call [eax+0x48]
        }
    }
    return r;
}

// 0x0041bc50
extern "C" __declspec(dllexport) __declspec(naked)
std::uint32_t __cdecl FUN_0041bc50()
{
    __asm {
        push eax                        // rec (arrives in EAX, 0x0041bc51)
        call HudRec29DispatchBody
        add  esp, 4
        ret                             // EAX = last pair's EAX
    }
}

RH_ScopedInstall(FUN_0041bc50, 0x0041bc50);  // 2026-10-12 re-ported from bytes (U-9675)


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
// Non-standard fastcall: EAX = byte* src, ECX = ushort* dst.
// Gets the byte length by calling the function pointer at obj+0xf4 (where
// obj = *(0x007d3ff8)) WITH src as its argument: len = (*(obj+0xf4))(src).
// Then widens each src byte to ushort (zero-extend) and null-terminates dst.
//
// We implement as __declspec(naked) to preserve EAX=src, ECX=dst.
//
// 2026-05-27 FIX: the prior reimpl OMITTED the `push src` argument before the
// `call [obj+0xf4]`, so the length-getter measured a stale stack slot (NULL) ->
// strlen(NULL) crash on the title screen (~94s). The two `mov eax`/`mov eax,[eax]`
// loads are CORRECT and required: s_VtableRootAddr holds the constant 0x007d3ff8,
// so the first mov loads that address and the second derefs it to obj. Fix =
// push src + add esp,4. See re/analysis/menu_crash_scoping/REPORT.md.
// ===========================================================================

// File-static pointer-global for forced memref (see 0x004c57a0 notes).
static volatile std::uint32_t* const s_VtableRootAddr_00427840 =
    reinterpret_cast<volatile std::uint32_t*>(0x007d3ff8u);

// 0x00427840
extern "C" __declspec(dllexport) __declspec(naked)
void __cdecl FontText_UTF16WidenCopy()
{
    __asm {
        // EAX = src (byte*), ECX = dst (ushort*). Mirrors FUN_00427840:
        // EDI=src, ESI=dst, EAX=len.
        push esi
        push edi
        mov  edi, eax                       // EDI = src        (orig 0x427842)
        mov  eax, dword ptr [s_VtableRootAddr_00427840]  // EAX = 0x007d3ff8 (const addr held in the C var)
        mov  eax, dword ptr [eax]           // EAX = *(0x007d3ff8) = obj
        push edi                            // arg = src        (orig 0x427849: PUSH EDI)
        mov  esi, ecx                       // ESI = dst        (orig 0x42784a)
        call dword ptr [eax+0xf4]           // len = (*(obj+0xf4))(src)  (orig 0x42784c)
        add  esp, 4                         // clean pushed arg (orig 0x427852)

        test eax, eax                       // if len <= 0 -> just null-terminate
        jle  null_term

copy_loop:
        movzx ecx, byte ptr [edi]           // load src byte zero-extended (orig MOVZX CX,[EDI])
        mov  word ptr [esi], cx             // store as ushort              (orig MOV [ESI],CX)
        add  esi, 2
        inc  edi
        dec  eax
        jnz  copy_loop

null_term:
        pop  edi
        mov  word ptr [esi], 0              // null terminate dst
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
