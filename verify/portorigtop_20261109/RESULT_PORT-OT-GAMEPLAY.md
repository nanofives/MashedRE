# RESULT PORT-OT-GAMEPLAY (2026-11-10): GREEN, three GAMEPLAY stand-ins on their matched bodies under MASHED_ORIG_TOP

L2 worker `cmwangf8r2cufle1c88sncsfp` for lead `cmw7xu1960f37p41557tefpnd`. Branch `orch/PORT-OT-GAMEPLAY` from
orch/integration `5e1069705` (includes the PORT-COLOURSEL-ICON-FLIP walk_04 reference). No queued branch merged in: no
candidate needs a row that PORT-OT-RWG-DEVSLOTS or PORT-OT-IMAGE-FMT releases. Restarted after the 09:40 LIVING-PC reboot
(the first run had no commits).

## Pre-scan

- `origtop_base_dump.py --dump-base` (the PORT-OT-HUD method): 146 rows with subsystem `gameplay` in the base runnable
  set, 7 of them `stand-in`, the rest matched or with no BINDINGS row (under a stand-in ancestor).
- BINDINGS.tsv stand-ins whose hooks.csv subsystem is `gameplay`: 9 = the 7 above + 0x004196f0, 0x0041ec00 (not in the
  base runnable set).
- 0x004210b0 is the PORT-OT-HUD-2 worker's row (cmwajfmms25dzle1ca6nsdgms), left out here.
- OT_MEASURE.tsv carries no RwGlobals or seed caveat for any of the 6 remaining candidates (none reads RwGlobals or
  nonzero original data, base dump columns `rwglobals` / `nonzero_data` = 0 / 0).
- Candidate count: **6**. Swapped: **3** (+ the exe-only callee 0x0046be10). Kept as stand-in: 3 (table below). The count is small: GAMEPLAY has 146 base rows but only 7 stand-ins, and the rest are matched already or sit under a stand-in ancestor.

## What changed (one commit)

| rva | name | before | now | TU |
|---|---|---|---|---|
| 0x00405400 | Clear639d70x3 | `PortTop_StandIn04C_00405400` (in 0x0040cfd0) | `Orig_00405400` | new Match/PortTop_00405400.cpp, **ORIGTOP-ONLY** CROSS-TARGET vs Util/PromoLoop_round53.cpp |
| 0x0040cf40 | FUN_0040cf40 | `PortTop_StandIn04C_0040cf40` (in 0x0040cf80) | `Orig_0040cf40` | new Match/PortTop_0040cf40.cpp, **ORIGTOP-ONLY** CROSS-TARGET vs Match/MatchS02_0040cf40.cpp |
| 0x004725f0 | FUN_004725f0 | `PortTop_StandIn04C_004725f0` (in 0x0040cfd0) | `Orig_004725f0` | new Match/PortTop_004725f0.cpp, **ORIGTOP-ONLY** CROSS-TARGET vs Match/MatchS11_004725f0.cpp |
| 0x0046be10 | Write46be10 | (not reached: its only knob-path caller was the 0x004725f0 stand-in) | `Orig_0046be10` | new Match/PortTop_0046be10.cpp (the existing Write46be10 in Util/PromoLoop_sessionB.cpp is .asi-only; the scanner sees no RVA header on it, so lint reports no pair) |

The group source re/match/s01/004053d0.c also holds 0x004053d0, 0x00405420 and 0x00405430 (same three globals). They
compile in the exe as `Orig_004053d0` / `Orig_00405420` / `Orig_00405430` and nothing in the exe calls them. Each has an
.asi-only body, so each is an ORIGTOP-ONLY CROSS-TARGET line too. Guarding them out with `#ifndef MATCH_PORT` was tried
first and dropped: `scripts/rva_body_scan.py` reads every `// MATCH:` line of an included source and does not evaluate
`#if`, so lint and the port column would have counted three bodies the exe does not compile.

Callee check (the BOOT / FRONTEND lesson). Every callee binds by name, no raw original address:
- 0x00405400: no callee. Stores DAT_00639d74 = 0.0f, DAT_00639d78 = 0, DAT_00639d70 = 0 (re/match/s01/004053d0.c).
- 0x0040cf40: 0x004c5a60 -> `MashedRw_TextureDestroy_004c5a60` (LibRw/RwApiBind.cpp:228, rw::Texture::destroy, null
  skipped) through an `extern "C"` forwarder with the source prototype. Both calls are guarded on a nonzero global in the
  source (re/match/s02/0040cf40.c).
- 0x004725f0: 0x0046be10 -> `Orig_0046be10` (Match/PortTop_0046be10.cpp).
- 0x0046be10: no callee. Returns 0 when param_2 >= 0x10, else two stores into the record DAT_008815a0[param_2] (stride
  0xd04, +0xbfc / +0xc7c arrays) and returns 1 (re/match/s11/0046be10.c).
Reachability. Every new body is reached only through matched callers that run only from the matched WinMain 0x00492370
(MASHED_ORIG_TOP): 0x0040cfd0 (AppDestroy 0x00402a40), and 0x0040cf80 (calls 0x0040cf40 only when
0x0041f320(i) is nonzero for some i < 4).

Source edit, re-checked with the DDK:
- re/match/s01/004053d0.c: the three `extern` globals became `#ifdef MATCH_PORT` references to their original addresses
  (the MGLOBAL form, written out because this source has no preamble). C text under the DDK is unchanged.
- `match.py --bytes`: 0x004053d0 45 B, 0x00405400 23 B, 0x00405420 11 B, 0x00405430 39 B, all `BYTE-IDENTICAL`.
  `relocheck.py`: all four OK (0 checked, the three globals unchecked).
- The other four sources are included unchanged.

Other files:
- `mashedmod/exe_sources.rsp` +4 TUs.
- Callers rebound: Match/PortTop_0040cfd0.cpp (0x004725f0, 0x00405400), Match/PortTop_0040cf80.cpp (0x0040cf40).
- Match/PortTop_StandIns04C.cpp: the three stand-in definitions removed.
- `re/tools/dual_copy_allowlist.txt` +6 `ORIGTOP-ONLY` lines (the `lint_rva_bodies.py --seed` keys).
- BINDINGS.tsv through the new `update_bindings_otgameplay.py` (runs `update_bindings_othud.py` first): 434 rows,
  matched 161 -> 165, stand-in 93 -> 90, 1 row added (0x0046be10).

Shared state. The three bodies write original-layout globals that the exe's hand code also reads at the same
addresses: 0x00639d70 / 0x00639d78 (D3d9Render/TrackRenderer.cpp, Race/CameraEntries.cpp), the per-car records at
0x008815a0 (Vehicle/VehicleStruct.h and the vehicle TUs) and the table 0x00691500 (Render/TrackHookSlots.h). The
stores are the ones the original makes from AppDestroy, on the same layout. No hand code stores a librw object in
DAT_0063ba90 / DAT_0063ba94 (grep: only a comment in Gameplay/Thresholds_ah4.cpp).

## 0x00458880: swapped, then reverted (first build)

The first build also swapped 0x00458880 (Match/PortTop_00458880.cpp: RpClumpDestroy on the 25 records at 0x0068b198,
the 0x004768c0 stand-in for the record 0x0068b968, RwTexDictionaryDestroy(DAT_0068b9ac)). On that exe (sha256
`635784810e08e303378984b11e945f59f0950bd31ad5013041037cdd7738956b`):
- knob-ON boot (no nav): rc=0. The teardown showed exactly the matched sequence: 25 `RpClumpDestroy(0x00000000)`
  after the 0x00412010 stand-in line, then the 0x004768c0 stand-in, then `RwTexDictionaryDestroy(0x00000000)`.
- 12 captures: both ON runs exited `rc=3221225477` (0xC0000005), the 12 hashes still equal in all four runs. Each ON log
  ends at `RpClumpDestroy(0x00F30C50) -> rw::Clump::destroy` right after the 0x00412010 stand-in line
  (D:/MashedWT/PORT-OT-GAMEPLAY_data/nav1_try1/r02_ON_pindflt.log:592).

Cause: the pool 0x0068b198 (25 x 0x50, count 0x0068b9a8) is also the exe's own pickup pool. The hand code
D3d9Render/PickupIconCube.cpp:27 and Gameplay/PickupTick.cpp use it on the original layout, and after the nav-demo walk
entry 0 holds 0x00F30C50, which is not a librw clump. The matched 0x00458880 hands it to librw. 0x00458880 went back to
its stand-in, and every check below is on the second build.

## GAMEPLAY rows left, with the reason

| rva | name | reason |
|---|---|---|
| 0x00458880 | FUN_00458880 | the pool 0x0068b198 it destroys is shared with the exe's hand pickup code, which stores objects that are not librw clumps there (section above). A swap needs that pool split first, or the hand code moved onto librw clumps. |
| 0x00412010 | FUN_00412010 | reads `*(DAT_0063bb38[i].m_obj + 0x3c)` with no null check (re/match/s02/00412010.c). The records are written by 0x00411f30, 0x00412100 and 0x00412620, none bound on this path, so the read would be at 0x3c. It runs on every knob-ON teardown (0x0045b930). |
| 0x004593b0 | FUN_004593b0 | calls 0x004c5770 (RwGlobals +0x11c free-list free with the module offset at 0x007d4028), which OT_MEASURE marks OK only with `MASHED_ORIG_TOP_RWMOD=1` (RESULT_PORT-OT-RWG-MODULES.md, released row). Its argument records at 0x0068ba30 are written only by 0x00459290, not bound on this path, so it would also free a zero handle. |
| 0x004196f0 | FUN_004196f0 | not in the base runnable set |
| 0x0041ec00 | FUN_0041ec00 | not in the base runnable set |
| 0x004210b0 | FUN_004210b0 | PORT-OT-HUD-2's row |

## Acceptance

Machine rules: the first build waited 811 s for merge train `20261110_094719` (PRIORITY-HOLD pids 5000 / 9976, then
its 13-track SOLO) to clear. A gate script read `%TEMP%\mashed_machine_priolinger_*.lock` (held + live pid) and
`mashed_lock.py status` (PRIORITY-HOLD / SOLO) and started `build.bat` only after two clear reads. The second build
went through the same gate (clear after 16 s). Every launch ran as `test-slot bash -c 'py -3.12
<orch-wt>/re/orchestrator/mashed_lock.py run --label PORT-OT-GAMEPLAY -- ...'`. Private root
`D:/MashedWT/PORT-OT-GAMEPLAY_root` (`original` = junction to `D:/MashedWT/PORT-TOP-04J_rootK/original`, as PORT-OT-HUD).

All rows below are on the second build.

| check | result |
|---|---|
| syntax check before the build | `cl /Zs /EHa /W3 /O2 /DMASHED_STANDALONE` on the new and edited TUs: no error |
| build (`MASHED_NO_DEPLOY=1`) | `=== Build OK ===`, `[skip] MASHED_NO_DEPLOY set: mashed_re_dev.asi NOT deployed to original\`. mashed_re.exe sha256 `bdd23bb9959e012ef534b8dfd45a2604c5793eab8ef69a0d1d188f3a747d2306` |
| knob-ON boot (`knob_boot.py knob_on_1 <root> 300 MASHED_ORIG_TOP=1 MASHED_DET_FRAMES=3000`) | `knob_on_1: exited rc=0 (0x00000000) after 52.3 s`. knob_on_1.log:477-479 the teardown stand-ins left: 0x00412010, 0x00458880, 0x004593b0. No stand-in line for 0x00405400 / 0x0040cf40 / 0x004725f0 (none was logged before either: 0x0040cfd0 and the 0x0040cf80 branch that calls 0x0040cf40 do not run in this boot) |
| 12 captures, OFF / ON / OFF / ON, pin default (`CAPFLAKE_DATA=<data> CAPFLAKE_ROOT=<root> capflake_runs.py nav1 OFF@dflt ON@dflt OFF@dflt ON@dflt`) | 4/4 `rc=0 pinned pos=55 hash=28538c74 class=E eqE=12`. The 12 hashes are equal in all four runs, knob ON and OFF. walk_04_colourselect.bmp `ba34030d1b7e2fe4` in all four (the reference after the ICON flip) |
| G-CTRL (H_DEF) | `8284be46/67931834/31fe043e` = merge_train.json `gctrl.ref` |
| B (K0 / K1 / K26) | `1d5bf4f9` x3 = `b.ref` |
| rva-lint | `D4 burn-down=2  ORIGTOP-ONLY=14 (class deleted at the ORIG_TOP flip; 13 marked TU(s))`, `OK -- no NEW duplicate bodies (16 known)`. Before: burn-down 2, ORIGTOP-ONLY 8 |
| match.py --bytes on the edited source | 0x004053d0 / 0x00405400 / 0x00405420 / 0x00405430 BYTE-IDENTICAL, relocheck OK |

`A4_OUT` was created before `run_arms.py` (the PORT-OT-HUD harness note).

### Port column deltas

`backfill_exe_file.py` (`rows given an exe_file: 7`, `old value changed: 0`), then `backfill_port.py`:
`none=5170 body=548 called=437`, 10 rows changed.

| rva | port |
|---|---|
| 0x00405400, 0x0040cf40, 0x004725f0, 0x0046be10 | none -> called |
| 0x004053d0, 0x00405420, 0x00405430 | none -> body (compiled from the group source, no caller in the exe) |
| 0x004d86d0 RwStrupr, 0x004d8700 RwStrlwr, 0x004d8750 RwStrrchr | body -> called. Drift, not this change: none of the new TUs calls them. orch/integration's hooks.csv cells for them predate the build of its current tree |

### Raw files (D:/MashedWT/PORT-OT-GAMEPLAY_data, not committed)

| file | sha256 |
|---|---|
| build.log (second build) | b94f749c99b9541aae5f211f03496ec4b58fce3eca93b87577c2d4be4140bf87 |
| knob_on_1.log | 9e1b6076118c862e2e1ff2b3f23f83dc4bd0f57e2d989d96dea620f948efb38c |
| knob_on_1.stdout | abfa1bfb11b07c75cb45a5ee8335bf5529f1bd47bb686b042d7f57887cb47184 |
| runs.tsv | 57330649dd80ec34fbcbb541d4200d97cfe6376ecad59ca71d2e3a696bc0f656 |
| capflake.stdout | 1007d454afff5c0e3dcfc223ee416c8097b1f7ebf4fe894efe7285924133dbd9 |
| nav1/r01_OFF_pindflt.log | d7f9c36fa1786dc2ca90832b9eeeeb9d0f3fb8af0fecc39f9224951c29b53ea3 |
| nav1/r02_ON_pindflt.log | 12a8764b97abb21f4dccfffac93bf87a25a798863fb87a219590bbb1f7482032 |
| nav1/r03_OFF_pindflt.log | 1b7832864e3dd3ccb636c65f3da473eb85e59d4ad01b8cf27c9441b909f7bfd7 |
| nav1/r04_ON_pindflt.log | e527fdc27ada9ed0f68a466db0dc80f4174df9585626b06dc133ef77506a6da2 |
| arms.stdout | ee1ee2868f9d900f9c537d1eae17c786d4ec7a6d85d2db82d3a04c92f5b1559b |
| arms/H_DEF.step.csv | 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198 |
| arms/H_DEF.gates.csv | 6793183492e0c0822570ad71254f9fdf27568b77c81a25c56347f1616863d58c |
| arms/H_DEF.ptrace.log | 31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3 |
| arms/B_DEF_K0.step.csv, B_DEF_K1.step.csv, B_DEF_K26.step.csv | 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 (all three) |
| try1_knob_on_1.log (first build, 0x00458880 swapped) | c007a7038086f7c6dd25d5491fd462f0c0cf5d108e04f5290b69721d8f9ce386 |
| try1_knob_on_2.log (first build) | 87f686a76026f7fcc364913ee70920509b710715477dd08b8eb4cd0698601913 |
| nav1_try1/r02_ON_pindflt.log (first build, AV) | a61a62e32b98dafedbbc358f3434730658c1dea422b16b7cb426abe085b2005e |
| nav1_try1/r04_ON_pindflt.log (first build, AV) | 35775045565bd9054e322dde408612c6551d43e400b9b0ea983bedfdf29435aa |

## Next

- No GAMEPLAY stand-in in the base runnable set is left that swaps without other work. The three left need:
  0x00458880 a split of the 0x0068b198 pool between the hand pickup code and the original-top path, 0x00412010 the
  0x0063bb38 record builders (0x00411f30 / 0x00412100 / 0x00412620) on the path, 0x004593b0 RWMOD plus the 0x0068ba30
  builder 0x00459290.
- Lesson for the other subsystem rows: a matched teardown body that only frees handles can still AV under
  MASHED_ORIG_TOP when the exe's hand code shares its original-layout pool. The knob-ON boot without nav did not show
  it (the pool was empty), only the nav-demo ON captures did. Grep the pool address over mashedmod/src before a swap.
