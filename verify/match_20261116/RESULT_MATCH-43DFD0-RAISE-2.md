# RESULT MATCH-43DFD0-RAISE-2 (slice 2 of 3, source lines 680-1509)

Row kind: match-only. No build, no gate, no game run, no Frida, no emulation of 0x0043dfd0 (L-168).
Base: `orch/integration` @ 7630e53ca. Branch `orch/MATCH-43DFD0-RAISE-2`.
Source: `re/match/s08/0043dfd0.c`, toolchain ddk (cl 13.00.9176 / c2 13.00.9178), `// FLAGS: /O2 /GS /Ob1`.
Final source sha256 `e212e26b11d8d7bb8c47730aa834df2dd1ad5c7a471b608b1ef05cb8fd3d3b1e`.

## Verdict

GREEN as a raise, not a match. Whole function 19.2% -> 29.3%. No byte-identical range is claimed, so no relocheck was run.

| | before (7630e53ca) | after |
|---|---|---|
| match.py whole function | 19.2% (regs 31.7%, ops 59.1%), 2829/2817 ins | 29.3% (regs 46.3%, ops 72.0%), 2828/2817 ins |
| match.py `--bytes` | 11106 B whole symbol, 7009 differing unmasked bytes (first at +0x2) | 11074 B whole symbol, 6999 differing unmasked bytes (first at +0x2) |
| slice 2 (original instructions matched / total) | 46/991 = 4.6% | 326/991 = 32.9% |

The slice-2 score counts the original instructions in `[0x0043e8cc, 0x0043f775)` (instruction index 606..1597) that sit in
a matching block of match.py's own alignment (`difflib.SequenceMatcher` over the normalized lists from `score_function`).
Tool: `verify/match_20261116/slice43_slice2.py <src> [--dump <file>]`, run from the worktree root.

## Address range of slice 2

- Source line 680 `switch(DAT_0067e9fc)` = original `0x0043e8cc mov ecx,[0x67e9fc]` / `0x0043e8de jmp [eax*4+0x440aac]`.
- Source line 1510 `switch((&DAT_0067ed7c)[(DAT_0067e9f8-2)*0x10])` = original `0x0043f775` / `0x0043f78b jmp [edx*4+0x440ad0]`.
- Slice 2 = original `0x0043e8cc..0x0043f775`.

## Changes (all inside source lines 680-1509, base line numbers 696..1052)

1. **Case order of `switch(DAT_0067e9fc)`.** Jump table at `0x00440aac` (cases 2..10):
   `0x43e8e5, 0x43e96c, 0x43e96c, 0x43e96c, 0x43f108, 0x43f108, 0x43f108, 0x43f108, 0x43e946`.
   Block order in the original: case 2 `0x0043e8e5`, case 10 `0x0043e946`, cases 3-5 `0x0043e96c`, cases 6-9 `0x0043f108`.
   Case 10 ends `0x0043e967 jmp 0x43f112`, and cases 6-9 fall into `0x0043f112 mov [0x67ea78],ebx`.
   The source had case 10 last. Moved it after case 2 with `goto LAB_0043f112`, and put the label inside the 6-9
   `if (iVar8 == 2)` block. Effect alone: 19.2% -> 25.1% (slice 4.6% -> 21.2%).
2. **Team-slot loops** (5 copies: `uVar10` 0x80000, 0x50000, {0x20000,0x30000,0xb0000,0x60000}, {0,0x10000,0xc0000},
   {0x90000,0xa0000}; original at `0x0043eb35`, `0x0043ec58`, `0x0043eddf`, `0x0043ef3a`, `0x0043f06b`).
   The original (0x80000 copy) reads the count then advances the pointer before the `if` (`0x0043eb35 mov ecx,[edi]`,
   `0x0043eb37 add edi,4`, `0x0043eb3a xor eax,eax`, `0x0043eb42 mov ebx,edi`), walks with `edi` and compares
   against `[ebx]`, `[ebx+4]`, `[ebx+8]` with `DAT_007f1a1c` held in `edx` (`0x0043eb76`, `0x0043eb88`), and its search
   loop exits through `0x0043eba2 mov ecx,eax` (found) or falls to `0x0043eba4` (not found).
   Rewritten as: `iVar8 = *piVar18; piVar18++; iVar9 = 0; local_2c = 0; if (0 < iVar8) { piVar13 = piVar18; ... iVar14 = *piVar18; piVar18++; if (iVar14 == DAT_007f1a1c) for (i = 0; i < 6; i++) if (i != DAT_007f1a1c && i != piVar13[0] && i != piVar13[1] && i != piVar13[2]) { iVar14 = i; break; } ... } while (iVar9 < iVar8);`
   Effect: 25.1% -> 25.6%.
3. **Base pointer of those loops.** `piVar18 = (int *)(iVar8 + DAT_0067f17c * 0x30)` -> `piVar18 = (int *)FUN_00413f90(); piVar18 = piVar18 + DAT_0067f17c * 0xc;`.
   The original keeps the call result in `edi` first (`0x0043eb0f mov edi,eax`, `0x0043eb16 lea edx,[eax+eax*2]`,
   `0x0043eb1e shl edx,4`, `0x0043eb21 add edi,edx`). After the change these 8 instructions match in all 5 copies
   (equal blocks at `0x0043eb0a`, `0x0043ec2d`, `0x0043edb4`, `0x0043ef0f`, `0x0043f040`). Effect: 25.6% -> 29.3%.

Variant tried and dropped: hoisting `piVar13 = piVar18` above `if (0 < iVar8)` (the original's `0x0043eb42 mov ebx,edi`
sits before the `jle` at `0x0043eb48`) gave 23.1% before step 3 and 29.1% with it (slice 33.3%), below 29.3%.

## Residual in slice 2 (out of budget)

Function-wide register and frame choice, not slice-local shape:
- The original holds 0xffffffff in `ebp` (`0x0043e8fd mov [0x7f1a24],ebp`, restored by `0x0043ebbb or ebp,0xffffffff`)
  and builds 0 locally per branch (`0x0043e9b8 xor esi,esi`, `0x0043ea3a`, `0x0043eabc`). This source pins 0 in `ebp`,
  0xffffffff in `ebx` and 1 in `edi` for the whole function, and stores 1 from `edi` where the original stores the literal
  (`0x0043e9c7 mov [0x67ea74],1`).
- Frame: original `sub esp,0x2c` at `0x0043dfd0`, this source `sub esp,0x18`, so every stack slot offset differs
  (`[esp+0x14]` / `[esp+0x10]` at `0x0043eb3e` / `0x0043eb44` against `[esp+0x1c]` / `[esp+0x18]` here).
- `piVar17` base: the original uses `0x0043eb4a mov ebp,0x7f1a2c` with `[ebp-8]`, `[ebp]`, `[ebp-4]`. This source
  produces `&DAT_007f1a2c - 4` in a register with offsets `-4`, `+4`, `0`.

## Checks

- `py -3.12 re/tools/match/match.py re/match/s08/0043dfd0.c --bytes` before and after (table above).
- C++ side: `cl /nologo /Zs /EHsc /std:c++17 /I.. /I..\.. ProvS08_0043dfd0.cpp` (VS2022 19.44, syntax only, no
  object, no link) in `mashedmod/src/mashed_re/Match`: exit 0. `ProvS08_0043dfd0.cpp` includes this source under
  `MATCH_PORT` (PROVISIONAL.tsv row 0x0043dfd0, self-test only).
- Diff touches only base lines 696..1052. Slice 1's flag declarations (lines 263-269) and the header comment are untouched.
  The header comment's score (19.2%) is stale after this branch; left for the lead to update after the three slices merge.

## [UNCERTAIN]

[UNCERTAIN] The three rewrites preserve the function's behaviour. This rests on reading the source: case 10 still
reaches `DAT_0067ea78 = 0`; the search loop returns the same `iVar14` (compare base moved from `piVar18+1` to
`piVar13 = piVar18` after the increment, so `piVar13[0..2]` = old `piVar18[1..3]`); `iVar9`, `piVar13` and `piVar18`
are reassigned before any later read (source lines 1057, 1172, 1247, 1251, 1255). Test: `re/tools/match/equiv.py` on this
source against 7630e53ca's, or the knob-path menu walk ON against OFF named in PROVISIONAL.tsv. Not run here (match-only
row, L-168). Registered in `re/orchestrator/UNCERTAIN_REGISTER.md`.

No promotion: the function is not byte-identical, so no `re/promotions/*.tsv`.
