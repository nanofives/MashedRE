# RESULT DDKOBJ-TRAIN (2026-11-11)

Row: run the object build and `ddkobj.py check` in the merge train on every pass that builds, as a reported step,
and make two classes fail the object build (RESULT_DDKOBJ-SPIKE section 10 items 4 and 9). Branch `orch/DDKOBJ-TRAIN`
off `orch/integration@778b8b91d`. Row kind: tool and train. No change to the default exe's inputs (build.bat edits
are REM lines and one `if errorlevel` inside the opt-in `MASHED_DDKOBJ` block).

## Verdict

**GREEN.** The train now builds with `MASHED_DDKOBJ=1` on every pass that builds, and reports the object build as
its own step. Both new failure classes stop the object build before the link (exit 3), and `build.bat` stops with it.
On today's tree the object build passes: 536 of 583 units linked, IDENTICAL 533 of 544 functions, 8 DIFFERENT (the
same 8 the spike found), 47 refusals.

## 1. What changed

### `re/tools/ddkobj/ddkobj.py`

1. **SEH, item 4.** An object that registers an SEH handler (`__except_handler*` reference or `.sxdata`) is now
   `fail:seh-unmarked` and fatal. Before, it was linked unmarked, so the linker dropped the safe-handler table, and
   the tool only reported it. The check runs before the unbindable refusal, so a later binding fix cannot let such an
   object in without anyone noticing. Every object that is linked is marked `@feat.00`.
2. **Wrapper `#define` forms, items 1 and 9.** Before, any wrapper `#define` that none of the five parsers read was
   dropped. A DDK object reference to that name then fell back to the address suffix or the cross-wrapper binding,
   which is a guess. Now every such define is classified (`classify_unparsed_define`) and checked for each symbol a
   DDK object actually references:
   - `@samedata`: a data name `<x>_<addr8>` whose body names that same address, directly or through a
     `static const ... k_<addr> = 0x...;` constant (`HUD/ScenarioWriters_sa2s2.cpp:184-187`,
     `Race/SubStateMachine.cpp` DAT_005cd08c). This is the binding the tool already used. It is linked.
   - `@portdata`: a data name re-bound to a port cell, function or table, with no original address in the body.
     The DDK object can only read the original address, so the unit is refused as `fail:wrapper-redefines-globals`,
     the same class as the spike's MGLOBAL / MATCH_CELL refusals. Not fatal.
   - `@unparsed`: anything else. When a DDK object references the name, the unit becomes
     `fail:unparsed-wrapper-define` and the object build fails. Names no object references (MATCH_PORT, SSC_REC,
     `say(...)`, kGameSaveFilename) cost nothing.
   - A data define in the `@abs` form (`(*(T*)0x00yyyyyy)`) now binds the data to the wrapper's address. Today
     every such address equals the name's own address.
3. **One new parsed form.** `#define FUN_x(a, ...) T(a)`: the variadic tail is dropped or forwarded
   (`Match/PortTop_004039c0.cpp:14`, the only instance). The named parameters must still appear in order. The form
   is safe only for a caller-cleans call. The DDK reference keeps its decoration, so a stdcall reference does not
   resolve against a cdecl body, and the unit is then refused as unbindable. Without this form, today's tree failed
   the new check on 0x004039c0 (first run, `build_ddkobj.log` before the fix). With it, 0x004039c0 links and is IDENTICAL.
4. **Fatal exit.** When a fatal unit exists, `build` writes `ddkobj_build.json` with a `fatal` list, prints one
   `[ddkobj] FATAL` line per unit, and returns 3 with no wrapper recompile and no link.
5. **`check` summary.** `check` writes `ddkobj_summary.json` (identical, different, different_rvas, not_in_map,
   units, units_linked, refusals by status) and prints one `[ddkobj summary]` line.
6. **`selftest` subcommand.** Four fixture units in a temp directory, fed through `DDKOBJ_EXTRA_WRAPPERS` (RVAs
   0x00400010..0x00400050, PE header range): an SEH object, a callee bound by an unparsed define (swapped arguments),
   a port-cell data define, and a same-address `k_` data define. It also runs pure checks of the classifier and
   the variadic form.

### `mashedmod/build.bat`

- `if errorlevel 1` after `ddkobj.py check` (it failed silently before).
- The DDKOBJ-SPIKE REM comment named the matched-source folder (`re/match`). `Get-BuildMatchRefs` in the train
  treats a build script that names that folder as reading every file under it, so since DDKOBJ-SPIKE merged, every
  re/match change counted as a build input. The train self-test check `match-textonly (live tree)` failed on
  `orch/integration@778b8b91d` for this reason. The wording is changed, and a REM line says why.

### `re/orchestrator/merge_train.ps1`

- `Try-Batch`: the build sets `MASHED_DDKOBJ=1` next to `MASHED_NO_DEPLOY=1` and clears both afterwards.
- `Get-DdkobjStep` reads `mashedmod\build\ddkobj\ddkobj_build.json` and `ddkobj_summary.json`. It returns
  `not run` (the default build failed first), `FAILED: <n> unit(s) in a fail-loudly class: ...`,
  `FAILED: no ddkobj_summary.json` (link or check died), or the reported line. A failure bounces the batch with
  `why = object build: ...`, checked before the `build failed` check and before the gates. DIFFERENT rows are
  reported, not failed: 0x00414060 is a 100% match.py source whose bytes differ (spike section 3).
- Where it is reported: the train log (`object build: ...`), a `ddkobj:` line in REPORT (after `gates:`), `ddkobj` in
  the pass result JSON, and `Object build: ...` in the INTEGRATION_LOG line.
- Self-test section 13: Get-DdkobjStep on fake outputs (not run, fatal, missing summary, reported line), an AST
  check of the Try-Batch order (text-only exit, then env, build.bat, step, step failure, build-failed check, gates),
  and a check that build.bat fails on a failed build and on a failed check.

## 2. Checks run

| check | result | log |
|---|---|---|
| `ddkobj.py selftest` (inside test-slot) | 15/15 PASS. All four fixtures: rc 3, SEH -> `fail:seh-unmarked` fatal (`__except_handler3`), swapped-argument define -> `fail:unparsed-wrapper-define` fatal, port cell -> refused not fatal, `k_` -> ok. Without the two fatal fixtures: rc 0, the `k_` data binds `__ddkabs_00400054`, `@feat.00` marked | `ddkobj_selftest.log` |
| `merge_train.ps1 -SelfTest` (inside test-slot) | PASS, 109 checks, 0 failures (it was 1 failure on the base, match-textonly, fixed above) | `train_selftest.log` |
| `merge_train.ps1 -Plan` | batch C4-REPLAY-RULE, HB-V5-SKILL, RWC-CENSUS on orch/integration@778b8b91d. Parses and plans | console |
| object build on today's tree, `MASHED_DDKOBJ=1 mashedmod\build.bat` | rc 0, 182 s with the default objects up to date. `[ddkobj summary] IDENTICAL 533/544  DIFFERENT 8 [0x00414060 0x0042f530 0x0045a3a0 0x00475a00 0x0048e820 0x004929d0 0x00493710 0x00493900]  not-in-map 3  units linked 536/583  refusals 47 (static-register-convention=38, wrapper-redefines-globals=9)` | `build_ddkobj.log` |
| train step line on those real outputs (`Get-DdkobjStep`) | `IDENTICAL 533/544, DIFFERENT 8 [...], not-in-map 3, units linked 536/583, refusals 47 (static-register-convention=38, wrapper-redefines-globals=9)` | console |
| default build | `=== Build OK ===`, exe and asi built, no deploy | `build_default.log` |

Committed extracts: `BYTECHECK.tsv` (544 rows) and `ddkobj_summary.json` from the real build, in this folder.

## 3. What changed in the variant (against the spike's 540 units)

4 more units are refused as `fail:wrapper-redefines-globals`. Each was linked before as a DDK object that read the
original address, while the wrapper (the default build's body) reads a port table:

| unit | functions | wrapper | define |
|---|---|---|---|
| `re/match/s03/h2_00414180.c` | 0x00414180 | `Race/SubStateMachine.cpp` | `DAT_005f2d80 (reinterpret_cast<const IPair*>(kCdRows))` |
| `re/match/s01/00404e80.c` | 0x00404e80, 0x00404ee0 | `Save/GameSaveBuffer.cpp` | `DAT_0080335c (*(Blk2423c*)kSaveBufProfile)` and 8 more DAT_ names |
| `re/match/s01/00404e50.c` | 0x00404e50, 0x00404f50, 0x00404f80 | `Save/GameSave.cpp` | `s_gamesave_bin_005cc8e0 ((char*)kGameSaveFilename)`, `DAT_00803358` |
| `re/match/s01/00404e00.c` | 0x00404e00, 0x00404e20 | `Vehicle/CarMaterialTables.cpp` | `PTR_s_DirtBody_005ea0b8 ((char**)kBodyNames)` and 3 more |

The 8 fewer IDENTICAL functions (541 -> 533) are these 8 RVAs. They run their VS2022 bodies in the variant, which is
what the default exe runs. DIFFERENT (8) and not-in-map (3) are the spike's rows, unchanged. The variant was not
booted in this row: the change only returns units to the default exe's code.

## 4. Files

- `re/tools/ddkobj/ddkobj.py`, `mashedmod/build.bat`, `re/orchestrator/merge_train.ps1`
- `verify/ddkobjtrain_20261111/RESULT_DDKOBJ-TRAIN.md`, `BYTECHECK.tsv`, `ddkobj_summary.json`

Raw (not committed), `D:/MashedWT/DDKOBJ-TRAIN_data`:

| file | sha256 |
|---|---|
| build_ddkobj.log | c1e0dadaae783cad6e05062909743503ebe6820f457cbadef1aecb5e11979f3a |
| build_default.log | 30ec82c99f718ac6c5e4cd76e121277cbf7807c98507650b8cdb4901f57a9f35 |
| train_selftest.log | 904e65f3b4611a1efe04ff10d072f0fd14d84ae310bf13ccd5f1e10064009c2b |
| ddkobj_selftest.log | 4cab99b1ab86af1fdb03f356266533304a8289d57577deb2b950675d4fa6916e |
| ddkobj_build.json | 9c6d3a1cea94642bf33ba1672e6675ec9b28426627429249e1af866ea1cf590a |
| mashed_re_ddk.exe (worktree build\ddkobj) | 4d85f900b17c1087f6e5d6b0e1a195de003a87a7b8dfa9cf2e0ca920b0b13fad |

## 5. Notes for the lead

- Train cost: the object build adds about 3 minutes to each pass that builds (182 s here, default objects cached).
- The four new refusals can come back once those wrappers read the original addresses (the top-down route's
  rule: the original's globals at their original addresses). That is a port decision, and it is not part of this row.
- The variadic form is the only parser extension. The next unparsed form a DDK object needs will fail the train
  pass that brings it, by design. Extend `fnlike_target` / `classify_unparsed_define`, and add a `selftest` case.

## 6. Rebase onto df08d4cea (MATCH-REBASE-3, 2026-11-11)

The merge train set this branch aside on a `re/orchestrator/merge_train.ps1` conflict with HB-V5-SKILL (landed). Rebased
onto `orch/integration` df08d4cea. HB-V5-SKILL added `Format-Bounced` / `New-LogLines` / `Add-RedLogLine` and a
`Bounced:` clause on the merged-pass log line. This branch added an `Object build:` clause to that same line. Both are
kept, in this order: `<gates>. Bounced: ... . Object build: ... . Run data ...`. The HB-V5-SKILL self-test regex on the
Bounced clause still matches.

Checks re-run on the rebased tree (logs in `D:/MashedWT/DDKOBJ-TRAIN_data/*_rebase.log`):
- `merge_train.ps1 -SelfTest` (test-slot): SELFTEST PASS, 0 failures, HB-V5-SKILL's RED-pass checks and ddkobj 13 included.
- `ddkobj.py selftest`: OK, 0 failures.
- default build (`MASHED_NO_DEPLOY=1`): rc 0, exe and asi built.
- object build (`MASHED_DDKOBJ=1`): rc 0, `=== Build OK ===`.
- `ddkobj.py check`: IDENTICAL 534/544, DIFFERENT 7 [0x0042f530 0x0045a3a0 0x00475a00 0x0048e820 0x004929d0 0x00493710
  0x00493900], not-in-map 3, units linked 536/583, refusals 47 (static-register-convention=38,
  wrapper-redefines-globals=9). 0x00414060 is now IDENTICAL (on integration since the first run). The committed
  `BYTECHECK.tsv` / `ddkobj_summary.json` are the pre-rebase extract.
