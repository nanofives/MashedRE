// Mashed RE — promote-round round 54 (one stride-0x58 getter + four per-vehicle table getters).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// All five are __cdecl, self-contained (zero callees), byte-verified from the listing.
//
// FUN_0045a0d0 (0x0045a0d0): signed-bounds stride-0x58 getter.
//   8b442404      MOV EAX,[ESP+4]              ; i
//   85c0 7c0f     TEST EAX,EAX ; JL  out       ; i < 0       -> 0xffffffff
//   83f804 7d0a   CMP EAX,4    ; JGE out        ; i >= 4      -> 0xffffffff
//   6bc058        IMUL EAX,EAX,0x58
//   8b8004ba6800  MOV EAX,[EAX+0x68ba04]
//   c3 / 83c8ff c3  RET / OR EAX,-1 ; RET
//
// FUN_0046d660/6a0/6d0/700: per-vehicle table getters `u32 fn(out_ptr, idx)`:
//   8b442408      MOV EAX,[ESP+8]              ; idx (2nd arg)
//   83f810 7203   CMP EAX,0x10 ; JC body        ; idx >= 0x10 -> XOR EAX,EAX ; RET (0)
//   8b4c2404      MOV ECX,[ESP+4]              ; out_ptr (1st arg)
//   69c0 040d0000 IMUL EAX,EAX,0xd04            ; per-vehicle byte stride 0xd04
//   8b90 <base>   MOV EDX,[EAX+base]  ; MOV [ECX],EDX   (repeat for each dword)
//   b801000000 c3 MOV EAX,1 ; RET
// Byte-verified (base, dword count n):
//   0046d660: base 0x00881f50 n=3
//   0046d6a0: base 0x008820ac n=1   0046d6d0: base 0x00881f84 n=1
// (0x0046d700 base 0x00881f68 n=3 is ALREADY C3 = VehicleVec3At9C8Get — excluded.)

#include "../Core/HookSystem.h"

#include <cstdint>

// 0x0045a0d0
extern "C" __declspec(dllexport) std::uint32_t __cdecl Table68ba04Get(std::int32_t i) {
    if (i >= 0 && i < 4) return *reinterpret_cast<std::uint32_t*>(0x0068ba04u + i * 0x58u);  // IMUL 0x58 ; MOV [EAX+0x68ba04]
    return 0xffffffffu;                                                                       // OR EAX,-1
}
RH_ScopedInstall(Table68ba04Get, 0x0045a0d0);

// Per-vehicle table getter: out[0..n-1] = *(u32*)(base + idx*0xd04 + j*4); idx>=0x10 -> return 0.
template <std::uint32_t Base, int N>
static inline std::uint32_t VehTblGet(std::uint32_t* out, std::uint32_t idx) {
    if (idx > 0xfu) return 0;                                  // CMP 0x10 / JC ; XOR EAX,EAX
    std::uint32_t b = idx * 0xd04u;                            // IMUL 0xd04
    for (int j = 0; j < N; ++j)
        out[j] = *reinterpret_cast<std::uint32_t*>(Base + b + j * 4u);
    return 1;                                                  // MOV EAX,1
}

// 0x0046d660
extern "C" __declspec(dllexport) std::uint32_t __cdecl VehTbl881f50Get3(std::uint32_t* out, std::uint32_t idx) { return VehTblGet<0x00881f50u, 3>(out, idx); }
RH_ScopedInstall(VehTbl881f50Get3, 0x0046d660);

// 0x0046d6a0
extern "C" __declspec(dllexport) std::uint32_t __cdecl VehTbl8820acGet1(std::uint32_t* out, std::uint32_t idx) { return VehTblGet<0x008820acu, 1>(out, idx); }
RH_ScopedInstall(VehTbl8820acGet1, 0x0046d6a0);

// 0x0046d6d0
extern "C" __declspec(dllexport) std::uint32_t __cdecl VehTbl881f84Get1(std::uint32_t* out, std::uint32_t idx) { return VehTblGet<0x00881f84u, 1>(out, idx); }
RH_ScopedInstall(VehTbl881f84Get1, 0x0046d6d0);
