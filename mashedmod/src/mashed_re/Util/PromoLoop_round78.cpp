// Mashed RE — promote-round round 78 (2-global equality predicate; opportunistic, beyond 200).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// FUN_00405890 (0x00405890): __cdecl, self-contained (zero callees), byte-verified.
//   MOV EAX,[0x0063a5d0] ; TEST/JNZ (g1==0 -> return EAX(=0)) ;
//   MOV EDX,[0x0063a5d4] ; XOR ECX,ECX ; CMP EDX,EAX ; SETZ CL ; MOV EAX,ECX
//   -> g1 = *(int*)0x0063a5d0 ; if (g1 == 0) return 0 ; return (*(int*)0x0063a5d4 == g1) ? 1 : 0

#include "../Core/HookSystem.h"

#include <cstdint>

// 0x00405890
extern "C" __declspec(dllexport) std::uint32_t __cdecl Pred405890(void) {
    int g1 = *reinterpret_cast<int*>(0x0063a5d0u);                         // MOV EAX,[0x63a5d0]
    if (g1 == 0) return 0;                                                 // TEST/JNZ (g1==0 -> 0)
    return (*reinterpret_cast<int*>(0x0063a5d4u) == g1) ? 1u : 0u;         // CMP/SETZ (g2==g1)
}
RH_ScopedInstall(Pred405890, 0x00405890);
