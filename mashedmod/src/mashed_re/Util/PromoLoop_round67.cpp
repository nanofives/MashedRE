// Mashed RE — promote-round round 67 (two global-array-to-bool-out helpers).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Both are __cdecl, self-contained (zero callees), byte-verified from the listing. Both read the
// 4-int global array at 0x008a94e0 and write 4 bool flags to the out pointer.
//
// FUN_0040b970 (0x0040b970): out[k] = (*(int*)(0x008a94e0 + k*4) == 0) ? 1 : 0.
//   ECX = 0x8a94e0 - out ; loop x4: MOV [out],0 ; CMP [ECX+out],0 ; JNZ ; MOV [out],1 ; out+=4
//
// FUN_0040ba00 (0x0040ba00): m = min(99, g0, g1, g2, g3) (signed); out[k] = (g[k] == m) ? 1 : 0.
//   ECX=min(0x63, [0x8a94e0]) ; min-fold [0x8a94e4],[0x8a94e8],[0x8a94ec] (CMP/JGE/MOV ECX,EAX)
//   loop x4: MOV [out],0 ; CMP [EDX+out],ECX ; JNZ ; MOV [out],1 ; out+=4

#include "../Core/HookSystem.h"

#include <cstdint>

// 0x0040b970
extern "C" __declspec(dllexport) void __cdecl Bool0Out8a94e0(int* out) {
    for (int k = 0; k < 4; ++k)
        out[k] = (*reinterpret_cast<int*>(0x008a94e0u + k * 4) == 0) ? 1 : 0;   // CMP [..],0 / JNZ
}
RH_ScopedInstall(Bool0Out8a94e0, 0x0040b970);

// 0x0040ba00
extern "C" __declspec(dllexport) void __cdecl BoolMinOut8a94e0(int* out) {
    int m = 99;                                                                // MOV ECX,0x63
    if (*reinterpret_cast<int*>(0x008a94e0u) < m) m = *reinterpret_cast<int*>(0x008a94e0u);
    if (*reinterpret_cast<int*>(0x008a94e4u) < m) m = *reinterpret_cast<int*>(0x008a94e4u);
    if (*reinterpret_cast<int*>(0x008a94e8u) < m) m = *reinterpret_cast<int*>(0x008a94e8u);
    if (*reinterpret_cast<int*>(0x008a94ecu) < m) m = *reinterpret_cast<int*>(0x008a94ecu);
    for (int k = 0; k < 4; ++k)
        out[k] = (*reinterpret_cast<int*>(0x008a94e0u + k * 4) == m) ? 1 : 0;   // CMP [..],ECX / JNZ
}
RH_ScopedInstall(BoolMinOut8a94e0, 0x0040ba00);
