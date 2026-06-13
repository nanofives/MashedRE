// Mashed RE — promote-round round 27 (worklist batch: global getters).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Five single-global getter leaves; all bodies byte-verified, all callers
// confirmed C2 via a round-27 Ghidra reference_to pass.
//   0x0042b900  Global67eca4Get   — render; mov eax,[0x0067eca4]; ret  (caller 00466b50 util C2)
//   0x00430820  Global67ed6cGet   — read_global 0x0067ed6c            (caller 004103a0 util C2)
//   0x00492d10  Global771968Get   — read_global 0x00771968            (caller 00465a30 audio C2)
//   0x00431b30  Float67eaa8Get    — fld [0x0067eaa8]; ret (float)      (caller 0043dfd0 frontend C2)
//   0x00452eb0  PowerupRangeGet   — fld [0x00684de0]; ret (float)      (caller 00417640 ai C2)

#include "../Core/HookSystem.h"

#include <cstdint>

// 0x0042b900  A1 A4 EC 67 00 C3
extern "C" __declspec(dllexport) std::uint32_t __cdecl Global67eca4Get(void) {
    return *reinterpret_cast<const std::uint32_t*>(0x0067eca4u);  // cited at 0x0042b900
}
RH_ScopedInstall(Global67eca4Get, 0x0042b900);

// 0x00430820  A1 6C ED 67 00 C3
extern "C" __declspec(dllexport) std::uint32_t __cdecl Global67ed6cGet(void) {
    return *reinterpret_cast<const std::uint32_t*>(0x0067ed6cu);  // cited at 0x00430820
}
RH_ScopedInstall(Global67ed6cGet, 0x00430820);

// 0x00492d10  A1 68 19 77 00 C3
extern "C" __declspec(dllexport) std::uint32_t __cdecl Global771968Get(void) {
    return *reinterpret_cast<const std::uint32_t*>(0x00771968u);  // cited at 0x00492d10
}
RH_ScopedInstall(Global771968Get, 0x00492d10);

// 0x00431b30  D9 05 A8 EA 67 00 C3  (FLD m32 -> ST0; true float load)
extern "C" __declspec(dllexport) float __cdecl Float67eaa8Get(void) {
    return *reinterpret_cast<const float*>(0x0067eaa8u);  // cited at 0x00431b30
}
RH_ScopedInstall(Float67eaa8Get, 0x00431b30);

// 0x00452eb0  D9 05 E0 4D 68 00 C3  (FLD m32; powerup pursuit range)
extern "C" __declspec(dllexport) float __cdecl PowerupRangeGet(void) {
    return *reinterpret_cast<const float*>(0x00684de0u);  // cited at 0x00452eb0
}
RH_ScopedInstall(PowerupRangeGet, 0x00452eb0);
