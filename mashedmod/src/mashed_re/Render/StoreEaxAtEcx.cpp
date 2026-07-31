// Mashed RE - two-register store helper.
// Original: 0x004b6b00  FUN_004b6b00  render  C2 -> C3
// Plate: re/analysis/render_3_c1_to_c2_s6/FUN_004b6b00.md
//
// The whole function, read from the listing (Mashed_pool11):
//   0x004B6B00  89 01    MOV dword ptr [ECX],EAX
//   0x004B6B02  C3       RET
//
// Three bytes. Both operands arrive in REGISTERS and nothing is read from the
// stack: ECX is the destination pointer, EAX is the value stored. There is no
// calling convention here in the usual sense — the caller simply has both
// registers loaded and falls into this.
//
// THE PORT MUST BE NAKED __asm, and that is forced by the harness rather than
// by taste. The `eax_ecx_insert` A/B drives BOTH sides through the same
// trampoline — `mov eax,bufA ; mov ecx,bufC ; jmp target` — so the reimpl is
// entered with the values in registers and no stack arguments at all. A
// `__cdecl(void* dst, int val)` port would read whatever happened to be at
// [ESP+4]/[ESP+8], which is the trampoline's return address and garbage.
//
// A C-level equivalent is deliberately NOT written: there is no portable way to
// name the incoming EAX/ECX, and any attempt to do so through a signature would
// change the ABI the original does not have. See
// memory feedback_installed_hook_abi_mismatch — the inline-JMP must preserve the
// original's register contract exactly.
//
// One instruction per line: ';' is an asm COMMENT in MSVC inline assembly, so a
// one-liner would silently swallow everything after it
// (memory feedback_msvc_asm_oneliner_comment).
#include "../Core/HookSystem.h"

// 0x004b6b00
extern "C" __declspec(dllexport) void __declspec(naked) StoreEaxAtEcx()
{
    __asm
    {
        mov dword ptr [ecx], eax
        ret
    }
}

RH_ScopedInstall(StoreEaxAtEcx, 0x004b6b00);
