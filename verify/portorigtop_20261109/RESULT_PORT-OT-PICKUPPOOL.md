# RESULT PORT-OT-PICKUPPOOL (2026-11-10): GREEN, 0x00458880 and 0x004593b0 on matched bodies under MASHED_ORIG_TOP, 0x00412010 kept

L2 worker `cmwawz0580eelsc1b69zcelrp` for lead `cmw7xu1960f37p41557tefpnd`. Branch `orch/PORT-OT-PICKUPPOOL` from
orch/integration `7e6eeff6c`. Code commit `22c2fac1f`. No queued branch merged in.

## Why the first try crashed, and a second blocker it did not show

The PORT-OT-GAMEPLAY first try (RESULT_PORT-OT-GAMEPLAY.md section "0x00458880: swapped, then reverted") AV'd in both
nav ON runs at `RpClumpDestroy(0x00F30C50)`. The pool 0x0068b198 (25 x 0x50) has two writers in the exe:
- the original's builder 0x004587a0 (re/match/s09/004587a0.c) stores 25 IconCube clumps at +0x00. Its only caller is
  0x0045bae0 (xref), a stand-in on this path (`PortTop_StandIn04E_0045bae0`, Match/PortTop_00402750_Pieces.cpp:221).
- the hand pickup code: Gameplay/PickupTick.cpp `BindAll` (called from `Pickups_FrameTick` and `PickupsOrig_OnSpawn`)
  writes one exe fake object (`s_obj[i]`, not a librw clump) into every entry's +0x00 word.

So on this path the only owner of +0x00 is the hand code, and the matched teardown handed its objects to librw.

The second blocker came in after that try. PORT-OT-HUD-2 bound 0x004768c0 to its matched body. 0x00458880 calls it on
the record 0x0068b968, and the matched body reads `*(*(p + 4) + 4)` with no null check (re/match/s12/004768c0.c). That
record is built only by 0x004770c0 from 0x004587a0 (0x004587c5..0x004587d2), under the same stand-in. Nothing else in
mashedmod/src writes 0x0068b968, 0x0068b9ac or 0x0068b9b8..0x0068bb90 (grep). All of them are BSS in MASHED.exe (.data
at 0x005ea000, raw size 0x4d000), so on this path they are zero. A plain swap would now AV at address 0x00000004 even
in a boot without nav.

## What changed (commit 22c2fac1f)

| rva | before | now | TU |
|---|---|---|---|
| 0x00458880 | `PortTop_StandIn04C_00458880` | `Orig_00458880` via `PortTop_0045b930_Call00458880` | new Match/PortTop_00458880.cpp = re/match/s09/00458880.c |
| 0x004593b0 | `PortTop_StandIn04C_004593b0` | `Orig_004593b0` with MASHED_ORIG_TOP_RWMOD=1, the stand-in without it, via `PortTop_0045b930_Call004593b0` | new Match/PortTop_004593b0.cpp = re/match/s09/004593b0.c |
| 0x004c5770 | (not reached) | `Orig_004c5770` | new Match/PortTop_004c5770.cpp = re/match/s19/004c5770.c |
| 0x004768c0 from 0x00458880 / 0x004593b0 | (stand-in callers) | record selector `PortTop_PKP_Call004768c0` | Match/PortTop_00458880.cpp |

Single owner for the pool, as built. The hand code stays the only owner of the +0x00 words while the game runs. Before
the matched teardown reads them, the 0x0045b930 call-site forwarder calls the new `PickupsOrig_ReleasePool`
(Gameplay/PickupTick.cpp). It sets each +0x00 word that holds the hand code's own `s_obj[i]` back to 0 and counts any
other non-zero word as foreign. With no foreign word, the matched `Orig_00458880` runs on the pool as the matched
builders left it: zero, because 0x004587a0 never ran. With a foreign word, the forwarder logs it and does nothing (the
stand-in behaviour). No run hit the foreign case. The hand code never sees a librw object and the matched body never
sees a hand object. The default path never calls the release (only Match/PortTop_0045b930.cpp does, and that TU is
reached only from the matched WinMain 0x00492370).

Callees, all bound by name, no raw-address call:
- 0x00458880: 0x004e6e00 -> `MashedRw_ClumpDestroy_004e6e00`, 0x004c5930 -> `MashedRw_TexDictionaryDestroy_004c5930`
  (LibRw/RwApiBind.cpp, both skip null), 0x004768c0 -> record selector.
- 0x004593b0: 0x004e6e00 and 0x004c5930 as above, 0x004c5770 -> `Orig_004c5770` (calls RwGlobals +0x11c, null without
  RWMOD; with RWMOD it is `RwGlobalsFreeListFree`, which returns 0 and frees nothing for a pointer it did not hand out,
  RESULT_PORT-OT-RWG-MODULES.md), 0x004768c0 -> record selector.
- Record selector: `Orig_004768c0` when the record's +0x04 word is non-zero, else it logs and skips. This is the
  0x00556e40 handle-selector shape (Match/PortTop_00427620.cpp). It is also the 0x004210f0 history: before HUD-2
  swapped the builder 0x004210b0, that matched teardown called the 0x004768c0 stand-in because its record was zero.

Source edits. Both use C++-only guards, and the DDK C text is unchanged:
- re/match/s09/004593b0.c: `void FUN_004768c0(void *p0)` under `#ifdef MATCH_PORT`. The C declaration `(int p0)` stays.
  C++ rejects a pointer argument there.
- re/match/s19/004c5770.c: `typedef int code(int, undefined4)` under `#ifdef MATCH_PORT`. C keeps `typedef int code()`.
  In C++ an empty list means no arguments.

| source | match.py --bytes | relocheck |
|---|---|---|
| re/match/s09/00458880.c | 100%, 55 B BYTE-IDENTICAL | OK 7/7 |
| re/match/s09/004593b0.c | 100%, 68 B BYTE-IDENTICAL | OK 7/7 |
| re/match/s19/004c5770.c | 100%, 35 B BYTE-IDENTICAL | OK 2/2 |

Other files: `mashedmod/exe_sources.rsp` +3 TUs. Match/PortTop_StandIns04C.cpp drops the unused 0x00458880 stand-in
(the 0x004593b0 one stays for the RWMOD-off arm). No other body of 0x00458880, 0x004593b0 or 0x004c5770 exists in either
target, so `dual_copy_allowlist.txt` is unchanged. The first build failed rva-lint with a false DUP-IN-TARGET for
0x004768c0, because a comment line starting `// 0x004768c0` sat over the selector (the scanner's call-through rule). The
comment was reworded, and the second build is the one measured.

## Rows kept, with the reason

| rva | reason |
|---|---|
| 0x00412010 | Its own body reads `*(DAT_0063bb38[i].m_obj + 0x3c)` with no null check (re/match/s02/00412010.c), so it cannot run on unbuilt records, and no callee selector can help. The six records at 0x0063bb38 are BSS. Their writers are 0x00411f30 (caller 0x0045bae0, stand-in), 0x00412100 (caller 0x0040bde0) and 0x00412620 (caller 0x0040ce00). Neither caller is in BINDINGS.tsv, so neither is on this path, and no exe code writes the range. The first read would be at 0x0000003c. It needs 0x0045bae0 or one of those builders on the path first. |

0x004593b0 RWMOD off: the stand-in runs, because 0x004c5770's RwGlobals +0x11c slot is null without the knob.

## Acceptance

Machine rules: both builds ran only after `mashed_lock.py status` read `free` and no
`%TEMP%\mashed_machine_priolinger_*.lock` existed. Every launch ran as `test-slot bash -c 'py -3.12
<orch-wt>/re/orchestrator/mashed_lock.py run --label PORT-OT-PICKUPPOOL[-gates] -- ...'` (test-slot outside, mashed_lock
inside). Private roots `D:/MashedWT/PORT-OT-PICKUPPOOL_root` (boots, captures) and `_rootA` (gates), each with
`original` = junction to `D:/MashedWT/PORT-TOP-04J_rootK/original`. `A4_OUT` = `D:/MashedWT/PORT-OT-PICKUPPOOL_data/arms`,
created beforehand.

| check | result |
|---|---|
| syntax check | `cl /Zs /EHa /W3 /O2 /std:c++17 /DMASHED_STANDALONE` on the 3 new and 3 edited TUs: no error |
| build (`MASHED_NO_DEPLOY=1`, tree = `22c2fac1f`) | `=== Build OK ===`, `[skip] MASHED_NO_DEPLOY set: mashed_re_dev.asi NOT deployed to original\`. mashed_re.exe sha256 `b2a29a6ecd576a461201e08324e3cfc37f872541ab41668f06ba3f2a0f5f1a29` |
| knob-ON boot, RWMOD off (`knob_boot.py knob_on_1 <root> 300 MASHED_ORIG_TOP=1 MASHED_DET_FRAMES=3000`) | `exited rc=0 (0x00000000) after 52.2 s`. :434 `rva 0x00458880: matched body after the hand pickup code released its binds (released, foreign) (0, 0)` (no nav, so the pool was never bound), then 25 `RpClumpDestroy(0x00000000)`, :460 `rva 0x004768c0(0x0068B968): stand-in, not run (record +0x04 is 0, builder not run)`, `RwTexDictionaryDestroy(0x00000000)`. :462 `rva 0x004593b0: stand-in (RWMOD off)`. :797 `WinMain 0x00492370 returned 0 after 3000 frames` |
| knob-ON boot, RWMOD on (same + `MASHED_ORIG_TOP_RWMOD=1`) | `exited rc=0 (0x00000000) after 47.0 s`. :495 0x00458880 matched (0, 0). :523 `rva 0x004593b0: matched body (RWMOD on)`. :521 / :528 the selector skips 0x0068B968 and 0x0068B9B8. 29 `RpClumpDestroy(0x00000000)` in :492-531 = 25 + 4. :861 `WinMain 0x00492370 returned 0 after 3000 frames` |
| 12 captures, RWMOD off (`CAPFLAKE_DATA=<data> CAPFLAKE_ROOT=<root> capflake_runs.py nav1 OFF@dflt ON@dflt OFF@dflt ON@dflt`) | 4/4 `rc=0 pinned pos=55 hash=28538c74 class=E eqE=12`. Both ON logs: `0x00458880: matched body after the hand pickup code released its binds (released, foreign) (25, 0)`. This is the state that AV'd in the GAMEPLAY first try. `0x004593b0: stand-in (RWMOD off)` |
| 12 captures, RWMOD on (arm `nav_rwmod`, `MASHED_ORIG_TOP_RWMOD=1` in every run's env) | 4/4 `rc=0 pinned pos=55 hash=28538c74 class=E eqE=12`. Both ON logs: 0x00458880 matched (25, 0), `0x004593b0: matched body (RWMOD on)`, both selector skips |
| walk_04 | `ba34030d1b7e2fe4` in all 8 runs (runs.tsv, 8 matching rows). No other capture moved (eqE=12 in every run) |
| G-CTRL (H_DEF) | `8284be46/67931834/31fe043e` = merge_train.json `gctrl.ref` |
| B (K0 / K1 / K26) | `1d5bf4f9` x3 = `b.ref` |
| rva-lint (`scripts/lint_rva_bodies.py`) | `D4 burn-down=2  ORIGTOP-ONLY=26 (class deleted at the ORIG_TOP flip; 27 marked TU(s))`, `OK -- no NEW duplicate bodies (28 known)`. Same counts as orch/integration `7e6eeff6c` |

## BINDINGS and the port column

`verify/portorigtop_20261109/update_bindings_otpickuppool.py` runs `update_bindings_othud2.py` first (the chain tip on
`7e6eeff6c`, which runs GAMEPLAY, VEHICLE and OTHER, and each of those runs the rest).
- Checked first: the HUD-2 tip alone reproduces the committed BINDINGS.tsv byte for byte (`git diff` empty, 451 rows).
- Then it writes 5 rows: 0x00458880 and 0x004593b0 stand-in -> matched, 0x004c5770 added (matched, caller 0x004593b0).
  The 0x004768c0 note gains the selector callers. The 0x00412010 note gains the reason above (still stand-in). Result:
  452 rows, matched 198 -> 201, stand-in 70 -> 68.

`backfill_exe_file.py` (`old value changed: 0`), then `backfill_port.py` on this build: `none=5128 body=552 called=475`, 3
rows changed, all this step's: 0x00458880, 0x004593b0 and 0x004c5770 none -> called. No drift row.

## Raw files (D:/MashedWT/PORT-OT-PICKUPPOOL_data, not committed)

| file | sha256 |
|---|---|
| build.log (second build) | d971a73b3b2b41ec93e16519cebc1a297067b6f2e3d53225102a73434fdd9088 |
| mashed_re.exe (copy of the measured build) | b2a29a6ecd576a461201e08324e3cfc37f872541ab41668f06ba3f2a0f5f1a29 |
| knob_on_1.log | 03000a5c8971ec148ea55b804e4355efa74723194acfb383766dfc79ebbbcb65 |
| knob_on_1.stdout | 038809257db6da2c6422fd489da5d47060d8db310c0aa8efa82e7154826fa2fc |
| knob_on_rwmod.log | cb1865b9707ced084b965f78f6e5aa707d95e94a9f5d63f18ff576197f0be713 |
| knob_on_rwmod.stdout | 05c42d01ad6da0ce62758b56cd937293aad5f9fb2ad7f961f4abb6303c3b5661 |
| runs.tsv | f560646e9825f27b38517ec0c4d851cd17931fc8944763b7fbaa1d1204796438 |
| capflake_nav1.stdout | 1d30ff3d4004d50f438a0ac6340552deb553f5b070b7d6a427b8b708a8627d61 |
| capflake_rwmod.stdout | c6922ce8b0d2af83f891ac646eeb6cab44ae11839b282893d3462f39ea42546f |
| nav1/r01_OFF_pindflt.log | 75966b288a576ec7d90a7ae586ec3f38781e96fdafaba93e43acc1886d60938a |
| nav1/r02_ON_pindflt.log | 34e547f2c10ae9fc0a6d1d08000bcf60f8e98abb4f0232eec1f8be1d4b3c4658 |
| nav1/r03_OFF_pindflt.log | 5ad7763ec5f7c0193a18c1814582a43c285795e387f70915703887c7ed6c70b5 |
| nav1/r04_ON_pindflt.log | c7d5b2ce3d39257661900f80434fa7a29c5c0d703d1d236fef88be54f1ffcc39 |
| nav_rwmod/r01_OFF_pindflt.log | 22a587342869365af2d35e3694db44f71faae767bc4fb8e6104143335ab4b15d |
| nav_rwmod/r02_ON_pindflt.log | 12bcafd42e9d48e33fa28cb8c1b13f32a688d78171199421e4e18afe63720962 |
| nav_rwmod/r03_OFF_pindflt.log | 89c3be30f6e1866430d5ef154129adcf1b8a57b4f6885d73efdedf27835398a8 |
| nav_rwmod/r04_ON_pindflt.log | f08864be51045d87aeb5c6bcd26f5b5667d2bb214516d95472cc088df39d7580 |
| arms.stdout | 5852358a8d1556af83c80c3391ccf5914f9785514af2d0d7a5a4f935cf586260 |
| arms/H_DEF.step.csv | 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198 |
| arms/H_DEF.gates.csv | 6793183492e0c0822570ad71254f9fdf27568b77c81a25c56347f1616863d58c |
| arms/H_DEF.ptrace.log | 31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3 |
| arms/B_DEF_K0.step.csv, B_DEF_K1.step.csv, B_DEF_K26.step.csv | 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 (all three) |

## Next

- These two swaps compile the matched teardowns and run them. They do not yet tear down real objects: on this path the
  pickup-family records stay empty until 0x0045bae0 runs its matched body. 0x0045bae0 calls 0x00459290, 0x004587a0,
  0x00453f30, `powerups_all.lua` (0x0047b9b0), 0x00411f30, 0x004039c0 and the 0x005f9998 init table. It is not in the
  base runnable set (RESULT_PORT-OT-VEHICLE.md). Once it is, then:
  - the two record-selector skips turn into matched 0x004768c0 calls by themselves
  - 0x00412010 becomes swappable
  - the hand pickup code can no longer store fake objects over 0x004587a0's clumps, so the pool's owner has to be
    settled again (the release would have to leave librw clumps alone, or the hand tick has to move onto them)
- Lesson for teardown swaps: look up the builder for every record a matched teardown reads, and also for every record it
  passes to an already-matched callee. A callee swapped later (here 0x004768c0 by HUD-2) can turn a teardown that used
  to boot clean into an AV.
