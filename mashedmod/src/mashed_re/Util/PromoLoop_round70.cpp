// Mashed RE — promote-round round 70 (bit-extract getter + 4-global clearer + 2 record-eq predicates).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// All four are __cdecl, self-contained (zero callees), byte-verified from the listing.
//
// FUN_0041efe0 (0x0041efe0): bit-extract getter.
//   MOV EAX,[ESP+4] ; IMUL EAX,0x2ac ; MOVZX EAX,byte [EAX+0x63dc74] ; SHR EAX,3 ; AND EAX,1
//   -> return (*(u8*)(0x0063dc74 + i*0x2ac) >> 3) & 1
//
// FUN_004298c0 (0x004298c0): zero four globals.
//   XOR EAX,EAX ; MOV [0x67d99c],0 ; MOV [0x67d994],EAX ; MOV [0x67d98c],EAX ; MOV [0x008991bc],EAX
//
// FUN_00432230 / FUN_00432260 (0x00432230 / 0x00432260): gated record-equality predicate.
//   g=*(int*)0x0067e9f8 ; rec=0x0067ed3c + g*0x40 ;
//   return (*(int*)(rec+0)==0x13 && *(int*)(rec+4)==V) ? 1 : 0    (V=1 for 230, V=2 for 260)

#include "../Core/HookSystem.h"

#include <cstdint>

// 0x0041efe0
extern "C" __declspec(dllexport) std::uint32_t __cdecl BitExtract63dc74(std::int32_t i) {
    return (*reinterpret_cast<std::uint8_t*>(0x0063dc74u + static_cast<std::uint32_t>(i) * 0x2acu) >> 3) & 1u;  // MOVZX ; SHR 3 ; AND 1
}
RH_ScopedInstall(BitExtract63dc74, 0x0041efe0);

// 0x004298c0
extern "C" __declspec(dllexport) void __cdecl Clear67d99c_x4(void) {
    *reinterpret_cast<std::uint32_t*>(0x0067d99cu) = 0;
    *reinterpret_cast<std::uint32_t*>(0x0067d994u) = 0;
    *reinterpret_cast<std::uint32_t*>(0x0067d98cu) = 0;
    *reinterpret_cast<std::uint32_t*>(0x008991bcu) = 0;
}
RH_ScopedInstall(Clear67d99c_x4, 0x004298c0);

// 0x00432230
extern "C" __declspec(dllexport) std::uint32_t __cdecl RecEq231(void) {
    std::int32_t g = *reinterpret_cast<std::int32_t*>(0x0067e9f8u);
    std::uint32_t rec = 0x0067ed3cu + static_cast<std::uint32_t>(g) * 0x40u;
    return (*reinterpret_cast<std::int32_t*>(rec + 0) == 0x13 && *reinterpret_cast<std::int32_t*>(rec + 4) == 1) ? 1u : 0u;
}
RH_ScopedInstall(RecEq231, 0x00432230);

// 0x00432260
extern "C" __declspec(dllexport) std::uint32_t __cdecl RecEq232(void) {
    std::int32_t g = *reinterpret_cast<std::int32_t*>(0x0067e9f8u);
    std::uint32_t rec = 0x0067ed3cu + static_cast<std::uint32_t>(g) * 0x40u;
    return (*reinterpret_cast<std::int32_t*>(rec + 0) == 0x13 && *reinterpret_cast<std::int32_t*>(rec + 4) == 2) ? 1u : 0u;
}
RH_ScopedInstall(RecEq232, 0x00432260);
