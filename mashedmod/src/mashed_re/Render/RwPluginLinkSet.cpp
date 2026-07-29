// Mashed RE — 0x004e4320: plugin-field store + RW list link.
//
// Verbatim 0x004e4320..0x004e4346:
//   004e4320 mov eax,[esp+8]           ; obj  (SECOND argument)
//   004e4324 mov ecx,[0x007d716c]      ; RW PLUGIN DATA OFFSET, not a pointer
//   004e432a push esi
//   004e432b lea esi,[ecx+eax]         ; esi = obj + pluginOffset
//   004e432e mov eax,[eax+4]           ; arg = *(obj+4)
//   004e4331 test eax,eax / je 0x4e433e
//   004e4335 push eax / call 0x4c0e50 / add esp,4
//   004e433e mov eax,[esp+8]           ; value (FIRST argument; esi is pushed)
//   004e4342 mov [esi+0xc],eax         ; *(obj + pluginOffset + 0xc) = value
//   004e4345 pop esi / ret
//
// 0x007d716c is a PLUGIN OFFSET, added to the object — the same shape as
// 0x007dc8d8 (memory feedback_pointer_param_described_as_int). It is not
// dereferenced, so `obj` is the pointer and the global is a small integer.
//
// ORDER MATTERS AND IS TRANSCRIBED: the callee runs BEFORE the store. The
// address `esi` is computed before the call but used after it.
//
// The callee stays ORIGINAL (called by RVA), so both A/B sides execute
// identical callee code from identical restored state.
//
// Binary anchor: MASHED.exe size=2,846,720
//   sha256 BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
#include "../Core/HookSystem.h"
#include <cstdint>

namespace {
// 0x004c0e50 — RW list insert + flag set. NOT IDEMPOTENT: it always does
//   *(u8*)(node+3) |= 3  and  *(u8*)(arg+3) |= 0xc
// and the `test cl,3 / jne` at 0x004c0e62 means a second call skips the list
// insertion entirely. That is precisely why this function cannot be verified by
// a synthetic A/B and needs the snapshot/restore lane.
using LinkFn = void(__cdecl*)(std::uint32_t);
static const LinkFn s_FUN_004c0e50 = reinterpret_cast<LinkFn>(0x004c0e50);

constexpr std::uintptr_t kPluginOff_7d716c = 0x007d716cu;
} // namespace

extern "C" __declspec(dllexport) void __cdecl RwPluginLinkSet(int value, void* obj) {
    const std::uint32_t off =
        *reinterpret_cast<const volatile std::uint32_t*>(kPluginOff_7d716c);
    const std::uintptr_t slot =
        reinterpret_cast<std::uintptr_t>(obj) + off + 0xcu;
    const std::uint32_t arg =
        *reinterpret_cast<const std::uint32_t*>(
            reinterpret_cast<std::uintptr_t>(obj) + 4u);
    if (arg != 0) s_FUN_004c0e50(arg);
    *reinterpret_cast<std::uint32_t*>(slot) = static_cast<std::uint32_t>(value);
}
RH_ScopedInstall(RwPluginLinkSet, 0x004e4320);
