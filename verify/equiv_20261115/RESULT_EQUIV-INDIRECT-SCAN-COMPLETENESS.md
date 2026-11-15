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

ROW2
