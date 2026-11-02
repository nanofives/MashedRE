# MATCH-S06 body-swap helper brief (2026-11-01)

Goal: for each RVA you are given, make `mashed_re.exe` (and the .asi where the RVA has a body there) use the
byte-matched source `re/match/s06/<va8>.c` as the function's ONLY body, without changing what the port computes.

Worktree (run everything from here): `C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\MATCH-S06`
Never: git commands that mutate, build.bat / any .bat / .ps1 edit or run, MASHED.exe / mashed_re.exe runs, Ghidra,
trackers (hooks.csv, STUBS.md, UNCERTAINTIES.md, DEFERRED.md), `mashedmod/*.rsp`, `mashedmod/x87_tus.txt`,
`re/tools/dual_copy_allowlist.txt` (the lead edits those four from your report). Touch only the old TUs named in
your task, `mashedmod/src/mashed_re/Match/MatchS06_<va8>.cpp`, and `re/match/s06/<va8>.c` (C++-compat edits only).

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
   `py -3.12 D:/MashedWT/MATCH-S06_data/tools/excise.py <TU rel to mashedmod/src/mashed_re> <va8> ...`
   (it prints what it removed: check it removed exactly the function and its RH_ScopedInstall; fix by hand if not).
   Keep any thin wrapper/caller the old TU has that is NOT the RVA's body (e.g. an exported entry that only calls
   the core), but it must call the matched function's symbol.
4. Write the wrapper: `py -3.12 D:/MashedWT/MATCH-S06_data/tools/mkwrap.py <va8> <PortSymbol> <install 0|1>
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
   `cmd //c "D:\\MashedWT\\MATCH-S06_data\\tools\\wrapcheck.bat mashedmod\\src\\mashed_re\\Match\\MatchS06_<va8>.cpp"`
   and the edited old TU the same way. If the .c needs C++ fixes, make them (casts, real function-pointer types,
   prototypes), then re-verify the DDK match is still 100%: `py -3.12 re/tools/match/match.py re/match/s06/<va8>.c`.
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

## S06 additions
- The S02 diff for the example: `git diff orch/integration orch/MATCH-S02 -- mashedmod/src/mashed_re/Frontend/MenuScoreGetters.cpp`.
  Raw-address forwarders (S02 0x0040dfc0, `git show orch/MATCH-S02:mashedmod/src/mashed_re/Match/MatchS02_0040dfc0.cpp`):
  when the OLD body called a callee through its raw original address (a function-pointer cast to `0x00xxxxxx`,
  no port implementation), the swap is SAFE with a small `extern "C"` forwarder in the wrapper that makes that same
  raw-address call (no RVA comment on it, named `MatchS06_<va8>_Call<callee va8>`). Bind `FUN_<callee>` to it.
  When the old body called a port implementation, bind to that implementation instead.
- A matched function that is a tail-jump dispatcher with N int args (e.g. 0x0041e8b0) may be declared elsewhere in
  the port with a different parameter list: check every declaration of the port symbol (`grep -rn <Symbol>
  mashedmod/src`). C-linkage callers with a different arg count are still correct (the jmp passes the caller's
  stack through), but a C++-linkage declaration will not link: report it, or make the caller's declaration match.
- "targets": an old body in a TU listed in both rsp files can still be exe-less (`#ifndef MASHED_STANDALONE`).
  Check the preprocessor guards around the old body and report exe / asi / both accordingly.
- Use `py -3.12 D:/MashedWT/MATCH-S06_data/tools/excise.py` / `mkwrap.py` / `wrapcheck.bat` (S06 copies; they write
  `MatchS06_` names). Export Git Bash `MSYS_NO_PATHCONV=1` before running `cmd //c` or Python with `/` flags.
- Other helpers edit other TUs at the same time: touch only the TUs named in your task.
- `cmd //c` under `MSYS_NO_PATHCONV=1` opens an interactive cmd: use `cmd /c "..."` instead.

## Top-down rule (user decision L-72, read this)
MATCH is top-down: a matched CALLER is swapped in too, so the port's entry points run the original chain.
- A matched function whose RVA has NO port body today becomes a new body when it is a callee of a function you
  swap (bind `FUN_<callee>` to it instead of a raw-address forwarder) or the caller of one. Port symbol for a new
  body: `FUN_<va8>` itself unless the task names one. install=0 for a new body (no hook existed). Put it in the
  same target(s) as the swapped function that calls it.
- A callee that is NOT matched and has a port body: bind to that port body (as in S04). A callee with neither: a
  raw-address forwarder (S04 rule). Say which in `binds=`.
- An RVA with two or more bodies today (dual copies, e.g. 0x00426bb0 has four): the swap removes every copy that is
  a body of that RVA (one body per RVA after the swap). Do not touch `re/tools/dual_copy_allowlist.txt`: report
  which of its entries your swap makes stale and the lead edits it.
- Never touch 0x00408a50 (another lead owns it).

## Batch 2 (sw3..sw6)
- **Symbol map**: `D:/MashedWT/MATCH-S06_data/symmap.tsv` (rva, port_symbol, state, owner helper, old body). Every
  slice RVA is there. Bind `FUN_<va8>` callees with it: `swapped-batch1` / `swap-batch2` / `new-body-batch2` ->
  bind to that port_symbol (another helper may be writing it right now: that is fine, the link happens later).
  `matched-not-swapped` and `unmatched` slice callees, and callees outside the slice: the port body if one exists
  (find it with rva_body_scan), else a raw-address forwarder (S04 rule).
- **Targets**: every batch-2 wrapper goes in BOTH targets (report `targets=both`) unless something in it cannot
  link or compile in one of them; then say why. A chain caller in the exe needs its callees in the exe too, so
  report any batch-1 asi-only callee your exe body needs (e.g. 0x00427780, 0x00426cd0): the lead moves it.
- **Held static callees (one body per RVA).** Some files hold a copy of a static callee next to their own function
  so the DDK passes its args in registers. Canonical body = the callee's OWN file where one exists:
  0x004277a0 -> 004277a0.c, 0x00427680 -> 00427680.c, 0x00427840 -> 00427840.c, 0x00427880 -> 00427880.c.
  Where no own file exists the holder is canonical and keeps its `// MATCH:` line: 0x00429240 in 00429290.c,
  0x00426460 in 004264d0.c. In every OTHER holder file, convert the held copy:
  ```
  #ifndef __cplusplus
  // MATCH-HELD: 0x004277a0 (static copy for the DDK register convention; port body: re/match/s06/004277a0.c)
  static ... FUN_004277a0(...) { ...unchanged... }
  #else
  <same return type> FUN_004277a0(<same params>);   /* port: one body, bound by the wrapper's #define */
  #endif
  ```
  (`// MATCH-HELD:` does not match the scanner's `// MATCH:` pattern, so the copy is not a second body.) The holder's
  wrapper then `--bind`s `FUN_004277a0` to the canonical port symbol from symmap.tsv. In the canonical own file,
  the callee definition must use `MATCH_STATIC` (pilot finding 1, see above) so the port can call it from other TUs.
  Re-run match.py on every file you edit: the holder's own `// MATCH:` function must stay 100%.
- **0x0055deb0 is NOT in this slice**: 004292d0.c defines it in-file. Convert it the same way (MATCH-HELD,
  `#ifndef __cplusplus`), and bind `FUN_0055deb0` to its existing port body or a raw-address forwarder.
- `x87=yes` for any function that uses float/double (the lead adds it to x87_tus.txt).
- Old bodies that are listed in `mashedmod/x87_tus.txt` by TU name (e.g. ViewportScaledRectDraw, MenuMenusBB) may
  become empty after the excise: report it, the lead edits x87_tus.txt / the rsp files.
- 0x00429300 (sw5): swap it although sw2 called it NOT-SAFE. The old body rounds the int at 0x008991b8 to float
  before the x87 return, the matched source returns it unrounded as the original does. That moves the port TO the
  original; the gates decide, and a gate move becomes a decision row. Report it as SWAPPED with that note.
- Wrapper check with `cmd /c` (not `cmd //c`) under MSYS_NO_PATHCONV=1, both without and with
  /DMASHED_STANDALONE (wrapcheck.bat defines MASHED_STANDALONE; also compile once without it for the asi).
