# RESULT BIND-LIB-ROUTE (run 55, section LIB, binder tool row)

Verdict: GREEN. The parent condition (merge drain 1, re-run the selftest) is done, see the section "Merge with drain 1"
at the end. The binder has a `lib` binding kind next to `crt`. No RVA is bound by this row, and every generated
output is unchanged (shown below).

Base: `orch/integration` @ 39ee297f2. Worker: claude-opus-5-5 medium on claude2.

## What changed

- `re/tools/bind/porttop_bind.py`
  - Two new rule kinds, read from `state/_shared/rules.tsv` and from each `state/<profile>/rules.tsv` (a profile line
    wins over a shared one):
    - `lib_root <lib> <root> <globs>`: library name, its tree relative to the worktree root, and the source globs
      (space-separated, relative to `<root>`) that define its symbols. Name and root are kept as written (new
      `NO_RVA_KINDS`, so they are not lower-cased like rva fields).
    - `lib <rva> <lib> <symbol>`: callers of `<rva>` bind the library's own `<symbol>`.
  - `resolve_libs()` / `lib_symbol_defs()`. An rva resolves only when its library has a `lib_root` line, the root
    exists, the symbol is a C identifier, and the symbol has **exactly one non-static definition** in the globs. A
    static definition is refused, with `static` on the same line or on the line before. Two definitions are refused,
    and so is zero, which covers a macro such as Lua's `lua_pop` / `lua_register`. Any error stops the binder
    (`Binder.__init__`, `sys.exit` with every error line).
  - Planner (`build_plan`): right after `stop_force`, a lib rva is planned `('reuse', <symbol>, 'lib:<lib>:<file>:<line>')`.
    It wins over `bound_wins`, `stop_reason`, cpp-unclean, alias, CRT and matched-source rules: a library rva never gets
    a PortTop wrapper, a matched-source bind or a named stop. Only an explicit `stop_force` beats it.
  - Wrappers: no new code path. A caller's wrapper writes the existing reuse line
    `#define FUN_<rva> <symbol>   // exe body (lib:<lib>:<file>:<line>)`. The wrapper includes the matched source inside
    `extern "C"` and a C library defines the symbol with C linkage, so the call links by name.
  - `bindings`: a lib reuse is written to BINDINGS.tsv with kind `library` (note: `vendored library symbol <sym>
    (lib:...)`), so it is not counted as `matched`. That keeps L-97 / L-177: library code is outside the port count,
    like the `crt` rows.
  - `selftest`: a new first line checks that every lib rule, shared and per profile, resolves. Today:
    `0 lib roots, 0 ... resolve, 0 fail`.
- `re/tools/bind/state/_shared/rules.tsv`: two comment lines that document the kinds (no rule added).
- `re/tools/bind/test_porttop_bind_lib.py` (new): the unit test of the kind.
- `re/tools/bind/fixtures/libfix/` (fixture library: `include/libfix.h`, `src/fix_api.c`, `src/fix_more.c`, written in
  the shapes Lua 4.0's `lapi.c` uses) and `re/tools/bind/fixtures/libfix_caller/00400200.c` (a caller written like a
  matched source). The rvas `0x00400100..0x00400200` are fixture names only, not game rvas.

## How a later binding row uses the lib kind

This row ships no profile data. The binding row (RACE-LOAD-LUA, after LUA-VENDOR lands `mashedmod/deps/lua-4.0`) adds
data only (rule 1).

1. One `lib_root` line per library, in `state/_shared/rules.tsv` because a library function has one exe body for every
   profile. For the Lua tree as `tools/_download/lua/src_4.0/lua` lays it out (`src/*.c` core, `src/lib/*.c` the
   standard libraries; LUA-VENDOR decides the final layout and the globs follow it):
   ```
   lib_root	lua-4.0	mashedmod/deps/lua-4.0	src/*.c src/lib/*.c
   ```
2. One `lib` line per API rva the matched callers reach. The symbol name comes from the libid evidence for that rva
   (hooks.csv note: `libid identical lapi.obj _lua_typename`, so the symbol is `lua_typename`, with the leading
   underscore of the decorated name dropped). Example:
   ```
   lib	004b6fc0	lua-4.0	lua_gettop
   ```
   The other rows follow the same form, one rva per line. Survey list (`log/orch/lua_survey_20261121.md`):
   `0x004b7330`, `0x004c0510`, `0x004b7480`, `0x004b7a70`, `0x004b7200`, `0x004b7250`, `0x004b6fc0`, `0x004b7090`.
   **[UNCERTAIN]**: this row did not check which symbol each rva maps to (only `lua_gettop` / `lua_tonumber` are named
   in the survey). The binding row takes every name from the rva's hooks.csv libid note, and the binder refuses a name
   the library does not define.
3. Run the usual checkpoint: `plan` / `write` for the profile, then `selftest`. The lib rvas show up as `reuse` in the
   listing and plan.json, and caller wrappers get `#define FUN_<rva> <symbol>`. The library objects must already be
   linked by LUA-VENDOR's build step (both builds). This tool does not add the library to `exe_sources.rsp`.
4. Later libraries (libpng, zlib, qhull) need only their own `lib_root` line and their `lib` lines. Symbol shapes such
   as `PNGAPI` / `ZEXPORT` before the name pass the same definition test, because any return-type prefix on the same
   line or on the line before is accepted.

Regex check against the real Lua 4.0 tree (read-only, gitignored `tools/_download/lua/src_4.0/lua`, not committed, not
a binding). `lua_gettop` resolves to `src/lapi.c:68`, `lua_tonumber` to `lapi.c:157`, `lua_open` to `lstate.c:66`,
`lua_dofile` to `ldo.c:287`, `lua_call` to `ldo.c:215` and `luaC_checkGC` to `lgc.c:349`, and 6 more resolve. The macros
`lua_pop` and `lua_register` are refused with "defined 0 times", which is correct.

## Checks run

| Check | Result |
|---|---|
| `mashed_lock.py status` before selftest, VS2022 build, object build, ddkobj check | free / free / HELD by a game run (`KNOBON-BOOTS-G5`, not PRIORITY-HOLD or SOLO) / free |
| Unit test `py -3.12 re/tools/bind/test_porttop_bind_lib.py` | **9/9 PASS** (`unittest.txt`): rules, resolve, static, dup, missing, plan, wrapper, bindings, link (cl VS2022 `/EHa /W3 /O2 /DMASHED_STANDALONE`: wrapper + fixture library link, `t.exe` prints 14 = fix_add(3,2) + fix_mul(3,3)) |
| Binder selftest (branch tool) | **FAIL 14**, the head's own 14, all audio (`selftest_branch.txt`) |
| Binder selftest, head tool (39ee297f2 `porttop_bind.py`) on the same tree | FAIL 14 (`selftest_head_tool.txt`). `diff` head vs branch output: the only difference is the added `[selftest] lib kind: ...` line, so **0 new failures** |
| Generated outputs, `plan --listing` per profile, head tool vs branch tool | raceload (`--roots-only`) 412 lines, audio 544, bulk 490, raceframe 534, renderframe 262: **all IDENTICAL** |
| Generated files in the tree after selftest and plan runs | `git status`: no file under `mashedmod/`, `verify/porttop_20261108/BINDINGS.tsv` or `re/tools/bind/state/*/` (plan.json, newfiles.txt, ...) changed. The only diffs are the tool, the two rules.tsv comment lines, the test and the fixtures |
| VS2022 build `set MASHED_NO_DEPLOY=1&& mashedmod\build.bat` | Build OK, `mashed_re.exe` sha256 `54389ceb...ff2064d` (`exe_sha_vs2022.txt`) |
| Object build `set MASHED_DDKOBJ=1` | Build OK |
| `py.exe -3.12 re/tools/ddkobj/ddkobj.py check` | IDENTICAL 2336/2435, DIFFERENT 72, not-in-map 27, units linked 2322/2595, refusals 19 (`check.txt`, `ddkobj_summary.json`) |
| vs `log/orch/run55_headcheck` (object build of 39ee297f2) | BYTECHECK.tsv **byte-identical** (sha256 `5c4196c5...b76f` both), `ddkobj_summary.json` equal. **0 newly DIFFERENT units, 0 pairs that fell back to a VS2022 body** |

Head FAIL 14, line by line (identical on the branch): audio `plan DIFFERENT` (1). The 14 plan rows under it: `005a6ea0`,
`005a9f10` provisional -> replay. `005a71f0`, `005a7b60`, `005a91b0`, `005ae250`, `005ba1d0`, `005bb000`, `005bb910`,
`005bba30`, `005bba60`, `005bbac0`, `005c7d30` stop -> reuse PortTop_Bound. `005b10a0` stop -> reuse
AudioTableDispatchAt4. Then `wrappers+stops DIFFERENT 13` (13): `Match/PortTop_0045d460.cpp`, `005a8f30`, `005a9e60`,
`005b10d0`, `005b1140`, `005b9f30`, `005ba780`, `005bb850`, `005bc860`, `005be0f0`, `005c7a90`, `005c7fb0`,
`PortTop_AudioStops.cpp`. Total 1 + 13 = 14. BIND-AUDIO-ONE-GENERATOR owns them.

Not run, by row kind: G-CTRL, B, 12x4 captures, knob boot (the row binds nothing and changes no exe source).

## Raw files (not committed)

| file | sha256 |
|---|---|
| `D:/MashedWT/BIND-LIB-ROUTE_data/build_vs2022.log` | `0688617c0080e97634afcad30cdefb1a3e64f644fea390e1bd3d137156d3b41a` |
| `D:/MashedWT/BIND-LIB-ROUTE_data/build_ddkobj.log` | `37bd5947063bfe2796c15c3cd553603b6b3c3ab51fe80d8cb42d5b2e58a284e9` |
| `mashedmod/build/ddkobj/BYTECHECK.tsv` (worktree build dir) | `5c4196c5fd95bbf6e80329405aea65b30ba2dc2830ede05955bb4cd3979be76f` |
| `mashedmod/build/ddkobj/mashed_re_ddk.exe` | `74292d14efa0071739fc065e2f6375f91ebf3bcec980e959558106afb5a147de` |

## Unfinished scope (deliberately outside this row)

- No lib line binds a real rva (RACE-LOAD-LUA).
- The library build and link step for both builds (LUA-VENDOR).
- The rva -> symbol table for the Lua API surface. It comes from the hooks.csv libid notes in the binding row (see the
  [UNCERTAIN] line above).
- [UNCERTAIN] line for the register, test in one line: "RACE-LOAD-LUA: for each lua `lib` line, the symbol equals the
  hooks.csv libid note's symbol of that rva, and `porttop_bind.py plan` resolves it against mashedmod/deps/lua-4.0".

## Merge with drain 1 (parent condition, run 55)

- Done on claude3 (moved there because git merge prompts on claude2): `git -c merge.autoStash=false merge orch/integration`,
  with integration at a2843847a (drain 1: HB-LOCK-ENFORCE, LUA-VENDOR, KNOBON-BOOTS-G5). Merge commit 9f939b788.
- One conflict, in `re/tools/bind/porttop_bind.py` `cmd_selftest`, resolved by keeping both. The four HB-LOCK-ENFORCE lines
  come first (`sys.path.insert`, `import mashed_lock`, `mashed_lock.wait_clear(label='porttop_bind selftest')`), then
  `bad = selftest_libs()` in place of its `bad = 0`. The resolution touched nothing else in the file.
- After the merge, `git diff orch/integration HEAD -- mashedmod` is empty: the exe sources are the new head's.

| Check on the merged tree | Result |
|---|---|
| `mashed_lock.py status` before each of: selftest, VS2022 build, object build, ddkobj check | free every time |
| Unit test | **9/9 PASS** (`unittest_merged.txt`) |
| Binder selftest, branch tool | **FAIL 35** (`selftest_merged.txt`) |
| Binder selftest, new head tool (orch/integration a2843847a `porttop_bind.py`) on the same tree | **FAIL 35** (`selftest_merged_head_tool.txt`). `diff` shows only the added lib line: **0 new failures**, and the remaining 35 are the new head's own |
| VS2022 build (re-run, because the merge changed `mashedmod/build.bat` and added `mashedmod/deps/lua-4.0`) | Build OK, `mashed_re.exe` sha256 `926fa459...6436d5` |
| Object build `MASHED_DDKOBJ=1` | Build OK |
| `ddkobj.py check` | IDENTICAL 2336/2435, DIFFERENT 72, not-in-map 27 (`check_merged.txt`, `ddkobj_summary_merged.json`). BYTECHECK.tsv is **byte-identical** to `log/orch/run55_headcheck` (sha256 `5c4196c5...b76f`) and the summary is equal: **0 newly DIFFERENT units, 0 fallbacks to a VS2022 body** |

The new head is at FAIL 35, up from 14. Drain 1 caused the 21 new lines, not this row: the head tool gives the same 35
lines on a tree whose `mashedmod/` equals orch/integration. Line by line, from the head output (identical on the branch):
- **audio (14, unchanged):** `plan DIFFERENT` (1) plus the 13 `file` lines listed under "Checks run" above.
- **bulk (3, new on the head):**
  - `plan DIFFERENT` (1): `plan 00420420 [stop] -> [stop, outside the bulk cp1 list ... re/match/s05/00420420.c]`.
  - `file Match/PortTop_BulkStops.cpp` (1).
  - `moved, not named: 00420420`, giving "60 lines moved, FAIL 1 unnamed" (1).
  - Cause: 0x00420420 is now C4 in hooks.csv, with a re/match source.
- **raceframe (18, new on the head):**
  - `plan DIFFERENT` (1): bind 256 -> 269, reuse 148 -> 154. Plan lines 0040ce80, 0041f3d0, 00420290, 00420870,
    00422440, 00422470, 00422aa0, 004260a0, 004260b0, 00426dc0, 00431d70, 0046b540, 0046bce0, 0046bd20, 0046bd60,
    0046bda0, 00479100, 004c5a60, 00540080, 00540100, 00540160.
  - 17 `file` lines: Match/PortTop_0040da50.cpp, 0040e590, 004111c0, 0041f3d0, 00420870, 00422440, 00422470,
    00426dc0, 0046b540, 0046bce0, 0046bd20, 0046bd60, 0046bda0, 00540080, 00540100, 00540160, and
    PortTop_RaceFrameStops.cpp.
  - Cause: new c4=match and c4=frida-replay rows on the head that the raceframe closure now reaches.
- Total 14 + 3 + 18 = 35. This row owns none of these profiles and regenerated nothing. Taking bulk and raceframe back
  to 0 is a regenerate step for whoever owns those profiles.

### Lua: a lib_root at the vendored tree (check only, no rule committed)

`lib_root lua-4.0 mashedmod/deps/lua-4.0 src/*.c src/lib/*.c` resolves all 22 Lua API symbols tried, each to exactly one
definition (`lua_libroot_check.txt`):
- `lua_open` lstate.c:66, `lua_close` lstate.c:100
- `lua_mathlibopen` lib/lmathlib.c:231, `lua_baselibopen`, `lua_strlibopen`, `lua_iolibopen`, `lua_dblibopen`
- `lua_dobuffer` ldo.c:304, `lua_dostring`, `lua_dofile`, `lua_call`
- `lua_setglobal` lapi.c:315, `lua_getglobal`, `lua_pushcclosure` lapi.c:230, `lua_pushnumber`, `lua_pushstring`
- `lua_gettop` lapi.c:68, `lua_settop`, `lua_tonumber` lapi.c:157, `lua_tostring`, `lua_rawcall`
- `luaC_checkGC` lgc.c:349

The macros `lua_register` and `lua_pop` are refused, which is correct.

Of the 8 survey rvas, hooks.csv names only two, and both resolve: `0x004b6fc0` lua_gettop and `0x004b7090` lua_tonumber.
The other six carry Ghidra names and mechanical notes only, with no `libid identical <obj> _<sym>` note: `0x004b7330`,
`0x004c0510`, `0x004b7480`, `0x004b7a70`, `0x004b7200` (thunk to `0x004b7fd0`) and `0x004b7250`. Their rva -> symbol
pairing is **[UNCERTAIN]** (register line updated). The binding row names them from a libid run against the vendored
objects, not from reading code.

Extra check: CRLF copies of the fixture sources (core.autocrlf checkouts) resolve the same way.

### Raw files of the merged run (not committed)

| file | sha256 |
|---|---|
| `D:/MashedWT/BIND-LIB-ROUTE_data/build_vs2022_merged.log` | `131fdecfae1926c41fdf8d80f70723c1f491ecce3f817918bea2b5fb58a47590` |
| `D:/MashedWT/BIND-LIB-ROUTE_data/build_ddkobj_merged.log` | `2ce089656806a0751c1b4f3a7def2d589578ded66927f7194c8e066b28d43cf0` |
| `mashedmod/build/ddkobj/mashed_re_ddk.exe` (merged) | `c64d0979ab4f1264bc94e9dd59ab537f6ac18e1328c537015abcc821982c04f1` |
