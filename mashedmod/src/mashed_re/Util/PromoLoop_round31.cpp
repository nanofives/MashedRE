// Mashed RE — promote-round round 31 (byte-scan batch: single-global getters).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// All bodies byte-verified as `A1 <addr32> C3` (mov eax,[global]; ret). Callers
// confirmed C2 (round-31 reference_to):
//   0x0042f760  Global67f19cGet — [0x0067f19c] (caller 004631f0 audio C2)
//   0x0042f770  Global67f1a0Get — [0x0067f1a0] (caller 004631f0 audio C2)
//   0x0042f780  Global67f1a4Get — [0x0067f1a4] (caller 004631f0 audio C2)
//   0x00426bb0  Global66d6d8Get — [0x0066d6d8] (caller 00408610 gameplay C2)
//   0x0045bfe0  Global80332cGet — [0x0080332c] (caller 00406160 gameplay C2)

#include "../Core/HookSystem.h"

#include <cstdint>

// 0x0042f760  A1 9C F1 67 00 C3  (mov eax,[0x0067f19c]; ret) — channel-B trigger flag
extern "C" __declspec(dllexport) std::uint32_t __cdecl Global67f19cGet(void) {
    return *reinterpret_cast<const std::uint32_t*>(0x0067f19cu);  // cited at 0x0042f760
}
RH_ScopedInstall(Global67f19cGet, 0x0042f760);

// 0x0042f770  A1 A0 F1 67 00 C3  (mov eax,[0x0067f1a0]; ret) — channel-C trigger-A flag
extern "C" __declspec(dllexport) std::uint32_t __cdecl Global67f1a0Get(void) {
    return *reinterpret_cast<const std::uint32_t*>(0x0067f1a0u);  // cited at 0x0042f770
}
RH_ScopedInstall(Global67f1a0Get, 0x0042f770);

// 0x0042f780  A1 A4 F1 67 00 C3  (mov eax,[0x0067f1a4]; ret) — channel-C trigger-B flag
extern "C" __declspec(dllexport) std::uint32_t __cdecl Global67f1a4Get(void) {
    return *reinterpret_cast<const std::uint32_t*>(0x0067f1a4u);  // cited at 0x0042f780
}
RH_ScopedInstall(Global67f1a4Get, 0x0042f780);

// 0x00426bb0  A1 D8 D6 66 00 C3  (mov eax,[0x0066d6d8]; ret) — path-count int
extern "C" __declspec(dllexport) std::uint32_t __cdecl Global66d6d8Get(void) {
    return *reinterpret_cast<const std::uint32_t*>(0x0066d6d8u);  // cited at 0x00426bb0
}
RH_ScopedInstall(Global66d6d8Get, 0x00426bb0);

// 0x0045bfe0  A1 2C 33 80 00 C3  (mov eax,[0x0080332c]; ret)
extern "C" __declspec(dllexport) std::uint32_t __cdecl Global80332cGet(void) {
    return *reinterpret_cast<const std::uint32_t*>(0x0080332cu);  // cited at 0x0045bfe0
}
RH_ScopedInstall(Global80332cGet, 0x0045bfe0);
