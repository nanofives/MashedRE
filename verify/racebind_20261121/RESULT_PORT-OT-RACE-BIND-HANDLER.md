# RESULT PORT-OT-RACE-BIND-HANDLER (run 54, 2026-11-21): STOP at 0x004b7330, a LIBRARY function (Lua 4.0, `third-party-library[lua-4.0]`, C2, no source), called from 0x0047b860

Branch `orch/PORT-OT-RACE-BIND-HANDLER` from `orch/integration` `65ef9f74a`. Worker claude2, Opus 5.5 medium, lead "Mashed run 54
RACE lead". PREREG `verify/racebind_20261121/PREREG_PORT-OT-RACE-BIND-HANDLER.md`, committed alone (`1888ba5ca`) before any
arm. Row kind: race-path binding, top-down. Profile data and generated outputs only: `porttop_bind.py` is not edited
(KICKOFF_run54 rule 1). No new knob, no default change, no tracker edit, no promotion file (every bound source already carries
its C4 row). G-CTRL / B / 12x4: not run (top-down row, the knob-off hashes cannot see it). One attempt. The session was paused
once for the LIVING-PC token-leak reboot (handoff `HANDOFF_PORT-OT-RACE-BIND-HANDLER.md`, 11:45 to 12:17) and resumed from it.

## 0. Verdict

| item | result |
|---|---|
| done-means | **second form met**: the stops passed are listed and the final stop is named with its class. Load length **not measured** (`meta.json` `race.reached` false, `race.n` 0, on every arm; ref1 = 108) |
| final stop | **0x004b7330** (`FUN_004b7330`, hooks.csv `third-party-library[lua-4.0]`, C2, no `re/match` source), exit arm `RO_ROLOADRWC-pr1` cp3, exit code 4944688 = `0x004B7330`, log line 461 `rva 0x004B7330: race-load stop (PORT-OT-RACE), call 1, callers 0x0047b860, return 002586A5, exits`. Disassembly of the anchored binary: `0x0047b860 push 0`, `0x0047b862 call 0x4b7330`. Per the lead's rule the row stops binding here: a Lua row is the parent's call |
| stops passed on the RW core arm (exit form, one per checkpoint) | head (RACE-RWCORE-RACELOAD-ENGINE): 0x00458dd0 -> cp1: **0x00458630** (exit 0x00458630, callers 0x00458dd0) -> cp2: **0x004e89a0** (exit 0x004E89A0, callers 0x00474f90 0x004b4080) -> cp3: **0x004b7330** (Lua, final) |
| class of each stop | 0x00458dd0 game (hooks.csv `gameplay`), 0x00458630 game (hooks.csv `frontend`), 0x004b4080 / 0x004b4050 game (hooks.csv `render`), 0x004e89a0 RW (`third-party-library[renderware]`, RW class, not a library stop), 0x004b7330 **library (Lua)** |
| count-mode list after the final stop (cp3, `RO_ROLOADRWCC-pr1`) | 0x004b7330 (Lua), 0x004c0510 (`third-party-library[lua-4.0]`, C4 libid), 0x0047b980 (RegisterScriptCommand, game, C4 match, caller 0x00440bc0), then `0xC0000005` ([UNCERTAIN] count-mode consequence, register line "PORT-OT-RACE-BIND-HANDLER (count-arm fault)") |
| self-tests (cp3 exit arm) | `[portstate-image]` PASS 0 of 8, `[OT_CODEPTR]` PASS 0 of 367, `[REPLAY]` init 66 checks 0 failed, `RwTexDictionaryCreate` PASS 0 of 11 (calls 1 and 2), `[RWC-DEVICE]` selftest PASS 0 fail, `[OT_TIMER]` PASS 0 of 2990. Binder selftest: raceload IDENTICAL (section 4) |
| 0x0040cea0 (L-175) | bound as a **replay-C4 body**: the binder includes `re/match/s02/0040cea0.c`, the file the PROV-INSITU-REF1 replay verified (`VERDICTS.tsv` line 282, `IDENTICAL-FOR-600`, source column `re/match/s02/0040cea0.c`; file unchanged since `c9134b0f7`, 2026-10-31). Pre-bound keep-alive only: its one caller 0x0040d110 is LAND's cp2 and still binds `PortTop_BulkStop_0040cea0` |
| 0x004790e0 | **not bound** (prediction 4): register-arg (world in EAX). Not reached on any arm of this row. Filed as row **PORT-OT-RACE-BIND-4790E0-EAX** (open): the MATCH_PORT branch of `re/match/s12/00479330.c` (lines 173-181, call at 483) calls `FUN_004790e0()` with no argument and `re/match/s12/004790e0.c` defines it `static`, so a data-only bind cannot express it |
| 0x004b7480 | **not bound**: `third-party-library[lua-4.0]`, C2, no source. A library function (Lua); behind the final stop |

## 1. Change (profile data + generated outputs)

`re/tools/bind/state/raceload/`:
- `c_hold.txt` -5: 0x00440bc0, 0x00458dd0, 0x0047b860, 0x0047b8d0 (cp1), 0x004e89a0 (cp3) now bind as C units (BIND-CUNCLEAN-AS-C emit).
- `roots.txt` +9: 0x00440bc0, 0x00458dd0, 0x0047b860, 0x0047b8d0, 0x0040cea0 (cp1); 0x00458630, 0x004b4080, 0x004b4050 (cp2);
  0x004e89a0 (cp3). All in `log/orch/run53_bindlists/race_owned.txt`, none in `audio_owned.txt`, none in the LAND closures
  (no `Match/PortTop_<rva>` and no plan entry on orch/PORT-TOP-06 or orch/PORT-TOP-RENDERFRAME-492E90).
- `rules.tsv` +28 `extra_pre` lines for the C units 0x0047b860 / 0x0047b8d0. First VS2022 link: 7 x LNK2019 (`_DAT_006bf1e0`,
  `_DAT_005cf010`, five `_thunk_FUN_*`): in C the source preamble's `MGLOBAL` is `extern T n`. The lines replace the preamble
  (MGLOBAL emptied, each global a `#define` at its original address) and bind the Ghidra `thunk_` names to the jump targets'
  RwCore bodies, each line citing the disassembly (`0x004b5360 jmp 0x5507b0`, `0x004b5370 jmp 0x550910`, `0x004b5380 jmp
  0x550950`, `0x004b5390 jmp 0x5509b0`, `0x004b53a0 jmp 0x550bc0`; calls at 0x0047b90b / 0x0047b917 / 0x0047b91d / 0x0047b929
  / 0x0047b937 / 0x0047b93f; `0x0047b868 mov [0x6bf1e0], eax`, `0x0047b905 push 0x5cf010`).
- `plan.json`, `newfiles.txt` regenerated. Final plan: **bind 182 (match 177, replay 3, provisional 2), reuse 114, stop 115**.

Generated (`porttop_bind.py write --profile raceload --roots-only`, `sync --step PORT-OT-RACE-BIND-HANDLER`): new
`Match/PortTop_0040cea0.cpp`, `_00440bc0.c`, `_00458dd0.c`, `_0047b860.c`, `_0047b8d0.c`, `_00458630.cpp`, `_004b4050.cpp`,
`_004b4080.cpp`, `_004e89a0.c`; regenerated callers (`_00458e00`, `_0047a020`, `_0047a0f0`, `_0047a130`, `_0047b9b0`, `_00474f90`,
`_00462950`, `_00478660`), `PortTop_RaceLoadStops.cpp`, `PortTop_BulkRefs.cpp`; `exe_sources.rsp` +9; `dual_copy_allowlist.txt`
+2 `ORIGTOP-ONLY DUP-IN-TARGET` (0x004b4050 with `Render/SlotObjectAccessors.cpp`, a frozen hand-path body; 0x004e89a0 with
the lifted RwCore scaffolding). rva-lint 467 known, NEW 0.

**One-generator cleanup** (the raceload selftest DIFFERENT files): the write refreshed the stale raceload plan rows
(0x00478660 / 0x0047a020 provisional -> replay, they are `c4=frida-replay` now; the roots-only stop texts; 0x005a7b60 reuse) and
the wrappers `PortTop_00462950.cpp`, `_00478660.cpp`, `_0047a020.cpp`, `PortTop_RaceLoadStops.cpp`. Selftest raceload before:
plan DIFFERENT 8 rows + 4 files; after: **plan IDENTICAL, wrappers+stops IDENTICAL (179 checked)**.

## 2. Binding numbers

| number | rvas |
|---|---|
| **8 executing or reachable from a bound caller** | executed on the arms (a later stop was reached through them): 0x00458dd0, 0x00458630, 0x004b4080, 0x004e89a0, 0x0047b860 (the final stop's caller), 0x00440bc0 (cp3 count arm: its callee 0x0047b980 was called from it). Reachable, not yet executed: 0x004b4050 (callee of the bound 0x004b4080), 0x0047b8d0 (callee of the bound 0x0047b9b0, after 0x0047b860) |
| **1 pre-bound keep-alive only** | 0x0040cea0 (`PortTop_Keep_0040cea0` in `PortTop_BulkRefs.cpp`; caller 0x0040d110 is LAND cp2's) |
| **bindable left unbound, with reason** | 0x004790e0 register-arg, needs a source port-branch edit (row PORT-OT-RACE-BIND-4790E0-EAX); 0x0047b8a0 (C4 match) roots-only stop, behind the Lua stop inside 0x0047b8d0; 0x0047b980 (C4 match, game) behind the Lua stop, reached only in count mode; 0x004c0510 Lua (C4 libid), 0x004b7330 / 0x004b7480 Lua (C2, no source): library, the parent's call |

## 3. Lua functions the callers reach (census)

Source `verify/census_20261116/census_T2-RACEPATH-CENSUS.tsv` (3436 rows) joined with `hooks.csv` subsystem
`third-party-library[lua-4.0]`; table `verify/racebind_20261121/lua_census_raceload.tsv`.

| phase | Lua-class functions | census calls |
|---|---|---|
| boot | 126 | 14,482 |
| race-load | **130** | **12,563** |

Race-load split: **88 C2** (8,625 calls, no source), **42 C4 libid** (3,938 calls). Top by calls: 0x004ba1b0 1086 (C2),
0x004b9b00 480 (libid), 0x004bf260 `lcode_getprevinstr` 467 (C2), 0x004b7cc0 446 (libid), 0x004baba0 433 (C2), 0x004bc590 433
(C2), 0x004ba140 426 (libid), 0x004b9ef0 410 (C2). The entry points this row's bound callers call directly: 0x004b7330 2 calls
(C2, from 0x0047b860), 0x004c0510 2 (libid, from 0x0047b860), 0x004b7480 2 (C2, from 0x0047b880), 0x004b7a70 2 (C2, from
0x0047b8a0). All 130 are `unbound` in the census.

## 4. Builds and checks

`mashed_lock.py status` and `test-slot --status` printed before every build, check and arm. **Rule 4 slip, reported:** the cp3
VS2022 and object builds (one chained command) ran while the status showed `PRIORITY-HOLD` / `SOLO` of merge-train
`20261121_123835` (12:56-13:06). The command printed the status but did not wait. From then on a wait loop
(`lockwait.py`, polls until neither PRIORITY-HOLD nor SOLO) ran before every arm and check. The cp3 arms and check started under
a plain HELD.

| check | result |
|---|---|
| VS2022 build (`MASHED_NO_DEPLOY=1`) | cp1 first link 7 x LNK2019 (section 1), then `=== Build OK ===` at cp1 / cp2 / cp3; rva-lint NEW 0. Final `mashed_re.exe` `b94cd620...` |
| object build (`MASHED_DDKOBJ=1`) | `=== Build OK ===` at cp1 / cp2 / cp3. Final `mashed_re_ddk.exe` `0322af30...` |
| `ddkobj.py check` (final, cp3) | IDENTICAL 1982/2042, DIFFERENT 33, not-in-map 27, units linked 1973/2240, refusals 14 (externalized-static-register-convention 8, wrapper-redefines-globals 6), provisional kept VS2022 253 units + 6 fns, dual body 33 |
| against `log\orch\run54_headcheck\` (1976/2035, DIFFERENT 32, nim 27, refusals 12, dual 32) | `BYTECHECK.tsv`: every head row unchanged; 7 new rows: 0x00440bc0, 0x00458630, 0x00458dd0, 0x004b4050, 0x004b4080, 0x004e89a0 **IDENTICAL**; 0x0040cea0 **DIFFERENT** (the one newly DIFFERENT unit) |
| newly DIFFERENT, classified | **0x0040cea0 -> "replay-C4 body"**: source `re/match/s02/0040cea0.c` (82.8%) is the replay-verified file (L-175, merge bar); `c4=frida-replay` in hooks.csv |
| refusals +2 | 0x0047b860 and 0x0047b8d0, `fail:wrapper-redefines-globals` (evidence `#define MGLOBALA(T, n, N, a) enum { mglobala_##n }`): ddkobj accepts only the C++ reference form of MGLOBAL (`MGLOBAL_ADDR_RE`, ddkobj.py:216), which C cannot use. They keep their VS2022 bodies (the same 100% source compiled by VS2022). Not worked around (WORKER_RULES). **Tool-row request**: ddkobj (or the binder's C emit) needs a C form of global binding for C units with MGLOBAL globals |
| dual body +1 | 0x004e89a0: IDENTICAL, and also listed in `PROVISIONAL.tsv` line 264 (`RwCoreLift_004e89a0`, lifted scaffolding L-137): "pre-bound" class, as the head's 32 |
| listed unit -> 100% neighbour fallback pairs | **none new** (provisional kept VS2022 253 units + 6 fns, equal to the head). The two refusals above are 100% sources that fell back by refusal, not by a listed neighbour |
| match.py `--bytes` + relocheck (`match_relocheck.txt`) | 0x00440bc0, 0x00458dd0, 0x0047b860, 0x0047b8d0, 0x00458630, 0x004b4080, 0x004b4050: **BYTE-IDENTICAL + relocheck OK** each (0x00440bc0 unchecked `_LAB_0047ad30`, `_LAB_004917a0`; 0x0047b8d0 unchecked the GS-cookie / `__chkstk` symbols). 0x004e89a0: 100% C4 match per hooks.csv, linked IDENTICAL in the object build (not re-run with match.py). 0x0040cea0: not bound as matched (BAD, 109 bytes differ, expected for an 82.8% replay-C4 source) |
| binder selftest | `FAIL 14`, all in the **audio** profile (plan rows 005a6ea0 / 005a9f10 replay, 12 stops now reuses: owned by the audio rows, not this row). raceload IDENTICAL, refs IDENTICAL, C emit 38 units + fixture + 20 held compile, BINDINGS.tsv IDENTICAL |
| knob boot / menu walk | not run separately: no menu code changed; the arms are knob-on boots of the object build and print the self-tests in section 0 |

## 5. Arms (object build, root `D:\MashedWT\PORT-OT-RACE-BIND-HANDLER_root`, image `81CC7775...` staged)

`A4_RO_CKPT=0,1,17,18,299,301,600`, test-slot outside, mashed_lock inside, background timeout 7200000. Stacks `ROLOADRWC`
(exit) and the new `ROLOADRWCC` (= `@ROLOADRWC` + `MASHED_OT_RL_STOPS=count`).

| checkpoint | exe | `RO_ROLOADRWC-pr1` (exit) | `RO_ROLOADRWCC-pr1` (count) |
|---|---|---|---|
| cp1 (5 binds) | `be4a9d66` | 0x00458630, 15 s | 0x00458630, 0x004b4080, then 0xC0000005 |
| cp2 (+3) | `af5e2019` | 0x004E89A0, 7 s | 0x004e89a0, then 0xC0000005 |
| cp3 (+1) | `0322af30` | **0x004B7330**, 11 s; COURSE.LUA found (`Searching piz for COURSE.LUA`, line 460) | 0x004b7330, 0x004c0510, 0x0047b980, then 0xC0000005 |

`race.reached` false on all six. Prediction check (PREREG): 1 holds (5 new units IDENTICAL or kept VS2022 with a stated reason;
the one newly DIFFERENT unit classified); 2 holds (no 0x00458dd0 stop); 3 holds (0x004b7330 is the first stop of the COURSE.LUA
chain, library); 4 holds (0x004790e0 stays a stop); 5 holds; 6: not measured, stop list given.

## 6. Next

1. Lua: the parent decides a Lua row (130 race-load functions, 88 C2 with no source, section 3) or a Lua stand-in route.
2. Row PORT-OT-RACE-BIND-4790E0-EAX (filed, open).
3. Tool row: a C form of MGLOBAL binding that ddkobj accepts, so 0x0047b860 / 0x0047b8d0 link their DDK objects.
4. LAND merge: when the lead says LAND (orch/PORT-TOP-06 + RENDERFRAME) is on the head, merge orch/integration into this branch
   (`git -c merge.autoStash=false`), `porttop_bind.py write --profile raceload --roots-only` + `refs` + `sync`, both builds,
   `ddkobj.py check`; no hand merge. 0x0040cea0 then gains its bound caller if LAND binds 0x0040d110.

## 7. Evidence

Committed (`verify/racebind_20261121/`): this RESULT, the PREREG, the HANDOFF, `racebind_log_extract.txt` (stop and self-test
lines of all six arms), `match_relocheck.txt`, `lua_census_raceload.tsv`, `raw_sha256_PORT-OT-RACE-BIND-HANDLER.txt` (SHA-256 of
every raw file cited, both exes and the image); stack `verify/savelink_20261023/stacks/ROLOADRWCC.txt`; one line in
`re/orchestrator/UNCERTAIN_REGISTER.md`.

Raw (not committed): `D:/MashedWT/PORT-OT-RACE-BIND-HANDLER_data` (cp1 arms at the top level, `cp2/`, `cp3/`; build logs
`build_vs{1..4}.log`, `build_ddk{1..3}.log`; `ddkcheck{1..3}.log`; `cp3/BYTECHECK.tsv`, `cp3/ddkobj_summary.json`;
`selftest_base.txt`, `selftest_after.txt`; plan listings).

Commands: as RACE-RWCORE-RACELOAD-ENGINE section "Commands", with `A4_OUT=D:/MashedWT/PORT-OT-RACE-BIND-HANDLER_data[/cp2|/cp3]`,
`A4_ROOT=D:/MashedWT/PORT-OT-RACE-BIND-HANDLER_root`, arms `RO_ROLOADRWC-pr1 RO_ROLOADRWCC-pr1`; binder
`py -3.12 re/tools/bind/porttop_bind.py write --profile raceload --roots-only`, `sync --profile raceload --step PORT-OT-RACE-BIND-HANDLER --date 2026-11-21 --evidence <this RESULT>`, `selftest`.

## 8. LAND merge (lead order after LAND reached the head): bar (c) NOT met, 12 new selftest lines in two profiles this row does not own

Head `5740dcc92` (race = main = orch/integration, LAND = PORT-TOP-LAND-06-RF merged at `95a7fca52`). Merge commit
**`219dc129b`** (`git -c merge.autoStash=false merge orch/integration`). One conflict, in the generated
`Match/PortTop_BulkRefs.cpp`: I took the head's copy and regenerated it with the binder. No hand edit of any generated file. Every raceload state
file merged clean (the head did not change them).

Regenerate (raceload only; audio, bulk, raceframe and renderframe not regenerated or edited):
`py -3.12 re/tools/bind/porttop_bind.py write --profile raceload --roots-only` (plan bind 182 / reuse 115 / stop 114; 1 file
changed: BulkRefs), `refs --profile raceload` (129 caller-less bound bodies, 0 files changed), `sync --profile raceload --step
PORT-OT-RACE-BIND-HANDLER` (rsp +0, allowlist +0). BulkRefs against `5740dcc92`: `+/include:_PortTop_Keep_0040cea0`,
`-/include:_PortTop_Keep_0041ecc0` (0x0041ecc0 now has a bound caller, 0x0040cea0), the same delta as before the merge.

RULE 4: `lockwait.py` (waits out PRIORITY-HOLD and SOLO) before each build, the check and the selftest; every wait ended at once
(`free`).

| check | result |
|---|---|
| VS2022 build | `=== Build OK ===`, rva-lint 642 known, NEW 0. `mashed_re.exe` `c9de8f69...` |
| object build | `=== Build OK ===`. `mashed_re_ddk.exe` `88afc397...` |
| `ddkobj.py check` | IDENTICAL 2336/2435, DIFFERENT 72, not-in-map 27, units linked 2322/2595, refusals 19 (externalized-static-register-convention 13, wrapper-redefines-globals 6), provisional kept VS2022 254 units + 7 fns, dual body 33 (`ddkobj_summary_land.json`) |
| against the head tree | LAND's `verify/porttopland_20261121/ddkobj_summary_cp3.json` is the object check of the same exe tree as `5740dcc92` (`git diff a79ff004b 5740dcc92 -- mashedmod re/match re/tools/bind` is empty). Delta: DIFFERENT +`0x0040cea0` (replay-C4 body, section 4), dual body +`0x004e89a0` (PROVISIONAL-listed lifted scaffolding, pre-bound class), refusals wrapper-redefines-globals +2 (0x0047b860 / 0x0047b8d0, section 4), IDENTICAL +6 (this row's binds), not-in-map equal. **No other newly DIFFERENT unit, no new fallback pair** |
| against `log\orch\run54_headcheck\` (77dedfece tree) | DIFFERENT 32 -> 72: the +40 are LAND's 39 (classified in RESULT_PORT-TOP-LAND-06-RF) + 0x0040cea0; not-in-map 27 both (5 in, 5 out, LAND's) |

Merge bar (lead, narrowed):

| bar | result |
|---|---|
| (a) every profile this row owns at 0 failures | **met**: raceload `plan IDENTICAL (bind 182 reuse 115 stop 114), wrappers+stops IDENTICAL (183 checked)`, refs IDENTICAL, C emit 39 units + fixture + 20 held compile |
| (b) audio state + generated outputs byte-identical to `5740dcc92` | **met**: `git diff --stat 5740dcc92 HEAD -- $(cat verify/racebind_20261121/audio_paths_checked.txt)` prints **nothing** (390 paths: `re/tools/bind/state/audio/**`, every `Match/PortTop_<rva>` the audio plan binds, `PortTop_AudioStops.cpp`, `verify/porttop_20261108/BINDINGS.tsv`); working tree clean |
| (c) selftest failures a subset of the head's audio lines | **NOT met**. `selftest_land.txt`: FAIL 22 (head: FAIL 14, `verify/porttopland_20261121/selftest_cp3.txt`). Line by line (`selftest_subset_land.tsv`): all 28 audio lines are in the head, and **12 lines are new** (below) |

The 12 new lines, with owning row. Both come from this row's new bodies: the bulk and raceframe profiles plan a body another
profile bound as a reuse (`bound_wins`), and their committed plans still name the old stop. I fixed nothing (not my profiles):

| profile (owner) | new lines |
|---|---|
| **bulk** (wrapper `PortTop_0040d110.cpp` = 0x0040d110, PORT-TOP-LAND-06-RF cp2; bulk profile = BIND-BULK-PROFILE / RACE-BIND-C4-BULK) | `bulk roots 362: plan DIFFERENT (bind 97 reuse 204 stop 188), wrappers+stops DIFFERENT 2 (98 checked)`; `plan 0040cea0 ['stop', 'outside the bulk cp1 list ...'] -> ['reuse', 'PortTop_Bound_0040cea0', 'Match/PortTop_0040cea0.cpp']`; `file Match/PortTop_0040d110.cpp`; `file Match/PortTop_BulkStops.cpp`; `moved, not named: 0040cea0 ...`; `moved, not named: totals ...`; `bulk listing vs verify/bindbulk_20261121/tools/last_plan.txt: 59 lines moved, FAIL 2 unnamed, 0 stale` |
| **raceframe** (PORT-TOP-LAND-06-RF; wrapper `PortTop_004b40f0.cpp`) | `raceframe roots 1: plan DIFFERENT (bind 256 reuse 148 stop 129), wrappers+stops DIFFERENT 2 (257 checked)`; `plan 004e89a0 ['stop', 'owned by RACE rows (run 53, race_owned.txt)'] -> ['reuse', 'PortTop_Bound_004e89a0', 'Match/PortTop_004e89a0.c']`; `file Match/PortTop_004b40f0.cpp`; `file Match/PortTop_RaceFrameStops.cpp` |
| (total) | `[selftest] FAIL 22` (head FAIL 14) |

What clears them (owner's call, or the lead authorizes this row): `porttop_bind.py write --profile bulk` + `refs --profile raceload` +
`sync --profile bulk`, and `write --profile raceframe` + `sync --profile raceframe`, on this branch. Then 0x0040d110 calls the bound
0x0040cea0 (its keep-alive anchor drops out) and 0x004b40f0 calls the bound 0x004e89a0. The bulk listing also needs its
`last_plan.txt` moved-line names for 0x0040cea0. Both are plan-only follow-ons of bodies this row bound. No body changes.

## 9. bulk + raceframe regeneration (authorized by the parent via L0, option 1): condition 4 NOT met, 2 bulk listing lines need a `state/bulk/moved.tsv` edit that condition 2 forbids

Branch at `63139442c` before this step. Regenerate commands, all through the tool: `porttop_bind.py write --profile bulk` (2 files
changed), `refs --profile raceload` (0 changed), `sync --profile bulk --step PORT-OT-RACE-BIND-HANDLER` (rsp +0, allowlist +0),
`write --profile raceframe` (2 files changed), `sync --profile raceframe ...` (rsp +0, allowlist +0). `lockwait.py` before
each build, the check and the selftest (every wait cleared at once, `free`).

What moved (the whole diff of this step):
- `Match/PortTop_0040d110.cpp`: `#define FUN_0040cea0 PortTop_BulkStop_0040cea0   // named stop` -> `PortTop_Bound_0040cea0   // exe body (Match/PortTop_0040cea0.cpp)`
- `Match/PortTop_004b40f0.cpp`: `#define FUN_004e89a0 PortTop_RFStop_004e89a0   // named stop` -> `PortTop_Bound_004e89a0   // exe body (Match/PortTop_004e89a0.c)`
- `Match/PortTop_BulkStops.cpp` -4 lines (stop 0x0040cea0), `Match/PortTop_RaceFrameStops.cpp` -4 lines (stop 0x004e89a0)
- `state/bulk/plan.json` key `0040cea0`: `['stop', 'outside the bulk cp1 list ...']` -> `['reuse', 'PortTop_Bound_0040cea0', 'Match/PortTop_0040cea0.cpp']`;
  `state/raceframe/plan.json` key `004e89a0`: `['stop', 'owned by RACE rows ...']` -> `['reuse', 'PortTop_Bound_004e89a0', 'Match/PortTop_004e89a0.c']`
  (`write` output; no other key differs from `5740dcc92`)

| condition | result |
|---|---|
| 1. generated outputs only | **met**: the six files above, all written by `write` (no hand edit) |
| 2. `state/bulk/` + `state/raceframe/` byte-identical to `5740dcc92` | **met for every data file**: `git diff --stat 5740dcc92 -- re/tools/bind/state/bulk re/tools/bind/state/raceframe ':!*/plan.json'` prints **nothing** (profile.json, rules.tsv, roots.txt, cpp_unclean.txt, stop_reasons.tsv, moved.tsv, disposition.json, newfiles.txt). **Deviation, stated:** the two `plan.json` files are `write` outputs and each differs by the one key above. `write` always rewrites its plan, so the two orders (write the plan, keep it byte-identical) conflict on that file |
| 3. audio untouched | **met**: `git diff --stat 5740dcc92 -- $(cat verify/racebind_20261121/audio_paths_checked.txt)` prints **nothing** (390 paths) |
| 4. selftest failures a subset of the head's 14 audio lines | **NOT met**: FAIL 16 (`selftest_regen.txt`, `selftest_subset_regen.tsv`). All 28 audio lines are in the head. bulk and raceframe plans + wrappers are now **IDENTICAL**. **2 new failure lines** remain, both in the bulk listing check (`bulk listing vs verify/bindbulk_20261121/tools/last_plan.txt: 59 lines moved, FAIL 2 unnamed, 0 stale`): `moved, not named: 0040cea0 \| 0040cea0 stop  C2 FUN_0040cea0 -> 0040cea0 reuse PortTop_Bound_0040cea0 C4 FUN_0040cea0` and `moved, not named: totals \| ... 'reuse': 203, 'stop': 189 ... -> 'reuse': 204, 'stop': 188 ...`. Clearing them takes two lines in `state/bulk/moved.tsv` (a `0040cea0` row with its cause, and the `totals` row's head column), the same kind of edit LAND made for its 5 moved lines. That is a `state/bulk` data edit, forbidden by condition 2, so it is **not done**. Owner: the bulk profile (BIND-BULK-PROFILE / RACE-BIND-C4-BULK), or this row on the lead's word |
| 5. ddkobj re-checked | IDENTICAL 2336/2435, DIFFERENT 72, not-in-map 27, units linked 2322/2595, refusals 19, provisional kept VS2022 254 + 7, dual body 33: the **same sets** as the LAND-merge check (section 8; `sumdiff`: different / dual / not-in-map +0 -0). Against the head tree: DIFFERENT +0x0040cea0 (replay-C4 body), dual +0x004e89a0 (pre-bound), refusals +2 (0x0047b860 / 0x0047b8d0). No newly DIFFERENT unit, no new fallback pair. Builds: VS2022 OK (rva-lint 642, NEW 0, `mashed_re.exe` `bbad7250...`), object OK (`mashed_re_ddk.exe` `2e574341...`) |
| 6. LAND's numbers, equal or better | **met**: raceframe plan bind 256 (+ 31 file-local members) = **287** reachable, equal to LAND; reuse 147 -> 148 (+0x004e89a0, a stop name that now resolves); stop 130 -> 129. bulk: bind 97 equal, reuse 203 -> 204 (+0x0040cea0), stop 189 -> 188. Keep-alive anchors in `Match/PortTop_BulkRefs.cpp`: **129**, equal (versus the head: +0x0040cea0, -0x0041ecc0). The refs step still anchors 0x0040cea0 although 0x0040d110 now calls it. [UNCERTAIN] `refs --profile raceload` reads callers from the raceload plan and not from the bulk wrappers (not read in the tool): the extra anchor is harmless (an /include of a symbol that is also called) |

Stopped here: the bulk `moved.tsv` edit is the only remaining failure and it needs the lead's word (it conflicts with condition 2).
Committed with the ledger set to blocked, not review.
