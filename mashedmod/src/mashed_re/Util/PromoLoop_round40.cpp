// Mashed RE — promote-round round 40 (first-party constant-return leaves).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Byte-verified `B8 <imm32> C3` (mov eax,imm; ret) — return a fixed constant, no
// input, no state. Diffed via early_window_leaf_diff const_return. Callers C2:
//   0x0044dfe0  Ret50         -> 0x50       (caller 00461650 audio         C2)
//   0x00493b40  Ret3          -> 0x3        (caller 00493b50 util fn-ptr    C2)
//   0x00443090  Ret897ff0     -> 0x00897ff0 (caller 004847d0 world-objects C2)
//   0x004098a0  Ret63a5f0     -> 0x0063a5f0 (caller 00441820 util          C2)
//   0x004924e0  Ret6147b4     -> 0x006147b4 (caller 004270f0 render        C2)

#include "../Core/HookSystem.h"

#include <cstdint>

// 0x0044dfe0  B8 50 00 00 00 C3
extern "C" __declspec(dllexport) std::uint32_t __cdecl Ret50(void) { return 0x50u; }  // cited at 0x0044dfe0
RH_ScopedInstall(Ret50, 0x0044dfe0);

// 0x00493b40  B8 03 00 00 00 C3
extern "C" __declspec(dllexport) std::uint32_t __cdecl Ret3(void) { return 0x3u; }  // cited at 0x00493b40
RH_ScopedInstall(Ret3, 0x00493b40);

// 0x00443090  B8 F0 7F 89 00 C3
extern "C" __declspec(dllexport) std::uint32_t __cdecl Ret897ff0(void) { return 0x00897ff0u; }  // cited at 0x00443090
RH_ScopedInstall(Ret897ff0, 0x00443090);

// 0x004098a0  B8 F0 A5 63 00 C3
extern "C" __declspec(dllexport) std::uint32_t __cdecl Ret63a5f0(void) { return 0x0063a5f0u; }  // cited at 0x004098a0
RH_ScopedInstall(Ret63a5f0, 0x004098a0);

// 0x004924e0  B8 B4 47 61 00 C3
extern "C" __declspec(dllexport) std::uint32_t __cdecl Ret6147b4(void) { return 0x006147b4u; }  // cited at 0x004924e0
RH_ScopedInstall(Ret6147b4, 0x004924e0);
