// Mashed RE - Intro-splash orchestration chain.
// Session c3-batch-m-s5 — C2->C3 promotions for the intro-splash cluster.
//
// This file implements 5 functions from the intro-video playback sequence:
//   IntroSplashOrchestrator  (0x00495350) — main loop: 4 indexed videos, button-skip
//   IntroSplashFrameTickShim (0x00492d20) — per-frame shim: calls input pipeline, returns 1
//   IntroVideoDimGetter      (0x00493f80) — leaf: writes 4 globals into two out-ptr pairs
//   IntroSplashVtableSlot6   (0x004c1a00) — vtable shim: tail-dispatch *(param_1+0x18)
//   IntroSplashRenderState   (0x004c1bb0) — render-state setter via DAT_007d3ff8+0x9c
//
// References:
//   re/analysis/promote_c2_piz_loader/00495350.md
//   re/analysis/intro_splash/0x00492d20.md
//   re/analysis/intro_splash/0x00493f80.md
//   re/analysis/intro_splash/0x004c1a00.md
//   re/analysis/intro_splash/0x004c1bb0.md

#include "../Core/HookSystem.h"
#include <cstdint>
#include <windows.h>   // Sleep

// ─── Forward declarations for callees not yet reimplemented ──────────────────
// These trampolines call the original binary at their known RVAs so that
// IntroSplashOrchestrator can be built and exercised without requiring all
// callees to be reimplemented first.

typedef std::uint32_t  uint32;
typedef std::uint8_t   uint8;

// Callee stubs — forward to original RVAs (boot-time trampolines via function pointer).
// Declared as function pointers to the original addresses so the linker doesn't
// need to know about them. All calls within IntroSplashOrchestrator that go to
// C1/C2 functions are forwarded through these.

// 0x00494a80  FUN_00494a80 — indexed video loader: (int, int index, int) -> void
static void OrigVideoLoad(int a, int index, int b) {
    typedef void (__cdecl* Fn)(int, int, int);
    reinterpret_cast<Fn>(static_cast<std::uintptr_t>(0x00494a80u))(a, index, b);
}

// 0x004671a0  FUN_004671a0 — vehicle-0 getter: (int) -> uint32
static uint32 OrigVehicle0Get(int param) {
    typedef uint32 (__cdecl* Fn)(int);
    return reinterpret_cast<Fn>(static_cast<std::uintptr_t>(0x004671a0u))(param);
}

// 0x00494480  FUN_00494480 — input/button check: (int) -> int
static int OrigInputCheck(int param) {
    typedef int (__cdecl* Fn)(int);
    return reinterpret_cast<Fn>(static_cast<std::uintptr_t>(0x00494480u))(param);
}

// 0x004c1be0  FUN_004c1be0 — render target helper: (uint32, uint32) -> uint32
static uint32 OrigRenderTargetHelper(uint32 a, uint32 b) {
    typedef uint32 (__cdecl* Fn)(uint32, uint32);
    return reinterpret_cast<Fn>(static_cast<std::uintptr_t>(0x004c1be0u))(a, b);
}

// 0x00499710  FUN_00499710 — HWND getter: () -> uint32
static uint32 OrigHwndGet() {
    typedef uint32 (__cdecl* Fn)();
    return reinterpret_cast<Fn>(static_cast<std::uintptr_t>(0x00499710u))();
}

// 0x00493f70  FUN_00493f70 — global reader: (int) -> int (0 = video done)
static int OrigVideoDone(int param) {
    typedef int (__cdecl* Fn)(int);
    return reinterpret_cast<Fn>(static_cast<std::uintptr_t>(0x00493f70u))(param);
}

// 0x00494460  FUN_00494460 — close video: (int) -> void
static void OrigVideoClose(int param) {
    typedef void (__cdecl* Fn)(int);
    reinterpret_cast<Fn>(static_cast<std::uintptr_t>(0x00494460u))(param);
}

// 0x00493fc0  FUN_00493fc0 — aspect ratio helper: (float, float) -> uint32
static uint32 OrigAspectHelper(float a, float b) {
    typedef uint32 (__cdecl* Fn)(float, float);
    return reinterpret_cast<Fn>(static_cast<std::uintptr_t>(0x00493fc0u))(a, b);
}

// 0x00493fd0  FUN_00493fd0 — textured quad render: (uint32, uint32*, uint32*, int, uint32) -> void
static void OrigQuadRender(uint32 a, uint32* b, uint32* c, int d, uint32 e) {
    typedef void (__cdecl* Fn)(uint32, uint32*, uint32*, int, uint32);
    reinterpret_cast<Fn>(static_cast<std::uintptr_t>(0x00493fd0u))(a, b, c, d, e);
}

// 0x004c19f0  RwVtableSlot07Call (C3): tail-call *(param_1+0x1c)
// Already reimplemented in Boot/FrameDispatch.cpp — forward to orig RVA here.
static void OrigRwSlot07(int param) {
    typedef void (__cdecl* Fn)(int);
    reinterpret_cast<Fn>(static_cast<std::uintptr_t>(0x004c19f0u))(param);
}

// 0x004967e0  FUN_004967e0 — per-frame input pipeline: () -> void (discarded)
static void OrigInputPipeline() {
    typedef void (__cdecl* Fn)();
    reinterpret_cast<Fn>(static_cast<std::uintptr_t>(0x004967e0u))();
}


// ─── Dimension globals read by IntroVideoDimGetter ───────────────────────────
// ref: re/analysis/intro_splash/0x00493f80.md constants table
static constexpr std::uintptr_t kIntroDim_007719ec = 0x007719ecu;  // *param_1[0]
static constexpr std::uintptr_t kIntroDim_007719f0 = 0x007719f0u;  // *param_1[1]
static constexpr std::uintptr_t kIntroDim_007719f4 = 0x007719f4u;  // *param_2[0]
static constexpr std::uintptr_t kIntroDim_007719f8 = 0x007719f8u;  // *param_2[1]

// ─── Render interface global ──────────────────────────────────────────────────
// ref: re/analysis/intro_splash/0x004c1bb0.md
static constexpr std::uintptr_t kRwDriver_007d3ff8 = 0x007d3ff8u;

// ─── Input scan constants ─────────────────────────────────────────────────────
// ref: re/analysis/promote_c2_piz_loader/00495350.md constants table
static constexpr std::uintptr_t kInputBase_007f1502 = 0x007f1502u;   // base of input state array
static constexpr std::uintptr_t kInputBound_007f1632 = 0x007f1632u;  // upper bound of scan
static constexpr int            kInputStride    = 0x4C;              // stride between entries
static constexpr int            kInputBackOffset = 0x4C0;            // back-offset (-0x4C0)


// ─── 0x00493f80  IntroVideoDimGetter ─────────────────────────────────────────
// 50 bytes. Pure leaf — no callees.
// Reads 4 globals (0x007719ec/f0/f4/f8) into two out-ptr pairs.
//   param_1: pointer to two undefined4 slots; writes DAT_007719ec, DAT_007719f0
//   param_2: pointer to two undefined4 slots; writes DAT_007719f4, DAT_007719f8
// Caller (IntroSplashOrchestrator) then divides:
//   ratio_a = DAT_007719ec / DAT_007719f4
//   ratio_b = DAT_007719f0 / DAT_007719f8
// and passes both to FUN_00493fc0 (aspect-ratio / scale computation).
// ref: re/analysis/intro_splash/0x00493f80.md
// U-0813: type of globals (float vs int) — caller treats them as floats post-divide.
// Leaf-exemption applies (C2->C3): zero static callees.

// 0x00493f80
extern "C" __declspec(dllexport)
void __cdecl IntroVideoDimGetter(std::uint32_t* param_1, std::uint32_t* param_2) {
    if (param_1 != nullptr) {
        param_1[0] = *reinterpret_cast<const std::uint32_t*>(kIntroDim_007719ec);
        param_1[1] = *reinterpret_cast<const std::uint32_t*>(kIntroDim_007719f0);
    }
    if (param_2 != nullptr) {
        param_2[0] = *reinterpret_cast<const std::uint32_t*>(kIntroDim_007719f4);
        param_2[1] = *reinterpret_cast<const std::uint32_t*>(kIntroDim_007719f8);
    }
}

RH_ScopedInstall(IntroVideoDimGetter, 0x00493f80);  // re-enabled 2026-05-24 c3-frontend-b


// ─── 0x004c1a00  IntroSplashVtableSlot6 ──────────────────────────────────────
// 10 bytes. No static callees.
// Tail-dispatches through vtable slot 6 of an object pointed to by param_1:
//   (**(code **)(param_1 + 0x18))()
// Ghidra warning: "Could not recover jumptable at 0x004c1a08. Too many branches"
// — this is a tail-jmp (JMP not CALL) through the vtable.
// Decompiler shows void return but caller tests eax != 0 as draw-proceed gate.
// ref: re/analysis/intro_splash/0x004c1a00.md
// U-0825: vtable slot 6 semantic ("IsReady"/"BeginScene" style check) — open.
// Leaf-exemption applies (C2->C3): no static callees (vtable indirect only).

static constexpr std::uintptr_t kVtableSlot6Offset = 0x18u;   // 6 × 4 = 24 = 0x18

// 0x004c1a00
extern "C" __declspec(dllexport)
int __cdecl IntroSplashVtableSlot6(int param_1) {
    using VFn = int (*)();
    const VFn fn = *reinterpret_cast<VFn*>(
        static_cast<std::uintptr_t>(static_cast<std::uint32_t>(param_1))
        + kVtableSlot6Offset);
    return fn();
}

RH_ScopedInstall(IntroSplashVtableSlot6, 0x004c1a00);  // re-enabled 2026-05-24 c3-frontend-b


// ─── 0x004c1bb0  IntroSplashRenderState ──────────────────────────────────────
// 39 bytes. No static callees (vtable indirect via DAT_007d3ff8).
// Calls (**(code **)(DAT_007d3ff8 + 0x9c))(param_1, param_2, param_3).
// Returns param_1 on vtable success, 0 on failure:
//   -(iVar1 != 0) & param_1
//     if vtable call != 0: -(1) = 0xFFFFFFFF; 0xFFFFFFFF & param_1 = param_1.
//     if vtable call == 0: -(0) = 0;           0 & param_1 = 0.
// Caller: IntroSplashOrchestrator passes (uVar2, &local_44, 1) where:
//   uVar2     = render target (from FUN_004671a0(0))
//   &local_44 = 4-byte RGBA color {0x00, 0x80, 0x80, 0xFF}
//   1         = param_3
// Caller discards the return value.
// ref: re/analysis/intro_splash/0x004c1bb0.md
// U-0826: slot 39 semantic (set-blend/init-render-pass) — open; does not affect correctness.
// Leaf-exemption applies (C2->C3): no static callees.

static constexpr std::uintptr_t kRwDriverVtable_9c = 0x9cu;   // slot 39 × 4 = 0x9c

// 0x004c1bb0
extern "C" __declspec(dllexport)
std::uint32_t __cdecl IntroSplashRenderState(std::uint32_t param_1,
                                              void*         param_2,
                                              std::uint32_t param_3) {
    const std::uintptr_t driver_vtable_root =
        *reinterpret_cast<const std::uintptr_t*>(kRwDriver_007d3ff8);
    using VFn = int (__cdecl*)(std::uint32_t, void*, std::uint32_t);
    const VFn fn = *reinterpret_cast<VFn*>(driver_vtable_root + kRwDriverVtable_9c);
    const int iVar1 = fn(param_1, param_2, param_3);
    // -(uint)(iVar1 != 0) & param_1
    const std::uint32_t mask = static_cast<std::uint32_t>(-static_cast<int>(iVar1 != 0));
    return mask & param_1;
}

RH_ScopedInstall(IntroSplashRenderState, 0x004c1bb0);  // re-enabled 2026-05-24 c3-frontend-b


// ─── 0x00492d20  IntroSplashFrameTickShim ────────────────────────────────────
// 10 bytes. Single callee: FUN_004967e0 (per-frame input pipeline).
// Calls FUN_004967e0() — no args, return value discarded.
// Returns literal 1.
// Caller (IntroSplashOrchestrator) discards the return value of this shim.
// Callee gate: FUN_004967e0 drift-promoted to C2 this session (full plate exists).
// ref: re/analysis/intro_splash/0x00492d20.md
// U-0811: FUN_004967e0 callee semantics — open (does not affect mechanical correctness).

// 0x00492d20
extern "C" __declspec(dllexport)
int __cdecl IntroSplashFrameTickShim() {
    OrigInputPipeline();   // FUN_004967e0() — per-frame input pipeline
    return 1;
}

RH_ScopedInstall(IntroSplashFrameTickShim, 0x00492d20);  // re-enabled 2026-05-24 c3-frontend-b


// ─── 0x00495350  IntroSplashOrchestrator ─────────────────────────────────────
// 401 bytes. 14 callees (10 intro-splash + Sleep + 4 helpers). No params/return.
//
// Sequence:
//   1. iVar6 = 0 (video index counter).
//   2. local_18[0..3] = {1, 2, 3, 4} (video index sequence).
//   3. Call FUN_00494a80(0, 1, 0) — start first video (index=1).
//   4. Call FUN_00493f80(&local_30, &local_38) — get dimension pair.
//   5. do { ... } while(true):
//      a. bVar1 = false.
//      b. uVar2 = FUN_004671a0(0) — vehicle-0 / render-target handle.
//      c. FUN_00492d20() — per-frame tick shim (return discarded).
//      d. iVar3 = FUN_00494480(0) — button check.
//         If iVar3 != 0: scan DAT_007f1502 in steps of 0x4C;
//           if pcVar4[-0x4C0] != 0 AND *pcVar4 == 0: bVar1=true; break.
//      e. FUN_004c1bb0(uVar2, &local_44, 1) — render-state setter.
//      f. iVar3 = FUN_004c1a00(uVar2) — vtable slot 6 gate.
//         If iVar3 != 0:
//           uStack_20=uStack_1c=0; uStack_28=uStack_24=0x3f800000 (1.0f).
//           fStack_3c = local_30 / local_38.
//           fStack_40 = fStack_2c / fStack_34.
//           uVar5 = FUN_00493fc0(fStack_3c, fStack_40).
//           FUN_00493fd0(uVar2, &uStack_20, &uStack_28, 0, uVar5).
//           FUN_004c19f0(uVar2).
//      g. uVar5 = FUN_00499710(0) — HWND getter.
//      h. FUN_004c1be0(uVar2, uVar5).
//      i. Sleep(5).
//      j. iVar3 = FUN_00493f70(0).
//         If iVar3 == 0 OR bVar1:
//           FUN_00494460(0).
//           ++iVar6.
//           If iVar6 > 3: return.
//           FUN_00494a80(0, local_18[iVar6], 0) — next video.
//           FUN_00493f80(&local_30, &local_38) — refresh dims.
//
// ref: re/analysis/promote_c2_piz_loader/00495350.md
// NOTE: Synthetic Frida force-call is UNSAFE for this function (infinite loop
// with Sleep(5), video playback side-effects). C3 evidence for the orchestrator
// requires a canonical-scenario observation run (main-menu boot shows no videos,
// intro is invoked exactly once at startup). A dedicated canonical-scenario
// Frida run is needed — deferred per STOP-AND-ASK batch rule.
// The hook is registered here so it can be toggled during canonical-scenario verification.
//
// Constants from analysis note:
//   local_44 init: {0x00, 0x80, 0x80, 0xFF} — RGBA pattern
//   uStack_28/uStack_24 = 0x3f800000 = IEEE 754 float 1.0
//   stride 0x4C in input scan; back-offset 0x4C0; base DAT_007f1502; bound 0x7f1632
//   Sleep arg = 5 ms

// 0x00495350
extern "C" __declspec(dllexport)
void __cdecl IntroSplashOrchestrator() {
    // Stack locals matching decompilation layout.
    // local_44 block: 4-byte RGBA colour {0x00, 0x80, 0x80, 0xFF} as LE uint32.
    // Bytes: local_44=0x00, local_43=0x80, local_42=0x80, local_41=0xFF.
    // Stored as uint32 so &rgba_block is the address passed to IntroSplashRenderState.
    std::uint32_t rgba_block = 0xFF808000u;  // LE: bytes[0]=0x00, [1]=0x80, [2]=0x80, [3]=0xFF

    // Video index sequence: indices 1,2,3,4.
    int local_18[5];
    local_18[0] = 1;
    local_18[1] = 2;
    local_18[2] = 3;
    local_18[3] = 4;

    // Dimension outputs from IntroVideoDimGetter.
    // The function writes param_1[0] and param_1[1], so each "dim" is a
    // 2-element array of uint32. Ghidra names them local_30/fStack_2c and
    // local_38/fStack_34 (adjacent pairs on the original stack frame).
    std::uint32_t dim_a[2] = {0u, 0u};  // [0]=DAT_007719ec  [1]=DAT_007719f0
    std::uint32_t dim_b[2] = {0u, 0u};  // [0]=DAT_007719f4  [1]=DAT_007719f8

    // Render quad vertices (passed to FUN_00493fd0 as two 8-byte arrays).
    // uStack_28/uStack_24 = IEEE 754 1.0f (initialised inside the gate block).
    // uStack_20/uStack_1c = 0 (initialised inside the gate block).
    std::uint32_t vert_scale[2] = {0x3f800000u, 0x3f800000u};  // [0]=uStack_28 [1]=uStack_24
    std::uint32_t vert_pos[2]   = {0u, 0u};                    // [0]=uStack_20 [1]=uStack_1c

    int  iVar6 = 0;   // video sequence counter

    // 1. Start first video.
    OrigVideoLoad(0, local_18[0], 0);

    // 2. Get initial dimensions.
    IntroVideoDimGetter(dim_a, dim_b);

    // Main loop (do...while true).
    for (;;) {
        bool bVar1 = false;

        // b. Get render-target handle.
        const uint32 uVar2 = OrigVehicle0Get(0);

        // c. Per-frame input tick (return discarded).
        IntroSplashFrameTickShim();

        // d. Button/input check.
        const int iVar3_btn = OrigInputCheck(0);
        if (iVar3_btn != 0) {
            // Scan input-state byte array at DAT_007f1502 in 0x4C strides.
            // Condition: pcVar4[-0x4C0] != 0 AND *pcVar4 == 0 → skip.
            const std::uint8_t* pcVar4 =
                reinterpret_cast<const std::uint8_t*>(kInputBase_007f1502);
            const std::uint8_t* bound =
                reinterpret_cast<const std::uint8_t*>(kInputBound_007f1632);
            while (pcVar4 < bound) {
                if (*(pcVar4 - kInputBackOffset) != 0 && *pcVar4 == 0) {
                    bVar1 = true;
                    break;
                }
                pcVar4 += kInputStride;
            }
        }

        // e. Render-state setter.
        IntroSplashRenderState(uVar2, &rgba_block, 1u);

        // f. Vtable slot 6 gate.
        const int iVar3_gate = IntroSplashVtableSlot6(static_cast<int>(uVar2));
        if (iVar3_gate != 0) {
            // Reset vertex fields per decompilation (re-initialised each iteration).
            vert_pos[0]   = 0u;           // uStack_20
            vert_pos[1]   = 0u;           // uStack_1c
            vert_scale[0] = 0x3f800000u;  // uStack_28 = 1.0f
            vert_scale[1] = 0x3f800000u;  // uStack_24 = 1.0f

            // Compute aspect ratios from dimension globals (interpreted as floats).
            // dim_a[0] = DAT_007719ec = local_30;  dim_a[1] = DAT_007719f0 = fStack_2c
            // dim_b[0] = DAT_007719f4 = local_38;  dim_b[1] = DAT_007719f8 = fStack_34
            // fStack_3c = local_30 / local_38 = dim_a[0] / dim_b[0]
            // fStack_40 = fStack_2c / fStack_34 = dim_a[1] / dim_b[1]
            const float fStack_3c = *reinterpret_cast<const float*>(&dim_a[0])
                                  / *reinterpret_cast<const float*>(&dim_b[0]);
            const float fStack_40 = *reinterpret_cast<const float*>(&dim_a[1])
                                  / *reinterpret_cast<const float*>(&dim_b[1]);

            const uint32 uVar5 = OrigAspectHelper(fStack_3c, fStack_40);
            // FUN_00493fd0(uVar2, &uStack_20, &uStack_28, 0, uVar5)
            OrigQuadRender(uVar2, vert_pos, vert_scale, 0, uVar5);
            OrigRwSlot07(static_cast<int>(uVar2));  // FUN_004c19f0
        }

        // g. HWND getter.
        const uint32 uVar5_hwnd = OrigHwndGet();

        // h. Render target + HWND present call.
        OrigRenderTargetHelper(uVar2, uVar5_hwnd);

        // i. Frame sleep.
        Sleep(5);

        // j. Check if current video is done or skip triggered.
        const int iVar3_done = OrigVideoDone(0);
        if (iVar3_done == 0 || bVar1) {
            OrigVideoClose(0);
            ++iVar6;
            if (iVar6 > 3) {
                return;  // all 4 videos played
            }
            // Start next video (indices 2, 3, 4 from local_18).
            OrigVideoLoad(0, local_18[iVar6], 0);
            // Refresh dimensions for new video.
            IntroVideoDimGetter(dim_a, dim_b);
        }
    }
}

// MASS-DISABLED 2026-05-24 phase-a2-no-registry-deferred: RH_ScopedInstall(IntroSplashOrchestrator, 0x00495350);
