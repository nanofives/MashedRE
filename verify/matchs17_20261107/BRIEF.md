# MATCH-S17 hand-matching helper brief (2026-11-07, from the S19 brief)

You hand-match MASHED.exe functions: write C that the period compiler turns into the original bytes.
Never write hooks.csv, STUBS.md, UNCERTAINTIES.md, DEFERRED.md, log/orch, original/, any Ghidra project, mashedmod/,
or anything outside the files named below. No git commands that mutate (no commit, stash, checkout, add).
No MASHED.exe / mashed_re.exe runs, no Frida, no Ghidra (do not run decomp_pc.py: everything you need is below).

Worktree (run everything from here): `C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\MATCH-S17`

## What this slice is: the statically linked MSVC CRT

Every function of this slice is a Microsoft C runtime function (libcmt) that hooks.csv files under boot/util/render.
`D:/MashedWT/MATCH-S17_data/libmatch_slice.tsv` (columns `status`, `lib_function`, `first_diff`, `note`) names the
closest DDK `libcmt.lib` function for each address and the first byte difference. Use the CRT name to recall the CRT
source (VC7 / VS.NET 2002 `crt\src`: `crt0dat.c` `_cinit` / `doexit`, `mlock.c` `_lock` / `_mtinitlocknum`,
`secfail.c` `report_failure`, `seccinit.c` / `gs_support.c` `__security_init_cookie`, `testfdiv.c`, `fpinit.c`,
`calloc.c`, `stdenvp.c` `_setenvp`, `a_env.c`, `strtok.c`, `ftime64.c`, `crt0msg.c` `_NMSG_WRITE`, `mbctype.c`,
`setlocal.c`, `output.c` `_output`, `crt0.c` `WinMainCRTStartup`, `handler.cpp`/`unhandld.cpp` C++ filters), then
write C to match.

**Flags: the CRT C objects match under `/O1`**, not the game flags (raw-converter sweep: 0x004a5f07 and 0x004a9744 go
to 100% with `/O1` and stay at 67-75% with `/O2 /GS /Ob1`; every sweep row's best is `/O1` or a no-gain tie). Start
each file with `// FLAGS: /O1`. Try `/O1 /GS` where the original has a cookie (`mov eax,[0x616038]` + `xor` at entry
and `call 0x4a2be9` near the exit). `__try` / `__except` / `__finally` in the source is what gives the
`push N; push scopetable; call 0x4a5984` (__SEH_prolog) entries: write them as real `__try` blocks.
The exe's CRT objects carry comp.id 28/9178 (C) and 29/9178 (C++) in the Rich header, the same compiler as
`tools/vc7ddk`. A row that `libmatch` marks `differs` is therefore a source or flag difference (exe CRT = VS.NET 2002
retail libcmt, DDK = the DDK's own libcmt build), not a compiler difference.

## Tools
- `py -3.12 re/tools/match/match.py <file.c> [--show] [--only 0x..]` compiles with cl 13.00.9176 / c2 13.00.9178 and
  compares each `// MATCH: 0x<va> <symbol>` function with the original. Prints `exact% (regs, ops)` and with --show
  the diff (`left = ours | right = original`). Relocated operands are masked (still use the real globals/constants:
  read them with odis.py).
  A function whose Ghidra size stops before its real end counts as matched when the whole obj symbol is
  byte-identical: match.py prints a bytes line for that; say so in the header comment.
- `py -3.12 re/tools/match/odis.py 0x<va>` original disassembly (if it dies with OSError 22 on a pipe, redirect
  to a file under your scratch dir and read the file).
- `py -3.12 re/tools/match/match_permute.py <file.c> 0x<va> --rounds 6 --walk 40 --jobs 2 --time 300` mechanical
  rewrites (FIRST `// MATCH:` for that address). Best variant -> `log/permute/<va8>.c`. Use it on 80-99% functions.
  Keep --jobs 2, at most one permuter at a time per helper.
- Starting points: `D:/MashedWT/MATCH-S17_data/g2s/<va8>.c` (raw Ghidra output made compilable, scored in
  `D:/MashedWT/MATCH-S17_data/g2s/results.tsv`; many did not compile) and `D:/MashedWT/MATCH-S17_data/decomp.json`
  (decomp + prototype + typed globals + callee prototypes per function; read it with a small python one-liner for
  your RVA, do not cat it).
- `py -3.12 D:/MashedWT/MATCH-S17_data/tools/to_s17.py --flags "/O1" <va8>` writes `re/match/s17/<va8>.c` from the
  raw g2s output in the required form (only for g2s rows whose stage is `ok`). Then
  `py -3.12 D:/MashedWT/MATCH-S17_data/tools/funrename.py <va8>` renames the function and its callees to
  `FUN_<va8>` (relocheck needs that). Finished examples: `re/match/s17/004a5f07.c`, `re/match/s17/004a31e1.c`.
- Worked examples of hand-matched forms: `re/match/pilot/*.c`, `re/match/s0[1-9]/`, `re/match/s1[0-6]/`,
  `re/match/s19/`. Read `verify/matchpilot_20261030/RESULT_MATCH-PILOT.md` section "Findings that change how to match
  this game" (static callees for in_EAX/unaff_ESI args, file-static globals, wrong Ghidra callee arity).

## Source form (REQUIRED)
One file per function: `re/match/s17/<va8>.c` (va8 = 8 hex digits, lowercase, no 0x). Layout:
1. line 1: `// FLAGS: /O1` (or the flags that matched).
2. a header comment: best exact %, regs %, what you tried, residual class (below), the CRT function name.
3. the preamble, copied VERBATIM from `D:/MashedWT/MATCH-S17_data/tools/preamble.h` (do not edit it).
4. globals ONLY through the preamble macros, one per line, with the real address:
   `MGLOBAL(<type>, DAT_xxxxxxxx, 0xxxxxxxxx);` or for arrays `MGLOBALA(<type>, DAT_xxxxxxxx, <N>, 0xxxxxxxxx);`.
   Never a plain `extern` for a global. A file-`static` global: `static <type> DAT_x;` under `#ifndef __cplusplus`
   and `MGLOBAL(...)` under `#else`, and say so in the header comment.
5. callee prototypes with the exact arity and the convention the callee's own `ret N` shows. Imports:
   `__declspec(dllimport) <ret> __stdcall Name(...)`.
6. the function(s), each preceded by `// MATCH: 0x<va> FUN_<va8>`. Name the function `FUN_<va8>`. A static
   callee you add also gets its own `// MATCH:` line when it is the real callee, named `FUN_<its va8>`.
- No `__asm`, no emitted bytes, no `#pragma` other than `#pragma intrinsic`. No `#include`.
- The file must also compile as C++: `cmd /c "D:\MashedWT\MATCH-S17_data\tools\cxxcheck.bat re\match\s17\<va8>.c"`
  from the **PowerShell** tool (Git Bash `cmd //c` hangs). The `vswhere` line is noise; any `error C` line is a
  failure. If a C++ fix costs match %, keep the match and report the C++ failure. (`__try` compiles under /EHa.)
- Every 100% file must also pass `py -3.12 re/tools/match/relocheck.py re/match/s17/<va8>.c` -> `OK`, `unchecked=0`
  where possible: call every callee as `FUN_<va8>` (addresses in decomp.json `callees`, `Name@0x...`), globals
  through MGLOBAL/MGLOBALA. Imports stay unchecked by name (fine). A BAD line is a wrong global/callee: fix it.

## Not C (do not attempt; already classed (c) by the lead)
MASM sources in the CRT: 0x004a34b0 strncpy, 0x004a9410 strlen, 0x004aaf90 memset, 0x004aaff0 memcpy, 0x004acd00
memmove, 0x004ac560 strcpy, 0x004ac570 strcat, 0x004a5984 __SEH_prolog, 0x004a3384 acos, 0x004a3700 / 0x004a37b0
__CIsin / __CIcos. If a function on your list turns out to be a fragment of another function (an SEH `__finally`
funclet reached only through a scope table, or a mid-function entry), class it (c), say whose, and move on.

## Budget (bounded)
About 6 min per function under 100 B, 15 min for 100-400 B, 25 min above 400 B, 30 min for 0x004a504f. Stop at the
budget, keep the best version, record it. Do the functions in the order given. Yield matters more than depth.

## Classify every remaining difference
- (a) compiler-version codegen: survives every plausible rewrite AND is an optimizer choice no source change moves.
- (b) source shape not found yet ("ran out of budget" is (b)).
- (c) tool limit: wrong function boundary, funclet/fragment, MASM source, non-standard register convention.

## Report back
Append one line to `D:/MashedWT/MATCH-S17_data/notes/<your helper id>.md` as you finish each function, and end with
the same compact text as your final message: one line per function
`0x<va>  size  best%  regs%  cxx:ok|FAIL  class  one-line reason`, plus any static callee a file holds
(`0x<file va> holds 0x<callee va>`). Nothing else.
- Run Python with `MSYS_NO_PATHCONV=1` (and `PYTHONUTF8=1`) exported if you use Git Bash, and pass Windows-style
  paths (`C:/...`, `D:/...`) to Python, never `/c/...`.
- Do not touch another helper's files. Scratch files go to `D:/MashedWT/MATCH-S17_data/scratch/<id>/`.
- The 13 files already in `re/match/s17/` are at 100% with relocheck OK: leave them.
