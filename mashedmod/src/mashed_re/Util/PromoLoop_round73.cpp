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

// MATCH-S07: the body of 0x0042c1a0 (StateAdvance2to3) moved to Match/MatchS07_0042c1a0.cpp,
// which compiles the byte-matched source re/match/s07/0042c1a0.c.

// MATCH-S08: the body of 0x00432290 (Trigger432290) moved to Match/MatchS08_00432290.cpp,
// which compiles the byte-matched source re/match/s08/00432290.c.
