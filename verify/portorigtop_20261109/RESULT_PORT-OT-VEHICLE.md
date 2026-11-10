# RESULT PORT-OT-VEHICLE (2026-11-10): GREEN, one VEHICLE stand-in (0x004114e0) and its callee 0x00483a40 on matched bodies under MASHED_ORIG_TOP

L2 worker `cmwaq9ez40dptle1mmr7sq78f` for lead `cmw7xu1960f37p41557tefpnd`. Branch `orch/PORT-OT-VEHICLE` from
orch/integration `9b4d5f452`. No queued branch merged in. A trial `git merge-tree` against orch/PORT-OT-GAMEPLAY
`5319e507c` is clean (section "Merge with PORT-OT-GAMEPLAY").

## Yield, plainly

**1 VEHICLE row plus 1 callee.** VEHICLE has 62 rows in the base runnable set, and only **one** of them is a stand-in.
The rest are matched (4) or have no BINDINGS row (57, under a matched or stand-in ancestor). The swapped body does
not run in any boot this step can drive (section "Reachability"), so the gain is the port column and one stand-in
fewer, not new behaviour.

## Pre-scan

- `origtop_base_dump.py --dump-base` (the PORT-OT-HUD method, SIZE.tsv restored afterwards): 62 rows with subsystem
  `vehicle` in the base runnable set. Their bindings: 57 `-`, 4 `matched`, **1 `stand-in`: 0x004114e0 ReplayCleanup**.
- BINDINGS.tsv stand-ins whose hooks.csv subsystem is `vehicle`: 2 = 0x004114e0 + **0x0045bae0** (not in the base
  runnable set: it loads PowerUpModels.txd and runs powerups_all.lua, hooks.csv:222).
- `ot_measure.py verdict` re-run on this tree (probe data `D:/MashedWT/PORT-OT-MEASURE_data`, no `--orig`. The output was
  kept out of the commit, and the committed OT_MEASURE.tsv is unchanged):
  - **0x00483a40** `OK`: `RwGlobals=0x00ff8290; call slots +0x10c=0x1b7450` (the committed file says the same, slot +0x10c only).
  - **0x004114e0** is in neither the RwGlobals set nor the seed set (base dump `rwglobals` / `nonzero_data` = 0 / 0). It needs nothing.
- Candidate count: **1** (+ the callee). Swapped: **1** (+ the callee). Kept: 0x0045bae0 (not runnable).

## What changed (one commit)

| rva | name | before | now | TU |
|---|---|---|---|---|
| 0x004114e0 | ReplayCleanup | `PortTop_StandIn04C_004114e0` (in 0x0040cfd0) | `Orig_004114e0` | new Match/PortTop_004114e0.cpp, **ORIGTOP-ONLY** CROSS-TARGET vs .asi-only Vehicle/Replay.cpp |
| 0x00483a40 | FUN_00483a40 | (not reached: its only knob-path caller was the 0x004114e0 stand-in) | `Orig_00483a40` | new Match/PortTop_00483a40.cpp (exe-only, no other body of the RVA: the .asi calls the original address, Vehicle/Replay.cpp:69) |

Callee check (the BOOT / FRONTEND lesson). Every callee binds by name, no raw original address:
- 0x004114e0: 0x00483a40 -> `Orig_00483a40`, called only for a nonzero DAT_0063bb04 / DAT_0063bb08
  (re/match/s02/004114e0.c). Then it stores 0 to DAT_0063bb14, DAT_0063bb10 and DAT_0063bb2c.
- 0x00483a40: RwGlobals +0x10c (librw free, LibRw/RwApiBind.cpp:1216) on `*(p + 0x14)`, then on `p`, and it returns 0
  (re/match/s13/00483a40.c). The slot is read through DAT_007d3ff8, the exe's fake RW device table (the B15 bridge).
  It is not a raw call address.

Shared state (the 0x00458880 lesson: grep each address over mashedmod/src):
- 0x0063bb04 / 0x0063bb08: no exe TU writes them. Their writers in the source tree are the .asi-only
  Vehicle/Replay.cpp and the matched 0x00411d90 (re/match/topm01/00411d90.c:117-134, no TU includes it, no BINDINGS row).
  Both stay 0 in the exe, so the free branch does not run.
- 0x0063bb10 and 0x0063bb14 are only read in the exe:
  - Vehicle/GhostSetupRender.cpp:44 (`kBB10`)
  - Vehicle/SmallLeaves_q4.cpp:121
  - Match/MatchS02_00411d60.cpp (re/match/s02/00411d60.c:43)
- 0x0063bb2c has no exe reader.
- The three zero stores are the ones the original makes on the same path (AppDestroy 0x00402a40 -> 0x0040cfd0).
No pool or record that the hand port owns is touched.

Source edit, re-checked with the DDK:
- re/match/s13/00483a40.c: `typedef int code();` is now `int code(int)` under `__cplusplus` only. This is the
  re/match/s12/004768c0.c pattern, and the C text is unchanged.
- `match.py --bytes`: 0x00483a40 41 B and 0x004114e0 77 B, both `BYTE-IDENTICAL`.
- `relocheck.py`: 0x00483a40 OK 2/2, 0x004114e0 OK 9/9.
- re/match/s02/004114e0.c is included unchanged (it already has the MATCH-S02 MGLOBAL preamble).

Other files:
- Match/PortTop_0040cfd0.cpp: `#define FUN_004114e0 Orig_004114e0`, an object-like define like the other rows.
- `mashedmod/exe_sources.rsp`: +2 TUs, inserted after `Match\PortTop_0040cfd0.cpp`, not at the tail where
  PORT-OT-GAMEPLAY appends.
- `re/tools/dual_copy_allowlist.txt`: +1 `ORIGTOP-ONLY CROSS-TARGET:004114e0` line (the `lint_rva_bodies.py` key),
  inserted above the 00428390 line, not at the tail.
- BINDINGS.tsv: written by the new `update_bindings_otvehicle.py`, which runs `update_bindings_othud.py` first. A new
  row goes right after its caller's row, not at the end of the file. Result: 440 -> 441 rows, matched 163 -> 165,
  stand-in 93 -> 92. The added row is 0x00483a40.
- Match/PortTop_StandIns04C.cpp is left as it was. The `PortTop_StandIn04C_004114e0` definition stays there,
  unreferenced. Deleting it conflicts with PORT-OT-GAMEPLAY's removal of the 0x00405400 definition right above it (a
  trial merge-tree reported `CONFLICT (content)`). It can go in a later tidy, after both branches are merged.

## Reachability

The new bodies are reached only through the matched 0x0040cfd0. Its only caller is the matched AppDestroy 0x00402a40
(Match/PortTop_00402a40.cpp), which is entered only from the matched WinMain 0x00492370 (MASHED_ORIG_TOP). There
`FUN_0040cfd0()` runs only when `FUN_00426ba0()` is nonzero (re/match/s01/raw/00402a40.c:93-97).

The flag is 0 in the knob-ON boot and in the nav-demo runs, so 0x004114e0 does not execute in any check below. It was
not logged before this change either: `004114e0` occurs 0 times in PORT-OT-HUD's and PORT-OT-GAMEPLAY's knob_on_1.log
and nav1 logs. This is the same situation PORT-OT-GAMEPLAY reported for 0x00405400 / 0x004725f0. The checks below show
that the default and knob paths are unchanged. They do not exercise the body. A run that sets the game-state flag
0x00426ba0 reads (a finished race before exit) would be the first one to execute it.

## Acceptance

Machine rules:
- Build: a gate script read `%TEMP%\mashed_machine_priolinger_*.lock` (held + live pid) and `mashed_lock.py status`
  (PRIORITY-HOLD / SOLO). `build.bat` started only after two clear reads. It waited about 11 min for merge train
  `20261110_103017` (PRIORITY-HOLD pids 4412 / 9056 / 10860).
- Every launch ran as `test-slot bash -c 'py -3.12 <orch-wt>/re/orchestrator/mashed_lock.py run --label PORT-OT-VEHICLE -- ...'`.
- Private root `D:/MashedWT/PORT-OT-VEHICLE_root`: `original` is a junction to `D:/MashedWT/PORT-TOP-04J_rootK/original`,
  as in PORT-OT-HUD / PORT-OT-GAMEPLAY.

| check | result |
|---|---|
| syntax check before the build (the 0x00411d90 sentence in the Match/PortTop_004114e0.cpp header comment was corrected after the build, comment only) | `cl /Zs /EHa /W3 /O2 /DMASHED_STANDALONE` on the 2 new TUs and Match/PortTop_0040cfd0.cpp: no error |
| build (`MASHED_NO_DEPLOY=1`) | `=== Build OK ===`, `[skip] MASHED_NO_DEPLOY set: mashed_re_dev.asi NOT deployed to original\`. mashed_re.exe sha256 `d3f670cb865a91f83a4732ecfe7f65316eeddb50a9692195f0a5b2a67ef899da`. `Orig_004114e0` and `Orig_00483a40` are in mashed_re.map |
| knob-ON boot (`knob_boot.py knob_on_1 <root> 300 MASHED_ORIG_TOP=1 MASHED_DET_FRAMES=3000`) | `knob_on_1: exited rc=0 (0x00000000) after 55.3 s`. :396-397 AppDestroy enter. :421-422 the teardown stand-ins 0x00412010 / 0x00458880 (unchanged). :619 AppDestroy returned. Last line `WinMain 0x00492370 returned 0 after 3000 frames`. No `004114e0` line (0x0040cfd0 does not run, section "Reachability") |
| 12 captures, OFF / ON / OFF / ON, pin default (`CAPFLAKE_DATA=<data> CAPFLAKE_ROOT=<root> capflake_runs.py nav1 OFF@dflt ON@dflt OFF@dflt ON@dflt`) | 4/4 `rc=0 pinned pos=55 hash=28538c74 class=E eqE=12`. The 12 hashes are equal in all four runs, knob ON and OFF. walk_04_colourselect.bmp `ba34030d1b7e2fe4` in all four (the reference) |
| G-CTRL (H_DEF) | `8284be46/67931834/31fe043e` = merge_train.json `gctrl.ref` |
| B (K0 / K1 / K26) | `1d5bf4f9` x3 = `b.ref` |
| rva-lint | `D4 burn-down=2  ORIGTOP-ONLY=9 (class deleted at the ORIG_TOP flip; 12 marked TU(s))`, `OK -- no NEW duplicate bodies (11 known)`. Before: ORIGTOP-ONLY 8 (orch/integration 9b4d5f452) |
| match.py --bytes on the edited source | 0x00483a40 BYTE-IDENTICAL 41 B, relocheck OK 2/2 (0x004114e0 77 B BYTE-IDENTICAL, relocheck OK 9/9, source unchanged) |

`A4_OUT` was created before `run_arms.py`. No new run_arms stack: this step has no knob.

### Port column deltas

`backfill_exe_file.py` (`rows given an exe_file: 11`, `old value changed: 0`), then `backfill_port.py`:
`none=5166 body=549 called=440`, 14 rows changed.

| rva | port |
|---|---|
| 0x004114e0, 0x00483a40 | none -> called (this step) |
| 0x00493600, 0x004ce790, 0x004935a0, 0x004935d0, 0x004d6910 | none -> called. Drift, not this change: bodies from PORT-OT-IMAGE-FMT / PORT-OT-RWG-DEVSLOTS (Match/PortTop_00493600.cpp, Match/PortTop_004ce790.cpp, Render/D3d9StateCache.cpp) whose orch/integration cells predate a build of the current tree |
| 0x004d5570, 0x004d53b0, 0x004d6b90, 0x004d7ac0 | none -> body. Same drift (Render/D3d9StateCache.cpp) |
| 0x004d86d0 RwStrupr, 0x004d8700 RwStrlwr, 0x004d8750 RwStrrchr | body -> called. Drift, the same three rows and values PORT-OT-GAMEPLAY reported |

## Merge with PORT-OT-GAMEPLAY

`git merge-tree --write-tree` of this tree against orch/PORT-OT-GAMEPLAY `5319e507c`: rc 0, no conflict. Both branches
touch Match/PortTop_0040cfd0.cpp, exe_sources.rsp, dual_copy_allowlist.txt, BINDINGS.tsv and hooks.csv, on separate
lines. After both are merged, `update_bindings_otvehicle.py` should chain `update_bindings_otgameplay.py` instead of
`update_bindings_othud.py`, a one-line change for whichever merges second. Running both updaters gives the same
BINDINGS rows in either order. They write disjoint rows.

## VEHICLE rows left, with the reason

| rva | name | reason |
|---|---|---|
| 0x0045bae0 | FUN_0045bae0 | not in the base runnable set (PowerUpModels.txd load + powerups_all.lua, hooks.csv:222) |

## Raw files (D:/MashedWT/PORT-OT-VEHICLE_data, not committed)

| file | sha256 |
|---|---|
| base.tsv (origtop_base_dump --dump-base) | 80bb10109aa1d11c410d499c4c5c4aff6945e5aa51bee92d759f60e3c88c21ef |
| OT_MEASURE.rerun.tsv (ot_measure.py verdict on this tree) | d25bcf4adb584e2525d036fcefaf77451b306afe73f9defb4916d853510989c2 |
| build.log | 555e2a8c5c88e22fa8d17c64909e30194d6201fe4cbcedc56c27ff39a1fb7890 |
| knob_on_1.log | 2b6898ec8f18b9e2719c38f512f8dd366c7c35a9755f773b2d04d1afa9ed8af2 |
| knob_on_1.stdout | 713a72124ad4e277b55d8f0ab1e37e4dc6c304e49df399671842af5fa6f2bac7 |
| runs.tsv | 57330649dd80ec34fbcbb541d4200d97cfe6376ecad59ca71d2e3a696bc0f656 |
| capflake.stdout | 4b724a2fa2fa5157520b9bdb6d45d5c8195b1fc3660ec085b4c0bc938802f537 |
| nav1/r01_OFF_pindflt.log | 3ff8f0b6371b5a6987d3e03860749becd2dcfbc5e5aeb745a3b3d3857de07919 |
| nav1/r02_ON_pindflt.log | 2c7a6a13031e0183dbbc5111b7089cf5cc8489d23e95e23396f65fd2d17f3e42 |
| nav1/r03_OFF_pindflt.log | c2b15772cfc58512403d223bc97acc69cd0b5a3f85698d7bd6a5d98ce39b7397 |
| nav1/r04_ON_pindflt.log | 70f29abfd78a45be11bb327514769cbc14ce177ba51e116fd8239418f9913e10 |
| arms.stdout | ce09320c8b42e2686352f7f052b36ea327e005d1f88012398bb3e998cd3877bc |
| arms/H_DEF.step.csv | 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198 |
| arms/H_DEF.gates.csv | 6793183492e0c0822570ad71254f9fdf27568b77c81a25c56347f1616863d58c |
| arms/H_DEF.ptrace.log | 31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3 |
| arms/B_DEF_K0.step.csv, B_DEF_K1.step.csv, B_DEF_K26.step.csv | 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 (all three) |

## Next

- No VEHICLE stand-in in the base runnable set is left. VEHICLE is done for L-111 (a) step 1.
- 0x0040cfd0's branch (and with it this body, 0x00405400 and 0x004725f0) needs a knob-ON run where `FUN_00426ba0()` is
  nonzero at AppDestroy before any of them executes. A harness item, not a swap.
- Tidy after PORT-OT-GAMEPLAY and this step are both merged: delete the unreferenced `PortTop_StandIn04C_004114e0` in
  Match/PortTop_StandIns04C.cpp, and point `update_bindings_otvehicle.py` at `update_bindings_otgameplay.py`.
