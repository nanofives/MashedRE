# RESULT POWERUPINIT-B2 (2026-11-10): 0x00455400 stays a stand-in. Residual = instruction order + recompute vs IV reuse

Verdict: **no byte match. The residual is not register-field-only.** It is instruction order (two hoisted/IV
instructions placed differently) plus a recompute: the original reloads the counter and recomputes `i*0x6c` with
`imul` for three stores, where ours reuses the strength-reduced induction variable. Per the brief, 0x00455400 stays
`PortTop_StandIn` (slot 2). There is no admission request. relocheck BAD is explained: it is positional after a −7 B
length change, and every relocation target agrees on the aligned streams. 0x0045a3a0 is skipped (already admitted
and merged). Text and scripts only.

## 1. Match attempt

- **Source:** `re/match/s29/00455400.c`, DDK `/O2 /GS /Ob1`. `94.0% (regs 97.0%, ops 98.5%) 131/133 ins`, 442 B whole
  symbol, 45 differing unmasked bytes (positional).
- **Every variant below stayed at the same 94.0% / 45 bytes unless noted** (scripts in the session scratchpad):

| lever | result |
|---|---|
| RESULT_POWERUPINIT-B "next try": `static __inline` helper for the three trailing stores, counter by value | 94.0%, 45 B |
| the same with the array base passed in, and with a record pointer passed in | 94.0%, 94.0% |
| the whole missile half in an `__inline` helper, counter by value | 34.6% (88/133 ins, body broken) |
| RESULT_POWERUPINIT-B "next try": the counter's address escapes before the loop (`int *volatile esc = &i`) | 68.1% |
| block-scope copy `k = i` for the trailing stores | 94.0% |
| trailing stores as char-offset expressions | 94.0% |
| trailing stores moved into the loop condition with the comma operator (`while (s1, s2, s3, i != 0)`), as `for` increment, or chained `=` | 94.0% each (cl emits the same code) |

Those come on top of B's tries: a second array symbol, record pointers at top / middle / tail, a launcher pointer, a
second counter, three loop forms, the tail moved first, and `match_permute.py` (450 compiles).

## 2. Full byte-diff table

Script: `verify/powerupinit_b2_20261110/bytetable_00455400.py`. Output:
`verify/powerupinit_b2_20261110/bytetable_00455400.txt`.

How it works:
- Each resolvable relocation in our obj is written as its resolved target, and both sides are disassembled at
  0x00455400.
- The two streams are aligned with difflib on mnemonic + operands. Jumps are keyed by function offset, and `.rdata`
  string pushes as `<STR>`.
- Raw bytes are compared on every aligned pair.

Totals: ours 442 B / 131 instructions, original 449 B / 133. 125 aligned pairs are equal, operands and relocation
targets included.

| # | ours | original | label |
|---|---|---|---|
| 1-2 | +0x060 `be dc 00 00 00` mov esi, 0xdc (after `mov edi, 0x21c`) | 0x0045545b same bytes, before `mov edi, 0x21c` | **order**: the launcher-IV init (5 x 0x2c) is scheduled after the missile-IV init (5 x 0x6c) in ours, before it in the original |
| 3-4 | +0x076 `83 ef 6c` sub edi, 0x6c (after `sub esi, 0x2c`) | 0x00455473 same bytes, before `sub esi, 0x2c` | **order**: the two IV decrements are swapped |
| 5 | +0x181 `cmp esi, ebp`, then `mov [edi+0x688400]`, `mov [edi+0x688410]`, `mov [edi+0x6883d8]` (each `89 af ..`) | 0x00455581 `8b 44 24 14` mov eax, [esp+0x14], `6b c0 6c` imul eax, eax, 0x6c, `3b fd` cmp edi, ebp, then the three stores through `eax` (`89 a8 ..`) | **recompute vs IV reuse**: the original reloads the spilled counter and recomputes `i*0x6c` for missile +0x48 / +0x58 / +0x20, and its loop test uses the missile IV `edi`. Ours stores through the missile IV and tests the launcher IV `esi`. The store targets are equal (0x688400 / 0x688410 / 0x6883d8). +7 B in the original. |
| 6 | +0x195 `0f 85 ca fe ff ff` jne 0x455465 | 0x0045559c `0f 85 c3 fe ff ff` | knock-on of row 5: same target, rel32 shifted by 7 |
| 7-8 | +0x1a0 / +0x1aa `e8 ..` call 0x4e6e00 | 0x004555a7 / 0x004555b1 | knock-on of row 5: same target, rel32 shifted by 7 |

Nothing else differs.

**Value equivalence:** at row 5, `edi` holds `i*0x6c` (decremented together with `i` at row 3), so ours and the
original write the same three addresses with the same value (`ebp` = 0). Even so, the code differs in instructions,
not only in register fields: an extra load, an `imul`, a different loop-test IV, and moved instructions.

## 3. Why relocheck is BAD

```
0x00455400 BAD checked=31 unchecked=0 ... +0x185 _DAT_006883b8: ours 0x00688400 vs orig 0x3b6cc06b; +0x18b ... vs
orig 0x688400a8; +0x191 ... vs orig 0x688410a8; +0x1a1 _FUN_004e6e00: ours 0x004e6e00 vs orig 0x2491e1a4; +0x1ab ...
```

relocheck reads the original at our relocation offsets. From +0x181 on, the original is 7 bytes longer (row 5), so
the five sites after it read mid-instruction bytes. For example, `0x3b6cc06b` is `6b c0 6c 3b` = the original's `imul`
plus the first byte of `cmp`. On the aligned streams:
- the three `_DAT_006883b8` targets 0x688400 / 0x688410 / 0x6883d8 equal the original's displacements (row 5);
- both `_FUN_004e6e00` targets equal 0x4e6e00 (rows 7-8);
- the other 26 sites lie before +0x181 and are equal.

BAD is a consequence of the residual. It does not show a wrong symbol.

## 4. Classification (per the brief)

The residual is **instruction order (rows 1-4) and a recompute vs induction-variable reuse (row 5)**. That is not
one register-field-only kind, so there is no admission request. 0x00455400 stays the slot-2 stand-in. Re-pickup
condition: a source form that makes cl keep the three trailing missile stores out of the missile IV. None of the
tries in section 1 does.

## Gates

- No build input changed.
- rva-lint: OK, 0 new.
