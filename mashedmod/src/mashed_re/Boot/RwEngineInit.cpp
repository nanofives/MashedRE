// Mashed RE - boot-time RenderWare/DInput init wrappers.
//
// This file is intended to host the engine-init chain (0x00493710 / 0x00493600
// / 0x00493640 / 0x004921d0 / etc.). Most of those candidates are blocked by
// callee-gate failures or unmapped/STUB callees (see DEFERRED.md entries
// authored by this session). The only candidate whose callee gate is currently
// satisfied is the 11-byte DInput predicate at 0x004955b0, whose sole callee
// FUN_00495530 (DI8Create wrapper) was promoted to C2 in session ma1-ghidra-s7.
//
// Reference: re/analysis/boot_subsystem_d3/0x004955b0.md
// Reference: re/analysis/promote_c2_dinput_init/00495530.md (callee, C2)

#include "../Core/HookSystem.h"

#include <cstdint>

// 0x004955b0 — DInput init predicate (bool wrapper).
//
// Decompilation:
//   uint8_t FUN_004955b0(void) {
//       int iVar1 = FUN_00495530();
//       return iVar1 != 0;
//   }
//
// 11 bytes total (0x004955b0..0x004955bb). Sole callee is the IDirectInput8A
// creation wrapper at 0x00495530, which:
//   - prints "Creating DInput object\n"
//   - calls GetModuleHandleA(NULL) for hinst
//   - calls DirectInput8Create(hinst, 0x0800, IID_IDirectInput8A,
//                              &DAT_00771e78, NULL)
//   - returns 1 on HRESULT >= 0, 0 otherwise.
//
// DirectInput8Create is idempotent at the COM level: re-calling it on a
// process that already has an interface returns a fresh IDirectInput8A* and
// overwrites DAT_00771e78. Both original and reimpl paths perform the same
// write, so quiescent main-menu Frida A/B yields identical predicate returns
// (1) and identical side-effect state. crash_equal_ok is set in the hook
// registry as belt-and-braces.

// Forward-declare the wrapped callee at its known RVA. We do NOT redirect it
// — the original code at 0x00495530 is left untouched; we just call through.
typedef int (__cdecl *DInputCreateWrapperFn)(void);
static constexpr std::uintptr_t kDInputCreateWrapperRva = 0x00495530u;

extern "C" __declspec(dllexport) unsigned char __cdecl DInputInitPredicate() {
    auto fn = reinterpret_cast<DInputCreateWrapperFn>(kDInputCreateWrapperRva);
    int result = fn();
    return (result != 0) ? 1u : 0u;
}

RH_ScopedInstall(DInputInitPredicate, 0x004955b0);  // re-enabled 2026-05-24 c3-input
