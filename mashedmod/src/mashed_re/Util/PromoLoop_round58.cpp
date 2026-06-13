// Mashed RE — promote-round round 58 (conditional getter + pointer-compute getter + equality predicate).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// All three are __cdecl, self-contained (zero callees), byte-verified from the listing.
//
// FUN_00472500 (0x00472500): conditional record getter (record at 0x00691500, stride 0x10).
//   8b442404      MOV EAX,[ESP+4]
//   c1e004        SHL EAX,4                 ; idx*0x10
//   8b8808156900  MOV ECX,[EAX+0x691508]    ; flag @ +8
//   85c9 7407     TEST ECX,ECX ; JZ slot0
//   8b8004156900  MOV EAX,[EAX+0x691504]    ; flag!=0 -> slot @ +4
//   c3 / ...      RET
//   8b8000156900  MOV EAX,[EAX+0x691500]    ; flag==0 -> slot @ +0
//
// FUN_0046d4a0 (0x0046d4a0): pointer-compute getter `u32 fn(out_ptr, idx)`.
//   idx [ESP+8], CMP 0x10 / JC ; IMUL EAX,0xd04 ; MOV ECX,[EAX+0x881f48] (stored index t)
//   SHL ECX,6 (t*0x40) ; LEA EDX,[ECX+EAX+0x881ec8] ; out [ESP+4] ; MOV [out],EDX ; MOV EAX,1
//   -> *out = 0x00881ec8 + idx*0xd04 + t*0x40,  t = *(u32*)(0x00881f48 + idx*0xd04)
//
// FUN_0045caf0 (0x0045caf0): equality predicate (table 0x007f1a18, stride 0x10).
//   CMP [0x008aa254],1 / JG ret0           ; gate: compute only when global <= 1 (< 2)
//   MOV ECX,[ESP+8] ; TEST ECX,ECX / JGE   ; p2 >= 0 required
//   MOV EAX,[ESP+4] ; SHL EAX,4 ; MOV EAX,[EAX+0x7f1a18]
//   SHL ECX,4 ; SUB EAX,[ECX+0x7f1a18] ; NEG ; SBB EAX,EAX ; INC EAX   ; (a==b) ? 1 : 0

#include "../Core/HookSystem.h"

#include <cstdint>

// 0x00472500
extern "C" __declspec(dllexport) std::uint32_t __cdecl CondGet691500(std::int32_t i) {
    std::uint32_t b = static_cast<std::uint32_t>(i) * 0x10u;                    // SHL EAX,4
    if (*reinterpret_cast<std::uint32_t*>(0x00691508u + b) != 0)                // flag @ +8
        return *reinterpret_cast<std::uint32_t*>(0x00691504u + b);              // slot @ +4
    return *reinterpret_cast<std::uint32_t*>(0x00691500u + b);                  // slot @ +0
}
RH_ScopedInstall(CondGet691500, 0x00472500);

// 0x0046d4a0
extern "C" __declspec(dllexport) std::uint32_t __cdecl PtrCompute881ec8(std::uint32_t* out, std::uint32_t idx) {
    if (idx > 0xfu) return 0;                                                   // CMP 0x10 / JC
    std::uint32_t b = idx * 0xd04u;                                            // IMUL 0xd04
    std::uint32_t t = *reinterpret_cast<std::uint32_t*>(0x00881f48u + b);       // MOV [EAX+0x881f48]
    *out = 0x00881ec8u + b + t * 0x40u;                                        // SHL 6 ; LEA +0x881ec8
    return 1;
}
RH_ScopedInstall(PtrCompute881ec8, 0x0046d4a0);

// 0x0045caf0
extern "C" __declspec(dllexport) std::uint32_t __cdecl EqPredicate7f1a18(std::int32_t p1, std::int32_t p2) {
    if (*reinterpret_cast<std::int32_t*>(0x008aa254u) < 2 && p2 >= 0)           // global < 2 ; p2 >= 0
        return (*reinterpret_cast<std::uint32_t*>(0x007f1a18u + static_cast<std::uint32_t>(p1) * 0x10u) ==
                *reinterpret_cast<std::uint32_t*>(0x007f1a18u + static_cast<std::uint32_t>(p2) * 0x10u)) ? 1u : 0u;
    return 0;
}
RH_ScopedInstall(EqPredicate7f1a18, 0x0045caf0);
