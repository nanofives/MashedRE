// Mashed RE — promote-round round 50 (indexed table getter + 3 indexed setters).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// All decomp-verified self-contained 1-load/1-store indexed table accessors.
// Callers C2:
//   0x00472550  Table691508Get  — return *(0x00691508 + i*0x10)     (caller 0045c110 gameplay C2)
//   0x00472520  Table69150cSet  — *(0x0069150c + i*0x10) = val      (caller 00450300 gameplay C2)
//   0x00488390  Table86ae38Set  — *(0x0086ae38 + i*0x38) = val      (caller 00448ef0 render   C2)
//   0x00416230  Table89a500Set  — *(0x0089a500 + i*0x74) = val      (caller 00443dc0 ai       C2)

#include "../Core/HookSystem.h"

#include <cstdint>

// 0x00472550  return (&DAT_00691508)[i*4]  (stride 0x10)
extern "C" __declspec(dllexport) std::uint32_t __cdecl Table691508Get(std::uint32_t param_1) {
    return *reinterpret_cast<const std::uint32_t*>(0x00691508u + param_1 * 0x10u);  // cited at 0x00472550
}
RH_ScopedInstall(Table691508Get, 0x00472550);

// 0x00472520  (&DAT_0069150c)[i*4] = val  (stride 0x10)
extern "C" __declspec(dllexport) void __cdecl Table69150cSet(std::uint32_t param_1, std::uint32_t param_2) {
    *reinterpret_cast<std::uint32_t*>(0x0069150cu + param_1 * 0x10u) = param_2;  // cited at 0x00472520
}
RH_ScopedInstall(Table69150cSet, 0x00472520);

// 0x00488390  (&DAT_0086ae38)[i*0xe] = val  (stride 0x38)
extern "C" __declspec(dllexport) void __cdecl Table86ae38Set(std::uint32_t param_1, std::uint32_t param_2) {
    *reinterpret_cast<std::uint32_t*>(0x0086ae38u + param_1 * 0x38u) = param_2;  // cited at 0x00488390
}
RH_ScopedInstall(Table86ae38Set, 0x00488390);

// 0x00416230  *(0x0089a500 + i*0x74) = val
extern "C" __declspec(dllexport) void __cdecl Table89a500Set(std::uint32_t param_1, std::uint32_t param_2) {
    *reinterpret_cast<std::uint32_t*>(0x0089a500u + param_1 * 0x74u) = param_2;  // cited at 0x00416230
}
RH_ScopedInstall(Table89a500Set, 0x00416230);
