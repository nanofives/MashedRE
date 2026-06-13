// Mashed RE — promote-round round 35 (float-getter vein: read_global + ret:float).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// All bodies byte-verified as `D9 05 <addr32> C3` (fld dword [global]; ret); the
// 32-bit float is returned in ST0 -> Frida read_global handler with signature
// {ret:'float'} seeds the global with a clean float bit-pattern and compares the
// ST0 return. Callers confirmed C2 (round-35 reference_to):
//   0x004039e0  Float5ea0a8Get — fld [0x005ea0a8] (caller 0041a250 hud C2)
//   0x004173a0  Float89a360Get — fld [0x0089a360] (caller 004046a0 util C2)
//   0x0046dd80  Float61313cGet — fld [0x0061313c] (caller 004177b0 ai  C2)

#include "../Core/HookSystem.h"

#include <cstdint>

// 0x004039e0  D9 05 A8 A0 5E 00 C3  (fld dword [0x005ea0a8]; ret)
extern "C" __declspec(dllexport) float __cdecl Float5ea0a8Get(void) {
    return *reinterpret_cast<const float*>(0x005ea0a8u);  // cited at 0x004039e0
}
RH_ScopedInstall(Float5ea0a8Get, 0x004039e0);

// 0x004173a0  D9 05 60 A3 89 00 C3  (fld dword [0x0089a360]; ret)
extern "C" __declspec(dllexport) float __cdecl Float89a360Get(void) {
    return *reinterpret_cast<const float*>(0x0089a360u);  // cited at 0x004173a0
}
RH_ScopedInstall(Float89a360Get, 0x004173a0);

// 0x0046dd80  D9 05 3C 31 61 00 C3  (fld dword [0x0061313c]; ret)
extern "C" __declspec(dllexport) float __cdecl Float61313cGet(void) {
    return *reinterpret_cast<const float*>(0x0061313cu);  // cited at 0x0046dd80
}
RH_ScopedInstall(Float61313cGet, 0x0046dd80);
