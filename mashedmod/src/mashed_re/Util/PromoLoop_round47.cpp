// Mashed RE — promote-round round 47 (stride-0x18 table getter + 2 clearers).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// All byte-verified self-contained (no calls). Callers C2:
//   0x00454a30  Table688304Get  — return *(0x00688304 + i*0x18) (caller 004177b0 ai       C2)
//   0x0045c850  Table88f09cClear— *(0x0088f09c + i*4) = 0        (caller 004657b0 util     C2)
//   0x004894f0  Clear894f0      — p->[0x54]=p->[8]=p->[0xc]=0    (caller 00486270 particle C2)

#include "../Core/HookSystem.h"

#include <cstdint>

// 0x00454a30  8B 44 24 04 8D 04 40 8B 04 C5 04 83 68 00 C3
//   mov eax,[esp+4]; lea eax,[eax+eax*2]; mov eax,[eax*8 + 0x00688304]; ret  (stride 0x18)
extern "C" __declspec(dllexport) std::uint32_t __cdecl Table688304Get(std::uint32_t param_1) {
    return *reinterpret_cast<const std::uint32_t*>(0x00688304u + param_1 * 0x18u);  // cited at 0x00454a30
}
RH_ScopedInstall(Table688304Get, 0x00454a30);

// 0x0045c850  8B 44 24 04 C7 04 85 9C F0 88 00 00 00 00 00 C3
//   mov eax,[esp+4]; mov dword [eax*4 + 0x0088f09c],0; ret
extern "C" __declspec(dllexport) void __cdecl Table88f09cClear(std::uint32_t param_1) {
    reinterpret_cast<std::uint32_t*>(0x0088f09cu)[param_1] = 0;  // cited at 0x0045c850
}
RH_ScopedInstall(Table88f09cClear, 0x0045c850);

// 0x004894f0  8B 44 24 04 33 C9 89 48 54 89 48 08 89 48 0C C3
//   mov eax,[esp+4]; xor ecx,ecx; mov [eax+0x54],ecx; mov [eax+8],ecx; mov [eax+0xc],ecx; ret
extern "C" __declspec(dllexport) void __cdecl Clear894f0(void* param_1) {
    std::uint8_t* p = reinterpret_cast<std::uint8_t*>(param_1);
    *reinterpret_cast<std::uint32_t*>(p + 0x54) = 0;  // cited at 0x004894f0
    *reinterpret_cast<std::uint32_t*>(p + 0x08) = 0;
    *reinterpret_cast<std::uint32_t*>(p + 0x0c) = 0;
}
RH_ScopedInstall(Clear894f0, 0x004894f0);
