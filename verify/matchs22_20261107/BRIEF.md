# MATCH-S22 hand-matching helper brief (2026-11-07, from the S19 brief)

You hand-match MASHED.exe game functions: write C that the period compiler turns into the original bytes.
Never write hooks.csv, STUBS.md, UNCERTAINTIES.md, DEFERRED.md, log/orch, original/, any Ghidra project, mashedmod/,
or anything outside the files named below. No git commands that mutate (no commit, stash, checkout, add).
No MASHED.exe / mashed_re.exe runs, no Frida, no Ghidra (do not run decomp_pc.py: everything you need is below).

Worktree (run everything from here): `C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\MATCH-S22`

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
- Starting points: `D:/MashedWT/MATCH-S22_data/g2s/<va8>.c` (raw Ghidra output made compilable, scored in
  `D:/MashedWT/MATCH-S22_data/g2s/results.tsv`) and `D:/MashedWT/MATCH-S22_data/decomp.json` (decomp + prototype +
  typed globals + callee prototypes per function; read it with a small python one-liner for your RVA, do not cat it).
- Worked examples: `re/match/pilot/*.c` (header comments say what moved the score), e.g. `f_00423e60.c`,
  `f_small_a.c`, `a5_00410510.c` (switch layout), `m_00422470.c` (register-passed args via static callee).
- Read `verify/matchpilot_20261030/RESULT_MATCH-PILOT.md` section "Findings that change how to match this game"
  before starting (5 short points: static callees for in_EAX/unaff_ESI args, file-static globals, wrong Ghidra
  callee arity, raw converter limits, acos).

## Source form (REQUIRED: these files later become the port's function bodies)
One file per function: `re/match/s22/<va8>.c` (va8 = 8 hex digits, lowercase, no 0x). Layout:
1. line 1: `// FLAGS: /O2 /Ob1 /G5` (RenderWare flags, see S22 additions; record what matched)
2. a header comment: best exact %, regs %, what you tried, residual class (below).
3. the preamble, copied VERBATIM from `D:/MashedWT/MATCH-S22_data/tools/preamble.h` (do not edit it).
   Examples of finished files: `re/match/s22/004f4900.c` (raw converter, this slice), `re/match/s19/004c2d90.c`, `re/match/s04/0041e8b0.c` (by hand), `re/match/s02/0040b7b0.c`; more hand-matched files in `re/match/s0[1-9]/`, `re/match/s1[0-6]/`, `re/match/s19/` (all merged). Earlier helper notes: `D:/MashedWT/MATCH-S1[0-9]_data/notes/h*.md`.
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
  `cmd /c "D:\MashedWT\MATCH-S22_data\tools\cxxcheck.bat re\match\s18\<va8>.c"` from the **PowerShell** tool (from Git Bash `cmd //c` hangs on this machine) (VS2022 /TP, MATCH_PORT;
  the `vswhere` line in its output is noise; any `error C` line is a failure). Typical fixes: explicit casts for
  void*/int->pointer conversions, real function-pointer types instead of Ghidra's `code`, `0` vs `NULL`, prototypes
  with the right arity. Casts that do not change the DDK score are free; if a C++ fix costs match %, keep the
  match and report the C++ failure in the header comment.

## Budget (bounded: this is a slice of 58 functions, not a research project)
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
Write `D:/MashedWT/MATCH-S22_data/notes/<your helper id>.md` and end with the same compact text as your final
message: one line per function `0x<va>  size  best%  regs%  cxx:ok|FAIL  class  one-line reason`, plus any static
callee a file holds (`0x<file va> holds 0x<callee va>`). Nothing else.

## Slice additions (read these too)
- Run Python with `MSYS_NO_PATHCONV=1` exported if you use Git Bash (otherwise `/O2` becomes a path), and pass
  Windows-style paths (`C:/...`, `D:/...`) to Python, never `/c/...`.
- Every 100% file must also pass `py -3.12 re/tools/match/relocheck.py re/match/s22/<va8>.c` -> `OK` with
  `unchecked=0` where possible. Relocheck resolves `MGLOBAL`/`MGLOBALA` names to their address and `FUN_<va8>` to
  its RVA, so: call every game callee as `FUN_<va8>` (rename the converter's names such as `RpClumpDestroy` to
  `FUN_004e6e00`; the callee's address is in decomp.json `callees`, `Name@0x...`), and declare every global through
  `MGLOBAL`/`MGLOBALA` (a string-typed global such as `s_training_005f33f8` -> `MGLOBALA(char, s_..., <N>, 0x...)`).
  Imports and CRT helpers stay unchecked by name (fine). A BAD line is a wrong global/callee: fix it.
- Tail-jump dispatchers (`mov r, [g]; jmp [r + k]`): this compiler picks the scratch register by the number of stack
  args (0/3 -> eax, 1/4 -> ecx, 2/5 -> edx). The same likely applies to other tail calls: arity changes registers.
- Do not touch another helper's files. Your function list is in your task; the other helpers work in parallel on
  other addresses in the same `re/match/s22/` directory. Scratch files go to `D:/MashedWT/MATCH-S22_data/scratch/<id>/`.
- When a raw g2s source of yours already exists in `re/match/s22/` it is at 100%: leave it.
- To start from the raw converter output in the required form: `py -3.12 D:/MashedWT/MATCH-S22_data/tools/to_s22.py <va8>`
  (writes `re/match/s22/<va8>.c`: FLAGS line, preamble, globals as MGLOBAL, LF endings; the raw files under
  `g2s/` have CR CR LF endings, so do not copy them by hand).


## S22 additions
- Slice 0x004f022d..0x004fb5a0 (58 functions, 22 library rows inside skipped). 14 are already at 100% with relocheck
  OK in `re/match/s22/` (raw converter): leave them. 44 remain, split over helpers h1..h7 (`helper_assign.txt` in
  `verify/matchs22_20261107/`). The range sits between the RenderWare core and D3DX: hooks.csv tags the neighbours
  `third-party-library[renderware]` (0x004f0910..0x004f46a0) and `[zlib-1.1.3]` (0x004f1f90, 0x004fa6f0 zcalloc).
  So these are RenderWare plugin/toolkit functions (RW SDK code), with two exceptions noted below.
- **Flags: `/O2 /Ob1 /G5` (no /GS) first.** The raw converter scored every row under both flag sets: `/G5` won or
  tied on 51 of 58, the game flags won on 7 by at most 8 points. All 14 raw matches are 100% under `/O2 /Ob1 /G5`.
  Evidence for RW code being `/G5` without `/GS`: `git show orch/MATCH-RWCORE-G5:verify/matchrwcoreg5_20261107/flags_brief.md`
  (read it: it also lists x87 residuals that no flag/permuter moved in RW code, do not spend budget on those).
  If a function stalls, run `py -3.12 D:/MashedWT/MATCH-S22_data/tools/flagsweep.py <va8>` once (scratch copy, 5 flag
  sets) and record whatever matched on line 1.
- **0x004f022d and 0x004f0307 (h1)** sit at odd addresses, use `ebp` frames and call 0x004eede9 (a
  `third-party-library[d3dx9-psgp]` row): they are D3DX code built by another toolchain. Spend at most 10 min on
  both: one flag sweep (add `/Oy-` to a scratch copy), then record class (a) or (c) with the evidence.
- **RwGlobals access.** `DAT_007d3ff8` is the RwGlobals pointer; plugin offset globals (e.g. `DAT_00911ae0`) are added
  to it. Write the RwGlobals pointer FIRST: `(DAT_007d3ff8 + DAT_00911ae0 + 0x58)` (relocheck BAD otherwise; that
  fixed 0x004f43b0 here).
- A function address used as a value (stored into a vtable / callback slot) must be a declared function, not an
  `MGLOBAL`: the raw converter declared `FUN_004ebc30` as `MGLOBAL(undefined1, ...)` in 0x004f13d0 and 0x004f1b00,
  which is why those two do not compile. Declare it with its real prototype and convention (odis.py on the target).
- Vtable / function-pointer calls: keep the call through a typed function pointer (C++ check needs a real type).
- 5-byte `jmp <other>` functions: `<ret> FUN_x(<args>) { return FUN_<target>(<args>); }`.
- Register-arg functions (`in_EAX`/`unaff_ESI`, `in_* register value` in g2s.log; here 0x004f1150, 0x004f1870,
  0x004f79f0): pilot finding 1 (static callee, or a stand-in caller `MatchStandIn_<va8>` WITHOUT a `// MATCH:` line).
- Tail-jump dispatchers: the scratch register follows the stack-arg count (0/3 -> eax, 1/4 -> ecx, 2/5 -> edx).
- Append one line to your notes file **as you finish each function** (same format as the final list).
- Other MATCH slices may run on other branches at the same time; their data dirs are `D:/MashedWT/MATCH-S*_data`.
  Do not touch them. Do not touch another helper's files.
- Keep `--jobs 2` on the permuter and run at most one permuter at a time per helper.
- Run the C++ check once on each 100% file and report `cxx:ok|FAIL`.
- Every 100% file must pass relocheck OK. Do not leave a 100% file relocheck BAD.
