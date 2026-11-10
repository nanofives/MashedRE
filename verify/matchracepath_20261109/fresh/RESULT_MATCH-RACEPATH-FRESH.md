# RESULT MATCH-RACEPATH-FRESH (race-path rows with no hand source), 2026-11-09

**Verdict: GREEN. 4 of 7 worked rows closed (57%), all 4 promoted to C4 by route match. 4 of the 11 rows not worked
(handoff below).**

Branch `orch/MATCH-RACEPATH-FRESH`, base `orch/integration` @ 3fbf77214. Every score uses `re/tools/match/match.py`
(XP DDK cl 13.00.9176 / c2 13.00.9178) with the file's own line-1 FLAGS, `/O2 /GS /Ob1` in every file. No `__asm`, no
naked body, no pragma. No mashedmod/ edit, no game run, no Ghidra write, no change to `re/tools/match/` code. The new
sources live in a new folder `re/match/racepath/`, so nothing in `mashedmod/`, `scripts/` or `re/tools/` references them
and the default gates are unaffected (not run).

Decomp: `re/tools/decomp_pc.py --port --callees --json` on read-only pool slot `Mashed_pool0` (raw JSON
`D:/MashedWT/fresh_decomp.json`). The "before" column is the raw `g2s.py` score of that JSON, re-run here
(`D:/MashedWT/fresh_g2s/results.tsv`); it reproduces the lead's numbers on all 11 rows, and gives 17.6% for
0x00412620, which had no earlier score. Per-row before/after, residual, approximate compile count and sha256:
`results.tsv`.

## Yield

Worked smallest first (0x004844a0, 0x0040d8f0, 0x0040aef0, 0x0048ab70, 0x00411ae0, 0x00484580, 0x00410860). After 6
worked, 3 had closed, so the yield rule did not stop the slice. 0x00484580 was taken out of size order because it is
the only caller of 0x004844a0 and the two share one file.

| rva | subsystem | before (raw g2s) | after | source |
|---|---|---|---|---|
| 0x004844a0 | debug-overlay | 44.3% | 213 B BYTE-IDENTICAL | `re/match/racepath/004844a0.c` |
| 0x00484580 | debug-overlay | compile-fail | 584 B BYTE-IDENTICAL | `re/match/racepath/004844a0.c` |
| 0x0040d8f0 | gameplay | 40.2% | 337 B BYTE-IDENTICAL | `re/match/racepath/0040d8f0.c` |
| 0x00410860 | util | 70.4% | 592 B BYTE-IDENTICAL | `re/match/racepath/00410860.c` |
| 0x0040aef0 | gameplay | 34.7% | 97.6%, 12 B differ | `re/match/racepath/0040aef0.c` (kept, not promoted) |
| 0x00411ae0 | vehicle | 43.9% | 97.2%, 27 B differ | `re/match/racepath/00411ae0.c` (kept, not promoted) |
| 0x0048ab70 | particle | 31.3% | 93.2%, 41 B differ | `re/match/racepath/0048ab70.c` (kept, not promoted) |

0x004844a0 scores insn 91.0% (67/61) only because the Ghidra body stops before the trailing `ret` (212 B against the
213 B symbol). The whole-symbol byte compare is identical and `c4_reclass.py` accepts it. 0x00484580's 584 B symbol
includes its two switch tables (0x00484778 / 0x004847ac).

## What moved the closed rows

1. **Literal constants, not globals (0x0040d8f0, and 0x0040aef0's first 9 B).** Several `_DAT_005c....` floats g2s
   declares as externs are the compiler's `__real@` constants: 0x005ccd70 = 450.0f, 0x005cc9a0 = 0.05f, 0x005cc33c =
   -1.0f, 0x005cc320 = 1.0f, 0x005cc9b4 = 0.99f, 0x005d757c = 0.0f (read from the anchored image). Writing the literal
   turns relocheck's unchecked externs into checked `__real@` slots and fixes the load order of mem*literal products.
2. **A same-TU `static` callee takes its argument in EAX (0x004844a0).** The caller 0x00484580 does `mov eax,esi;
   call 0x004844a0` (0x004846f0) after pushing a stack argument the callee never reads. Declaring the callee `static`
   before its caller in the same file gives this convention, as in `re/match/pilot/a6_00419090.c`. Here the real caller
   is also in the file, so no stand-in is needed.
3. **Statement shape for control flow (0x00410860, 0x00484580, 0x0040d8f0).** `if (n == 0) T = 400; T += 2;` in place of a
   temp with a conditional assignment (0x00410860 0x004108f4..0x00410908, 82.3% to 100%); a goto into the win tail
   (Ghidra's `LAB_00410a49`); reading the old global into a local before the `+0xc` store (0x00484580: 76.5% to 100%,
   the store can alias the global, so the read must come first in the source); each switch case with its own call pair
   (the compiler cross-jumps the tails); the `else` call written first where the binary lays it out first.
4. **Data shape (0x0040d8f0).** A float/V3 matrix type in place of a char buffer removes the `/GS` cookie the char
   array triggers. The slot table written as a 2-D `int[4][4]` indexed `[car][i]` gives the `cmp [esi],0` reload and EBP
   for the counter (86.7% to 93.8%). `if ((tbl[car][i] = FUN_00477b60()) != 0)` gives the EBP/EBX choice (99.1%), and a
   4-step zero loop gives `xor ecx,ecx` + 4 register stores (100%).
5. **cl unrolls a 4-step constant loop (0x00410860).** The four repeated end-of-round checks in Ghidra (0x00410903..
   0x004109b9) are one source loop. Writing it as a loop matched from the first compile.

Guard checks: no closed row introduces a `qword` operand. The semantics of each rewrite were read before keeping it:
the store reorders touch distinct fields of different globals, `-(x / 10) + y / 10` equals `y / 10 - x / 10` for these
byte operands, and the two faithful oddities are kept as in the binary (0x0040aef0's second branch zeroes a0 at
0x0040af6d; 0x0048ab70's kill path also does `inc edi` at 0x0048ac4a).

## Not closed (3 worked, kept as the next slice's starting point)

- **0x0040aef0, 12 B:** the `fld` of `m->pos.y` and of `v.y` one slot after the next `push` (0x0040afbe, 0x0040b020).
  dblgreedy (W, F) took it from 21 to 12 B. W/D/E/F/C on both y lines: no gain.
- **0x00411ae0, 27 B:** the final integer sum is reassociated. Ours is `(bb1c + d) + b7a`, the original
  `(b7a + bb1c) + d` (0x00411b3f..0x00411b45). 19 term orders, locals for the sum and the difference, and int/uint widths
  all compile to the same form. Second residue: the `&best` store is scheduled before the 0x0063bb0c test. relocheck on
  an earlier variant showed the load order of 0x007f1038/0x007f1039 swapped, which `-(b38/10) + b39/10` fixed
  (33 to 27 B).
- **0x0048ab70, 41 B:** the bitmap walk now matches with Ghidra's own loop shape (two nested loops, `next: i = k*8+8`
  at the bottom of the inner loop, the 8-way switch after it with `break` = found and `default: goto next`). The s15
  template form (`re/match/s15/0048d360.c`) put the not-found block after the epilogue here. Writing the vertex buffer
  indexed gives the strength-reduced pointer. **The second field written becomes the pointer base** (a.y with order
  x,y,z, col.g with col first, a.z with x,z,y), so the source writes a.z second to get EBP = 0x0071ba38. Residue: `d.y`
  operand order (0x0048ac5c), the a.y store slot, and the pos.x / pos.y `fadd` operand order. dblgreedy (452 variants)
  and 30 store orders: no further gain.

## Handoff: rows not worked (4)

| rva | subsystem | raw g2s | note for the next slice |
|---|---|---|---|
| 0x0048b440 | particle | 33.3% | Same 8-way bitmap walk (pool bits at 0x007200c0): start from `re/match/racepath/0048ab70.c`'s scan shape. Body has an `fsin`, a size pair reset to 0.5f per iteration and 5 calls (0x00474de0, 0x004769a0/d0/f0, 0x00476a30, 0x00476d00). |
| 0x0048b230 | particle | 23.4% | Not opened. Same particle TU (0x0048a830..0x0048b960); expect the same scan. |
| 0x0048aff0 | particle | 43.8% | Not opened. Same TU. |
| 0x00412620 | render | 17.6% | Not opened. |

For the particle rows: check each `_DAT_005c....` float against the image first (they are literals), copy the scan
from 0x0048ab70, and write the pool as a struct array indexed `[i - 1]` through a named index local (0x0048ab70 went
68.0% to 76.9% from that local alone).

## Files

| file | what |
|---|---|
| `results.tsv` | per-row before/after, verdict, residual, approximate compile count, sha256 |
| `c4_batch.tsv`, `c4_dryrun.tsv` | promotion batch and gate dry run (4 PASS, 0 refused) |
| `relocheck.tsv` | relocheck of the 3 closed sources (4 functions) |
| `tools/` | `tryv.py`, `dblgreedy.py` (copied from MATCH-RACEPATH-X87A), `odis.py` (capstone listing of the unpatched image) |

Raw data kept off the repo:

| path | SHA-256 |
|---|---|
| `D:/MashedWT/fresh_decomp.json` (decomp JSON, 11 rows) | `96d59509a33a0046cedabec1ebd3f941081a9711725110a5a4fe779c0bf08f1d` |
| `D:/MashedWT/fresh_g2s/results.tsv` (raw g2s scores, the "before" column) | `b095865c8a291a76f899ed1add83a084238c658396bad1f153cc427f95fcda99` |
| `D:/MashedWT/fresh_g2s/*.c`, `D:/MashedWT/fresh_tools/var/`, `D:/MashedWT/fresh_tools/dg/` | every variant compiled (not cited row by row) |
