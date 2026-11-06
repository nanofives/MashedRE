# MATCH-S19 helper h4 notes

FLAG FINDING (h4, 2026-11-02): RW-core functions in this slice match with `/O2 /GS /Ob1 /G5` (also /G3 or /G4,
same result), not the game flags alone. flagscan over the slice's existing s19 files: 0x004c0740, 0x004c0ad0,
0x004c0b10, 0x004c0b70, 0x004c1a10, 0x004c1bb0, 0x004c1be0, 0x004c1c80, 0x004c1cf0, 0x004c2de0, 0x004c2e40,
0x004c2ea0, 0x004c2f00 go to 100% with /G5 unchanged source; none of the 100% files drops. My files carry
`// FLAGS: /O2 /GS /Ob1 /G5`. Symptoms /G5 fixes: param loaded before global loads, byte RMW split
(mov al/and al/mov), loop-head alignment pad, push/prologue scheduling.

0x004c5800  26  100.0  100.0  cxx:?  -  /G5; raw body
0x004c5830  26  100.0  100.0  cxx:?  -  /G5; raw body
0x004c5860  47  100.0  100.0  cxx:?  -  if/return-0/return-p shape
0x004c5890  152  100.0  100.0  cxx:?  -  /G5; RW tex-dict create macros
0x004c59c0  64  100.0  100.0  cxx:?  -  /G5; RW ForAll loop
0x004c5c80  21  100.0  100.0  cxx:?  -  /G5; raw body
0x004c7730  47  100.0  100.0  cxx:?  -  /G5; slot local + ternary
0x004c75e0  27  100.0  100.0  cxx:?  -  /G5; returns p (Ghidra said void)
0x004c7620  36  100.0  100.0  cxx:?  -  /G5; raw-shaped body
0x004c5ae0  105  100.0  100.0  cxx:?  -  RwError local struct, >= 32 test
0x004c5b50  105  100.0  100.0  cxx:?  -  RwError local struct, >= 32 test
0x004c5bc0  58  100.0  100.0  cxx:?  -  RW link macros, returns texture
0x004c5c00  115  100.0  100.0  cxx:?  -  /G5; __forceinline case-insensitive compare helper
0x004c5930  133  89.1  91.3  cxx:?  b  push esi shrink-wrapped around the loop in ours, 5 forms + permuter
0x004c76c0  40  100.0  100.0  cxx:?  -  /G5; address-taken param as out slot
