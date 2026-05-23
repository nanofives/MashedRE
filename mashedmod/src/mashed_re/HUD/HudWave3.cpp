// Mashed RE — HUD subsystem Wave 3 mixed-candidate cluster.
// Session: wave3-s4   Branch: c3-sweep/wave3-s4
//
// Two HUD functions authored to C2 quality:
//   0x00427620  FontText_HudShutdown  — 7-step font/style teardown sequence
//   0x0041b340  HudSlotFlagsDispatch  — EAX-thiscall, flag-driven vtable dispatcher
//
// Analysis notes:
//   re/analysis/hud_promote_c2_b/0x00427620.md
//   re/analysis/hud_ingame_d2/0x0041b340.md
//
// Binary anchor:
//   original\MASHED.exe  size 2,846,720
//   SHA-256 BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E

#include "../Core/HookSystem.h"
#include <cstdint>

// ---------------------------------------------------------------------------
// Callee typedefs (addresses Ghidra-cited in analysis notes)
// ---------------------------------------------------------------------------

// 0x00555830  — destroy font context object (takes ctx ptr; void return)
typedef void (__cdecl* FnFontCtxDestroy)(void* ctx);
// 0x00556e40  — destroy text style object (takes style ptr; void return)
typedef void (__cdecl* FnStyleDestroy)(void* style);
// 0x00556cd0  — get active font / current-font getter (no args; returns ptr or NULL)
typedef void* (__cdecl* FnGetCurrentFont)();
// 0x004c5930  — free/release object (takes ptr; void)
typedef void (__cdecl* FnRelease)(void* p);
// 0x00552b90  — font subsystem low-level shutdown (no args; void)
typedef void (__cdecl* FnFontSysShutdown)();

static inline void CallFontCtxDestroy(void* ctx) {
    reinterpret_cast<FnFontCtxDestroy>(0x00555830u)(ctx);
}
static inline void CallStyleDestroy(void* style) {
    reinterpret_cast<FnStyleDestroy>(0x00556e40u)(style);
}
static inline void* CallGetCurrentFont() {
    return reinterpret_cast<FnGetCurrentFont>(0x00556cd0u)();
}
static inline void CallRelease(void* p) {
    reinterpret_cast<FnRelease>(0x004c5930u)(p);
}
static inline void CallFontSysShutdown() {
    reinterpret_cast<FnFontSysShutdown>(0x00552b90u)();
}

// ---------------------------------------------------------------------------
// Global addresses (Ghidra-cited in hud_promote_c2_b/0x00427620.md)
// ---------------------------------------------------------------------------

// DAT_0067d838 — primary font context ptr
static std::uint32_t* const g_FontCtxPtr =
    reinterpret_cast<std::uint32_t*>(0x0067d838u);
// DAT_0067d844 — style object 0 ptr
static std::uint32_t* const g_Style0 =
    reinterpret_cast<std::uint32_t*>(0x0067d844u);
// DAT_0067d848 — style object 1 ptr
static std::uint32_t* const g_Style1 =
    reinterpret_cast<std::uint32_t*>(0x0067d848u);
// DAT_0067d83c — style object 2 ptr
static std::uint32_t* const g_Style2 =
    reinterpret_cast<std::uint32_t*>(0x0067d83cu);
// DAT_0067d840 — style object 3 ptr
static std::uint32_t* const g_Style3 =
    reinterpret_cast<std::uint32_t*>(0x0067d840u);

// ===========================================================================
// 0x00427620  FontText_HudShutdown
//
// Teardown sequence for the HUD text subsystem.
// 7 sequential operations (body 0x00427620–0x0042767d, ~90 bytes):
//
//   1. FUN_00555830(DAT_0067d838)  — destroys primary font context
//   2. DAT_0067d838 = 0            — zeroes the font-context global
//   3. FUN_00556e40(DAT_0067d844)  — destroys style 0
//   4. FUN_00556e40(DAT_0067d848)  — destroys style 1
//   5. FUN_00556e40(DAT_0067d83c)  — destroys style 2
//   6. FUN_00556e40(DAT_0067d840)  — destroys style 3
//   7. result = FUN_00556cd0()     — if result != NULL: FUN_004c5930(result)
//   8. FUN_00552b90()              — font system low-level shutdown
//
// No parameters. No return value.
// Callers: FUN_00402a40 (subsystem shutdown dispatcher).
//
// [UNCERTAIN] markers: none in the analysis note.
// ===========================================================================

// 0x00427620
extern "C" __declspec(dllexport) void __cdecl FontText_HudShutdown() {
    // Step 1: destroy primary font context
    CallFontCtxDestroy(reinterpret_cast<void*>(*g_FontCtxPtr));
    // Step 2: zero the context pointer
    *g_FontCtxPtr = 0u;
    // Steps 3–6: destroy style objects 0..3
    CallStyleDestroy(reinterpret_cast<void*>(*g_Style0));
    CallStyleDestroy(reinterpret_cast<void*>(*g_Style1));
    CallStyleDestroy(reinterpret_cast<void*>(*g_Style2));
    CallStyleDestroy(reinterpret_cast<void*>(*g_Style3));
    // Step 7: get active font; if non-null, free it
    void* cur = CallGetCurrentFont();
    if (cur != nullptr) {
        CallRelease(cur);
    }
    // Step 8: font system shutdown
    CallFontSysShutdown();
}

RH_ScopedInstall(FontText_HudShutdown, 0x00427620);

// ===========================================================================
// 0x0041b340  HudSlotFlagsDispatch
//
// Per-HUD-slot vtable dispatcher. __thiscall-via-EAX: object pointer passed
// in EAX, not on the stack. Dispatches vtable[0x48] on member object pointers
// stored in EAX-indexed int arrays, gated by a 32-bit flags dword at EAX[0x1a]
// (byte offset 0x68).
//
// Body 0x0041b340..0x0041b435, 245 bytes.
// Callers: HudSlotLoopB630 (0x0041b630, C3) — passes EAX = per-slot struct ptr.
//
// Flags word layout (from 0x0041b340.md):
//   Bits 0x0800 / 0x1000 select car-type variant:
//     Bit 0x0800 set → dispatch slots 5, 4, optionally 6 (if bit 0x40 set)
//     Bit 0x1000 set → dispatch slots 2, 1, optionally 3 (if bit 0x40 set)
//     Neither set    → skip car-type block
//   Bit 0x0001 → slot  7    Bit 0x0002 → slot  8
//   Bit 0x0004 → slot  9    Bit 0x0008 → slot 10
//   Bit 0x0010 → slot 11    Bit 0x0020 → slot 12
//   Bit 0x2000 → slot 13    Bit 0x4000 → slot 14
//   Bit 0x8000 (sign of byte at flags>>8) → slot 15
//   Slots 0 and 16 are always dispatched (unconditional).
//
// Each slot dispatch: obj = *(int*)(this + slot*4);  (*vtable[0x48])(obj);
//   i.e.  (**(void(**)(void*))((*(int*)obj) + 0x48))(obj);
//
// Implemented via __declspec(naked) to preserve the EAX-this convention
// exactly (matching the binary's call sites which do MOV EAX,<entry>; CALL).
// A thunk helper receives EAX in a local variable and does the dispatches.
//
// [UNCERTAIN] markers: none in the C2 analysis note (all offsets Ghidra-cited).
// ===========================================================================

// Helper that does the actual dispatch work given the explicit 'this' pointer.
// Declared static so it gets no dllexport name or RH_ScopedInstall.
static void HudSlotFlagsDispatch_Body(int* obj_this)
{
    // Convenience macro: dispatch vtable[0x48] on slot N
    // Each slot is stored as an int* at obj_this[N] = *(obj_this + N)
    // (int array, so slot N is at byte offset N*4)
#define DISPATCH(slot) \
    do { \
        int s = obj_this[(slot)]; \
        (*reinterpret_cast<void (**)(int)>(s + 0x48))(s); \
    } while (0)

    // Unconditional: slots 16 and 0
    DISPATCH(0x10);  // slot 16
    DISPATCH(0);     // slot 0

    // Flags at EAX[0x1a] (byte offset 0x68 = 0x1a * 4)
    const std::uint32_t flags = static_cast<std::uint32_t>(obj_this[0x1a]);

    // Car-type selection
    if (flags & 0x0800u) {
        // Bit 0x0800: car-type A → slots 5, 4, optionally 6 (if bit 0x40)
        DISPATCH(5);
        DISPATCH(4);
        if (flags & 0x40u) {
            DISPATCH(6);
        }
    } else if (flags & 0x1000u) {
        // Bit 0x1000: car-type B → slots 2, 1, optionally 3 (if bit 0x40)
        DISPATCH(2);
        DISPATCH(1);
        if (flags & 0x40u) {
            DISPATCH(3);
        }
    }
    // (else: neither bit set → skip car-type block entirely)

    // Low-byte element flags
    if (flags & 0x01u)   DISPATCH(7);
    if (flags & 0x02u)   DISPATCH(8);
    if (flags & 0x04u)   DISPATCH(9);
    if (flags & 0x08u)   DISPATCH(10);
    if (flags & 0x10u)   DISPATCH(11);
    if (flags & 0x20u)   DISPATCH(12);
    if (flags & 0x2000u) DISPATCH(13);
    if (flags & 0x4000u) DISPATCH(14);
    // Bit 0x8000 check: sign of the byte at (flags >> 8), i.e. high byte of low word
    if (static_cast<std::int8_t>((flags >> 8) & 0xFFu) < 0) DISPATCH(15);

#undef DISPATCH
}

// 0x0041b340 — naked trampoline: capture EAX as 'this', call body, RET.
// MSVC x86 __declspec(naked): no prologue/epilogue generated.
extern "C" __declspec(dllexport) __declspec(naked)
void __cdecl HudSlotFlagsDispatch()
{
    __asm {
        push  esi
        push  edi
        push  ebx
        sub   esp, 4          // align + local slot for 'this'
        mov   [esp], eax      // save the EAX-this into the local
        push  eax             // arg: obj_this
        call  HudSlotFlagsDispatch_Body
        add   esp, 8          // pop arg + local
        pop   ebx
        pop   edi
        pop   esi
        ret
    }
}

RH_ScopedInstall(HudSlotFlagsDispatch, 0x0041b340);
