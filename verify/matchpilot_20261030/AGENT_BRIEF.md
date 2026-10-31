# Matching pilot: helper brief (2026-10-30)

You hand-match MASHED.exe game functions: write C that the period compiler turns into the original bytes.
This is a measurement, not a promotion. Never write hooks.csv, STUBS.md, UNCERTAINTIES.md, DEFERRED.md, log/orch,
original/, the master Ghidra project, or anything outside the files named below. No git commands that mutate
(no commit, no stash, no checkout). No MASHED.exe / mashed_re.exe runs, no Frida.

Worktree (run everything from here): `C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\MATCH-PILOT`

## Tools
- `py -3.12 re/tools/match/match.py <file.c> [--show] [--only 0x..]` compiles with cl 13.00.9176 / c2 13.00.9178
  (`tools/vc7ddk/bin` of the main checkout) and compares each `// MATCH: 0x<va> <symbol>` function with the
  original. Prints `exact% (regs = registers folded, ops = mnemonics only)` and the diff (`left = ours | right = original`).
  Relocated operands are masked, so global addresses and constant-pool values are not compared (still use the
  real values: read them with odis.py / match.original_bytes).
- `py -3.12 re/tools/match/odis.py 0x<va>` original disassembly.
- `py -3.12 re/tools/match/match_permute.py <file.c> 0x<va> --rounds 6 --walk 40 --jobs 3 --time 300` mechanical
  rewrites (compare spelling, if/else swap, statement moves, commute, signedness...). It uses the FIRST
  `// MATCH:` for that address in the file. Best variant -> `log/permute/<va>.c`. Use it on 80-99% functions.
- Starting points: `log/matchpilot/g2s/<va8>.c` (raw Ghidra output made compilable, already scored in
  `log/matchpilot/g2s/results.tsv`) and `log/matchpilot/decomp.json` (decomp + prototype + typed globals + callee
  prototypes per function). Ghidra re-reads only if you really need them:
  `cd C:\Users\maria\Desktop\Proyectos\Mashed && py -3.12 re/tools/decomp_pc.py 0x<va> --callees` (read-only pool
  slot, run it from the MAIN checkout, ~15 s). Batch addresses into one call.
- Worked examples: `re/match/pilot/f_00423e60.c` (loop + hoisted temp), `re/match/pilot/f_small_a.c` (float
  return, `sin` intrinsic, `>= 0x10` instead of `> 0xf`).

## Source rules
- Header lines: `// FLAGS: /O2 /GS` (established: /G5, /Oy-, /Os, /O1 break matched functions; /GS is on: game
  functions with local buffers load the cookie at 0x00616038). Default language C (Rich header: 373 C objects,
  18 C++). Use `// LANG c++` only if the function is a C++ method (thiscall: `this` in ECX + `ret N`).
- Plain C only: no `__asm`, no emitted bytes, no `#pragma` tricks other than `#pragma intrinsic`. Globals as
  `extern` with the real type/layout; arrays of structs where the code strides. Callees as prototypes with the
  convention the callee's own `ret N` shows (`ret N` -> __stdcall, ECX/EDX args -> __fastcall).
- Self-contained file: no #include (no SDK headers here). Declare the Win32 imports you need yourself with
  `__declspec(dllimport) ... __stdcall` (import calls are `call dword ptr [__imp_X]`).
- One output file per function: `re/match/pilot/<agent>_<va8>.c`, the best version you reached, with a header
  comment: best exact %, what you tried, and the residual classification.
- Ghidra's `_DAT_x` globals are often mistyped in the converter (int where the code does fld): fix the type.

## Budget
About 15 min per function under 400 B, 30-40 min per larger one. Stop at the budget and record the best.
Large functions: get the structure right first (types, loops, switch), then the permuter.

## Classify every remaining difference (this is the point of the pilot)
- (a) compiler-version codegen: the difference survives every plausible rewrite you tried AND looks like an
  optimizer/peephole/scheduling choice independent of source (e.g. same operations, different instruction
  selection or order that no statement order / type / spelling change moves). Say what you tried. If you see
  anything that looks specific to 13.00.9176/9178 vs the retail 13.00.9466 (the game's compiler), describe it
  precisely: these are the evidence the verdict needs.
- (b) source shape: a source form not found yet (unknown struct layout, Ghidra-merged/split temporaries, an
  inlined callee, macro, different loop/switch form, register-allocation that moved when you changed source).
- (c) tool limit: the converter/matcher cannot express it (wrong function boundary, register-passed params
  `in_EAX` from a non-standard convention, C++ EH, thiscall in C...). Report `in_*`/`unaff_*` register
  inputs explicitly: a non-standard register convention could mean link-time codegen (/GL), which matters.
Be strict: (a) only with evidence that source changes do not move it; "I ran out of time" is (b) unknown.

## Report back (final message, compact)
One line per function: `0x<va>  size  best%  regs%  class(es)  one-line reason`, then at most 5 lines of
observations relevant to "is 13.00.9176/9178 enough or is 13.00.9466 needed". Also write the same as
`verify/matchpilot_20261030/notes/<agent>.md`.
