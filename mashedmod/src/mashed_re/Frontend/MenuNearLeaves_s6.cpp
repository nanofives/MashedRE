// Mashed RE — Frontend near-leaf reimplementations (c3-batch-s session 6).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Functions in this file:
//   0x00426cb0  SlotIndexToPtr       — 12B pure leaf; returns &DAT_00663664 + param_1*0x4c
//   0x00426d90  HandleArrayRelease   — void(void) bulk-release; callee 0x004e6680 C1 (DEFERRED)
//   0x0042a640  PathBuilderLoad      — char*(char*) vtable+file dispatch; no suitable arg_type (DEFERRED)
//   0x0042b920  ConstantGetter22     — 5B constant getter; returns 0x16 unconditionally
//   0x0042bde0  HudRectEmitter       — void(int,byte) 5-call HUD rect emitter via ChromeBaseDraw
//
// Analysis notes:
//   re/analysis/frontend_c1_to_c2_s4/FUN_00426cb0.md
//   re/analysis/frontend_c1_to_c2_s5/FUN_00426d90.md
//   re/analysis/frontend_c1_to_c2_s6/FUN_0042a640.md
//   re/analysis/frontend_c1_to_c2_s6/FUN_0042b920.md
//   re/analysis/frontend_c1_to_c2_s6/FUN_0042bde0.md

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// Forward declarations for callees used in this file.
// ---------------------------------------------------------------------------

// 0x00472c60  ChromeBaseDraw  — C3 (DrawQuadPrimitives.cpp)
// void __cdecl ChromeBaseDraw(float x, float y, float w, float h, uint32_t colour)
// RwIm2D filled-quad draw (float coords, ARGB colour).
static auto* const s_ChromeBaseDraw =
    reinterpret_cast<void(__cdecl*)(float, float, float, float, std::uint32_t)>(0x00472c60);

// ---------------------------------------------------------------------------
// SlotIndexToPtr  --  0x00426cb0
//
// Original: FUN_00426cb0 (12 bytes, 0x00426cb0..0x00426cbc)
// Signature: undefined* FUN_00426cb0(int param_1)
//   param_1: slot index
// Returns: pointer to slot N = &DAT_00663664 + param_1 * 0x4c
//
// Body (pure leaf, no branches):
//   return &DAT_00663664 + param_1 * 0x4c;  [0x00426cb0]
//
// Constants (cited from 0x00426cb0 body):
//   0x00663664 — base address of the 0x4c-stride slot table   [0x00426cb0]
//   0x4c (76)  — stride per slot (bytes)                      [0x00426cb0]
//
// Callees: none (pure leaf).
//
// ref: re/analysis/frontend_c1_to_c2_s4/FUN_00426cb0.md
// ---------------------------------------------------------------------------

// 0x00426cb0
extern "C" __declspec(dllexport) std::uint8_t* __cdecl SlotIndexToPtr(int param_1)
{
    // Map slot index to pointer: base 0x00663664, stride 0x4c. [0x00426cb0]
    return reinterpret_cast<std::uint8_t*>(0x00663664u) + param_1 * 0x4c;
}

RH_ScopedInstall(SlotIndexToPtr, 0x00426cb0);

// ---------------------------------------------------------------------------
// HandleArrayRelease  --  0x00426d90
//
// DEFERRED — callee 0x004e6680 (RpClumpRender) is C1; callee gate fails.
// Implementation authored as placeholder; NOT registered for hook install.
//
// Original: FUN_00426d90 (void(void))
// Body: iterates DAT_00656f48..DAT_00656f54 (4 int handles);
//       calls FUN_004e6680 on each non-null handle.
//
// Constants:
//   0x00656f48 — first handle slot   [0x00426d90]
//   0x00656f58 — exclusive end addr  [0x00426d90]
//
// ref: re/analysis/frontend_c1_to_c2_s5/FUN_00426d90.md
// ---------------------------------------------------------------------------

// Forward: 0x004e6680  RpClumpRender  (C1 — not yet verified)
static auto* const s_FUN_004e6680 =
    reinterpret_cast<void(__cdecl*)(int)>(0x004e6680);

// 0x00426d90  (DEFERRED — callee gate fails; not installed)
extern "C" __declspec(dllexport) void __cdecl HandleArrayRelease()
{
    // Iterate the 4-element int handle array at DAT_00656f48..DAT_00656f54.
    // Release each non-null handle via FUN_004e6680. [0x00426d90..0x00426d9a]
    int* piVar1 = reinterpret_cast<int*>(0x00656f48u);
    do {
        if (*piVar1 != 0) {
            s_FUN_004e6680(*piVar1);   // 0x004e6680 — C1; deferred
        }
        piVar1 = piVar1 + 1;
    } while (reinterpret_cast<std::uintptr_t>(piVar1) < 0x00656f58u);
}

// NOTE: RH_ScopedInstall intentionally omitted — callee 0x004e6680 is C1.
// Re-enable once 0x004e6680 reaches C2+.

// ---------------------------------------------------------------------------
// PathBuilderLoad  --  0x0042a640
//
// DEFERRED — no suitable Frida arg_type for char* filename + vtable dispatch.
// Implementation authored as placeholder; not promoted this session.
//
// Original: FUN_0042a640 (undefined4 FUN_0042a640(char* param_1))
// Body:
//   1. vtable[0xcc](param_1) — copy filename into DAT_0067e3a8.
//   2. if (!_strchr(param_1, '.')) vtable[0xd4](&ext) — append extension.
//   3. if (FUN_0042a530(&DAT_0067dba8, &DAT_0067e3a8) != 0)
//        return FUN_004b3c60();
//   4. return 0;
//
// Constants:
//   0x007d3ff8 — vtable-like dispatch table base   [0x0042a640 body]
//   0xcc       — copy-string offset                [0x0042a640 body]
//   0xd4       — append-extension offset           [0x0042a640 body]
//   0x005f65b4 — pointer-to-default-extension str  [0x0042a640 body]
//   0x0067dba8 — start of path/name buffer         [0x0042a640 body]
//   0x0067e3a8 — destination string buffer         [0x0042a640 body]
//   0x2e ('.')  — extension separator              [0x0042a640 body]
//
// ref: re/analysis/frontend_c1_to_c2_s6/FUN_0042a640.md
// ---------------------------------------------------------------------------

// Forward callees:
// 0x004a2fd0  _strchr  (CRT; C2+)
static auto* const s_strchr =
    reinterpret_cast<char*(__cdecl*)(const char*, int)>(0x004a2fd0);
// 0x0042a530  FUN_0042a530  (C2; piz searcher)
static auto* const s_FUN_0042a530 =
    reinterpret_cast<std::int32_t(__cdecl*)(void*, void*)>(0x0042a530);
// 0x004b3c60  FUN_004b3c60  (C2; stream reader)
static auto* const s_FUN_004b3c60 =
    reinterpret_cast<std::uint32_t(__cdecl*)()>(0x004b3c60);

// 0x0042a640  (DEFERRED — no suitable Frida arg_type; not installed)
extern "C" __declspec(dllexport) std::uint32_t __cdecl PathBuilderLoad(char* param_1)
{
    // Dispatch table pointer at 0x007d3ff8. [0x0042a640]
    void** vtable = *reinterpret_cast<void***>(0x007d3ff8u);

    // Step 1: vtable[0xcc/4] copies param_1 into DAT_0067e3a8. [0x0042a640 +0xcc]
    auto copy_fn = reinterpret_cast<void(__cdecl*)(const char*)>(
        reinterpret_cast<std::uint8_t*>(vtable)[0xcc / sizeof(void*)]);
    (void)copy_fn;  // placeholder — indirect call through table offset
    // Emit the actual vtable-offset call as in the original:
    reinterpret_cast<void(__cdecl*)(const char*)>(
        *reinterpret_cast<void**>(
            reinterpret_cast<std::uint8_t*>(*reinterpret_cast<void**>(0x007d3ff8u)) + 0xcc)
    )(param_1);

    // Step 2: if no '.' in param_1, append default extension. [0x0042a6xx]
    if (s_strchr(param_1, 0x2e) == nullptr) {
        const char* ext = *reinterpret_cast<const char**>(0x005f65b4u);
        reinterpret_cast<void(__cdecl*)(const char*)>(
            *reinterpret_cast<void**>(
                reinterpret_cast<std::uint8_t*>(*reinterpret_cast<void**>(0x007d3ff8u)) + 0xd4)
        )(ext);
    }

    // Step 3: load resource from path; return 0 on failure, FUN_004b3c60() on success.
    if (s_FUN_0042a530(
            reinterpret_cast<void*>(0x0067dba8u),
            reinterpret_cast<void*>(0x0067e3a8u)) != 0)
    {
        return s_FUN_004b3c60();
    }
    return 0u;
}

// NOTE: RH_ScopedInstall intentionally omitted — no Frida arg_type for char* path.
// Queue harness-ext work before re-attempting C3 diff for this function.

// ---------------------------------------------------------------------------
// ConstantGetter22  --  0x0042b920
//
// Original: FUN_0042b920 (5 bytes, 0x0042b920..0x0042b924)
// Signature: undefined4 FUN_0042b920(void)
//   Returns: 0x16 (22) unconditionally, no side-effects.
//
// Body (pure constant getter):
//   MOV EAX, 0x16   [0x0042b920]
//   RET              [0x0042b924]
//
// Callees: none (pure leaf).
//
// ref: re/analysis/frontend_c1_to_c2_s6/FUN_0042b920.md
// ---------------------------------------------------------------------------

// 0x0042b920
extern "C" __declspec(dllexport) std::uint32_t __cdecl ConstantGetter22()
{
    // Returns 0x16 = 22 unconditionally. [0x0042b920]
    return 0x16u;
}

RH_ScopedInstall(ConstantGetter22, 0x0042b920);

// ---------------------------------------------------------------------------
// HudRectEmitter  --  0x0042bde0
//
// Original: FUN_0042bde0 (0x0042bde0..0x0042bf2b)
// Signature: void FUN_0042bde0(int param_1, byte param_2)
//   param_1: base row index; fVar1 = (float)(param_1 - 13)
//   param_2: colour/alpha byte; packed into ARGB words per call
// Returns: void
//
// Algorithm (5 calls to ChromeBaseDraw = 0x00472c60):
//   fVar1 = (float)(param_1 - 13)                          [0x0042bde0]
//   fVar2 = _DAT_005cc35c + 534.0                           [0x0042bde0]
//   fVar3 = 26.0 - _DAT_005cc574                            [0x0042bde0]
//   CONCAT13(a, b) = ((uint32_t)(a) << 24) | (b & 0x00FFFFFF)
//
//   Call 1: ChromeBaseDraw(48.0,   fVar1, 534.0,  26.0, CONCAT13(param_2>>1, 0x146ef0))
//   Call 2: ChromeBaseDraw(46.0,   fVar1, 2.0,    26.0, CONCAT13(param_2,    0x1050b4))
//   Call 3: ChromeBaseDraw(46.0,   fVar1, fVar2,  2.0,  CONCAT13(param_2,    0x1050b4))
//   Call 4: ChromeBaseDraw(46.0,   fVar1+fVar3, fVar2, 2.0, CONCAT13(param_2, 0x1050b4))
//   Call 5: ChromeBaseDraw(_DAT_005cd784+534.0, fVar1-fVar3, 2.0, 26.0, CONCAT13(param_2, 0x1050b4))
//
// Constants (cited from 0x0042bde0 body):
//   0x42400000 = 48.0   [call 1 x-arg]
//   0x44058000 = 534.0  [call 1 w-arg, call 3/4 w-arg]
//   0x41d00000 = 26.0   [call 1/2 h-arg, call 5 h-arg]
//   0x42380000 = 46.0   [call 2/3/4 x-arg]
//   0x40000000 = 2.0    [call 2/3/4/5 w-arg or h-arg]
//   0x005cc35c — global float addend for fVar2               [0x0042bde0]
//   0x005cc574 — global float for fVar3 = 26.0 - this       [0x0042bde0]
//   0x005cd784 — global float for call-5 x = this + 534.0   [0x0042bde0]
//   0x00146ef0 — low-24-bit colour for call 1
//   0x001050b4 — low-24-bit colour for calls 2-5
//
// Callees: ChromeBaseDraw (0x00472c60) — C3.  Gate passes.
//
// ref: re/analysis/frontend_c1_to_c2_s6/FUN_0042bde0.md
// ---------------------------------------------------------------------------

// Helper: CONCAT13(high_byte, low_3bytes) — pack 1 byte into high 8 bits of 4-byte word.
static inline std::uint32_t concat13(std::uint8_t high, std::uint32_t low24) {
    return (static_cast<std::uint32_t>(high) << 24u) | (low24 & 0x00FFFFFFu);
}

// 0x0042bde0
extern "C" __declspec(dllexport) void __cdecl HudRectEmitter(int param_1, std::uint8_t param_2)
{
    // fVar1 = (float)(param_1 - 13) — base Y offset. [0x0042bde0]
    float fVar1 = static_cast<float>(param_1 - 13);

    // fVar2 = _DAT_005cc35c + 534.0 [0x0042bde0]
    float fVar2 = *reinterpret_cast<float*>(0x005cc35cu) + 534.0f;

    // fVar3 = 26.0 - _DAT_005cc574 [0x0042bde0]
    float fVar3 = 26.0f - *reinterpret_cast<float*>(0x005cc574u);

    // Colour packs: [0x0042bde0 body]
    std::uint32_t colour1 = concat13(static_cast<std::uint8_t>(param_2 >> 1u), 0x146ef0u);
    std::uint32_t colour2 = concat13(param_2, 0x1050b4u);

    // Call 1: (48.0, fVar1, 534.0, 26.0, colour1)
    s_ChromeBaseDraw(48.0f, fVar1, 534.0f, 26.0f, colour1);

    // Call 2: (46.0, fVar1, 2.0, 26.0, colour2)
    s_ChromeBaseDraw(46.0f, fVar1, 2.0f, 26.0f, colour2);

    // Call 3: (46.0, fVar1, fVar2, 2.0, colour2)
    s_ChromeBaseDraw(46.0f, fVar1, fVar2, 2.0f, colour2);

    // Call 4: (46.0, fVar1+fVar3, fVar2, 2.0, colour2) — fVar3 = 26.0 - DAT_005cc574
    s_ChromeBaseDraw(46.0f, fVar1 + fVar3, fVar2, 2.0f, colour2);

    // Call 5: (DAT_005cd784+534.0, fVar1-fVar3, 2.0, 26.0, colour2)
    float x5 = *reinterpret_cast<float*>(0x005cd784u) + 534.0f;
    s_ChromeBaseDraw(x5, fVar1 - fVar3, 2.0f, 26.0f, colour2);
}

RH_ScopedInstall(HudRectEmitter, 0x0042bde0);
