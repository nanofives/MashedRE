// Mashed RE - Per-frame game-logic dispatch entry + 5 direct callees.
// Session ma2-frida-s5 — C2->C3 promotions for the FrameDispatch fanout.
//
// FrameDispatch (0x00492e90) is the per-frame game-logic tick. The 5 small
// helpers below are direct callees as catalogued in
// re/analysis/physics_collision/00492e90.md (callees_depth1 yaml block).
//
// Naming: PascalCase exports; comment cites the RVA per hook-author convention.

#include "../Core/HookSystem.h"
#include <cstdint>


// MATCH-S07: the body of 0x0042b8d0 (StatePhaseIsIdle) moved to Match/MatchS07_0042b8d0.cpp,
// which compiles the byte-matched source re/match/s07/0042b8d0.c.


// MATCH-S07: the body of 0x0042b8f0 (StatePhaseIsFinal) moved to Match/MatchS07_0042b8f0.cpp,
// which compiles the byte-matched source re/match/s07/0042b8f0.c.


// MATCH-S07: the body of 0x0042f510 (Vehicle0HandleGet) moved to Match/MatchS07_0042f510.cpp,
// which compiles the byte-matched source re/match/s07/0042f510.c.


// ─── 0x00498bf0  DisplayActiveFlagGet ─────────────────────────────────────────
// 5 bytes (0x00498bf0..0x00498bf4).
// Decompilation: `return DAT_00773204;` (Ghidra: single MOV-from-mem + RET).
// Caller FUN_004951f0 checks: if non-zero, calls ShowCursor(0).
// Called from FUN_00492e90 per-frame render dispatch.
// ref: re/analysis/promote_c2_video_display/00498bf0.md
static constexpr std::uintptr_t kDisplayActive_00773204 = 0x00773204u;

// 0x00498bf0
extern "C" __declspec(dllexport) std::uint32_t __cdecl DisplayActiveFlagGet() {
    return *reinterpret_cast<const std::uint32_t*>(kDisplayActive_00773204);
}

RH_ScopedInstall(DisplayActiveFlagGet, 0x00498bf0);  // re-enabled 2026-05-24 c3-render-b


// ─── 0x004c19f0  RwVtableSlot07Call ───────────────────────────────────────────
// 10 bytes (0x004c19f0..0x004c19f9).
// Decompilation: `(**(code **)(param_1 + 0x1c))();` (Ghidra: indirect vtable
// dispatch through slot 7 — 0x1c = 7 * 4 bytes). No static callees; Ghidra
// warns "Could not recover jumptable at 0x004c19f8".
// Caller FUN_00495350 passes a render-target handle as param_1.
// Called from FUN_00492e90 per-frame render dispatch.
// ref: re/analysis/intro_splash/0x004c19f0.md
// vtable slot 7 (offset 0x1c from object base).
static constexpr std::uintptr_t kVtableSlot7Offset = 0x1cu;

// MATCH-S19: the body of 0x004c19f0 (RwVtableSlot07Call) moved to Match/MatchS19_004c19f0.cpp,
// which compiles the byte-matched source re/match/s19/004c19f0.c.


// =============================================================================
// Hot-path behavioral lane — session canonical/hot-path-behavioral
// These three functions fire per-frame and cannot be tested with Interceptor.
// They are verified via the survival observation pattern:
//   frida.spawn(suspended) → load ASI → resume → idle 30s → confirm alive.
// Analysis refs:
//   re/analysis/boot_subsystem_d3/0x00492770.md  (MainLoopInit)
//   re/analysis/boot_subsystem_d3/0x00493480.md  (FpsDiscretise)
//   re/analysis/boot_subsystem_d3/0x004926c0.md  (AudioTickAndAvg)
// =============================================================================


// ---------------------------------------------------------------------------
// Callee function pointers for MainLoopInit (0x00492770)
// ---------------------------------------------------------------------------

// FUN_00495110 — called once during main-loop init; purpose not yet classified.
// Signature: void FUN_00495110(void)
using FUN_00495110_t = void (__cdecl*)();
static FUN_00495110_t const s_FUN_00495110 =
    reinterpret_cast<FUN_00495110_t>(0x00495110u);

// FUN_0042b920 — getter called at game-loop init; returns unknown handle/int.
// Signature: int FUN_0042b920(void)
using FUN_0042b920_t = int (__cdecl*)();
static FUN_0042b920_t const s_FUN_0042b920 =
    reinterpret_cast<FUN_0042b920_t>(0x0042b920u);

// FUN_00433240 — init called with result of FUN_0042b920.
// Signature: void FUN_00433240(int)
using FUN_00433240_t = void (__cdecl*)(int);
static FUN_00433240_t const s_FUN_00433240 =
    reinterpret_cast<FUN_00433240_t>(0x00433240u);

// FUN_0042b950 — no-arg init called after FUN_00433240.
// Signature: void FUN_0042b950(void)
using FUN_0042b950_t = void (__cdecl*)();
static FUN_0042b950_t const s_FUN_0042b950 =
    reinterpret_cast<FUN_0042b950_t>(0x0042b950u);

// FUN_004c57a0 — return stored to DAT_007e9dc0; exact RW operation not yet classified.
// Signature: int FUN_004c57a0(void)
using FUN_004c57a0_t = int (__cdecl*)();
static FUN_004c57a0_t const s_FUN_004c57a0 =
    reinterpret_cast<FUN_004c57a0_t>(0x004c57a0u);


// ─── 0x00492770  MainLoopInit ─────────────────────────────────────────────────
// ~60 bytes (0x00492770..~0x004927b0).
// Main game-loop initializer: clears exit flag, resets frame-time globals,
// sets game state to 1, calls 5 subsystem inits.
//
// Decompilation (cited addresses from re/analysis/boot_subsystem_d3/0x00492770.md):
//   DAT_00828300 = 0        (0x00492772) — clear game-loop exit flag
//   FUN_00495110()          (0x00492774) — subsystem init [UNCERTAIN U-3930 callees]
//   DAT_007f1000 = 0        (0x0049277a) — reset frame tick counter
//   DAT_007f101c = 0        (0x00492781) — reset unknown global
//   DAT_007f1020 = 0        (0x00492787) — reset unknown global
//   DAT_00771968 = 1        (0x0049278e) — set game state machine = 1
//   uVar1 = FUN_0042b920()  (0x00492790) — getter [UNCERTAIN U-3928]
//   FUN_00433240(uVar1)     (0x00492797) — init with above [UNCERTAIN U-3929]
//   FUN_0042b950()          (0x0049279c) — no-arg init [UNCERTAIN U-3930]
//   DAT_007e9dc0 = FUN_004c57a0() (0x004927a5) — RW value store [UNCERTAIN U-3931]
//   return 1
//
// ref: re/analysis/boot_subsystem_d3/0x00492770.md

static constexpr std::uintptr_t kGameLoopExitFlag_00828300 = 0x00828300u;
static constexpr std::uintptr_t kFrameTickCounter_007f1000  = 0x007f1000u;
static constexpr std::uintptr_t kUnkGlobal_007f101c         = 0x007f101cu;
static constexpr std::uintptr_t kUnkGlobal_007f1020         = 0x007f1020u;
static constexpr std::uintptr_t kGameStateMachine_00771968  = 0x00771968u;
static constexpr std::uintptr_t kRwValueStore_007e9dc0      = 0x007e9dc0u;

// MATCH-S15: the body of 0x00492770 (MainLoopInit) moved to Match/MatchS15_00492770.cpp,
// which compiles the byte-matched source re/match/s15/00492770.c.


// ---------------------------------------------------------------------------
// Callee function pointer for FpsDiscretise (0x00493480)
// ---------------------------------------------------------------------------

// FUN_00493390 — updates DAT_007f1000 with raw frame delta.
// Signature: void FUN_00493390(void)
using FUN_00493390_t = void (__cdecl*)();
static FUN_00493390_t const s_FUN_00493390 =
    reinterpret_cast<FUN_00493390_t>(0x00493390u);


// ─── 0x00493480  FpsDiscretise ────────────────────────────────────────────────
// ~180 bytes (0x00493480..~0x00493530).
// Frame tick discretizer with FPS snapping: calls FUN_00493390 to update the
// raw frame delta into DAT_007f1000, then snaps to 50/100/150/200Hz buckets,
// accumulates integer ticks, and writes integer + float tick outputs.
//
// Decompilation (re/analysis/boot_subsystem_d3/0x00493480.md):
//   FUN_00493390()                               — raw frame delta update
//   Snap buckets (signed, EAX only; disasm 0x0049348a..0x004934cf):
//     [0x2f..0x35] -> 0x32, [0x61..0x67] -> 0x64, [0x93..0x99] -> 0x96, [0xc5..0xcb] -> 0xc8
//   acc = DAT_007719d4 + ms; n = 0
//   if (int)acc >= 50: n = acc / 50; acc = acc % 50
//   DAT_007719d4 = acc
//   if (int)acc > 47: n += 1; DAT_007719d4 = 0      (runs on every call)
//   else if (int)acc < 3: DAT_007719d4 = 0
//   DAT_007f1000 = n * 50
//   DAT_007f1004 = (float)(int)DAT_007f1000 * DAT_005cc948    (x87 fild/fmul/fstp)
//   return 1
//
// DAT_005cc948 raw bytes: 3e c3 ae 39 = 0.00033333... = 1/3000.0f (resolved U-3933).
// float(uVar1 * 50) * (1/3000) = uVar1 / 60.0f — converts tick-count to seconds.
//
// ref: re/analysis/boot_subsystem_d3/0x00493480.md

static constexpr std::uintptr_t kSubFrameAccum_007719d4 = 0x007719d4u;
// kFrameTickCounter_007f1000 shared with MainLoopInit (0x007f1000) above.
static constexpr std::uintptr_t kFloatTickOutput_007f1004 = 0x007f1004u;

// MATCH-S15: the body of 0x00493480 (FpsDiscretise) moved to Match/MatchS15_00493480.cpp,
// which compiles the byte-matched source re/match/s15/00493480.c.



// ---------------------------------------------------------------------------
// Callee function pointers for AudioTickAndAvg (0x004926c0)
// ---------------------------------------------------------------------------

// FUN_004950b0 — QPC-based time sampler; returns current tick count as int.
// Signature: int FUN_004950b0(void)
using FUN_004950b0_t = int (__cdecl*)();
static FUN_004950b0_t const s_FUN_004950b0 =
    reinterpret_cast<FUN_004950b0_t>(0x004950b0u);

// FUN_00496930 — input button query; param = button index; returns 0/nonzero.
// Signature: int FUN_00496930(int button_index)
using FUN_00496930_t = int (__cdecl*)(int);
static FUN_00496930_t const s_FUN_00496930 =
    reinterpret_cast<FUN_00496930_t>(0x00496930u);

// FUN_0045d3f0 — audio skip op (param=0) [UNCERTAIN U-3932]
using FUN_0045d3f0_t = void (__cdecl*)(int);
static FUN_0045d3f0_t const s_FUN_0045d3f0 =
    reinterpret_cast<FUN_0045d3f0_t>(0x0045d3f0u);

// FUN_00466b50 — audio skip op (param=0) [UNCERTAIN U-3932]
using FUN_00466b50_t = void (__cdecl*)(int);
static FUN_00466b50_t const s_FUN_00466b50 =
    reinterpret_cast<FUN_00466b50_t>(0x00466b50u);

// FUN_0045d430 — audio skip op (no args) [UNCERTAIN U-3932]
using FUN_0045d430_t = void (__cdecl*)();
static FUN_0045d430_t const s_FUN_0045d430 =
    reinterpret_cast<FUN_0045d430_t>(0x0045d430u);


// ─── 0x004926c0  AudioTickAndAvg ──────────────────────────────────────────────
// ~80 bytes (0x004926c0..~0x00492715).
// Per-frame boot-loop tick: measures frame execution time with QPC, handles
// input-state toggle for audio skip, accumulates 60-frame timing average.
//
// Decompilation (re/analysis/boot_subsystem_d3/0x004926c0.md):
//   iVar1 = FUN_004950b0()             — QPC start sample
//   if FUN_00496930(3) != 0:
//     DAT_006147b8 = 1                 — button-3 pressed → set toggle
//   if FUN_00496930(4) != 0:
//     if DAT_006147b8 != 0:
//       FUN_0045d3f0(0)                — audio op [UNCERTAIN U-3932]
//       FUN_00466b50(0)                — audio op [UNCERTAIN U-3932]
//       FUN_0045d430()                 — audio op [UNCERTAIN U-3932]
//     else:
//       DAT_006147b8 = 0              — button-4 pressed → clear toggle
//   iVar2 = FUN_004950b0()             — QPC end sample
//   DAT_00771994 += (iVar2 - iVar1)    — accumulate elapsed ticks
//   DAT_00771978 += 1                  — frame counter
//   if DAT_00771978 == 0x3c (60):
//     DAT_0077197c = DAT_00771994 / 60 — store 60-frame average
//     DAT_00771994 = 0; DAT_00771978 = 0
//
// ref: re/analysis/boot_subsystem_d3/0x004926c0.md

static constexpr std::uintptr_t kInputToggle_006147b8   = 0x006147b8u;
static constexpr std::uintptr_t kQpcAccum_00771994      = 0x00771994u;
static constexpr std::uintptr_t kFrameCount_00771978    = 0x00771978u;
static constexpr std::uintptr_t kAvgTickTime_0077197c   = 0x0077197cu;

// 0x004926c0
extern "C" __declspec(dllexport) void __cdecl AudioTickAndAvg() {
    // QPC start sample at function entry
    const int iVar1 = s_FUN_004950b0();

    // Button-3 pressed → set toggle (at 0x004926d0)
    if (s_FUN_00496930(3) != 0) {
        *reinterpret_cast<std::uint32_t*>(kInputToggle_006147b8) = 1u;
    }

    // Button-4 pressed → conditional audio skip or toggle clear (at 0x004926d5)
    if (s_FUN_00496930(4) != 0) {
        if (*reinterpret_cast<std::uint32_t*>(kInputToggle_006147b8) != 0u) {
            // Audio skip ops [UNCERTAIN U-3932]
            s_FUN_0045d3f0(0);
            s_FUN_00466b50(0);
            s_FUN_0045d430();
        } else {
            *reinterpret_cast<std::uint32_t*>(kInputToggle_006147b8) = 0u;
        }
    }

    // QPC end sample
    const int iVar2 = s_FUN_004950b0();

    // Accumulate elapsed ticks (DAT_00771994 at 0x00492706)
    *reinterpret_cast<std::uint32_t*>(kQpcAccum_00771994) +=
        static_cast<std::uint32_t>(iVar2 - iVar1);
    // Increment frame counter (DAT_00771978 at 0x004926fa)
    std::uint32_t& frame_count = *reinterpret_cast<std::uint32_t*>(kFrameCount_00771978);
    frame_count += 1u;

    // Every 60 frames: store average and reset (0x3c = 60 at 0x004926f5)
    if (frame_count == 0x3cu) {
        const std::uint32_t accum = *reinterpret_cast<std::uint32_t*>(kQpcAccum_00771994);
        *reinterpret_cast<std::uint32_t*>(kAvgTickTime_0077197c) = accum / 60u;
        *reinterpret_cast<std::uint32_t*>(kQpcAccum_00771994) = 0u;
        frame_count = 0u;
    }
}

// MASS-DISABLED 2026-05-24 phase-a2-no-registry-deferred: RH_ScopedInstall(AudioTickAndAvg, 0x004926c0);
