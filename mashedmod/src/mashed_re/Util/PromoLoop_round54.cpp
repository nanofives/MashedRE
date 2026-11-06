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

// MATCH-S09: the body of 0x0045a0d0 (Table68ba04Get) moved to Match/MatchS09_0045a0d0.cpp,
// which compiles the byte-matched source re/match/s09/0045a0d0.c.

// MATCH-S11: the body of 0x0046d660 (VehTbl881f50Get3) moved to Match/MatchS11_0046d660.cpp,
// which compiles the byte-matched source re/match/s11/0046d660.c.

// MATCH-S11: the body of 0x0046d6a0 (VehTbl8820acGet1) moved to Match/MatchS11_0046d6a0.cpp,
// which compiles the byte-matched source re/match/s11/0046d6a0.c.

// MATCH-S11: the body of 0x0046d6d0 (VehTbl881f84Get1) moved to Match/MatchS11_0046d6d0.cpp,
// which compiles the byte-matched source re/match/s11/0046d6d0.c.
