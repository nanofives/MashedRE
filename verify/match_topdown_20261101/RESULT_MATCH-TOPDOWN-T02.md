# RESULT MATCH-TOPDOWN-T02 (2026-11-01)

Verdict: **GREEN, no swap.** 0 of 38 swapped; 0x0041d910 and 0x0041da90 are already C4. The other 36 are left, each with a reason. No T02-specific source change; the branch build carries T01 / T08 swaps.

T01, T02, T09, T12 and T08 share the branch `orch/MATCH-TOPDOWN-T01` (one build, one gate run). T01 / T02 / T09 / T12 hang off Race::Tick 0x0040fc00 / race frame 0x004111c0; T08's one swap edits `AiPreTickRubberBand`, a race-frame function, so it rides the same build.

L-72 (Mariano 2026-11-01). T02 = map unit 2: the race frame 0x004111c0 (logic tick 0x00492d30), 38 matched functions in 5 roots, 9 x87. Blocking gap 0x004111c0 (S02 not attempted, 400 B, no source).

## Method

`re/tools/match/match.py <src> --toolchain ddk --bytes` (cl 13.00.9176 / c2 13.00.9178, `/O2 /GS /Ob1`) and
`re/tools/match/relocheck.py`, re-run 2026-11-01 on every source (`D:/MashedWT/MATCH-TOPDOWN-T01_data/t02.tsv`). `exe body`
= `scripts/rva_body_scan.py`. `exe callers` = call edges to that symbol in the baseline `mashed_re.exe` of
orch/integration `3a3186c4a` (/MAP + `verify/match_topdown_20261101/cg_build.py`), the `s_AutoReg_*` entry not
counted; the two swapped rows give this branch's count. `orig callers` = the map's original call graph; the bracket
says whether that caller has an exe body (`none`, `unreached`, `reached`, `port body` = a body whose symbol the /MAP
lookup did not bind). A default reason "no exe call site" means no original caller has an exe body that could make
the call; "state ... has no exe TU reference" lists original addresses no exe TU names (zero in the exe, T13 RESULT).

The blocker for the whole race group is the same as T05 / T10: the exe runs the race frame and Race::Tick as
RVA-less port code (`D3d9Render/TrackRenderer.cpp`, `Race/SubStateMachine.cpp`, `Ai/AiStandalone.cpp`), whose state
lives in port objects. A matched body is a sound swap only where an exe port function already does the same work on
the same live original addresses. Two such sites exist in this group: `PathProg_RaceInit` (0x00407a60) and
`AiPreTickRubberBand`'s reads of `0x0089a880` (0x00417730, T08).

## Per-RVA table

| rva | outcome | matched source | match.py | relocheck | exe body | exe callers | orig callers [exe body of caller] | reason |
|---|---|---|---|---|---|---|---|---|
| 0x00405430 | left | `re/match/s01/004053d0.c` | 100.0% 13/13, 39 B identical | OK 0/3 | none (asi `Pred405430`) | 0 | 0x0040d470[none] | no exe call site: every original caller has no exe body (gap or unswapped); state `0x00639d74` has no exe TU reference |
| 0x00405460 | left | `re/match/s01/00405460.c` | 100.0% 65/65, 223 B identical | OK 8/10 | none | 0 | 0x004102f0[none] | no exe call site: every original caller has no exe body (gap or unswapped); state `0x00639d74`, `0x00803344` has no exe TU reference |
| 0x00407a00 | left | `re/match/s01/raw/00407a00.c` | 100.0% 6/6, 24 B identical | OK 0/1 | `ReadAiCheckpointField`, `Gameplay/RangeTable_ah1.cpp` | 0 | 0x00411600[none] | no exe call site: every original caller has no exe body (gap or unswapped) |
| 0x004099a0 | left (C4 port body) | `re/match/s02/004099a0.c` | 100.0% 10/10, 53 B identical | OK 5/1 | `AutosaveTriggerCore`, `Save/GameSaveVFS.cpp` | 0 | 0x00409a80[none] 0x004117b0[none] 0x00430290:tail[none] | see T06 RESULT: port `AutosaveTriggerCore` writes `s_autosave`; a match-route swap needs the GameSave.cpp rebinding |
| 0x0040d470 | left | `re/match/s02/0040d470.c` | 100.0% 80/80, 285 B identical | OK 21/0 | none | 0 | 0x0040fc00[none] 0x004102f0[none] | no exe call site: every original caller has no exe body (gap or unswapped); state `0x007f0f34` has no exe TU reference |
| 0x0040dbd0 | left (port body) | `re/match/s02/0040dbd0.c` | 100.0% 27/27, 149 B identical | OK 13/0 | `SubStateEnter4`, `Race/SubStateMachine.cpp` | 1 | 0x004111c0[none] | exe arm `SubStateEnter4` skips callees and uses port host objects and knobs (MATCH-S02 'Not swapped') |
| 0x0040dd60 | left | `re/match/s02/0040dd60.c` | 100.0% 8/8, 24 B identical | OK 2/0 | none (asi `GuardConcludedAndP1Won`) | 0 | 0x00430290[none] | no exe call site: every original caller has no exe body (gap or unswapped) |
| 0x0040de00 | left | `re/match/s02/0040de00.c` | 100.0% 1/1, 5 B identical | OK 1/0 | none (asi `ThunkReplaySave`) | 0 | 0x004111c0[none] | no exe call site: every original caller has no exe body (gap or unswapped) |
| 0x0040e360 | left | `re/match/s02/0040e360.c` | 100.0% 3/3, 10 B identical | OK 1/0 | none (asi `RaceModeSet`) | 0 | 0x0040e560[none] 0x00429310[none] 0x0043d7c0[none] 0x0043dfd0[port body] | the reached caller port does not make this call (port state) |
| 0x0040e4b0 | left | `re/match/s02/0040e4b0.c` | 100.0% 36/36, 73 B identical | OK 2/0 | none | 0 | 0x00414220[none] 0x00448220[none] 0x0045d250[none] | reads `0x005f2770[0xd+i]` (zero table in the exe); see T07 RESULT |
| 0x0040e560 | left | `re/match/s02/0040e560.c` | 100.0% 11/11, 37 B identical | OK 5/0 | none | 0 | 0x004103a0[unreached] | the reached caller port does not make this call (port state) |
| 0x004102f0 | left | `re/match/s02/004102f0.c` | 100.0% 51/51, 172 B identical | OK 14/0 | none | 0 | 0x004111c0[none] | no exe call site: every original caller has no exe body (gap or unswapped) |
| 0x004103a0 | left (port body) | `re/match/s02/004103a0.c` | 100.0% 110/110, 360 B identical | OK 32/0 | `SubStateHeldCountdown5`, `Race/SubStateMachine.cpp` | 0 | 0x004111c0[none] | exe arm `SubStateHeldCountdown5` is a port core over host objects (MATCH-S02 'Not swapped'); its RVA-bound symbol has no exe caller |
| 0x00411170 | left | `re/match/s02/00411170.c` | 100.0% 19/19, 70 B identical | OK 9/0 | none (asi `TimeTrialRecordPlayback`) | 0 | 0x004111c0[none] | no exe call site: every original caller has no exe body (gap or unswapped) |
| 0x00411350 | left | `re/match/s02/00411350.c` | 100.0% 28/28, 87 B identical | OK 2/1 | `ReplayTimeFormat`, `Match/MatchS02_00411350.cpp` | 0 | 0x00411530[none] 0x00411600[none] 0x00411d90[none] 0x00429310[none] | no exe call site: every original caller has no exe body (gap or unswapped) |
| 0x00411530 | left | `re/match/s02/00411530.c` | 100.0% 20/20, 65 B identical | OK 1/0 | none (asi `void __cdecl`) | 0 | 0x00411580[none] 0x004115c0[none] | no exe call site: every original caller has no exe body (gap or unswapped) |
| 0x00411580 | left | `re/match/s02/00411580.c` | 100.0% 21/21, 58 B identical | OK 2/0 | none (asi `ReplayGetBestTime`) | 0 | 0x0041e1e0[none] 0x00429310[none] 0x004298e0[none] | no exe call site: every original caller has no exe body (gap or unswapped) |
| 0x00411600 | left | `re/match/s02/00411600.c` | 100.0% 99/99, 334 B identical | OK 22/4 | none (asi `ReplayRecordFrame`) | 0 | 0x00411170[none] | no exe call site: every original caller has no exe body (gap or unswapped); state `0x0063bb20` has no exe TU reference |
| 0x004117b0 | left | `re/match/s02/004117b0.c` | 100.0% 46/46, 182 B identical | OK 17/2 | none (asi `ReplaySave`) | 0 | 0x0040de00:tail[none] | no exe call site: every original caller has no exe body (gap or unswapped); state `0x005f29c8`, `0x00616038` has no exe TU reference |
| 0x00411d60 | left | `re/match/s02/00411d60.c` | 100.0% 11/11, 40 B identical | OK 3/0 | `ReplayCheckTimer`, `Match/MatchS02_00411d60.cpp` | 0 | 0x004103a0[unreached] | the reached caller port does not make this call (port state) |
| 0x00413c70 | left | `re/match/s03/h2_00413c70.c` | 100.0% 15/15, 59 B identical | OK 0/4 | none (asi `Idx2RecordCondSet413c70`) | 0 | 0x00412f30[none] 0x00418de0[none] 0x00419a00[none] 0x00467300[none] 0x00470c70[none] | no exe call site: every original caller has no exe body (gap or unswapped); state `0x0063bc60` has no exe TU reference |
| 0x00414220 | left | `re/match/s03/h2_00414220.c` | 100.0% 67/67, 213 B identical | OK 8/14 | none | 0 | 0x0040dd60[none] 0x00417750[none] | writes live `0x0089a364` / `0x0089a380` (AI difficulty); callers 0x0040dd60 (asi only) and 0x00417750 (no exe body); see T07 RESULT |
| 0x00418860 | left | `re/match/s03/h3_00418860.c` | 100.0% 95/95, 280 B identical | OK 19/3 | none (asi ``) | 0 | 0x004103a0[unreached] 0x00425a40[none] | the reached caller port does not make this call (port state) |
| 0x00418990 | left | `re/match/s03/h1_00418990.c` | 100.0% 1/1, 5 B identical | OK 1/0 | none | 0 | 0x0040fc00[none] 0x00411170:tail[none] | no exe call site: every original caller has no exe body (gap or unswapped) |
| 0x00418a30 | left | `re/match/s03/h1_00418a30.c` | 100.0% 14/14, 50 B identical | OK 0/2 | none (asi `Init418a30`) | 0 | 0x004194f0[none] 0x00419760[none] 0x0041a060[none] | no exe call site: every original caller has no exe body (gap or unswapped); state `0x005f3298`, `0x005f32b0`, `0x0063bd98`, `0x0063bda0` has no exe TU reference |
| 0x00418de0 | left | `re/match/s03/h1_00418a30.c` | 100.0% 28/28, 73 B identical | OK 5/0 | none (asi `VehicleEliminationSlotPostInit_Body`) | 0 | 0x00418e30[none] 0x00419760[none] 0x00419a00[none] | no exe call site: every original caller has no exe body (gap or unswapped); state `0x005f3298`, `0x005f32b0`, `0x0063bd98`, `0x0063bda0` has no exe TU reference |
| 0x00418e30 | left | `re/match/s03/h1_00418a30.c` | 100.0% 9/9, 26 B identical | OK 1/2 | none | 0 | 0x004194f0[none] | no exe call site: every original caller has no exe body (gap or unswapped); state `0x005f3298`, `0x005f32b0`, `0x0063bd98`, `0x0063bda0` has no exe TU reference |
| 0x00418e50 | left | `re/match/s03/raw_00418e50.c` | 100.0% 7/7, 20 B identical | OK 0/1 | none (asi `Fill64_63c508`) | 0 | 0x004194f0[none] 0x00419ff0:tail[none] | no exe call site: every original caller has no exe body (gap or unswapped); state `0x0063c508` has no exe TU reference |
| 0x004190f0 | left | `re/match/s03/h1_00418a30.c` | 100.0% 4/4, 9 B identical | OK 0/0 | none (asi `ClearEax4190f0`) | 0 | 0x004194f0[none] | no exe call site: every original caller has no exe body (gap or unswapped); state `0x005f3298`, `0x005f32b0`, `0x0063bd98`, `0x0063bda0` has no exe TU reference |
| 0x004194f0 | left | `re/match/s03/h1_00418a30.c` | 100.0% 13/13, 51 B identical | OK 4/4 | none | 0 | 0x00418990:tail[none] | no exe call site: every original caller has no exe body (gap or unswapped); state `0x005f3298`, `0x005f32b0`, `0x0063bd98`, `0x0063bda0` has no exe TU reference |
| 0x0041d910 | left (already C4) | `re/match/s04/0041d910.c` | 100.0% 3/3, 21 B identical | OK 2/0 | `Clear63d584Pair`, `Match/MatchS04_0041d910.cpp` | 1 (`SubStateEnter4`) | 0x0040dbd0[reached] | already the matched body, called, C4 |
| 0x0041da90 | left (already C4) | `re/match/s04/0041da90.c` | 100.0% 6/6, 17 B identical | OK 1/0 | `DeltaTimeOutGet`, `Match/MatchS04_0041da90.cpp` | 1 (`SubStateHeldCountdown5_C`) | 0x004103a0[unreached] 0x00462f50[none] | already the matched body, called, C4 |
| 0x0041e080 | left | `re/match/s04/0041e080.c` | 100.0% 14/14, 68 B identical | OK 12/0 | `ScoreboardStateZeroInit`, `Match/MatchS04_0041e080.cpp` | 0 | 0x0040dbd0[reached] | the reached caller port does not make this call (port state); state `0x0063d79c`, `0x0063d7a0`, `0x0063d7b0`, `0x0063d7b4` has no exe TU reference |
| 0x0041e130 | left | `re/match/s04/0041e130.c` | 100.0% 3/3, 10 B identical | OK 1/0 | `TimerTrackSetter`, `Match/MatchS04_0041e130.cpp` | 0 | 0x0040e560[none] 0x0040fc00[none] | no exe call site: every original caller has no exe body (gap or unswapped) |
| 0x00429840 | left | `` |  | OK  | none (asi `RaceStateLatchSet`) | 0 | 0x0040e560[none] 0x004103a0[unreached] 0x0042c960[none] | matched in MATCH-S06 (branch orch/MATCH-S06, in review), not on this base; callers 0x0040e560 / 0x004103a0 / 0x0042c960 have no exe call to it |
| 0x004298e0 | left | `` |  | OK  | none | 0 | 0x0040e560[none] | matched in MATCH-S06 (in review), not on this base; caller 0x0040e560 has no exe body |
| 0x00430290 | left | `re/match/pilot/a3_00430290.c` | 100.0% 300/300, 987 B identical | OK 12/62 | none (asi `ChampionshipComplete`) | 0 | 0x00411170:tail[none] | no exe call site: every original caller has no exe body (gap or unswapped); state `0x00899140` has no exe TU reference |
| 0x00483ca0 | left | `re/match/pilot/a4_00483ca0.c` | 100.0% 39/39, 102 B identical | OK 0/0 | none (asi `FindNodeStructCopy483ca0`) | 0 | 0x004117b0[none] | no exe call site: every original caller has no exe body (gap or unswapped) |

## What T02 needs

1. 0x004111c0 matched. 2. The port sub-state machine (`Race/SubStateMachine.cpp`) rebuilt on original state, not host objects (S02 "Not swapped" for 0x0040dbd0 / 0x004103a0). Deep gaps (map): 0x00448220 (15), 0x00410d10 (14), 0x00418560 (14, no S03 result), 0x0040e590 (10), 0x00411ae0 (9).

## Build and gates

Worktree `.worktrees/MATCH-TOPDOWN-T01`, `MASHED_NO_DEPLOY=1`, test-slot wrapped: `=== Build OK ===` both targets,
`[rva-lint] 25 finding(s) ... NEW=0`, `[asm-lint] OK`. Source commit `dd844a168`.

Runner `verify/savelink_20261023/run_arms.py`, A4_ROOT `D:/MashedWT/MATCH-TOPDOWN-T01_root` (robocopy of `original/`), A4_EXE =
the build copied out first (exe sha256/16 `fca9b9fcce892772`), wrapped in `test-slot` + `mashed_lock.py run`.

| run | arm | result | vs merge_train.json ref (G-CTRL 8284be46 / ba407a16 / 31fe043e, B 1d5bf4f9 x3) |
|---|---|---|---|
| g1 | B_DEF_K0 / K1 / K26 | self-exited, 762 lines each, `1d5bf4f9` x3 | **equal** |
| g1 | H_DEF | **killed at the 900 s cap**, not self-exited: 8000 step / 30233 gates / 15116 ptrace lines | full hash not comparable; each file is **byte-identical** to the same number of leading lines of the reference run (T13's `arms_g1`, full hash 8284be46): step `654c2186`, gates `28ea1248`, ptrace `e8ae5994` on both sides |
| g2 (`--solo`) | H_DEF | killed at the 900 s cap again: 5473 / 20129 / 10064 lines | byte-identical to the reference prefix again |

Why the cap: the machine was saturated during both runs (95% CPU on 12 cores: `MsMpEng.exe` 263%, the search indexer and
other sessions' Python jobs), so H_DEF ran about 7x slower than T13's 125 s run and the B arms 15x slower (171-215 s vs 12 s).
A solo lock window holds the game lock only, not the CPU. No step, gate or ptrace line differs from the reference where both
exist, and B is equal. **The full G-CTRL hash is not measured on this build.** The C4 lines below are therefore conditional:
apply them only when the merge train's own G-CTRL run on the merged tree reproduces 8284be46 / ba407a16 / 31fe043e (the
train runs it for every source row). If it does not, the swap moved a gate: file an L-n decision.

Raw data (`D:/MashedWT/MATCH-TOPDOWN-T01_data`, not committed):

```
fca9b9fcce89277242178af0d3f27a32a34a7b88efcb557b75d73b16b6b973ac  exe_g1/mashed_re.exe
76db33f09f5a3b16b87773cd9574c3a374a71cd73317db2bc7c20c45115d1d96  build1.log
8de86361d82c04d6b93e7add140857406a50e87dabd485b410b07eaaa5ba411f  build2.log
f354ec744a69f7460be490eca3f7c675be4f54de6417ad1b81cba226c0197c32  gates_g1.log
e996a967b02b68f9e0477a6bf28e3a00130b3e6578c0d5bf023319e9bcd4f42f  gates_g2.log
5a1fe409fda4a4d33ffdbb443e28cd9d725601f2a4bbfe80639240b55e6f0c29  arms_g1/B_DEF_K0.log
1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457  arms_g1/B_DEF_K0.step.csv
bf9cea9641532456cc5e6614fbbd387c5a47cbc3883f30555d1f470e23170baa  arms_g1/B_DEF_K1.log
1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457  arms_g1/B_DEF_K1.step.csv
fe46c5bf451896595d7ed57504bafb82d722775c7d7439ed8fb2e37b02129101  arms_g1/B_DEF_K26.log
1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457  arms_g1/B_DEF_K26.step.csv
28ea1248d3b7f1e4c707efe15cd063b4e642c8415befc68e313474b8a5912cfc  arms_g1/H_DEF.gates.csv
98ca388cab8f883f87c49d5686b33b541801ff3468ded39b38f8a892b1e549c3  arms_g1/H_DEF.log
e8ae59944b5e2aeb0a66af4c3c6dc6062cb2b273b885f27a9e9ab774c11efc09  arms_g1/H_DEF.ptrace.log
654c218615499de64993daa4f8d41fd0068ee1e278014f9ba305d276ea9c2381  arms_g1/H_DEF.step.csv
79ede613f9f446a1d102c5006f978ee8a603685d69eb7e1700428771a62c4d60  arms_g2/H_DEF.gates.csv
c02361edc3b343f764c1f5848561e30dbbde98216e9da87fd259a752987c8e54  arms_g2/H_DEF.log
1299892f20c889b459c7c04e967eb6b879dae4ab54a66991342fc1cac54539a5  arms_g2/H_DEF.ptrace.log
4c3044b33d9d7097901e9e05cc4c4e249f7950da1ec3668151c258a242436f3d  arms_g2/H_DEF.step.csv
```

## Re-classify (prepared, apply after merge through re-classify)

None.
