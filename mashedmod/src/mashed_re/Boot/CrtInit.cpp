// Mashed RE - CRT pre-init fn-ptr table iterator (zero-slot).
// Original: 0x004a78b0  FUN_004a78b0  promote_c2_boot_crt  C2 -> C3 candidate
//
// Decompilation at 0x004a78b0:
//   for (local_20 = &DAT_005e7b84; local_20 < &DAT_005e7b84; local_20 = local_20 + 1) {
//       if ((code *)*local_20 != (code *)0x0) {
//           (*(code *)*local_20)();
//       }
//   }
//   return;
//
// Loop start and end are both 0x005e7b84 (table occupies zero slots per decompiler).
// [UNCERTAIN U-0005] Decompiler may have collapsed two distinct table pointers
//   (start + end) into the same symbol. Instruction-level listing at loop-bound
//   instructions would disambiguate. Reproduced as-is; loop body is unreachable.
// SEH frame present (__SEH_prolog / __SEH_epilog injected by MSVC).
// Analysis: re/analysis/promote_c2_boot_crt/004a78b0.md

#include "../Core/HookSystem.h"

#include <cstdint>

// 0x005e7b84 — fn-ptr table start/end (decompiler collapses to same address → zero slots)
static constexpr std::uintptr_t kCrtPreInitTableStart = 0x005e7b84u;
static constexpr std::uintptr_t kCrtPreInitTableEnd   = 0x005e7b84u;

// 0x004a78b0
extern "C" __declspec(dllexport) void __cdecl CrtPreInitLoop() {
    typedef void (__cdecl *fn_ptr_t)();
    auto* p = reinterpret_cast<fn_ptr_t*>(kCrtPreInitTableStart);
    auto* end = reinterpret_cast<fn_ptr_t*>(kCrtPreInitTableEnd);
    for (; p < end; ++p) {
        if (*p != nullptr) {
            (*p)();
        }
    }
}

// MASS-DISABLED 2026-05-24 c3-boot-refused: RH_ScopedInstall(CrtPreInitLoop, 0x004a78b0);
