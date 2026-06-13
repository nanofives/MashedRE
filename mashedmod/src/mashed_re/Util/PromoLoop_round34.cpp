// Mashed RE — promote-round round 34 (byte-scan batch: getter/setters + bounds getter).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// All bodies byte-verified; callers confirmed C2 (round-34 reference_to):
//   0x00495790  Global772facGet  — A1 [0x00772fac]            (caller 004972b0 input C2)
//   0x0044d6e0  Set684b34        — [0x00684b34]=param_1        (caller 00480340 render C2)
//   0x00493590  Set771968_1      — [0x00771968]=1             (caller 00403250 util C2)
//   0x0045a0f0  VehPwrState68ba00Get — v<0||v>=4?-1:[0x0068ba00+v*0x58] (caller 00415220 ai C2)

#include "../Core/HookSystem.h"

#include <cstdint>

// 0x00495790  A1 AC 2F 77 00 C3  (mov eax,[0x00772fac]; ret)
extern "C" __declspec(dllexport) std::uint32_t __cdecl Global772facGet(void) {
    return *reinterpret_cast<const std::uint32_t*>(0x00772facu);  // cited at 0x00495790
}
RH_ScopedInstall(Global772facGet, 0x00495790);

// 0x0044d6e0  8B 44 24 04 A3 34 4B 68 00 C3  (mov eax,[esp+4]; mov [0x00684b34],eax; ret)
extern "C" __declspec(dllexport) void __cdecl Set684b34(std::uint32_t param_1) {
    *reinterpret_cast<std::uint32_t*>(0x00684b34u) = param_1;  // cited at 0x0044d6e0
}
RH_ScopedInstall(Set684b34, 0x0044d6e0);

// 0x00493590  C7 05 68 19 77 00 01 00 00 00 C3  (mov dword [0x00771968],1; ret)
//   Base of the 3-word flag cluster at 0x00771968 (siblings +4/+8 set in round 29).
extern "C" __declspec(dllexport) void __cdecl Set771968_1(void) {
    *reinterpret_cast<std::uint32_t*>(0x00771968u) = 1u;  // cited at 0x00493590
}
RH_ScopedInstall(Set771968_1, 0x00493590);

// 0x0045a0f0  8B 44 24 04 85 C0 7C 0F 83 F8 04 7D 0A 6B C0 58 8B 80 00 BA 68 00 C3 83 C8 FF C3
//   mov eax,[esp+4]; test eax,eax; jl ret-1; cmp eax,4; jge ret-1;
//   imul eax,eax,0x58; mov eax,[eax+0x0068ba00]; ret;  (ret-1:) or eax,-1; ret
//   Signed bounds [0,3] vehicle-powerup-state field getter; -1 out of range.
extern "C" __declspec(dllexport) std::uint32_t __cdecl VehPwrState68ba00Get(std::uint32_t param_1) {
    if (static_cast<std::int32_t>(param_1) < 0) return 0xFFFFFFFFu;       // cited at 0x0045a0f0 (test/jl)
    if (param_1 >= 4u) return 0xFFFFFFFFu;                                // cited at 0x0045a0fa (cmp 4/jge)
    return *reinterpret_cast<const std::uint32_t*>(0x0068ba00u + param_1 * 0x58u);  // cited at 0x0045a103
}
RH_ScopedInstall(VehPwrState68ba00Get, 0x0045a0f0);
