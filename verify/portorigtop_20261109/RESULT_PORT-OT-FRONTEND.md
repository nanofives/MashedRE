# RESULT PORT-OT-FRONTEND (2026-11-10): GREEN, 0x00428390 FrontendStateSet on its matched body under MASHED_ORIG_TOP; hooks.csv records repaired

L2 worker `cmwa3n8rb1jw9le1culs8yzdt` for lead `cmw7xu1960f37p41557tefpnd`. Branch `orch/PORT-OT-FRONTEND` from
orch/integration `e9fce5b22`, with `orch/PORT-OT-BOOT` (`a90c0355a`) merged (`git -c merge.autoStash=false`). The
SEED-2 branch was not needed (0x00428390 reads no initialised data) and is not merged.

## Verdict

One FRONTEND row runs its matched body on the MASHED_ORIG_TOP path: **0x00428390 FrontendStateSet**. Every other
FRONTEND stand-in on the path stays, each with a reason below. The 0x0041def0 + 0x0041e0d0 pair stays a stand-in
(agreed with the lead, filed as PORT-OT-BIND-41DEF0). The four malformed hooks.csv records are repaired, so the
backfills now write their `exe_file` / `port` cells.

## Scope: how the FRONTEND rows were found

- Base runnable set: `py -3.12 verify/portorigtop_20261109/origtop_base_dump.py --dump-base <tsv>` on this tree
  (SIZE.tsv restored afterwards, it belongs to PORT-ORIGTOP-SIZE). 174 rows have subsystem `frontend`: 10 already
  `matched` in BINDINGS, 1 `stand-in` (0x00428390), 163 with no BINDINGS row. A row with no BINDINGS row sits under a
  stand-in ancestor (mostly the hand frame tick 0x00492d30 / render 0x00492e90), so it does not run on the path and is
  not a swap of its own.
- BINDINGS.tsv stand-ins whose hooks.csv subsystem is `frontend`: 4 (0x00428390, 0x00495350, 0x00426b40, 0x00429290).
  Plus the pair the lead named (0x0041def0, 0x0041e0d0, subsystem `boot` in hooks.csv, the "Panel family" in the
  stand-in notes).
- OT_MEASURE.tsv: none of these rows carry a RwGlobals or seed caveat (0x00428390 reads no data, writes 0x0067d960).

## What changed (one commit)

| rva | before | now | TU |
|---|---|---|---|
| 0x00428390 | `PortTop_Piece_00428390` (hand piece 5 only) | `Orig_00428390(state)`, then hand piece 5 | new Match/PortTop_00428390.cpp, **ORIGTOP-ONLY** CROSS-TARGET vs the .asi-only Match/MatchS06_00428390.cpp |

- The matched source is re/match/s06/00428390.c (match.py 100%, hooks.csv C4 by match): `DAT_0067d960 = param_1`.
  0x00402750 calls it once, `FUN_00428390(3)` at re/match/s01/00402750.c:189.
- `PortTop_Piece_00428390` (Match/PortTop_00402750_Pieces.cpp) now calls `Orig_00428390(state)` first,
  unconditionally as the original does, then the hand frontend asset init it ran before (piece 5 is hand work placed at
  this call point, not a body of 0x00428390). Its log line now prints the value at 0x0067d960 after the call.
- Reachability: `Orig_00428390` has one caller, `PortTop_Piece_00428390`, whose one caller is the matched 0x00402750
  (`#define FUN_00428390 PortTop_Piece_00428390`, Match/PortTop_00402750.cpp:35), entered only through
  `PortTop_Bound_00402750` from the matched WinMain 0x00492370 (MASHED_ORIG_TOP). Not reachable without the switch.
- Readers of 0x0067d960 in the exe: only the matched sources re/match/s06/00428a30.c and 00429290.c, neither on this
  path (0x00429290 is only stored as a callback by 0x004b6560, which never calls it). No hand code reads it.
- `mashedmod/exe_sources.rsp` +1 TU. `re/tools/dual_copy_allowlist.txt` +1 `ORIGTOP-ONLY CROSS-TARGET:00428390` line
  (the `lint_rva_bodies.py --seed` key). BINDINGS.tsv through the new
  `verify/portorigtop_20261109/update_bindings_otfrontend.py` (runs `update_bindings_otboot.py` first, which runs the
  04J updater): 431 rows, matched 156 -> 157, stand-in 96 -> 95, only the 0x00428390 row changed.

## FRONTEND rows left, with the reason

| rva | name | reason |
|---|---|---|
| 0x00429290 | FUN_00429290 (title tick) | Not called on the path: 0x00402750 only passes it to 0x004b6560 as a callback, and the exe piz reader records it and never calls it (Match/PortTop_00402750_Pieces.cpp, `PortTop_PizFs_SetIdle_004b6560`). Its exe body Match/MatchS06_00429290.cpp calls 0x0045d430, 0x00466b50, 0x004967e0 (and 0x0045d3f0 in the exe) at raw original addresses, so a rebind would store a pointer to code that cannot run. 0x0045d3f0 / 0x0045d430 / 0x00466b50 have no exe body. Not in the base runnable set |
| 0x00495350 | HardwareShowIntroVideo | Hand piece 2 (dsound.dll guard, boot movies). Not in the base runnable set (DirectShow family, fam:d3d-dshow) |
| 0x00426b40 | FUN_00426b40 | Under 0x0040cfd0. Not in the base runnable set: closure of 34 nodes, 0x004b3fc0 / 0x004827a0 with no source, RW toolkit gen nodes (rwl02 / rwl04 / rwl05) with no librw binding (`closure_04c.py 0x00426b40`) |
| 0x0041def0 + 0x0041e0d0 | Panel family (stopclock2.dff) | 0x0041def0 is not in the base runnable set and has no OT_MEASURE verdict. Its closure needs bindings that do not exist: 0x004e69a0 RpAtomicClone (gen rwl02, RW-internal plugin copy), 0x004c1040 (frame reparent, no source, RW identity not confirmed per PORT-TOP-03), 0x004c1340 (writes the dirty list at RwGlobals+0xbc, which reads 0 on the exe record), 0x004b3fc0 (clump atomics into a list, banded CRT, no source), the four geometry user-data callees of 0x004b5190 (0x00543d40 / 0x00543d70 / 0x0055deb0 / 0x00543df0) that 0x004b51d0 uses to index the atomics by part id, the 0x004b52f0 chain, and an exe-only copy of 0x0042a5d0 (its exe body Match/MatchS06_0042a5d0.cpp calls 0x004b3bf0 at the raw address; `Orig_004b3bf0` exists). 0x0041e0d0 dereferences `+4` of each of the 66 entries of 0x0063d610 with no null check and only 0x0041def0 writes them (re/match/s04/0041def0.c:63-71), so it stays with it. Filed by the lead as PORT-OT-BIND-41DEF0 (after RWG-SLOTS / RWCORE-2) |

## hooks.csv repair (re-classify, tracker hygiene, no C-level change)

Four records did not have 11 fields, so `backfill_exe_file.py` / `backfill_port.py` left them untouched. Only those
four lines were rewritten (`csv` round-trip asserted per line), every other byte of hooks.csv kept. One CHANGELOG line
each below `<!-- ENTRIES -->`.

| rva | was | fix |
|---|---|---|
| 0x00404830 | 13 fields: the unquoted rtex notes text `(generated from the decomp, Vehicle/CarMaterialTables_dirt.inc, MASHED_STANDALONE only; not hooked, C-level unchanged)` was split at its commas, and the C4 batch then appended its fragment to the first piece | the three stray pieces put back after `(generated from the decomp`, before ` \| 2026-11-07 C4` |
| 0x004464c0 | 9 fields: no `exe_file` / `port` cells | two empty cells added, notes unchanged |
| 0x0041f360 | 11 fields, but the `exe_file` cell held the notes text `compiled by both targets (MASHED_STANDALONE resolves the MASHED tables); the standalone car set-up calls it` | moved back after `Vehicle/CarMaterialTables.cpp`, before ` \| 2026-11-07 C4` |
| 0x00420870 | 12 fields: unquoted notes with two commas | the notes are one quoted cell |

After the backfills: 0x00404830 `exe_file` Match/PortTop_00404830.cpp, port `called`. 0x004464c0 Util/CameraEntryDispatch.cpp,
`called`. 0x0041f360 Vehicle/CarMaterialTables.cpp, `called`. 0x00420870 `none`.

## Acceptance

Machine rules: no live PRIORITY-HOLD at build time (`%TEMP%\mashed_machine_priolinger_16512.lock` showed `held: true`
but pid 16512 was not running). Every game launch ran as `test-slot bash -c 'py -3.12
../orch-integration/re/orchestrator/mashed_lock.py run --label PORT-OT-FRONTEND -- ...'`.

| check | result |
|---|---|
| build (`MASHED_NO_DEPLOY=1`) | `=== Build OK ===`, `[skip] MASHED_NO_DEPLOY set: mashed_re_dev.asi NOT deployed`. mashed_re.exe sha256 `343fc031f08a34b8c8d66e59c7bfa5848b2c462640318470d4108d2c5de56fe2` |
| knob-ON boot (`knob_boot.py knob_on_1 D:/MashedWT/PORT-TOP-KNOBON-CAPFLAKE_rootK 300 MASHED_ORIG_TOP=1 MASHED_DET_FRAMES=3000`) | `knob_on_1: exited rc=0 (0x00000000) after 27.2 s`. knob_on_1.log:401 `0x00428390 FrontendStateSet(3): matched body ran (DAT_0067d960 = 3), piece 5 HandTop_InitFrontendAssets`, last line `WinMain 0x00492370 returned 0 after 3000 frames`. 0x0041def0 / 0x0041e0d0 still log their stand-in lines |
| 12 captures, OFF / ON / OFF / ON, pin default (`capflake_runs.py nav1 OFF@dflt ON@dflt OFF@dflt ON@dflt`, `CAPFLAKE_ROOT=D:/MashedWT/PORT-TOP-KNOBON-CAPFLAKE_rootK`) | 4/4 `rc=0`, `pinned pos=55 hash=28538c74`. The 12 hashes are equal in all four runs, knob ON and OFF. The driver prints `class=? eqE=11`: walk_04 (`walk_04_colourselect.bmp`) is `4d53d93f87f0332d` in all four, where its built-in 04E list has `2871487f1fcef51c`. That is the colour-select bar fix, not this step: `4d53d93f87f0332d` is the "BAR_Y ON" capture of verify/portstate_20261109/RESULT_PORT-COLOURSEL-BAR-Y.md:49 (OFF = `2871487f1fcef51c`, :48), default ON since L-110 (a) (Frontend/MenuNavSM.cpp:1148) and merged into orch/integration after PORT-OT-BOOT took its captures. The OFF runs do not reach any code this step touches (0x00402750 runs only under MASHED_ORIG_TOP) and show the same value. The other 11 equal the 04E list. `capflake_runs.py`'s E list is stale on walk_04 since that merge |
| G-CTRL (H_DEF) | `8284be46/67931834/31fe043e` = merge_train.json ref |
| B (K0 / K1 / K26) | `1d5bf4f9` x3 = ref |
| rva-lint | `D4 burn-down=2  ORIGTOP-ONLY=5 (class deleted at the ORIG_TOP flip; 6 marked TU(s))`, `OK -- no NEW duplicate bodies (7 known)`. Before: burn-down 2, ORIGTOP-ONLY 4 |

### Port column deltas

`backfill_exe_file.py` (4 rows given an exe_file: 0x00428390 and the three repaired rows that have one), then
`backfill_port.py`: `none=5181 body=551 called=423`, 39 rows changed.

This step:

| rva | port |
|---|---|
| 0x00428390 | none -> called (exe_file Match/PortTop_00428390.cpp) |
| 0x00404830, 0x004464c0, 0x0041f360 | (cell absent) -> called |
| 0x00420870 | (cell absent) -> none |

The other 34 changed rows are BOOT's own rows and the drift BOOT listed (all none -> called, 0x004026d0 and 0x004b64e0
body -> called): the merge of orch/PORT-OT-BOOT into this branch kept orch/integration's port cells for them, and the
backfill on this tree's build writes them again. 0x004014b0, 0x004025f0, 0x004026d0, 0x0041d870, 0x0041e850,
0x0041d410, 0x0041e630, 0x00496900, 0x004e7e30, 0x004b5320, 0x004b3bf0, 0x004e4900, 0x004b6520, 0x004b64e0,
0x004881d0, 0x0047ba00, 0x00420d00, 0x00412890, 0x00471eb0, 0x00489250, 0x004723d0, 0x00401000, 0x00401630,
0x00401690, 0x00471df0, 0x00496940, 0x004880a0, 0x0048bc10, 0x004e48e0, 0x004e4b90, 0x004e8090, 0x00471530,
0x004c1800, 0x004c1840.

### Raw files (D:/MashedWT/PORT-OT-FRONTEND_data, not committed)

| file | sha256 |
|---|---|
| build.log | d0224723bd1539d5329cda19422642cf10b4a919935fd4db6704ae9f37e9eea2 |
| knob_on_1.log | 704eccf33dedc8d00e41333940abd5b75995694b9876f7154f4cf8c056b9c6ce |
| runs.tsv | 4e426a6cccea90f5b8aac7c5e6cf501b2286df8c331c1f394c9557e564acdb49 |
| nav1/r01_OFF_pindflt.log | 6b976aed9d6db2556170d9f62331779ef990c390836bdd1111c5c96246bddc90 |
| nav1/r02_ON_pindflt.log | 614344cfccfd35ab436516d95bd81a3ef718a5e6d8e79881ca0fa6911d9892f1 |
| nav1/r03_OFF_pindflt.log | 698ef0b06351085b8f62a5b010a5f674a7573cb63990d609cf98bfa627a1663d |
| nav1/r04_ON_pindflt.log | 0b41fe9bdaeb6330d14fc4073d28f4e02f066df2eb67e5c4454a34b55e6fefaa |
| nav1/r0{1,2,3,4}_*/walk_04_colourselect.bmp | 4d53d93f87f0332d... (all four equal) |
| arms/H_DEF.step.csv | 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198 |
| arms/H_DEF.gates.csv | 6793183492e0c0822570ad71254f9fdf27568b77c81a25c56347f1616863d58c |
| arms/H_DEF.ptrace.log | 31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3 |
| arms/B_DEF_K0.step.csv, B_DEF_K1.step.csv, B_DEF_K26.step.csv | 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 (all three) |

## Next

- PORT-OT-BIND-41DEF0 (filed by the lead): the bindings listed above for 0x0041def0, then the pair in one commit.
- capflake_runs.py's built-in E list should take walk_04 = `4d53d93f87f0332d` (harness row, not done here: the driver
  belongs to PORT-TOP-KNOBON-CAPFLAKE).
