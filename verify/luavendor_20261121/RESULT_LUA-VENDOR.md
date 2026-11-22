# LUA-VENDOR (run 55, section LIB, Lua lane row 1) - 2026-11-21

Build row, default-path kind for the wiring. Branch `orch/LUA-VENDOR` from `orch/integration` 39ee297f2.
Reference image: `original\MASHED.exe.unpatched` (anchor SHA-256 BDCAE093..., checked by `coffpe.Image` on load).

## Verdict

GREEN. The vendored tree is **stock Lua 4.0**, copied to `mashedmod/deps/lua-4.0` and built by
`mashedmod/deps/lua-4.0/build_lua.bat` into one static library per build. The DDK library reproduces all 95 identities
the recorded flags give on 4.0 (94 unambiguous, the same 94 rows that are C4 libid today, plus 1 ambiguous). Both
exes are byte-identical to the same link without the library (timestamps masked). The object build's
`ddkobj_summary.json` and `BYTECHECK.tsv` have the same SHA-256 as the head check's. The stop rule did not fire,
because both trees reproduce the libid-identical functions.

## 1. 4.0 against 4.0.1: per-function identity counts (recorded flags)

`re/tools/match/sweep_flags.py lua|lua401 --search --per-obj --flags "/Og /Oi /Ot /Oy /Ob1 /GF /G5"`, XP DDK cl 13.00.9176 /
c2 13.00.9178 (@comp.id 28/9178 on every object), 302 hooks.csv rows `third-party-library[lua-4.0]` (294 at LIB-IDENTITY,
rows were added since; that run's 87 is today's 95 on the same flags).

| tree | identical | of which ambiguous | differs | not compared |
|---|---|---|---|---|
| Lua 4.0 (lua-4.0.tar.gz sha256 b476abc7...0e71337) | **95** | 1 | 193 | 14 |
| Lua 4.0.1 (lua-4.0.1.tar.gz sha256 df746e14...484d2a) | **96** | 1 | 193 | 13 |

Per object they are the same except `lgc` (4.0: 0, 4.0.1: 1). The 4.0 identical set is a strict subset of 4.0.1's, and
the one extra is `luaC_checkGC` at `0x004ba570` (not compared on 4.0, identical on 4.0.1). No function is identical
only on 4.0. Tables: `identity_lua40_recflags.tsv`, `identity_lua401_recflags.tsv`.

## 2. Why the pick is 4.0 and not the count winner

The 9 source differences between the trees (`diff -ru`, lua.h, lapi.c, ldo.c, lgc.c/.h, lbaselib.c, lparser.c,
lstring.c, lvm.c) were checked one by one against the game's bytes, and against both trees' objects compiled with the
recorded flags (`obj_discriminators.txt`, `game_disasm.txt`, `discriminators.txt`):

| change 4.0 -> 4.0.1 | 4.0.1 object | 4.0 object | game |
|---|---|---|---|
| `LUA_VERSION` "Lua 4.0" -> "Lua 4.0.1" | | | `$Lua: Lua 4.0 Copyright ...` at `0x005d8790`: **4.0** |
| lvm.c `traceexec`: `luaD_lineHook(L, base-2, ...)` -> `base-1` | `add ebx, -0x10` | `add ebx, -0x20` | `add ebx, -0x20` at `0x004b93dc`: **4.0** |
| lparser.c `retstat`: `&& ls->t.token != ';'` added | has `cmp edx, 0x3b` | no `0x3b` compare | `0x004be360`..`0x004be3a5`: no `0x3b` compare: **4.0** |
| lapi.c `lua_newuserdata`: `(size==0) ? 1 : size` | extra `test eax, eax` | none | `0x004b7180`: size from `[esp+0xc]` pushed unconditionally to `0x004b9950`: **4.0** |
| ldo.c `protectedparser`: `luaC_checkGC(L)` -> inline test + `luaC_collectgarbage` | | | `call 0x4ba570` (luaC_checkGC) at `0x004b79da`: **4.0** |
| lgc.c `luaC_collectgarbage` static -> extern | `luaC_checkGC` identical | differs | `0x004ba570` identical to **4.0.1** only |
| lstring.c `luaS_newudata`, ldo.c `lua_dofile`, lbaselib.c | | | not separable (best-pair ratios equal or low, `discriminators.txt`). lbaselib is not linked by the game: none of its strings (`dofile`, `dostring`, `rawget`, `pre-compiled`) is in the image |

Four semantic changes and the version string follow 4.0. A 4.0.1 library would compute differently from the game in
`traceexec` (line-hook stack slot), `retstat` (`return;` parsing) and `lua_newuserdata(L, 0)`, and the replay oracle
links this library. The only 4.0.1 witness is the linkage of one function, with no semantic effect.

[UNCERTAIN] Why `luaC_checkGC 0x004ba570` matches only the extern form: the game re-stores the argument at `0x004ba57e`
before the tail jump at `0x004ba582`, and stock 4.0's static form compiled by 9178 tail-jumps without that store
(LIB-IDENTITY finding 1). Missing evidence: a cl 13.00.9466 back end (the game's Rich header build), or a local lgc.c
edit. Test: compile stock 4.0 lgc.c with 9466 and libmatch it at `0x004ba570`. Register line added to
`re/orchestrator/UNCERTAIN_REGISTER.md`. **The vendored tree is not edited for it.** Stock 4.0 leaves this one function
DIFFERENT. That costs one non-C4 row (`0x004ba570` is C2 today) and no C4 row.

## 3. What is vendored

`mashedmod/deps/lua-4.0/` = a fresh extraction of `tools/_download/lua/lua-4.0.tar.gz` (sha256 above; `diff -r` against
the existing `src_4.0/lua` gives no difference): `COPYRIGHT`, `README`, `include/*.h`, `src/*.c *.h` without `ltests.c`,
`src/lib/*.c`. 49 files, 382 KB. The `lua`/`luac` drivers, `test/`, `doc/` and `etc/` are not vendored. No file is edited.
`.gitignore`: `obj/`, `obj_ddk/`, `*.lib`.

Compiled into both libraries: the 17 core files plus `src/lib/lauxlib.c` and `src/lib/lmathlib.c`. Those are the two
standard libraries the image carries strings of: `randomseed` at file offset 0x217e74, `atan2` at 0x1d51cc, and
`_ERRORMESSAGE` at 0x217324. lbaselib, liolib, lstrlib and ldblib stay in the tree, stock, but are not compiled. None
of their strings is in the image (`dofile`, `readfrom`, `writeto`, `strsub`, `gsub`, `tinsert`, `setlinehook`), and the
DDK compile of liolib references the single-threaded CRT data `_errno`, which the VS2022 CRT does not define.

| library | compiler and flags | used by |
|---|---|---|
| `lua_4_0.lib` | VS2022 cl, `/O2 /arch:IA32 /fp:precise` (the qhull convention) | `mashedmod\build.bat` exe link line (`%LUA_LIB%`), built by `build_lua.bat vs` when absent |
| `lua_4_0_ddk.lib` | XP DDK cl 13.00.9176 / c2 13.00.9178, `/W0 /Og /Oi /Ot /Oy /Ob1 /GF /G5`, `INCLUDE=tools\vc7ddk\inc\crt` (the `period_cc.py` environment) | `re\tools\ddkobj\ddkobj.py do_link` (object build), built by `build_lua.bat ddk` in the `MASHED_DDKOBJ=1` branch when absent |

`tools\vc7ddk` is found at `MASHED_VC7DDK`, then `<repo>\tools\vc7ddk`, then the main checkout from a worktree. With
none of them, `build_lua.bat` (both) skips the DDK library with a note, so the LAN git-archive build still builds.
The DDK link also gets `legacy_stdio_definitions.lib`, because the DDK objects reference `_sprintf` / `_vsprintf`,
which the UCRT only defines inline. `ddkobj.py default_link_symbols()` lists `lua_4_0.lib`, the library now on the
default link line.

The asi target does not link the library, since MASHED.exe carries its own Lua.

## 4. Byte check of the DDK library (every function where the DDK build gives identity)

`re/tools/match/libmatch.py --objs mashedmod/deps/lua-4.0/lua_4_0_ddk.lib --subsystem "third-party-library[lua-4.0]"`:
rows 302, **identical 95 (ambiguous 1)**, differs 181, not compared 26, obj-functions 381. Its identical set is the
same set as the 4.0 sweep in section 1, RVA for RVA. All 94 rows at C4 today (route libid) are identical. The one
identical row that is not C4 is `0x004c0560` (C1, `identical-ambiguous`, `_math_mod` / `_math_pow` share the bytes),
which stays refused by the libid rule. No promotion file: every unambiguous identity is already C4. Table:
`bytecheck_lua_4_0_ddk_lib.tsv`.

## 5. Checks run

| check | result |
|---|---|
| mashed_lock status before the sweeps, the build and each relink | free each time |
| identity sweep 4.0 / 4.0.1, recorded flags | 95 / 96, section 1 |
| discriminator objects for both trees + game disassembly | section 2 |
| `build_lua.bat` (both libraries) | `lua_4_0.lib OK`, `lua_4_0_ddk.lib OK` |
| link + run smoke test (`luatest.c`, `run.bat`: lua_open, lua_mathlibopen, lua_register, lua_dobuffer, lua_gettop, lua_tonumber, lua_getglobal, lua_close) against each library with the VS2022 CRT | both link rc 0 and print `dobuffer=0 acc=15.217059868831708 x=2.3420598688317069 top=1` |
| libmatch byte check of `lua_4_0_ddk.lib` | 95 identical, section 4 |
| `cmd /c "set MASHED_NO_DEPLOY=1&& set MASHED_DDKOBJ=1&& mashedmod\build.bat"` (rva-body lint, asm lint, seed check, exe, object build, asi) | exit 0 |
| `ddkobj.py check` (in the build) | IDENTICAL 2336 / DIFFERENT 72 / not-in-map 27, units linked 2322/2595, refusals 19 |
| against `log\orch\run55_headcheck\` | `ddkobj_summary.json` sha256 a278a5c8... and `BYTECHECK.tsv` sha256 5c4196c5... equal the head's byte for byte. **No newly DIFFERENT unit**, and no listed unit -> 100% neighbour pair fell back to its VS2022 body |
| `mashed_re.map`, `mashed_re_ddk.map`: a `lua_4_0` member linked? | 0 lines in each |
| default exe relinked without `%LUA_LIB%` (`relink_nolua.bat`), `pe_compare.py` | SAME: 0 differing bytes after masking TimeDateStamp, CheckSum and the debug-directory stamps, identical section sizes |
| object-build exe relinked without the two added libraries, `pe_compare.py` | SAME, 0 differing bytes |
| default gates (G-CTRL, B) | **not run**: the default exe links byte-identical to the link without the library, so the brief's condition ("only if the default exe links differently") does not hold |

## 6. Not done, by scope

- No RVA bound, no binder or BINDINGS edit (BIND-LIB-KIND and RACE-LOAD-LUA own that). No tracker edit, no promotion.
- `0x004ba570` stays DIFFERENT in the object build's Lua library (section 2, register line).
- The asi target does not link the library (section 3).

## Files

Committed in this folder: this RESULT, `identity_lua40_recflags.tsv`, `identity_lua401_recflags.tsv`,
`bytecheck_lua_4_0_ddk_lib.tsv`, `discriminators.txt`, `obj_discriminators.txt`, `game_disasm.txt`,
`ddkobj_summary.json`, scripts `lua_dis.py`, `lua_disc.py`, `luatest.c`, `run.bat`, `relink_nolua.bat`, `pe_compare.py`.

Raw files (`D:/MashedWT/LUA-VENDOR_data`), SHA-256:

| file | sha256 |
|---|---|
| sweep/lua40.log | 36a8b6d27cf1a022560eb776ef943c58c4ce9583f145ed8313590fdb0eb373bf |
| sweep/lua401.log | 8a860f9af0051dc279615b3e3996f6d2226f53fc392462860a0eaabdbeb767d6 |
| sweep/lua_Og_Oi_Ot_Oy_Ob1_GF_G5.tsv | 8bfdf8c6e6c65d3705afe513ce83be543fdc5fefb8dab1cec41c96aff1f8d7fc |
| sweep/lua401_Og_Oi_Ot_Oy_Ob1_GF_G5.tsv | 4c4ebeee062cd7d59d8c76a6526316b60cbe6d53ad3e7f207fc6a6071f89f022 |
| bytecheck/lua_4_0_ddk_lib.tsv | 633228fe7f269865a6fb47f4d5a5647aec1c5a81e60a0f2c7d85d3c68d45bf96 |
| bytecheck/ddk_lib.log | 4f8f90c3f6b0d864a748bf6f620c7c58703a0e3d57bc77d9f29ed0fd8b394afb (the second byte check, after the library set was cut to core + lauxlib + lmathlib) |
| build/build.log | 0f99b7523153c5434b6cf46422dc1f4b603d772d20499274187a9efc0e8598dd |
| nolua/mashed_re.exe | 2a568f37c20b630da60f30a6460c59700c6544d50e08c206385dbeb7240af590 |
| nolua/mashed_re_ddk.exe | 50ac6bde2c8168d08a5c96a688e69e0ab4a70cbdcda540752092044a6146413d |
| worktree build/mashed_re.exe | e50abe33df29eb94c68dcbe8d952c17065103341f9bec53e5c4198b6c65ab26c |
| worktree build/ddkobj/mashed_re_ddk.exe | 3645303846a0f250790cc3ce08087acf51e064c7a96ad40895048c3484253c5e |
| lua_4_0.lib / lua_4_0_ddk.lib | 781a24e81991a35b6bdc0b279e229e6fcf0b494d0dd0ebdb76a8a7f0474666de / aaa7eba013f7b20c37676e32f78256fc3332d8f466d0a941be136aeb50407091 |
