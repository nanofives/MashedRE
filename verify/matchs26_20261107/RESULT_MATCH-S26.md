# RESULT MATCH-S26, 2026-11-07

**Verdict: GREEN (match-only, L-93).** Slice 0x005ac9e0..0x005b1140: 135 functions (`slice_rvas.txt`, `slice_rows.tsv`,
`tools/mkslice.py`). 0 library rows fall inside it. All 135 are hooks.csv subsystem `audio` (RenderWare audio / RWS code).

**89/135 identical** under `re/tools/match/match.py` (XP DDK cl 13.00.9176 / c2 13.00.9178, each file's line-1 flags).
All 89 pass `relocheck.py` with OK (`relocheck.tsv`, 89 lines). 83 have `unchecked=0`. The other 6 have only Win32 imports or
`___beginthread` unchecked (`CreateSemaphoreA`, `WaitForSingleObject`, `SetThreadPriority`, `InterlockedExchange` x2,
`InterlockedIncrement`), which the brief allows. All 89 are scored by instruction match (`by = insn` in `results.tsv`), none by
the bytes rule. No file has `__asm`, no file has a `#pragma` other than `intrinsic`. 134 of 135 files pass the VS2022 C++ check
(`cxxmany.bat`, `cxx_all.txt`): all 89 identical files and 45 of the 46 below 100%. The one C++ failure is 0x005afcf0 (31.4%, below).
0x005ad8b0 failed in `cxx_all.txt` and was fixed afterwards with a `(uint *)` cast (match 100%, relocheck OK, cxxcheck clean re-run).
Sources: `re/match/s26/<va8>.c`. Scores, source sha256 (16 hex) and how each row was scored: `results.tsv`.

This step does no body swap, edits no trackers (hooks.csv, CHANGELOG, UNCERTAINTIES, STUBS, pc_funcs.csv) and runs no gates.
A later C4R batch promotes the 89 identical rows.

## Tools used (uncommitted overlays in the worktree, not part of the commit)
- `match.py`: `orch/C4R-MATCHPY-REL32` version (fixes the false 1-byte diff on self-recursive REL32 calls). Final scoring used it.
- `relocheck.py`: `orch/MATCH-S17` version (SEH scope tables). sha256 94cb562f5d65ef07589ef4307547ccba7548c8e565541b673d6759734359fb16.
- `re/console/pc_funcs.csv`: `orch/MATCH-PCFUNCS-REGEN` version. sha256 1298644ace0896559a18a53817d5bcb5d4d199b80d64d7330d6637b28fff91a4.
  All 135 slice rows have a size in it, so no row is scored with a forced (nosize) size.
- Overlay match.py sha256 75f7aebc940a18772d70a865a75c809804a314f8337950daf20cc7f8912ea9d8.

## Flags
Raw converter (`g2s.py`) ran on all 135 rows under three flag sets: `/O2 /Ob1 /G5`, `/O2 /GS /Ob1`, `/O1` (`tools/g2s_best.py` merges,
best per row, ties to `/G5`): 122 rows best under /G5, 11 under the game flags, 2 under /O1. 43 rows are 100% from the raw converter,
10 did not compile. Final line-1 flags of the 89 identical files: 87 `/O2 /Ob1 /G5`, 2 `/O2 /GS /Ob1`. Helpers moved several rows to
`/G5` (e.g. 0x005ad6a0, 0x005ad5f0, 0x005b0970) or tried `/Op` (0x005acda0, below 100%).

## Counts
| | count |
|---|---|
| slice functions | 135 |
| raw converter at 100% (kept at 100% after C++ cast edits, relocheck OK) | 43 |
| hand-matched to 100% (helpers h1-h3, `notes/h*.md`) | 46 |
| **identical total** | **89** |
| below 100% | 46: (a) 10, (b) 35, (c) 1 |

27 raw-converter files needed C++-check edits only (explicit casts, corrected callee arity/types, real function-pointer types
for `code`); helper h4, `notes/h4.md`. DDK bytes unchanged, match and relocheck re-run after each edit.
No file holds a static callee. 0x005aea00 is `uint(uint,uint)` cdecl (bare tail jump); four files not updated for it
(0x005ad420, 0x005ae470, 0x005ae650, 0x005afa00) were checked: 0x005ae470 is identical, the other three are below 100% and compile.

## Below 100% (budget spent)
Classes: (a) compiler-version codegen, (b) source shape not found within the budget, (c) tool limit. Permuter variants that changed
behaviour (0x005ae800, 0x005acaa0, 0x005ad420, 0x005af860, 0x005b0740, 0x005ae250) were rejected.

| rva | size | best % | regs % | class | residual |
|---|---|---|---|---|---|
| 0x005af170 | 12 | 0.0 | 0.0 | a | orig push/call/pop ecx/ret; cl always emits the tail jump; 10 flag sets, int/void callee tried |
| 0x005b0f40 | 15 | 66.7 | 66.7 | b | orig mov ecx,[eax+0x28];push ecx vs our push [eax+0x28] under /O1 |
| 0x005aee20 | 28 | 78.6 | 78.6 | b | orig keeps counter in al, copies to cl without and 0x1f, esi=1 hoisted; 5 spellings |
| 0x005b10a0 | 38 | 42.9 | 78.6 | b | orig loads word [entry+4] into ax first, param_1 last |
| 0x005b10e0 | 38 | 42.9 | 78.6 | b | table call: orig loads u16 via [tab+idx*8+4] before lea |
| 0x005af230 | 47 | 79.2 | 79.2 | b | orig loads list head from [eax+0x18] with param in eax; 4 loop forms |
| 0x005addd0 | 49 | 88.2 | 88.2 | b | null-check layout: orig jne body/ret first |
| 0x005acd10 | 74 | 78.0 | 78.0 | b | orig keeps a zero in ecx as fall-out return; 8 forms + flags |
| 0x005ae170 | 81 | 61.9 | 76.2 | b | ops 97.6%, only register allocation differs (a in al then mov cl,al) |
| 0x005ae3a0 | 93 | 84.8 | 84.8 | b | orig returns the last call result via shared mov eax,edi epilogue |
| 0x005ad910 | 97 | 66.7 | 83.3 | a | orig copies param_3 onto its own slot, ret-first layout; unmoved by any variant |
| 0x005af690 | 97 | 88.9 | 88.9 | a | orig add esp after each call (0xc/0xc/0x10); DDK defers to one add esp,0x28 |
| 0x005ad9d0 | 98 | 88.9 | 88.9 | b | epilogue layout: ret block after the call, jne to fixup tail |
| 0x005aeb90 | 98 | 43.3 | 43.3 | c | orig expands log10 inline (fldlg2/fyl2x); cl 9176 calls it |
| 0x005aec30 | 104 | 73.2 | 75.6 | b | ecx load before shr/je, shl/and schedule, dh/dl order |
| 0x005b0f90 | 104 | 71.8 | 76.9 | b | switch; /O2 gives regs 94.7% but param_1 lands in ecx, orig eax |
| 0x005aea60 | 111 | 41.7 | 79.2 | b | orig loads global into ecx before the null test, param in eax; ours swaps |
| 0x005b0df0 | 116 | 95.1 | 95.1 | a | orig add esp,4 after nested 4-arg call; DDK defers to add esp,0x14 |
| 0x005aeca0 | 119 | 78.4 | 80.4 | b | big-endian writer as switch; case-4 instruction order differs |
| 0x005af180 | 120 | 84.6 | 98.1 | b | register choice edx/eax vs ecx/edx for param_1 and the loaded node |
| 0x005aeda0 | 128 | 90.0 | 100.0 | b | last 5 instructions have the uVar2/uVar3 register roles swapped; 4 tail orderings |
| 0x005af860 | 129 | 58.1 | 83.7 | b | stack layout exact, residual is load order/regs; permuter variant changed behaviour (rejected) |
| 0x005ae4c0 | 139 | 35.5 | 79.0 | b | allocator; register assignment/scheduling from first lea differs |
| 0x005af7d0 | 140 | 42.0 | 44.0 | a | orig truncates float inline (fnstcw/fldcw/fistp), DDK calls _ftol; /Op /QIfist /G5-7 /Oi tried |
| 0x005b09c0 | 161 | 79.7 | 81.2 | b | ops 96.9%, flags-byte load fused into test |
| 0x005ae250 | 162 | 64.9 | 73.7 | b | orig pushes edi in prologue before first branch; arg load order; permuter variant read uninitialised local (rejected) |
| 0x005b0cf0 | 170 | 43.9 | 47.0 | b | orig keeps iVar4 in ebx and does not hoist param_6; DDK the opposite |
| 0x005b0bb0 | 190 | 58.8 | 80.0 | b | orig keeps param_3 in ebp, zero-extends bytes via ebx |
| 0x005b03e0 | 199 | 85.9 | 87.2 | b | ops 100%, only frame differs (orig 0xc, ours 0x10) |
| 0x005acda0 | 202 | 44.3 | 50.0 | a | orig inlines float->uint truncation, DDK calls _ftol |
| 0x005af070 | 207 | 97.0 | 98.5 | b | store scheduling of [esi+0x50]/[esi+0x54]; 6 orderings |
| 0x005ae920 | 211 | 96.8 | 100.0 | a | regs and ops 100%, only SIB base/index order in 3 byte accesses |
| 0x005ad320 | 251 | 65.2 | 78.3 | b | register allocation only (ops 98.9%) |
| 0x005af8f0 | 268 | 90.8 | 90.8 | a | ops 100%, only 8 branch targets: cl cross-jumps fail blocks to the default copy |
| 0x005ae800 | 279 | 38.2 | 56.1 | b | orig uses blk+j indexing and a stack bits-left counter; permuter variant changed (k+j*8) (rejected) |
| 0x005ad420 | 287 | 74.5 | 79.1 | b | orig holds zero in ebp; permuter variant stored before allocation (rejected) |
| 0x005ae650 | 292 | 74.1 | 76.9 | a | prologue pushes ebp/esi before first branch (orig) vs after alloc block (DDK) |
| 0x005ad770 | 307 | 69.9 | 84.1 | b | permuter reordered independent stores; orig order differs |
| 0x005adc10 | 321 | 78.9 | 89.8 | b | ops 96.9%, callee arities were wrong in Ghidra |
| 0x005ace70 | 374 | 89.7 | 82.4 | a | DDK cross-jumps first and third return 1; orig keeps 4 exits |
| 0x005ad0b0 | 545 | 56.2 | 73.4 | b | orig reloads param_1 in the search loop |
| 0x005b0740 | 558 | 56.7 | 74.9 | b | ops 98.1%, orig frame 0x24 vs ours 0x28 shifts every param offset |
| 0x005b04b0 | 584 | 30.6 | 52.4 | b | typed rewrite builds ebp frame; orig esp-relative, no frame |
| 0x005acaa0 | 610 | 60.4 | 84.0 | b | memcpy intrinsic, branch order flipped; permuter reordered side-effecting calls (rejected) |
| 0x005afa00 | 751 | 74.7 | 76.8 | b | orig keeps inline epilogue copies where cl merges |
| 0x005afcf0 | 1635 | 31.4 | 60.8 | b | budget stop; 1635 B; raw body compiles under DDK only, about 70 C++ errors from wrong callee arities |

## Raw data (D:/MashedWT/MATCH-S26_data, sha256)
```
09846f678b5c7a0f60cfc3ae12c1410d6087d887552f7b50d6700ccdc61e09cd *decomp.json
4a5e43d457ca05a602ed4f6c42f6e05afd0065c94a344d1e5af1d4b31284a50a *g2s/results.tsv
a934ed201e0e502a462e129059918b9c587745580c81887824130fadf1b19ed1 *g2s_g5/results.tsv
9b1138b6cbb4f75b2a9fa65ef1581d77763bda5b86d861f7d0aa4b9b5c0f9d84 *g2s_game/results.tsv
9093ef4f37eb990c27c03fa1a5e7c348aef3d4a7038ba5144260d50a5c5c3c1b *g2s_o1/results.tsv
1298644ace0896559a18a53817d5bcb5d4d199b80d64d7330d6637b28fff91a4 *pc_funcs.csv
94cb562f5d65ef07589ef4307547ccba7548c8e565541b673d6759734359fb16 *relocheck.py
```
Ghidra pool slots Mashed_pool0 and Mashed_pool10 were used by decomp_pc.py (auto-released). Helpers h1-h4 (claude2,
claude-sonnet-5-5, medium) are closed.
