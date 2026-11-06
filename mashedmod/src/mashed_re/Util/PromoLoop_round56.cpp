// Mashed RE — promote-round round 56 (one per-vehicle vec3 getter + three 2-index wheel getters).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// All four are __cdecl, self-contained (zero callees), byte-verified from the listing.
//
// FUN_0046bd20 (0x0046bd20): per-vehicle vec3 getter `u32 fn(out_ptr, idx)`.
//   idx [ESP+8], CMP 0x10 / JC ; out [ESP+4] ; IMUL 0xd04 ;
//   MOV [EAX+0x8820a0]->out[0], +0x8820a4->out[1], +0x8820a8->out[2] ; MOV EAX,1
//   (base 0x008820a0, n=3, stride 0xd04, bound 0x10) — same shape as round 54.
//
// FUN_0046d320/d360/bd60: 2-index wheel getters `u32 fn(out_ptr, i1, i2)`.
//   i1 [ESP+8], CMP 0x10 / JNC oob ; i2 [ESP+0xc], CMP 4 / JC body (else oob->0) ;
//   IMUL EAX,i1,0x11 ; ADD EAX,i2 ; out [ESP+4] ; IMUL EAX,0xc4 ;
//   MOV [EAX+base]->*out ; MOV EAX,1
//   Byte-verified base:  0046d320: 0x00881790   0046d360: 0x00881738   0046bd60: 0x00881744

#include "../Core/HookSystem.h"

#include <cstdint>

// MATCH-S11: the body of 0x0046bd20 (VehTbl8820a0Get3) moved to Match/MatchS11_0046bd20.cpp,
// which compiles the byte-matched source re/match/s11/0046bd20.c.

// MATCH-S11: the body of 0x0046d320 (Idx2Wheel881790Get) moved to Match/MatchS11_0046d320.cpp,
// which compiles the byte-matched source re/match/s11/0046d320.c.

// MATCH-S11: the body of 0x0046d360 (Idx2Wheel881738Get) moved to Match/MatchS11_0046d360.cpp,
// which compiles the byte-matched source re/match/s11/0046d360.c.

// MATCH-S11: the body of 0x0046bd60 (Idx2Wheel881744Get) moved to Match/MatchS11_0046bd60.cpp,
// which compiles the byte-matched source re/match/s11/0046bd60.c.
