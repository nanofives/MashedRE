# MATCH-S23 hand-matching helper brief (2026-11-07, from the S20 brief)

You hand-match MASHED.exe functions: write C that the period compiler turns into the original bytes.
Never write hooks.csv, STUBS.md, UNCERTAINTIES.md, DEFERRED.md, log/orch, original/, any Ghidra project, mashedmod/,
or anything outside the files named below. No git commands that mutate (no commit, stash, checkout, add).
No MASHED.exe / mashed_re.exe runs, no Frida, no Ghidra (do not run decomp_pc.py: everything you need is below).

Worktree (run everything from here): `C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\MATCH-S23`

## What this slice is: libpng 1.0.12 + zlib 1.1.3 (read this first)
The slice 0x00516bf0..0x00534920 is almost entirely **statically linked libpng 1.0.12 and zlib 1.1.3** (the exe
carries the strings `1.0.12`, `deflate 1.1.3`, `inflate 1.1.3`; hooks.csv notes say `library_residue libpng/zlib`),
plus 8 functions of a BMP/image loader (0x0052cb00..0x0052df40) and 2 RenderWare-style functions (0x00517250,
0x00534920). MATCH-LIB found that the official sources compiled as-is give identical bytes for only ~15% of the
functions; the rest differ. So this is not a library-identity job. You still hand-match, but you have a big head start:
- `verify/matchs23_20261107/libpair.tsv`: per RVA, the official library function it pairs with (`lib_function`,
  `lib_object`; libmatch mnemonic-similarity pairing, so a HINT: confirm it against the decomp; empty = not paired,
  find it in the source by its strings/callees).
- Official sources: `C:\Users\maria\Desktop\Proyectos\Mashed\tools\_download\pngzlib\src\libpng-1.0.12\*.c` and
  `...\zlib-1.1.3\*.c`. Use them for the control flow, statement order, loop shapes, local variables, macro
  expansions (png_ptr->..., PNG_ROWBYTES, zmemcpy) and the order of `if`s. Use the Ghidra decomp and `odis.py` for the
  **struct offsets**: the game's `png_struct` / `png_info` layout is NOT the official one (example: the official
  `png_info_destroy` reads `[esi+0x220]`, the original `[esi+0x1d0]`; `png_struct+0x40` is `error_fn`, right after
  the 64-byte `jmp_buf`). zlib's `deflate_state` offsets did match in the one function checked (`_tr_init`).
- Known source-shape difference: the original inlines static callees that the official file defines LATER in the same
  file (zlib `_tr_init` 0x00528e30 has `init_block` inlined; `tr_static_init` is empty). No flag set reproduces that
  with the official file order (tried /Ob2, /GL + /LTCG). So when the original has a static callee's body inline,
  write that body inline in your C (or define the static callee above the function as `static` and let /Ob1 skip it:
  test both).
- Do NOT `#include` png.h / zlib.h (no `#include` at all). Write the code with explicit offsets (`*(int *)(p + 0x1d0)`)
  or with a small local struct typedef with explicit padding, whichever matches. Struct typedefs are free.

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
  (copy back what helps). Use it on 80-99% functions. Keep --jobs 2 (other helpers share the machine), one at a time.
- Starting points: `D:/MashedWT/MATCH-S23_data/g2s/<va8>.c` (raw Ghidra output made compilable, scored in
  `D:/MashedWT/MATCH-S23_data/g2s/results.tsv`, column `flags` = which flag set won) and
  `D:/MashedWT/MATCH-S23_data/decomp.json` (decomp + prototype + typed globals + callee prototypes per function; read
  it with a small python one-liner for your RVA, do not cat it). 29 functions did not compile from the raw converter
  (stage `compile` in results.tsv; `g2s/<va8>.c` holds the attempt): for those, write from the library source +
  decomp directly.
- `py -3.12 D:/MashedWT/MATCH-S23_data/tools/to_s23.py <va8>` writes the raw converter output in the required form to
  `re/match/s23/<va8>.c` (FLAGS line, preamble, globals as MGLOBAL, LF endings; the raw files have CR CR LF endings,
  so do not copy them by hand). Works only where the raw output compiled.
- `py -3.12 D:/MashedWT/MATCH-S23_data/tools/flagsweep.py <va8>` runs 5 FLAGS variants on a scratch copy.
- Worked examples: `re/match/pilot/*.c` (header comments say what moved the score), `re/match/s19/004c3c60.c`,
  `re/match/s04/0041e8b0.c` (by hand), and this slice's raw matches `re/match/s23/005171a0.c`, `00518080.c`.
- Read `verify/matchpilot_20261030/RESULT_MATCH-PILOT.md` section "Findings that change how to match this game"
  (static callees for in_EAX/unaff_ESI args, file-static globals, wrong Ghidra callee arity, raw converter limits).

## Flags
libpng/zlib measured identical at `/O2 /G5` = `/O2 /Ob1 /G5` (MATCH-LIB, 13 flag sets; the game flags `/O2 /GS /Ob1`
gave 8 of 42). In this slice's raw converter `/O2 /Ob1 /G5` won 63, `/O2 /GS /Ob1` 16, equal 39. Default
`// FLAGS: /O2 /Ob1 /G5`. Try the other set once when stuck (functions with a `__security_check_cookie` call are /GS).

## Source form (REQUIRED: these files later become the port's function bodies)
One file per function: `re/match/s23/<va8>.c` (va8 = 8 hex digits, lowercase, no 0x). Layout:
1. line 1: `// FLAGS: /O2 /Ob1 /G5` (or the set that matched)
2. a header comment: best exact %, regs %, the library function it is (`libpng 1.0.12 pngrtran.c png_do_unpack`),
   what you tried, residual class (below).
3. the preamble, copied VERBATIM from `D:/MashedWT/MATCH-S23_data/tools/preamble.h` (do not edit it).
4. globals ONLY through the preamble macros, one per line, with the real address:
   `MGLOBAL(<type>, DAT_xxxxxxxx, 0xxxxxxxxx);` or for arrays `MGLOBALA(<type>, DAT_xxxxxxxx, <N>, 0xxxxxxxxx);`.
   Never a plain `extern` for a global. A string-typed global -> `MGLOBALA(char, s_..., <N>, 0x...)`. A file-`static`
   global (pilot finding 2): `static <type> DAT_x;` under `#ifndef __cplusplus` and `MGLOBAL(...)` under `#else`.
   Library constant tables (zlib `cplens`, `inflate_mask`, libpng `png_pass_start`, `png_IHDR` etc.) are globals at
   their image address: declare them with MGLOBALA at the address the original reads.
5. callee prototypes with the exact arity and the convention the callee's own `ret N` shows. Imports:
   `__declspec(dllimport) <ret> __stdcall Name(...)`. CRT helpers (`_memcpy`, `_setjmp3`, `_longjmp`, `_free`)
   are called by their CRT name (relocheck leaves them unchecked by name, fine).
6. the function(s), each preceded by `// MATCH: 0x<va> <symbol>`. Name the function `FUN_<va8>` exactly as Ghidra.
   A static callee you add for register-passed args also gets its own `// MATCH:` line when it is the real callee;
   name it `FUN_<its va8>` too, and say in the header comment that the file holds it.
- No `__asm`, no emitted bytes, no `#pragma` other than `#pragma intrinsic`. No `#include`.
- Call every game callee as `FUN_<va8>` (callee addresses: decomp.json `callees`, `Name@0x...`).
- The file MUST ALSO compile as C++ (the port includes it into a .cpp). Check every 100% file with
  `cmd /c "D:\MashedWT\MATCH-S23_data\tools\cxxcheck.bat re\match\s23\<va8>.c"` from the **PowerShell** tool (from
  Git Bash `cmd //c` hangs on this machine) (VS2022 /TP, MATCH_PORT; the `vswhere` line is noise; any `error C` line
  is a failure). Typical fixes: explicit casts for void*/int->pointer conversions, real function-pointer types instead
  of Ghidra's `code`, prototypes with the right arity. If a C++ fix costs match %, keep the match and report the C++
  failure in the header comment.
- Every 100% file must also pass `py -3.12 re/tools/match/relocheck.py re/match/s23/<va8>.c` -> `OK` (`unchecked=0`
  where possible). A BAD line is a wrong global/callee: fix it.

## Budget (bounded: 108 functions over 5 helpers, not a research project)
About 8 min per function under 100 B, 20 min for 100-400 B, 40 min above 400 B; above 1000 B 60 min, and skip those
if your smaller ones are not done. Stop at the budget, keep the best version, record it. Large functions: get
structure (types, loops, switch, case order) right first, then the permuter. Do the functions in the order given
(up to 400 B first in address order, then the large ones smallest first). **Yield matters more than depth**: a quick
pass over every function first beats a deep dive into one.

## Classify every remaining difference
- (a) compiler-version codegen: survives every plausible rewrite you tried AND is an optimizer/scheduling choice
  that no statement order / type / spelling change moves. Say what you tried.
- (b) source shape: a source form not found yet (struct layout, merged temporaries, inlined callee, loop/switch
  form, register allocation that moves when source changes). "Ran out of budget" is (b).
- (c) tool limit: wrong function boundary, non-standard register convention, C++ EH, thiscall, setjmp.

## Rules for parallel work
- Run Python with `MSYS_NO_PATHCONV=1` exported if you use Git Bash (otherwise `/O2` becomes a path), and pass
  Windows-style paths (`C:/...`, `D:/...`) to Python, never `/c/...`.
- Do not touch another helper's files. Your function list is in your task; other helpers work in parallel on other
  addresses in the same `re/match/s23/` directory. Scratch files go to `D:/MashedWT/MATCH-S23_data/scratch/<id>/`.
- When a file of yours already exists in `re/match/s23/` at 100%, leave it.
- 5-byte `jmp <other>` functions: `<ret> FUN_x(<args>) { return FUN_<target>(<args>); }`, target declared extern.
- Tail-jump dispatchers pick the scratch register by stack-arg count (0/3 -> eax, 1/4 -> ecx, 2/5 -> edx).
- Other MATCH slices run on other branches at the same time (`D:/MashedWT/MATCH-S*_data`): do not touch them.
- If something general moves many functions (a struct layout that fixes a family, a flag), write it at the top of
  your notes file so the other helpers' notes can pick it up; you may read `D:/MashedWT/MATCH-S23_data/notes/*.md`.

## Report back
Write `D:/MashedWT/MATCH-S23_data/notes/<your helper id>.md`, appending one line per function **as you finish it**:
`0x<va>  size  best%  regs%  cxx:ok|FAIL  class  one-line reason`, plus any static callee a file holds
(`0x<file va> holds 0x<callee va>`). End with the same compact list as your final message. Nothing else.
