// Mashed RE — promote-round round 75 (global-field getter + float-threshold predicate).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Both are __cdecl, self-contained (zero callees), byte-verified from the listing.
//
// FUN_004495d0 (0x004495d0): global-field getter + constant.
//   MOV EAX,[0x00896278] ; MOV EAX,[EAX+0x4] ; ADD EAX,0x40
//   -> return *(int*)(*(int*)0x00896278 + 4) + 0x40
//
// FUN_0044e020 (0x0044e020): per-record float threshold predicate.
//   MOV EAX,[ESP+4] ; IMUL EAX,0xf8 ; FLD float [EAX+0x8900b0] ; FCOMP float [0x5ce2d8] ;
//   FNSTSW AX ; TEST AH,5 ; JP ret0 ; MOV EAX,1   (x87 "below" test -> matches C++ < for finite floats)
//   -> return (*(float*)(0x008900b0 + idx*0xf8) < *(float*)0x005ce2d8) ? 1 : 0

#include "../Core/HookSystem.h"

#include <cstdint>

// MATCH-S08: the body of 0x004495d0 (GlobalFieldGet896278) moved to Match/MatchS08_004495d0.cpp,
// which compiles the byte-matched source re/match/s08/004495d0.c.

// MATCH-S08: the body of 0x0044e020 (FloatLt44e020) moved to Match/MatchS08_0044e020.cpp,
// which compiles the byte-matched source re/match/s08/0044e020.c.
