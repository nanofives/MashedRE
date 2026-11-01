# RESULT MATCH-TOPDOWN-T03 (2026-11-01)

Verdict: **GREEN.** 1 of the 17 RVAs swapped in and called by `mashed_re.exe`: the subtree's top,
`HudIngameDispatch` 0x0040dfc0. The other 16 are left, each with a one-line reason below. One branch, one build.
Default gates unchanged on this build (G-CTRL 8284be46 / ba407a16 / 31fe043e, B 1d5bf4f9 x3 = merge_train.json refs).

L-72 (Mariano 2026-11-01): swap matched callers in, top-down. T03 is the HUD unit of
`verify/match_topdown_20261101/RESULT_MATCH-TOPDOWN-MAP.md` (call site E = 0x0040dfc0, called by FrameDispatch
0x00492e90). The soundness rule is T13's: the matched source reads the original's globals, so a swap is sound only
where the exe keeps those addresses live. In the exe the original range is zero-filled exe memory (`exe_main.cpp`
B17 image pad, `g_b17_low_arena_pad`), the original's `.rdata` constants included, so an address no exe TU writes
reads 0.

## Where the exe draws the HUD

`Boot/FrameDispatch.cpp` holds no body for 0x00492e90 (its bodies are the small leaves and `MainLoopInit`). The exe's
in-race HUD is `LibRw::RaceFx_RenderHud` (`LibRw/RwRaceFx.cpp`), called by `RaceSubmit_Render` after the race camera's
`endUpdate` (`LibRw/RwRaceSubmit.cpp:2037`). Its inputs are `HudInputs`, filled each frame by `TrackRenderer.cpp:9874`
from the live words (DAT_0063ba8c, DAT_0063d588, DAT_0067ea64, DAT_0067e9fc, DAT_007f0fd0, ...), with two port
stand-ins: `state` is the countdown stand-in while DAT_0063ba8c is 0, and `world_up` is always true. Before this
branch, `RaceFx_RenderHud` carried its own copy of 0x0040dfc0's branch (a `switch` on round type / mode / team play)
and drew the librw team panels, non-team panels and start lights from it. That copy was the RVA-less dual of the
matched source.

## The swap

`RaceFx_RenderHud` now calls the matched `HudIngameDispatch` (the body of `Match/MatchS02_0040dfc0.cpp`, source
`re/match/s02/0040dfc0.c`) inside its HUD camera pass, where the old `switch` chose the draws. The `switch` stays only
as the gate of the non-team records' logic (`NtSetup` / `NtState4` / `NtTick`), which in the original runs on the race
tick, not in this dispatcher. The exe arm of the wrapper (`#ifdef MASHED_STANDALONE`) binds the 13 callees; the asi
arm is unchanged (each forwarder still calls the original address).

| callee | exe binding | why |
|---|---|---|
| 0x0042f6a0 | `GetRaceSubMode` (Util/GameStateGetters.cpp, the exe's one body) | live: DAT_0067e9fc is written by the race launch (`exe_main.cpp:2126`, `:9792`) |
| 0x0042f500 | `GetDat0067ea64` (GameState/StateAccessors.cpp, the exe's one body) | live: DAT_0067ea64 (`exe_main.cpp:9999`) |
| 0x00426ba0 | `RaceFx_HudBindWorldUp` = the pass's `world_up` | 0x00426ba0 is `mov eax,[0x0066d704]`. 0x0066d704 is the course object (capstone over the anchored image: read at 0x00426640 / 0x00426670 / 0x004266b0 / 0x00426ba0 / 0x004270f0, indexed at 0x00426b94 / 0x004270af). No exe TU writes it, so the exe's own body `HudDrawEnabled` returns 0 and would turn the HUD off. The pass's `world_up` (true) is the port's existing stand-in for it |
| 0x0041c0c0 | `RaceFx_HudBindPanels` (librw `g_panel`, `RenderPanel`) | the exe's team panels are librw clumps, not the 0x0063cab8 records |
| 0x0041b630 | `RaceFx_HudBindNonTeam` (librw `RenderNonTeam`, `MASHED_HUD_NONTEAM=0` still turns it off) | the exe's non-team panels are `g_nt[4]` |
| 0x0041db80 | `RaceFx_HudBindLights` (librw `RenderLights(timer)`) | the exe's start lights are `g_lights` on DAT_0063d588 |
| 0x0041ccc0, 0x0041d870 | no-op | state 7 only. The exe draws the standings as its 2D overlay (`exe_main.cpp:4052`), and the pass calls the dispatcher for states 5 and 6 only, so they are not reached |
| 0x00403160, 0x0041a3e0, 0x0041c300, 0x0041ded0, 0x0041e850 | no-op | the per-mode HUDs. Not ported in the exe ([UNCERTAIN U-9704]); the old switch drew nothing for them either |

DAT_0063ba8c and DAT_007f0fd0 are read by the matched body itself (`MGLOBAL`), and both are live in the exe
(DAT_0063ba8c: `Race/SubStateMachine.cpp`, `TrackRenderer.cpp`; DAT_007f0fd0: `SetRaceRule`, `TrackRenderer.cpp:224`).

`MASHED_HUD_REPLAY` (diagnostic, the d1r1 / rfx parity recipe): the replayed row's state, mode, round type and team
words are written to their four addresses for the dispatcher call only and the exe's values put back after it, so the
parity harness still draws from the original's inputs, now through the matched body.

Where the draw output can differ from before: the matched body reads the real DAT_0063ba8c, the old switch read the
pass's `state`. They differ only when DAT_0063ba8c is 0 and the countdown stand-in says 5 or 6. Then the matched body
returns at its `DAT_0063ba8c < 5` test and draws nothing, as the original does at that state. The witness below
counts those frames.

## Per-RVA table

`match.py` = `re/tools/match/match.py <src> --toolchain ddk --bytes` (flags `/O2 /GS /Ob1`, cl 13.00.9176 /
c2 13.00.9178). `exe callers` = E8 `call` sites to the symbol in this branch's `mashed_re.exe`, counted by scanning
`.text` against the `/MAP` address.

| rva | outcome | matched source | match.py | relocheck | exe callers | reason / note |
|---|---|---|---|---|---|---|
| 0x0040dfc0 | **swapped, called** | `re/match/s02/0040dfc0.c` (c6274d547d7821c7) | 100.0% 84/84, 424 B identical (re-run on this branch) | OK checked=29 unchecked=0 (re-run) | 2 (`RaceFx_RenderHud` 0x0016c044, 0x0016c068: default arm, replay arm) | was in both targets already (MATCH-S02) and unreached; now the exe HUD pass calls it. Callee bindings above |
| 0x0041c0c0 | left | `re/match/s04/0041bc50.c` | 100% (S04) | OK (S04) | 0 | renders through each record's object +0x48 (`m48`) for the 2 records at 0x0063cab8. The exe keeps its team panels as librw clumps in `g_panel` and creates no objects in those records, so the matched body would draw nothing. Bound to the librw panels instead |
| 0x0041bc50 | left | `re/match/s04/0041bc50.c` | 100% (S04) | OK (S04) | 0 | static callee of 0x0041c0c0, same records |
| 0x0041b630 | left | `re/match/s04/0041b630.c` | 100% (S04) | OK (S04) | 0 | 4 records at 0x0063c8d0 (stride 0x74). The exe's matched 0x0041b520 writes positions there from DAT_005f3304, which reads 0 (T13), and the records' atomics are not created in the exe (`g_nt[4]` librw, `RwRaceFx.cpp`). Bound to librw `RenderNonTeam` |
| 0x0041b340 | left | `re/match/s03/h1_grp_0041ad60.c` | 100% (S03) | OK (S03) | 0 | 0x0041b630's per-record draw, same records |
| 0x0041db80 | left | `re/match/s04/0041db80.c` | 100% (S04) | OK (S04) | 0 | record at 0x0063d55c holds 6 RW object pointers called through +0x48, no exe writer, and the thresholds are `.rdata` floats 0x005cc35c / 0x005cd230 / 0x005cd234 / 0x005cd238 / 0x005ccdf4 that read 0 in the exe. Bound to librw `RenderLights` |
| 0x0041ccc0 | left | `re/match/s04/0041c9a0.c` | 100% (S04) | OK (S04) | 0 | state-7 standings, records at 0x0063ce20 not filled in the exe; the exe draws standings as a 2D overlay |
| 0x0041c9a0 | left | `re/match/s04/0041c9a0.c` | 100% (S04) | OK (S04) | 0 | static callee of 0x0041ccc0 |
| 0x0041d870 | left | `re/match/s04/0041d410.c` | 100% (S04) | OK (S04) | 0 | state-7 standings, records at 0x0063d298, no exe mention |
| 0x0041d410 | left | `re/match/s04/0041d410.c` | 100% (S04) | OK (S04) | 0 | static callee of 0x0041d870 |
| 0x0041c300 | left | `re/match/s04/0041c2d0.c` | 100% (S04) | OK (S04) | 0 | mode-5 HUD, guard DAT_0063cdbc / block 0x0063cda4, no exe writer; not ported (U-9704) |
| 0x0041c2d0 | left | `re/match/s04/0041c2d0.c` | 100% (S04) | OK (S04) | 0 | static callee of 0x0041c300 |
| 0x0041a3e0 | left | `re/match/s03/h1_0041a1e0.c` | 100% (S03) | OK (S03) | 0 | mode-10 HUD, block 0x0063c610 / guard 0x0063c628, no exe writer; not ported (U-9704) |
| 0x0041e850 | left | `re/match/s04/0041e850.c` | 100% (S04) | OK (S04) | 0 | round-type-2 HUD, manager at 0x0063d610, no exe mention; not ported (U-9704) |
| 0x0041e630 | left | `re/match/s04/0041e850.c` | 100% (S04) | OK (S04) | 0 | static callee of 0x0041e850 |
| 0x00403160 | left | `re/match/s01/00403160.c` | 100% (S01) | OK (S01) | 0 | round-type-0xb HUD: draws through the RW device slots of `*(0x007d3ff8)`, which in the exe is the B15 Im2D bridge's fake device, not librw; reads DAT_00771964. Not ported (U-9704) |
| 0x00402fb0 | left | `re/match/s01/00402fb0.c` | 100% (S01) | OK (S01) | 0 | callee of 0x00403160; reads DAT_00636afc, no exe mention |

Deep gaps (unmatched callees in the subtree, from the map): 0x0041ded0, 0x00426ba0, 0x00427e00, 0x00428760,
0x0042f500, 0x0042f6a0 and 4 more. 0x00426ba0 / 0x0042f500 / 0x0042f6a0 are bound as above; the rest sit under the
left rows.

## Build

Worktree `.worktrees/MATCH-TOPDOWN-T03`, `MASHED_NO_DEPLOY=1`, test-slot wrapped. `=== Build OK ===` both targets.
`[rva-lint] 25 finding(s) ... allowlisted=25 NEW=0`, `[asm-lint] OK`. Exe `/MAP`: `_HudIngameDispatch`
(MatchS02_0040dfc0.obj) at 0x001503e0. Source commit 71fd65a70.

## Gates

Runner `verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`, A4_ROOT
`D:/MashedWT/MATCH-TOPDOWN-T03_root` (robocopy of `original/`), A4_EXE = the build copied out first
(`D:/MashedWT/MATCH-TOPDOWN-T03_data/gates.sh`), wrapped in `test-slot` + `mashed_lock.py run --label MATCH-TOPDOWN-T03`.

| run | build | exe sha256/16 | G-CTRL step / gates / ptrace | B K0 / K1 / K26 | vs merge_train.json ref |
|---|---|---|---|---|---|
| g1 | source commit 71fd65a70 | `ab1a841eaaa09655` | 8284be46 / ba407a16 / 31fe043e | 1d5bf4f9 x3 | equal |

H_DEF self-exited in 128 s (13962 step lines), each B arm in 10-11 s (762 lines), exit 0 for all four. No gate hash
moved, so no DECISION. The gates are physics step data: they show the swap changed nothing the race logic computes,
not what the HUD draws (next section).

## Witness: the dispatcher's draw choice against the old switch

A scratch build (this branch's source plus `witness.diff`, not committed) kept the pre-swap `switch` beside the
dispatcher call. Each HUD pass compares the switch's choice (team panels / non-team panels / lights) with the
bindings the matched dispatcher actually calls, and logs every mismatch plus every 300th pass with DAT_0063ba8c and
the pass's `state`. Same runner and root, `mashed_lock.py run --solo` (`witness.sh`).

| arm | HUD passes | mismatches | states seen (state / DAT_0063ba8c) | inputs seen |
|---|---|---|---|---|
| H_DEF | 9,301 to 9,600 (every 300th logged, last 9301) | 0 | 5/5, 6/6 | round 10, mode 0, team 0: non-team + lights |
| B_DEF_K0 | 301 to 600 (last logged 301) | 0 | 5/5, 6/6 | round 10, mode 0, team 0: non-team + lights |

No pass had DAT_0063ba8c = 0 under a stand-in state, the one input where the swap can change the draw. The witness
exe's H_DEF self-exited at 105 s (9758 step lines) against 128 s on the gate build; its first 9757 lines are
byte-identical to the gate run's (sha256 of the prefix `429eec94...` on both), so the shorter run is the wall-clock
exit, not a divergence. B_DEF_K0 on the witness exe: `1d5bf4f9`, the ref.

Coverage limit: both arms run round type 10 with Team Play off, so the team-panel arm (0x0041c0c0) and the
mode-gated arms are exercised only through the static argument above, not by a run.

## Render parity

Not measured with the draw-list harness. The in-race HUD parity recipe (`verify/rfx2_20261016/run_arm.py`, the d1r1
matched-pose replay under `MASHED_HUD_REPLAY`) was not run on this branch. What stands for it: the static argument
(same branch, same four input words, the replay route feeds the replayed words to the dispatcher), and the witness
above (0 mismatches over every default-route HUD pass of H_DEF and B_DEF_K0). The train's `-Full` 13-track table
(`run13`) is not triggered by this branch's paths (`LibRw/`, `Match/` are outside `full_set_paths`).

## Prepared re-classify (C4 by match)

For the `re-classify` leg after merge. Route `match`, toolchain ddk (cl 13.00.9176 / c2 13.00.9178), flags
`/O2 /GS /Ob1`, gates unchanged on this branch's build (G-CTRL 8284be46/ba407a16/31fe043e, B 1d5bf4f9 x3), evidence
`verify/match_topdown_20261101/RESULT_MATCH-TOPDOWN-T03.md`.

| rva | name | level | file | notes text |
|---|---|---|---|---|
| 0x0040dfc0 | HudIngameDispatch | C3 -> C4 | `mashedmod/src/mashed_re/Match/MatchS02_0040dfc0.cpp` (unchanged) | `match evidence: identical (re/match/s02/0040dfc0.c c6274d547d7821c7, match.py 100% 84/84, 424 B), relocheck OK checked=29 unchecked=0, swapped, exe-callers=2 (RaceFx_RenderHud, LibRw/RwRaceFx.cpp), gates unchanged (MATCH-TOPDOWN-T03). Exe callee bindings: 0x0042f6a0/0x0042f500 exe bodies, 0x00426ba0 = HUD pass world_up (DAT_0066d704 course object not kept in the exe), 0x0041c0c0/0x0041b630/0x0041db80 = librw HUD pieces, state-7 and per-mode callees no-op (U-9704)` |

For the lead: the C4 letter holds (identical, called, gates unchanged), but the body decides over callees of which
three are librw stand-ins, one (0x00426ba0) is a port stand-in value, and seven are no-ops in the exe. If that is
not C4 under L-68's spirit, keep it at C3 with the same notes text; the swap itself stands either way.
The 16 left rows: no level change.

## Raw data (D:/MashedWT/MATCH-TOPDOWN-T03_data, not committed)

```
ab1a841eaaa09655926d9ec29ac518268b5dfa519d7d9fbcf06e33410ba925f1  exe_g1/mashed_re.exe
9f5f68ee59df615c4dcfaa7ec275c7f47d51c22e242632e987ed1676034e61e0  build1.log
d303834b720db9671cf7386c4395384d4527f87fa9d4c00595d36bbb9ae72e63  gates_g1.log
8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198  arms_g1/H_DEF.step.csv
ba407a169a8ec433f486492814e7e450e32c6c48be5f1a0e38e9c53074db1ceb  arms_g1/H_DEF.gates.csv
31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3  arms_g1/H_DEF.ptrace.log
1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457  arms_g1/B_DEF_K0.step.csv
1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457  arms_g1/B_DEF_K1.step.csv
1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457  arms_g1/B_DEF_K26.step.csv
6d4339513b46e5e0d95784f05e07de912ab39070bebd898ac888eeb3271eea33  arms_g1/H_DEF.log
038808e656d34fb4951ce6671ec1904dc3ce378acf2142504dc9ed6bac93397c  witness.diff (scratch patch, not committed)
a4abe137d7674ecfa52f9b9a95f2f79c2777c161a3709e8c52501bfa91df10b1  exe_w1/mashed_re.exe (witness build)
afd221aab66ffedb294666597187fd9d49ed6de90b8b7a436bcd3d5d9a36d226  arms_w1/t03_witness_H_DEF.txt
f8b1aa2392b5bad065cb36222c5cd5a8cc43ba05c53f19c04538a49bf58331e3  arms_w1/t03_witness_B_DEF_K0.txt
b357937747fef28b950a3b978698859d59ae4249a06228154aa3dde9a0f6ae64  arms_w1/H_DEF.step.csv (witness build, 9758 lines)
ee84cfb916ff65c81daffa4b2ae8a0967448efb56c1fedcd487e3f9b728c32f5  witness_w1.log
```
