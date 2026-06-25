// Mashed RE — race-seed write pair (0x00448700).
//
// Disasm 0x00448700 (verbatim):
//   push esi ; mov esi,0x64
//   loop: push 0x897fe0 ; call 0x4464c0 ; add esp,4 ; dec esi ; jne loop  (x100)
//   mov eax,[esp+8]   ; param_1
//   mov ecx,[esp+0xc] ; param_2
//   mov [0x897ffc],eax ; mov [0x898000],ecx ; pop esi ; ret
//
// FUN_004464c0 (C2, CameraEntry::DispatchAll) is __cdecl(void* param_1) — the
// `add esp,4` after each call confirms the caller cleans. It iterates a DIFFERENT
// array (DAT_008964c0, stride 0xd8) and does NOT touch 0x00897ffc/0x00898000, so
// the post-loop writes are the sole producers of those globals.
// Ref: re/analysis/skeleton_prep_render/00448700.md
// Binary anchor: MASHED.exe size=2,846,720 sha256=BDCAE093...EFD3C0E
#include "../Core/HookSystem.h"
#include <cstdint>

namespace {
constexpr std::uintptr_t kDispatchArg_897fe0 = 0x00897fe0u;
constexpr std::uintptr_t kOut1_897ffc        = 0x00897ffcu;
constexpr std::uintptr_t kOut2_898000        = 0x00898000u;

// 0x004464c0  FUN_004464c0  CameraEntry::DispatchAll — void __cdecl(void* param_1).
// Tail-jump thunk: the original returns directly to our caller (cdecl, caller cleans).
__declspec(naked) void __cdecl CallDispatchAll(void* /*param_1*/) {
    __asm {
        push 0x004464c0
        ret
    }
}
} // namespace

// ─────────────────────────────────────────────────────────────────────────────
// 0x00448700  VehicleSeedWritePair
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) void __cdecl VehicleSeedWritePair(std::uint32_t param_1,
                                                                   std::uint32_t param_2) {
    for (int i = 100; i != 0; --i)
        CallDispatchAll(reinterpret_cast<void*>(kDispatchArg_897fe0));
    *reinterpret_cast<std::uint32_t*>(kOut1_897ffc) = param_1;
    *reinterpret_cast<std::uint32_t*>(kOut2_898000) = param_2;
}
RH_ScopedInstall(VehicleSeedWritePair, 0x00448700);
