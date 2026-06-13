// Mashed RE — promote-round round 44 (EAX-implicit field clearer; naked asm).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Byte-verified `33 C9 89 48 4C 89 48 50 C3`
//   xor ecx,ecx; mov [eax+0x4c],ecx; mov [eax+0x50],ecx; ret
//   `this` is in EAX (register-this calling convention) -> reimpl is naked asm so
//   it consumes EAX identically. Diffed via early_window_leaf_diff
//   eax_implicit_void (trampoline sets EAX=buf, checks buf+0x4c / buf+0x50).
//   Caller FUN_004194f0 gameplay C2.

#include "../Core/HookSystem.h"

// 0x004190f0  xor ecx,ecx; mov [eax+0x4c],ecx; mov [eax+0x50],ecx; ret
extern "C" __declspec(dllexport) __declspec(naked) void __cdecl ClearEax4190f0(void) {
    __asm {
        xor ecx, ecx
        mov dword ptr [eax + 0x4c], ecx
        mov dword ptr [eax + 0x50], ecx
        ret
    }
}
RH_ScopedInstall(ClearEax4190f0, 0x004190f0);
