# RESULT PORT-BATCH-GAMEPLAY (D4, L-107 (a)): 5 in-place rows swapped, 20 left out with the reason

Branch `orch/PORT-BATCH-GAMEPLAY`. Base: `df0a63dc8` (PORT-BATCH-TOOL `5da842022` + orch/integration `bba25cb03`).
Swap commit `d80154ba9`, this RESULT and the port-column rows in the commit after it. No knob (L-107 (a)).

## 1. What landed

The gameplay set of RESULT_PORT-BATCH-TOOL has 25 rows. Five are swapped. Each now compiles its byte-matched source
under the hand body's exported name and calling convention, in the same targets the hand body was compiled in. The
hand bodies are deleted in the same commit.

| rva | name (kept) | matched source | wrapper (both targets) | hand body deleted from |
|---|---|---|---|---|
| 0x00407b00 | RangeTableFindGroup | re/match/s01/00407b00.c | Match/PortBatch_GAMEPLAY_00407b00.cpp | Gameplay/RangeTable_ah1.cpp (and its TT-11 shadow A/B entry) |
| 0x00407b70 | RangeTableMemberTest | re/match/s01/00407b70.c | Match/PortBatch_GAMEPLAY_00407b70.cpp | Gameplay/RangeTable_ah1.cpp |
| 0x00407d70 | RangeTableCountGroup | re/match/s01/00407d70.c | Match/PortBatch_GAMEPLAY_00407d70.cpp | Gameplay/RangeTable_ah1.cpp |
| 0x00407db0 | RangeTableGroupOffset | re/match/s01/00407db0.c | Match/PortBatch_GAMEPLAY_00407db0.cpp | Gameplay/RangeTable_ah1.cpp |
| 0x005c9d00 | GetRaceEndTrigger | re/match/gffu/005c9d00.c | Match/PortBatch_GAMEPLAY_005c9d00.cpp | Util/GameStateGetters.cpp |

Binding checks (done by hand, `extern "C"` hides mismatches at link time):
- The four walkers: the matched prototypes are `int (int)` / `int (int, int)`, cdecl, result in eax. They match the
  hand declarations in `Race/PathProgStep.cpp:159-162` and the `call <Name>` sites in `Race/PathProgStep_naked.inc`.
  `RangeTableMemberTest` was declared `uint32_t`, the matched one returns `int`: same register, same bits.
- Their one callee `FUN_00426090` binds per target exactly as the hand body did: the exe binds the byte-matched
  `GlobalDat0066ce58Get` (Match/MatchS06_00426090.cpp), the .asi calls the original address. Both read the table at
  0x0066ce58, so the data source does not change.
- `GetRaceEndTrigger`: `int (void)` returning 0, callers bind `FUN_005c9d00` by name (Race/GameModeMachine.cpp,
  Match/PortTop_00492370.cpp, Match/PortTop_00493710.cpp). Still no install (3-byte original).
- The `RH_ScopedInstall` lines moved with the bodies, so the .asi hooks the same RVAs, now to the matched bodies.

Build: both targets build, `[rva-lint] 1 finding(s): CROSS-TARGET=1 | allowlisted=1 NEW=0`, the same as the base.
Allowlist unchanged. Exe `9fe71a2e7af0e13deb9187a56d9fae157031424cca1ec118ef25163ecf409935`, asi
`97b52deebbbd2d3fef3824488d84ee931585c820daf07f022d89f2614af00d07`. Base exe (this branch before the swap)
`902de74cdb399acd2fc04ac52df3442cdc7ab1cba2c3af6e0ec8a89d089590eb`.

Generator: `re/tools/port_batch_gen.py` still has no in-place mode. The five wrappers are the MATCH-TOPDOWN shape
(`Match/MatchS01_00407a20.cpp`), written by a one-off script, because deleting hand bodies is per-file editing a
generator cannot do safely. The safety review below is the part that cannot be automated.

## 2. Port column

`scripts/backfill_exe_file.py` then `scripts/backfill_port.py` on the batch build. For the 5 rows: `exe_file` moves to
the wrapper, `port` stays `called` (it was `called` before). No row of the set loses `called`.

The backfill also re-derived 12 rows this branch does not touch. They are left at their committed values, so this
branch's hooks.csv diff is the 5 rows only:
- `0x00418860` called -> body: the known PORT-COL-0x00418860 drift (decision pending), kept at `called`.
- 11 rows none/body -> called (`0x004025f0`, `0x005ac740`, `0x004e7e30`, `0x004b3bf0`, `0x004881d0`, `0x00489250`,
  `0x00401630`, `0x00401690`, `0x004880a0`, `0x0048bc10`, `0x004e8090`): PORT-TOP-04I / PORT-TOP-04 work merged from
  orch/integration whose port column was not re-derived. The next tracker pass should take them.
- The hooks.csv `file` column of the 5 rows still names the old hand TUs (re-classify's job, not done here).
- `0x0041f360` is malformed (11 fields with an unquoted comma), so both backfills skip it. It is not swapped (below),
  so it was not fixed here. Same for `0x00404830`, `0x004464c0`, `0x00420870`.

## 3. Rows left out (20)

Rule applied before swapping: an in-place swap is a swap only when the hand body has the matched prototype and reads
the same data. Where the exe hand body is a stand-in, takes caller-supplied inputs, or reads a port-side value, the
swap would change what the exe computes and needs caller rewrites, which is a behaviour change, not a swap. Those rows
are NOT-SAFE in the sense of MATCH-S11-FU-VEHBASE.

Grouped by reason (added on orch/PORT-BATCH-FRONTEND at the lead's request; one reason per row, "not reached" rows
carry their own blocker in brackets; the per-row detail follows):

| reason | rows |
|---|---|
| swapped (5) | 0x00407b00 FUN_00407b00; 0x00407b70 FUN_00407b70; 0x00407d70 RangeTableCountGroup; 0x00407db0 RangeTableGroupOffset; 0x005c9d00 GetRaceEndTrigger |
| stand-in (3) | 0x00451820 FUN_00451820; 0x0045bff0 Pred45bff0; 0x0045c110 FUN_0045c110 |
| caller-supplied inputs (2) | 0x0040b890 FUN_0040b890; 0x0040b8e0 FUN_0040b8e0 |
| port-side value (4) | 0x0041f360 GatedSwitch636ad0; 0x0046c7b0 VehicleSlotGetter; 0x0046cbb0 VehicleCarStateRead; 0x0046d4a0 PtrCompute881ec8 |
| not reached (11) | 0x00409a80 FUN_00409a80 [none]; 0x0040ba60 Active4Slots40ba60 [CROSS]; 0x0040e4b0 Race::GetSoleFinishedPlayer [none]; 0x00414220 FUN_00414220 [none]; 0x00417750 FUN_00417750 [none]; 0x0042c1a0 StateAdvance2to3 [CROSS]; 0x0045cab0 SlotAliveState45cab0 [CROSS]; 0x0045d0e0 FUN_0045d0e0 [SEED]; 0x0045dff0 FUN_0045dff0 [none]; 0x0046be10 Write46be10 [none]; 0x00472550 Table691508Get [CROSS] |

| rva | name | reason |
|---|---|---|
| 0x0046c7b0 | VehicleSlotGetter | VEHBASE (ledger MATCH-S11-FU-VEHBASE): rebased vehicle record |
| 0x0046cbb0 | VehicleCarStateRead | VEHBASE |
| 0x0046d4a0 | PtrCompute881ec8 | VEHBASE |
| 0x0040b890 | GridCount8or12Core | exe body takes the three inputs as arguments (`GridCount8or12Core(cars, ea64, fd0)`); exe callers pass standalone values (`exe_main.cpp:4453` kParticipants/kEa64/rule_now, `LibRw/RwRaceFx.cpp:1473` in.n4d0/team_play/mode_fd0). The matched body is zero-arg and reads 0x0040e340 / 0x0042f500 / 0x007f0fd0 itself |
| 0x0040b8e0 | ScoreThreshold7or10Core | same as 0x0040b890 (`exe_main.cpp:4455`, `RwRaceFx.cpp:1476`) |
| 0x0041f360 | GatedSwitch636ad0 | exe body reads the pinned constant `g_dat_00636ad0 = 0` (Vehicle/CarMaterialTables.cpp:58); the matched body reads the exe pad at 0x00636ad0, which exe-compiled matched bodies can write (re/match/s01/00401000.c, s16/00498a00.c and others name DAT_00636ad0). Also the malformed hooks.csv row |
| 0x0045c110 | SurfaceGate (stand-in) | exe body is `int SurfaceGate(const WorldHit*, uint32_t)` (Powerup/PowerupContact.cpp:231), a stand-in over the standalone's collision triangles (no material colour); the matched body takes an RpMaterial and calls 0x00472550 |
| 0x00472550 | Table691508Get | callee of 0x0045c110 only; nothing reaches it without that swap |
| 0x00451820 | Trigger (WarzonePontoon) | exe body is C++ `Trigger(Obj&)` over the port's object and host callbacks (Track/WarzonePontoon.cpp:361) |
| 0x0046be10 | Write46be10 | callee of 0x00451820 only |
| 0x0045bff0 | Pred45bff0 | the exe binds `FUN_0045bff0` to the stand-in `PK_SlotEmpty` (Gameplay/PickupTick.cpp:140, host callback / replay) in Match/MatchS10_0045c330.cpp; the tool's `rebind` site is the .asi forwarder. Note: `PK_SlotEmpty`'s `// FUN_0045bff0` comment is not seen by rva_body_scan, so the tool did not count it as an exe body |
| 0x0045d0e0 | FUN_0045d0e0 | SEED (reads 0x005ce658) and not reached on the default path: dropped rather than extending the seed |
| 0x00409a80, 0x0040ba60, 0x0040e4b0, 0x00414220, 0x00417750, 0x0042c1a0, 0x0045cab0, 0x0045dff0 | | not reached: each is the head (or only-callee) of a group that the default path reaches only through a VEHBASE member (0x0046c7b0) or 0x005c9d00. Compiling them gives `body`, not `called`; giving them a caller is a behaviour change outside L-107 (a) |

## 4. Gates

Run: `re/tools/port_batch_bisect.py --tag GAMEPLAY --exe-off <base exe> --exe-on <batch exe>` (new build mode: no
switch, OFF = the base build `902de74c`, ON = the batch build `9fe71a2e`, both with the DEF stack), inside `test-slot`
and `mashed_lock.py run --label PORT-BATCH-GAMEPLAY`. Roots: `D:/MashedWT/PORT-BATCH-TOOL_root` (arms) and `_rootK` (nav),
the PORT-BATCH-TOOL copies, with the PORT-TOP-04E settings baseline restored before each nav run. The slot waited 10 min
for memory (18.8 to 19.9 GB committed by other runs) and then started under the cap.

| pass | exe | G-CTRL | B (K0/K1/K26) | 12 nav captures vs 04E list |
|---|---|---|---|---|
| OFF | base | `8284be46/67931834/31fe043e` | `1d5bf4f9` x3 | 12/12 |
| ON | batch | `8284be46/67931834/31fe043e` | `1d5bf4f9` x3 | 12/12 |
| OFF repeat | base | `8284be46/67931834/31fe043e` | `1d5bf4f9` x3 | 12/12 |
| ON repeat | batch | `8284be46/67931834/31fe043e` | `1d5bf4f9` x3 | 12/12 |

- Every pass equals the `merge_train.json` refs. Both sides repeat. Bisect: not needed (responsible: none).
  Table: `verify/portbatch_20261109/bisect_GAMEPLAY.tsv`.
- All arms exited 0 (pass rc 0) and every nav run ended `NAV_DEMO phase=28 done ok=1`.
- Coverage: the five rows are `called` (reachable from WinMain in the linked exe). The walkers are called from the
  path-progress transcription (`Race/PathProgStep_naked.inc`) and 0x005c9d00 from the game-mode machine.
  [UNCERTAIN] whether the G-CTRL / B races execute each of them: neither the hand bodies nor the matched bodies log a
  call, so the gates show no difference but do not prove execution per row.

## Evidence (raw data in `D:/MashedWT/PORT-BATCH-GAMEPLAY_data`, not committed)

| file | SHA-256 |
|---|---|
| exe_base/mashed_re.exe (OFF) | 902de74cdb399acd2fc04ac52df3442cdc7ab1cba2c3af6e0ec8a89d089590eb |
| exe_batch/mashed_re.exe (ON, = commit d80154ba9) | 9fe71a2e7af0e13deb9187a56d9fae157031424cca1ec118ef25163ecf409935 |
| build_base.log | 262c38ec454df87b8d147ea9208fbc2311443e9d3dc91c261a05624dbfc13b32 |
| build_batch.log (Build OK, rva-lint NEW=0) | f44e26828a50f148d4d55b1f1bb2e2d6c7e77e2a224a7c0c9100b7b0338b3f0d |
| gate/01_off/arms/H_DEF.step.csv | 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198 |
| gate/02_on_all/arms/H_DEF.step.csv | 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198 |
| gate/02_on_all/arms/H_DEF.gates.csv | 6793183492e0c0822570ad71254f9fdf27568b77c81a25c56347f1616863d58c |
| gate/02_on_all/arms/H_DEF.ptrace.log | 31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3 |
| gate/02_on_all/arms/B_DEF_K0.step.csv (K1, K26 identical) | 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 |
| gate/04_on_all2/arms/H_DEF.step.csv | 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198 |
| gate/01_off/navdemo.log | 6eb99f1050933dcd306dcdcac3af644e29126de8d170c8b1c5b0bb4b64600e45 |
| gate/02_on_all/navdemo.log | 351307b28a0f3717a43f6bcf6c7fb0b92b739c1588ff7f6e007dae202e50d231 |
| gate/passes.tsv (all 4 passes incl. nav hashes) | 8e1c096f91603ae3729f15fe15cad45cba67ddb95f1f10ea4b9d90bef981a2f3 |
| gate_run.log | 0906f2c52206d42e80e38cdd6c72b6181aed65b40755ea9720dc4124e0624c8c |
| hooks.backfilled.csv (full backfill output, before keeping the 5 rows) | see section 2 |

The batch sources as set aside for the base build are in `batch_src/` (batch.patch + the five wrappers).
