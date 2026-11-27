# RESULT DDKOBJ-ADDR-REGISTERED (run 56, 2026-11-27, LIVING-PC)

Met: 75 of 75 units 0x00440bc0 registers (or reaches through a registered callback) are now linked and byte-checked in the object
build, 74 IDENTICAL and 1 DIFFERENT (0x0047a320, a 95.2% source, C4 by frida-replay, not by match). The 66 function addresses
0x00440bc0 passes to RegisterScriptCommand are the exe bodies' addresses in mashed_re_ddk.exe (66 ok, 0 bad). The ROLOADRWC arm
still ends at the RACE-VFS-COURSELUA stop, unchanged against a control arm on the head's tool.

Row kind: tool row (re/tools/ddkobj/ only), top-down kind. Ran: both builds, `ddkobj.py check`, `ddkobj.py selftest`, the binder
selftest, the knob boot, one RO_ROLOADRWC arm plus a head-tool control arm. No G-CTRL, no B, no 12 captures (the default path
does not run ddkobj; the change is the object build only). Branch orch/DDKOBJ-ADDR-REGISTERED from 1379467a6. No RVA bound.

## 1. Cause (both halves from the head's build json, log/orch/run56_headcheck/ddkobj_build.json)

- **60 not-in-map (50 callbacks + 10 callees).** `re/match/tlr/00440bc0.c` declares each callback as data
  (`extern undefined1 FUN_0047a320;` at line 47, then `FUN_0047b980("Course_Id",&FUN_0047a180);` at line 123). The DDK object
  therefore holds an undefined data symbol `_FUN_<rva>`. `process_unit` took the address from the name suffix before it looked at
  the wrapper (`Match/PortTop_00440bc0.c:11` `#define FUN_0047a180 PortTop_Bound_0047a180`), so the reference became
  `__ddkabs_0047a180` (the original code address). Nothing referred to `_PortTop_Bound_<rva>` and /OPT:REF dropped the DDK body.
  The 10 callees are reached only from those callbacks, so they dropped one level down.
- **15 refused `fail:wrapper-redefines-globals`.** The C-unit wrappers (BIND-CUNCLEAN-AS-C) write
  `#define MGLOBAL(T, n, a) enum { mglobal_##n }` and then one `#define DAT_x (*(T *)0x00xxxxxxu)` per name
  (`Match/PortTop_0047a1b0.c:21-23`). `exe_wrappers` accepted only the `static T &n = *(T *)(a)` form and refused every other
  MGLOBAL #define. The same refusal held 0x0047b860 and 0x0047b8d0 (17 wrappers in all).

## 2. Change (re/tools/ddkobj/ddkobj.py, commit a1b6acb77)

1. **fn-addr binding.** An undefined non-function symbol named `FUN_<8 hex>` / `thunk_FUN_<8 hex>`, not a source MGLOBAL, whose
   wrapper #define is a plain identifier, is renamed to that identifier's one decorated definition in the default link
   (`fnaddr_target`: `_name`, else exactly one `_name@N` / `@name@N` / `?name@@`). Class `fn-addr`. No definition: class
   `unbound:fn-addr`, the unit falls to `fail:unbindable-callee` (keeps its VS2022 body, not fatal). A `&FUN_x` with no wrapper
   #define stays `data-abs` as before.
2. **enum-form MGLOBAL.** `MGLOBAL_ENUM_RE` matches only the two exact enum forms. Such a unit is accepted only when every source
   MGLOBAL/MGLOBALA name has a wrapper `@abs:` #define at the source's own address (`mglobal_enum_mismatch`). Otherwise it is
   refused `fail:wrapper-redefines-globals` with the mismatching names as evidence.
3. **check.** For each fn-addr DIR32 relocation, `check` reads the linked dword and compares it with the map address of the target
   symbol (`fn_addr_refs_ok` / `fn_addr_refs_bad` per row in BYTECHECK.tsv and in ddkobj_summary.json). The relocation stays masked
   in the byte compare, since the original holds its own code address there.
4. **selftest.** Four fixtures and seven checks (fnaddr, fnaddrnx, mgenum, mgenumbad, the regex).

## 3. ddkobj check against the saved head check (log/orch/run56_headcheck/)

`compare_head.py` output: `compare_head.txt`. Summary: `ddkobj_summary.json`. Per function: `BYTECHECK.tsv`.

| count | head 1379467a6 | head-tool control (this worktree) | this branch | delta |
|---|---|---|---|---|
| IDENTICAL | 2360/2526 | 2360/2526 | 2436/2543 | +76 |
| DIFFERENT | 78 | 78 | 79 | +1 (0x0047a320) |
| not-in-map | 88 | 88 | 28 | -60 |
| units linked | 2405/2694 | 2405/2694 | 2422/2694 | +17 |
| refusals | 34 (regconv 13, wrapper-redefines-globals 21) | 34 | 17 (regconv 13, wrapper-redefines-globals 4) | -17 |
| dual body | 33 | 33 | 33 | 0 |
| provisional kept VS2022 | 255 units + 7 fns | same | same (same 7 rvas) | 0 |
| fn-addr refs | (none) | (none) | ok 66, bad 0 | |

The control is the head's ddkobj.py (`git show HEAD~1`) run on this worktree's default objects. It reproduces the saved head
numbers exactly, so every delta above is this change.

**The 75 in scope:**

| group | head | now |
|---|---|---|
| 50 callbacks (RACE-LOAD-LUA-2 section 2 c) | not-in-map 50 | IDENTICAL 49, DIFFERENT 1 (0x0047a320) |
| 10 callees (section 2 d) | not-in-map 10 | IDENTICAL 10 |
| 15 refused (section 2 b) | refused 15 | IDENTICAL 15 |

Also out of the refusal: **0x0047b860 IDENTICAL, 0x0047b8d0 IDENTICAL** (BIND-TOOLS-SMALL-3's two units, enum form). The 4
wrapper-redefines-globals refusals left are C++ cell forms, not this class: 0x00414180 (Race\SubStateMachine.cpp), 0x00404e80 /
0x00404ee0 (Save\GameSaveBuffer.cpp), 0x00404e50 / 0x00404f50 / 0x00404f80 (Save\GameSave.cpp), 0x00404e00 / 0x00404e20
(Vehicle\CarMaterialTables.cpp).

0x00440bc0 itself stays IDENTICAL (its 66 fn-addr relocations are masked, and verified by value as above). 0x0047b980 IDENTICAL.

**match.py --bytes + relocheck.py on the 75 sources** (`scope_match.py`, output `scope_match.tsv`): 74 BYTE-IDENTICAL and relocheck
OK. 1, 0x0047a320 (`re/match/s12/0047a320.c`): 44 differing unmasked bytes, first at +0x2c, relocheck BAD (`+0x4d _FUN_004b7090`).

**Newly DIFFERENT, classified:** 1.
- 0x0047a320 SetCopter, class **replay-C4** (hooks.csv:1357 `c4=frida-replay`, replay IDENTICAL-FOR-2724,
  verify/provwsprom_20261112/RESULT_PROV-WRITESET-PROMOTE.md). Source header: best 95.2%, residual (b) at the `add esi, 0x218`.
  Not in ADMITTED.tsv or PROVISIONAL.tsv. Before this row the object build linked no body for it, and the registrar passed the
  original address 0x0047a320. Now its DDK body links, as the head already does for its 48 other replay-C4 DIFFERENT units.
  Counted apart: of the 75, **74 C4 by match** (byte-identical + relocheck OK) and **1 replay-C4**. RACE-LOAD-LUA-2's "all C4 match"
  for the callbacks is 74 + 1, as L0's RULE 9 note says (76 + 1 there, with 0x0047b980 and 0x0047b8a0).
- No newly not-in-map, no unit that went from IDENTICAL to anything else ("no longer DIFFERENT: none", "new not-in-map: none").

**Listed unit -> 100% neighbour pairs that fell back to a VS2022 body:** none. The provisional list is unchanged (255 units, the same
7 functions in linked units), and each of the 75 is its own wrapper.

## 4. Other checks

| check | result | evidence |
|---|---|---|
| build (VS2022 exe + asi + object build, `MASHED_NO_DEPLOY=1 MASHED_DDKOBJ=1`) | `=== Build OK ===`, link rc 0, mashed_re.exe e927414c, mashed_re_ddk.exe 995ee21d | D:/MashedWT/DDKOBJ-ADDR-REGISTERED_data/build.log |
| `ddkobj.py selftest` | OK, 0 failures, 44 PASS (7 new) | ddkobj_selftest.txt |
| binder selftest (`porttop_bind.py selftest`) | PASS, 0 failures, plans IDENTICAL | binder_selftest.txt |
| knob boot (VS2022 exe, `MASHED_ORIG_TOP=1 MASHED_DETERMINISTIC=1 MASHED_DET_FRAMES=600`) | rc 0, `WinMain 0x00492370 returned 0 after 600 frames` | D:/MashedWT/DDKOBJ-ADDR-REGISTERED_data/kb_vs.log |
| RO_ROLOADRWC-ar1 (object build) | exit 0xC0000005 at 20 s, AV `_Orig_0040d270+0x5c` read 0. **Setup error, not a finding:** my root had no `original/mashed_image.bin` (`[portstate-image] REFUSED(missing)`, log line 2), so 0x005F2728 (original .data value 0x005F2570) read 0 at the `**(param_1+param_2*4)` load (original 0x0040d2cc, `re/match/top07/0040d270.c:6-7`). Re-run as ar2 | RO_ROLOADRWC-ar1.log |
| RO_ROLOADRWC-ar2 (object build, image from `re/tools/image/extract_image.py`, sha 81cc7775 = RACE-LOAD-LUA-2_root's copy) | exit 0xC0000005 at 76 s: `Searching piz for COURSE.LUA`, AV `RwCore_005509b0+0x4` read 0x38, `[esp+0]` = `PortTop_Bound_0047b8d0+0x4c` in **ddk_s13_0047b8d0** (the DDK body, a refused unit at the head), `[esp+14]` = 0x005CF010 | RO_ROLOADRWC-ar2.log lines 464-467 |
| RO_ROLOADRWC-ctrl1 (head-tool object build 42ab1812, same root) | exit 0xC0000005 at 13 s, same stop: AV `RwCore_005509b0+0x4` read 0x38, `[esp+0]` = `PortTop_Bound_0047b8d0+0x4f` in **PortTop_0047b8d0.obj** (VS2022 body), `[esp+14]` = 0x005CF010 | RO_ROLOADRWC-ctrl1.log lines 464-467 |

The stop is RACE-LOAD-LUA-2's RACE-VFS-COURSELUA stop (VFS_Open returns 0 for COURSE.LUA, RW core FS install), unchanged by this row.
The arm does not reach lua_dobuffer, so no registered callback runs. The registrar's call is not observed at run time: the value
check is on the linked image only. Register line updated (re/orchestrator/UNCERTAIN_REGISTER.md, RACE-LOAD-LUA-2 line: settled at
link level, run-time call still open).

The ticks.bin / checkpoint data of ar2 and ctrl1 differ from byte 256 on: the two exes differ in code addresses, and the image
holds code pointers (OT_CODEPTR binds 188 + 107 slots to exe bodies). Not compared further. [UNCERTAIN] whether any difference is
outside code-pointer slots; test: diff the two ckpt00.data.bin with the OT_CODEPTR slot list masked.

## 5. Not done

- No arm past the VFS stop (needs RACE-VFS-COURSELUA). The run-time fn argument of PortTop_Bound_0047b980 is not watched.
- The 4 C++ cell-form refusals (section 3) are out of scope.
- The control build `mashedmod/build/ddkobj_headctrl` (50 MB) stays in the worktree for the lead; delete with the worktree.
- frida-<hash> folders in %LOCALAPPDATA%\Temp: 698 before, 3 after. This run used no Frida and made none (the drop is someone
  else's cleanup).

## 6. Raw files (D:/MashedWT/DDKOBJ-ADDR-REGISTERED_data)

| file | sha256 |
|---|---|
| build.log | 280c7bfda1ebe5e3a175bc3b2f4b76041788a56e82569b9e75c8e0733e8e7349 |
| headctrl_build.log | 36348fdf6a12500416e2ad5426422e1a6462113cbe9ba0c779bfc4d80cfd68e8 |
| ddkobj_build.json | 552e5368325574a644e5738393b0de47f0160f468b7ef62f07484ee061413165 |
| BYTECHECK.tsv (also committed here) | f021c50559a28fac0cc74abcc701ff7e396ef95f18f8f1788adbb85c0421d491 |
| kb_vs.log | 75b850762ca72fb81bfaaa42a6221be6b6e9780d74af48c5d81cbd8a4db87b94 |
| RO_ROLOADRWC-ar1.log | f91453ef89fef83315e4106534f8b4224c3cc84905fc2a32adacaa395dfc35a2 |
| RO_ROLOADRWC-ar2.log | db0873c34ead4b9cd6770b58dadcc20cf9bfd8997d7dbcd11dd4f2117ef65775 |
| RO_ROLOADRWC-ar2/ticks.bin | 919be8a8d99ea5d68497b2d3f24c7896f5980dc39798c97be9bf83c5e35b7339 |
| RO_ROLOADRWC-ctrl1.log | e143b2ded81f940c84f57ea85953493258b9e5afe27fed7b9df561c28f8de3d4 |

Exes: mashed_re_ddk.exe 995ee21d6c05231f1d3871f603043a54029e85475b0c07296a7569efb5fd723b, head-tool control
42ab1812690e4077d4e155c12a8c9067a644bf86e1ca68bd52b218a6a702ca6b, mashed_re.exe e927414cbeca0cf386257b43cb88db94387bb4497733dc7aa54ea4ab1d2a8a14.

Commands (worktree root): `cmd /c "set MASHED_NO_DEPLOY=1&& set MASHED_DDKOBJ=1&& mashedmod\build.bat"`;
`py -3.12 re/tools/ddkobj/ddkobj.py selftest`; `py -3.12 re/tools/bind/porttop_bind.py selftest`;
`py -3.12 verify/ddkobj_addrreg_20261127/compare_head.py mashedmod/build/ddkobj`;
`py -3.12 verify/ddkobj_addrreg_20261127/scope_match.py mashedmod/build/ddkobj`;
arms: `A4_OUT=D:/MashedWT/DDKOBJ-ADDR-REGISTERED_data A4_ROOT=D:/MashedWT/DDKOBJ-ADDR-REGISTERED_root A4_EXE=<wt>/mashedmod/build/ddkobj/mashed_re_ddk.exe A4_RO_CKPT=0,1,17,18,299,301,600 test-slot bash -c 'py -3.12 <orch-wt>/re/orchestrator/mashed_lock.py run --label DDKOBJ-ADDR-REGISTERED -- py -3.12 verify/savelink_20261023/run_arms.py RO_ROLOADRWC-ar2'`
(ctrl1: A4_EXE = `mashedmod/build/ddkobj_headctrl/mashed_re_ddk.exe`).
