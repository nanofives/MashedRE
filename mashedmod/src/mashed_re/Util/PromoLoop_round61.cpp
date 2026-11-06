// Mashed RE — promote-round round 61 (global-indexed float getter + vec16 copy setter).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Both are __cdecl, self-contained (zero callees), byte-verified from the listing.
//
// FUN_004077e0 (0x004077e0): global-indexed float getter (single FLD; x87-safe, no arithmetic).
//   a1d8a56300    MOV EAX,[0x0063a5d8]      ; idx global
//   69c0ec000000  IMUL EAX,EAX,0xec
//   d980e09d6300  FLD float ptr [EAX+0x639de0]
//   c3            RET                       ; returns *(float*)(0x00639de0 + idx*0xec)
//
// FUN_0046d4d0 (0x0046d4d0): vec16 copy setter `u32 fn(idx, in_ptr)`.
//   8b442404      MOV EAX,[ESP+4]          ; idx
//   83f810 7203   CMP EAX,0x10 ; JC body    ; idx >= 0x10 -> XOR EAX,EAX ; RET (0)
//   8b542408      MOV EDX,[ESP+8]          ; in_ptr
//   69c0040d0000  IMUL EAX,EAX,0xd04
//   LEA EDI,[EAX+0x881ec8] ; MOV ECX,0x10 ; REP MOVSD   ; copy 16 dwords in -> region1
//   LEA EDI,[EAX+0x881f08] ; MOV ECX,0x10 ; REP MOVSD   ; copy 16 dwords in -> region2 (contiguous, +0x40)
//   MOV EAX,1 ; RET

#include "../Core/HookSystem.h"

#include <cstdint>

// 0x004077e0
extern "C" __declspec(dllexport) float __cdecl FloatIdx639de0(void) {
    std::int32_t idx = *reinterpret_cast<std::int32_t*>(0x0063a5d8u);                   // MOV EAX,[0x63a5d8]
    return *reinterpret_cast<float*>(0x00639de0u + static_cast<std::uint32_t>(idx) * 0xecu);  // IMUL 0xec ; FLD [+0x639de0]
}
RH_ScopedInstall(FloatIdx639de0, 0x004077e0);

// MATCH-S11: the body of 0x0046d4d0 (VecCopy881ec8) moved to Match/MatchS11_0046d4d0.cpp,
// which compiles the byte-matched source re/match/s11/0046d4d0.c.
