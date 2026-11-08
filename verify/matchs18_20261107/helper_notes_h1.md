# MATCH-S18 helper h1 (2026-11-07)

Shapes that moved the score here:
- RW callback iterators (0x004b4010, 0x004b41b0/41f0/4230/4280): an 8-byte ctx struct {key, out*} and the result
  initialized in its declarator (`undefined4 result = 0;`). As a separate statement the compiler reuses param_2's
  stack slot (33%). One form covers all five.
- 0x004b40c0 returns the count (Ghidra: void). 0x004b4150: 4-byte RwRGBA default {ff,ff,ff,ff} (byte stores).
- 0x004b3e40: the two params copied into one 8-byte {start,length} struct local.
- 0x004b5750: `1.0f / (divisor)` gives fld divisor; fdivr [1.0] (the extern MGLOBAL form gives fld const; fdiv).
- 0x004b42c0 / 0x004b4430 (look-at on a local matrix / a matrix pointer): same x87 residual in both; product
  operand order in source never changes the emitted order (the compiler canonicalizes).

0x004b3bb0  55  100  100  cxx:ok  -  pointer types, one reused local for both callee results
0x004b3e40  83  100  100  cxx:ok  -  8-byte struct local for the two copied params
0x004b4010  55  100  100  cxx:ok  -  ctx struct, result = 0 in its declarator
0x004b4050  29  100  100  cxx:ok  -  r = 0 kept in esi, assigned in the if
0x004b4080  53  100  100  cxx:ok  -  r = 0 before the param test, callback as function pointer
0x004b40c0  39  100  100  cxx:ok  -  returns the count (Ghidra void)
0x004b40f0  44  100  100  cxx:ok  -  callback as function pointer, one && test
0x004b4130  14  100  100  cxx:ok  -  tail call with rewritten arg 1
0x004b4150  80  100  100  cxx:ok  -  RwRGBA-shaped default {ff,ff,ff,ff}, struct copy loop
0x004b41b0  60  100  100  cxx:ok  -  4010 form + if (param_1 != 0) around the iterator
0x004b41f0  60  100  100  cxx:ok  -  same form as 41b0
0x004b4230  60  100  100  cxx:ok  -  same form as 41b0
0x004b4280  60  100  100  cxx:ok  -  same form as 41b0
0x004b42c0  356  89.0  89.0  cxx:ok  b  x87 scheduling: up-cross products vs normalize arg pushes, fstp st(0) placement
0x004b4430  178  81.9  63.9  cxx:ok  b  x87 operand order in 4 of 12 cross products + fstp st(0) placement
0x004b5750  623  81.6  94.3  cxx:ok  b  frame slot order of the two RwInt16 offset locals vs the int conversion temp (eax/ecx flip follows)
