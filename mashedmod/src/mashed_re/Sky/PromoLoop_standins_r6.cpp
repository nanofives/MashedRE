// Mashed RE: stand-in promotion round 6 (plan item #3 step 2, 2026-10-11): 0x004880a0 and its caller 0x004881d0.
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// 0x004880a0  Obj7030acCreate  (FUN_004880a0, 295 B, cdecl(tex) -> *(0x007dc57c); caller 0x004881d0)
// Frame F = esp after `sub esp, 0x10; push esi`; locals F+4..F+0x10 = (0, 0, 0, 0x41200000) (0x004880b7..0x004880cf);
// tex = F+0x18 (0x00488183). p(obj) below is *(obj + *(0x007dc57c)), re-read from both globals at every store as the
// original does.
//   0x004880a4  frame = 0x004c0b30()
//   0x004880d7  obj = 0x00534b60(0x2000, 0x10000087, 0);  0x004880e2 [0x007030ac] = obj
//   0x004880ea  [p+0xa4] = 5;  0x00488102 [p+0xa8] = 2;  0x0048811a [p+0x40] |= 0x10000000;  0x0048812f [p+0x40] |= 0x800000;
//   0x00488145  [p+4] = 0x200
//   0x00488154  0x004e7e30(obj, frame)
//   0x00488159..0x00488180  [[[obj]+0x18]+0x5c]+4 .. +0x10 = the four locals   (obj re-read from 0x007030ac)
//   0x0048818a  tex != 0: byte [tex+0x50] = 2; 0x004881a3 0x004e8090(*[[obj+0x18]+0x20], tex)
//   0x004881b9  [p+0xac] = 1;  EAX on return = *(0x007dc57c) (0x004881ab)

#include "../Core/HookSystem.h"

#include <cstdint>

// MATCH-S14: the BuildInto helper both old bodies shared was removed with them (no other user).

// MATCH-S14: the body of 0x004880a0 (Obj7030acCreate) moved to Match/MatchS14_004880a0.cpp,
// which compiles the byte-matched source re/match/s14/004880a0.c.

// MATCH-S14: the body of 0x004881d0 (Obj7030a8Create) moved to Match/MatchS14_004881d0.cpp,
// which compiles the byte-matched source re/match/s14/004881d0.c.
