// Mashed RE - Intro-splash orchestration chain.
// Session c3-batch-m-s5 — C2->C3 promotions for the intro-splash cluster.
//
// This file implements 5 functions from the intro-video playback sequence:
//   HardwareShowIntroVideo  (0x00495350) — main loop: 4 indexed videos, button-skip
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
// HardwareShowIntroVideo can be built and exercised without requiring all
// callees to be reimplemented first.

typedef std::uint32_t  uint32;
typedef std::uint8_t   uint8;

// ─── Ported callees implemented in SplashGameMode_t5.cpp ─────────────────────
// Resolved 2026-07-23 (account2 Lane-A): these two intro-splash callees now have
// C4/C3-verified reimplementations, so HardwareShowIntroVideo calls them
// directly instead of trampolining to the original RVA — retires STUBS S-0801
// and S-0803. Both bodies are 5-byte global getters that ignore their stack args
// (see SplashGameMode_t5.cpp; U-0814).
extern "C" std::uint32_t __cdecl VideoStateFlagGet(void);      // 0x00493f70
extern "C" std::uint32_t __cdecl AspectRatioGlobalGet(void);   // 0x00493fc0

// MATCH-S15: these four now have their only body in Match/MatchS15_<va8>.cpp (byte-matched sources
// re/match/s15/<va8>.c). FUN_00494480 and FUN_00494460 were raw-address calls into the original here;
// neither body reads a stack arg, so the 0 the caller pushed is dropped.
extern "C" int  __cdecl IntroSplashFrameTickShim(void);                                      // MatchS15_00492d20.cpp
extern "C" void __cdecl IntroVideoDimGetter(std::uint32_t* param_1, std::uint32_t* param_2); // MatchS15_00493f80.cpp
extern "C" void __cdecl IntroVideoClose(void);                                               // MatchS15_00494460.cpp
extern "C" std::uint32_t __cdecl IntroInputCheck(void);                                      // MatchS15_00494480.cpp

// Callee stubs — forward to original RVAs (boot-time trampolines via function pointer).
// Declared as function pointers to the original addresses so the linker doesn't
// need to know about them. All calls within HardwareShowIntroVideo that go to
// C1/C2 functions are forwarded through these.

// MATCH-S16: the body of 0x00494a80 (OrigVideoLoad) moved to Match/MatchS16_00494a80.cpp,
// which compiles the byte-matched source re/match/s16/00494a80.c.

// MATCH-S11: FUN_004671a0 (called here through its original address as OrigVehicle0Get) now has a port body,
// Match/MatchS11_004671a0.cpp (BackBuffRasterOpen, the hooks.csv name), which compiles the byte-matched source
// re/match/s11/004671a0.c; its callees 0x0042b930 / 0x0042f510 are called through their original addresses there.
extern "C" uint32 __cdecl BackBuffRasterOpen(int param);

// MATCH-S15: the body of 0x00494480 (OrigInputCheck, a raw-address call into the original; now IntroInputCheck) moved to Match/MatchS15_00494480.cpp,
// which compiles the byte-matched source re/match/s15/00494480.c.

// 0x004c1be0  FUN_004c1be0 — render target helper: (uint32, uint32) -> uint32
static uint32 OrigRenderTargetHelper(uint32 a, uint32 b) {
    typedef uint32 (__cdecl* Fn)(uint32, uint32);
    return reinterpret_cast<Fn>(static_cast<std::uintptr_t>(0x004c1be0u))(a, b);
}

// Call-through to the ORIGINAL FUN_00499710 — HWND getter: () -> uint32 (not a body)
static uint32 OrigHwndGet() {
    typedef uint32 (__cdecl* Fn)();
    return reinterpret_cast<Fn>(static_cast<std::uintptr_t>(0x00499710u))();
}

// 0x00493f70 — now called directly via ported VideoStateFlagGet (see fwd decl above).

// MATCH-S15: the body of 0x00494460 (OrigVideoClose, a raw-address call into the original; now IntroVideoClose) moved to Match/MatchS15_00494460.cpp,
// which compiles the byte-matched source re/match/s15/00494460.c.

// 0x00493fc0 — now called directly via ported AspectRatioGlobalGet (see fwd decl above).

// 0x00493fd0  FUN_00493fd0 — textured quad render: (uint32, uint32*, uint32*, int, uint32) -> void
static void OrigQuadRender(uint32 a, uint32* b, uint32* c, int d, uint32 e) {
    typedef void (__cdecl* Fn)(uint32, uint32*, uint32*, int, uint32);
    reinterpret_cast<Fn>(static_cast<std::uintptr_t>(0x00493fd0u))(a, b, c, d, e);
}

// Call-through to the ORIGINAL RwVtableSlot07Call (address 0x004c19f0, C3): tail-call *(param_1+0x1c)
// Already reimplemented in Boot/FrameDispatch.cpp — forward to orig RVA here (not a body).
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
// Caller (HardwareShowIntroVideo) then divides:
//   ratio_a = DAT_007719ec / DAT_007719f4
//   ratio_b = DAT_007719f0 / DAT_007719f8
// and passes both to FUN_00493fc0 (aspect-ratio / scale computation).
// ref: re/analysis/intro_splash/0x00493f80.md
// U-0813: type of globals (float vs int) — caller treats them as floats post-divide.
// Leaf-exemption applies (C2->C3): zero static callees.

// MATCH-S15: the body of 0x00493f80 (IntroVideoDimGetter) moved to Match/MatchS15_00493f80.cpp,
// which compiles the byte-matched source re/match/s15/00493f80.c.


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

// 2026-07-28 — transcribed verbatim. Sweep A flagged this RVA as an ECX Class-A defect; that
// verdict is a FALSE POSITIVE. The original tail-JMPs, so ECX on return is whatever the
// vtable callee leaves — identical either way. Measuring the emitted body settled it:
// MSVC compiled `return fn();` as a tail jump too, so the callee still inherits the frame and
// still sees param_1 at [ESP+4]:
//     00011dd0  mov eax,dword ptr [esp+4]
//     00011dd4  mov eax,dword ptr [eax+0x18]
//     00011dd7  jmp eax
// A REAL discrepancy survives that, though: the original enters the callee with EAX = param_1
// (it jumps through memory and never overwrites EAX), while ours enters with EAX = the
// function pointer. Any callee that reads EAX sees a different value.
// [UNCERTAIN U-9027] vtable slot 6 targets are not enumerable statically (Ghidra: "Could not
// recover jumptable at 0x004c1a08"), so whether a target reads EAX is unproven. Transcribed
// verbatim regardless — 3 instructions, and exactness costs nothing here.
//
// 0x004c1a00
extern "C" __declspec(dllexport) __declspec(naked)
int __cdecl IntroSplashVtableSlot6(int /*param_1*/) {
    __asm {
        mov eax, dword ptr [esp + 4]        // 0x004c1a00
        mov dword ptr [esp + 4], eax        // 0x004c1a04  (no-op rewrite, kept verbatim)
        jmp dword ptr [eax + 0x18]          // 0x004c1a08  tail-jmp: callee inherits the frame
    }
}

RH_ScopedInstall(IntroSplashVtableSlot6, 0x004c1a00);  // re-enabled 2026-05-24 c3-frontend-b


// ─── 0x004c1bb0  IntroSplashRenderState ──────────────────────────────────────
// 39 bytes. No static callees (vtable indirect via DAT_007d3ff8).
// Calls (**(code **)(DAT_007d3ff8 + 0x9c))(param_1, param_2, param_3).
// Returns param_1 on vtable success, 0 on failure:
//   -(iVar1 != 0) & param_1
//     if vtable call != 0: -(1) = 0xFFFFFFFF; 0xFFFFFFFF & param_1 = param_1.
//     if vtable call == 0: -(0) = 0;           0 & param_1 = 0.
// Caller: HardwareShowIntroVideo passes (uVar2, &local_44, 1) where:
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
// Caller (HardwareShowIntroVideo) discards the return value of this shim.
// Callee gate: FUN_004967e0 drift-promoted to C2 this session (full plate exists).
// ref: re/analysis/intro_splash/0x00492d20.md
// U-0811: FUN_004967e0 callee semantics — open (does not affect mechanical correctness).

// MATCH-S15: the body of 0x00492d20 (IntroSplashFrameTickShim) moved to Match/MatchS15_00492d20.cpp,
// which compiles the byte-matched source re/match/s15/00492d20.c.


// ─── 0x00495350  HardwareShowIntroVideo ─────────────────────────────────────
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

// MATCH-S16: the body of 0x00495350 () moved to Match/MatchS16_00495350.cpp,
// which compiles the byte-matched source re/match/s16/00495350.c.

// MASS-DISABLED 2026-05-24 phase-a2-no-registry-deferred: RH_ScopedInstall(HardwareShowIntroVideo, 0x00495350);
// 2026-07-23 account3 canonical-verify: subset-install (MASHED_HOOK_ONLY=0x00495350) CRASHES at boot —
// access-violation from the (inlined) IntroSplashVtableSlot6 dispatch `call [edi+0x18]` (asm 0x10011887),
// edi=OrigVehicle0Get(0)=FUN_004671a0(0) render-target handle whose +0x18 slot is garbage (0x25ff5dec) at
// intro time. NOT the S-0801/S-0803 rewire (both rewired getters are downstream, never reached — disasm).
// The mis-nested-gate fix above (was a real fidelity bug) did NOT fix it: FUN_00494480 unconditionally
// returns 1, so the gate is always entered — the crash persists at the same dispatch.
// GROUND TRUTH (re/frida trace, .asi loaded + 0 hooks -> original orchestrator runs): the ORIGINAL
// FUN_004c1a00 sees param_1=0x4f2e9dc with *(param_1+0x18)=0x4e3850 (valid MASHED.exe+0xe3850 code ptr),
// 5/5 iterations, boots fine. So .asi loading is passive; the corruption is specific to OUR orchestrator
// reimpl driving the sequence. Sub-fn reimpls (RenderState/VtableSlot6) are faithful + inlined. Exact
// writer of the garbage +0x18 is UNRESOLVED (needs a memory watchpoint on uVar2+0x18; function hooks can't
// observe the inlined calls). Orchestrator stays MASS-DISABLED; C2->C3 canonical observation REFUSED.
// 2026-07-24 ROOT CAUSE CLOSED: a boot-time RW OBJECT-LIFECYCLE ORDERING sensitivity (NOT register/logic).
// [uVar2+0x18] is a RenderWare PLUGIN method ptr, bound at object-CREATE by the plugin-registry constructor
// FUN_004d8000 (callers RpWorldCreate/RpAtomicCreate/RpLightCreate) via the plugin ctor callback 0x004e3800.
// The binder runs on the MAIN thread (not cross-thread) and is TIMING-SENSITIVE (Heisenbug): a bare canonical
// observe dispatches [obj+0x18] before the object's plugins are constructed -> 0x25ff5dec AV; adding ANY Frida
// instrumentation shifts timing so create/bind finishes first -> obj+0x18=0x4e3850, NO crash. Our reimpl's
// timing loses the window the original wins. A real fix must ensure the render target is fully created/plugin-
// constructed before the VtableSlot6 dispatch. Deferred (C4-blocked by 8 Orig* trampolines). Details below:
// 2026-07-24 UPDATE: the EDI-register-clobber hypothesis was tested (volatile uVar2 -> stack reload) and
// REFUTED — same 0x25ff5dec crash with uVar2 provably correct. ROOT CAUSE (revised): the render-target
// object (DAT_006905b0, from FUN_004671a0(0)) has an UNBOUND +0x18 method pointer at intro-boot in the
// hooked run. That slot is set by the binder at 0x004e3800 (writes [obj+0x10/0x18/0x1c]=0x4e38a0/0x4e3850/
// 0x4e3880), dispatched indirectly (xref 0x004e3491). In the stock run (0-hook) the slot is bound (0x4e3850,
// Frida ground truth); with our orchestrator hooked it is not. Next step to close it: a MEMORY WATCHPOINT on
// uVar2+0x18 (now feasible — canonical boot restored 2026-07-24) to catch the bind-vs-dispatch ordering.
