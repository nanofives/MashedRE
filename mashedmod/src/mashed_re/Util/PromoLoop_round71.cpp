// Mashed RE — promote-round round 71 (bounded TTL setter + Pool-J 2-field reset).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Both are __cdecl, self-contained (zero callees), byte-verified from the listing.
//
// FUN_00458fa0 (0x00458fa0): bounded indexed setter `u32 fn(idx, val)`.
//   MOV EAX,[ESP+4] ; TEST/JL ; CMP 0x19/JGE (0<=idx<0x19) ; MOV ECX,[ESP+8] ;
//   LEA EAX,[EAX+EAX*4] ; SHL EAX,4 (idx*0x50) ; MOV [EAX+0x68b1b0],ECX ; MOV EAX,1 ; (else XOR EAX,EAX)
//
// FUN_00459540 (0x00459540): Pool-J per-slot 2-field reset `void fn(idx)`.
//   MOV EAX,[ESP+4] ; IMUL EAX,0x58 ; MOV [EAX+0x68ba08],0 ; MOV [EAX+0x68ba00],0xffffffff

#include "../Core/HookSystem.h"

#include <cstdint>

// 0x00458fa0
extern "C" __declspec(dllexport) std::uint32_t __cdecl TtlSet68b1b0(std::int32_t idx, std::uint32_t val) {
    if (idx >= 0 && idx < 0x19) {                                                       // TEST/JL ; CMP 0x19/JGE
        *reinterpret_cast<std::uint32_t*>(0x0068b1b0u + static_cast<std::uint32_t>(idx) * 0x50u) = val;  // SHL 4 (idx*0x50)
        return 1;                                                                       // MOV EAX,1
    }
    return 0;                                                                           // XOR EAX,EAX
}
RH_ScopedInstall(TtlSet68b1b0, 0x00458fa0);

// 0x00459540
extern "C" __declspec(dllexport) void __cdecl PoolJReset68ba00(std::uint32_t idx) {
    *reinterpret_cast<std::uint32_t*>(0x0068ba08u + idx * 0x58u) = 0;                   // [EAX+0x68ba08],0
    *reinterpret_cast<std::uint32_t*>(0x0068ba00u + idx * 0x58u) = 0xffffffffu;         // [EAX+0x68ba00],0xffffffff
}
RH_ScopedInstall(PoolJReset68ba00, 0x00459540);
