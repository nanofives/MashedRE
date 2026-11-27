Binder selftest: before (head 2637e5b37) PASS 0 on all 6 profiles (raceload, audio, bulk, raceframe, renderframe, gates); after (orch/BIND-LIB-VOIDPROTO) PASS 0 on all 6 profiles; the two logs are byte-identical (sha256 35542de2...45967 both).

# RESULT BIND-LIB-VOIDPROTO (run 56, BIND section, binder tool row)

Verdict: GREEN. Base orch/integration 2637e5b37. Gates: not run (tool row, no G-CTRL / B in the worker).

## What changed

- `re/tools/bind/porttop_bind.py`: in `wrapper_text`, a C-unit callee whose plan entry is a lib reuse
  (`('reuse', sym, 'lib:...')`) and whose source call passes more arguments than the source prototype declares no longer
  takes the generic drop form `#define FUN_x(...) sym()`. It goes through the new `lib_keep_args`, which keeps every
  caller-side argument:
  ```
  #define PT_LIBARG_004b7330_void   // the source declaration `FUN_004b7330(void)` becomes the non-prototype `lua_open()`
  #define PT_LIBARG_004b7330_0 0   // a source call `FUN_004b7330(0...)` keeps its argument(s)
  #define FUN_004b7330(...) lua_open(PT_LIBARG_004b7330_##__VA_ARGS__)   // exe body (lib:lua-4.0:src/lstate.c:66); ...
  ```
  This is the same mechanism RACE-LOAD-LUA wrote by hand as `extra_pre`. It is now generated per rva from the source's
  call sites (`call_first_tokens`). The caller-side count is the source call's count. The source is the byte-matched
  caller, and it agrees with the game at 0x0047b860 `push 0` / 0x0047b862 `call 0x4b7330`. The tool refuses (sys.exit
  naming the rva, "add a rule") two shapes the paste cannot express: a lib callee whose source prototype declares
  parameters and a call passes more, and a call whose first argument does not start with an identifier or a number
  token. Neither shape occurs in any profile today. The selftest would have exited on it.
  The generic drop form is unchanged for every non-lib callee.
- `re/tools/bind/state/raceload/rules.tsv`: removed the lua_open workaround, 6 lines: the 2-line RACE-LOAD-LUA comment and
  the **4** `extra_pre 0047b860` lines (`#undef FUN_004b7330`, `RL_ARG_void`, `RL_ARG_0`, `#define FUN_004b7330(...)
  lua_open(RL_ARG_##__VA_ARGS__)`). The brief said three lines, but four exist and all four were needed only for the
  workaround. No other raceload line was touched.
- `re/tools/bind/test_porttop_bind_lib.py`: new case `voidproto` (11 cases now).
- Regenerated with the tool: `py -3.12 re/tools/bind/porttop_bind.py write --profile raceload --roots-only` (raceload is
  rebound roots-only, profile.json `selftest_roots_only`). It reported "1 files changed": `Match/PortTop_0047b860.c`.

## Regenerated-output diff (only the lua_open wrapper)

`git diff 2637e5b37 HEAD -- mashedmod/` = 1 file, `mashedmod/src/mashed_re/Match/PortTop_0047b860.c` (+3 -5):
the dropped-argument line `#define FUN_004b7330(...) lua_open()` and the 4 hand lines at the tail are replaced by the 3
generated lines above. plan.json, newfiles.txt, stops, refs and every other profile's outputs are unchanged. The selftest
confirms this: it regenerates every profile in memory and every one reports plan IDENTICAL and wrappers+stops IDENTICAL.

## Checks run

| Check | Result | File (sha256) |
|---|---|---|
| binder selftest, head 2637e5b37 (this worktree before edits) | PASS 0, all 6 profiles | voidproto_selftest_head.txt (35542de20a1cd71bab1909e05232fc89d5f71c253b28b0514ab3ef5f3fc45967) |
| binder selftest, after | PASS 0, all 6 profiles, byte-identical log | voidproto_selftest_after.txt (35542de20a1cd71bab1909e05232fc89d5f71c253b28b0514ab3ef5f3fc45967) |
| `test_porttop_bind_lib.py` | PASS 11/11 (new `voidproto`: lines exact, 2 refusals, C unit compiles/links, prints 20 = fix_open(7)+fix_open(3)) | voidproto_unittest.txt (1f65e3852958ad8a658f8764f60da052fdf3900957e39b6584aaad29f1842f18) |
| VS2022 build (`MASHED_NO_DEPLOY=1`) | Build OK, PortTop_0047b860.c compiles with no C2197 | D:/MashedWT/BIND-LIB-VOIDPROTO_data/voidproto_build_vs.log (8ebf7c3b4d1d4ea9fcd77810ae996701b8cf24323a8ce6c08bfd0a0138dcc0b2) |
| VS2022 object `PortTop_0047b860.obj` (dumpbin) | `6A 00 push 0` / `E8 call _lua_open` / `push eax` / `mov [0x006BF1E0],eax` / `call _lua_mathlibopen` / `add esp,8` / `ret`: argument kept, same shape as 0x0047b860 | (inline, build output) |
| object build (`MASHED_DDKOBJ=1`) | Build OK, link rc=0 | D:/MashedWT/BIND-LIB-VOIDPROTO_data/voidproto_build_obj.log (7bfef1e08c30bc8bc6e11df77fe90ee7e0e9acc1415cbdfdbc559e2f02a360ce) |
| `ddkobj.py check` | IDENTICAL 2442/2549, DIFFERENT 79, not-in-map 28, refusals 17, fn-addr refs ok 66 bad 0; **0x0047b860 IDENTICAL** (32 bytes, diff 0) | voidproto_ddkobj_check.txt (19f6e9b160a9bdd9770341f38858c89c573e81c809819af650d5dbd1b7cc12ce); D:/MashedWT/BIND-LIB-VOIDPROTO_data/BYTECHECK.tsv (43ec62ccd7d2f6d16d3fafaa21a06a9ffa951f9285d7e4888ef20f1630e8ce88, not committed) |

Lock: `mashed_lock.py status` and `test-slot --status` ran before each selftest and build. Before the VS2022 build the lock
was HELD by RACE-PIZ-DEVICE (pid 22832), a plain hold, not PRIORITY-HOLD or SOLO. It was free before the object build.
C: had 70 GB free.

## ddkobj classification against log\orch\run56_headcheck\ (object build of 1379467a6)

- Newly DIFFERENT units: 1, 0x0047a320 (`re/match/s12/0047a320.c`). It is **not from this row**. Its source scores
  95.2%, not 100% (source header, verify/c4rflagsweep_20261107/best_pct.tsv). It was `not-in-map` at 1379467a6 and became
  linked when RACE-LOAD-LUA-2 (b23ac4316) bound the 65 script callbacks, which is before head 2637e5b37. This row's
  mashedmod diff is only PortTop_0047b860.c.
- No longer DIFFERENT: 0.
- Pairs "listed unit -> 100% neighbour that fell back to its VS2022 body": none from this row. 0x0047b860 is IDENTICAL.
- 0x0047b860 was absent from the 1379467a6 bytecheck (bound later, RACE-LOAD-LUA), so there is no before value.
  [UNCERTAIN] whether 0x0047b860 was IDENTICAL on head 2637e5b37 too. No object build of 2637e5b37 was run here.
  Test: object build of 2637e5b37, then read its BYTECHECK row for 0x0047b860. It does not change this row's verdict:
  the row requires IDENTICAL after, and it is.

## Frida folders

Count before my first run: 12 `frida-*` names. Count after my last run: 5 (another session removed 7). New names: 0.
Deleted by this row: 0. This row ran no Frida.

## Not done / out of scope

- C++ wrappers (`.cpp` units) are not covered: `call_excess` runs only for C units (`is_c`), as before. No lib callee is
  in a C++ unit with excess arguments today.
- A lib callee with a non-void prototype and excess arguments is refused, not handled. No case exists today.
