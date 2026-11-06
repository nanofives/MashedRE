// Mashed RE — promote-round round 60 (4-way pointer-global selector).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// FUN_0045c640 (0x0045c640): __cdecl, self-contained. Sets two pointer-globals
// 0x0088fbc4 / 0x0088fbc8 to constant image VAs selected by param_1 (DEC/JZ chain).
// Byte-verified:
//   8b442404 48 7445   MOV EAX,[ESP+4] ; DEC ; JZ case1
//   48 742d            DEC ; JZ case2
//   48 7415            DEC ; JZ case3
//   default: MOV [0x0088fbc4],0x88f6b0 ; MOV [0x0088fbc8],0x88eba0 ; RET
//   case3:   MOV [0x0088fbc4],0x88eb80 ; MOV [0x0088fbc8],0x88f6c0 ; RET
//   case2:   MOV [0x0088fbc4],0x88f0b0 ; MOV [0x0088fbc8],0x88f0e0 ; RET
//   case1:   MOV [0x0088fbc4],0x88e67c ; MOV [0x0088fbc8],0x88e680 ; RET

#include "../Core/HookSystem.h"

#include <cstdint>

// MATCH-S10: the body of 0x0045c640 (Sel88fbc4) moved to Match/MatchS10_0045c640.cpp,
// which compiles the byte-matched source re/match/s10/0045c640.c. The 2026-07-27 register note
// (caller FUN_0045c6b0 carries its loop index in EDX across the call) moved with it.
