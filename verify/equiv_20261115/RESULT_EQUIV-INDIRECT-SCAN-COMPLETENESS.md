# RESULT EQUIV-INDIRECT-SCAN-COMPLETENESS + TOOL-EQUIV-EBPDATA-REPSTOS (run 49, 2026-11-15)

Row kind: tool rows on `re/tools/match/equiv.py`. No build, no gates. Branch `orch/EQUIV-INDIRECT-SCAN-COMPLETENESS`
from orch/integration 38a6f3ae5, one commit per row (row 1 first). Image: `MASHED.exe.unpatched` through
`match.pe()` (the anchored binary).

## Row 1: EQUIV-INDIRECT-SCAN-COMPLETENESS

### Verdict

Scan extended, completeness **not** proven. The [UNCERTAIN] of RESULT_EQUIV-CALLER-0045AC2A.md section 3 is narrowed
and stays open on one named residue (below). The 0x0045ac2a verdict is unchanged (`ignores`): no reference found by
the extended scan reaches a call through record +0x24.

### What the old scan missed

The old scan looked for `mov r32, [r32+0xa8]` in disp32 form. The holder array that stores the id-lookup record is a
global, `0x0088fbe0 + k * 0xb4` (k < 4, loop bound `cmp esi, 0x88feb0` at 0x0045bec2), and five of its readers address
the +0xa8 field with the absolute base folded into the displacement, `[idx*0xb4 + 0x0088fc88]` after an `imul`:
0x004144b3, 0x004152ae, 0x00416517, 0x00416c67, 0x00418067 (plus 0x0045bffa in the module). None of those has
`0xa8` as its displacement, so the old pattern could not see them. None of them calls through the record:

| site | instruction | use of the record pointer |
|---|---|---|
| 0x004144b3 | `mov edx, [ecx+0x88fc88]` | `test edx, edx` only |
| 0x004152ae | `mov eax, [eax+0x88fc88]` | `mov eax, [eax]` (record id) only |
| 0x00416517 / 0x00416c67 / 0x00418067 | `mov eax, [edx+0x88fc88]` | `cmp eax, ecx` (0), then eax overwritten by `call 0x004a2c48` |
| 0x0045bffa | `mov edx, [eax+0x88fc88]` | `test` / `sete` only |

### What equiv.py now checks (`route_audit(entry)`, run by `indirect_sites()` before the route is used)

Every check is byte-anchored: a dword field is matched at every byte offset, so the disp32 / imm32 / moffs32 field of
any addressing form (SIB, absolute-indexed, immediate) is found whatever the instruction start. A disp8 cannot address
+0xa8 (0xa8 sign-extends to -0x58; asserted in the test). Each check raises ValueError on any difference from the
registered list in `INDIRECT_CALLERS[0x0045ac20]["audit"]`:

1. The dword 0x0045ac20 appears in the image (every section) only at the slot 0x005f9abc.
2. No rel32 call / jmp / jcc and no rel8 jmp / jcc in .text lands on 0x0045ac20.
3. The 28 .text dwords in the table window [0x005f9990, 0x005fa400) are exactly the registered ones (count reads of
   0x005f9bd8, nine column loads, the id-lookup base `mov eax, 0x5f9998` at 0x0045baac). The +0x24 column base
   0x005f99bc is loaded only by the route's `mov edi, 0x5f99bc` at 0x0045b9dd. No dword of the window outside .text.
4. The id lookup 0x0045baa0 has one caller, 0x0045bfa5 (no other rel branch, no dword of it in the image). Its record
   goes to `[elem+0xa8]` at 0x0045bfaf in 0x0045bfa0, which is entered only by `jmp 0x45bfa0` at 0x0045c024
   (FUN_0045c010: `elem = arg0 * 0xb4 + 0x88fbe0`) and is not referenced as a dword.
5. The 15 .text dwords in the holder array [0x0088fbe0, 0x0088feb0] are exactly the registered ones, each with its
   use written next to it in equiv.py: the six field loads above, the element-pointer producers 0x0045baf0 (`lea esi,
   [ebx+0x88fbe0]` into 0x0045ba10, which clears the element), 0x0045bbc0 (`mov edi, 0x88fc70`, FUN_0045bba0: record
   calls +0x8 at 0x0045bdbb, +0xc at 0x0045bd58, +0x10 through 0x0045bac0, +0x4 through 0x0045c010), the loops at
   0x0045beb2 / 0x0045bed1 / 0x0045bf37 (0x0045ba10, and +0x10 through 0x0045bac0), their three loop bounds, and
   0x0045c01a (into 0x0045bfa0, +0x4). No dword of the array outside .text.
6. The 95 distinct record methods (every non-zero pointer in columns +0x4..+0x3c of the 9 records): each body, walked
   as a CFG (jcc and in-function jmp edges, jump tables `jmp [idx*4+table]` followed to the entries inside the body, a
   jmp to a pc_funcs.csv entry or a method is a tail call), has no memory operand with displacement 0xa8 off a non-ESP
   base and no `call [r+0x24]`. All 95 walks end `ok`.

### Residue

[UNCERTAIN] Not followed: the callees that receive an element pointer, past the record method bodies: the record
methods called with `elem` (+0x4 / +0x8 / +0xc / +0x10) may pass `elem` on to their own callees, and FUN_0045bba0
passes `elem+0x40` / `elem+0x80` to 0x0041f060, 0x004b4b60, 0x0045c350, 0x00476880. A callee that reads `[p+0xa8-off]`
of one of those and calls `+0x24` would be a route the audit does not see. A depth-3 transitive scan of direct callees
was tried and does not close it: it reaches large unrelated functions whose `+0xa8` / `+0x24` hits cannot be typed
statically. Closing test, unchanged: a Frida return-address sample on 0x0045ac20 (row EQUIV-CALLER-0045AC20-FRIDA-RETS).
This text is in row 58 (0x0045a3a0) of `re/match/ADMITTED.tsv`, column `caller_scan`, and in
`INDIRECT_CALLERS[0x0045ac20]["residue"]`.

### Test

`test_route_audit` in `re/tools/match/test_equiv.py`: the audit result on the image (1 entry dword, 28 table refs, 15
holder refs, 95 methods), the disp8 encoding, and three refusals (table list without 0x0045b9de, holder list without
0x0045bffc, an empty branch list for 0x0045bfa0 each raise ValueError).

`py -3.12 -m pytest re/tools/match/test_equiv.py -v` on the row-1 commit (test-slot slot 6/8, local):

```
test_equiv.py::test_cases PASSED                                         [ 16%]
test_equiv.py::test_references PASSED                                    [ 33%]
test_equiv.py::test_indirect_callers PASSED                              [ 50%]
test_equiv.py::test_route_audit PASSED                                   [ 66%]
test_equiv.py::test_summary_precision PASSED                             [ 83%]
test_equiv.py::test_budgets PASSED                                       [100%]
======================== 6 passed in 74.63s (0:01:14) =========================
```

`test_references` includes 0x0045a3a0 (`re/match/s09/0045a3a0.c`), still EQUIV-IF-CALLERS-IGNORE with `indirect_sites()`
now running the audit first.

## Row 2: TOOL-EQUIV-EBPDATA-REPSTOS

### Verdict

Both gaps fixed and tested. 0x005ae650 is still **NOT-PROVEN**, now on a third, different point (below), so it is not
admitted and stays provisional (`re/match/PROVISIONAL.tsv`, unchanged). No ADMITTED.tsv change, no decision row.

### (1) Callee summary with EBP as a data register

`_summary_body` tracked EBP only as a frame pointer: after any other write to EBP it set the offset to None, and a
later `[ebp+k]` made N (stack arguments read) unknown. New: `_ebp_data_write()` marks EBP as holding data
(`EBP_DATA`) when it is written by a load from memory (`mov / movzx / movsx ebp, m`), a constant, the zero idiom, or
arithmetic on an EBP that already holds data with no ESP operand. `[ebp+k]` with a data EBP is then an ordinary memory
access, as `[esi+k]` already is. A copy from another register, `lea ebp, [...]` and `pop ebp` still leave EBP unknown
(the value may be a stack address), and the frame-pointer cases (`mov ebp, esp`, `leave`, `mov esp, ebp`, the
`and esp` alignment) only act on an integer frame offset.

On the image, 0x005ae800: `mov ebp, [esp+0x10]` at 0x005ae803 (counted as a read of argument 0), `mov edi, [ebp+0x10]`
at 0x005ae80b (now ordinary memory). Summary before: `N=None, n_why=0x005ae80b`. After:
`{'n_why': False, 'P': 0, 'N': 2, 'livein': frozenset(), 'eax_def': True, 'noreturn': False, 'known': True}`.
N = 2 is what RESULT_PORT-TOP-AUDIOPOOL-5AE650.md section 1 read from the disassembly; that [UNCERTAIN] now has a tool
result and a test.

### (2) `rep stos` with a variable count

`strop` raised `unsupported: rep stosd with a variable count`. New `Ctx.stos_var()` (DF clear, as the constant-count
path already assumes): memory becomes the uninterpreted term `Mfill(previous memory, destination, count, value, size)`,
EDI moves by count * size, ECX becomes 0, rule `rep-stos-var` is reported. Two sides are equal only when all five
arguments are equal terms. Nothing is forwarded through the term and no disjointness is claimed across it. A
destination, count or value holding a stack address is refused (`unsupported ... on a stack address`): the
private-slot model has no symbolic-size write. `rep movs` with a variable count is unchanged (still unsupported; the
0x005441f0 reference row pins it).

### 0x005ae650 re-run

`py -3.12 re/tools/match/equiv.py re/match/s26/005ae650.c --only 0x005ae650` (source commit 12234ab8b, flags from the
source), output `verify/equiv_20261115/equiv_005ae650.txt` / `.json`:

```
0x005ae650	NOT-PROVEN	ret-memory: region ours +0xeb / orig 0x005ae73d: ...
```

The proof now passes the call to 0x005ae800 (no call-args failure) and both `rep stosd` / `rep stosb` of the cell
loop (0x005ae729, 0x005ae730 on the original side). It stops after the loop, at orig 0x005ae73d
(`mov ecx, [0x7dda80]`): the two memory images differ in the order of a store to `[p+0x20]` (p = a value from the
0x005ae800 call, stored value 0x7dda80) and a load from the global 0x7dda80. Ours stores first and then loads, the
original loads first. The tool cannot show that `p+0x20` and 0x7dda80 are different addresses, so the loaded value
is not equal on both sides. Read from the printed terms of the verdict, not a tool proof: [UNCERTAIN] which source
statement this is. A rule that a pointer returned by a callee never aliases a fixed global would be a new
assumption, not in this row's scope (and a decision, not a tool fix).

### Tests

New cases in `re/tools/match/equiv_tests/cases.asm`: `stosv` (same fill, different set-up order: EQUIV, reports
`rep-stos-var`), `neg_stosv_val` (fill 0 against fill -1: NOT-PROVEN ret-memory), `neg_stosv_stack` (fill of the own
frame: NOT-PROVEN unsupported). `test_summary_precision` adds the 0x005ae800 summary (N = 2, no n_why) and seven
`_ebp_data_write` encodings (load, zero idiom, `mov ebp, esp`, `mov ebp, eax`, `lea ebp, [esp+8]`, `add ebp, 4` on
data, `add ebp, esp`).

`py -3.12 -m pytest re/tools/match/test_equiv.py -v` on the row-2 commit (test-slot slot 3/8, local):

```
test_equiv.py::test_cases PASSED                                         [ 16%]
test_equiv.py::test_references PASSED                                    [ 33%]
test_equiv.py::test_indirect_callers PASSED                              [ 50%]
test_equiv.py::test_route_audit PASSED                                   [ 66%]
test_equiv.py::test_summary_precision PASSED                             [ 83%]
test_equiv.py::test_budgets PASSED                                       [100%]
============================= 6 passed in 40.05s ==============================
```

## Row 3 (section 3): TOOL-EQUIV-PUSHECX-SLOT

### Verdict

Gap fixed and tested. 0x005adc10 is still **NOT-PROVEN**, now on a different call (EDX at orig 0x005adcea, below),
so it is not admitted and stays provisional (`re/match/PROVISIONAL.tsv`, unchanged). No ADMITTED.tsv change. No
`--relax-icall` admission (that is a decision row).

### The change

In `_summary_body`, `lea r, [esp+k]` over a pushed save slot used to add every pushed register to the live-ins at
once ("its address escapes: count it read"). Now those slots move to a pending set `escaped`. A pending register is
counted read when, before a full 4-byte ESP-relative overwrite of its slot, one of these comes first: an ESP-relative
read of the slot, a partial write of it, a `pop` of it into another register, any memory read whose base is not ESP
(it may go through the copied address), or any call, jmp, jcc, ret, leave or enter. A full overwrite first makes the
slot a plain local and drops it from the set.

0x005ad910 (disassembly of the anchored image): `push ecx` at 0x005ad910 (ECX undefined: a save), three pushes, then
`lea ecx, [esp+8]` at 0x005ad927 (the slot's address), `push eax`, `push ecx`, `push 0x2000` (no memory read), and
`mov dword ptr [esp+0x14], 0` at 0x005ad932 (full overwrite of the slot) before `call 0x5ad770` at 0x005ad93a.
Summary before: `livein {ecx}`, after: `livein {}`. N stays None (`n_why` 0x005ad922: `lea eax, [esp+0x14]` takes the
address of an argument slot, a separate and unchanged rule). The [UNCERTAIN] of RESULT_PORT-TOP-AUDIOSTOP-5ADC10.md
section 1 (ECX not read, from the disassembly) now has a tool result and a test.

### 0x005adc10 re-run

Source: the 92.2% `re/match/s26/005adc10.c` from orch/PORT-TOP-AUDIOSTOP-5ADC10 @ ed9591c42 (not on this branch; copy
in `verify/equiv_20261115/pushecx_5adc10/005adc10_ed9591c42.c`, flags `/O2 /Ob1 /G5` from its first line). Output
`pushecx_5adc10/equiv_005adc10.txt`:

```
0x005adc10	NOT-PROVEN	call-regargs: region ours +0xda / orig 0x005adcea call 0x5adfe0: (('edx', retd32(call0(0x5ae010, ...
```

The proof passes the call at 0x005adc33 (the row's gap) and stops at the next call, 0x005adfe0 at orig 0x005adcea, on
EDX (the high half left by the earlier call to 0x005ae010, different on the two sides). Why 0x005adfe0 counts EDX as
live-in: 0x005adfe0 calls 0x005ae050 (0x005adfe6), which calls 0x004522d0 (0x005ae063), and 0x004522d0 is
`mov ecx, [0x7d3ff8]` then `jmp dword ptr [ecx+0x10c]`, an indirect tail jump. Rule A4 counts ECX and EDX as read
by an unknown pointer target unless `--relax-icall`; ECX is written there, EDX is not. Summaries:
0x004522d0 `livein {edx}`, 0x005ae050 `livein {edx}`, 0x005adfe0 `livein {edx}`.

For information only (not an admission, `pushecx_5adc10/equiv_005adc10_relax_icall.txt`): with `--relax-icall`
the verdict is `EQUIV-IF-CALLERS-IGNORE(EDX,ECX) [REG,LAYOUT,VALUE-FLOW] callers 1, scan says ignore: 0/1:
0x005add67:unknown/unknown`, so it would also need the caller 0x005add67 resolved. Both are decision-row matters.

### Regression sample

`pushecx_5adc10/sweep_livein.py` compares `summary()` of the row-2 commit against this change. A full pc_funcs.csv
sweep ran over 30 min with no output and was stopped (its own PIDs only). The bounded rerun is a **sample**: the
direct-call trees of 0x005adc10 and 0x005ae650 to depth 3 (28 functions) plus 298 pc_funcs.csv entries drawn with
seed 49 (326 total). It hit its 900 s cap at 99/326: all 28 call-tree functions and the first 71 random entries.
Result (`pushecx_5adc10/sweep_livein_sample.txt`): one change, `0x005ad910 livein ['ecx'] -> []`, 98 unchanged.
The other 227 sampled entries were not compared.

### Test

`test_push_slot` in `re/tools/match/test_equiv.py`: the 0x005ad910 summary has no ECX live-in, and four synthetic
byte sequences run through `summary()` at an address outside the image (image reader and size lookup patched): a
full overwrite before a read through the address (ECX not read), a read through the address first, a partial
(byte) overwrite, and a branch before the overwrite (each: ECX read).

`py -3.12 -m pytest re/tools/match/test_equiv.py -v` on the row-3 commit (test-slot slot 2/8, local):

```
test_equiv.py::test_cases PASSED                                         [ 14%]
test_equiv.py::test_references PASSED                                    [ 28%]
test_equiv.py::test_indirect_callers PASSED                              [ 42%]
test_equiv.py::test_route_audit PASSED                                   [ 57%]
test_equiv.py::test_summary_precision PASSED                             [ 71%]
test_equiv.py::test_push_slot PASSED                                     [ 85%]
test_equiv.py::test_budgets PASSED                                       [100%]
============================= 7 passed in 20.80s ==============================
```
