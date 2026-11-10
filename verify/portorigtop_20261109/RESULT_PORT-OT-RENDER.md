# RESULT PORT-OT-RENDER (2026-11-10): GREEN, six RENDER stand-ins and two exe-only callees on matched bodies under MASHED_ORIG_TOP

L2 worker `cmwarkkjt0qrple1mu1pz2s1w` for lead `cmw7xu1960f37p41557tefpnd`, the PORT-OT step-1 sweep over AI, RENDER, AUDIO,
UTIL, SAVE, POWERUPS and OTHER (summary: RESULT_PORT-OT-SWEEP.md). Branch `orch/PORT-OT-RENDER` from orch/integration
`dabf91efe` with orch/PORT-OT-VEHICLE `4883ee9ee` merged (`git -c merge.autoStash=false`, no conflict: this step edits
BINDINGS.tsv, exe_sources.rsp and dual_copy_allowlist.txt, which VEHICLE also edits). The worktree is
`.worktrees/PORT-OT-AI` (named before the pre-scan found AI has no row). orch/PORT-OT-OTHER is stacked on this branch.

## Yield, plainly

**6 RENDER rows plus 2 exe-only callees.** RENDER has 166 rows in the base runnable set, 9 of them stand-ins. Six swap.
Three stay (table at the end). Five of the six run in every knob-ON boot (section "Executed"): 0x004759b0, 0x00476390
and 0x00477e40 from 0x0040bbb0, and 0x00475ea0 and 0x004d7ff0 at AppDestroy. 0x0041a980 sits on a branch no check here
takes.

## Pre-scan (shared with the sweep)

- `origtop_base_dump.py --dump-base` on bde8b3774 (SIZE.tsv restored afterwards): 1,008 base rows. RENDER 166: 121 `-`,
  26 matched, 9 stand-in, 10 under-stand-in.
- The 10 under-stand-in rows are under the 0x00492e90 / 0x00492d30 stand-ins (PORT-OT-RACE), not swaps here.
- RENDER stand-ins in BINDINGS that are not in the base runnable set (0x004caea0, 0x004d8480, 0x004951f0, 0x004b6700,
  0x00477870, 0x00422140, 0x004b3d20, 0x00475a00, 0x004775b0, 0x00402240, 0x00402590, 0x004b4080): out of scope.
- `ot_measure.py verdict` re-run on this tree (probe data `D:/MashedWT/PORT-OT-MEASURE_data`, output kept out of the
  commit, sha256 below): 0x00475d90 `OK` (+0x10c), 0x00477e40 seed `OK` (0x005ceb10). The other RENDER candidates are in
  neither caveat set (base dump `rwglobals` / `nonzero_data` 0 / 0).

## What changed (one commit)

| rva | before | now | TU |
|---|---|---|---|
| 0x004759b0 | `PortTop_StandIn04G_004759b0` (0x0040bbb0) | `Orig_004759b0` | Match/PortTop_004759b0.cpp, **ORIGTOP-ONLY** CROSS-TARGET vs .asi-only Match/MatchS12_004759b0.cpp |
| 0x00476390 | `PortTop_StandIn04G_00476390` (0x0040bbb0) | `Orig_00476390` | Match/PortTop_00476390.cpp |
| 0x00475ef0 | (not reached: only caller was the 0x00476390 stand-in) | `Orig_00475ef0` | Match/PortTop_00475ef0.cpp, **ORIGTOP-ONLY** CROSS-TARGET vs .asi-only Match/MatchS12_00475ef0.cpp |
| 0x00477e40 | `PortTop_StandIn04G_00477e40` (0x0040bbb0) | `Orig_00477e40` | Match/PortTop_00477e40.cpp |
| 0x00477b40 | (not reached) | `Orig_00477b40` | Match/PortTop_00477b40.cpp, **ORIGTOP-ONLY** CROSS-TARGET vs .asi-only Match/MatchS12_00477b40.cpp |
| 0x00475ea0 | `PortTop_StandIn04C_00475ea0` (0x0040bd00) | `Orig_00475ea0` | Match/PortTop_00475ea0.cpp |
| 0x0041a980 | `PortTop_StandIn04C_0041a980` (0x0040cf80) | `Orig_0041a980` | Match/PortTop_0041a980.cpp |
| 0x004d7ff0 | `PortTop_StandIn04C_004d7ff0` (0x005581f0) | `Orig_004d7ff0` | Match/PortTop_004d7ff0.cpp, **ORIGTOP-ONLY** CROSS-TARGET vs .asi-only Render/HighAB3Helpers_p6.cpp |

Callee check (the BOOT / FRONTEND lesson). Every callee binds by name, no raw original address:
- 0x004759b0: 0x00474db0 -> `Orig_00474db0` (RwGlobals +0x108 librw malloc, zero fill).
- 0x00476390: 0x00475ef0 -> `Orig_00475ef0`, 0x0040bb30 -> `Search40bb30`, 0x004770c0 -> `Orig_004770c0` (flags 0x817 / 0x807:
  bit 0x800 calls 0x00476c10 `Batch476c10`, Render/RfxEmitter.cpp, which writes PTank block +0xd8..+0xe4), 0x00476cb0 ->
  `Batch476cb0`. Forwarders carry the source's `Rec40*` prototypes.
- 0x00475ef0: 0x004b6520 -> `Orig_004b6520`.
- 0x00477e40: 0x0040bb30 -> `Search40bb30` (C++ overload taking the pointer, the source declares undefined4), 0x00477b40 ->
  `Orig_00477b40`.
- 0x00477b40: 0x004b6520 -> `Orig_004b6520`.
- 0x00475ea0: 0x004768c0 -> `Orig_004768c0` (forwarder from `undefined*`).
- 0x0041a980: RpClumpDestroy -> `MashedRw_ClumpDestroy_004e6e00` (rw::Clump::destroy, null skipped), 0x004b6520 ->
  `Orig_004b6520` (C++ overload taking the pointer).
- 0x004d7ff0: no callee.

Pairs. 0x004768c0 reads `*(record + 4)` with no null check (re/match/s12/004768c0.c). So a teardown swaps only with the init
that builds its records: 0x00475ea0 with 0x00476390 (the same five records 0x00691618 x2, 0x00692418 x2, 0x00692498).

Shared state (the 0x00458880 lesson, each address grepped over mashedmod/src, the 0x0040bbb0 / 0x0040bd00 / stand-in TUs
excluded): 0x00691600..0x00691617, 0x00691618, 0x00692418, 0x00692498, 0x00691698, 0x00613254, 0x00693190, 0x00693198,
0x006bf198, 0x0063c7b8: **no hit**. No pool or record the hand port owns is touched.

Source edit, re-checked with the DDK: re/match/s03/raw_0041a980.c had the raw g2s preamble (C++ keywords `bool` /
`wchar_t` as typedefs) and an `extern` global. It now has the MATCH-S02 preamble and `MGLOBAL(undefined4, DAT_0063c7b8,
0x0063c7b8)`, the raw_0041a3d0.c form, CRLF kept. `match.py --bytes`: BYTE-IDENTICAL 45 B before and after, relocheck OK
3/3 (RpClumpDestroy unchecked, as raw_0041a3d0.c).

`match.py --bytes` / `relocheck.py` on every swapped source (D:/MashedWT/PORT-OT-SWEEP_data/match_bytes.txt):

| rva | bytes | relocheck |
|---|---|---|
| 0x004759b0 | 68 B BYTE-IDENTICAL | OK 9 |
| 0x00475ef0 | 57 B BYTE-IDENTICAL | OK 4 |
| 0x00476390 | 149 B BYTE-IDENTICAL | OK 14 |
| 0x00475ea0 | 67 B BYTE-IDENTICAL | OK 8 |
| 0x00477e40 | 23 B BYTE-IDENTICAL | OK 4 |
| 0x00477b40 | 31 B BYTE-IDENTICAL | OK 4 |
| 0x0041a980 | 45 B BYTE-IDENTICAL | OK 3 (+1 unchecked) |
| 0x004d7ff0 | 5 B BYTE-IDENTICAL | OK 0 |

Other files:
- Match/PortTop_0040bbb0.cpp: the three rebinds are one `#undef` / `#define` block below the stand-in list, and the SI
  definitions stay (unreferenced). PORT-OT-HUD-2 (still `working`) edits the 0x004210b0 lines right next to 0x004759b0's,
  and an in-place edit there would conflict in the merge train.
- Match/PortTop_0040bd00.cpp, Match/PortTop_0040cf80.cpp, Match/PortTop_005581f0.cpp: one define each, an object-like
  `#define FUN_x Orig_x`.
- `mashedmod/exe_sources.rsp`: +8 TUs after `Match\PortTop_0040bbb0.cpp`, not at the tail.
- `re/tools/dual_copy_allowlist.txt`: +4 `ORIGTOP-ONLY CROSS-TARGET` lines after the 004770c0 line, not at the tail.
- BINDINGS.tsv by the new `update_bindings_otrender.py`. It runs `update_bindings_othud.py` once and then applies the
  GAMEPLAY and VEHICLE tables (running both of their chains in turn resets the other one's rows: each re-runs othud, and the
  second run put 0x0040cf40 / 0x004725f0 / 0x00405400 back to stand-in in a first try). 442 -> 444 rows (0x00475ef0,
  0x00477b40 added after their callers), matched 169 -> 177, stand-in 89 -> 83.
- Comment lines of the new TUs write callees as `0xRVA -> Name`: `0xRVA Name` above a declaration is an RVA header to
  `scripts/rva_body_scan.py`, and the first lint run counted three false bodies from it.

## Executed

Read from the knob-ON boot log (D:/MashedWT/PORT-OT-SWEEP_data/knob_on_1.log) against PORT-OT-VEHICLE's knob_on_1.log,
the same boot on the commit below this one. Before, VEHICLE's log has one `stand-in, not run` line each for 0x004759b0,
0x00476390, 0x00477e40, 0x00475ea0 and 0x004d7ff0. Here it has none of them.

| rva | ran? | evidence |
|---|---|---|
| 0x00476390 (+ 0x00475ef0) | **yes**, boot | knob_on_1.log:221-225: five `RpPTankAtomicCreate(64, ...)`, four with flags 0x10080027 (0x817) and one with 0x10080007 (0x807), the five records 0x00476390 builds |
| 0x00475ea0 | **yes**, AppDestroy | the same five PTank records get `RpAtomicDestroy(... record freed)` after the 0x00477870 stand-in line, each followed by the RwGlobals +0x10c frees of its per-flag arrays |
| 0x004759b0 | **yes**, boot | it is on 0x0040bbb0's straight-line path (re/match/s02/0040bbb0.c) and its stand-in line is gone. It writes no log line of its own |
| 0x00477e40 (+ 0x00477b40) | **yes**, boot | same: straight-line in 0x0040bbb0, stand-in line gone, no log line of its own |
| 0x004d7ff0 | **yes**, AppDestroy | 0x005581f0's param_1 == 0 branch runs in this boot: VEHICLE's log has the 0x004d7ff0 stand-in line right above the 0x004d8480 one (:458-459). Here only the 0x004d8480 line is left (:592) |
| 0x0041a980 | no | 0x0040cf80 calls it only when 0x0041f320(i) != 0 for some i < 4. No 0x0041a980 line before or after, in any log here |

The knob-ON nav runs (r02 / r04) show the same 24 creates and 24 destroys.

## Acceptance

Verified once, at the top of the stack (orch/PORT-OT-OTHER, whose parent commit is this RENDER commit: a strict ancestor,
so every RENDER body is in the verified exe). The RENDER build itself (`=== Build OK ===`, mashed_re.exe sha256
`5bbcfcf05ce0c5a4cccba73eb8e544689a99509a33703d5d392d5370c6d2f708`) was used only for the port column below.

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

`backfill_exe_file.py` (`rows given an exe_file: 8`, `old value changed: 0`), then `backfill_port.py`:
`none=5151 body=552 called=452`, 8 rows changed, all **none -> called**: 0x004759b0, 0x00476390, 0x00475ef0, 0x00477e40,
0x00477b40, 0x00475ea0, 0x0041a980, 0x004d7ff0. No drift row.


## RENDER rows left, with the reason

| rva | name | reason |
|---|---|---|
| 0x00475d90 | FUN_00475d90 | its loop runs 0x004768c0 on each 0x40-byte record of DAT_00691608 (2 after 0x004759b0). Those records are filled by 0x00475a00 (called twice from 0x0040bbb0, no matched source, a stand-in), so they are zero and 0x004768c0's first read is `*(0 + 4)`. Needs 0x00475a00 first. |
| 0x00490000 | FUN_00490000 | calls 0x004e8090(*p, 0) for the 30 0x50-byte entries from 0x0086a4ac. 0x004e8090 reads `*param_1` with no null check (re/match/rwl02/004e8090.c). No exe TU writes 0x0086a4ac (Util/UtilLeaves_ab6.cpp's 0x004904d0 zeroes +0 of the same table), so on this path the first call reads `*(0)`. Its caller 0x0040cfd0 does not run in any check here (RESULT_PORT-OT-VEHICLE.md, Reachability). |
| 0x004768c0 | FUN_004768c0 | its stand-in is for the caller 0x004210f0 only, PORT-OT-HUD-2's row. |

## Raw files (D:/MashedWT/PORT-OT-SWEEP_data, not committed)

| file | sha256 |
|---|---|
| build_render.log (this commit's build) | 7bf94ae529e949cb364984b514a4937bbc129c72d49b9fb1e453a8890790641c |
| mashed_re_render.exe (this commit's build) | 5bbcfcf05ce0c5a4cccba73eb8e544689a99509a33703d5d392d5370c6d2f708 |
| build.log (top of the stack) | f46770500d9d42f1f64f733f8995771cfccd0844a3814e7407be4c3adc520330 |
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

- RENDER has no stand-in left in the base runnable set that swaps without other work. 0x00475d90 needs 0x00475a00 (C2, no
  matched source) on the path, 0x00490000 needs a writer of 0x0086a4ac, and 0x004768c0's last stand-in caller is HUD-2's.
- 0x0041a980 runs only when 0x0040cf80's 0x0041f320 test passes, a harness item like PORT-OT-VEHICLE's 0x0040cfd0.
