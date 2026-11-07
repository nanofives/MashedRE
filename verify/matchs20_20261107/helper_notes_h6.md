# MATCH-S20 h6 notes (round 2 near-miss pass, 2026-11-07)

Scratch: D:/MashedWT/MATCH-S20_data/scratch/h6/ (variants, permuter logs, micro tests).

## Findings

- 0x004cbe80: the round-1 file (78.6%) is NOT behaviour-faithful. Its default case calls FUN_004d8480(&err) before
  it fills err; the original fills {1, FUN_004d7ff0(0xe)} first. That reorder is what the permuter finds (it does it
  again from a faithful start). Every faithful store order (16 combinations over the 4 RWERROR blocks) tops out at
  71.7% (scratch/h6/cbe80_keep.c), because this compiler cross-jumps the file-case and default RWERROR tails into
  one block. The original keeps them apart (lea ecx in one, lea eax in the other). Not overwritten (the brief allows
  overwrite on a higher score only); the caller should pick the faithful 71.7% form for the port.
- 0x004d1e30: 87.5 -> 87.9 (written). `#define RwRasterGetFormat(r) ((unsigned int)(((r)->cFormat) << 8))` plus
  levels = `(~RwRasterGetFormat(raster) >> 15) & 1` makes case 5 emit the original's `xor edx, edx; mov dh, [esi+0x23];
  not; shr 0xf; and 1`. Case 0/4 stays 1 byte short (`mov al; test al, 0x60` against `mov ah; test ah, 0x60`), so
  every later jump offset is off by one. Micro tests (scratch/h6/micro*.c): this compiler keeps `mov ah` for a
  `<< 8` value only when the whole value has another use (an argument, or a value computed before the switch and
  used in two cases); a value used only in a mask test folds to the byte. The permuter got 97.4% with `hr > -1`
  for `hr >= 0` (one byte longer, re-aligns the offsets), not kept: it can never reach 100%.
- 0x004d1640: the original has the byte-identical block `or esi, 0x200; pop edi; mov eax, esi; pop esi; add esp, 8;
  ret` twice (0x004d19c8 and 0x004d1b6d). `return format | 0x200` gives `mov eax, esi; or ah, 2`; every form that
  routes the 16-bit case through `format |= 0x200` gets cross-jumped into the 0x004d1b6d copy (547/553 ins).
  Tried break, else-if chain, combined condition, `return (format |= 0x200)`, result local, param_2 reuse, int
  format, the whole case 5 as a __forceinline helper, /Zi /Z7 /Zd, permuter 189 compiles.
- 0x004caea0: the zero-register split survives int/unsigned/float/short/pointer types on every global, `i = 0`
  forms, all 120 orders of the five statement groups before the pool test, permuter 205 compiles.
- 0x004cecb0: case order does not matter at all (all 24 orders of the 4 case groups give the same object). Loop forms,
  goto-tail, no-break default, local image pointer for the tail: no change.
- 0x004d3e90, 0x004cc050: callee-saved push placement (shrink-wrap) differs; if/else and break forms, fp local
  placements, permuter: no gain. 0x004d3e90 permuter 83.3% only with `numNodes > i - 1` (changes behaviour).
- 0x004c9fb0: a result variable puts the zero in ecx (76.0%); a device local fixes the device-load slot but rotates
  registers (61.2%, regs 89.9%). Flags /G3 /G4 /G5 /G6 /GS /Ob2 swept on 4 files: nothing above the current best.
- 0x004cf7d0: typed RwGlobals struct with dOpenDevice at +0x10, 4 device positions: the natural position gives
  `mov esi, [g]; add esi, 0x10` (77.7%), never `mov edx, [g]; lea esi, [edx + 0x10]`; permuter from that form 82.3%.

## Result

0x004cbe80  445  78.6%  93.1%  cxx:ok  b  file unchanged but its default case is not faithful (FUN_004d8480 before err is filled); faithful forms max 71.7% (cross-jumped RWERROR tails), scratch/h6/cbe80_keep.c
0x004caea0  300  77.5%  86.2%  cxx:ok  b  zero-register split (eax for 4578/457c/4580/4584 + loop offset, ebx elsewhere) survives types, 120 statement orders, permuter
0x004cc050  250  78.5%  81.7%  cxx:ok  b  push placement: original pushes esi at entry and edi only in the file case, ours the reverse; RW break form, fp placements, permuter no gain
0x004c9fb0  424  81.3%  85.6%  cxx:ok  b  original preloads xor eax, eax in the prologue for both early exits; result variable lands in ecx (76.0%), device local rotates registers
0x004cf7d0  514  84.2%  86.7%  cxx:ok  b  natural device position gives mov esi,[g]; add esi,0x10 against mov edx,[g]; lea esi,[edx+0x10] plus rotation (77.7%); file keeps h1's entry form
0x004d3e90  546  82.8%  86.6%  cxx:ok  b  ebp pushed at entry in the original, inside the locked path here; if/else forms, loop forms, permuter (83.3% only with a behaviour change)
0x004cecb0  468  79.6%  79.6%  cxx:ok  b  layout only: shared flag tail after the default case in the original, after the 0x420/0x820 loop here; case order has no effect (24 orders), loop/goto forms no change
0x004d1640  1735  92.4%  92.4%  cxx:ok  b  one instruction (mov eax,esi; or ah,2 against or esi,0x200; mov eax,esi); break/else forms get cross-jumped into the identical block at 0x004d1b6d
0x004d1e30  1270  87.9%  87.9%  cxx:ok  b  87.5 -> 87.9: unsigned RwRasterGetFormat + (~fmt >> 15) & 1 gives case 5's mov dh; case 0/4 still mov al/test al (1 byte short, all later offsets shift)
