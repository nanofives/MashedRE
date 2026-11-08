# RESULT MATCH-RWR04 (MATCH-RWLIB-2 slice R04, 0x00550670..0x005655a0)

Verdict: GREEN partial (match-only). 18 of 79 rows at a 100% byte match with relocheck OK.
No build, no game, no gates, no tracker edits, no promotion. A later C4R batch promotes from this file.

Two sessions: the first worker (6 rows, commit e8a953a82) stopped on a reboot request. This session resumed
from that commit and discarded nothing.

## Counts

- Rows at 100% byte match, relocheck OK: **18 / 79** (before 0 / 79).
- Compiler: XP DDK cl 13.00.9176 / c2 13.00.9178 via `re/tools/match/match.py`, each file's `// FLAGS:` line
  (`/O2 /Ob1 /G5`, unchanged). No `__asm`, no naked function in any of the 18.
- `relocheck.tsv`: 18 rows, all OK.
- C++ check (`verify/matchrwl04_20261108/tools/cxxcheck.bat`, VS2022 `/TP /DMATCH_PORT`): all 23 touched sources compile.
- Each touched source has a `/* MATCH-RWR04 2026-11-08: ... */` line under its `// FLAGS:` line.
- `results.tsv`: all 79 rows re-scored by `tools/collect.py`, with the round-1 score in `before%`.

## The 18 rows at 100% (C4 candidates by route match)

| rva | size | before | what moved it |
|---|---|---|---|
| 0x0055fe40 | 12 | 25.0 | returns param 1 (Ghidra typed void) |
| 0x00565570 | 28 | 16.7 | returns param 1 (Ghidra typed void) |
| 0x0055a140 | 172 | 12.3 | returns param 1 (Ghidra typed void) |
| 0x00552720 | 43 | 6.7 | returns param 1 (Ghidra typed void) |
| 0x0055ade0 | 102 | 90.2 | `return param_2;` duplicated inside the if block (no shrink-wrap) |
| 0x00551550 | 77 | 89.2 | returns int: callback result on that path, param_1 at the end |
| 0x0055ac50 | 197 | 88.3 | word index and bit mask inline in the test, no temps |
| 0x0055ad30 | 161 | 87.7 | `ret = param_2` assigned before the bit set |
| 0x0055c380 | 88 | 85.2 | match_permute.py store order (independent field stores) |
| 0x005551d0 | 162 | 84.8 | 4-iteration zeroing `for` loop |
| 0x00559c00 | 31 | 83.3 | returns piVar1 (Ghidra typed void), `return 0` reuses eax |
| 0x005507b0 | 341 | 82.0 | error block inline in the then-branch (as 0x00550670) |
| 0x00564100 | 47 | 81.0 | fill value typed int |
| 0x0055dca0 | 184 | 80.6 | natural `for` loop over a `{int a,b}` array |
| 0x00558030 | 204 | 75.0 | offset var first, if/else return, zeroing `for` loop (not memset) |
| 0x00550670 | 200 | 70.9 | error block inline in the then-branch, NULL test at the found label |
| 0x00557ec0 | 226 | 64.9 | same three fixes as 0x00558030 |
| 0x00564130 | 81 | 48.5 | aligned alloc returning `p + pad`, pad byte in the dead param_3 slot |

Note on 0x00564130: the pad byte is spelled `*(unsigned char *)&param_3` (param_3 is dead after the alloc call).
Same semantics as a `unsigned char pad` local, and the original keeps that byte at the param_3 slot
(`mov byte ptr [esp+0x14],cl`). A plain local lands in the param_2 slot instead (90.9%).

## Other rows changed (not 100%)

| rva | before | after | note |
|---|---|---|---|
| 0x0055ded0 | 70.3 | 98.4 | separate loop counter; one store keeps `[edx+eax-0xc]` vs original `[eax+edx-0xc]` |
| 0x00552890 | 82.4 | 91.2 | permuter moved the v.y store; zero-store order and dot-product schedule remain |
| 0x00550a20 | 82.4 | 87.1 | ops pointer and `i - n` hoisted; edi shrink-wrap remains |
| 0x00552920 | 78.0 | 79.8 | products written out; original keeps w*s / h*s as stack temps |
| 0x00563840 | 75.8 | 70.3 | **source bug fixed**: the round-1 source wrote `puVar3[1]` before `puVar3` was assigned (uninitialised pointer). The store now follows the alloc. The lower score is the honest one. |

Every other row keeps its round-1 source and score (see `results.tsv`).

## Findings for the other slices

1. **Ghidra/g2s `memset` can be a source `for` loop.** Three rows went to 100% when a `memset(p+k, 0, n)` became
   `for (i = 0; i < n/4; i++) p[k+i] = 0;` (0x00558030, 0x00557ec0: rep stosd with `lea edi` scheduled first;
   0x005551d0: the 4-store unrolled form with its own `xor edx,edx` zero register, which no memset/typed-store
   spelling reproduced).
2. **Ghidra do-while with a count-down is often a natural `for` loop** (0x0055dca0), or needs its own counter
   variable distinct from the value used elsewhere (0x0055ded0: regs 95.3 -> 100).
3. **When the original does not use a register as a shared zero constant, look at where the result variable is
   assigned** (0x0055ad30: moving `ret = param_2` up stopped ebx = 0 being reused for stores).
4. **Error-block layout**: the original's "fail" block that falls through after a test came from writing the error
   code inline in the then-branch even though an identical copy exists at the end (the compiler tail-merges the
   two). 0x00550670 and 0x005507b0 both 100% with that.
5. **Return value sweep again**: 0x00559c00 is typed void by Ghidra but returns a pointer; with that, a single
   `return 0` reuses the known-zero eax instead of xor.

## Open classes worth a follow-up

- **Two or more `return 0` paths after `test eax,eax`** (0x005584c0 90.0, 0x00551840 46.5, 0x00551c40 80.5; same as
  R01's 0x004e5d30): the original keeps one inline `ret` per fail path with eax reused (no `xor`). Under 9176 every
  spelling tried (distinct result variables, `||` chains, nesting, gotos, pointer types, inline helper, 8 flag sets
  as a probe) merges the paths into one `xor eax,eax` block. With a single such path the compiler does reuse eax
  (0x00559c00). [UNCERTAIN] whether a source shape exists for this under 9176; no evidence either way about the
  original's exact compiler build (9466 is not available here).
- **Shrink-wrap of a callee-saved push** (0x00551190 90.7, 0x00550a20 87.1, 0x00551c40): original pushes at entry,
  ours after the first early return. Return shapes that fixed 0x0055ade0 / 0x00551550 do not move these.
- **x87 term order canonicalised** (0x0055c230 93.4, 0x0055c2d0 89.2): 12 term orders x operand orders, pointer
  identity forms and struct-typed forms all give the same order.
- `(x+0x1f)>>5<<5` folded to `and ~0x1f` (0x00559560): 9 more spellings incl. an inline helper, all folded.
- 0x00551a50 96.4: `lea edx,[eax+8]` vs `mov edx,..; add edx,8`; base pointer forms, index form, hoisted base all same.
- 0x00563df0 66.7: the compiler CSEs `n + 2m` between the size and the loop bound; 20 association/typing spellings.
- 0x00558cb0 78.4: original reserves an 8-byte frame and spills `end` to it; ours reuses the param slot.

Permuter runs this session (all bounded, `--walk`): 0x00551190 (93.3 only via a `>= 1` literal swap that broke
another line, not applied), 0x00558cb0 (no gain), 0x0055ded0 (no gain), 0x00551a50 / 0x0055c230 / 0x0055c2d0
(no gain), 0x00552890 (applied, 91.2), 0x00563840 (75.8 in its walk from the fixed source, not reviewed or applied).

## Files

- Sources changed (23): `re/match/rwl04/{00550670,005507b0,00550a20,00551550,00552720,00552890,00552920}.c`,
  `re/match/rwl05/{005551d0,00557ec0,00558030,00559c00,0055a140,0055ac50,0055ad30,0055ade0,0055c380,0055dca0,
  0055ded0,0055fe40,00563840,00564100,00564130,00565570}.c`.
- `results.tsv`, `relocheck.tsv`, `tools/` (`collect.py`, `headers.py`, `retv.py`, `retall.py`, `show.py`, `vtry.py`).
- Raw, not committed (D:/MashedWT/MATCH-RWR04_data): `r2/` variant specs and permuter logs, `scratch/` variant
  sources, `r2/collect2.log` sha256 e844783b8c70c1de47b609927214e9fae6f9863aace4c54e14f04b151ffd87a6,
  `r2/cxxcheck.log` sha256 6c195327feb0754a4783aa5fe0f3fcd6e76b2e76f1ab93593e34f475d8e366ac.
