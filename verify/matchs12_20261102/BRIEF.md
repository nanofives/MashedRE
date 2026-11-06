# MATCH-S12 hand-matching helper brief (2026-11-02, from the S11 brief)

You hand-match MASHED.exe game functions: write C that the period compiler turns into the original bytes.
Never write hooks.csv, STUBS.md, UNCERTAINTIES.md, DEFERRED.md, log/orch, original/, any Ghidra project, mashedmod/,
or anything outside the files named below. No git commands that mutate (no commit, stash, checkout, add).
No MASHED.exe / mashed_re.exe runs, no Frida, no Ghidra (do not run decomp_pc.py: everything you need is below).

Worktree (run everything from here): `C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\MATCH-S12`

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
- Starting points: `D:/MashedWT/MATCH-S12_data/g2s/<va8>.c` (raw Ghidra output made compilable, scored in
  `D:/MashedWT/MATCH-S12_data/g2s/results.tsv`) and `D:/MashedWT/MATCH-S12_data/decomp.json` (decomp + prototype +
  typed globals + callee prototypes per function; read it with a small python one-liner for your RVA, do not cat it).
- Worked examples: `re/match/pilot/*.c` (header comments say what moved the score), e.g. `f_00423e60.c`,
  `f_small_a.c`, `a5_00410510.c` (switch layout), `m_00422470.c` (register-passed args via static callee).
- Read `verify/matchpilot_20261030/RESULT_MATCH-PILOT.md` section "Findings that change how to match this game"
  before starting (5 short points: static callees for in_EAX/unaff_ESI args, file-static globals, wrong Ghidra
  callee arity, raw converter limits, acos).

## Source form (REQUIRED: these files later become the port's function bodies)
One file per function: `re/match/s12/<va8>.c` (va8 = 8 hex digits, lowercase, no 0x). Layout:
1. line 1: `// FLAGS: /O2 /GS /Ob1`
2. a header comment: best exact %, regs %, what you tried, residual class (below).
3. the preamble, copied VERBATIM from `D:/MashedWT/MATCH-S12_data/tools/preamble.h` (do not edit it).
   Examples of finished files: `re/match/s12/0047aa50.c` (raw converter, this slice), `re/match/s04/0041e8b0.c` (by hand), `re/match/s02/0040b7b0.c`; more hand-matched files in `re/match/s0[1-8]/` (merged slices). S09, S10, S11, S13, S15, S16 are not merged: read their files with `git show orch/MATCH-S16:re/match/s16/<va8>.c` (same for the others), their notes in `D:/MashedWT/MATCH-S16_data/notes/h*.md` (also S09..S15). S11 (0x004670a0..0x00474e60, just below this slice) and S13 (0x0047aad0.., just above) are the nearest code: their notes (`D:/MashedWT/MATCH-S11_data/notes/h*.md`, `D:/MashedWT/MATCH-S13_data/notes/`) carry reusable findings.
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
  `cmd /c "D:\MashedWT\MATCH-S12_data\tools\cxxcheck.bat re\match\s11\<va8>.c"` from the **PowerShell** tool (from Git Bash `cmd //c` hangs on this machine) (VS2022 /TP, MATCH_PORT;
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
Write `D:/MashedWT/MATCH-S12_data/notes/<your helper id>.md` and end with the same compact text as your final
message: one line per function `0x<va>  size  best%  regs%  cxx:ok|FAIL  class  one-line reason`, plus any static
callee a file holds (`0x<file va> holds 0x<callee va>`). Nothing else.

## Slice additions (read these too)
- Run Python with `MSYS_NO_PATHCONV=1` exported if you use Git Bash (otherwise `/O2` becomes a path), and pass
  Windows-style paths (`C:/...`, `D:/...`) to Python, never `/c/...`.
- Every 100% file must also pass `py -3.12 re/tools/match/relocheck.py re/match/s12/<va8>.c` -> `OK` with
  `unchecked=0` where possible. Relocheck resolves `MGLOBAL`/`MGLOBALA` names to their address and `FUN_<va8>` to
  its RVA, so: call every game callee as `FUN_<va8>` (rename the converter's names such as `RpClumpDestroy` to
  `FUN_004e6e00`; the callee's address is in decomp.json `callees`, `Name@0x...`), and declare every global through
  `MGLOBAL`/`MGLOBALA` (a string-typed global such as `s_training_005f33f8` -> `MGLOBALA(char, s_..., <N>, 0x...)`).
  Imports and CRT helpers stay unchecked by name (fine). A BAD line is a wrong global/callee: fix it.
- Tail-jump dispatchers (`mov r, [g]; jmp [r + k]`): this compiler picks the scratch register by the number of stack
  args (0/3 -> eax, 1/4 -> ecx, 2/5 -> edx). The same likely applies to other tail calls: arity changes registers.
- Do not touch another helper's files. Your function list is in your task; the other helpers work in parallel on
  other addresses in the same `re/match/s12/` directory. Scratch files go to `D:/MashedWT/MATCH-S12_data/scratch/<id>/`.
- When a raw g2s source of yours already exists in `re/match/s12/` it is at 100%: leave it.
- To start from the raw converter output in the required form: `py -3.12 D:/MashedWT/MATCH-S12_data/tools/to_s12.py <va8>`
  (writes `re/match/s12/<va8>.c`: FLAGS line, preamble, globals as MGLOBAL, LF endings; the raw files under
  `g2s/` have CR CR LF endings, so do not copy them by hand).

## S12 additions
- Slice 0x00474e70..0x0047aaa0, 129 game functions: mostly render (60 left), track (23), util, vehicle, ai. Expect
  RenderWare calls (RpClump/RwFrame/RpAtomic callees as `FUN_<va8>`), x87 float math, small getters/setters over
  global tables, and a family of near-identical track functions at 0x0047a1e0..0x0047a8b0 (155 B each, raw converter
  at 96.9%: solve one, then reuse the same fix on the others).
- 31 files already in `re/match/s12/` are 100% + relocheck OK (raw converter): leave them. Several are callees of yours.
- **5 files in `re/match/s12/` are NOT 100%** (raw converter 100% before the globals moved to MGLOBAL; the extern
  declaration order changed codegen/global binding): 00475a00, 00475a60, 00475ea0, 004769a0, 004769d0. They are in
  helper lists like any other function: fix them (declaration order of the MGLOBAL lines, typed globals; relocheck
  BAD lines in two of them show the globals swapped). Compare with the raw `D:/MashedWT/MATCH-S12_data/g2s/<va8>.c`.
- Gap rows (`verify/match_topdown_20261101/gap_value.tsv`, their match unlocks callers): 0x004783f0 (ai, h4),
  0x00476390 (render, h2), 0x00476430 (5 B `jmp` thunk to 0x00475ef0, h2). Do them FIRST in your list.
- 5-byte functions that are only `jmp <other>`: the original is a tail call. Try `<ret> FUN_x(<args>) { return
  FUN_<target>(<args>); }` with the target declared extern (arity from the target's own `ret N`); if no form gives
  `jmp rel32`, class (c).
- Register-arg functions (`mov eax, ecx` at entry, `in_EAX`/`unaff_ESI`): pilot finding 1, as in S11's notes.
- Your function list comes in your task in the order to work it: gap rows first, then functions up to 400 B (address
  order), then the large ones smallest first. Yield matters more than depth. Functions above 1000 B: budget 60 min.
- Append one line to your notes file **as you finish each function** (same format as the final list).
- S13 and other slices run on other branches at the same time; their data dirs (`D:/MashedWT/MATCH-S13_data`, ...)
  are read-only for you.
- Keep `--jobs 2` on the permuter and run at most one permuter at a time.
- Run the C++ check once on each 100% file and report `cxx:ok|FAIL`.
