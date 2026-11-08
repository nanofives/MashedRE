# RESULT MATCH-RWL06, 2026-11-08

**Verdict: GREEN (match-only, L-93, partial slice with honest counts).** Slice 0x00565590..0x005cb42b: 126 library rows
(121 `third-party-library[RenderWare-Physics-3.7]`, 5 `third-party-library[renderware]`; list `D:/MashedWT/MATCH-RWLIB_data/rwl06_rows.tsv`).
125 rows have a source in `re/match/rwl06/<va8>.c`. 0x005cb404 has none: it is hand-written asm (below).

**38/126 identical** under `re/tools/match/match.py` (XP DDK cl 13.00.9176 / c2 13.00.9178, line-1 flags `/O2 /Ob1 /G5`),
all 38 with `relocheck.py` OK (`relocheck.tsv`). 37 have unchecked=0. 0x0057c270 has one unchecked call target: Ghidra
labels it `RpAtomicRegisterPlugin`, but the bytes call 0x004e7d40 with 5 pushed args. RWL05 found the same at 0x00562460 and 0x005578a0.
All 38 pass the VS2022 C++ check. Scores, sha256, class and reason per row are in `results.tsv`.
No trackers, no build, no gates. Promotion is for a later `c4_reclass.py` batch.

## Flags evidence (g2s raw converter on all 126 rows)

| flag set | compiled | 100% |
|---|---|---|
| `/O2 /Ob1 /G5` (RW build) | 102 | 13 |
| `/O2 /GS /Ob1` (game) | 102 | 9 |
| `/O1` | 102 | 3 |

4 rows are 100% only under `/G5`. No row is 100% only under the game flags or `/O1`. The game flags beat /G5 on 14 low
raw rows (best 50.0 at 0x0057c270, which later matched 100% under /G5). `/O1` beat both on 11 low rows (best 77.0 at 0x0056d350,
which helper h2 moved back to /O2 /Ob1 /G5: 55.9 / regs 94.9). This agrees with RWL05: Physics 3.7 is built `/O2 /Ob1 /G5`.
Every C source carries `/O2 /Ob1 /G5` on line 1. Helper h3 changed several raw files from `/GS` to `/G5`.
The three `/O1` files are the asm rows 0x005cb000/07f/0ef: they are the raw-run best, kept for the record only.
24 rows did not compile in raw form under any flag set (Ghidra casts such as `undefined *` to float, undeclared `ABS`/`SQRT`).
All but 0x0056dd40 were made to compile.

## Counts

| | |
|---|---|
| rows | 126 |
| raw converter 100% (relocheck OK) | 13 (6 needed C++-check edits only: variadic `code` typedef under C++, casts, callee arity; `tools/cxxfix.py`) |
| hand-matched to 100% (3 helpers, `notes/`) | 25 (h1 10, h2 9, h3 6) |
| **identical** | **38** |
| below 100% | 88: class (a) 7, (b) 68, (c) 13 |
| no Ghidra size | 0 |

Class (c), tool limit / not C (13):
- 0x005cb000, 0x005cb07f, 0x005cb0ef, 0x005cb404, 0x005cb42b: **hand-written asm, not compiled C** (checked with `odis.py`).
  The first three are a bare `push ebp; mov ebp,esp` frame that saves eax/ebx/ecx/edx and loads args into fixed registers
  for an x87 matrix-vector product. 0x005cb404 is `push eax; wait; fnstcw [0x915000]` plus mask `and 0xfffffcff / or 0x3f`, then `fldcw`.
  0x005cb42b is `fldcw [0x915004]; ret`. A naked-asm source would not count, so they are recorded and not attempted.
  The other four keep their raw g2s file for the record (scores 6-50%). 0x005cb404 has no file because the converter failed on `DAT_00915000_2`.
- 0x0056ac40, 0x0056dd40: SSE bodies (xmm, rsqrtps/movmskps). DDK cl cannot emit them. 0x0056dd40 is the one file that does not compile (DDK or C++): raw body with `in_XMM` registers.
- 0x0056a7a0, 0x0056adb0: `and esp,-16` aligned frames with about 30 stack args. The period compiler does not emit this.
- 0x005749b0, 0x00574920: C++ EH frame plus inline SSE xorps / cpuid.
- 0x00574a20: inline cpuid.
- 0x00569690: by-value 0x88-byte struct plus 12 stack params. Raw form only, not attempted (its sibling 0x0056aae0 did match, so this is a budget call, not a proven limit).

Near misses (80-99%), the cheap next targets: 0x0057c0d0 98.7 (a, one mov a slot late), 0x005735f0 98.0 (b, one mov ecx slot),
0x0057ae30 90.7 (a), 0x00566c10 88.9 (b, final return-1 block), 0x0057bf30 84.6 (b), 0x00578b20 81.2 (b).

## Below-100% rows

| rva | size | best% | class | reason |
|---|---|---|---|---|
| 0x005655a0 | 468 | 14.8 | b | quaternion-to-matrix: original stores every product in its own 4-byte slot (0x74 frame), x87 order |
| 0x00565780 | 620 | 30.3 | b | loop nest layout/regalloc; int count param, inline fsqrt |
| 0x005659f0 | 179 | 58.8 | b | double temps t0..t5, spill pattern differs |
| 0x00565ab0 | 272 | 63.2 | b | c[16] RwMatrix frame found (0x88); identity stores use registers for constants |
| 0x00565cd0 | 118 | 68.3 | a | x87 add/mul operand order canonicalised by compiler |
| 0x00565d50 | 415 | 67.7 | b | file already compiles (float10 temps, sqrt intrinsic); float/double temp variants are worse; compare forms (fcom mem vs fld/fcompp) differ |
| 0x00565ef0 | 171 | 45.5 | b | fcompp vs fcom mem form and param reg order not moved |
| 0x00566050 | 425 | 43.0 | b | x87 scheduling, 6 derived values stored then scaled |
| 0x00566200 | 411 | 39.7 | b | (prior run) see header |
| 0x00566420 | 382 | 31.1 | b | rewritten with proper float *poly / *dpoly parameters; the original keeps x in the FPU across the loop, double locals tried and are worse |
| 0x005665a0 | 542 | 27.8 | b | (prior run) raw form kept |
| 0x005667c0 | 109 | 63.4 | a | x87 operand order and fcom form |
| 0x00566830 | 163 | 48.4 | b | fabs intrinsic ok; original keeps param_2 in ebx and builds the selector with add/or/add/or/shl 2 while the compiler distributes the shift (shl 3 + shl 2); permuter variants change semantics and are not used |
| 0x00566b40 | 198 | 76.7 | a | float fVar2, sqrt intrinsic; x87 operand order of the sum of squares (source permutations change nothing) |
| 0x00566c10 | 141 | 88.9 | b | only the final return-1 block layout differs |
| 0x00566ca0 | 167 | 73.7 | b | second dot product emitted [2],[1],[0] under negation, tail layout |
| 0x00566d50 | 334 | 75.7 | b | (prior run) see header |
| 0x00566ea0 | 500 | 42.3 | b | flags fixed to /G5 (was /GS), DAT consts float, d[3] array; reg allocation differs (esi/edi extra), structure only (>400 B) |
| 0x005670a0 | 532 | 41.8 | b | d[3]/c[5] arrays (dead-store trap) and float constants raise ops to 92.7; original frame is 0x60 (spill temps) so offsets shift; fVar3..5 double |
| 0x005672c0 | 133 | 18.8 | b | long align/sum expression, compiler reschedules; single-expression form kept |
| 0x00567350 | 640 | 8.2 | b | flags fixed (was /GS), param_5/4 as cdq form; the original total is built as an add chain of separately aligned terms; structure only |
| 0x005676f0 | 1289 | 4.0 | b | long aligned-size add chain + arena carve; not reworked, compiler orders the chain differently |
| 0x005685f0 | 431 | 69.1 | b | (prior run) hoisted mov esi,1 and push edi |
| 0x005687d0 | 129 | 58.7 | b | 8 aligned terms from the bytes (Ghidra formula wrong), compiler orders the add chain differently |
| 0x00568990 | 675 | 40.6 | b | 675 B integer function, frame 0x5c vs 0x54 and spill layout |
| 0x00568c80 | 109 | 73.0 | b | instruction stream identical, register assignment differs |
| 0x00568d20 | 175 | 51.4 | b | double align (+0x1f) found; original keeps param_1 in esi and the new record in edi, ours swaps them; loop index forms and declaration order tried |
| 0x00569690 | 342 | 36.8 | c | by-value 0x88 struct args on stack plus 12 params, raw Ghidra form only |
| 0x00569e90 | 951 | 26.5 | b | 336 B, in_stack_ vars turned into params (39), x87 + spill layout |
| 0x0056a250 | 503 | 26.5 | b | arg spill layout: original keeps 21 stack params live in many copies |
| 0x0056a450 | 836 | 3.3 | b | 453 B, our body 451 ins vs 211 original, loops/locals shape unknown; in_stack_ vars turned into params (extended signature to 53) |
| 0x0056a7a0 | 826 | 7.0 | c | prologue push ebp / and esp,-16 and about 30 stack args (in_stack_*): 16-byte-aligned frame the period compiler does not produce; raw form kept |
| 0x0056ac40 | 357 | 4.1 | c | SSE xmm body (cmpltps/movmskps/andnps), rewritten as scalar 4-lane loops with extended 15-param signature; bytes cannot match under DDK cl |
| 0x0056adb0 | 1370 | 10.6 | c | prologue push ebp / and esp,-16 and in_stack_* args: 16-byte-aligned frame the period compiler does not produce; ROUND replaced by a plain cast; raw form kept |
| 0x0056bb80 | 337 | 41.3 | b | fabs/sin/cos intrinsics ok; compare-with-memory form and temp scheduling differ (4 compare rewrites tried) |
| 0x0056be80 | 534 | 45.3 | b | x87 order and temp slot layout, 534 B, structure only |
| 0x0056c8e0 | 434 | 21.1 | b | raw Ghidra form; original accumulates sequentially from out[] loaded first (fadd st(k)) and mixes (p4+2)*0x20 with iVar15+0x44 addressing; not rebuilt |
| 0x0056d350 | 158 | 55.9 | b | flags /O2 not /O1; only eax/ecx assignment differs |
| 0x0056d3f0 | 2375 | 24.3 | b | raw form, inline sqrt and C++ callee cast fixed; real signature has six 12-byte struct params (not reworked) |
| 0x0056dd40 | 2357 | - | c | 2357 B body with rsqrtps/movmskps SSE and in_XMM registers, raw Ghidra form does not compile under DDK or VS2022 (no scalar rewrite done) |
| 0x0056f350 | 1918 | 15.0 | b | compiles (earlier fix), sqrt/fabs intrinsics; raw Ghidra form |
| 0x0056fad0 | 186 | 76.5 | a | V3 struct copy found; the fmul/fld operand order of every product is canonicalised by the compiler (source flips change nothing); permuter no gain |
| 0x0056fb90 | 770 | 28.5 | b | 770 B quaternion/matrix x87 function, structure only |
| 0x00570090 | 10500 | 9.2 | b | now compiles (local_2ec/2a0/pfVar13 split into pointer and float variables, FUN_0056f1f0 returns double, FUN_0056fb90 p3 float**); 893 vs 2328 ins: Ghidra scalar locals are really arrays/structs (dead-store elimination), not rebuilt (>2000 B structure-only cap) |
| 0x005729a0 | 3138 | 31.4 | b | 3138 B structure only (45 min class), compiles after prototype fixes |
| 0x005735f0 | 126 | 98.0 | b | double temps; one mov ecx scheduling slot differs |
| 0x00573670 | 544 | 36.9 | b | struct-relative field access (piVar9+0x70..) and double temps for the fld/fstp copies (23.5 to 36.9); frame/register order differs (0x58 vs 0x54) |
| 0x00573890 | 313 | 67.6 | b | (prior run) stack slot / operand load order |
| 0x005739d0 | 1844 | 29.6 | b | not reworked beyond raw (structure-only budget) |
| 0x005742c0 | 1490 | 11.8 | b | 1490 B x87 function, structure only (compiles after PTR_DAT_005ceabc typed float) |
| 0x00574920 | 94 | 12.5 | c | C++ EH frame + inline cpuid asm |
| 0x005749b0 | 77 | 8.0 | c | C++ EH frame + inline SSE asm, no __asm allowed |
| 0x00574a20 | 148 | 48.1 | c | inline cpuid asm, no intrinsic in DDK |
| 0x00574ad0 | 1615 | 21.0 | b | /GS flag fixed to /G5, fabs/sqrt intrinsics; raw Ghidra form otherwise (structure only) |
| 0x00575120 | 208 | 69.3 | b | two separate return-0 epilogues + esi base register |
| 0x005752b0 | 680 | 56.0 | b | min() select shape on the x87 stack; uint types fixed |
| 0x00575560 | 618 | 31.2 | b | cmp ax 16-bit and loop reg alloc, 216 B-class int function; regs-renamed 59% (structure differs in locals/spill) |
| 0x00575880 | 732 | 59.1 | b | int pointer param_1, lo/hi locals (lo reuses the 0x74 slot), flag branch swapped, x<=y form, ja/jae order (36.9 to 59.1); register allocation differs |
| 0x00575b60 | 250 | 79.8 | b | 3x3 product term order r2,r0,r1 vs compiler r2,r1,r0 |
| 0x00575c60 | 890 | 4.4 | b | compiles (earlier fix), goto-based raw form (LAB_00575e25 ...), not rebuilt |
| 0x00575fe0 | 1632 | 13.9 | b | 1632 B x87 function, structure only |
| 0x00576880 | 4958 | 23.9 | b | raw, SQRT replaced by inline sqrt; structure-only budget |
| 0x00577be0 | 196 | 60.8 | b | x87 register-held components of p2 across both dot products |
| 0x00577cb0 | 513 | 29.8 | b | loop register allocation, cross-sum order (raw kept) |
| 0x00577ec0 | 1497 | 15.1 | b | (prior run) raw form kept |
| 0x005784a0 | 366 | 69.6 | b | dz spill / frame size 0xc, dot term order |
| 0x00578610 | 1287 | 22.1 | b | 398 B x87/regs, structure only |
| 0x00578b20 | 176 | 81.2 | b | edi/eax load order for param_1 copy |
| 0x00578bd0 | 213 | 41.7 | b | strength-reduced pointer lea edi/esi, param_2 copy reg |
| 0x00578cb0 | 212 | 70.3 | a | float local_8[2] array, compare form; x87 operand order of the dot product differs (6 permutations tried) |
| 0x00578d90 | 185 | 78.3 | b | float consts at 0x5ceabc/0x5cc558, first dot term order |
| 0x00578f20 | 207 | 64.2 | b | loop counter kept in memory, register pressure differs |
| 0x00578ff0 | 2899 | 13.8 | b | compiles (earlier fix), raw Ghidra form |
| 0x00579c00 | 328 | 72.2 | b | x87 min/max keep in st0, reg alloc, loop counter memory |
| 0x00579d50 | 243 | 34.7 | b | arrays for locals, branch order swapped; call scheduling differs |
| 0x00579e50 | 134 | 60.4 | b | callee-saved reg assignment ebx/edi swapped |
| 0x00579ee0 | 866 | 28.0 | b | not reworked beyond raw (structure unknown: GJK-style simplex det update) |
| 0x0057a250 | 1038 | 54.7 | b | v[3]/u[3]/pts[6] arrays, param_4 is an int pointer (fMax float), fabs intrinsic (ABS was an undeclared function), float globals, lim local; permuter swaps of independent statements kept |
| 0x0057a660 | 824 | 26.6 | b | (prior run) raw form kept |
| 0x0057a9a0 | 1035 | 34.1 | b | (prior run) raw form kept |
| 0x0057ae30 | 348 | 90.7 | a | fadd/fmul operand order and shift distribution (shl 3 + shl 2 against add,add,shl 2) are compiler canonicalisation; flips, temp t and permuter tried |
| 0x0057bf30 | 407 | 84.6 | b | x87 float10 temp ordering and slot reuse |
| 0x0057c0d0 | 221 | 98.7 | a | one mov scheduled a slot late (d,a,b vectors + two cross products as float arrays) |
| 0x005cb000 | 127 | 7.9 | c | hand-written asm (pushes eax/ebx/ecx/edx after a bare ebp frame, or wait/fnstcw/fldcw on 0x00915000/4); not compiled C |
| 0x005cb07f | 112 | 12.3 | c | hand-written asm (pushes eax/ebx/ecx/edx after a bare ebp frame, or wait/fnstcw/fldcw on 0x00915000/4); not compiled C |
| 0x005cb0ef | 113 | 6.1 | c | hand-written asm (pushes eax/ebx/ecx/edx after a bare ebp frame, or wait/fnstcw/fldcw on 0x00915000/4); not compiled C |
| 0x005cb404 | 39 | - | c | hand-written asm (pushes eax/ebx/ecx/edx after a bare ebp frame, or wait/fnstcw/fldcw on 0x00915000/4); not compiled C |
| 0x005cb42b | 7 | 50.0 | c | hand-written asm (pushes eax/ebx/ecx/edx after a bare ebp frame, or wait/fnstcw/fldcw on 0x00915000/4); not compiled C |

## C++ check
124/125 files pass VS2022 /TP with MATCH_PORT (`cxx_check.txt`). The one failure is 0x0056dd40 (class c, SSE, does not compile under DDK either).

## Notes and follow-ups
- Static callees without rows: no file holds a second MATCH function.
- Helper caveats (h1): the in_stack_ rows 0x00569e90, 0x0056a450, 0x0056a250, 0x0056ac40 and 0x005729a0 were made to compile with
  prototype and cast fixes. Their semantics were not byte-checked against the original, so treat those files as structure drafts.
- Helper findings: a scalar local whose address goes to a callee must be an array, or its stores are dropped. Several raw files typed
  struct pointers as float and needed int. `ABS(` / `SQRT(` in raw files were undeclared and became fabs/sqrt intrinsics.
- The h2 notes file was overwritten once during the session's license moves and rebuilt from file headers and match.py scores.
  Each row's header comment is authoritative.
- The session moved between licenses twice while helpers ran. The resumed helpers continued from file state, and a WIP checkpoint commit kept progress.
- Follow-up: 6 near misses above, then the 0x0056aae0 pattern applied to 0x00569690.
  Large x87 rows (0x00570090 10500 B, 0x00576880 4958 B, 0x005729a0, 0x00578ff0, 0x0056d3f0, 0x0056dd40) are structure only.
- Tool changes: none under `re/tools/match/`. The RE_G relocheck fix from orch/MATCH-RWL01 is already on orch/integration
  (`relocheck.py:22`), and no BAD came up.

## Raw data (not committed), `D:/MashedWT/MATCH-RWL06_data`
decomp_raw.json sha256 92a80ff3a0a5e17cf7b337112a62112773cfbd0c5ca38d0ad94cc7b0a95abb44.
g2s_g5/game/o1 results.tsv sha256 365d4d24c0dbd7c92773f19fe4609bf69a685c23552e60513348bdeca0425d18 /
864f77264bcb9eaa9325b3cb090d20aa336082d71e54f965a7f2a9155cba4f54 / cc57c5c95df240d6e69279e7617772869fc3eb6a271e2054dec011c3d80d4095.
decomp_pc.py ran from the main checkout (pool slot Mashed_pool0), because a worktree has no `mashed_pool`.

## Reproduce
`py -3.12 verify/matchrwl06_20261108/tools/collect.py > verify/matchrwl06_20261108/results1.tsv`, then `tools/mkresults.py`.
Run `relocheck.py re/match/rwl06/<va8>.c` for each identical row. Helper brief: `BRIEF.md`. Assignment: `helper_assign.txt`.
