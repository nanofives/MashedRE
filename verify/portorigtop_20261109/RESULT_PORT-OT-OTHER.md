# RESULT PORT-OT-OTHER (2026-11-10): GREEN, ten particle stand-ins and three exe-only callees on matched bodies under MASHED_ORIG_TOP

L2 worker `cmwarkkjt0qrple1mu1pz2s1w` for lead `cmw7xu1960f37p41557tefpnd`, the PORT-OT step-1 sweep (summary:
RESULT_PORT-OT-SWEEP.md). Branch `orch/PORT-OT-OTHER`, stacked on orch/PORT-OT-RENDER (strict ancestor; this branch's
first parent is the RENDER commit). OTHER = every hooks.csv subsystem not named by a PORT-OT row (boot, frontend, hud,
gameplay, vehicle, ai, render, audio, util, save, powerups). In the base runnable set that is particle (37 rows), the two
third-party RW classes (32), track (9), input (8), smplfzx (4), unknown (3), physics / video / world-objects (2 each),
camera / sky / debug-overlay (1 each). Only **particle** has stand-ins (13). The others have none.

## Yield, plainly

**10 particle rows plus 3 exe-only callees.** Seven of the ten run in every knob-ON boot from 0x0040bbb0 (0x00489290,
0x00487e00, 0x00485d90, 0x004862d0, 0x0048eac0, 0x0048a460, plus the callees), and the four teardowns run from 0x0040bd00
at AppDestroy (section "Executed"). Three particle stand-ins stay (table at the end).

## Pre-scan

- Base dump (RESULT_PORT-OT-RENDER.md, same run): particle 37 rows = 22 `-`, 2 matched, 13 stand-in.
- `ot_measure.py verdict` re-run: 0x00485d90 seed `OK` (0x005ce5f8), 0x00485e10 RwGlobals `OK` (+0x10c), 0x004862d0 seed
  `OK` (0x005ce5f8, 0x005ccca0), and the callee 0x004892c0 seed `OK` (0x005cc320). The others are in neither caveat set.
- Particle stand-ins in BINDINGS not in the base runnable set (0x0048af70, 0x0048ae00, 0x0048e820, 0x0048ff20): out of scope.

## What changed (one commit)

| rva | before | now | TU |
|---|---|---|---|
| 0x00489290 | `PortTop_StandIn04G_00489290` (0x0040bbb0) | `Orig_00489290` | Match/PortTop_00489290.cpp, **ORIGTOP-ONLY** CROSS-TARGET vs .asi-only Match/MatchS14_00489290.cpp |
| 0x0048a460 | `PortTop_StandIn04G_0048a460` (0x0040bbb0) | `Orig_0048a460` | Match/PortTop_0048a460.cpp, **ORIGTOP-ONLY** vs Match/MatchS14_0048a460.cpp |
| 0x00487e00 | `PortTop_StandIn04G_00487e00` (0x0040bbb0) | `Orig_00487e00` | Match/PortTop_00487e00.cpp (re/match/s14/00487150.c) |
| 0x00487150 | (static in the same source, not reached before) | `Orig_00487150` | Match/PortTop_00487e00.cpp |
| 0x00487140 | `PortTop_StandIn04C_00487140` (0x0040bd00) | `Orig_00487140` | Match/PortTop_00487140.cpp, **ORIGTOP-ONLY** vs Match/MatchS14_00487140.cpp |
| 0x00485d90 | `PortTop_StandIn04G_00485d90` (0x0040bbb0) | `Orig_00485d90` | Match/PortTop_00485d90.cpp |
| 0x004892c0 | (not reached) | `Orig_004892c0` | Match/PortTop_004892c0.cpp |
| 0x004893a0 | (not reached) | `Orig_004893a0` | Match/PortTop_004893a0.cpp, **ORIGTOP-ONLY** vs Match/MatchS14_004893a0.cpp |
| 0x00485e10 | `PortTop_StandIn04C_00485e10` (0x0040bd00) | `Orig_00485e10` | Match/PortTop_00485e10.cpp |
| 0x004862d0 | `PortTop_StandIn04G_004862d0` (0x0040bbb0) | `Orig_004862d0` | Match/PortTop_004862d0.cpp |
| 0x00486350 | `PortTop_StandIn04C_00486350` (0x0040bd00) | `Orig_00486350` | Match/PortTop_00486350.cpp |
| 0x0048eac0 | `PortTop_StandIn04G_0048eac0` (0x0040bbb0) | `Orig_0048eac0` | Match/PortTop_0048eac0.cpp |
| 0x0048fdd0 | `PortTop_StandIn04C_0048fdd0` (0x0040bd00) | `Orig_0048fdd0` | Match/PortTop_0048fdd0.cpp |

Callee check. Every callee binds by name, no raw original address: 0x004770c0 -> `Orig_004770c0`, 0x00476cb0 ->
`Batch476cb0`, 0x00476be0 -> `Batch476be0`, 0x0040bb30 -> `Search40bb30`, 0x00474db0 -> `Orig_00474db0`, 0x004768c0 ->
`Orig_004768c0`, plus the exe-only 0x004892c0 / 0x004893a0 / 0x00487150. 0x00485e10's RwGlobals +0x10c is read through
DAT_007d3ff8 (the exe's fake device table, OT_MEASURE OK). 0x0048eac0, 0x00486350 and 0x0048fdd0 pass string literals or
undefined1 addresses to undefined4 / uint* / int parameters (a C conversion C++ refuses), so those names bind to C++
overloads taking pointers.

Init / teardown pairs (0x004768c0 reads `*(record + 4)` with no null check):

| init (0x0040bbb0) | records | teardown (0x0040bd00) |
|---|---|---|
| 0x00487e00 -> 0x00487150 | 0x00703068 | 0x00487140 |
| 0x00489290 then 0x00485d90 -> 0x004892c0 | four pools of the 100 at 0x00703118, pointers at 0x006fde10 + 4 (stride 0x23c) | 0x00485e10 |
| 0x004862d0 | 0x006fe700, 0x006fe740 | 0x00486350 |
| 0x0048eac0 | nine records 0x007668f8..0x007706d8 | 0x0048fdd0 |

0x004892c0 takes the first pool with m_58 != 0. 0x00489290 sets m_58 = 1 on all 100 and runs before 0x00485d90 in
0x0040bbb0 (re/match/s02/0040bbb0.c), so all four calls return a pool. With 0x00489290 left a stand-in, 0x004892c0 would
return 0 and 0x004893a0(0) would write at 0x10.

Shared state (each range grepped over mashedmod/src, the 0x0040bbb0 / 0x0040bd00 / stand-in TUs excluded):
- 0x006fde10, 0x006fe790, 0x007668f8, 0x00766978, 0x0076d940, 0x007706d8: no hit.
- 0x006fe700 / 0x006fe740: Lane2/L2_00486f50.cpp. 0x00703118: Lane2/L2_00489890.cpp. 0x00709238:
  Util/PromoLoop_round64.cpp. 0x00766938 / 0x007669b8 / 0x0076a0b8 / 0x00766f00 / 0x0076d900: Lane2/L2_0048fce0.cpp,
  L2_0048fd40.cpp, L2_0048fd10.cpp, Util/PromoLoop_round53.cpp. 0x00703050 / 0x00703068: Util/PromoLoop_round29.cpp,
  Render/PromoLoop_round256.cpp. **None of these is in exe_sources.rsp** (.asi only).
- In the exe: HUD/ScenarioWriters_sa2s1.cpp `Particle3rdEmitterSubmit` (the 0x00487d00 hook body, raw-address calls into
  MASHED.exe, reached only through an install, which the exe's `RH_ScopedInstall` no-op does not do), and comments in
  LibRw/RwRaceFx.cpp:1155 / RwRaceFx.h:58. No exe code reads or writes these records on the knob path.

Source edit, re-checked with the DDK: re/match/s14/00485e10.c: `typedef int code();` is `int code(int)` and
`DAT_006fde14` is `MGLOBAL(int, ...)` under `__cplusplus` only (C++ refuses `int* = &undefined4`). The C text under the
DDK is unchanged: BYTE-IDENTICAL 52 B, relocheck OK 3/3.

`match.py --bytes` / `relocheck.py` (D:/MashedWT/PORT-OT-SWEEP_data/match_bytes.txt): 0x00489290 36 B, 0x0048a460 34 B,
0x00487150 38 B, 0x00487e00 20 B, 0x00487140 12 B, 0x004892c0 218 B, 0x004893a0 16 B, 0x00485d90 114 B, 0x00485e10 52 B,
0x004862d0 126 B, 0x00486350 24 B, 0x0048eac0 250 B, 0x0048fdd0 94 B: all BYTE-IDENTICAL, relocheck OK on each.

Other files: Match/PortTop_0040bbb0.cpp (6 rebinds in RENDER's block below the list), Match/PortTop_0040bd00.cpp (4
defines), exe_sources.rsp (+12 TUs after RENDER's), dual_copy_allowlist.txt (+4 ORIGTOP-ONLY lines after RENDER's),
BINDINGS.tsv by the new `update_bindings_otother.py` (runs `update_bindings_otrender.py`, then this table): 444 -> 447
rows (0x004892c0, 0x004893a0, 0x00487150 added after their callers), matched 177 -> 190, stand-in 83 -> 73.

## Executed

Read from knob_on_1.log against PORT-OT-VEHICLE's knob_on_1.log (the boot below RENDER). Before, VEHICLE's log has one
`stand-in, not run` line for each of the ten. Here it has none of them. **All thirteen bodies run in every knob-ON boot.**

| rva | ran? | evidence (knob_on_1.log) |
|---|---|---|
| 0x0048eac0 | **yes**, boot | :227-235 nine `RpPTankAtomicCreate`: caps 10, 80, 10, 100, 40, 256 with 0x10000087 (0x27), 256 with 0x10080007 (0x807), 80 and 80 with 0x10080027 (0x817), the order and caps of re/match/s15/0048eac0.c |
| 0x00487e00 (+ 0x00487150) | **yes**, boot | :239 `RpPTankAtomicCreate(50, 0x1008000A ...)` (n = 0x32, flags 0x80c) |
| 0x00489290 | **yes**, boot | no line of its own. 0x004892c0 returns a pool only for m_58 != 0, and the four pools below were returned |
| 0x00485d90 (+ 0x004892c0, 0x004893a0) | **yes**, boot | :241-244 four `RpPTankAtomicCreate(280, 0x100000A7 ...)` (0x23 * 8 = 280, flags 0x37) |
| 0x004862d0 | **yes**, boot | :245-246 `RpPTankAtomicCreate(250, 0x10020087 ...)` (0xfa, 0x427) and `(150, 0x1008000A ...)` (0x96, 0x80c) |
| 0x0048a460 | **yes**, boot | the straight-line last call of 0x0040bbb0 before `DAT_0063b8f4 = 1`. Its stand-in line is gone, no line of its own |
| 0x0048fdd0, 0x00487140, 0x00485e10, 0x00486350 | **yes**, AppDestroy | every PTank record above is destroyed (`RpAtomicDestroy(PTank record ...)`: the 24 created ids equal the 24 destroyed ids), the pools with their RwGlobals +0x10c frees |

The knob-ON nav runs (r02 / r04) show the same 24 creates and 24 destroys and reach AppDestroy. Nothing in the exe reads
these records on the frontend path, so nothing new is drawn and the captures are unchanged.

## Acceptance

This branch is the top of the stack and carries the verification for both commits (orch/PORT-OT-RENDER is its parent).

Machine rules: each build was gated by a script that read `%TEMP%\mashed_machine_priolinger_*.lock` (held + live pid)
and `mashed_lock.py status` (PRIORITY-HOLD / SOLO) and started `build.bat` only after two clear reads 20 s apart. The
RENDER build waited for merge train `20261110_110657` (PRIORITY-HOLD pid 4840, its SOLO and 13-track SOLO). Every launch ran
as `test-slot bash -c 'py -3.12 <orch-wt>/re/orchestrator/mashed_lock.py run --label PORT-OT-OTHER -- ...'`. Private root
`D:/MashedWT/PORT-OT-SWEEP_root` (`original` = junction to `D:/MashedWT/PORT-TOP-04J_rootK/original`, as PORT-OT-VEHICLE).

| check | result |
|---|---|
| syntax check before the builds | `cl /Zs /EHa /W3 /O2 /DMASHED_STANDALONE` on the 20 new TUs and the 5 edited callers: no error (one C2440 in 00485e10.c fixed first, section "Source edit" of RESULT_PORT-OT-OTHER.md) |
| build of the top (`MASHED_NO_DEPLOY=1`) | `=== Build OK ===`, `[skip] MASHED_NO_DEPLOY set: mashed_re_dev.asi NOT deployed to original\`. mashed_re.exe sha256 `c04fb0b03abf3515c1dab03052b78779a23c1e38fb452df665b12fafa7fc8c14` |
| knob-ON boot (`knob_boot.py knob_on_1 <root> 300 MASHED_ORIG_TOP=1 MASHED_DET_FRAMES=3000`) | `knob_on_1: exited rc=0 (0x00000000) after 57.4 s`. Last line `WinMain 0x00492370 returned 0 after 3000 frames`. 24 `RpPTankAtomicCreate` and 24 `RpAtomicDestroy(PTank record ...)` on the same 24 records (PORT-OT-VEHICLE's knob_on_1.log: 3 and 3) |
| 12 captures, OFF / ON / OFF / ON, pin default (`CAPFLAKE_DATA=<data> CAPFLAKE_ROOT=<root> capflake_runs.py nav1 OFF@dflt ON@dflt OFF@dflt ON@dflt`) | 4/4 `rc=0 pinned pos=55 hash=28538c74 class=E eqE=12`. The 12 hashes are equal in all four runs, knob ON and OFF. walk_04_colourselect.bmp `ba34030d1b7e2fe4` in all four. Both ON logs: 24 creates, 24 destroys, AppDestroy reached, no stand-in line of a swapped RVA |
| G-CTRL (H_DEF) | `8284be46/67931834/31fe043e` = merge_train.json `gctrl.ref` |
| B (K0 / K1 / K26) | `1d5bf4f9` x3 = `b.ref` |
| rva-lint | `D4 burn-down=2  ORIGTOP-ONLY=23 (class deleted at the ORIG_TOP flip; 24 marked TU(s))`, `OK -- no NEW duplicate bodies (25 known)`. On RENDER alone: ORIGTOP-ONLY 19. Before (bde8b3774): 15 |

`A4_OUT` (D:/MashedWT/PORT-OT-SWEEP_data/arms) was created before `run_arms.py`, `A4_ROOT` = the private root. No new
run_arms stack: no knob.

### Port column deltas (on this commit's build)

`backfill_exe_file.py` (`rows given an exe_file: 13`, `old value changed: 0`), then `backfill_port.py`:
`none=5138 body=552 called=465`, 13 rows changed, all **none -> called**: 0x00489290, 0x0048a460, 0x00487e00, 0x00487150,
0x00487140, 0x00485d90, 0x004892c0, 0x004893a0, 0x00485e10, 0x004862d0, 0x00486350, 0x0048eac0, 0x0048fdd0. No drift row.


## OTHER rows left, with the reason

| rva | name | reason |
|---|---|---|
| 0x0048cf70 | FUN_0048cf70 | runs 0x004768c0 on the 17 0x40-byte records 0x00722470..0x007228b0. No exe TU and no body swapped here writes them, so the first one's `*(0 + 4)` is the first read. |
| 0x0048ffd0 | FUN_0048ffd0 | for the 30 0x50-byte entries from 0x0086a4a4 calls 0x004e8090(entry[2], 0) and RpClumpDestroy(entry[0]). 0x004e8090 reads `*param_1` with no null check, and no exe TU writes entry +8 (0x0086a4ac). The table loader in 0x0040bbb0 is 0x0048ff20, a stand-in. |
| 0x0048a830 | FUN_0048a830 | needs 0x004b6480 (BitArrayClear) in the exe. The only exe-only copy of it is PORT-OT-HUD-2's Match/PortTop_004b6480.cpp (branch still `working`). A second copy here would be a duplicate symbol once both merge. One-line swap after HUD-2 merges. |

## Raw files (D:/MashedWT/PORT-OT-SWEEP_data, not committed)

| file | sha256 |
|---|---|
| build.log | f46770500d9d42f1f64f733f8995771cfccd0844a3814e7407be4c3adc520330 |
| knob_on_1.log | 93904ec47deebbce017d2a32dfdbab1536ecd0303563d59f75aa383c87b14a22 |
| knob_on_1.stdout | ea0ca4b49c1e0de25705e13dc1943d568b69fac208a1a7a6aac1ee338036b8e2 |
| runs.tsv | 57330649dd80ec34fbcbb541d4200d97cfe6376ecad59ca71d2e3a696bc0f656 |
| capflake.stdout | ec4d1cabb94fb84568581840f3789fdb5ace0e95ef16db9a401797e6089734b7 |
| nav1/r01_OFF_pindflt.log | 08e0e8cd074b3962936db188d78bc9bde09de5518e23f44dbdf68600b7acad23 |
| nav1/r02_ON_pindflt.log | fa75d157968905686aeaa1475a7dccfc4193ab77ecf6c116736b17fd5cc7379e |
| nav1/r03_OFF_pindflt.log | 5084088cd99875c6289331245f7afd78be6320a47bc2f61917f66a34a9a1000d |
| nav1/r04_ON_pindflt.log | a4f96cfee67883b6e5ed39f9693e1ee445db5215d61a9443e196cc24530c79b2 |
| arms.stdout | 3a5ef83908c9e3d45e8bb110af447d82a757e4bd13ca3436c3a9bdc359c9ee1b |
| arms/H_DEF.step.csv | 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198 |
| arms/H_DEF.gates.csv | 6793183492e0c0822570ad71254f9fdf27568b77c81a25c56347f1616863d58c |
| arms/H_DEF.ptrace.log | 31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3 |
| arms/B_DEF_K0.step.csv, B_DEF_K1.step.csv, B_DEF_K26.step.csv | 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 (all three) |
| base.tsv, OT_MEASURE.rerun.tsv, match_bytes.txt | in RESULT_PORT-OT-SWEEP.md |

## Next

- 0x0048a830: one line in Match/PortTop_0040bbb0.cpp's rebind block plus a short TU, once PORT-OT-HUD-2
  (Match/PortTop_004b6480.cpp) merges.
- 0x0048cf70 needs the builder of the 17 records at 0x00722470. 0x0048ffd0 needs 0x0048ff20 (the 0x0086a4a4 table loader).
