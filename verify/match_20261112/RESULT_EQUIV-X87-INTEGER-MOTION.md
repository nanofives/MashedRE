# RESULT EQUIV-X87-INTEGER-MOTION (L-147 (a), 2026-11-13)

Verdict: GREEN. Text/tool row: equiv self-test only, no build, no gates.

- Tool commit: `a9ca480f1` on `orch/EQUIV-X87-INTEGER-MOTION` (`re/tools/match/equiv.py`, `equiv_rules.md` s5 and s6,
  `equiv_tests/cases.asm`, `test_equiv.py`, `equiv_tests/refs/0048e820_held.c`).
- Self-test: `py -3.12 re/tools/match/test_equiv.py` (under test-slot) **63/63 passed** (output below).

## What changed in equiv.py

- `x87_rule` (called per region pair when either side has x87 code). Unchanged: x87 instructions must be the same text in
  the same order (L-126, x87 order never admitted). New: calls in such a region are hard barriers (NOT-PROVEN
  `x87-region`). The integer mnemonic multiset of the region must match. A moved integer instruction (its text is not in the
  other side's same segment) is checked against every x87 instruction between its segment and the farthest candidate
  partner segment (same mnemonic, the other side's surplus), on its own side's state. A dependency gives NOT-PROVEN
  `x87-motion` naming the instruction, the crossed x87 op and the reason. Rule tag `x87-motion` when a move was admitted.
- `Ctx.effects`: per-instruction registers read/written (capstone `regs_access`, full width, implicit included: `fnstsw ax`
  -> EAX, `fcomi`/`fcompi` -> EFLAGS, `sahf` reads EAX writes EFLAGS, push/pop ESP, address registers) and memory accesses
  (symbolic address on that side's state, size, written, private-A1-slot). The x87 store direction comes from `X87_STORE`
  (capstone 5 reports `fstp m32` as a read). String ops, pushad/popad/pushfd/popfd/leave/enter and unformable addresses
  count as all memory.
- `_x87_dep`: refuses a register write/any or any/write overlap, and any memory pair with a write that is not
  `disjoint(...)`. Read-read is allowed. A private frame/argument slot against a non-private access does not alias (A1, the
  split the proof already makes). The strict rerun marks nothing private, so a move needing A1 reports FRAME.
- Model fixes found on the way: `fcompi` / `fucompi` (capstone 5's spelling) now set the flags term like `fcomi` (they left
  the flags untouched before). `fcmov*` is NOT-PROVEN `unsupported` (the x87 term does not read EFLAGS).

## Self-test cases (new)

| case | expected | got |
|---|---|---|
| x87_move_indep (was neg_x87_sched: `mov eax,[esp+4]` crosses `fld [G1]`) | EQUIV, x87-motion | EQUIV [ORDER] |
| x87_move_rr (`mov eax,[G1]` crosses `fld [G1]`, read-read) | EQUIV, x87-motion | EQUIV [ORDER] |
| x87_move_frame (frame-slot load crosses `fstp [eax]`, A1) | EQUIV FRAME | EQUIV [ORDER,FRAME] |
| neg_x87_fnstsw (`mov eax` crosses `fnstsw ax`) | x87-motion | NOT-PROVEN x87-motion |
| neg_x87_sahf (`sahf` crosses `fnstsw ax`) | x87-motion | NOT-PROVEN x87-motion |
| neg_x87_alias (store [G3] crosses `fstp [G3]`) | x87-motion | NOT-PROVEN x87-motion |
| neg_x87_eflags (`xor ecx,ecx` crosses `fcomip`) | x87-motion | NOT-PROVEN x87-motion |
| neg_x87_slot (load [esp+4] crosses `fstp [esp+4]`) | x87-motion | NOT-PROVEN x87-motion |
| neg_x87_call (load crosses a call in an x87 region) | x87-region | NOT-PROVEN x87-region |

Reference row fix: `0x0048e820` failed the self-test with the unmodified tool too (it predates this row). Its live
`re/match/s15/0048e820.c` is byte-identical since 58cc241be (EQUIV-FU-48E820), so it no longer shows the EDI defect.
The reference now runs on a frozen copy of the pre-58cc241be source (`refs/0048e820_held.c`). Result: NOT-PROVEN
call-regargs, as recorded.

`0x00556e90` (frozen held 97.5% source): NOT-PROVEN `x87-motion`: ours `mov esi,m4[esp+0x18]` (slot esp0+0xc) moved
across `fstp m4[esp+0x14]` (the same slot, written). This is a real dependency, not a tool limit. Before this row it was
`x87-region`.

## The two L-147 rows, preview (tool at a9ca480f1, their branch sources, default mode)

Run here as information for the follow-up rows. Their admission, ADMITTED.tsv and caller scan are their own work.

- `0x00536e90` (`orch/MATCH-EQUIV-536E90:re/match/rwl03/00536e90.c`): **EQUIV [ORDER, FRAME]**, rules x87-barrier,
  x87-motion. Before the A1 split it was refused on `mov ebp,[esp+0x14]` (private slot esp0-0x1c) against
  `fstp [eax+4]` through a loaded pointer. That answers DECISIONS L-147's [UNCERTAIN] item for this row.
- `0x004c4a50` (`orch/MATCH-NM-5441F0-4C4A50:re/match/s19/004c4a50.c`): **EQUIV [LAYOUT, VALUE-FLOW]**, rules A1-exposed,
  A6-paired-pop, A6-pinned-pop, A7-stack-image, jmp-threading, x87-barrier, x87-motion.

Not done: `equiv_mutate.py` and the Unicorn cross-check (`equiv_dyn.py`) were not run (row scope: self-test only).
No promotions. Nothing C4 (L-128).

## Self-test output

```
[1/63] enc_movrr PASS  got EQUIV [ENC]
[2/63] enc_imm PASS  got EQUIV [ENC]
[3/63] enc_jmp PASS  got EQUIV [ENC]
[4/63] enc_nop PASS  got EQUIV [ENC]
[5/63] neg_enc_imm PASS  got NOT-PROVEN ret-memory
[6/63] sib PASS  got EQUIV [SIB]
[7/63] neg_sib_scale PASS  got NOT-PROVEN ret-memory
[8/63] reg_swap PASS  got EQUIV [REG,FRAME]
[9/63] reg_call PASS  got EQUIV [REG,FRAME]
[10/63] neg_reg_ret PASS  got EQUIV-IF-CALLERS-IGNORE [REG]
[11/63] neg_reg_regarg PASS  got NOT-PROVEN call-regargs
[12/63] neg_reg_nosave PASS  got NOT-PROVEN ret-callee-saved
[13/63] neg_reg_one PASS  got NOT-PROVEN ret-memory
[14/63] order_st PASS  got EQUIV [ORDER]
[15/63] order_alu PASS  got EQUIV [ORDER]
[16/63] order_ldst PASS  got EQUIV [ORDER]
[17/63] neg_order_alias PASS  got NOT-PROVEN ret-memory
[18/63] neg_order_overlap PASS  got NOT-PROVEN ret-memory
[19/63] neg_order_ldst PASS  got NOT-PROVEN ret-memory
[20/63] neg_order_flags PASS  got NOT-PROVEN branch-condition
[21/63] layout_pol PASS  got EQUIV [LAYOUT]
[22/63] layout_dup PASS  got EQUIV [LAYOUT]
[23/63] neg_layout PASS  got NOT-PROVEN ret-memory
[24/63] comm_add PASS  got EQUIV [COMM]
[25/63] comm_imul PASS  got EQUIV [COMM]
[26/63] neg_comm_sub PASS  got NOT-PROVEN ret-memory
[27/63] frame_reload PASS  got EQUIV [FRAME,VALUE-FLOW]
[28/63] neg_frame_escape PASS  got NOT-PROVEN call-args
[29/63] x87_same PASS  got EQUIV []
[30/63] neg_x87_comm PASS  got NOT-PROVEN x87-region
[31/63] x87_move_indep PASS  got EQUIV [ORDER]
[32/63] x87_move_rr PASS  got EQUIV [ORDER]
[33/63] neg_x87_fnstsw PASS  got NOT-PROVEN x87-motion
[34/63] neg_x87_sahf PASS  got NOT-PROVEN x87-motion
[35/63] neg_x87_alias PASS  got NOT-PROVEN x87-motion
[36/63] neg_x87_eflags PASS  got NOT-PROVEN x87-motion
[37/63] neg_x87_call PASS  got NOT-PROVEN x87-region
[38/63] x87_move_frame PASS  got EQUIV [ORDER,FRAME]
[39/63] neg_x87_slot PASS  got NOT-PROVEN x87-motion
[40/63] neg_call_arg PASS  got NOT-PROVEN call-args
[41/63] neg_call_order PASS  got NOT-PROVEN call-args
[42/63] neg_call_storeorder PASS  got NOT-PROVEN call-memory
[43/63] loop_reg PASS  got EQUIV [REG]
[44/63] neg_loop_bound PASS  got NOT-PROVEN branch-condition
[45/63] loop_ptr PASS  got EQUIV [REG]
[46/63] neg_loop_stride PASS  got NOT-PROVEN ret-memory
[47/63] com_ident PASS  got EQUIV []
[48/63] com_stdcall PASS  got EQUIV [ENC]
[49/63] com_cdecl PASS  got EQUIV []
[50/63] com_loop PASS  got EQUIV [ENC]
[51/63] mxcsr PASS  got EQUIV-IF-CALLERS-IGNORE [REG,VALUE-FLOW]
[52/63] neg_com_argcount PASS  got NOT-PROVEN call-args
[53/63] neg_com_order PASS  got NOT-PROVEN call-target
[54/63] neg_com_conv PASS  got NOT-PROVEN ret-esp
[55/63] com_ambiguous PASS  got EQUIV [ENC]
[56/63] neg_com_solution PASS  got NOT-PROVEN call-target
[57/63] neg_mxcsr PASS  got NOT-PROVEN ret-x87
[58/63] 0x00475a00 PASS  got EQUIV [SIB]
[59/63] 0x0045a3a0 PASS  got EQUIV-IF-CALLERS-IGNORE [ENC,FRAME,VALUE-FLOW]
[60/63] 0x00555360 PASS  got EQUIV-IF-CALLERS-IGNORE [ORDER,LAYOUT,VALUE-FLOW]
[61/63] 0x0048e820 PASS  got NOT-PROVEN call-regargs
[62/63] 0x005c4ad0 PASS  got EQUIV [FRAME,VALUE-FLOW]
[63/63] 0x00556e90 PASS  got NOT-PROVEN x87-motion
63/63 passed
```
