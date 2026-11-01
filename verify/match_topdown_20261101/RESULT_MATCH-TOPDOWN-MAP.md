# RESULT MATCH-TOPDOWN-MAP (2026-11-01)

Verdict: **GREEN.** Static map delivered. 13 swap rows cut into the ledger (MATCH-TOPDOWN-T01..T13).
No game, no build, no test slot, no tracker edit.

L-72 (Mariano 2026-11-01): MATCH goes top-down. Matched callers get swapped in, walking up the original
call graph until the port's entry points run the original chain. This row maps where that walk starts,
what each swap unlocks, and what blocks it.

## Headline

| fact | value |
|---|---|
| matched (byte-identical: PILOT + S01..S04 results.tsv) | 461 (417 game, 44 in platform subsystems boot/audio/input) |
| matched with any exe body (rva_body_scan, exe target) | 110 |
| matched with **no exe body at all** | 351 |
| matched already swapped into the exe (`Match/` TU) | 53, **0 of them reached from WinMain** (the L-68 finding, reproduced statically) |
| matched whose exe body is reached from `_WinMain@16` | 23 (all port-only bodies, none is the matched source) |
| hooks.csv rows with an RVA-bound exe body | 805 bodies, 184 reached, 228 not found in the /MAP (inlined or renamed, counted unreached) |

The port runs the game through RVA-less port code. The original's entry chain has no RVA-bound exe body at
any level: main loop `0x00492290` (boot) calls logic tick `0x00492d30` (7-case state machine on
`DAT_00771968`), which calls race frame `0x004111c0` -> `Race::Tick` `0x0040fc00`, the frontend machine
`0x0043dfd0`, the pause menu `0x0043d7c0`. FrameDispatch `0x00492e90` (boot) runs the render/HUD
dispatchers (`HudIngameDispatch` `0x0040dfc0`, `0x00410b30`, `PerModeRenderMachine` `0x00404320`,
`MenuDrawLoop` `0x0043c5b0`). The port code that plays these is found by RVA mention only
(`Race/GameFlow.cpp`, `exe_main.cpp`, `D3d9Render/TrackRenderer.cpp`, `Ai/AiStandalone.cpp`,
`Boot/GameStateCluster.cpp`, `Boot/FrameDispatch.cpp`). 410 of the 438 matched functions the exe does not run
have no reached ancestor except these tops: `0x00492d30` holds 215 of them, `0x004929d0` (game-mode
machine) 42.

## Method (static, reproducible)

1. `cg_build.py` builds two call graphs.
   - Original `MASHED.exe.unpatched` (SHA-256 `bdcae093...efd3c0e`, the anchor). Nodes = the 5,948 hooks.csv
     RVAs in `.text`. Edges from a linear capstone sweep of each node: E8 calls (15,907), E9/Jcc tail jumps
     (451), imm32 function pointers (462), IAT imports (568). Plus 1,062 installer edges from
     `re/analysis/pointer_callers_2026-10-11.tsv` (vtable, fn_table, callback_arg, store_mem, ...).
   - Port `mashed_re.exe` built from orch/integration `0e3d71a75` (HB-ARCTIC-NOLIGHT-CAP worktree build,
     0 commits ahead/behind, clean `mashedmod/`) with its /MAP. Edges = rel32 calls/jumps + every `.reloc`
     site, symbol to symbol. Reach = BFS from `_WinMain@16`. The CRT initializer table is not a root, so a
     body reached only through the `s_AutoReg_*` hook registry is not "called". Cross-check: S01's by-name
     exe-caller column (0 for CupSpinSpeedAndColor, EntryTableScanByKey, ...) agrees with reach=False.
2. `topdown_rank.py` joins them. RVA -> exe body via `scripts/rva_body_scan.py` bound to its /MAP symbol.
   Platform = third-party library rows, the audio/input/boot/video subsystems, file/piz I/O by name, imports:
   descent stops there and the call is listed under its port bridge.
   - **Root**: a matched node. Its unlock set U = matched nodes the exe does not run, reachable from it
     through such nodes only. Only maximal roots kept (269).
   - **Connector**: the cheapest path upward (fewest unmatched nodes) from the root to an anchor: a node the
     exe already runs, or an original top (no game caller: its port counterpart is RVA-less port code).
   - **Swap unit** = (call site E, anchor): E is the root's parent on the connector, the function whose port
     counterpart must call the matched chain. Blocking gaps = unmatched nodes on the connector below the
     anchor (E included when unmatched). Deep gaps = unmatched callees inside U (the chain calls their
     port body, or needs one). 148 units, greedy set cover so each matched function counts once.
   - x87 = the original function holds an x87 FPU instruction (capstone group FPU): its wrapper TU goes in
     `mashedmod/x87_tus.txt`.
   - Deterministic: two runs with PYTHONHASHSEED 1 and 2 give identical TSVs.

## Ranked swaps cut into the ledger

U = matched functions unlocked (become called, C4-eligible together). new = matched functions with no exe
body (the swap adds the body). repl = port-only exe bodies replaced. sw = already swapped, still unreached.
Full columns (connector path, deep-gap list, platform calls, every RVA): `topdown_ranked.tsv`.

| row | call site (E) | entry point / port site | U (roots) | new / repl / sw | blocking gaps | deep gaps | x87 |
|---|---|---|---|---|---|---|---|
| T01 | `Race::Tick` 0x0040fc00 | logic tick 0x00492d30 > race frame 0x004111c0; port RVA-less (TrackRenderer.cpp x38, AiStandalone.cpp) | 50 (24) | 38 / 4 / 8 | 0x0040fc00, 0x004111c0 (both S02 "not attempted", over 300 B) | 69 (0x0040be50 hides 9) | 7 |
| T02 | race frame 0x004111c0 | logic tick 0x00492d30; port RVA-less (TrackRenderer.cpp x21, SubStateMachine.cpp) | 38 (5: TimeTrialRecordPlayback 0x00411170, SubStateHeldCountdown5 0x004103a0, 0x0040de00, 0x004102f0, SubStateEnter4 0x0040dbd0) | 29 / 3 / 6 | 0x004111c0 | 26 (0x00448220 hides 15, 0x00410d10 14, 0x00418560 14) | 9 |
| T03 | `HudIngameDispatch` 0x0040dfc0 | FrameDispatch 0x00492e90 (Boot/FrameDispatch.cpp); body is Match/MatchS02_0040dfc0.cpp, unreached | 17 (1) | 8 / 2 / 7 | none | 10 | 2 |
| T04 | render frame 0x00410b30 | FrameDispatch 0x00492e90; port RVA-less (TrackRenderer.cpp, RwRaceSubmit.cpp, TrackHookSlots.cpp) | 16 (1) | 16 / 1 / 0 | none | 44 (0x00429e10 hides 11) | 1 |
| T05 | track-node dispatch 0x00426e10 | game-mode machine 0x004929d0 (Boot/GameStateCluster.cpp) > 0x0040d440 > Course::Finish 0x0040d270 > 0x0040d020 | 12 (11) | 12 / 0 / 0 | 0x0040d270 (S02 not attempted), 0x00426e10 (S06) | 10 | 0 |
| T06 | 0x00409a80 (save/gameplay) | no static caller, no exe mention: the indirect installer must be found first | 10 (1) | 5 / 6 / 0 | none | 5 | 1 |
| T07 | pause menu 0x0043d7c0 | logic tick 0x00492d30; port RVA-less (exe_main.cpp x4, MenuNavSM.cpp) | 9 (4) | 12 / 0 / 0 | 0x0043d7c0 (S08) | 10 | 2 |
| T08 | `PerModeRenderMachine` 0x00404320 | FrameDispatch 0x00492e90; no exe mention | 8 (1) | 8 / 1 / 0 | none | 5 | 4 |
| T09 | 0x0040e590 | race frame 0x004111c0 > 0x004102f0; port RVA-less (TrackRenderer.cpp x7) | 8 (8) | 5 / 0 / 3 | 0x0040e590 (S02 not attempted, 2,360 B), 0x004111c0 | 0 | 1 |
| T10 | game-mode machine 0x004929d0 | main loop 0x00492290; port RVA-less (Boot/GameStateCluster.cpp x5) | 7 (7) | 6 / 0 / 2 | none | 4 (Course::Finish 0x0040d270 hides 31) | 0 |
| T11 | menu 0x00434720 | MenuDrawLoop 0x0043c5b0 > 0x0043bf30; port RVA-less (Frontend/Cluster_v1.cpp, exe_main.cpp x11) | 7 (7) | 2 / 0 / 6 | 0x00434720, 0x0043bf30 (S08) | 0 | 1 |
| T12 | AI 0x00416250 | race frame 0x004111c0 > SubStateHeldCountdown5 0x004103a0 > 0x00418860 > 0x00418560; port RVA-less (Ai/AiStandalone.cpp x23) | 7 (6, incl. 0x00414a70) | 4 / 3 / 0 | 0x004111c0, 0x00418560 (no S03 result), 0x00416250 (S03 58.2%) | 5 | 5 |
| T13 | ready-now batch: 18 units whose call site the exe already runs | 0x00407e20 (4), 0x00408610, 0x0040dbd0, 0x00414570, 0x00414c30 (2 each), 13 singletons (PathProgStep, AiTargetClosest, PickupTick, RuleEngine, ForceIntegrator, VehicleInit, CameraEntries, LaunchRevCharge, WarzonePontoon, MenuButtonDetect, AiLeafTargets) | 25 | 7 / 20 / 0 | none | 0 | - |

T13 is not one subtree. It batches the 18 units with zero gaps whose call site has a reached port body, so
the swap is a body replacement plus a by-name call from that reached body. **11 of its 27 RVAs already have
a live ledger owner** (2026-11-01 15:23Z): D4-DC-INC-00407580 / -00407b00 / -00407b70 / -00407d70 / -00407db0 /
-00408a50 / -00408af0 / -0040e370 / -0040e470 (+ -X), -00426d00, MATCH-0040E370-NAKED, HB-LINT-408A50, and
0x0040e480 sits in the parked D4-DC-77b0-RUNTIME-PARKED. The T13 row carries the other 16:
0x00407a20 0x00407cd0 0x0040cda0 0x0040ce80 0x0040e180 0x0040e350 0x00414490 0x004173a0 0x0041ae20 0x0041b520
0x0041f030 0x0041f1e0 0x00422b50 0x0046c5f0 0x0046d510 0x0046dbe0. These are the cheapest C4 now,
but S02/S03 left them unswapped for a stated reason: the exe bodies are adapted to port state
(MATCH-S02 "Not swapped": 0x0040b890/0x0040b8e0 "3-arg core fed port state", 0x004103a0/0x0040dbd0 "exe arms
skip callees, use port host objects, carry knobs", 0x00410510 "C++ core over a virtual EvalResultInputs
interface fed the port snapshot"; MATCH-S03: 0x00414a70 "exe arm is a verbatim naked-x87 pair",
MATCH-S04: 0x0041f360 reads the port const `g_dat_00636ad0`). The same blocker applies to every row: the
matched chain reads the original globals (relocheck-checked `MGLOBAL`s), so a swap is sound only where the
port keeps those globals live, not port host objects. A call graph does not show that; each Tnn checks it.

Gate risk: T01, T02, T09, T12 and most of T13 sit in the race frame (sub-state machine, AI targets,
ForceIntegrator, VehicleInit). A swap there that moves G-CTRL / B / D2 / 13-track is a port divergence
surfacing: per L-72 batch it per subtree as a decided flip with before/after evidence and an L-n
recommending (a) accept, refs refresh. T03, T04, T08, T11 are HUD/render/menu and are not in the gate
step data, but the draw-list parity harness applies.

## The rest (not cut; full list in topdown_ranked.tsv)

148 units cover all 417 matched game functions. T01-T12 = 189, T13 = 25. The remaining 203 sit in 118
units of 1 to 6 functions. The largest:

| unit | call site | U | blocking gaps |
|---|---|---|---|
| 13 | 0x0041e6c0 (under Race::Tick) | 6 | 0x0041e6c0 (S04 98.8%), 0x0040fc00, 0x004111c0 |
| 14 | frontend machine 0x0043dfd0 (Race/RaceModes.cpp, unreached) | 5 | 0x0043dfd0 (S08) |
| 15 | 0x00459620 (under 0x004538b0, Powerup/PowerupAim.cpp) | 5 | 0x00459620 (S09) |
| 16 | Ghost::PlaybackTick 0x00411ae0 | 5 | 0x00411ae0, 0x004111c0 |
| 17 | 0x00426ab0 (under Race::Tick) | 5 | 0x00426ab0, 0x0040fc00, 0x004111c0 |
| 19 | 0x0041a060 (matched top) | 4 | none |
| 20 | 0x00426b40 (top) | 4 | none |
| 21-24 | Course::Finish 0x0040d270, 0x00419530, CourseRenderFrame 0x004270f0, 0x00456140 | 4 each | 1 each |

45 units are matched tops with no static caller (115 functions, 0 connector gaps). Their callers are indirect
(register or vtable calls in boot code), so each needs its port call site identified by hand. T03, T04, T06,
T08 are the four largest.

## Gap priority for S05-S28 ("unmatched gaps under a matched caller go first")

`gap_value.tsv`: 196 unmatched game functions, ranked by the matched functions directly under them through
matched-only chains. The top of the list is **inside slices already run**: S02 skipped its 15 functions over
300 B, and those include the whole race chain. Top 16:

| gap | name | sub | exe body | matched below | slice |
|---|---|---|---|---|---|
| 0x0040fc00 | Race::Tick | util | - | 64 | S02 (not attempted, 621 B) |
| 0x004111c0 | race frame | util | - | 35 | S02 (not attempted, 400 B) |
| 0x0043d7c0 | pause menu | util | - | 14 | S08 |
| 0x00426e10 | track-node dispatch | render | - | 13 | S06 |
| 0x0043dfd0 | frontend machine | frontend | Race/RaceModes.cpp | 13 | S08 |
| 0x00434720 | menu | frontend | - | 12 | S08 |
| 0x004929d0 | game-mode machine | util | - | 12 | S15 |
| 0x0041e6c0 | | hud | - | 10 | S04 (98.8%) |
| 0x0040e590 | | util | - | 9 | S02 (not attempted, 2,360 B) |
| 0x00429310 | TimeTrial::Tick | util | - | 9 | S06 |
| 0x0042c960 | CameraTransitionStateMachine | render | - | 8 | S07 |
| 0x00416250 | | ai | - | 7 | S03 (58.2%) |
| 0x00429e10 | | render | - | 7 | S06 |
| 0x00459620 | | gameplay | Powerup/PowerupAim.cpp | 7 | S09 |
| 0x00406ae0 | | gameplay | - | 6 | S01 (91.8%) |
| 0x00410d10 | | vehicle | Race/RaceCamera.cpp | 6 | S02 (94.9%) |

Slice demand in the top 40 gaps: S06 8, S03 8, S02 7, S08 6, S05 4, S09 3. A residue row for S02's
not-attempted 0x0040fc00 / 0x004111c0 / 0x0040e590 / 0x0040d270 unblocks T01, T02, T05, T09 at once.

## Platform calls and the bridge each maps to

Calls out of the 148 units' matched sets, by bridge (count of call edges; top callees):

| bridge | calls | callees |
|---|---|---|
| librw renderer (LibRw/RwBridge, rw shims) | 66 | RpClumpDestroy, 0x004c0b30, 0x0053fbb0, 0x0053d090, 0x004e6d80 |
| platform CRT (L-67 waived) | 45 | 0x004a2c48, 0x004b3fc0, sprintf, printf, 0x004b3f90 |
| window/boot (exe_main, Boot/*) | 18 | __security_check_cookie, 0x0040bb30, 0x004c5930 |
| RW Physics 3.7 library (port Physics TUs, x87) | 11 | 0x00562560, 0x00558d30, 0x005592f0, 0x0055afc0, 0x0055af80 |
| file/piz I/O (port Util file layer) | 4 | FileReadWrapper_i3, FileWriteWrapper_i3, SaveFileExists |
| input (port Input/*) | 3 | 0x004987b0 |
| audio backend (port Audio/*) | 3 | 0x004661a0, 0x00465e80 |
| window/boot (user32) | 3 | GetDlgItem, LoadStringA, SetWindowTextA |
| qhull 2002.1 (QhullBridge, libid) | 1 | 0x0057c4b0 |
| not in hooks.csv | 6 | 0x0040bb10, 0x004936f0, 0x00493700, 0x004827a0, 0x004827c0 (call targets with no function row) |

A matched source calls each of these as `FUN_<rva>`. The swap binds it to the port's bridge function for that
RVA (librw entry, port Audio/Input body, CRT). Where the port has no RVA-bound body for the callee, the wrapper
has to name the bridge symbol, as the S01 wrappers do (`FUN_<rva>` mapped to the export name).

## Limits

- Indirect calls through registers or heap vtables are edges only when `pointer_callers` found the installer.
  That is why 45 matched tops show no static caller. Boot-subsystem callers are platform by rule, so the
  callers of the tops show up only in the `entry_callers` column.
- Reach counts an RVA as called only through its RVA-bound exe body. 228 bodies have no /MAP symbol (inlined
  or renamed) and count as unreached. RVA-less port code that reimplements an RVA is invisible to the
  one-body lint and is the real dual copy each Tnn removes. The port site for RVA-less code comes from RVA
  mentions in exe TUs, a pointer for the worker, not proof.
- `pointer_callers_2026-10-11.tsv` predates some function-boundary repairs. hooks.csv function starts are taken
  as they are (5,948 in `.text`).
- The matched set is S01-S04 plus PILOT only. MATCH-S05 (working) and S06 (claimed) are not merged, so their
  matches are not in this map. Re-run both scripts after each slice merges: the ranking moves.

## Reproduce

```
py -3.12 verify/match_topdown_20261101/cg_build.py <out>/cg.json <mashed_re.exe> <mashed_re.map>
py -3.12 verify/match_topdown_20261101/topdown_rank.py <out>/cg.json <out_dir>
```

Committed: `cg_build.py`, `topdown_rank.py`, `topdown_ranked.tsv` (148 units), `gap_value.tsv` (196 gaps),
`reached_matched.tsv` (23), `summary.json`.

Raw data (D:/MashedWT/MATCH-TOPDOWN-MAP_data, not committed):
```
50521efb7b9168068fb115fb6b623ab1e72d86eeabe9cee6e0c6d1fd0c7faeab  cg.json (both graphs)
19e494fe89cea5116850c6420fd2eadee808661f02313c8f258c5fe7cdfe5915  mashed_re.exe (build of 0e3d71a75)
ae06e4eff669d1bb17d8b39c8e22e97858894a9bb38db1d7681ea02aaf41494b  mashed_re.map
c14db1e1a1518750e82bff362441c4e86a4c3b3ec8952c510c1e98cac316fcf3  topdown_roots.tsv (269 maximal roots)
bdcae093a30fbf226bdd852b9c36798a987aee33b3ae82bf7404b0336efd3c0e  original/MASHED.exe.unpatched (anchor)
```

## Next

MATCH-TOPDOWN-T01..T13 in the ledger (wave 4). Suggested order: first an S02 residue pass on 0x0040fc00,
0x004111c0, 0x0040e590 and 0x0040d270, which unblocks T01, T02, T05 and T09. In parallel, T03, T04, T08 and
T10 (0 gaps: wire the port call site, swap the chain) and T13 (0 gaps, port-state check per RVA).
