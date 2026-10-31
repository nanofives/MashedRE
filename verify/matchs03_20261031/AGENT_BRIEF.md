# MATCH-S03 helper brief (2026-10-31)

You hand-match MASHED.exe game functions: write C that the period compiler turns into the original bytes.
Derived from `verify/matchpilot_20261030/AGENT_BRIEF.md` (read its "Classify" section, it applies unchanged).
Never write hooks.csv, STUBS.md, UNCERTAINTIES.md, DEFERRED.md, log/orch, original/, mashedmod/, the master Ghidra
project, or anything outside the files named below. No git commands that mutate. No game runs, no Frida.

Worktree (run everything from here): `C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\MATCH-S03`
In Git Bash ALWAYS `export MSYS_NO_PATHCONV=1 MSYS2_ARG_CONV_EXCL='*' PYTHONUTF8=1` first, or `/O2` turns into a path.

## Tools
- `py -3.12 re/tools/match/match.py <file.c> [--show] [--only 0x..] [--bytes]` compiles with cl 13.00.9176 /
  c2 13.00.9178 and compares each `// MATCH: 0x<va> <symbol>` function with the original. Use `--bytes` for the
  final check (whole obj symbol byte for byte; needed when Ghidra's size is short).
- `py -3.12 re/tools/match/odis.py 0x<va>` original disassembly.
- `py -3.12 re/tools/match/match_permute.py <file.c> 0x<va> --rounds 6 --walk 40 --jobs 2 --time 300` on 80-99%.
  Best variant -> `log/permute/<va>.c` (inside this worktree).
- Starting points: `D:/MashedWT/MATCH-S03_data/g2s/<va8>.c` (raw converter output, flags already `/O2 /GS /Ob1`,
  scores in `D:/MashedWT/MATCH-S03_data/g2s/results.tsv`) and `D:/MashedWT/MATCH-S03_data/decomp.json` (decomp,
  prototype, typed globals, callee prototypes). Do not re-run Ghidra.
- Worked examples: `re/match/pilot/*.c` (header comments say what worked). Pilot findings that matter most:
  register-passed args (`in_EAX`, `unaff_ESI`, `extraout_*`) = a `static` callee defined BEFORE its caller in the
  same file, address not taken, `/Ob1`; parameter order picks the registers. A stand-in caller in the file is
  allowed (say so in the header). Thunks (`jmp X`, 5 B) are `void f(void){ X(); }` style tail calls.
  Ghidra's callee arity is often wrong: check the callee's `ret N` / the caller's `add esp`.

## Source rules (MATCH-S03 adds the port-compatibility rule)
- Header lines: `// FLAGS: /O2 /GS /Ob1` then one `// MATCH: 0x<va> <symbol>` per matched function.
- Plain C only: no `__asm`, no emitted bytes, no `#pragma` other than `#pragma intrinsic`. Self-contained, no #include.
- **Port-compatible layout** (the matched body will later be `#include`d into a C++ TU of mashed_re.exe):
  ```
  // FLAGS: /O2 /GS /Ob1
  // MATCH: 0x0041ae20 FUN_0041ae20
  /* header comment: best %, tried, class */
  #ifndef MATCH_PORT
  typedefs; extern globals; callee prototypes      (declarations only)
  #endif
  function definitions                               (the body: this text is what ships)
  ```
  In the body: no implicit int, no K&R, explicit casts for every int<->pointer and void* conversion, no C++
  keywords as identifiers (`this`, `new`, `class`, `bool` as a typedef is fine only inside the #ifndef block).
  Globals referenced by name (`DAT_005f3304` or an array/struct name you declare), never by a literal address cast,
  unless the original's code shape needs it. If port-compatibility costs bytes, keep the bytes and note it.
- One file per function (or per static group: a static callee + its caller), `re/match/s03/<agent>_<va8>.c`.

## Budget
About 15 min per function under 200 B, 30 min under 500 B. Stop at the budget and record the best.

## Report back (final message, compact)
One line per function: `0x<va>  size  best%  bytes-ok(y/n)  class  file  one-line reason`. Also write the same to
`verify/matchs03_20261031/notes/<agent>.md`.
