// Mashed RE — Boot/Teardown.cpp
// HardwareExitApplication-side teardown helpers (FUN_00402a40 callees).
//
// Scope of this file: only includes reimplementations that are safely
// Frida-testable at the quiescent main menu.  COM-Release and free()-style
// destructors (0x00494bc0 Boot_ReleaseAllCom, 0x00489250 Boot_PairFree,
// 0x00494f20 thunk_FUN_00494460 close-video) are NOT included here because
// invoking them while the game is alive would release live D3D/DirectInput
// interfaces or use-after-free RenderWare frames, instantly crashing the
// process and producing nothing diffable.  Those three are refused this
// session (D-10774 / D-10775 / D-10776 in re/DEFERRED.md) with pickup
// condition "needs canonical HardwareExitApplication scenario harness".

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// 0x00494ef0  thunk_FUN_00493f70 → returns DAT_00771a04
//
// 5-byte E9 JMP thunk at 0x00494ef0 → 0x00493f70.
// Target FUN_00493f70 body (per re/analysis/intro_splash/0x00493f70.md):
//   uint32_t FUN_00493f70(void) { return DAT_00771a04; }
//
// Three callers (function_callers 2026-05-16):
//   FUN_00402a40  (HardwareExitApplication teardown — checks "video still
//                   active?" before invoking thunk_FUN_00494460 to close)
//   FUN_0043dfd0  (intro_splash state poll)
//   FUN_00473c20  (intro_splash state poll)
//
// Reimplementation strategy:
//   Pure global read of DAT_00771a04.  The thunk wraps a read of an integer
//   flag (0 == video done, per FUN_00494460 zeroing this on close).  Both
//   the original and the reimpl deref the same address and return the same
//   value bit-for-bit on every call.  Safe to invoke 10x at quiescent main
//   menu (read-only).
//
// Plate: re/analysis/boot_app_init_d5/0x00494ef0.md (C2 promotion log).
// ---------------------------------------------------------------------------

// 0x00771a04 — DAT_00771a04: "video done" flag read by FUN_00493f70.
static constexpr std::uintptr_t kVideoStateFlagAddr = 0x00771a04u;

// 0x00494ef0
extern "C" __declspec(dllexport) std::uint32_t __cdecl ThunkVideoStateGet(void) {
    return *reinterpret_cast<const std::uint32_t*>(kVideoStateFlagAddr);
}

RH_ScopedInstall(ThunkVideoStateGet, 0x00494ef0);  // re-enabled 2026-05-24 c3-boot-a
