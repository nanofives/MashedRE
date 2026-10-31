# MATCH-S01 hand-matching brief (2026-10-31)

You hand-match MASHED.exe game functions: write C that the period compiler turns into the original bytes.
Static work only. Never write hooks.csv, STUBS.md, UNCERTAINTIES.md, DEFERRED.md, log/orch, original/, any Ghidra
project, mashedmod/, or anything outside the paths named below. No git commands at all (the parent commits). No
MASHED.exe / mashed_re.exe runs, no Frida, no builds of mashedmod.

Worktree (run everything from here): `C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\MATCH-S01`
In Git Bash, ALWAYS `export MSYS_NO_PATHCONV=1` first, or `/O2` turns into `C:/Program Files/Git/O2` and every score is junk.

## Tools
- Score one or more files, one line per function (va, exact %, BYTES-OK or bad<n>/<size>):
  `py -3.12 D:/MashedWT/MATCH-S01_data/score.py <file.c> ...`
- Diff: `py -3.12 re/tools/match/match.py <file.c> --show [--only 0x..] --bytes` (left = ours | right = original).
  Relocated operands are masked (global addresses, constant-pool slots), so read real values with
  `py -3.12 re/tools/match/odis.py 0x<va>` (original disassembly).
- Permuter for 80-99% functions: `py -3.12 re/tools/match/match_permute.py <file.c> 0x<va> --rounds 6 --walk 40 --jobs 3 --time 300`
  (best variant -> `log/permute/<va>.c`; uses the FIRST `// MATCH:` for that address in the file).
- Starting points: `D:/MashedWT/MATCH-S01_data/g2s/<va8>.c` (raw Ghidra output made compilable, scored in
  `D:/MashedWT/MATCH-S01_data/g2s/results.tsv`) and `D:/MashedWT/MATCH-S01_data/decomp.json` (decomp, prototype,
  typed globals, callee prototypes per function; it is 700 KB, so extract one entry with a short python one-liner,
  do not Read it whole).
- Worked examples: `re/match/pilot/*.c` (header comment of each = what worked). Pilot RESULT with the findings:
  `verify/matchpilot_20261030/RESULT_MATCH-PILOT.md`, section "Findings that change how to match this game".

## Source rules
- Header line `// FLAGS: /O2 /GS /Ob1` in every file. Default language C. Never /G5, /Oy-, /Os, /O1, /Op.
- `// MATCH: 0x<va> FUN_<va8>` on the line before each matched definition. Keep the symbol `FUN_<va8>` (plain C name).
- Plain C only: no `__asm`, no emitted bytes, no `#pragma` other than `#pragma intrinsic`. Self-contained file, no
  #include. Win32 imports declared yourself as `__declspec(dllimport) ... __stdcall`.
- Globals as `extern` with the real type: Ghidra's `_DAT_x` / `DAT_x` are often mistyped (int where the code does
  `fld`, a float stored as immediate `0` instead of reusing a zeroed register means a different type). Arrays of
  structs where the code strides (example in this slice: base 0x00639d80, stride 0xec). Keep the original names
  `DAT_<addr8>` for globals (the port maps those names to addresses later), one name per address.
- Callees as prototypes with the convention the callee's own `ret N` shows (`ret N` -> __stdcall, ECX/EDX args ->
  __fastcall). Ghidra's arity is often wrong (check the caller's pushes and `add esp`).
- Register-passed args (`in_EAX`, `unaff_ESI`, `extraout_ECX`): the callee is a `static` function defined earlier
  in the SAME file, address not taken, kept a call by /Ob1. Parameter order picks the registers. So you may put
  several consecutive functions of your range in one file. If the static callee is itself in your range, it must
  be its own real matched body (with its own `// MATCH:`). If a stand-in for an out-of-range callee was needed,
  say so in the header comment (it is a caveat).
- One file per function, or per group of consecutive functions that need each other: `re/match/s01/<va8>.c`
  named after the first function in it. Header comment: best exact %, what you tried, residual class.

## Budget and order
Work your list smallest first. About 15 min per function under 400 B, 30 min for larger ones; stop at the budget
and keep the best. Total budget about 3 hours. Large functions: types, loops and switch first, then the permuter.
A compile-failing g2s output usually needs the prototype or a struct fixed first.

## Classify every remaining difference
(a) compiler-version codegen candidate (survives every plausible rewrite; say what you tried), (b) source shape
not found yet, (c) tool limit (wrong Ghidra boundary, C++ EH, thiscall...). "Ran out of time" is (b).

## Report back (final message, compact)
One line per function: `0x<va>  size  best%  BYTES-OK|bad  class  file  one-line reason`. Then at most 5 lines of
observations (struct layouts found, shared statics, things the other agents should know). Also write the same as
`D:/MashedWT/MATCH-S01_data/notes/<agent>.md`.
