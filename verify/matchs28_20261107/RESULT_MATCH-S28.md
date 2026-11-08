# RESULT MATCH-S28, 2026-11-07

**Verdict: GREEN (match-only, L-93).** Slice 0x005bd9e0..0x005cb2a0, tail of .text: 137 game functions (`slice_rvas.txt`,
`slice_rows.tsv`, `tools/mkslice.py`). 123 library rows inside the range are skipped (C4R-LIBID). The 137 rows are audio
(RenderWare RWS/DirectSound glue, 0x005bd9e0..0x005bff40 and 0x005c7070..0x005c9d10), CRT (0x005c1da2..0x005c6cc0: `_strtod`,
`_strcoll`, `_sscanf`, `__getptd`, libm tails), a mixer/bit-stream block and a render matrix helper.

**81/137 identical** under `re/tools/match/match.py` (XP DDK cl 13.00.9176 / c2 13.00.9178, each file's line-1 flags).
All 81 pass `relocheck.py` with OK (`relocheck.tsv`, 81 lines). 64 have `unchecked=0`. The other 17 have only Win32 imports
or a CRT helper unchecked (`Enter/LeaveCriticalSection`, `ReleaseSemaphore`, `WaitForSingleObject`, `CloseHandle`,
`DeleteCriticalSection`, `CoTaskMemAlloc`, `InterlockedDecrement`, `InitializeCriticalSection`, `__alldiv`, `__allmul`,
`GetSystemTimeAsFileTime`, `__security_check_cookie`, `___security_cookie`). None of the 81 contains `__asm` or `naked`.
All 137 files pass the VS2022 C++ check (`cxxmany.bat`, 137 CXXOK, `cxx_all.txt`). Sources: `re/match/s28/<va8>.c`.
Scores, raw-converter score, source sha256 prefix: `results.tsv`.

This step does no body swap, edits no trackers (hooks.csv, CHANGELOG, UNCERTAINTIES, STUBS, pc_funcs.csv) and runs no gates.
Step C4R-MATCHBATCH-2 promotes the 81 identical rows.

**Tool overlay.** Three files from queued branches were overlaid UNCOMMITTED in the worktree and restored with
`git checkout` before the commit: `re/tools/match/match.py` (orch/C4R-MATCHPY-REL32), `re/tools/match/relocheck.py`
(orch/MATCH-S17), `re/console/pc_funcs.csv` (orch/MATCH-PCFUNCS-REGEN). Every row of this slice has a size in the
regenerated pc_funcs.csv, so no row is scored as a no-size row and there is no `nosize_ends.tsv`.

## Flags

| line 1 `// FLAGS:` | identical rows |
|---|---|
| `/O2 /Ob1 /G5` (RW build) | 75 |
| `/O1` (CRT) | 5 |
| `/O1 /GS` (CRT with stack cookie, 0x005c31d0) | 1 |

The raw converter ran under `/O2 /Ob1 /G5`, `/O2 /GS /Ob1` and `/O1` (`g2s_g5`, `g2s_game`, `g2s_o1`; `tools/g2s_best.py` merges
the best per row, ties to `/G5`). 100% raw rows: 41 under `/G5`, 28 under the game flags (`/O2 /GS /Ob1`), 10 under `/O1` (overlapping); the merged best has 41
identical with relocheck OK. Helpers found the audio rows (0x005bd9e0..0x005c9d10) match under `/G5` and not under `/O1`;
only the CRT rows (`_sscanf` 0x005c22bd, `_strcoll` 0x005c2025, 0x005c2a0e, 0x005c27e6, 0x005c228e, 0x005c31d0) need `/O1`.

## Counts

| | count |
|---|---|
| slice functions | 137 |
| raw converter (merged best) at 100%, relocheck OK | 41 |
| hand-matched or hand-fixed to 100% (helpers h1-h3, `notes/h*.md`) | 40 |
| **identical total** | **81** |
| below 100% | 56: (a) 3, (b) 39, (c) 14 |

20 of the raw-converter files failed the C++ check (void*/undefined4 casts, `code` typedef called with arguments, a callee
prototype with the wrong arity) and were fixed with casts and prototypes only. Each says so in its header; match 100% and
relocheck OK were re-run after the edits.

**Static callees.** 0x005c2f84 holds 0x005c2f8d (a 9-byte fragment that falls through; the file contains the 0x005c2f8d body, below 100%).
No other file holds a static callee. 0x005c22bd is the CRT `_sscanf` (stack FILE image + va pointer).

## Below 100% (budget spent)

Classes: (a) compiler-version codegen, (b) source shape not found within the budget, (c) tool limit. 14 class (c) rows are
CRT or x87 hand-assembly (`_strncat` dword scan 0x005c2300, SSE2 `exp` 0x005c419e, x87 matrix multiply 0x005cb2a0, vec3*mat3
0x005cb160), SEH funclets and fragments that run in a caller's ebp frame (0x005c1da2, 0x005c1e27, 0x005c2f70, 0x005c2f84,
0x005c340a, 0x005c3403, 0x005c35d6), the `sqrt` tail 0x005c2f8d, and two adjustor thunks (0x005bf350, 0x005bf670).
0x005c5780 is a compile-only raw form, not port-ready. 0x005c7500 uses one `__asm` macro for an inline `fnstcw/fldcw/fistp`
and is at 89.3% (not counted as identical).

| rva | size | best % | regs % | class | residual |
|---|---|---|---|---|---|
| 0x005c4ad0 | 205 | 96.7 | 96.7 | a | imul operand order [esi+0xc]*[esi+8] vs orig reversed |
| 0x005bf8a0 | 73 | 95.7 | 95.7 | b | store [esi+0x13c]=0 scheduled after reload of [esi+0x130] |
| 0x005c4e20 | 383 | 95.0 | 95.0 | b | two flag temps live in arg1/arg2 stack slots |
| 0x005c4c60 | 186 | 93.0 | 94.4 | b | schedule of final imul/add/store; flags /O2 /Ob1 /G5 |
| 0x005beae0 | 108 | 91.9 | 100.0 | a | post-increment temp in ecx vs original edx (ops identical) |
| 0x005be260 | 481 | 90.6 | 90.6 | b | success epilogue duplicated in original after FUN_005aef30 test; ours merges |
| 0x005c7500 | 101 | 89.3 | 89.3 | b | inline x87 ftol via one __asm block; saved control word at esp+4 not esp+6 |
| 0x005bdff0 | 135 | 89.1 | 98.2 | b | switch var in eax / base in ecx swapped vs original |
| 0x005c88b0 | 86 | 88.9 | 88.9 | b | rep stosd dest reg (edi=[esi+8] vs reuse of esi) |
| 0x005bff40 | 293 | 86.1 | 86.1 | b | prologue push esi/edi placed after early-return checks (original before) |
| 0x005bf710 | 192 | 83.6 | 83.6 | b | tail block layout: success epilogue vs free-call block order |
| 0x005bfa40 | 108 | 82.2 | 82.2 | b | orig keeps two separate return-E_FAIL epilogues; flag must be int |
| 0x005be9f0 | 111 | 80.0 | 82.2 | b | ebp/ebx save placement + load order |
| 0x005c47e0 | 710 | 80.0 | 81.2 | b | x87 compare shapes + schedule (permuter +17.6) |
| 0x005bfd10 | 136 | 77.2 | 77.2 | b | shared fail epilogue / 4-reg prologue not reproduced |
| 0x005c9770 | 310 | 76.0 | 80.0 | b | prologue load order, add ebx,ebx vs lea (permuter +11) |
| 0x005c9730 | 14 | 75.0 | 75.0 | a | pop ecx vs add esp,4 for 1-arg cdecl cleanup (flags/ret type tried) |
| 0x005be540 | 171 | 73.2 | 73.2 | b | failure epilogues cross-jumped by ours, separate in original; flag sweep did not move it |
| 0x005bf4d0 | 108 | 70.5 | 81.8 | b | original pushes esi in prologue and loads loc into esi; ours late push, loc in eax |
| 0x005bf350 | 31 | 70.0 | 70.0 | c | adjustor-thunk shape (arg rewritten in place + jmp), C cannot emit |
| 0x005c1f97 | 142 | 69.4 | 69.4 | b | _strtod /O1: original pushes eax(0) twice before strlen, returns fldz directly; relocheck BAD only from misalignment |
| 0x005c4670 | 263 | 68.2 | 87.5 | b | frame hand-laid (uint l[11]); arg-push/prologue scheduling differs |
| 0x005bdf40 | 162 | 60.6 | 90.1 | b | sete temp of p[1]==p[2] + load schedule; memcpy/memset intrinsics |
| 0x005c6b60 | 75 | 54.8 | 77.4 | b | param_1 lands in esi vs original ecx; divide-by-64 form found, allocation not reached |
| 0x005be990 | 89 | 48.5 | 72.7 | b | flag kept in param slot [esp+0xc], only esi/edi; ours uses ebx |
| 0x005bf160 | 198 | 47.7 | 81.8 | b | register allocation (zero reg edi) and loop-bottom/tail block order not reached |
| 0x005c4fa0 | 416 | 44.7 | 62.9 | b | raw converter shape; x87 compare idioms and register allocation not reworked |
| 0x005bde70 | 120 | 42.3 | 65.4 | b | original keeps p3 in ebx and const 1 in edi with duplicated tails; allocation not reached |
| 0x005bdad0 | 407 | 40.9 | 48.9 | b | ebp frame (/Oy-); zero kept in edi, inline 64x64 mul form differs |
| 0x005c6cc0 | 603 | 40.6 | 42.4 | b | macro-expanded mk() conditional: original keeps the constant path unfolded and duplicates tails |
| 0x005bf8f0 | 290 | 40.5 | 69.0 | b | loop counter lives in arg1 slot, orig never uses ebp |
| 0x005bf670 | 17 | 40.0 | 40.0 | c | adjustor-thunk shape (arg rewritten in place + jmp import), C cannot emit |
| 0x005bf050 | 99 | 38.1 | 57.1 | b | orig reloads array ptr/count each iteration; sub eax,4 not folded |
| 0x005c8910 | 98 | 37.8 | 70.3 | b | register allocation of arg compares / esi base |
| 0x005c7a90 | 617 | 32.0 | 29.1 | b | nested inlined log2 helper re-queries FUN_005aee20 per branch (172 vs 97 instrs) |
| 0x005c8120 | 618 | 32.0 | 29.1 | b | twin of 5c7a90: inlined bit-scan helper nesting not recovered (97 vs 172 ins) |
| 0x005c95b0 | 204 | 30.1 | 53.0 | b | bit-range locals live in stack bytes; clean rewrite scored worse |
| 0x005c7330 | 449 | 29.3 | 43.1 | b | inline fnstcw/fistp ROUND + stack-dword float temps (ours calls __ftol) |
| 0x005c93a0 | 240 | 28.9 | 51.5 | b | nested lg8/lg16/lg32 helper shape (+7 then +0x10 adds) |
| 0x005bf0c0 | 145 | 26.5 | 41.2 | b | loop-invariant array/count hoisted by ours, reloaded per iteration by original |
| 0x005cb160 | 98 | 24.4 | 24.4 | c | hand-scheduled x87 vec3*mat3 with saved edx/ebx/eax |
| 0x005c98d0 | 1063 | 20.7 | 32.0 | b | raw Ghidra text, mixer loops (unroll-by-4 + tail) not reworked, budget |
| 0x005c94c0 | 227 | 17.5 | 35.1 | b | dead max-run kept in memory + param_2 reloaded in bit loop in original; raw structure only |
| 0x005cb2a0 | 355 | 15.6 | 17.5 | c | hand-written x87 asm matrix multiply (FPU-stack operands, fxch), not expressible in C |
| 0x005c35d6 | 2672 | 14.3 | 24.3 | c | SEH prolog frame, ebp-relative INC helper 0x5c35c0, alloca |
| 0x005c5780 | 4178 | 13.6 | 20.1 | b | compile-only raw form (int/float local confusion patched, NAN placeholder); needs full rewrite |
| 0x005c2f70 | 20 | 12.5 | 12.5 | c | x87 ST0 funclet; modelled as f(double) |
| 0x005c2f8d | 157 | 11.9 | 16.7 | c | CRT sqrt: ZF from register-convention call, fsqrt, jumps into shared tails |
| 0x005c9d10 | 1472 | 11.8 | 16.3 | b | raw shape only; 64-bit fixed-point helpers (__aullrem/__aullshr/__allmul, x87 quotient) and ebp frame not reproduced |
| 0x005c340a | 327 | 9.8 | 13.4 | c | CRT math tail running in a caller's ebp frame (no prologue), not a standalone function |
| 0x005c2300 | 309 | 4.1 | 8.9 | c | hand-written CRT assembly strncat (dword zero-scan), not expressible in C |
| 0x005c419e | 639 | 2.6 | 2.6 | c | hand-written SSE2 libm exp, argument in xmm0; call-through placeholder only |
| 0x005c2f84 | 9 | 2.4 | 2.4 | c | 9-byte fragment falls through into 0x005c2f8d (file holds 5c2f8d body) |
| 0x005c1da2 | 10 | 0.0 | 0.0 | c | ebp-frame funclet (SEH finally body) of the parent function, no prologue |
| 0x005c1e27 | 10 | 0.0 | 0.0 | c | SEH funclet reading [ebp-0x1c]; modelled as f(FILE*) |
| 0x005c3403 | 7 | 0.0 | 0.0 | c | first insn of a CRT FP-exception fragment, caller ebp frame (empty stub) |

## Gates

Not run: match-only, the merge train runs them. No build input changed (`re/match/s28/` and `verify/` only).

## Raw data (not committed), `D:/MashedWT/MATCH-S28_data`

- decomp_raw.json (`re/tools/decomp_pc.py --port --callees --create`, run from the main checkout, pool slot Mashed_pool0, released) sha256 70fe3f9200ce50373f3c712596da3fe76f2abc6bf7268cd799fa05b9d8b0627b (decomp.json is the same bytes, no non-ASCII text)
- g2s/results.tsv (merged best) sha256 027f88f90060bae7940cd0d22c52cf4a8b93176cfd43cf0855199ee28bacc7ef
- g2s_g5/results.tsv sha256 a393dccc227c6a6a07c7997f7a9c5a218d596b8e0c9bc55f72d976777fd234d9
- g2s_game/results.tsv sha256 5d7c4be309793ba2833cffd07fae0ed5f01b4b278f5a90c4dc4557b575653608
- g2s_o1/results.tsv sha256 bd9d0188e2d43123aaa9c49c00a7a61de93351aceee1c62fab2dfc6e72c871ed

## Reproduce

From the worktree (with the three overlaid tool files): `py -3.12 verify/matchs28_20261107/tools/collect.py > results.tsv`,
`py -3.12 re/tools/match/relocheck.py re/match/s28/<va8>.c` per identical row, `tools/cxxmany.bat` for the C++ check,
`tools/mkresult_tables.py` for the tables. Helper brief: `BRIEF.md`; row assignment: `todo_h1.tsv`, `todo_h2.tsv`, `todo_h3.tsv`.
