# MATCH-RWL03 hand-matching helper brief (2026-11-07, from the S19 brief)

You hand-match MASHED.exe game functions: write C that the period compiler turns into the original bytes.
Never write hooks.csv, STUBS.md, UNCERTAINTIES.md, DEFERRED.md, log/orch, original/, any Ghidra project, mashedmod/,
or anything outside the files named below. No git commands that mutate (no commit, stash, checkout, add).
No MASHED.exe / mashed_re.exe runs, no Frida, no Ghidra (do not run decomp_pc.py: everything you need is below).

Worktree (run everything from here): `C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\MATCH-RWL03`

## Tools
- `py -3.12 re/tools/match/match.py <file.c> [--show] [--only 0x..]` compiles with cl 13.00.9176 / c2 13.00.9178 and
  compares each `// MATCH: 0x<va> <symbol>` function with the original. Prints `exact% (regs, ops)` and with --show
  the diff (`left = ours | right = original`). Relocated operands are masked (still use the real globals/constants:
  read them with odis.py).
  A function whose Ghidra size stops before its real end (jump table / padding) counts as matched when the whole
  obj symbol is byte-identical: match.py prints a bytes line for that; say so in the header comment.
- `py -3.12 re/tools/match/odis.py 0x<va>` original disassembly.
- `py -3.12 re/tools/match/match_permute.py <file.c> 0x<va> --rounds 6 --walk 40 --jobs 2 --time 300` mechanical
  rewrites. It uses the FIRST `// MATCH:` for that address in the file. Best variant -> `log/permute/<va8>.c`
  (copy back what helps). Use it on 80-99% functions. Keep --jobs 2 (other helpers share the machine).
- Starting points: `D:/MashedWT/MATCH-RWL03_data/g2s/<va8>.c` (raw Ghidra output made compilable, scored in
  `D:/MashedWT/MATCH-RWL03_data/g2s/results.tsv`) and `D:/MashedWT/MATCH-RWL03_data/decomp.json` (decomp + prototype +
  typed globals + callee prototypes per function; read it with a small python one-liner for your RVA, do not cat it).
- Worked examples: `re/match/pilot/*.c` (header comments say what moved the score), e.g. `f_00423e60.c`,
  `f_small_a.c`, `a5_00410510.c` (switch layout), `m_00422470.c` (register-passed args via static callee).
- Read `verify/matchpilot_20261030/RESULT_MATCH-PILOT.md` section "Findings that change how to match this game"
  before starting (5 short points: static callees for in_EAX/unaff_ESI args, file-static globals, wrong Ghidra
  callee arity, raw converter limits, acos).

## Source form (REQUIRED: these files later become the port's function bodies)
One file per function: `re/match/rwl03/<va8>.c` (va8 = 8 hex digits, lowercase, no 0x). Layout:
1. line 1: the FLAGS line: `// FLAGS: /O2 /Ob1 /G5` (see "S24 additions")
2. a header comment: best exact %, regs %, what you tried, residual class (below).
3. the preamble, copied VERBATIM from `D:/MashedWT/MATCH-RWL03_data/tools/preamble.h` (do not edit it).
   Examples of finished files: `re/match/s19/004c2d90.c` (raw converter, RW core), the 17 raw-converter files already in `re/match/rwl03/`, `re/match/s04/0041e8b0.c` (by hand), `re/match/s02/0040b7b0.c`; more hand-matched files in `re/match/s0[1-8]/` (merged slices). Hand-matched RW-core files with `/G5`: `re/match/s19/*.c` whose line 1 has `/G5`, helper notes `D:/MashedWT/MATCH-S19_data/notes/h5.md`, `h6.md`.
4. globals ONLY through the preamble macros, one per line, with the real address:
   `MGLOBAL(<type>, DAT_xxxxxxxx, 0xxxxxxxxx);` or for arrays `MGLOBALA(<type>, DAT_xxxxxxxx, <N>, 0xxxxxxxxx);`.
   Never a plain `extern` for a global. A global with a struct type: declare the struct typedef above it.
   If matching needs a file-`static` global (pilot finding 2), write `static <type> DAT_x;` under
   `#ifndef __cplusplus` and `MGLOBAL(...)` under `#else`, and say so in the header comment.
5. callee prototypes with the exact arity and the convention the callee's own `ret N` shows. Imports:
   `__declspec(dllimport) <ret> __stdcall Name(...)`.
6. the function(s), each preceded by `// MATCH: 0x<va> <symbol>`. Name the function `FUN_<va8>` exactly as Ghidra.
   A static callee you add for register-passed args (pilot finding 1) also gets its own `// MATCH:` line when it is
   the real callee; name it `FUN_<its va8>` too, and say in the header comment that the file holds it.
- No `__asm`, no emitted bytes, no `#pragma` other than `#pragma intrinsic`. No `#include`.
- The file MUST ALSO compile as C++ (the port includes it into a .cpp). Check every file with
  `cmd /c "verify\matchrwl03_20261108\tools\cxxcheck.bat re\match\rwl03\<va8>.c"` from the **PowerShell** tool (from Git Bash `cmd //c` hangs on this machine) (VS2022 /TP, MATCH_PORT;
  the `vswhere` line in its output is noise; any `error C` line is a failure). Typical fixes: explicit casts for
  void*/int->pointer conversions, real function-pointer types instead of Ghidra's `code`, `0` vs `NULL`, prototypes
  with the right arity. Casts that do not change the DDK score are free; if a C++ fix costs match %, keep the
  match and report the C++ failure in the header comment.

## Budget (bounded: this is a slice of 77 functions, 17 already identical, not a research project)
About 8 min per function under 100 B, 20 min for 100-400 B, 40 min above 400 B. Stop at the budget, keep the best
version, record it. Large functions: get structure (types, loops, switch, case order) right first, then the
permuter. Do the functions in the order given.

## Classify every remaining difference
- (a) compiler-version codegen: survives every plausible rewrite you tried AND is an optimizer/scheduling choice
  that no statement order / type / spelling change moves. Say what you tried.
- (b) source shape: a source form not found yet (struct layout, merged temporaries, inlined callee, loop/switch
  form, register allocation that moves when source changes). "Ran out of budget" is (b).
- (c) tool limit: wrong function boundary, non-standard register convention, C++ EH, thiscall.

## Report back
Write `D:/MashedWT/MATCH-RWL03_data/notes/<your helper id>.md` and end with the same compact text as your final
message: one line per function `0x<va>  size  best%  regs%  cxx:ok|FAIL  class  one-line reason`, plus any static
callee a file holds (`0x<file va> holds 0x<callee va>`). Nothing else.

## RWL03 additions (this overrides anything above that says S24)
- Slice 0x004f0910..0x005405c0, 125 rows of the RenderWare library (21 rows 0x004f0910..0x004f46a0 are the RW D3D9 driver, then RW toolkit rows from 0x0052d8e0). Raw data dir `D:/MashedWT/MATCH-RWL03_data`
  (g2s/, decomp.json, tools/). Sources `re/match/rwl03/<va8>.c` already exist for every row (raw converter output in the
  required form, via verify/matchrwl03_20261108/tools/to_s24.py). 28 rows are already 100% with relocheck OK (results0.tsv best% 100.0): leave them.
  Your list is in your task (smallest first). Where the text above says S24 / s24, read RWL03 / rwl03. Slices S22/S23/S24 data
  dirs are NOT yours: use D:/MashedWT/MATCH-RWL03_data.
- Flags: `// FLAGS: /O2 /Ob1 /G5` (all 28 raw 100% rows need it; /O2 /GS /Ob1 and /O1 win only on low raw forms). If a row stalls on
  prologue/cookie/byte-op residue, try the others once and record which won on line 1.
- Read `git show orch/MATCH-RWCORE-G5:verify/matchrwcoreg5_20261107/flags_brief.md` (last two sections) for residual classes
  that no flag or source form moves (x87 operand order, shrink-wrapped pushes, sibling loop reloads): class (a), move on.
- C++ check: `cmd /c "verify\matchrwl03_20261108\tools\cxxcheck.bat re\match\rwl03\<va8>.c"` from the PowerShell tool, run on
  every file you touch at the end (DDK score first; casts that do not change the DDK bytes are free: typedef `code`
  under `__cplusplus`, real function-pointer types, explicit casts, correct callee arity).
- No-size rows (not in re/console/pc_funcs.csv): score with `py -3.12 verify/matchrwl03_20261108/tools/nosize.py re/match/rwl03/<va8>.c 0x<va>`.
- Several rows are tiny (8-30 B): usually a thunk, a one-store setter or a tail jump (`mov r,[g]; jmp [r+k]`). Read `odis.py`
  first, then write the C that gives those bytes; many of the 25% rows are the raw converter adding a wrong prototype/arity.
- Scratch to `D:/MashedWT/MATCH-RWL03_data/scratch/<id>/`. Notes to `D:/MashedWT/MATCH-RWL03_data/notes/<id>.md`.
- Append one line to your notes file as you finish each function. Budget for your whole list: about 2 h wall; report what you have by then.
