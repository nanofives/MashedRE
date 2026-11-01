# MATCH-S05 body-swap helper brief (2026-11-01)

Goal: for each RVA you are given, make `mashed_re.exe` (and the .asi where the RVA has a body there) use the
byte-matched source `re/match/s05/<va8>.c` as the function's ONLY body, without changing what the port computes.

Worktree (run everything from here): `C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\MATCH-S05`
Never: git commands that mutate, build.bat / any .bat / .ps1 edit or run, MASHED.exe / mashed_re.exe runs, Ghidra,
trackers (hooks.csv, STUBS.md, UNCERTAINTIES.md, DEFERRED.md), `mashedmod/*.rsp`, `mashedmod/x87_tus.txt`,
`re/tools/dual_copy_allowlist.txt` (the lead edits those four from your report). Touch only the old TUs named in
your task, `mashedmod/src/mashed_re/Match/MatchS05_<va8>.cpp`, and `re/match/s05/<va8>.c` (C++-compat edits only).

## How a swap looks (done example on the S02 branch: `git show orch/MATCH-S02:mashedmod/src/mashed_re/Match/MatchS02_0040b7b0.cpp` and
`git diff mashedmod/src/mashed_re/Frontend/MenuScoreGetters.cpp`)
1. Read the old body(ies) of the RVA. Find them with
   `py -3.12 -c "import sys;sys.path.insert(0,'scripts');import rva_body_scan as S;s=S.scan_all();[print(t,b[0x<RVA>]) for t,b in s['bodies'].items() if 0x<RVA> in b]"`
   and the active installs the same way via `s['installs']`. Note every TU that has a body (exe-only, asi-only,
   both). Find the port's callers of the old symbol (`grep -rn <Symbol> mashedmod/src`).
2. Decide if the swap is SAFE: the matched source must compute what the old exe body computes, reading/writing the
   same memory. SAFE when the old body reads/writes the same original addresses (absolute `0x00xxxxxx` casts, or
   port globals that are themselves defined at those addresses) and calls the same callees. NOT SAFE (do not swap,
   report it) when the old body: reads/writes port-owned state at other addresses (a C++ object, a port global,
   a struct member not at the original address), skips/replaces a callee, has a knob/env branch, or differs in
   behaviour from the matched source in any way. Report exactly what differs.
3. If SAFE: remove the old body/bodies with
   `py -3.12 D:/MashedWT/MATCH-S05_data/tools/excise.py <TU rel to mashedmod/src/mashed_re> <va8> ...`
   (it prints what it removed: check it removed exactly the function and its RH_ScopedInstall; fix by hand if not).
   Keep any thin wrapper/caller the old TU has that is NOT the RVA's body (e.g. an exported entry that only calls
   the core), but it must call the matched function's symbol.
4. Write the wrapper: `py -3.12 D:/MashedWT/MATCH-S05_data/tools/mkwrap.py <va8> <PortSymbol> <install 0|1>
   [--bind FUN_<callee va8>=<PortSymbolOfCallee> ...] --old <old TU>`.
   `<PortSymbol>` = the symbol the port's callers and the old RH_ScopedInstall use (keep it, so no caller changes).
   install=1 when an old ACTIVE RH_ScopedInstall existed for the RVA (it moves into the wrapper).
   Every callee the matched source calls must bind to the port's existing implementation: `--bind
   FUN_<va8>=<PortSymbol>`; the port symbol must be callable with C linkage and a compatible signature. If a
   callee's port symbol is C++ (namespace/member) add a tiny `extern "C"` forwarding function in the wrapper
   ABOVE the include (that is not a body of the callee RVA: give it NO RVA comment) and bind to it. If the port has
   no implementation of a callee at all, the swap is NOT SAFE: report it.
   A matched function the port uses as a C++ member (`Class::Method`): keep the member as a one-line forwarder to
   the matched symbol only if the member adds nothing; delete its RVA comment so it is not a second body.
5. Compile-check the wrapper alone:
   `cmd //c "D:\\MashedWT\\MATCH-S05_data\\tools\\wrapcheck.bat mashedmod\\src\\mashed_re\\Match\\MatchS05_<va8>.cpp"`
   and the edited old TU the same way. If the .c needs C++ fixes, make them (casts, real function-pointer types,
   prototypes), then re-verify the DDK match is still 100%: `py -3.12 re/tools/match/match.py re/match/s05/<va8>.c`.
   A C++ fix that drops the match below 100% is not allowed: report instead.
6. Does the function use float/double? Then its wrapper needs x87 (report `x87: yes`).

## Report (final message, compact, one line per RVA)
`0x<va8>  SWAPPED|NOT-SAFE|SKIP  symbol=<PortSymbol>  targets=exe|asi|both  install=0|1  x87=yes|no
 removed=<TU:lines,...>  binds=<...>  note=<one line: what differs if NOT-SAFE; callers; anything odd>`
Also list any dual_copy_allowlist.txt entry that your swap makes stale (RVA had bodies in two TUs).

## Register-passed static callees (pilot finding 1)
A matched file whose function is `static` only so the DDK passes an argument in a register (e.g. 0x004103a0) cannot
stay `static` in the port (other TUs call it). Write `MATCH_STATIC` instead of `static` on that definition and add,
right after the preamble: `#ifdef __cplusplus` / `#define MATCH_STATIC` / `#else` / `#define MATCH_STATIC static` /
`#endif`. A stand-in caller that exists only to make the DDK pick the register convention goes under
`#ifndef __cplusplus` (the port never compiles it). Re-verify 100% after the edit.

## S05 additions
- The S02 diff for the example: `git diff orch/integration orch/MATCH-S02 -- mashedmod/src/mashed_re/Frontend/MenuScoreGetters.cpp`.
  Raw-address forwarders (S02 0x0040dfc0, `git show orch/MATCH-S02:mashedmod/src/mashed_re/Match/MatchS02_0040dfc0.cpp`):
  when the OLD body called a callee through its raw original address (a function-pointer cast to `0x00xxxxxx`,
  no port implementation), the swap is SAFE with a small `extern "C"` forwarder in the wrapper that makes that same
  raw-address call (no RVA comment on it, named `MatchS05_<va8>_Call<callee va8>`). Bind `FUN_<callee>` to it.
  When the old body called a port implementation, bind to that implementation instead.
- A matched function that is a tail-jump dispatcher with N int args (e.g. 0x0041e8b0) may be declared elsewhere in
  the port with a different parameter list: check every declaration of the port symbol (`grep -rn <Symbol>
  mashedmod/src`). C-linkage callers with a different arg count are still correct (the jmp passes the caller's
  stack through), but a C++-linkage declaration will not link: report it, or make the caller's declaration match.
- "targets": an old body in a TU listed in both rsp files can still be exe-less (`#ifndef MASHED_STANDALONE`).
  Check the preprocessor guards around the old body and report exe / asi / both accordingly.
- Use `py -3.12 D:/MashedWT/MATCH-S05_data/tools/excise.py` / `mkwrap.py` / `wrapcheck.bat` (S05 copies; they write
  `MatchS05_` names). Export Git Bash `MSYS_NO_PATHCONV=1` before running `cmd //c` or Python with `/` flags.
- Other helpers edit other TUs at the same time: touch only the TUs named in your task.

## S05 additions (L-72, Mariano 2026-11-01: MATCH goes top-down)
- Matched CALLERS are swapped in too, and a matched callee under a swapped matched caller becomes a port body
  (bind `FUN_<callee>` to the callee's matched symbol, both wrappers in the same targets as the caller) so the
  port runs the original chain. When the old body of a caller called a slice callee that is matched in
  `re/match/s05/`, bind to that matched source's symbol (the lead gives you the callee's wrapper symbol or tells
  you to make it). A matched callee with no old body anywhere gets a wrapper with install=0 and a fresh symbol
  `FUN_<va8>` (no #define needed) when a swapped caller calls it.
- In `mashed_re.exe` there is no original image: a raw-address forwarder (`0x00xxxxxx` call) only stays SAFE in
  the exe when the old exe body made that same raw call. Never add a new raw-address call into exe code.
- Report for every swapped RVA the port callers you found (`grep -rn <Symbol> mashedmod/src`, excluding the
  definition, comments, RH_ScopedInstall and #pragma EXPORT), split exe / asi-only (check `#ifndef
  MASHED_STANDALONE` guards and whether the caller TU is in `mashedmod/exe_sources.rsp`).

## S05 lead notes for every swap helper
- Port symbols must stay what other TUs call. Cross-helper binds (another helper keeps these symbols, bind to them):
  0x004215c0 -> `Accum4215c0` (W2), 0x00422440 -> `CosineLerp` (W1; check the real C symbol in Math/CosineLerp.cpp),
  0x00423480 -> `AiFilenameBuild` (W3), 0x00422120 -> `TimerInitLoop` (W1).
- A callee that gets an argument in a REGISTER in the original (a `static` function in a held file, EAX/ECX/ESI/...)
  can be bound only when the caller's own file holds it. A raw source that calls such a callee as `FUN_x()` with no
  args (e.g. 00421c10.c -> 0x00421980) matches by accident and is NOT SAFE as a port body: report it.
- Duplicate sources were moved out to `D:/MashedWT/MATCH-S05_data/dup_sources/`; use only `re/match/s05/<va8>.c`
  files named in your task. Results: `verify/matchs05_20261101/results.tsv`; pre-swap bodies:
  `D:/MashedWT/MATCH-S05_data/bodies.tsv`.
- Do NOT touch 0x004233e0 (dual body owned by D4 row D4-DC-INC-004233e0) or 0x00408a50.
- Report also: wrapper file, whether each RVA the wrapper's file holds becomes a NEW body (no earlier body), and the
  exe callers you found by symbol (TU:line), so the lead can count exe-callers.
