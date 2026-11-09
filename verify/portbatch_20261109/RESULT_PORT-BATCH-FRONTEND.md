# RESULT PORT-BATCH-FRONTEND (D4, L-107 (a)): 1 row swapped (0x00496900 SlotActiveThunk), plus the left-out count over all five batches

Branch `orch/PORT-BATCH-FRONTEND`, from `orch/PORT-BATCH-GAMEPLAY` `eb980e57c` (enqueued, not merged). No knob.
This branch also carries RESULT_PORT-BATCH-HUD / -AI / -RENDER (0 rows each, no code) and a grouped left-out table
added to RESULT_PORT-BATCH-GAMEPLAY section 3.

## 1. What landed

| rva | name (kept) | matched source | where | hand body removed |
|---|---|---|---|---|
| 0x00496900 | SlotActiveThunk | re/match/s16/00496900.c (100%, relocheck OK) | Match/MatchGFFU_00497450.cpp (both targets, as before) | the hand forwarder `SlotActiveThunk(int) { return Pred497450(int); }` in the same TU |

- 0x00496900 is `E9 4B 0B 00 00` = `jmp 0x00497450`. The matched source is `int FUN_00496900(int p) { return
  FUN_00497450(p); }`; under the wrapper `FUN_00496900` = `SlotActiveThunk` and `FUN_00497450` = `Pred497450` (the
  byte-matched 0x00497450 body the same TU already compiles). Same prototype as the hand forwarder (one int, the
  0x00497450 result in eax). The one exe caller, `Match/MatchS07_0042c220.cpp` (`MatchS07_0042c220_Fwd00496900`),
  binds it by name. Path: SlotActiveThunk <- FUN_0042c220 <- GameModeMachine_004929d0 <- WinMain (hooks.csv row).
- `RH_ScopedInstall(SlotActiveThunk, 0x00496900)` stays, so the .asi hooks the thunk to the matched body.
- The TU is not in x87_tus.txt and stays out (integer code).

Build: both targets build, `[rva-lint] 1 finding(s): CROSS-TARGET=1 | allowlisted=1 NEW=0` (same as the base),
allowlist unchanged. Exe `b5598a02643f0078474dc515429a64de7ba0929c3d7b7d82b86b6b9e69dcc79e`, asi
`973bf79d8c14e912286a579e5b4d06892f000103616c8a92da6092052818f3e7`. Swap commit `28fd12729`.

Port column (`backfill_exe_file.py` then `backfill_port.py` on this build): 0x00496900 none -> **called**, `exe_file`
-> `Match/MatchGFFU_00497450.cpp` (the hand forwarder had no RVA anchor the scan could see). Only that row is
committed; the other 12 re-derived rows are the ones the lead filed as PORT-TRACKER-REDERIVE-2.

## 2. Gates

Build mode of `re/tools/port_batch_bisect.py`, inside `test-slot` + `mashed_lock.py run --label PORT-BATCH-FRONTEND`.
OFF = the GAMEPLAY batch exe `9fe71a2e` (the build of `d80154ba9`; `eb980e57c` adds no code), ON = this build
`b5598a02`, both with the DEF stack. Roots as in GAMEPLAY (`D:/MashedWT/PORT-BATCH-TOOL_root`, `_rootK`).

| pass | exe | G-CTRL | B (K0/K1/K26) | 12 nav captures vs 04E list |
|---|---|---|---|---|
| OFF | GAMEPLAY batch | `8284be46/67931834/31fe043e` | `1d5bf4f9` x3 | 12/12 |
| ON | FRONTEND batch | `8284be46/67931834/31fe043e` | `1d5bf4f9` x3 | 12/12 |
| OFF repeat | GAMEPLAY batch | `8284be46/67931834/31fe043e` | `1d5bf4f9` x3 | 12/12 |
| ON repeat | FRONTEND batch | `8284be46/67931834/31fe043e` | `1d5bf4f9` x3 | 12/12 |

- Every pass equals the `merge_train.json` refs, both sides repeat, bisect not needed (responsible: none). Table:
  `verify/portbatch_20261109/bisect_FRONTEND.tsv`. All arms rc 0, every nav run ended `NAV_DEMO phase=28 done ok=1`.
- Coverage: 0x00496900 is `called` (FUN_0042c220 <- GameModeMachine_004929d0 <- WinMain). [UNCERTAIN] whether the
  gate scenarios execute it: no per-call log on either body.

## 3. FRONTEND rows by reason (19)

Same rule and categories as the other batches (RESULT_PORT-BATCH-GAMEPLAY section 3, RESULT_PORT-BATCH-HUD).

| reason | rows |
|---|---|
| swapped (1) | 0x00496900 SlotActiveThunk |
| RWGLOBALS (1) | 0x004c3b30 FastSqrt |
| DUPFILE (2) | 0x00430830 SplitScreenTrackAssignment; 0x00432b30 MenuPromptStrip |
| not reached (15) | 0x0042ad10 FUN_0042ad10 [DUPFILE]; 0x0042add0 FUN_0042add0 [DUPFILE]; 0x0042c1a0 StateAdvance2to3 [CROSS]; 0x0042c1f0 FUN_0042c1f0 [CROSS]; 0x0042f6b0 MenuModeSync [DUPFILE]; 0x00430910 MenuOptionSlotGet [DUPFILE]; 0x004322c0 FUN_004322c0 [DUPFILE]; 0x004323c0 MenuCursorBack [DUPFILE]; 0x00432450 FUN_00432450 [none]; 0x004324a0 FUN_004324a0 [none]; 0x00432800 FUN_00432800 [none]; 0x00473220 FUN_00473220 [RWGLOBALS]; 0x004733b0 FUN_004733b0 [RWGLOBALS]; 0x00473ee0 LogoOverlayDraw [RWGLOBALS]; 0x00492d10 Global771968Get [CROSS] |

- `DUPFILE`: `re/match/s08/004322c0.c` holds 0x00430830 / 0x0042f6b0 / 0x00430910 / 0x004322c0, and
  `re/match/s08/00432b30.c` holds 0x00432b30 / 0x0042ad10 / 0x0042add0. Including either file compiles every body in
  it, and the exe already has bodies for some of them (0x00430830 and 0x0042f6b0 from `Match/MatchS07_*`, 0x0042ad10
  from `Match/MatchS06_0042ad10.cpp`, all byte-matched, plus the hand 0x00430910 / 0x004323c0 / 0x00432b30).
- `RWGLOBALS`: FastSqrt 0x004c3b30 reads through RwGlobals (`DAT_007d3ff8`, 0 in the exe pad).
- Not reached: the frontend-004 head 0x004324a0 and its callees 0x00432450 / 0x0042c1f0 / 0x0042c1a0 have no exe
  caller; the group counts as reached only through 0x00496900.

## 4. What would unblock the left-out rows (all five subsystems)

### Counts

The five default-reached sets hold 99 member rows (gameplay 25, hud 23, frontend 19, ai 16, render 16), **86
distinct** (0x0040b890 / 0x0040b8e0 / 0x0046c7b0 / 0x0046d4a0 / 0x004c3b30 / 0x004c3bf0 and others sit in several).
One reason per distinct row:

| reason | distinct rows | rows |
|---|---|---|
| swapped (this route) | 6 | 0x00407b00, 0x00407b70, 0x00407d70, 0x00407db0, 0x005c9d00, 0x00496900 |
| already in the exe behind MASHED_BATCH_HUD (L-106 (b)) | 4 | 0x0041d870, 0x0041e850, 0x0041d410, 0x0041e630 |
| port-side value | 9 | VEHBASE 0x0046c7b0, 0x0046cbb0, 0x0046d4a0, 0x0046c730, 0x0046c750, 0x0046d780, 0x0046d7f0; 0x00417cf0 (calls 0x0046d4a0); 0x0041f360 (pinned constant) |
| stand-in | 5 | 0x0045c110, 0x0045bff0, 0x00451820, 0x0041db80, 0x005554d0 |
| naked x87 transcription | 4 | 0x004233e0, 0x00414300, 0x00414490, 0x00414a70 |
| RWGLOBALS | 3 | 0x00403160, 0x004c3b30, 0x004c3bf0 |
| caller-supplied inputs | 2 | 0x0040b890, 0x0040b8e0 |
| DUPFILE | 2 | 0x00430830, 0x00432b30 |
| not reached (no exe caller on the default path) | 51 | own blocker: CROSS 14, RWGLOBALS 7, DUPFILE 6, SEED 3, DUP 1, none 20 |

So the default path reaches **35** of the 86 rows. 6 are swapped and 4 are already in behind a switch, which leaves
**25 reachable rows blocked**. The other 51 are in the sets because the tool closes every group that has any reached
member, including groups reached only through a callee (hud-002, render-001..004, frontend-004, gameplay-001..006).
Swapping those 51 compiles bodies nothing calls (`body`, not `called`): they need an exe caller, which means porting
their caller path (mostly the original's HUD / panel draw code, which the exe replaces with librw), not a swap. CROSS
and SEED appear only among those 51, plus as a second blocker on 4 stand-in / RwGlobals rows.

### The 3 cheapest unblocks, by reachable rows released

1. **VEHBASE binding (data addressing, ledger MATCH-S11-FU-VEHBASE): up to 8 rows.** The matched sources address the
   original record array at 0x008815a4; the exe arms read the port's `g_vehicleArrayBase` with null-base guards
   (verify/matchs11_20261102/RESULT_MATCH-S11.md NOT-SAFE table). Binding the matched sources' array global to
   `g_vehicleArrayBase` in the wrappers (the MATCH_CELL pattern of `Match/MatchS06_004282a0.cpp`) releases the five
   read-only leaves 0x0046c7b0, 0x0046cbb0, 0x0046d4a0, 0x0046c730, 0x0046c750, if a null base never reaches them on
   the gated paths. 0x0046d780 also needs the two FUN_00422b50 calls the exe skips, 0x0046d7f0 a producer for
   `g_launchAccelByte`, and 0x00417cf0 the VEHBASE row plus a resolution of its file's second 0x00417730 body. It is
   one decision (the open VEHBASE row) and one binding header, no caller rewrite.
2. **RwGlobals producer at 0x007d3ff8: 3 rows, and the RwGlobals blocker of 7 unreached rows.** The exe pad holds 0
   there. A table at 0x007d3ff8 whose slots point at the exe / librw functions the matched bodies call through it
   (memory: DAT_007d3ff8 is RwGlobals; validate each slot by its call) releases 0x00403160, FastSqrt 0x004c3b30 and
   Vec2Length 0x004c3bf0, and removes the blocker from 0x004c0e50, 0x004c13e0, 0x004c1480, 0x004c3ac0, 0x00473220,
   0x004733b0, 0x00473ee0. FastSqrt and Vec2Length are hot math leaves, so expect a gate move to bisect.
3. **Threshold-getter inputs (caller rewrite): 2 rows, and the shared blocker of 5 HUD / render groups.** The exe
   callers pass `GridCount8or12Core(cars, ea64, fd0)` / `ScoreThreshold7or10Core(...)` the standalone's values
   (`exe_main.cpp:4453/4455`, `LibRw/RwRaceFx.cpp:1473/1476`). The zero-arg matched 0x0040b890 / 0x0040b8e0 read
   `FUN_0040e340`, `FUN_0042f500` and 0x007f0fd0 themselves. If the exe publishes its mode at 0x007f0fd0 and the two
   getters return the values the callers pass today, the callers can call the matched bodies. That releases both rows
   and drops the blocker from hud-002 and render-001..004 (render-004, 0x0041c380 / 0x0041cb80, then has no blocker
   left but still no caller).

Next after those: the 5 stand-ins are one port each (collision material colour, pickup slot table, librw start
lights, font measure, pontoon object), and the 4 naked x87 rows need a precision decision (VS2022-compiled C in place
of an x87 transcription kept for bit identity), which will move G-CTRL. The volume the route was expected to bring
sits mostly in the 51 unreached rows, and a swap cannot release them.

## Evidence (raw data in `D:/MashedWT/PORT-BATCH-FRONTEND_data`, not committed)

| file | SHA-256 |
|---|---|
| exe_batch/mashed_re.exe (ON, = 28fd12729) | b5598a02643f0078474dc515429a64de7ba0929c3d7b7d82b86b6b9e69dcc79e |
| OFF exe: PORT-BATCH-GAMEPLAY_data/exe_batch/mashed_re.exe | 9fe71a2e7af0e13deb9187a56d9fae157031424cca1ec118ef25163ecf409935 |
| build_batch.log (Build OK, rva-lint NEW=0) | ee3dfeb94b094dd2ad0988059b4bdae592d5706c59e5d605e1e044492af8aae3 |
| gate/02_on_all/arms/H_DEF.step.csv | 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198 |
| gate/02_on_all/arms/H_DEF.gates.csv | 6793183492e0c0822570ad71254f9fdf27568b77c81a25c56347f1616863d58c |
| gate/02_on_all/arms/H_DEF.ptrace.log | 31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3 |
| gate/02_on_all/arms/B_DEF_K0.step.csv (K1, K26 identical) | 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 |
| gate/04_on_all2/arms/H_DEF.step.csv | 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198 |
| gate/02_on_all/navdemo.log | 5e789f98e0af2b2c02370b5fce2f139c334457ebba684e0672e70e279ca7a636 |
| gate/passes.tsv (all 4 passes incl. nav hashes) | 9d4475807851ec4c54068583c9c21a6e4900e2b76ec7d22723edad0973056d17 |
| gate_run.log | 7526a914b6bd47862d6c9582d9984826683d2970dc99067edaf9948c40694651 |

The counts in section 4 come from `verify/portbatch_20261109/classify_leftout.py` (committed; `--tables` prints the
per-subsystem grouped tables). Its per-row reasons are the hand review of RESULT_PORT-BATCH-GAMEPLAY section 3 and the
three 0-row RESULTs.
