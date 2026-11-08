# RESULT MATCH-S25, 2026-11-07

**Verdict: GREEN (match-only, L-93).** Slice 0x005a5910..0x005ac980: 135 functions (`slice_rvas.txt`, `slice_rows.tsv`,
`tools/mkslice.py`), game rows of hooks.csv with the one library row inside skipped.

**81/135 identical** under `re/tools/match/match.py` (XP DDK cl 13.00.9176 / c2 13.00.9178, each file's line-1 flags).
All 81 pass relocheck with OK (`relocheck.tsv`, 81 lines). 78 have `unchecked=0`. The other 3 (0x005a8960, 0x005a89d0,
0x005ab040) have only Win32 imports unchecked (`ReleaseSemaphore`, `WaitForSingleObject`, `QueryPerformanceCounter`,
`timeGetTime`), which the brief allows. No file contains `__asm`. 133 of 135 files pass the VS2022 C++ check
(`cxx_all.txt`); the two that fail are below 100% (0x005a6f30, 0x005ac210, header NOTE in each). Sources:
`re/match/s25/<va8>.c`. Scores, source sha256: `results.tsv`. No body swap, no hooks.csv / CHANGELOG / tracker /
pc_funcs.csv edits, no gates. Step C4R-MATCHBATCH-2 promotes the 81.

## Tools and tables used (all uncommitted)

- `pc_funcs.csv`: the regenerated table from `orch/MATCH-PCFUNCS-REGEN` @ 3406a0397 (6354 rows), overlaid on
  `re/console/pc_funcs.csv` in the worktree and checked back out before the commit. sha256 `1298644ace0896559a18a53817d5bcb5d4d199b80d64d7330d6637b28fff91a4`.
  With it every slice row has a size (0x005aa1e0 = 26 B), so **no nosize rows**; `by` is `insn` for every identical row.
- `match.py`: overlaid with `orch/C4R-MATCHPY-REL32` (REL32 false 1-byte diff on self-recursive calls) for the final
  collect and re-score, checked back out before the commit.
- `relocheck.py`: the `orch/MATCH-S17` version (SEH scope tables), data-dir copy with only its `sys.path` line pointed
  at this worktree's `re/tools/match`. sha256 `e0f84075dfe251eca0b00ec1fabb4b8116ffaa25c33fb09438adbfef92682e69`. Not committed.
- Raw data in `D:/MashedWT/MATCH-S25_data` (decomp_raw.json sha256 `389eafc36e17c35dcb2a813321f0be3df0e3e62cbd7667f9377063840e2142b5`, via
  `re/tools/decomp_pc.py --port --callees --create`, pool slot Mashed_pool0 (run from the main checkout, because the pool lives there), slot released).
  `decomp.json` is the same JSON with non-ASCII characters escaped.

## Flags

134 files use `/O2 /Ob1 /G5` (RW/middleware build, no `/GS`), 1 file (0x005abe30, 20.7%, structure not recovered) has `/O1`
from the permuter. The raw converter ran on all 135 rows under three flag sets (`g2s_g5` = `/O2 /Ob1 /G5`, `g2s_game` = `/O2 /GS /Ob1`,
`g2s_o1` = `/O1`); the 37 rows at 100% in the raw converter all score under `/O2 /Ob1 /G5` (`g2s_best_results.tsv`, column `run`).
Helpers re-tried `/GS`, `/O1`, `/Ox`, `/Og`, `/Ob2`, `/Oy-` and `/QIfist` on the stalled rows; none reached 100%.

## Counts

| | count |
|---|---|
| slice functions | 135 |
| raw converter (g2s, `/O2 /Ob1 /G5`) at 100% | 37 |
| of those, C++ prototype/cast fixes needed (re-run relocheck OK after) | 27 |
| hand-matched to 100% (helpers h1-h3, `notes/h*.md`) | 44 |
| **identical total** | **81** |
| below 100% | 54: (a) 14, (b) 39, (c) 1 |

Helpers h1 to h3 were claude2 sessions on claude-sonnet-5-5 (medium), all closed.

## Below 100% (budget spent)

Classes: (a) compiler-version codegen, (b) source shape not found within the budget, (c) tool limit. Per-row reasons are
from the helper notes (`notes/h*.md`). The percentages for 0x005a6f30 and 0x005ac210 score code with known semantic
errors and are not match quality. 0x005a8f30 emits 97 of 168 instructions because Ghidra dropped unreachable blocks.

| rva | size | best % | regs % | class | residual |
|---|---|---|---|---|---|
| 0x005a8390 | 195 | 98.5 | 98.5 | b | one scratch-register load position in the list-link sequence |
| 0x005a86a0 | 176 | 98.5 | 98.5 | a | one store ([esi]=ecx) sits between the 3rd and 4th struct dwords in the original |
| 0x005a8560 | 215 | 98.4 | 98.4 | b | list-head load position / scratch register in the list-link sequence |
| 0x005ab560 | 185 | 98.2 | 98.2 | a | one load scheduled after the sentinel store; a temp fixes order but swaps the second load register; fields via piVar1 = param_1 + 1 |
| 0x005ac7e0 | 148 | 98.2 | 98.2 | a | cmp eax,edi scheduled one store earlier in the original; permuter and reorder tried |
| 0x005a7ee0 | 135 | 97.7 | 97.7 | a | two adjacent list-head stores schedule order vs lea edi; fixing one breaks the other (permuter swap adopted) |
| 0x005a9c50 | 171 | 96.7 | 96.7 | b | one unfolded 'or ecx,eax' (eax=0) on the +0x40 read-back is folded away in ours |
| 0x005a8750 | 250 | 92.1 | 92.1 | a | uint->float conversion: original fild qword + fidiv dword on a zero-extended stack temp, DDK fild qword + fdivp |
| 0x005aa7e0 | 177 | 91.7 | 95.0 | b | add-chain / word-load order of the final +8 + w*8 + w*0xc |
| 0x005a6cb0 | 176 | 88.9 | 88.9 | b | zero-register compare (cmp eax,ebx;jbe) vs cmp eax,1;jb; ebx/edi assignment trade-off |
| 0x005ab1c0 | 56 | 88.9 | 88.9 | a | (x >> 11) << 11 folds to "and ecx,-2048" in the DDK; tried temps, two statements |
| 0x005a8890 | 207 | 88.2 | 97.1 | b | param_2/puVar2 register split differs (edx vs eax) at the two struct copies |
| 0x005aa3d0 | 163 | 86.0 | 93.0 | b | original hoists the field+0x10 load above the ushort test (ecx), ushort "> 0" form |
| 0x005ab090 | 106 | 85.7 | 88.6 | b | switch(param_3){case 2}; store order of [0x10]/[0x11] and constant-1 register reuse |
| 0x005aa060 | 82 | 85.2 | 100.0 | a | tail-store register naming (ecx/edx vs eax/ecx), same order; 6 spellings; struct copy fixed lea scheduling |
| 0x005a65d0 | 112 | 84.6 | 84.6 | b | memset intrinsic + base-relative stores; 1.0f register choice (ecx vs eax) and load order |
| 0x005a6b70 | 233 | 84.6 | 84.6 | a | original duplicates the null-return epilogue inline, DDK shares it; locals as array |
| 0x005a6340 | 604 | 82.4 | 82.4 | b | inline bit-scan macro (M evaluated per use, 1<<FUN(0x28)); +7+0x10 and -1+0x10 folded, m kept in memory in the original |
| 0x005aa7a0 | 60 | 82.1 | 82.1 | a | original duplicates pop/pop/ret inline for the early return, DDK shares the tail; 8 source forms + permuter |
| 0x005aba20 | 642 | 81.9 | 85.2 | b | 642 B, structure right (struct copies, memcpy intrinsic), branch layout and register allocation; permuter 300 s |
| 0x005aa560 | 568 | 79.8 | 79.8 | b | prologue/arg load order and register allocation; 568 B, structure right, permuter 300 s |
| 0x005aab00 | 97 | 77.8 | 77.8 | b | ops identical, original reloads param_1 from [esp+0x10] where ours does mov eax,esi; permuter no gain |
| 0x005a62b0 | 137 | 77.6 | 77.6 | a | compiler cross-jumps the three failure returns; original keeps them inline (no flag/source form found) |
| 0x005aaac0 | 53 | 77.3 | 81.8 | a | callee-saved pushes shrink-wrapped past the early exit; while/for/do forms equal |
| 0x005a9d00 | 211 | 75.9 | 75.9 | b | flags dword load and epilogue block placement; uint temp gives ops 94% but exact 70% |
| 0x005aab70 | 97 | 75.6 | 75.6 | b | fn-pointer register (eax vs ecx) in first block, param_2 reload |
| 0x005a9e60 | 161 | 75.4 | 78.5 | b | or-flag register (eax/al vs ecx) and reload of [esi+0x48] |
| 0x005ab410 | 140 | 75.4 | 75.4 | b | loop rotation / failure-return placement |
| 0x005ab380 | 137 | 75.0 | 84.6 | b | zero register reused across both branches in the original; locals as array |
| 0x005a9f10 | 65 | 73.9 | 73.9 | b | 2nd branch (1-arg callee) not emitted as tail jump, jns vs jge; flags test as uint |
| 0x005a9ab0 | 297 | 72.3 | 76.8 | b | param_1 lives in ebp in the original, spilled by the DDK; permuter 300 s |
| 0x005a61b0 | 111 | 71.4 | 71.4 | b | return-4 block placement (after cmp 0x400) not reproduced |
| 0x005aac20 | 194 | 69.7 | 81.6 | b | two unlink loops: register choice and arg-push order differ; permuter move 60->70% |
| 0x005aa200 | 290 | 66.1 | 77.7 | b | register allocation / ushort loop-entry compare, 112 vs 108 ins |
| 0x005a6f30 | 690 | 65.4 | 70.9 | b | raw converter + permuter; callee float-arg calls (fld/fstp by value), ROUND() macro compiled as implicit call, goto block drops stores; needs a hand rewrite |
| 0x005abfa0 | 620 | 62.9 | 77.0 | b | register allocation of ebx/ebp/edi and entry load hoisting (ops 97%) |
| 0x005a9f60 | 138 | 58.8 | 66.2 | b | flag byte tested via push ebx/mov bl/test/pop in original and dword temp for [edi+0xc]; ours CSEs the address; over budget |
| 0x005ac210 | 804 | 57.1 | 69.0 | b | raw converter + permuter; ops 90.5% but register allocation differs; callee array/pointer prototypes and several `code` call arities not fixed |
| 0x005aa0c0 | 177 | 56.2 | 76.2 | b | original spills the sentinel into the dead param_5 slot and keeps param_1 in esi with arg-slot write-back; permuter swapcmp 26->56% |
| 0x005aa330 | 151 | 54.7 | 96.2 | b | branch form matched via (ushort)0 < n; register allocation of count/iterator/base differs (esi/edi swapped, base load order) |
| 0x005a7b60 | 770 | 53.5 | 59.0 | b | 770 B, structure transcribed, locals as array and int flags; ran out of budget |
| 0x005abd30 | 255 | 52.3 | 61.5 | b | raw converter + permuter (43->52%); prologue/register allocation (param_1 in ebp, 4 callee-saved) differs; C++ protos fixed |
| 0x005a75b0 | 593 | 51.6 | 53.7 | a | x87 operand order / fcomp flag-test forms in distance-attenuation and sort-insert (not tuned past permuter) |
| 0x005a6d60 | 48 | 50.0 | 72.7 | b | scheduling of arg loads / edi use differs |
| 0x005ab010 | 39 | 50.0 | 61.1 | b | original has unfolded 64-bit compare (hi halves xor 0), ours folds/swaps operands |
| 0x005ac540 | 16 | 50.0 | 83.3 | a | byte load zero-extends via xor eax,eax; mov al (ecx arg); 15 variants + 5 flag sets no change |
| 0x005ab620 | 233 | 49.3 | 88.0 | b | import cached in edi / semaphore else-branch layout; fields via piVar1 base, flag as int set in branch |
| 0x005a71f0 | 428 | 47.9 | 69.4 | b | size-sum reassociation ((n+1)*0x18 etc. not folded in the original) and register allocation |
| 0x005ab710 | 617 | 44.9 | 62.6 | b | large fn: original uses a dec/dec chain for the param_4 switch where ours has cmp chain; structure only |
| 0x005a8060 | 744 | 40.0 | 45.2 | b | macro re-evaluates 1<<FUN_005aee20(0x28) in every sub-expression, Ghidra pruned it (141 vs 210 insns) |
| 0x005a6ea0 | 129 | 38.9 | 44.4 | c | original truncates inline (fnstcw/or ah,0xc/fldcw/fistp) and uses fidiv dword on a zero-extended temp; DDK calls _ftol and fild qword |
| 0x005a9280 | 1036 | 38.0 | 40.4 | a | x87 operand order in the four dot-product clamps and arg-slot-reuse frame (0x30 vs 0x28), not tuned past permuter |
| 0x005a8f30 | 589 | 30.4 | 33.9 | b | raw converter + permuter; 97/168 ins so the body is incomplete (Ghidra dropped unreachable blocks); FUN_005a91b0 now taken as a real address (cast), C++ protos fixed |
| 0x005abe30 | 328 | 20.7 | 29.6 | b | raw converter + permuter at /O1 (the /G5 form scores 18.5%); structure not recovered; C++ protos fixed |

No function file holds a static callee of another function.
