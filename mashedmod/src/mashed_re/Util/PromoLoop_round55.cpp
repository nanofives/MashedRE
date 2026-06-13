// Mashed RE — promote-round round 55 (three 200-entry scenery-actor getters + a 4-global clearer).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// All four are __cdecl, self-contained (zero callees), byte-verified from the listing.
//
// FUN_0047ce00/ce80/d130: signed-bounds 200-entry dword-table getters.
//   8b442404      MOV EAX,[ESP+4]              ; i
//   85c0 7c..     TEST EAX,EAX ; JL  oob        ; i < 0       -> OOB
//   3dc8000000    CMP EAX,0xc8                  ; 200
//   7d.. / 7c..   JGE/JL                        ; i >= 200    -> OOB
//   8b0485 <base> MOV EAX,[EAX*4 + base]
//   c3            RET
//   33c0 c3 / 83c8ff c3   OOB: XOR EAX,EAX (=0)  OR  OR EAX,-1 (=0xffffffff)
// Byte-verified (base, OOB value):
//   0047ce00: base 0x006c9438 OOB 0           0047ce80: base 0x006c9758 OOB 0xffffffff
//   0047d130: base 0x006c71d8 OOB 0
//
// FUN_0045c860 (0x0045c860): zero four consecutive globals.
//   33c0          XOR EAX,EAX
//   a3 a0f08800   MOV [0x0088f0a0],EAX   (then 0x0088f0a4 / a8 / ac)
//   c3            RET

#include "../Core/HookSystem.h"

#include <cstdint>

// 0x0047ce00
extern "C" __declspec(dllexport) std::uint32_t __cdecl Table6c9438Get(std::int32_t i) {
    if (i >= 0 && i < 0xc8) return *reinterpret_cast<std::uint32_t*>(0x006c9438u + i * 4u);  // MOV [EAX*4+0x6c9438]
    return 0;                                                                                 // XOR EAX,EAX
}
RH_ScopedInstall(Table6c9438Get, 0x0047ce00);

// 0x0047ce80
extern "C" __declspec(dllexport) std::uint32_t __cdecl Table6c9758Get(std::int32_t i) {
    if (i >= 0 && i < 0xc8) return *reinterpret_cast<std::uint32_t*>(0x006c9758u + i * 4u);
    return 0xffffffffu;                                                                       // OR EAX,-1
}
RH_ScopedInstall(Table6c9758Get, 0x0047ce80);

// 0x0047d130
extern "C" __declspec(dllexport) std::uint32_t __cdecl Table6c71d8Get(std::int32_t i) {
    if (i >= 0 && i < 0xc8) return *reinterpret_cast<std::uint32_t*>(0x006c71d8u + i * 4u);
    return 0;
}
RH_ScopedInstall(Table6c71d8Get, 0x0047d130);

// 0x0045c860
extern "C" __declspec(dllexport) void __cdecl Clear88f0a0x4(void) {
    *reinterpret_cast<std::uint32_t*>(0x0088f0a0u) = 0;
    *reinterpret_cast<std::uint32_t*>(0x0088f0a4u) = 0;
    *reinterpret_cast<std::uint32_t*>(0x0088f0a8u) = 0;
    *reinterpret_cast<std::uint32_t*>(0x0088f0acu) = 0;
}
RH_ScopedInstall(Clear88f0a0x4, 0x0045c860);
