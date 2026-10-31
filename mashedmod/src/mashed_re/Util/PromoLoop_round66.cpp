// Mashed RE — promote-round round 66 (per-player flag-bit toggle + gated switch predicate).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Both are __cdecl, self-contained (zero callees), byte-verified from the listing.
//
// FUN_0041ef80 (0x0041ef80): per-player flag-bit toggle.
//   MOV EAX,[ESP+4] ; MOV ECX,[ESP+8] ; IMUL EAX,0x2ac ; ADD EAX,0x63d9e0
//   TEST ECX,ECX ; MOV ECX,[EAX+0x294] ; JZ clear
//   OR ECX,0x400 ; MOV [EAX+0x294],ECX ; RET          ; set != 0
//   clear: AND ECX,0xfffffbff ; MOV [EAX+0x294],ECX   ; set == 0
//   -> flag = *(u32*)(0x0063dc74 + idx*0x2ac) (0x63d9e0+0x294); set ? |=0x400 : &=~0x400
//
// FUN_0041f360 (0x0041f360): gated membership predicate.
//   if (*(int*)0x00636ad0 == 0) switch(arg){ 4,5,8,10,0xb,0xe,0xf,0x25,0x26,0x29,0x2b,0x2c,0x2f,0x30: return 1; }
//   return 0;

#include "../Core/HookSystem.h"

#include <cstdint>

// MATCH-S04: the body of 0x0041ef80 (FlagToggle63dc74) moved to Match/MatchS04_0041ef80.cpp,
// which compiles the byte-matched source re/match/s04/0041ef80.c.

// [rtex 2026-10-16] GatedSwitch636ad0 (FUN_0041f360) moved, unchanged for the .asi, to
// Vehicle/CarMaterialTables.cpp, which both targets compile (the standalone's car env set-up
// calls it; one body per RVA, build.bat rva-lint).
