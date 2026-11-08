# MATCH-S28 hand-matching helper brief (2026-11-07, from the S19 brief)

You hand-match MASHED.exe game functions: write C that the period compiler turns into the original bytes.
Never write hooks.csv, STUBS.md, UNCERTAINTIES.md, DEFERRED.md, log/orch, original/, any Ghidra project, mashedmod/,
or anything outside the files named below. No git commands that mutate (no commit, stash, checkout, add).
No MASHED.exe / mashed_re.exe runs, no Frida, no Ghidra (do not run decomp_pc.py: everything you need is below).

Worktree (run everything from here): `C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\MATCH-S28`

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
- Starting points: `D:/MashedWT/MATCH-S28_data/g2s/<va8>.c` (raw Ghidra output made compilable, scored in
  `D:/MashedWT/MATCH-S28_data/g2s/results.tsv`) and `D:/MashedWT/MATCH-S28_data/decomp.json` (decomp + prototype +
  typed globals + callee prototypes per function; read it with a small python one-liner for your RVA, do not cat it).
- Worked examples: `re/match/pilot/*.c` (header comments say what moved the score), e.g. `f_00423e60.c`,
  `f_small_a.c`, `a5_00410510.c` (switch layout), `m_00422470.c` (register-passed args via static callee).
- Read `verify/matchpilot_20261030/RESULT_MATCH-PILOT.md` section "Findings that change how to match this game"
  before starting (5 short points: static callees for in_EAX/unaff_ESI args, file-static globals, wrong Ghidra
  callee arity, raw converter limits, acos).

## Source form (REQUIRED: these files later become the port's function bodies)
One file per function: `re/match/s28/<va8>.c` (va8 = 8 hex digits, lowercase, no 0x). Layout:
1. line 1: the FLAGS line: `// FLAGS: /O2 /Ob1 /G5` (see "S28 additions")
2. a header comment: best exact %, regs %, what you tried, residual class (below).
3. the preamble, copied VERBATIM from `D:/MashedWT/MATCH-S28_data/tools/preamble.h` (do not edit it).
   Examples of finished files: `re/match/s19/004c2d90.c` (raw converter, RW core), the 17 raw-converter files already in `re/match/s28/`, `re/match/s04/0041e8b0.c` (by hand), `re/match/s02/0040b7b0.c`; more hand-matched files in `re/match/s0[1-8]/` (merged slices). Hand-matched RW-core files with `/G5`: `re/match/s19/*.c` whose line 1 has `/G5`, helper notes `D:/MashedWT/MATCH-S19_data/notes/h5.md`, `h6.md`.
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
  `cmd /c "D:\MashedWT\MATCH-S28_data\tools\cxxcheck.bat re\match\s28\<va8>.c"` from the **PowerShell** tool (from Git Bash `cmd //c` hangs on this machine) (VS2022 /TP, MATCH_PORT;
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
Write `D:/MashedWT/MATCH-S28_data/notes/<your helper id>.md` and end with the same compact text as your final
message: one line per function `0x<va>  size  best%  regs%  cxx:ok|FAIL  class  one-line reason`, plus any static
callee a file holds (`0x<file va> holds 0x<callee va>`). Nothing else.

## Slice additions (read these too)
- Run Python with `MSYS_NO_PATHCONV=1` exported if you use Git Bash (otherwise `/O2` becomes a path), and pass
  Windows-style paths (`C:/...`, `D:/...`) to Python, never `/c/...`.
- Every 100% file must also pass `py -3.12 re/tools/match/relocheck.py re/match/s28/<va8>.c` -> `OK` with
  `unchecked=0` where possible. Relocheck resolves `MGLOBAL`/`MGLOBALA` names to their address and `FUN_<va8>` to
  its RVA, so: call every game callee as `FUN_<va8>` (rename the converter's names such as `RpClumpDestroy` to
  `FUN_004e6e00`; the callee's address is in decomp.json `callees`, `Name@0x...`), and declare every global through
  `MGLOBAL`/`MGLOBALA` (a string-typed global such as `s_training_005f33f8` -> `MGLOBALA(char, s_..., <N>, 0x...)`).
  Imports and CRT helpers stay unchecked by name (fine). A BAD line is a wrong global/callee: fix it.
- Tail-jump dispatchers (`mov r, [g]; jmp [r + k]`): this compiler picks the scratch register by the number of stack
  args (0/3 -> eax, 1/4 -> ecx, 2/5 -> edx). The same likely applies to other tail calls: arity changes registers.
- Do not touch another helper's files. Your function list is in your task; the other helpers work in parallel on
  other addresses in the same `re/match/s28/` directory. Scratch files go to `D:/MashedWT/MATCH-S28_data/scratch/<id>/`.
- When a raw g2s source of yours already exists in `re/match/s28/` it is at 100%: leave it.
- To start from the raw converter output in the required form: `py -3.12 D:/MashedWT/MATCH-S28_data/tools/to_s28.py <va8>`
  (writes `re/match/s28/<va8>.c`: FLAGS line, preamble, globals as MGLOBAL, LF endings; the raw files under
  `g2s/` have CR CR LF endings, so do not copy them by hand).


## S28 additions
- Slice 0x005bd9e0..0x005cb2a0, tail of .text: 137 game rows. 41 are already byte-identical with relocheck OK in
  `re/match/s28/` (leave them). Your rows are listed in your task (`D:/MashedWT/MATCH-S28_data/todo_<id>.tsv`:
  va8, Ghidra size, raw best %, raw run), smallest first. Every row has a Ghidra size.
- Contents: RenderWare audio (RWS/Toast glue, 0x005bd9e0..0x005bff40 and 0x005c7070..0x005c9d10), the CRT
  (0x005c1da2..0x005c6cc0: startup, `__getptd`, `_ld12tod`-style conversions, heap/stdio helpers) and a D3DX/RW tail.
- **Flags per row** (line 1 `// FLAGS:`): CRT = `/O1`; game and RW audio = `/O2 /Ob1 /G5` (no /GS) or `/O2 /GS /Ob1`; D3DX = `/O1`.
  The raw converter ran three sets (`g2s_g5` = `/O2 /Ob1 /G5`, `g2s_game` = `/O2 /GS /Ob1`, `g2s_o1` = `/O1`, under
  `D:/MashedWT/MATCH-S28_data/`); `g2s/` holds the best per row (`to_s28.py` converts it). Try the other flag sets
  with `VARS="/O1|/O1 /Oy-|/O2 /GS /Ob1|/O2 /Ob1 /G5" SRCDIR=re/match/s28 py -3.12 verify/matchs28_20261107/tools/flagsweep.py <va8>`.
  Flags brief: `git show orch/MATCH-RWCORE-G5:verify/matchrwcoreg5_20261107/flags_brief.md`.
- Many rows did not compile in the raw converter (`compile` in `g2s/results.tsv`: callee prototype with an undeclared
  type, `void` casts, `__getptd`, `FID_conflict___ld12tod`). `to_s28.py <va8>` converts the text anyway; you fix the errors.
  CRT helpers are called as ordinary C functions with the right convention (the callee's `ret N` or register args).
- Register-passed args (`in_EAX`, `unaff_ESI`, `in_ECX`): pilot finding 1 (static callee / `__fastcall`), and CRT
  `__usercall` helpers usually need a file-static helper with the matching convention.
- `__asm` is barred except one macro-style inline `__asm` inside a compiled C body (for inline `fnstcw/fldcw/fistp`
  or `rep movs` that C cannot reach). Say so in the header comment. Never a naked function, never a lone `__asm` block.
- This slice is MATCH-ONLY: do not touch `mashedmod/`.
- `match_permute.py` with `--jobs 2`, one permuter at a time, at most `--time 300` per run (three helpers share the machine).
- A row whose raw % is below 50 is a rewrite from `odis.py` + decomp.json, not a tweak. If the budget is spent, keep the best compiling version.
- Append one line to your notes file as you finish each function (same format as the final list).
- Scratch: `D:/MashedWT/MATCH-S28_data/scratch/<id>/`. Notes: `D:/MashedWT/MATCH-S28_data/notes/<id>.md`.
- Tools dir for this slice: `D:/MashedWT/MATCH-S28_data/tools/` (preamble.h, cxxcheck.bat, nosize.py, to_s28.py).
- Overlaid UNCOMMITTED tool files in the worktree (do not revert them): `re/tools/match/match.py`, `relocheck.py`,
  `re/console/pc_funcs.csv`.
