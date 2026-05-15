// Mashed RE — HUD in-game dispatch wrappers (game-mode routers).
// Original functions from hud_ingame_promote_c2 cluster.
//
// All RVAs are for MASHED.exe (size 2,846,720 / SHA-256 BDCAE093...).
//
// Callee dependencies (C1 drift-promoted to C2 for this session):
//
//   0x0041c2d0  FUN_0041c2d0  (drift-promoted C1→C2)
//     __thiscall via EAX; unconditionally dispatches vtable[0x48] on
//     EAX[3], EAX[1], EAX[0], EAX[2] (draw order 3→1→0→2).
//     Body 0x0041c2d0..0x0041c2f2 (34 bytes).
//     Shared by sub_0041a3e0 (game-mode 10) and sub_0041c300 (game-mode 5).
//
//   0x0041bc50  FUN_0041bc50  (drift-promoted C1→C2)
//     __thiscall via EAX; 29-slot guarded vtable dispatcher.
//     guard_off → render_off = guard_off + 0xA0 (29 pairs, see analysis note).
//     Body 0x0041bc50..0x0041beaa (603 bytes).
//     Called unconditionally per entry from sub_0041c0c0.
//
// Not promoted in this session (callee gate budget exhausted):
//   0x0041b340  FUN_0041b340  (C1) — needed by 0x0041b630
//   0x0041c9a0  FUN_0041c9a0  (C1) — needed by 0x0041ccc0
#include "../Core/HookSystem.h"

#include <windows.h>
#include <cstdint>

// ---------------------------------------------------------------------------
// Typedef for the C1 callees we forward to (original RVAs still live).
// During A/B verification, the original binary is loaded and these RVAs
// point to original code. Our wrappers simply reproduce the control flow.
// ---------------------------------------------------------------------------
typedef void (__cdecl *VoidVoidFn)(void);

// 0x0041c2d0 — 4-vtable __thiscall dispatcher (shared draw for modes 10 & 5)
static constexpr std::uintptr_t kFun0041c2d0 = 0x0041c2d0;
static inline void Call0041c2d0() {
    reinterpret_cast<VoidVoidFn>(kFun0041c2d0)();
}

// 0x0041bc50 — 29-slot HUD render dispatcher (EAX-thiscall).
// FUN_0041bc50 reads its struct fields via EAX (int in_EAX). The caller
// (FUN_0041c0c0) implicitly passes puVar1 as EAX for each loop iteration.
// We use a naked trampoline to set EAX = entry_ptr before the call.
static constexpr std::uintptr_t kFun0041bc50 = 0x0041bc50;

// Naked trampoline: sets EAX from the first cdecl arg, then calls original.
// Signature: void __cdecl CallFun0041bc50WithEAX(void* entry_ptr)
static __declspec(naked) void __cdecl CallFun0041bc50WithEAX(void* /*entry_ptr*/) {
    __asm {
        mov eax, [esp+4]    ; load entry_ptr into EAX (first cdecl arg)
        mov ecx, 0x0041bc50 ; target address
        call ecx            ; call with EAX already set
        ret                 ; clean return
    }
}

// ---------------------------------------------------------------------------
// Game globals referenced by this cluster (read-only; game owns all writes)
// ---------------------------------------------------------------------------

// 0x0063c628 — int32_t enable flag for game-mode 10 HUD draw path
static constexpr std::uintptr_t kDAT_0063c628 = 0x0063c628;

// 0x0063cdbc — int32_t enable flag for game-mode 5 HUD draw path
static constexpr std::uintptr_t kDAT_0063cdbc = 0x0063cdbc;

// 0x0063cab8 — base of 2-entry array (stride 0x16c = 364 bytes/entry)
//              iterated by sub_0041c0c0; exclusive upper bound 0x0063cd90
static constexpr std::uintptr_t kDAT_0063cab8 = 0x0063cab8;
static constexpr std::uintptr_t kLoop0041c0c0_End = 0x0063cd90;
static constexpr std::uintptr_t kStride_0041c0c0 = 0x16c; // 364 bytes

// ---------------------------------------------------------------------------
// 0x0041a3e0  sub_0041a3e0
// Guard + call: game-mode 10 HUD draw path.
// Reads int32_t at DAT_0063c628; if non-zero → calls FUN_0041c2d0.
// 19 bytes. Shares callee FUN_0041c2d0 with sub_0041c300.
// Called from FUN_0040dfc0 when sub-mode ∈ {3,4,5} AND DAT_007f0fd0 == 10.
// Note: declared as uint32_t for Frida-diff compatibility (EAX probe).
//   Returns 0 always so both original and reimpl produce deterministic EAX=0
//   when the guard fails (main-menu quiescent state).
// ---------------------------------------------------------------------------
// 0x0041a3e0
extern "C" __declspec(dllexport) std::uint32_t __cdecl HudDispatchMode10() {
    // 0x0063c628: int32_t enable flag; non-zero → draw
    if (*reinterpret_cast<std::int32_t*>(kDAT_0063c628) != 0) {
        Call0041c2d0();
    }
    return 0;
}

RH_ScopedInstall(HudDispatchMode10, 0x0041a3e0);

// ---------------------------------------------------------------------------
// 0x0041c300  sub_0041c300
// Guard + call: game-mode 5 HUD draw path.
// Reads int32_t at DAT_0063cdbc; if non-zero → calls FUN_0041c2d0.
// 19 bytes. Parallel structure to sub_0041a3e0 (same callee, different guard).
// Called from FUN_0040dfc0 when sub-mode ∈ {3,4,5} AND DAT_007f0fd0 == 5.
// Note: declared as uint32_t for Frida-diff compatibility (EAX probe).
// ---------------------------------------------------------------------------
// 0x0041c300
extern "C" __declspec(dllexport) std::uint32_t __cdecl HudDispatchMode5() {
    // 0x0063cdbc: int32_t enable flag; non-zero → draw
    if (*reinterpret_cast<std::int32_t*>(kDAT_0063cdbc) != 0) {
        Call0041c2d0();
    }
    return 0;
}

RH_ScopedInstall(HudDispatchMode5, 0x0041c300);

// ---------------------------------------------------------------------------
// 0x0041c0c0  sub_0041c0c0
// 2-entry unconditional loop: iterates array at 0x0063cab8, stride 0x16c.
// No per-entry guard — FUN_0041bc50 called unconditionally each iteration.
// 28 bytes. Loop condition: (int32_t)puVar1 < 0x0063cd90 (pointer-as-signed).
// Called from FUN_0040dfc0 ({5,6}-path) when FUN_0042f500() != 0
//   AND DAT_007f0fd0 != 2.
//
// ABI note: FUN_0041bc50 is an EAX-thiscall function — its object pointer
// is the loop's puVar1 entry pointer, passed implicitly via EAX by the original
// caller. We use CallFun0041bc50WithEAX() to set up EAX correctly.
//
// Note: declared as uint32_t for Frida-diff compatibility (EAX probe).
// ---------------------------------------------------------------------------
// 0x0041c0c0
extern "C" __declspec(dllexport) std::uint32_t __cdecl HudDispatchSlot2() {
    auto* puVar1 = reinterpret_cast<std::uint8_t*>(kDAT_0063cab8);
    do {
        // Pass puVar1 as EAX to FUN_0041bc50 (EAX-thiscall convention).
        CallFun0041bc50WithEAX(static_cast<void*>(puVar1));
        puVar1 += kStride_0041c0c0; // advance 0x16c bytes per entry
    } while (static_cast<std::int32_t>(
                 reinterpret_cast<std::uintptr_t>(puVar1)) <
             static_cast<std::int32_t>(kLoop0041c0c0_End));
    return 0;
}

RH_ScopedInstall(HudDispatchSlot2, 0x0041c0c0);

// ---------------------------------------------------------------------------
// 0x0042f6a0  HudSubModeGet  (export alias for GetRaceSubMode)
// Returns DAT_0067e9fc (uint32_t global). 5 bytes: MOV EAX,[imm32]; RET.
// Used as switch discriminant in FUN_0040dfc0 (per-frame HUD dispatch).
// RH_ScopedInstall omitted: hook already installed via GetRaceSubMode (c3-batch-o-s89).
// ref: re/analysis/hud_ingame_promote_c2/0x0042f6a0.md
// ---------------------------------------------------------------------------
// 0x0042f6a0
extern "C" __declspec(dllexport) std::uint32_t __cdecl HudSubModeGet() {
    // 0x0067e9fc: game sub-mode global; primary HUD dispatch discriminant
    return *reinterpret_cast<const std::uint32_t*>(0x0067e9fcu);
}
