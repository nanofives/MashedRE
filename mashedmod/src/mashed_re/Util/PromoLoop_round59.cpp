// Mashed RE — promote-round round 59 (two get-and-return-with-ptrout accessors).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Both are __cdecl, self-contained (zero callees), byte-verified from the listing.
// Same record base 0x0063e4b8 + idx*0x24 (idx*9 dwords), different field offsets.
//
// FUN_00420da0 (0x00420da0):
//   8b442404      MOV EAX,[ESP+4]          ; idx
//   8b4c2408      MOV ECX,[ESP+8]          ; out
//   85c9          TEST ECX,ECX
//   8d04c0        LEA EAX,[EAX+EAX*8]      ; idx*9
//   8d0485b8e46300 LEA EAX,[EAX*4+0x63e4b8] ; addr = 0x63e4b8 + idx*0x24
//   7404          JZ  +4                    ; out==0 -> skip store
//   8b10 8911     MOV EDX,[EAX] ; MOV [ECX],EDX   ; *out = *(u32*)(addr+0)
//   8b4008        MOV EAX,[EAX+8]           ; return *(u32*)(addr+8)
//
// FUN_00420dc0 (0x00420dc0): identical, but *out = *(addr+4) and return *(addr+0xc).

#include "../Core/HookSystem.h"

#include <cstdint>

// MATCH-S05: the body of 0x00420da0 (TblRetPtrout63e4b8_0) moved to Match/MatchS05_00420da0.cpp,
// which compiles the byte-matched source re/match/s05/00420da0.c.

// MATCH-S05: the body of 0x00420dc0 (TblRetPtrout63e4b8_4) moved to Match/MatchS05_00420dc0.cpp,
// which compiles the byte-matched source re/match/s05/00420dc0.c.
