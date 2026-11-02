# RESULT MATCH-TOPDOWN-T08 (2026-11-01)

Verdict: **GREEN.** 1 of 9 swapped: the exe now calls the matched 0x00417730 (`VehicleRaceAngleGet`) from `AiPreTickRubberBand`. 8 left, each with a reason.

T01, T02, T09, T12 and T08 share the branch `orch/MATCH-TOPDOWN-T01` (one build, one gate run). T01 / T02 / T09 / T12 hang off Race::Tick 0x0040fc00 / race frame 0x004111c0; T08's one swap edits `AiPreTickRubberBand`, a race-frame function, so it rides the same build.

L-72 (Mariano 2026-11-01). T08 = map unit 8: `PerModeRenderMachine` 0x00404320 under FrameDispatch 0x00492e90, no exe mention, 9 matched, 4 x87, 0 gaps. The map listed 0x00417730 under T08 through 0x004041c0; its reached original caller 0x004177b0 (`AiPreTickRubberBand`) is what made the swap possible.

## Method

`re/tools/match/match.py <src> --toolchain ddk --bytes` (cl 13.00.9176 / c2 13.00.9178, `/O2 /GS /Ob1`) and
`re/tools/match/relocheck.py`, re-run 2026-11-01 on every source (`D:/MashedWT/MATCH-TOPDOWN-T01_data/t08.tsv`). `exe body`
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
| 0x00403c60 | left | `re/match/s01/00403c60.c` | 100.0% 53/53, 207 B identical | OK 4/0 | none | 0 | 0x00404320[none] | no exe call site: every original caller has no exe body (gap or unswapped) |
| 0x00403d30 | left | `re/match/s01/00403d30.c` | 100.0% 31/31, 124 B identical | OK 2/0 | none (asi `Render_00403d30`) | 0 | 0x00404320[none] | no exe call site: every original caller has no exe body (gap or unswapped) |
| 0x00403db0 | left | `re/match/s01/00403db0.c` | 100.0% 69/69, 278 B identical | OK 6/0 | none | 0 | 0x00404320[none] | no exe call site: every original caller has no exe body (gap or unswapped) |
| 0x00403ed0 | left | `re/match/s01/00403ed0.c` | 100.0% 53/53, 206 B identical | OK 4/0 | none (asi `Render_00403ed0`) | 0 | 0x00404320[none] | no exe call site: every original caller has no exe body (gap or unswapped) |
| 0x00403fa0 | left | `re/match/s01/00403fa0.c` | 100.0% 150/150, 529 B identical | OK 23/4 | none | 0 | 0x00404320[none] | no exe call site: every original caller has no exe body (gap or unswapped); state `0x005ea0b0`, `0x005ea0b4`, `0x00636b88` has no exe TU reference |
| 0x004041c0 | left | `re/match/s01/004041c0.c` | 100.0% 83/83, 343 B identical | OK 16/9 | none | 0 | 0x00404320[none] | no exe call site: every original caller has no exe body (gap or unswapped); state `0x00636b80` has no exe TU reference |
| 0x00404320 | left | `re/match/s01/00403fa0.c` | 808 B whole symbol identical | OK 39/19 | none (asi `PerModeRenderMachine`) | 0 | 0x00492e90[none] | PerModeRenderMachine: its caller FrameDispatch 0x00492e90 has no exe counterpart that calls it (the exe renders through RVA-less librw code); body asi-only (`Render/PerModeRender.cpp`) |
| 0x0040e350 | left (swapped on orch/MATCH-TOPDOWN-T13) | `re/match/s02/0040e350.c` | 100.0% 2/2, 6 B identical | OK 1/0 | none (asi `GetRenderSubMode`) | 0 | 0x00404320[none] 0x00406ae0[none] 0x0040b290[reached] 0x0040fc00[none] 0x00410b30[none] 0x00414490[reached] 0x00416250[none] 0x00416a30[none] 0x004177b0[reached] 0x00417da0[none] 0x00418560[none] 0x004189a0[none] 0x004189c0[none] 0x0041e1e0[none] 0x0041e6c0[none] 0x00424eb0[none] 0x004252c0[none] 0x00429310[none] 0x00429e10[none] 0x00442c80[reached] 0x00446520[none] 0x00448220[none] 0x0044b000[unreached] 0x0044c370[none] 0x0044ecc0[none] 0x00450300[none] 0x00455b50[none] 0x00458920[reached] 0x0045bba0[none] 0x0045c7f0[none] 0x0045cc50[none] 0x0045cc80[none] 0x0045d250[none] 0x0045d330[none] 0x0045d3a0[none] 0x00462f50[none] 0x00463640[none] 0x00463c80[none] 0x00463f40[none] 0x004642f0[none] 0x00467350[none] 0x00467650[reached] 0x00468d80[reached] 0x00469aa0[reached] 0x00469df0[reached] 0x0046c7d0[none] 0x0046cc40[reached] 0x0046dc20[none] 0x0046e9e0[none] 0x00470670[reached] 0x004709a0[none] 0x00470c70[none] 0x0047d640[port body] 0x0047def0[none] 0x0047eb30[unreached] 0x004922e0[unreached] 0x004929d0[none] | T13 swaps it with 2 exe callers (`TG_SubMode`); not on this base |
| 0x00417730 | **swapped (caller)** | `re/match/s03/h1_00417730.c` | 100.0% 3/3, 12 B identical | OK 0/1 | `VehicleRaceAngleGet`, `Vehicle/VehicleState.cpp` | 2 (`AiPreTickRubberBand`, this branch's build) | 0x004039f0[none] 0x004041c0[none] 0x00410510[reached] 0x00410d10[reached] 0x004177b0[reached] 0x00417cf0[reached] 0x0041dc30[none] 0x00429e10[none] 0x00446520[none] | `AiPreTickRubberBand` (0x004177b0 port, reached) read `F32(0x0089a880 + v*4)` inline where the original calls 0x00417730; `0x0089a880` is live (written at `Ai/AiStandalone.cpp:1695`). Both reads now call the matched `VehicleRaceAngleGet` (`Vehicle/VehicleState.cpp`, both targets) |

## What T08 needs

An exe call site for PerModeRenderMachine: the exe renders through RVA-less librw code and has no FrameDispatch case that calls 0x00404320. Until one exists, 0x00403c60 .. 0x004041c0 stay matched with no exe body.

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

| rva | name | level | notes text |
|---|---|---|---|
| 0x00417730 | VehicleRaceAngleGet | C3 -> C4 when the gates below are unchanged | `match evidence: identical (re/match/s03/h1_00417730.c, match.py 100% 3/3, 12 B), relocheck OK checked=0 unchecked=1 (_DAT_0089a880), swapped (Vehicle/VehicleState.cpp, both targets), exe-callers=2 (AiPreTickRubberBand), gates unchanged (MATCH-TOPDOWN-T08)`; replaces the `exe-callers=0` note |
