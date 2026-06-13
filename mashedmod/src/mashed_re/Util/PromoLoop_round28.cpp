// Mashed RE — promote-round round 28 (worklist batch: setters + derived getter).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// All bodies byte-verified; all callers confirmed C2 (round-27 reference_to).
//   0x00431b40  Set67eaa8         — void(uint32)->0x0067eaa8  (caller 00402750 boot C2)
//   0x0049a2e0  Set77396c         — void(uint32)->0x0077396c  (caller 0049a0a0 render C2)
//   0x0049a740  Set773978         — void(uint32)->0x00773978  (caller 0049a0a0 render C2)
//   0x0042f7a0  GhostFlagReset    — void()->0x0067ea70=0      (caller 0040d270 Course::Finish track C2)
//   0x00452160  PowerupTargetPtrGet — uint32()-> *(0x00684dac)+0x30 (caller 00417640 ai C2)

#include "../Core/HookSystem.h"

#include <cstdint>

// 0x00431b40  8B 44 24 04 A3 A8 EA 67 00 C3  (mov eax,[esp+4]; mov [0x0067eaa8],eax; ret)
extern "C" __declspec(dllexport) void __cdecl Set67eaa8(std::uint32_t param_1) {
    *reinterpret_cast<std::uint32_t*>(0x0067eaa8u) = param_1;  // cited at 0x00431b40
}
RH_ScopedInstall(Set67eaa8, 0x00431b40);

// 0x0049a2e0  8B 44 24 04 A3 6C 39 77 00 C3
extern "C" __declspec(dllexport) void __cdecl Set77396c(std::uint32_t param_1) {
    *reinterpret_cast<std::uint32_t*>(0x0077396cu) = param_1;  // cited at 0x0049a2e0
}
RH_ScopedInstall(Set77396c, 0x0049a2e0);

// 0x0049a740  8B 44 24 04 A3 78 39 77 00 C3
extern "C" __declspec(dllexport) void __cdecl Set773978(std::uint32_t param_1) {
    *reinterpret_cast<std::uint32_t*>(0x00773978u) = param_1;  // cited at 0x0049a740
}
RH_ScopedInstall(Set773978, 0x0049a740);

// 0x0042f7a0  C7 05 70 EA 67 00 00 00 00 00 C3  (mov dword [0x0067ea70],0; ret)
//   Resets the ghost-mode flag read by GhostMode::IsActive (0x0042f790, round 8).
extern "C" __declspec(dllexport) void __cdecl GhostFlagReset(void) {
    *reinterpret_cast<std::uint32_t*>(0x0067ea70u) = 0u;  // cited at 0x0042f7a0
}
RH_ScopedInstall(GhostFlagReset, 0x0042f7a0);

// 0x00452160  A1 AC 4D 68 00 83 C0 30 C3  (mov eax,[0x00684dac]; add eax,0x30; ret)
//   Returns the powerup-target pointer field: *(0x00684dac) + 0x30.
extern "C" __declspec(dllexport) std::uint32_t __cdecl PowerupTargetPtrGet(void) {
    return *reinterpret_cast<const std::uint32_t*>(0x00684dacu) + 0x30u;  // cited at 0x00452160
}
RH_ScopedInstall(PowerupTargetPtrGet, 0x00452160);
