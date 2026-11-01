# MATCH-S05 hand-matching helper brief (2026-11-01)

You hand-match MASHED.exe game functions: write C that the period compiler turns into the original bytes.
Never write hooks.csv, STUBS.md, UNCERTAINTIES.md, DEFERRED.md, log/orch, original/, any Ghidra project, mashedmod/,
or anything outside the files named below. No git commands that mutate (no commit, stash, checkout, add).
No MASHED.exe / mashed_re.exe runs, no Frida, no Ghidra (do not run decomp_pc.py: everything you need is below).

Worktree (run everything from here): `C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\MATCH-S05`

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
- Starting points: `D:/MashedWT/MATCH-S05_data/g2s/<va8>.c` (raw Ghidra output made compilable, scored in
  `D:/MashedWT/MATCH-S05_data/g2s/results.tsv`) and `D:/MashedWT/MATCH-S05_data/decomp.json` (decomp + prototype +
  typed globals + callee prototypes per function; read it with a small python one-liner for your RVA, do not cat it).
- Worked examples: `re/match/pilot/*.c` (header comments say what moved the score), e.g. `f_00423e60.c`,
  `f_small_a.c`, `a5_00410510.c` (switch layout), `m_00422470.c` (register-passed args via static callee).
- Read `verify/matchpilot_20261030/RESULT_MATCH-PILOT.md` section "Findings that change how to match this game"
  before starting (5 short points: static callees for in_EAX/unaff_ESI args, file-static globals, wrong Ghidra
  callee arity, raw converter limits, acos).

## Source form (REQUIRED: these files later become the port's function bodies)
One file per function: `re/match/s05/<va8>.c` (va8 = 8 hex digits, lowercase, no 0x). Layout:
1. line 1: `// FLAGS: /O2 /GS /Ob1`
2. a header comment: best exact %, regs %, what you tried, residual class (below).
3. the preamble, copied VERBATIM from `D:/MashedWT/MATCH-S05_data/tools/preamble.h` (do not edit it).
   Examples of finished files: `re/match/s05/00426030.c` (raw converter), `re/match/s04/0041e8b0.c` and `re/match/s04/0041c2d0.c` (by hand, merged S04), `re/match/s02/0040b7b0.c`.
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
  `cmd //c "D:\\MashedWT\\MATCH-S05_data\\tools\\cxxcheck.bat re\\match\\s05\\<va8>.c"` (VS2022 /TP, MATCH_PORT;
  the `vswhere` line in its output is noise; any `error C` line is a failure). Typical fixes: explicit casts for
  void*/int->pointer conversions, real function-pointer types instead of Ghidra's `code`, `0` vs `NULL`, prototypes
  with the right arity. Casts that do not change the DDK score are free; if a C++ fix costs match %, keep the
  match and report the C++ failure in the header comment.

## Budget (bounded: this is a slice of 129 functions, not a research project)
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
Write `D:/MashedWT/MATCH-S05_data/notes/<your helper id>.md` and end with the same compact text as your final
message: one line per function `0x<va>  size  best%  regs%  cxx:ok|FAIL  class  one-line reason`, plus any static
callee a file holds (`0x<file va> holds 0x<callee va>`). Nothing else.

## S05 additions (read these too)
- Run Python with `MSYS_NO_PATHCONV=1` exported if you use Git Bash (otherwise `/O2` becomes a path), and pass
  Windows-style paths (`C:/...`, `D:/...`) to Python, never `/c/...`.
- Every 100% file must also pass `py -3.12 re/tools/match/relocheck.py re/match/s05/<va8>.c` -> `OK` with
  `unchecked=0` where possible. Relocheck resolves `MGLOBAL`/`MGLOBALA` names to their address and `FUN_<va8>` to
  its RVA, so: call every game callee as `FUN_<va8>` (rename the converter's names such as `RpClumpDestroy` to
  `FUN_004e6e00`; the callee's address is in decomp.json `callees`, `Name@0x...`), and declare every global through
  `MGLOBAL`/`MGLOBALA` (a string-typed global such as `s_training_005f33f8` -> `MGLOBALA(char, s_..., <N>, 0x...)`).
  Imports and CRT helpers stay unchecked by name (fine). A BAD line is a wrong global/callee: fix it.
- Tail-jump dispatchers (`mov r, [g]; jmp [r + k]`): this compiler picks the scratch register by the number of stack
  args (0/3 -> eax, 1/4 -> ecx, 2/5 -> edx). The same likely applies to other tail calls: arity changes registers.
- Families: 0x00423bc0/0x00423c40/0x00423d50/0x00423e60/0x00423ee0/0x00423ff0 (127 B, 50.0% raw) and
  0x00423cc0/0x00423dd0/0x00423f60/0x00424070 (140 B, 56.1%) are near-identical. Solve one, reuse it.
- Do not touch another helper's files. Your function list is in your task; the other helpers work in parallel on
  other addresses in the same `re/match/s05/` directory. Scratch files go to `D:/MashedWT/MATCH-S05_data/scratch/<id>/`.
- When a raw g2s source of yours already exists in `re/match/s05/` it is at 100%: leave it.
- To start from the raw converter output in the required form: `py -3.12 D:/MashedWT/MATCH-S05_data/tools/to_s05.py <va8>`
  (writes `re/match/s05/<va8>.c`: FLAGS line, preamble, globals as MGLOBAL, LF endings; the raw files under
  `g2s/` have CR CR LF endings, so do not copy them by hand).

## S05 additions
- Pilot head starts (already 100% under the same compiler/flags, in `re/match/pilot/`): `a5_00422b50.c` (0x00422b50),
  `a6_00423040.c` (0x00423040), `m_00422470.c` (0x00422470 + its static callee 0x004223f0, EAX/ECX),
  `f_00423e60.c` (`_v_loop2` = the 0x00423e60 shape, rename to FUN_00423e60). Bring them into the s05 form (FLAGS line,
  preamble, MGLOBAL globals, FUN_<va8> names, relocheck OK, cxxcheck ok) instead of re-solving.
- 0x004248b0 is at 100% raw but relocheck says BAD: it stores to swapped arrays (the masked match cannot tell
  0x00899560 from 0x008995b4). Fix the globals so relocheck is OK and match stays 100%.
- Files already in `re/match/s05/` (37 raw 100% sources) belong to nobody: do not edit them unless your task names them.
- Raw g2s sources: `D:/MashedWT/MATCH-S05_data/g2s/<va8>.c`; raw scores `D:/MashedWT/MATCH-S05_data/g2s/results.tsv`.
- Merged S01..S04 sources under `re/match/s0{1..4}/` are solved examples of the same game's code; grep them for a
  similar shape (x87 scale blocks, tail-jump dispatchers, register-passed callees) before inventing one.
