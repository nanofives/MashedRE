// Mashed RE — SmplFzx state-block getter by id, with log-on-miss (0x004853b0).
//
// Logging variant of 0x00485380 PhysicsObject_GetStateBlock.
// Disasm 0x004853b0 (verbatim):
//   push esi ; mov esi,[esp+8]          ; param_1 (id)
//   push esi ; call 0x485340 ; add esp,4 ; test eax,eax ; jne valid  ; FUN_00485340(id)
//   push esi ; push 0x5cf310 ; call 0x4987b0 ; add esp,8 ; xor eax,eax ; ret  ; log + 0
//  valid:
//   mov eax,[0x6e71cc] ; mov ecx,[eax+0xc]   ; base = *( *(0x6e71cc) + 0xc )  (DOUBLE deref)
//   shl esi,4 ; mov eax,[esi+ecx] ; ret       ; return *(base + id*0x10)
//
// Callees stay ORIGINAL:
//   0x00485340 FUN_00485340 (C3) PhysicsObject_ValidateHandle — int __cdecl(uint id)
//   0x004987b0 FUN_004987b0 (C2) variadic __cdecl debug-printf -> OutputDebugStringA
//   fmt string lives at .rdata VA 0x005cf310 ("SmplFzx : Not an active SmplFzx object %d\n").
// Ref: re/analysis/bucket_physics_smplfzx_00478cb0_0057c4b0/0x004853b0.md
// Binary anchor: MASHED.exe size=2,846,720 sha256=BDCAE093...EFD3C0E
#include "../Core/HookSystem.h"
#include <cstdint>

namespace {
constexpr std::uintptr_t kMgrPtr_6e71cc = 0x006e71ccu;  // -> manager struct; base at +0xc
constexpr std::uintptr_t kMissFmt_5cf310 = 0x005cf310u; // original .rdata format string

// 0x00485340  FUN_00485340  PhysicsObject_ValidateHandle — int __cdecl(uint id).
__declspec(naked) int __cdecl ValidateHandle(unsigned int /*id*/) {
    __asm {
        push 0x00485340
        ret
    }
}
// 0x004987b0  FUN_004987b0  debug printf — variadic __cdecl. Tail-jump preserves varargs.
__declspec(naked) void __cdecl GamePrintfDbg(const char* /*fmt*/, ...) {
    __asm {
        push 0x004987b0
        ret
    }
}
} // namespace

// ─────────────────────────────────────────────────────────────────────────────
// 0x004853b0  SmplFzxStateBlockGetLogged(id) -> state-block dword, or 0 on miss.
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) std::uint32_t __cdecl SmplFzxStateBlockGetLogged(int id) {
    if (ValidateHandle(static_cast<unsigned int>(id)) == 0) {
        GamePrintfDbg(reinterpret_cast<const char*>(kMissFmt_5cf310), id);
        return 0u;
    }
    std::uintptr_t mgr  = *reinterpret_cast<const std::uintptr_t*>(kMgrPtr_6e71cc);
    std::uintptr_t base = *reinterpret_cast<const std::uintptr_t*>(mgr + 0xcu);
    return *reinterpret_cast<const std::uint32_t*>(
        base + (static_cast<std::uintptr_t>(static_cast<std::uint32_t>(id)) << 4));
}
RH_ScopedInstall(SmplFzxStateBlockGetLogged, 0x004853b0);
