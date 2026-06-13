// Mashed RE — promote-round round 32 (byte-scan batch: getters + param setters).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// All bodies byte-verified; callers confirmed C2 (round-32 reference_to):
//   0x0042d390  Global67ea6cGet — A1 [0x0067ea6c]      (caller 00492e90 boot C2)
//   0x00499710  Global7e9584Get — A1 [0x007e9584] HWND  (caller 00495150 input C2)
//   0x005a7b50  Global7dcabcGet — A1 [0x007dcabc] audioctx (caller 005a7b60 audio C2)
//   0x00472640  Set86ecc8       — [0x0086ecc8]=param_1  (caller 004030d0 util C2)
//   0x0040e170  Set63ba7c       — [0x0063ba7c]=param_1  (caller 0043df00 frontend C2)

#include "../Core/HookSystem.h"

#include <cstdint>

// 0x0042d390  A1 6C EA 67 00 C3  (mov eax,[0x0067ea6c]; ret) — race-state field
extern "C" __declspec(dllexport) std::uint32_t __cdecl Global67ea6cGet(void) {
    return *reinterpret_cast<const std::uint32_t*>(0x0067ea6cu);  // cited at 0x0042d390
}
RH_ScopedInstall(Global67ea6cGet, 0x0042d390);

// 0x00499710  A1 84 95 7E 00 C3  (mov eax,[0x007e9584]; ret) — window handle global
extern "C" __declspec(dllexport) std::uint32_t __cdecl Global7e9584Get(void) {
    return *reinterpret_cast<const std::uint32_t*>(0x007e9584u);  // cited at 0x00499710
}
RH_ScopedInstall(Global7e9584Get, 0x00499710);

// 0x005a7b50  A1 BC CA 7D 00 C3  (mov eax,[0x007dcabc]; ret) — current audio context
extern "C" __declspec(dllexport) std::uint32_t __cdecl Global7dcabcGet(void) {
    return *reinterpret_cast<const std::uint32_t*>(0x007dcabcu);  // cited at 0x005a7b50
}
RH_ScopedInstall(Global7dcabcGet, 0x005a7b50);

// 0x00472640  8B 44 24 04 A3 C8 EC 86 00 C3  (mov eax,[esp+4]; mov [0x0086ecc8],eax; ret)
extern "C" __declspec(dllexport) void __cdecl Set86ecc8(std::uint32_t param_1) {
    *reinterpret_cast<std::uint32_t*>(0x0086ecc8u) = param_1;  // cited at 0x00472640
}
RH_ScopedInstall(Set86ecc8, 0x00472640);

// 0x0040e170  8B 44 24 04 A3 7C BA 63 00 C3  (mov eax,[esp+4]; mov [0x0063ba7c],eax; ret)
extern "C" __declspec(dllexport) void __cdecl Set63ba7c(std::uint32_t param_1) {
    *reinterpret_cast<std::uint32_t*>(0x0063ba7cu) = param_1;  // cited at 0x0040e170
}
RH_ScopedInstall(Set63ba7c, 0x0040e170);
