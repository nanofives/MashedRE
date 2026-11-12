# RESULT PROVISIONAL-BODIES (D4, run 38, 2026-11-12) - HANDOFF

Branch orch/PROVISIONAL-BODIES = orch/integration 32c958b55 + merge of orch/PROV-DIVERGED-18 (14b2186fe, carries
re/match/PROVISIONAL.tsv) + commit 2c365ba0a. Row kind: top-down.

## 1. Landed: the L-126 (6a) convention (tools and list, no build needed)
- `re/match/PROVISIONAL.tsv`: columns source, best_pct, equiv first difference, replay, status. 21 data rows now.
  Added here: `0x00499400` (s16, 90.1%, NOT-PROVEN call-regargs at call 0x498a00, old and fixed equiv.py both;
  RESULT_PROV-DIVERGED-18 section 5) and `0x0043dfd0` (s08, 17.7%, NOT-PROVEN x87-region at +0x0, `fst m4[esp+0xc]`
  against `fst m4[esp+0x4]`; replay NOT-EXECUTED, 54 menu calls before the warp).
  Note: `0x00499400` had no row in re/match/ADMITTED.tsv at orch/integration 32c958b55 (grep: 0 hits, no history),
  so it was listed directly instead of moved. No BINDINGS.tsv row exists for it either.
- `re/tools/ddkobj/ddkobj.py`: `load_provisional()` reads PROVISIONAL.tsv. A unit whose RVAs are all listed there, or
  have BINDINGS kind `provisional`, is skipped with status `skip:provisional` (VS2022 body kept, never in the object
  build, never in the byte-check count). `ddkobj_summary.json` gains `provisional_units_skipped`.
  Checked: loader returns 21 RVAs including 0x0043dfd0, 0x00499400, 0x00408b00. The tool parses. `ddkobj.py
  selftest` / `check` not run (no build, see section 3).
- `re/tools/port_closed_sets.py`: BINDINGS kind `provisional` is never a binding (callee stays `open`, why
  "provisional"), so it cannot enter the port column or a closed set.
- Convention for BINDINGS.tsv: kind `provisional`, exe_symbol the provisional wrapper's symbol, note "PROVISIONAL.tsv
  row, never C4, never byte-matched". No BINDINGS row was added: no provisional body is compiled in the exe yet.

## 2. Not done: wiring 0x0043dfd0, 0x00492d30 stand-in, 0x004111c0 on the knob path
- Cause 1, hard: `mashed_lock.py status` at start showed a merge-train PRIORITY-HOLD (pid 26636, since 00:09:03) and a
  SOLO (merge-train13, expires in about 14300 s). Rule: no `build.bat` and no game boot then. This worktree has no
  `mashedmod/build` yet, so both builds, `ddkobj.py check`, the knob boot, its self-test and the menu walk are not run.
  Nothing was wired without a compile, so default behaviour is untouched (no exe source changed).
- Cause 2, scope: `re/match/s08/0043dfd0.c` is 2115 lines with 64 distinct FUN_ callees and about 820 global refs.
  Wiring it as a provisional body needs a `Match/MatchS08_0043dfd0.cpp` wrapper (preamble MATCH_PORT, `#define
  FUN_<rva>` per callee) and a knob in `PortTop_StandIn_00492d30` (Match/PortTop_00492370.cpp:236) that calls it in
  place of the menu-nav part of `HandTop_FrameUpdate` (exe_main.cpp:10496). Callee survey (grep of mashedmod/src for
  each rva, not a compile): every one of the 64 has at least one mention except 0x0042f400 and 0x004324a0 (0 files).
  Whether each mention is a C-linkage exe body is not established. BINDINGS marks 0x0043dfd0 `under-stand-in`
  (parent 0x00492d30, port=called Race/RaceModes.cpp) and 0x004111c0 `under-stand-in` (port=body
  Match/MatchT01G_004111c0.cpp).
- 0x00492d30: the matched source (Match/MatchS15_00492d30.cpp) is asi-only. Its callees 0x004030d0, 0x0043d7c0,
  0x00496920, 0x00498860 have port=none in BINDINGS, so the frame subtree cannot run on the exe until they have bodies.

## 3. Handoff (next worker)
1. Wait for `mashed_lock.py status` to show no PRIORITY-HOLD / SOLO. `cmd /c "set MASHED_NO_DEPLOY=1&& mashedmod\build.bat"`
   then `set MASHED_DDKOBJ=1` build, then `py.exe -3.12 re/tools/ddkobj/ddkobj.py check` and `selftest` (expect the new
   `skip:provisional` units, `provisional_units_skipped` > 0, IDENTICAL count unchanged).
2. Order: 0x0043dfd0 first. Build the callee table (rva, exe body symbol or none) with
   `re/tools/port_closed_sets.py`, wrap callees with no exe body as logging stubs behind the knob, add the knob
   default OFF plus stack file `verify/savelink_20261023/stacks/<TAG>.txt`, boot with a private root
   `D:\MashedWT\PROVISIONAL-BODIES_root` under `mashed_lock.py run --label PROVISIONAL-BODIES`, menu walk ON, OFF, ON,
   OFF compared (P1 class).
3. Then the 0x00492d30 frame subtree and 0x004111c0.

## 4. Gates
Not run: top-down row (G-CTRL / B / 12 captures not required) and no exe source changed.
