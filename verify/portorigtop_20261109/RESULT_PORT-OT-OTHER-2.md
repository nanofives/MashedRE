# RESULT PORT-OT-OTHER-2 + PORT-OT-STANDIN-CLEANUP (2026-11-10): GREEN, 0x0048a830 on its matched body under MASHED_ORIG_TOP, one dead stand-in deleted, 0x0048cf70 / 0x0048ffd0 kept with the reason

L2 worker `cmwawz2jt0eersc1boxm5kd86` for lead `cmw7xu1960f37p41557tefpnd`, L-111 (a) step 1. Branch
`orch/PORT-OT-OTHER-2` from orch/integration `7e6eeff6c` (PORT-OT-HUD-2 merged). One code commit carries both rows.

## Yield, plainly

- **1 swap:** 0x0048a830 (port none -> called).
- **1 deletion:** `PortTop_StandIn04C_004114e0`, which has had no reference since PORT-OT-VEHICLE.
- **2 kept:** 0x0048cf70 and 0x0048ffd0. Neither record set can be built on the knob path today (below).
- **0x004114e0 cannot be exercised:** its gate flag is never non-zero in mashed_re.exe, whatever the scenario (below).

## Goal 1: 0x0048a830 (PORT-OT-OTHER-2)

| rva | before | now | TU |
|---|---|---|---|
| 0x0048a830 | `PortTop_StandIn04G_0048a830` (in 0x0040bbb0) | `Orig_0048a830` | new Match/PortTop_0048a830.cpp, **ORIGTOP-ONLY** CROSS-TARGET vs .asi-only Match/MatchS14_0048a830.cpp |

- Source re/match/s14/0048a830.c, unedited: `DAT_0071fa34 = 0`, then `FUN_004b6480(&DAT_007151f0, 0x200)`.
  `match.py --bytes`: 29 B BYTE-IDENTICAL, relocheck OK 3/3. No `#ifndef MATCH_PORT` guard was needed.
- Callee by name, no raw address: 0x004b6480 -> `Orig_004b6480` (PORT-OT-HUD-2's exe-only Match/PortTop_004b6480.cpp). One
  forwarder, `PortTop_0048a830_Call004b6480(unsigned char*, unsigned int)`, because the source declares `(byte*, uint)` and the
  exe copy is `(unsigned char*, int)`. The HUD-2 0x004210b0 pattern.
- Caller: the matched 0x0040bbb0 calls `FUN_0048a830(0)` (re/match/s02/0040bbb0.c:53 declares `(int)`, :95 calls it). The
  body takes no argument, cdecl, and the caller pops. Match/PortTop_0040bbb0.cpp: one `#undef` / `#define` pair in the rebind block.
- Shared state: no exe TU names 0x0071fa34 or 0x007151f0. The other sources that name them (re/match/s14/0048a850.c,
  0048ade0.c, re/match/racepath/0048ab70.c) are in .asi-only TUs or no TU at all. 0x0071fa34 is in .data BSS.
- `// RVA-LINT: ORIGTOP-ONLY` in the TU, plus one `ORIGTOP-ONLY CROSS-TARGET:0048a830` allowlist line.

Executed (knob_on_1.log against PORT-OT-HUD-2's knob_on_1.log): HUD-2's line 237
`rva 0x0048a830: stand-in, not run (zero fill ...)` is gone, and no other line changed. The rest of the diff is blank
lines and heap-total text. The body has no log line of its own. It is a straight-line call in 0x0040bbb0, the same evidence
class as 0x0048a460 in PORT-OT-OTHER. Both nav ON logs also have no 0x0048a830 line.

## Goal 2: 0x0048cf70 and 0x0048ffd0 kept (PORT-OT-OTHER-2)

- **0x0048cf70** (re/match/s15/0048cf70.c) calls 0x004768c0 on the 17 records 0x00722470..0x007228b0. The only writer of those
  records is 0x0048cee0 (re/match/s14/0048cee0.c: `FUN_004770c0(&DAT_00722470[i], 0x817, 0x100, tex)`), and its only caller is
  0x0048e820. 0x0048e820 has no matched body: re/match/s15/0048e820.c is 85.5% (register-pair swap residual,
  verify/matchs15_20261102/results.tsv:17), hooks.csv C2, and it is `PortTop_StandIn04G_0048e820` in 0x0040bbb0 (knob_on_1.log:239).
  So on the knob path the records are 0, and 0x004768c0's first read is `*(0 + 4)`. 0x0048cee0.c carries a
  `Standin_0048e820` caller, but it is not 0x0048e820's body. It skips the 17 x 256 particle and parameter writes.
- **0x0048ffd0** (re/match/s15/0048ffd0.c), for the 30 0x50-byte entries from 0x0086a4a4, calls `FUN_004e8090(entry[2], 0)`,
  which reads `*param_1` with no null check. The only writer of the table is 0x0048ff20 (re/match/s15/0048ff20.c, 100%, a
  stand-in, knob_on_1.log:219). Its swap is blocked by its `RpClumpForAllAtomics` callback **0x0048fed0**, which has no
  matched source and no hooks.csv row (it is `LAB_0048fed0` in the C1 plate, U-4034), and by 0x004b65a0, whose only body is the
  .asi-only Util/PromoLoop_round30.cpp. The rest of the chain is bound: 0x004b6520, 0x004b65b0 (matched), 0x004b3b70,
  0x004b3e40 (matched, stream calls 0x004cc230 / 0x004cc5e0 / 0x004e7420 / 0x004cc160 bound), 0x004e66d0 and 0x0048fef0 (matched).
- BINDINGS: both rows stay `stand-in`, and their notes now give this reason.

## Goal 3: stand-in deleted (PORT-OT-STANDIN-CLEANUP)

`PortTop_StandIn04C_004114e0` (and its comment) is removed from Match/PortTop_StandIns04C.cpp. Before the edit, a grep over
mashedmod/src, the rsp files, verify/portorigtop_20261109/*.py and re/tools/*.txt found that definition as its only
occurrence. Match/PortTop_0040cfd0.cpp binds `FUN_004114e0 Orig_004114e0` (since PORT-OT-VEHICLE). The build links, so
nothing referenced it. The generator verify/porttop_20261108/gen_top04c_tus.py:105 still lists the RVA. It is history and was
not re-run. The BINDINGS row 0x004114e0 was already `matched` and is unchanged.

## Goal 4: updater chain (PORT-OT-STANDIN-CLEANUP)

Confirmed. verify/portorigtop_20261109/update_bindings_othud2.py:56 runs `update_bindings_otgameplay.py`, then
`update_bindings_otvehicle.py`, then `update_bindings_otother.py`. So VEHICLE runs after GAMEPLAY, and no change was needed.
Before the new rows were added, othud2 alone rewrote BINDINGS.tsv byte for byte (no diff).
The new `update_bindings_otother2.py` runs othud2, then writes 3 rows: 451 rows, matched 198 -> 199, stand-in 70 -> 69.

## Goal 5: exercising 0x004114e0 (PORT-OT-STANDIN-CLEANUP): no path exists today

0x004114e0's only knob-path caller is 0x0040cfd0. AppDestroy calls 0x0040cfd0 only when `FUN_00426ba0() != 0`
(re/match/s01/raw/00402a40.c:93-97), and 0x00426ba0 returns `DAT_0066d704` (re/match/s06/00426ba0.c). Static evidence that
the flag is 0 in every mashed_re.exe run:
- 0x0066d704 is in .data BSS: section VA 0x005ea000, raw size 0x4d000, offset 0x83704 (read from MASHED.exe.unpatched). So
  it is 0 at load.
- The only sources in the tree that store to it are re/match/s06/00426e10.c:150 (`= 1`, the track loader) and
  re/match/s06/00426b40.c:60 (`= 0`). 00426e10.c is compiled only by the .asi-only Match/MatchS06_00426e10.cpp. No exe TU
  writes 0x0066d704. The other mashedmod hits are comments, plus the port's own stand-in field `RaceFx` `world_up`
  (LibRw/RwRaceFx.h:131).
- So no frontend nav, race scenario or AppDestroy order sets it. The knob logs have no 0x0040cfd0-tree line.

To make it run, 0x00426e10 (the track load into 0x0066d704) needs a body on the knob path. That is a race-path port, not
part of this step.

## Acceptance

Machine rules: before build.bat, no `%TEMP%\mashed_machine_priolinger_*.lock` existed, and `mashed_lock.py status` showed
`mashed_machine` free. Every launch ran as `test-slot bash -c 'py -3.12 <orch-wt>/re/orchestrator/mashed_lock.py run --label PORT-OT-OTHER-2 -- ...'`.
Private roots `D:/MashedWT/PORT-OT-OTHER-2_root` (boot, captures) and `_rootA` (gates), each with `original` = junction to
`D:/MashedWT/PORT-TOP-04J_rootK/original`. `A4_OUT` = `D:/MashedWT/PORT-OT-OTHER-2_data/arms`, created beforehand.

| check | result |
|---|---|
| build (`MASHED_NO_DEPLOY=1`) | `=== Build OK ===`, `[skip] MASHED_NO_DEPLOY set`. mashed_re.exe sha256 `978cf2ad29c6874b2ecff3453a9de66a8392795bb511695af22ebcd9c97ba61f` |
| knob-ON boot (`knob_boot.py knob_on_1 <root> 300 MASHED_ORIG_TOP=1 MASHED_DET_FRAMES=3000`) | `exited rc=0 (0x00000000) after 58.8 s`. Last line `WinMain 0x00492370 returned 0 after 3000 frames`. 25 `RpPTankAtomicCreate`, 25 `RpAtomicDestroy(PTank record ...)` (= HUD-2) |
| 12 captures, OFF / ON / OFF / ON (`capflake_runs.py nav1 OFF@dflt ON@dflt OFF@dflt ON@dflt`) | 4/4 `rc=0 pinned pos=55 hash=28538c74 class=E eqE=12`. The 12 hashes are equal in all four runs. walk_04 `ba34030d1b7e2fe4` in all four. No other capture moved. Both ON logs: 25 creates, 25 destroys, AppDestroy reached |
| G-CTRL (H_DEF) | `8284be46/67931834/31fe043e` = merge_train.json `gctrl.ref` |
| B (K0 / K1 / K26) | `1d5bf4f9` x3 = `b.ref` |
| rva-lint | `D4 burn-down=2  ORIGTOP-ONLY=27 (class deleted at the ORIG_TOP flip; 28 marked TU(s))`, `OK -- no NEW duplicate bodies (29 known)`. Base 7e6eeff6c: 26 |

### Port column (on this build)

`scripts/backfill_exe_file.py` (`rows given an exe_file: 1`, `old value changed: 0`), then `scripts/backfill_port.py`:
`none=5130 body=552 called=473`, 1 row changed: **0x0048a830 none -> called**. No drift row (HUD-2: 5131 / 552 / 472).

## Raw files (D:/MashedWT/PORT-OT-OTHER-2_data, not committed)

| file | sha256 |
|---|---|
| build.log (log/otother2/build.log in the worktree) | 6c78df8060c4803d0a974e7b85e039241468508ad1dfc3d2810080084a1e499f |
| knob_on_1.log | 8f8e335c41f955d0205f224c4575c953c3caece83eb7192cf65441e3a7e01ce2 |
| knob_on_1.stdout | efd75524c6d0100f1b97cde5be6c62a8a380e96e40cf15179c6ab07fedc8c874 |
| runs.tsv | 57330649dd80ec34fbcbb541d4200d97cfe6376ecad59ca71d2e3a696bc0f656 |
| cap_nav1.out | af5b35e6adc783f9d57f4a69cba319f7539cb3edcaf325db5f6c5d83d233e6ab |
| nav1/r01_OFF_pindflt.log | ddde25c7e1537722f86532529a1cd0ff56bfec674dba0f5d08a15d2f6819e3b5 |
| nav1/r02_ON_pindflt.log | 3ca4d8474f5a110ca4596564cf19883f64fe4296e6b90560c6fb4cd64a381cab |
| nav1/r03_OFF_pindflt.log | d45752ab95ced7cb8a575a50b91c9c3a5861a862e21343577b7180d489a92706 |
| nav1/r04_ON_pindflt.log | 7ed8e95865c9613b812c7bc5aff7a768a36f31838cd03d1f387955ea9d00150a |
| gates_run.log | 7e6657738427b7291694084a190d186dbd5e8a115aa247f34bcef9e1f752feee |
| arms/H_DEF.step.csv / gates.csv / ptrace.log | 8284be46.. / 67931834.. / 31fe043e.. |
| arms/B_DEF_K0, K1, K26 .step.csv | 1d5bf4f9.. (all three) |

## Next

- 0x0048cf70: needs a matched 0x0048e820 (85.5%, register-pair residual) or an L-n decision to run its C2 body on the knob path.
- 0x0048ffd0: needs 0x0048ff20, which needs a source for the callback 0x0048fed0 (not a Ghidra function yet) and an exe-only
  copy of 0x004b65a0.
- 0x004114e0 / 0x0040cfd0 tree: runs only once 0x00426e10 (track load) writes 0x0066d704 on the knob path.
