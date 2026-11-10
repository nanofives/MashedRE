# RESULT MATCH-RACEPATH-BIG (race-path big functions by byte match), 2026-11-09

**Verdict: GREEN. 0x00446520 closed (94.9% / 2,725 bad B -> 100% / 0 B, 7,412 B BYTE-IDENTICAL) and promoted to C4
by route match. 0x00443dc0 not closed (57.5% / 2,206 B, source unchanged).**

Branch `orch/MATCH-RACEPATH-BIG`, base `orch/integration` @ 3fbf77214. Every score uses `re/tools/match/match.py`
(XP DDK cl 13.00.9176 / c2 13.00.9178) with the file's own line-1 FLAGS (`/Od` for 0x00446520, unchanged). No
`__asm`, no naked body, no pragma added. No mashedmod/ edit, no game run, no Ghidra use, no tool code change.
`grep -rn "00446520.c" mashedmod/src`: 0 hits, so the port does not include the source and the default gates are
unaffected (not run). `grep "00443dc0" mashedmod/src` hits only comments in `Ai/AiNavHooks.cpp` /
`Ai/AiLineOfSight.cpp`, no include, and that source was not changed.

| rva | before | after | source |
|---|---|---|---|
| 0x00446520 | insn 94.9%, 2,725 of 7,418 B differ | insn 100%, 7,412 B BYTE-IDENTICAL, relocheck OK (223 checked) | `re/match/s08/00446520.c` |
| 0x00443dc0 | insn 57.5%, 2,206 of 3,228 B differ | unchanged | `re/match/s08/00443dc0.c` |

## 0x00446520: first-divergence analysis

1. **One 6-byte growth explained 2,509 of the 2,725 bad bytes.** The first byte difference was at +0x2f1, and the
   instruction listing was aligned everywhere except one statement: `vcl = (float)(acos(vay) * 57.2958f);
   vay = 90.0f - (180.0f - vcl);`. Ours: `fstp [ebp-0x1d0]; fld [180.0]; fsub [ebp-0x1d0]`. Original:
   `fst [ebp-0x1d0]; fsubr [0x5cd09c]` (vcl stays in ST0). Ours was 6 B longer, so every later byte compared
   against the wrong position, and every relative jump across that point differed too (the `jmp 0x1cf6` vs
   `0x448210` rows). Written as one assignment expression, `vay = 90.0f - (180.0f - (vcl = (float)(acos(vay) *
   57.2958f)));`, the symbol is 7,412 B (the original's length) and the bad bytes drop to 216.
2. **The remaining 216 B were the old "residual (a)", 16 statements, all one shape.** The original loads the
   `param_1->field` operand into ST0 before the frame operand (`fld [ecx+0x9a4]; fmul [ebp-0x11c]`,
   `fld [eax+0x964]; fsubr [ebp-0xf4]`, `fld [edx+0x970]; fadd [ebp-0x20]` for `+=`, and for field + field it
   loads the left field before computing the right field's address). The previous slice tried operand order,
   `(*p).f`, `p[0].f`, casts, `+=`, `(&p->a)->x`, `-(p - w)` and attributed it to a 9176/9466 difference.
   9176 reproduces it: written as an rvalue, `(0, param_1->f)`, the field is evaluated into ST0 first. The value
   and the type are the same; only the lvalue-ness of the operand changes. Applied to the 16 statements
   (3 `vln += vah * f9a4`, 9 `vbr = v - p->a`, 6 `p->a += vbr` as `p->a = (0, p->a) + vbr`, 6 `vbr = p->a + p->b`):
   0 differing bytes.
   Tried and not used: the pilot's double intermediate (it fixes the field + field and `+=` forms, but in
   frame-op-field it moves the frame operand first, and `(float)(double)p->f` gives the right order with one
   extra spill and a 4 B larger frame), `(float)p->f`, `+p->f`, `*(volatile float *)&p->f`, `(&p->a)->x`,
   `(&p->a.x)[0]`, `-p->a + v` forms (fchs), and compiling as C++ (`// LANG cpp`: no change on these statements
   and it breaks the source's finding 2, ST0 kept across statements: 91.5%).
3. **Extent.** `hooks.csv` and pc_funcs give 7,411 B. The original bytes at 0x00446520+7405 are
   `83 c4 04 8b e5 5d c3 cc cc cc cc`: the `ret` is byte 7,412 and `cc` padding follows, so the compiled
   symbol's 7,412 B is the function's real extent and no extent override was needed. The instruction compare is
   1751/1751 and `c4_reclass.py` reports `byte-identical 7412 B`.

Gate: `c4_reclass.py batch --tsv c4_batch.tsv` dry run 1 PASS (`c4_dryrun.tsv`), then `--apply --result` this file.
relocheck OK, 223 checked; the 5 unchecked names (`__CIatan`, `__CIcos`, `__CIsin`, `__ftol`, `_acos`) are CRT
intrinsics, as in the base source.

## 0x00443dc0: what remains (not closed, 17 compiles)

`/O2 /GS /Ob1`, 912 of 969 instructions, 26 structural clusters. Three independent residue classes:

1. **Frame layout**: ours `sub esp, 0x1ac`, original `0x1a0`, and most `[esp+x]` operands differ (col at +0xc vs
   +8, best +0x54 vs +0x18, tmp +0x78 vs +0x6c). Unlike /Od, renaming a local (5 names for `best`) or moving its
   declaration moves no slot at /O2 (`variants/00443dc0_v5.py`, all identical to the base). The slot assignment is
   optimizer-driven, so this needs a different lever (which temporaries exist and their lifetimes).
2. **Duplicated y-range compares** in draws 1 and 2: the original tests a.x, b.x, a.y, b.y ranges and then the
   a.y / b.y group a second time (12 compares, 0x444239 / 0x444368 fail targets). 9176 drops the second group
   entirely for every semantics-preserving spelling tried: `&&` chain, nested if, `__inline` helper, macro, and
   alias access paths `((float *)&a)[1]`, `(&a.x)[1]`, `(&a)->y`, `*(float *)((char *)&a + 4)` (`v6`, `v7`; all
   byte-identical to the base). A semantics-changing control (different constant in group 3) does emit it, so the
   fold is a redundancy elimination, and the open question is what source form 9176 does not see as redundant.
3. **Loop entry after draw 2**: the original enters the 16-step look-ahead loop with a `jmp` into a rotated body
   and copies bi / t / one more value into fresh registers and slots first (`mov ebx, esi; mov [esp+0x28], ebp;
   ... jmp 0x444390`); ours falls into the body.

Next slice for 0x00443dc0: class 2 first (it removes 20 instruction rows and changes the following block
layout), then re-measure class 3, then the frame.

## Files

| file | what |
|---|---|
| `results.tsv` | per-function before/after, verdict, residual, compiles, sha256 (pilot columns) |
| `c4_batch.tsv`, `c4_dryrun.tsv` | promotion batch and gate dry run |
| `relocheck.tsv` | relocheck of the changed source |
| `variants/` | the variant specs run (`00446520_v1..v4.py`, `00443dc0_v5..v8.py`) and the pilot's `tryv.py` used to run them |
