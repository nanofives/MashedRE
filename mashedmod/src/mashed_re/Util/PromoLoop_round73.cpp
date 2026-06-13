// Mashed RE — promote-round round 73 (state-global 2->3 transition + 2-global trigger predicate).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Both are __cdecl, self-contained (zero callees), byte-verified from the listing.
//
// FUN_0042c1a0 (0x0042c1a0): state-global transition.
//   CMP [0x0067eab0],2 ; JNZ ret ; MOV [0x0067eab0],3   -> if(*(int*)0x67eab0==2) *(int*)0x67eab0=3
//
// FUN_00432290 (0x00432290): 2-global trigger predicate.
//   MOV EAX,[0x0067eab0] ; TEST/JZ (==0 -> 0) ; MOV EAX,[0x0067eabc] ;
//   CMP 0xff210000 / JZ true ; CMP 0xff220000 / JNZ false ; true -> 1
//   -> if(*(int*)0x67eab0!=0 && (*(int*)0x67eabc==0xff210000 || *(int*)0x67eabc==0xff220000)) return 1; return 0

#include "../Core/HookSystem.h"

#include <cstdint>

// 0x0042c1a0
extern "C" __declspec(dllexport) void __cdecl StateAdvance2to3(void) {
    if (*reinterpret_cast<std::int32_t*>(0x0067eab0u) == 2)             // CMP [0x67eab0],2 / JNZ
        *reinterpret_cast<std::int32_t*>(0x0067eab0u) = 3;             // MOV [0x67eab0],3
}
RH_ScopedInstall(StateAdvance2to3, 0x0042c1a0);

// 0x00432290
extern "C" __declspec(dllexport) std::uint32_t __cdecl Trigger432290(void) {
    if (*reinterpret_cast<std::int32_t*>(0x0067eab0u) != 0) {           // TEST/JZ
        std::int32_t v = *reinterpret_cast<std::int32_t*>(0x0067eabcu);
        if (v == static_cast<std::int32_t>(0xff210000) || v == static_cast<std::int32_t>(0xff220000))
            return 1;                                                   // MOV EAX,1
    }
    return 0;                                                           // XOR EAX,EAX
}
RH_ScopedInstall(Trigger432290, 0x00432290);
