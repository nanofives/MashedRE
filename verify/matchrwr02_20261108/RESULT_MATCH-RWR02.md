# RESULT MATCH-RWR02 (MATCH-RWLIB-2 slice R02)

Status: **GREEN, honest partial**. Match-only: no build, no game, no gates, no tracker edits, no promotion.
Compiler: XP DDK cl 13.00.9176 / c2 13.00.9178 via `re/tools/match/match.py`, file flags `/O2 /Ob1 /G5` on every row.
All 11 rows below are plain C, no `__asm`, no naked function, and `re/tools/match/relocheck.py` reports OK for each
(`relocheck.tsv`). `results.tsv` covers all 79 rows (the 9 no-source rows read `no source`), with the round-1 best in
`before%`.

## Counts

- Rows at 100% byte match with relocheck OK: **11 / 79** (before 0 / 79).
- Two came from the session before the reboot (0x004e67b0, 0x004e8940), nine from this resume.
- Raised but not at 100%: 0x004ebb00 83.3 -> 98.1, 0x004f2e70 89.0 -> 91.0, 0x004e6710 and 0x004e6760 54.8 -> 87.1.

## Per-row before / after (rows touched)

| rva | size | before | after | what moved it |
|---|---|---|---|---|
| 0x004e67b0 | 201 | 97.1 | 100 | +0x64 list head init written after the +0x6c store (pre-reboot) |
| 0x004e8940 | 82 | 92.3 | 100 | one `return param_1` at the end with if/else stores (pre-reboot) |
| 0x00538310 | 329 | 95.5 | 100 | extent read through a `float *a = p + 0x1c` box pointer, length stored through a local |
| 0x004e8750 | 443 | 91.1 | 100 | `count += n` then reload; the source wrote entry +0x18 and a doubled +0x14 offset, now +0x14/+0x18 of the entry |
| 0x004ebc30 | 196 | 89.3 | 100 | two call arms (type 8 / type 1) instead of a shared call; vtable slot +8 is `__stdcall` (no `add esp` after it in the original) |
| 0x005327d0 | 212 | 87.3 | 100 | the condition tests the global just stored (`!DAT_007dc540`), not the local |
| 0x004f2f90 | 151 | 87.3 | 100 | `for` list walk with the match path inside; the source had `param_5 + param_6*4`, now `param_6 + param_5*4` (arr[index], as the original's `[ecx+edx*4]`) |
| 0x004e6f80 | 37 | 37.5 | 100 | returns the list owner (Ghidra typed it void; eax = param 1 at ret), RW link-insert statement order |
| 0x00535910 | 34 | 27.3 | 100 | returns param_1 (Ghidra void; the original saves esi to keep eax) |
| 0x004e89a0 | 62 | 76.7 | 100 | count in a local, `return world` inside the loop |
| 0x004e8ea0 | 165 | 76.5 | 100 | `rc++; rc--;` before the lock-all `flags |= 0xfff` (the original reads and rewrites +0xe around the or), one permuter swap |
| 0x004ebb00 | 296 | 83.3 | 98.1 | plain `for` loops and one return |
| 0x004f2e70 | 273 | 89.0 | 91.0 | `for` loop with a typed triangle array |
| 0x004e6710 | 67 | 54.8 | 87.1 | RW `while (cur != end)` with early return |
| 0x004e6760 | 67 | 54.8 | 87.1 | same as 0x004e6710 (RpClumpForAllLights) |

## 100% rows (sources and sha256)

- `re/match/rwl02/004e67b0.c` ba8f83d95bc965b2b6aefb0b12a4c1de3d155bac63c311a944b6da493e5c0444
- `re/match/rwl02/004e8940.c` f2eefa66758c38daa226eddaa08cf74e399256e29025cac5982c1fcec01e7ae8
- `re/match/rwl02/004e8750.c` fe0f09268b92f65226963375e90a9561a2b2f3eff60ed212a69a7e2421e654c0
- `re/match/rwl02/004ebc30.c` f3ba73503466e28544e85ff527aec0d7517701b3c2f3d8c50daaf121ae2ff10b
- `re/match/rwl02/004e89a0.c` 125146bd3432de356c595780748137de162dff6dda0bea1bc99f925b145db4b5
- `re/match/rwl02/004e8ea0.c` 96cfd57e53cd79dc9825fc2380af318b06ae841118ffca1c91342dc1421ded45
- `re/match/rwl02/004e6f80.c` 325a92c5cd103a9e96c555fa952244b827df1fdbc741882104454b65b7ce629f
- `re/match/rwl03/00538310.c` 0553af2735ff55c475f94857ecef38704f7e74833474615544dd1d9a04a90adb
- `re/match/rwl03/004f2f90.c` 27efd11269eb90c43abeca6b1f228a452c38e1e34db5bb37395581cde8dd5638
- `re/match/rwl03/005327d0.c` d55939325236d4336552621d5db806c4211fb445472ac6a1350375944cb72534
- `re/match/rwl03/00535910.c` bf432f8aff106bddf08de10cd728ca714d2d3c1b4a9a92d5b8a2f9bdadb7da4a

Note for the C4R batch: 0x00538310 keeps the round-1 `volatile` reads on the second box copy (`b2[k] = *(volatile int
*)&param_2[k]`). They read the same values as plain reads (nothing writes `param_2` in between), and without them the
row drops to 60.0%. 0x004e8ea0's `rc++; rc--;` is a net identity, and the original does read and rewrite +0xe.

## Findings for the other slices

1. **Return inside the loop** (0x004e89a0): `for (...) { if (!cb(...)) return world; } return world;` gives the
   original's register priorities (callback and data in callee-saved registers, count spilled). `break` then one return
   does not. Same family as R04's explicit return inside the if block.
2. **retscan pays off on small rows**: 0x004e6f80 and 0x00535910 were Ghidra `void` and went from 37.5 / 27.3 to 100 by
   returning param 1.
3. **Ghidra/g2s sources can be semantically wrong in address operands**: 0x004e8750 and 0x004f2f90 had swapped or
   doubled index terms. Read the original's `[base+index*4]` and fix the C before shaping.
4. **`__stdcall` vtable slots**: a `push eax; call [ecx+8]` with no `add esp` is a COM-style Release, declare the
   pointer `__stdcall`.
5. **Refcount bump**: a load and store of the same field around another store is `x++ ... x--`, which the compiler
   folds to a reload and rewrite.

## Open rows worth a follow-up (do not repeat)

- **Shrink-wrap** 0x004e6920 / 0x004e7cc0 / 0x004e6470 (90.9 / 90.9 / 90.7): R04's return-in-if lever does not apply
  (no early return in the body). Duplicating the tail into the if block gives 71-73%, a `static __inline` helper for
  the +0x18 block stays at 90.9, and `/O1 /Os /Oy- /Ob0 /GX /Gy` flags do not move it. 0x004f3be0 / 0x0052fdb0 untouched
  this round (pre-reboot attempts listed in git history of this file).
- **List terminator CSE** 0x004e6710 / 0x004e6760 (87.1) and 0x004e6650 (70.0): the original loads `first` through
  the object pointer and forms `end` with a separate `lea`; ours folds both into one register. Tried: end variable,
  terminator in the loop condition, copy of the object pointer, RwLinkList pointer, char-offset load, struct-typed list.
- **Early-fail returns** 0x0052fe90 (87.8): the original keeps one epilogue per failing call and returns eax without
  `xor`. 12 shapes (nested, flat, result variable, ternary, C++ compile) all merge the returns. Same open class as R01
  0x004e5d30 and R04 0x00551c40.
- 0x004f0ae0 (81.8, ops 100%): esi/edi swap. Declaration order, size placement, RW early-error layout and the permuter
  (516 compiles) do not move it.
- 0x004ebb00 (98.1): the original loads param_3 into eax and forms the count with `lea ecx,[eax-2]`. Count variants
  (int/uint, param mutation, param as loop counter) do not move it.
- 0x004f2e70 (91.0, regs 98): eax/ecx/edx choice at the free-list call inside the loop.
- 0x004e5fc0 (77.3): x87 stack allocation of the scaled temps. Its source also has a broken flag clear
  (`*(unsigned int *)0xfffffffd & (param_1 + 0x4c)`). The correct form `&= 0xfffffffd` scores 75.5, so the file was
  left unchanged. Fix it before any promotion attempt.
- Not reached: the rows below 77% other than those above, and the 9 no-source rows (0x0052df20, 0x00536780, 0x00536e90,
  0x00536fe0, 0x00537b20, 0x00537d20, 0x00537d70, 0x00537e60, 0x005422c0). No Ghidra slot was taken this round.

## Tools and raw (not committed)

D:/MashedWT/MATCH-RWR02_data/tools/ (vt.py variant runner, collect.py, retscan.py), D:/MashedWT/MATCH-RWR02_data/specs/
(variant specs), D:/MashedWT/MATCH-RWR02_data/scratch/ (variant sources), D:/MashedWT/MATCH-RWR02_data/collect2.log.
Git Bash gotcha: `match.py --flags "/O2 ..."` needs `MSYS_NO_PATHCONV=1`.
