# RESULT MATCH-NM-5441F0-4C4A50: compiler shaping for near misses 0x005441f0 and 0x004c4a50 (2026-11-13)

Row kind: match-only (no build, no gates, no game run, no game lock). Opus 5.5 high, claude3 then claude2.
Toolchain for both: rw9044 (VC6 SP5 cl 12.00.8804 + Processor Pack c2 13.00.9044), the recorded tag in each source.

## Verdict

| rva | path | before | after | result | equiv.py (tool eee2fc626) |
|---|---|---|---|---|---|
| 0x005441f0 | race_load | 98.7 | **99.1** (regs 99.1, ops 99.6) | still near | timed out after 50 min (no verdict): unproven refusal |
| 0x004c4a50 | race_run | 97.2 | 97.2 (unchanged) | still near | NOT-PROVEN, x87-region (refusal, see below) |

No byte match, so no `re/promotions/MATCH-NM-5441F0-4C4A50.tsv` and no `c4_reclass.py` dry run.
Neither body is admitted: no ADMITTED.tsv row. Both best sources are committed. 0x005441f0 stays provisional
(PROVISIONAL.tsv binds the lifted body RwCore_Lifted_NN.cpp), 0x004c4a50 stays provisional (PROVISIONAL.tsv, re/match/s19/004c4a50.c).

## 0x005441f0 (re/match/rwtb/005441f0.c, FLAGS /O2 /Ob1 /G5)

**Moved 98.7 -> 99.1.** The format-3 string copy written as an index loop
`for (param_2 = 0; param_2 < piVar8[2]; ...)` over `piVar5[param_2]` / `piVar9[param_2]` closes the
`mov ebp,eax; sub ebp,esi` residue: c2 strength-reduces the two bases into one pointer plus a difference by itself, and
then forms the difference the original's way. A do/while with the same body also gives 99.1. The previous
session's index-form loop (92.0) had also rewritten the memcpy source and dropped `piVar9 = (int *)piVar9[3]`;
keeping those as they are is what makes it work.

**Residue (2 instructions).** In the copy loop preheader the original computes `lea edx,[esi+edi+4]`
(param_2 + param_3 + 4, the source array base, spilled to `[esp+0x1c]`) before `xor ebp,ebp` (the 16-byte offset,
spilled to `[esp+0x18]`), and stores edx before ebp. Here the pair comes out the other way round. Tried:

- the base as a split variable (`param_2 + 4`, `param_2 + param_3`, `param_3 + param_2` with `[1]`) at 5 positions
  (before the malloc, before the `*piVar2` store, before `iStack_c = 0`, before the `if`, after `iStack_8 = 0`):
  99.1, or 54-56 at the `if` position;
- the full sum `param_2 + param_3 + 4` as an int or `int *` variable at the same 5 positions: the order flips the
  original's way, but c2 then gives ebp the job of a zero register for the whole function (param roles swap, 68.9 / 63.1);
- the full sum inline in the loop (4 operand orders): 62.7; index-form outer loop (`iStack_c * 0x10`): 62.7;
- swapping the `iVar7` / `iStack_8` assignments: 99.1;
- `match_permute.py` on the 99.1 source, 4 rounds + 400-step walk: 2606 compiles, no change.

## 0x004c4a50 (re/match/s19/004c4a50.c, FLAGS /O2 /GS /Ob1 /G5)

**Residue (same as MATCH-RWT-B).** The x87 instruction stream is identical. The original places the `combineOp` load,
`sub eax,0`, the three `pos = 0` stores and `flags = 3` inside the x87 chain (after `fsub [esp+0x10]` for right.z and
after `fsub st(1)` for up.x). Here c2 emits all six after the chain, right before the `je`.

**Findings this session (all 97.2 or worse):**

- The pos/flags block at each of 25 statement positions: 97.2 every time, the stores always land at the end.
- `int op = combineOp;` at each of 29 positions, `switch (op)`: no change.
- The comma rvalue is not the cause. All 32 on/off combinations of the 5 comma sites, plus 9 other wrappers
  that block the operand swap (`*&`, `(float)`, unary `+`, volatile read, `(double)`, `(1, x)`, `(sine, x)`):
  every form that yields the original's x87 order puts the integer block at the end (left listing line 85), and every
  form with any swapped product puts it at the top (line 9, after the first `fmulp`). The placement follows the x87
  order. No form reaches the original's line 64.
- C++ front end (`// LANG cpp`, c1xx): identical output (92.9 / 97.2, same placements).
- Reordering the FP statements (per-component scale, block orders, 9 matrix-store orders incl. diagonal-first and
  column-major, 8 adjacent swaps, 12 random orders): the x87 stream follows source order, 59.4-95.8.
- Flag sets (`flagscan.py`, 19 sets, rw9044): 97.2 best (/O2, /Ox, /G5, /G7, /GB, /Ob1, /Ob2, /Oi-, /QIfi, /GX, /GS).
- `match_permute.py`, 4 rounds + 600-step walk, seed 7: 3803 compiles, no change.

**equiv.py:** `0x004c4a50 NOT-PROVEN x87-region: region ours +0x0 / orig 0x004c4a50: integer code moved across x87
instruction 49 (fsub m4[esp+0x10]): [] | ['mov', 'mov', 'mov', 'sub']`. The difference is the order of integer code
against the x87 chain, which the admission rule never admits (WORKER_RULES: x87 instruction order is never admitted).
The body stays provisional (`PROVISIONAL.tsv`, unchanged).

## Checks run

- `py -3.12 re/tools/match/match.py re/match/rwtb/005441f0.c` -> 99.1% (225/225 ins, rw9044).
- `py -3.12 re/tools/match/match.py re/match/s19/004c4a50.c` -> 97.2% (212/212 ins, rw9044).
- `py -3.12 re/tools/match/equiv.py re/match/s19/004c4a50.c` -> NOT-PROVEN x87-region (line above, about 1 min).
- `py -3.12 re/tools/match/equiv.py re/match/rwtb/005441f0.c` -> **timed out after 50 min (no verdict)**. Started 03:14,
  CPU-bound for the whole run (about 2480 CPU-seconds), stopped at 04:04 by PID (21168 and its two parents) on the lead's
  instruction. A restart at 03:54 was stopped at 04:04 too (PIDs 17180, 9980). No partial output, and the run did not
  end on the explorer cap (`Explorer.MAX_VISITS` = 4000, which raises NOT-PROVEN too-many-iterations). Where the time went
  is [UNCERTAIN]: a `--trace` run would show it. Treated as an unproven refusal: no ADMITTED.tsv row.

## Next

0x005441f0 needs a lever for the preheader order that does not turn on the zero register. 0x004c4a50 needs the
c2 rule that pulls integer code into the x87 latency slots. All placements seen so far are line 9 or line 85, the
original is at line 64. Both stay on the knob path as provisional bodies.
