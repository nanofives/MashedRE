# RESULT MATCH-RACEPATH-FRESH2 (FRESH's 4 unworked rows + its 3 partials), 2026-11-09

**Verdict: GREEN. 3 of 6 worked rows closed (50%), all 3 promoted to C4 by route match.** One partial moved
(0x0048ab70 41 to 34 B), one fresh row kept as a partial above its start (0x00412620 17.6% to 49.4%), one partial
unchanged (0x0040aef0), one not worked (0x00411ae0).

Branch `orch/MATCH-RACEPATH-FRESH2`, base `orch/MATCH-RACEPATH-FRESH` @ 636b61e23. Every score uses
`re/tools/match/match.py` (XP DDK cl 13.00.9176 / c2 13.00.9178) with the file's line-1 FLAGS, `/O2 /GS /Ob1`. No
`__asm`, no naked body. The only pragma is `#pragma intrinsic(sin[, cos])`, which selects the fsin/fcos intrinsic
(the binary has `fsin` inline at 0x0048b506). No mashedmod/ edit, no game run, no Ghidra use (the decomp is FRESH's
JSON, the original listing is capstone over `original/MASHED.exe.unpatched` via FRESH's `tools/odis.py`), no change
to `re/tools/match/` code. Default gates not run: nothing outside `re/match/racepath/` and this folder changed.

## Yield

Worked: 0x0048aff0, 0x0048b230, 0x0048b440, 0x00412620, 0x0048ab70, 0x0040aef0 (6). After 6 worked, 3 closed, so the
yield rule did not stop the slice. 0x00411ae0 was not worked: FRESH's 110 compiles covered the term orders and widths
for its sum, and no new lever from the other slices applies to integer reassociation.

| rva | subsystem | before | after | source |
|---|---|---|---|---|
| 0x0048aff0 | particle | 43.8% raw g2s | 576 B BYTE-IDENTICAL | `re/match/racepath/0048aff0.c` |
| 0x0048b230 | particle | 23.4% raw g2s | 519 B BYTE-IDENTICAL | `re/match/racepath/0048aff0.c` |
| 0x0048b440 | particle | 33.3% raw g2s | 524 B BYTE-IDENTICAL | `re/match/racepath/0048aff0.c` |
| 0x00412620 | render | 17.6% raw g2s | 49.4% insn, 444 B differ | `re/match/racepath/00412620.c` (new, kept) |
| 0x0048ab70 | particle | 41 B differ (93.2% insn) | 34 B differ (91.2% insn) | `re/match/racepath/0048ab70.c` (kept) |
| 0x0040aef0 | gameplay | 12 B differ | 12 B differ | unchanged |

0x0048b440 scores insn 96.8% only because the Ghidra body (483 B) stops before the epilogue. The whole-symbol compare
(524 B, the switch table at 0x0048b62c included) is identical and `c4_reclass.py` accepts it. On 0x0048ab70 the byte
count fell while the insn score fell too: the new form matches the x87 order and store slots, and every vertex-store
displacement is now off by 4 (pointer base, below), which the insn compare counts per line.

The three particle rows share one file and one declaration block (the puff pool 128 x 0x34 B at 0x007206f0 with its
bitmap at 0x007200c0, the blast records 8 x 0xa4 B at 0x007201d0). Each function is byte-identical in that combined
TU, and also was in its own file before the merge. 0x0048ab70 stays in FRESH's file: it walks a different pool
(0x00715230, 512 x 0x34 B).

## What moved the closed rows

1. **FRESH's scan, reused verbatim (0x0048aff0, 0x0048b440).** The two-loop bitmap walk from 0x0048ab70 with
   `next: i = k*8+8` at the bottom of the inner loop and the 8-way fallthrough switch matched both rows at the first
   compile. 0x0048aff0 searches for a clear bit: `if (m != 0xff) break;` then `m = ~m` before the switch (the `not cl`
   at 0x0048b03c sits between the `cmp edx,7` and the `ja`), and the 128 exit jumps to the fallback
   (`goto full`, 0x0048b008). 0x0048b440 searches for a set bit like 0x0048ab70.
2. **Literals read from the image.** 0.14f 0x005cf4a0, 1.6f 0x005cf49c, 0.7f 0x005cc340, 36.0f 0x005cda5c,
   3.1415927f 0x005ce2f4, 1.0f 0x005cc320, 0.75f 0x005cc950, 255.0f 0x005cd04c, 0.0f 0x005d757c. With them
   0x0048b230 was byte-identical at its first compile.
3. **Store shape (0x0048aff0, 13 B to 0).** The source writes the +0x2c field before +0x30 (the binary stores +0x30
   first, 0x0048b1d3 / 0x0048b1db, so the register choice follows the source order, not the binary order), and the
   position as a struct copy `p->pos = *pos`, which moves the `add esp,0x14` after the z load (0x0048b202).
   Semantics: two stores to distinct fields from by-value params, and a 12-byte copy of the same three floats.
4. **A double product kept on the x87 stack (0x0048b440, the only residue).** `t = age * f2c` declared `double`:
   the compiler then keeps `t` in st(1) and loads each velocity, `fld vel.x; fmul st(1)` (0x0048b52b, 0x0048b538),
   and pops it with `fstp st(0)` after the z line. With `float t` it consumed t on the z line. A named float local
   for vel.x / vel.z (X87A transform F) gives the same bytes. The value is not stored between the product and its
   two uses, so the binary computes it in extended precision either way.
5. **Faithful oddities kept as in the binary.** 0x0048aff0's walk does not stop at the first clear slot: after a hit,
   the `jg` at 0x0048b0ab returns to the scan at 0x0048b003, so it sets the bit of every clear slot it meets and keeps
   the last one (`while (i > 0)`). Its fallback is a max of +0x28 over 128 slots written as one plain loop (cl
   unrolls it by 8, 0x0048b0c5..0x0048b17b). 0x0048b440 resets both size floats to 0.5f at the loop head
   (0x0048b450) on every iteration.

Guard checks: no closed row has a `qword` operand (0x0048aff0 / 0x0048b230 / 0x0048b440 are byte-identical and the
original has none in these ranges). The `#pragma intrinsic(sin)` gives the `fsin` instruction, not a call.

## Not closed

- **0x00412620 (render, 49.4%, 444 B).** Two structural differences. (1) The original keeps the counter i and the
  count n in memory ([esp+0] / [esp+4]; i = 0 is stored before the 0x0041f300 call, 0x00412625) and walks three
  pointer IVs: esi = slot, edi = &slot.p0, ebx = &slot.p1 (0x0041264a..0x00412654). Ours drops i, counts n down in
  memory and walks one pointer (base slot+0x28), deriving &p0 / &p1 with `lea`. Tried: index, slot pointer, p0/p1
  pointer locals, unsigned counters, unsized array, separately aliased p0/p1 arrays, while / do-while / for forms, a
  forceinline per-slot helper, volatile counter: none changes the loop shape. (2) cl reuses the `dx * 100.0f` product
  for `dx * -100.0f` (an `fsub`), where the original keeps dx / dy on the x87 stack and multiplies by the -100.0f
  literal (0x00412769, 0x00412777). Named dx/dy and inline subtraction forms both CSE. What is known: the f20 test is
  `!(f20 <= 1e-4f)` (gives the `jnp` at 0x004126b3; `f20 > 1e-4f` gives `jne`), the clamp against 5.0 is a double
  compare (`fcom qword [0x5cd030]`) followed by the 5.0f literal (0x005cc358), DAT_005f2a40 is writable .data.
- **0x0048ab70 (34 B).** a.x, a.y, a.z in natural order plus `d.y = (float)((double)scale * vel.y)` fixed the d.y load
  order (`fld scale; fmul vel.y`, 0x0048ac5c) and the a.y slot. Residual: the vertex pointer base is EBP = &a.y, the
  original's is &a.z (0x0071ba38), so every vertex-store displacement differs by 4; the col.g / col.b byte stores are
  scheduled before the a.x `fstp` (the original puts them after it); and the pos.x fadd order
  (`fld vel.x; fadd pos.x`, 0x0048accd). FRESH's x,z,y order gives base &a.z but breaks the a.y slot. Tried 9 orders
  of the 10 vertex stores with a.x/a.y/a.z in order (none gives base &a.z), d.y as W/V/F/comma, pos.x as E/F/comma/`+=`.
- **0x0040aef0 (12 B, unchanged).** Arguments as comma expressions `FUN_004c51a0(m, (v.x = ..., &v), 2)`: 16 to 21 B.
  Constant-first products, long double, pointer and temp forms on both y lines: 12 to 16 B. The residue is still the
  `fld` of m->pos.y / v.y one slot after the next `push` (0x0040afbe, 0x0040b020).
- **0x00411ae0 (27 B):** not worked (see Yield).

## Files

| file | what |
|---|---|
| `results.tsv` | per-row before/after, verdict, residual, approximate compile count, sha256 |
| `c4_batch.tsv`, `c4_dryrun.tsv` | promotion batch and gate dry run (3 PASS, 0 refused) |
| `relocheck.tsv` | relocheck of the closed source (3 functions, OK; `__ftol` is the one unchecked symbol) |

Raw data kept off the repo (`D:/MashedWT/fresh2_data/`): capstone listings `0048aff0.dis`, `0048b230.dis`,
`0048b440.dis`, `00412620.dis`, and every variant compiled (`try/`, `try/var/`). Inputs reused from FRESH:
`D:/MashedWT/fresh_decomp.json` (SHA-256 `96d59509a33a0046cedabec1ebd3f941081a9711725110a5a4fe779c0bf08f1d`).
Helper scripts: FRESH's `verify/matchracepath_20261109/fresh/tools/tryv.py` and `odis.py`, unchanged.

## Next slice

- 0x00412620: the loop shape is the blocker. Next things to try: the Xbox twin (`re/tools/console/xtwin.py
  0x00412620`) for the loop form, and the sibling builder FUN_00412190 (same 0x2c stride) for how the source indexes
  the slot array.
- 0x0048ab70: find the source form that biases the vertex pointer to &a.z while keeping x,y,z store order.
