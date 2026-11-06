# MATCH-S09 body-swap helper brief (2026-11-02)

Goal: for each RVA you are given, make `mashed_re.exe` (and the .asi where the RVA has a body there) use the
byte-matched source `re/match/s09/<va8>.c` as the function's ONLY body, without changing what the port computes.

Worktree (run everything from here): `C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\MATCH-S09`
Never: git commands that mutate, build.bat / any .bat / .ps1 edit or run, MASHED.exe / mashed_re.exe runs, Ghidra,
trackers (hooks.csv, STUBS.md, UNCERTAINTIES.md, DEFERRED.md), `mashedmod/*.rsp`, `mashedmod/x87_tus.txt`,
`re/tools/dual_copy_allowlist.txt` (the lead edits those four from your report). Touch only the old TUs named in
your task, `mashedmod/src/mashed_re/Match/MatchS09_<va8>.cpp`, and `re/match/s09/<va8>.c` (C++-compat edits only).

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
   `py -3.12 D:/MashedWT/MATCH-S09_data/tools/excise.py <TU rel to mashedmod/src/mashed_re> <va8> ...`
   (it prints what it removed: check it removed exactly the function and its RH_ScopedInstall; fix by hand if not).
   Keep any thin wrapper/caller the old TU has that is NOT the RVA's body (e.g. an exported entry that only calls
   the core), but it must call the matched function's symbol.
4. Write the wrapper: `py -3.12 D:/MashedWT/MATCH-S09_data/tools/mkwrap.py <va8> <PortSymbol> <install 0|1>
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
   `cmd //c "D:\\MashedWT\\MATCH-S09_data\\tools\\wrapcheck.bat mashedmod\\src\\mashed_re\\Match\\MatchS09_<va8>.cpp"`
   and the edited old TU the same way. If the .c needs C++ fixes, make them (casts, real function-pointer types,
   prototypes), then re-verify the DDK match is still 100%: `py -3.12 re/tools/match/match.py re/match/s09/<va8>.c`.
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

## Slice additions
- The S02 diff for the example: `git diff orch/integration orch/MATCH-S02 -- mashedmod/src/mashed_re/Frontend/MenuScoreGetters.cpp`.
  Raw-address forwarders (S02 0x0040dfc0, `git show orch/MATCH-S02:mashedmod/src/mashed_re/Match/MatchS02_0040dfc0.cpp`):
  when the OLD body called a callee through its raw original address (a function-pointer cast to `0x00xxxxxx`,
  no port implementation), the swap is SAFE with a small `extern "C"` forwarder in the wrapper that makes that same
  raw-address call (no RVA comment on it, named `MatchS09_<va8>_Call<callee va8>`). Bind `FUN_<callee>` to it.
  When the old body called a port implementation, bind to that implementation instead.
- A matched function that is a tail-jump dispatcher with N int args (e.g. 0x0041e8b0) may be declared elsewhere in
  the port with a different parameter list: check every declaration of the port symbol (`grep -rn <Symbol>
  mashedmod/src`). C-linkage callers with a different arg count are still correct (the jmp passes the caller's
  stack through), but a C++-linkage declaration will not link: report it, or make the caller's declaration match.
- "targets": an old body in a TU listed in both rsp files can still be exe-less (`#ifndef MASHED_STANDALONE`).
  Check the preprocessor guards around the old body and report exe / asi / both accordingly.
- Use `py -3.12 D:/MashedWT/MATCH-S09_data/tools/excise.py` / `mkwrap.py` / `wrapcheck.bat` (S09 copies; they write
  `MatchS09_` names). Export Git Bash `MSYS_NO_PATHCONV=1` before running `cmd //c` or Python with `/` flags.
- Other helpers edit other TUs at the same time: touch only the TUs named in your task.

## Slice additions
- Report, per SWAPPED RVA, the exe callers of the port symbol you found (`file:line` of each active call in a TU of
  `mashedmod/exe_sources.rsp`, outside `#ifndef MASHED_STANDALONE`; "none" when there is none). Definitions,
  prototypes, `#define`, `/EXPORT` pragmas and `RH_ScopedInstall` are not callers. A raw-address call
  (`reinterpret_cast<...>(0x00xxxxxx)`) in the exe calls the original image, not the port body: not a caller.
- An RVA whose `.c` holds a `static` callee with its own `// MATCH:` line makes that callee a port body too: if the
  callee already has a port body elsewhere, report it (two bodies) instead of swapping.
- `mashedmod/src/mashed_re/Util/PromoLoop_*.cpp` and `Render/PromoLoop_*.cpp` are .asi-only promotion TUs; a body
  there is asi-only (targets=asi).
- Other MATCH slices (S08, S10, top-down rows) work on other branches; this branch only. Do not touch other RVAs' bodies in a TU
  beyond what excise removes.

## Round 2: held static callees (a matched file that defines another RVA's function)
Some matched files define a callee in-file (pilot finding 1: register-passed args, or codegen that needs the callee's
body in the same object), with its own `// MATCH: 0x<callee>` line. The scanner counts every `// MATCH:` line of an
included file as a body, so when the callee already has (or will get) its own port body, the file must not be a
second body of it. Convert the in-file callee to a HELD copy:
1. Replace its `// MATCH: 0x<callee> FUN_<callee>` line with `// HELD: 0x<callee> (body elsewhere; this copy exists
   only so the DDK compiles the caller as the original; not compiled in the port)`.
2. Wrap the callee's definition in `#ifndef MATCH_PORT` ... `#else` <prototype with the same signature> `#endif`.
3. In the wrapper, `--bind FUN_<callee>=<the callee's port symbol>` (its existing body: an S0x wrapper, the old TU, or
   another slice's body found by rva_body_scan). Never bind to a raw address when a port body exists.
4. Re-verify: `match.py` still 100% for the caller's `// MATCH:` line (the HELD line is not compared), `relocheck.py`
   OK, the C++ check passes.
When the callee has NO port body anywhere and is in this slice's matched set, keep its `// MATCH:` line (the file
becomes its only body) and say so.

## S09 additions
- Run `wrapcheck.bat` from the **PowerShell** tool: `cmd /c "D:\MashedWT\MATCH-S09_data\tools\wrapcheck.bat mashedmod\src\mashed_re\Match\MatchS09_<va8>.cpp"`
  (from Git Bash `cmd //c` hangs on this machine). Python from Git Bash is fine with `MSYS_NO_PATHCONV=1`.
- This branch is cut from orch/integration 035dc5bff: MATCH-S01..S07 are merged (their `Match/MatchS0x_*.cpp` bodies are
  here), MATCH-S08 is NOT (merge-queued on another branch). A held callee binds to the body `rva_body_scan` finds on
  THIS branch. If the held callee has no port body on this branch and the old body called it by raw address, use a
  raw-address forwarder.
- Raw `0x0040e470` forwarders: bind `FUN_0040e470` to `CarSlotStateGet` (`Ai/CarSlotState.cpp`, both targets), as
  S05..S08 did.
- Many S09 sources hold callees in-file (pilot finding 1: register-passed args; see
  `verify/matchs09_20261102/helper_notes.txt`, lines `0x<file> holds 0x<callee>`). Several files hold each other's
  callees with full `// MATCH:` lines (e.g. 00454170.c and 00454790.c). Before swapping a file, list every `// MATCH:`
  line in it: each one must become the ONLY port body of its RVA, or be converted to HELD (round 2). A held callee
  that is below 100% (e.g. 0x00458570 at 90.0% inside 00458920.c) must be HELD, never a body.
- Gameplay leaves that run in the gate scenario (pickups, powerups): be strict on SAFE. Report `x87=yes` for every
  float function; the lead lists them in `mashedmod/x87_tus.txt`.
- Do NOT edit `mashedmod/*.rsp`, `mashedmod/x87_tus.txt`, `re/tools/dual_copy_allowlist.txt`: the lead does.
