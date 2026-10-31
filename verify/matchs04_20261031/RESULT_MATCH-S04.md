# RESULT MATCH-S04 (run 27): C4 by byte match, slice 0x0041b540..0x0041f590

Branch `orch/MATCH-S04` off `orch/integration` @ `0116b9fec` (MATCH-MERGE + MATCH-SCOPE merged). Worktree
`.worktrees/MATCH-S04`. **No tracker was edited** (MATCH-RUBRIC is not merged on orch/integration yet): this file
carries what re-classify needs. Raw data: `D:/MashedWT/MATCH-S04_data` (SHA-256 list at the end).

## Verdict

**GREEN.** 118 of 128 functions matched byte for byte; 53 matched sources are now the port body of their RVA
(22 in both targets, 18 of them replacing an existing exe body); the default gates are unchanged on that exe.

- **Matched: 118 of 128** slice functions are byte-identical under `re/tools/match/match.py` with the XP DDK
  compiler and `/O2 /GS /Ob1` (C, x87), and every one passes `re/tools/match/relocheck.py` OK (`relocheck.tsv`).
  Raw converter alone: 37 of 128. 2 of the 118 count by the whole-symbol byte rule (Ghidra's size is short):
  0x0041b7a0 (1,188 B symbol), 0x0041def0 (389 B symbol). 10 are below 100% (table below).
- **Swapped: 53 RVAs** now get their port body from a matched source (one body per RVA, `lint_rva_bodies.py`:
  `16 known, NEW=0`, unchanged). 22 are bodies in both targets, 31 in the .asi only. 18 of the 22 replaced an
  existing exe body: these are the C4-by-match candidates. Plus `// HIDDEN: 0x0041dab0` (no Ghidra entry, no
  hooks.csv row), compiled with 0x0041db80 in the .asi.
- **Gates:** on the exe with all 53 swaps (commit `af19355a4`, exe sha256/16 `795de6dc9e7a5727`): G-CTRL
  `8284be46/ba407a16/31fe043e`, B `1d5bf4f9/1d5bf4f9/1d5bf4f9`, equal to `merge_train.json` gctrl.ref / b.ref.
  No gate hash moved, so no DECISION is needed for any swap.

## Tools and conventions (same as MATCH-S02, plus three changes)

- `re/tools/match/relocheck.py` and the third pass of `scripts/rva_body_scan.py` are **byte-identical copies** of
  orch/MATCH-S02's (`git diff orch/MATCH-S02 -- <both>` is empty), so the merge with S02 is clean either way.
- Port bodies: `mashedmod/src/mashed_re/Match/MatchS04_<va8>.cpp`, the S02 wrapper pattern (`#define FUN_<va8>
  <PortSymbol>`, `extern "C" { #include "../../../../re/match/s04/<va8>.c" }`, the old `RH_ScopedInstall`). The
  matched source is compiled as C by the DDK (the match) and as C++ by VS2022 (the port).
- **rsp block mid-file.** S01 and S02 both append their wrappers at the end of `exe_sources.rsp` /
  `asi_sources.rsp`, so they collide with each other there. S04's entries (16 in exe, 44 in asi) sit in a sorted block right after the
  `"Util\TimerInit.cpp"` line (`tools/rspadd.py`), and its x87 block sits after `RwpSolverCore17` in
  `x87_tus.txt` (`tools/x87add.py`). `git merge-tree` of orch/MATCH-S04 against orch/MATCH-S01, -S02 and -S03:
  all three clean.
- **Two-function files.** Most functions in this slice are pairs: a caller and the static callee that gets its
  record in a register (EAX/ECX/ESI/EDI/EBX). The helpers matched both in one file, so one wrapper makes both
  RVAs port bodies (rva_body_scan's third pass counts every `// MATCH:` of an included file). Where two helpers
  solved the same pair, one file was kept (the 14 duplicates are in `D:/MashedWT/MATCH-S04_data/dup_sources/`).
- **Register-ABI entry shims (needs the rubric owner's eye).** VS2022 cannot give a C function a register
  argument, so where the old port symbol was entered with the original's register convention (the asi hook, or an
  inline-asm caller), the wrapper keeps that symbol as a small `__declspec(naked)` adapter with **no RVA comment**
  (push the register, call the matched C body, return) and the matched function is the cdecl body under a
  `_impl`/`Body` name: 0x0041bc50, 0x0041c2d0, 0x0041c320, 0x0041cd20 (both targets), 0x0041b720, 0x0041cdb0
  (asi). The bytes the scanner counts as the body are the matched C; the shim computes nothing. If the rubric
  reads this as "naked inline asm", these six lose their C4 claim, the rest do not depend on it.
- Tail-jump dispatchers 0x0041e8b0..0x0041e970 (`mov r, [0x0063d7e4]; jmp [r+k]`): under this compiler the scratch
  register is set by the stack-arg count (0/3 eax, 1/4 ecx, 2/5 edx, checked for 0..4 args). The sources take the
  smallest count that gives the original's register (1 or 2 ints). A tail jmp passes the caller's stack through,
  so the arg count does not change behaviour.

## Swapped rows

Full table: `swaps.tsv` (rva, wrapper, targets, x87, port symbol, install, source, source git blob sha, match,
relocheck, old body). Common to every row:
- tool: `py -3.12 re/tools/match/match.py re/match/s04/<file>.c` -> 100.0% for that `// MATCH:` line (or the
  whole-symbol bytes line), and `py -3.12 re/tools/match/relocheck.py re/match/s04/<file>.c` -> OK
- compiler: XP DDK `cl` 13.00.9176 / `c2` 13.00.9178 (`tools/vc7ddk`, toolchain `ddk`), flags `/O2 /GS /Ob1` (file
  `// FLAGS:` line, match.py adds `/Gy`), language C
- port body: the `MatchS04_<file va8>.cpp` wrapper named in swaps.tsv, in the rsp list(s) named there

**C4-by-match candidates (18: matched source replaced an existing exe body; both targets):**

| rva | port symbol | old body | exe caller (from the swap helpers' greps) |
|---|---|---|---|
| 0x0041bc50 | FUN_0041bc50 (shim) -> HudRec29DispatchBody | HUD/HudDispatch.cpp | none by symbol |
| 0x0041c0c0 | HudDispatchSlot2 | HUD/HudDispatch.cpp | none by symbol |
| 0x0041c2d0 | FUN_0041c2d0 (shim) -> HudDispatchDraw4Body | HUD/HudDispatch.cpp | HudDispatchMode10 (inline asm, EAX) |
| 0x0041c300 | HudDispatchMode5 | HUD/HudDispatch.cpp | none by symbol |
| 0x0041ccc0 | HudSlotLoopCcc0 | HUD/HudDispatch.cpp | none by symbol |
| 0x0041c320 | ParticleEmitterCtorC (shim) -> _impl | Render/ParticleEmitterCtors.cpp | none by symbol |
| 0x0041cd20 | ParticleEmitterCtorD (shim) -> _impl | Render/ParticleEmitterCtors.cpp | none by symbol |
| 0x0041c010 | HudConstTableInitAndSweep | HUD/ScenarioLeaves_sa2.cpp | none by symbol |
| 0x0041cbc0 | FloatTableInit (x87) | Util/TimerInit.cpp | none by symbol |
| 0x0041d820 | TimerSlotClear | Util/TimerInit.cpp | exe_main.cpp:9276 |
| 0x0041e130 | TimerTrackSetter | Util/TimerInit.cpp | none |
| 0x0041eda0 | TimerBitFieldSet | Util/TimerInit.cpp | none by symbol |
| 0x0041f000 | TimerSlotDataCopy | Util/TimerInit.cpp | none by symbol |
| 0x0041d910 | Clear63d584Pair (x87) | Race/SubStateCallees.cpp | SubStateMachine.cpp:125 |
| 0x0041da90 | DeltaTimeOutGet | Race/SubStateCallees.cpp | SubStateMachine.cpp:245 |
| 0x0041e080 | ScoreboardStateZeroInit | Frontend/SmallLeaves_t1.cpp | none |
| 0x0041f090 | GetPlayerStateBits | Util/EventTable.cpp | none by symbol |
| 0x0041f1c0 | GetEventFlag | Util/EventTable.cpp | none |

New exe bodies with no earlier exe body (callees held in a candidate's file, not C4 candidates on their own until
the rubric says how a held static callee counts): 0x0041b770 (old body asi-only), 0x0041c9a0, 0x0041cb10,
0x0041d6e0 (no old body anywhere).

asi only (31, not C4 candidates: the exe has no body): 0x0041b720, 0x0041bf20, 0x0041cdb0, 0x0041d730,
0x0041db80 (+HIDDEN 0x0041dab0), 0x0041e140, 0x0041e870, 0x0041e8b0, 0x0041e8c0, 0x0041e8f0, 0x0041e970,
0x0041e980, 0x0041e9b0, 0x0041e9d0, 0x0041e9e0, 0x0041ea80, 0x0041ea90, 0x0041ede0, 0x0041ee50, 0x0041eeb0,
0x0041ef60, 0x0041ef80, 0x0041efc0, 0x0041efe0, 0x0041f030, 0x0041f0d0, 0x0041f100, 0x0041f260, 0x0041f300,
0x0041f320.

Witness notes for re-classify:
- Calls that changed target. The old exe bodies of 0x0041c010 and 0x0041ccc0 called the ORIGINAL 0x0041b770 /
  0x0041c9a0 through raw addresses; they now call the matched port bodies, which compute the same thing. Any other
  callee with no port implementation stays a raw-address forwarder (`MatchS04_<va8>_Call<callee>`, no RVA comment),
  as in S02 (0x0041c320, 0x0041cb10, 0x0041cd20, 0x0041d6e0).
- 0x0041bc50, 0x0041c0c0, 0x0041c2d0, 0x0041c300 return void (as the original). The old bodies returned the
  last EAX. The swap helper found no reader.
- 0x0041e9b0, 0x0041efe0: return type is `MATCH_RET_<va8>`: `bool`/`byte` under the DDK (the match), `unsigned
  int` under C++, because VS2022 sets only AL for a bool/byte return and the original (and the old body) set EAX.
- **0x0041d730 (PlayerSlotConfigInit) old body diverged from the original** (byte-offset +4 stores where the
  original indexes, and a loop on the return of a no-arg call to 0x0041cdb0). It is dead in both targets (asi
  install MASS-DISABLED, no port caller), so the swap changes nothing that runs, but the export now computes what
  the original does. Revert it if a dead-code divergence must go through a decision first.

## Matched but not swapped (each has a port body)

| rva | reason |
|---|---|
| 0x0041f360 | NOT SAFE as is: the exe body reads the port const `g_dat_00636ad0` (= 0, CarMaterialTables.cpp:58), the matched source reads 0x00636ad0. The original's .data there is 0 and no port TU writes it, so the values agree, but it is a port-global read. Left for the lead |
| 0x0041b630 | its file also holds 0x0041b340 (static callee, EAX), which is MATCH-S03's slice: S03 has its own matched 0x0041b340. Swapping both slices' files would give 0x0041b340 two bodies. Swap after S03 lands, binding to S03's body |
| 0x0041cc50 | its file holds 0x0041c410 (99.4%, not matched): including it would make an unmatched function a port body |
| 0x0041e150, 0x0041e170, 0x0041e4b0 | asi-only bodies; they are static callees held in 0x0041e6c0.c, whose caller is 98.8% |

## Not matched (10)

| rva | size | best % | class | residual (helper notes) |
|---|---|---|---|---|
| 0x0041c410 | 1294 | 99.4 | a | fld scheduled one slot later in the `scale*1.25f` + 0x004c13e0 block |
| 0x0041e6c0 | 385 | 98.8 | b | tail-merged fstp of scale.z scheduled 3 instructions early (372 permuter compiles) |
| 0x0041ce00 | 1388 | 98.7 | a | one x87 load one slot later vs push edx (318 permuter variants) |
| 0x0041d930 | 336 | 97.0 | a | one fld issued after a push instead of before |
| 0x0041c180 | 311 | 96.7 | a | same scale-block fld scheduling as 0x0041c410 |
| 0x0041f120 | 150 | 89.1 | b | add ecx,0x40 before fsub [ecx] in the original, ours folds [ecx+0x40] |
| 0x0041dca0 | 464 | 83.2 | b | original self-copies scale, clamp's x=1 arm after the ret |
| 0x0041f590 | 384 | 78.4 | b | endpoint copies fld/fstp vs mov, clamp value spilled vs kept in st0 |
| 0x0041de80 | 68 | 65.5 | b | record in EAX copied to ESI under push/pop (original: ESI unsaved + tail jump) |
| 0x0041ded0 | 30 | 27.3 | b | follows from 0x0041de80's convention (caller tail-jumps) |

All four "a" rows are one x87 load scheduled one slot apart around a call; it survived rewrites and the permuter.
Per-function notes: `notes/h1..h9.md`. Full per-RVA table: `results.tsv`.

## Commands (reproduce)

```
cd C:\Users\maria\Desktop\Proyectos\Mashed && py -3.12 re/tools/decomp_pc.py --file <slice_rvas.txt> --port --callees --json -o decomp.json
   (run from the MAIN checkout: decomp_pc.py resolves mashed_pool/ relative to itself; one pool slot, released by the tool)
export MSYS_NO_PATHCONV=1   # Git Bash, else /O2 becomes C:/Program Files/Git/O2
py -3.12 re/tools/match/g2s.py --decomp decomp.json --out g2s --flags "/O2 /GS /Ob1"
py -3.12 verify/matchs04_20261031/tools/to_s04.py <va8> ...      # raw 100% outputs -> s04 form (LF; g2s writes CR CR LF)
py -3.12 re/tools/match/match.py re/match/s04/<va8>.c
py -3.12 re/tools/match/relocheck.py re/match/s04/*.c
py -3.12 re/tools/match/match_permute.py re/match/s04/<va8>.c 0x<rva> --rounds 6 --walk 40 --jobs 2 --time 300
py -3.12 verify/matchs04_20261031/tools/excise.py <old TU> <va8>; mkwrap.py <va8> <Symbol> <install> [--bind ...] --old <TU>
py -3.12 verify/matchs04_20261031/tools/rspadd.py exe|asi|both <va8> ...; x87add.py <va8> ...
py -3.12 scripts/lint_rva_bodies.py
cmd /c "set MASHED_NO_DEPLOY=1&& mashedmod\build.bat"
A4_EXE=<exe copy> A4_ROOT=D:\MashedWT\MATCH-S04_root A4_OUT=<arms dir> mashed_lock.py run --label MATCH-S04 -- py -3.12 verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26
py -3.12 verify/matchs04_20261031/tools/collect.py > results.tsv; tools/swaps.py <pre-swap bodies.tsv> > swaps.tsv
```
Helper briefs: `BRIEF.md` (matching, 9 helpers), `SWAP_BRIEF.md` (body swap, 3 helpers).

## Gates

Runner `verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`, A4_ROOT `D:/MashedWT/MATCH-S04_root`
(robocopy of `original/`), wrapped in `mashed_lock.py run --label MATCH-S04`, exe copied out of the build dir first.
Refs (`re/orchestrator/merge_train.json`): G-CTRL `8284be46/ba407a16/31fe043e`, B `1d5bf4f9` x3.

| run | build | exe sha256/16 | G-CTRL step / gates / ptrace | B K0 / K1 / K26 | vs ref |
|---|---|---|---|---|---|
| 1 | all 53 swaps, commit af19355a4 | `795de6dc9e7a5727` | 8284be46 / ba407a16 / 31fe043e | 1d5bf4f9 x3 | equal |

H_DEF exit 0 self-exited after 520 s, each B arm exit 0 after 43-44 s.

## needs Mariano

- `mashedmod/build_objs.ps1` rejects `.c` rsp entries (same as S02): accepting them would compile each matched
  source as C in the port too, and the wrappers would shrink to an rsp line. Not done (no `.ps1` edits from a worker).

## Raw data (D:/MashedWT/MATCH-S04_data, not committed)

```
9cd0ca52067f90a6a5f11eb4468d5e255f95df0216477bcbb0433da8d3f19448  decomp.json (decomp_pc.py --port --callees, 128 functions)
e6a3d0a397406efc70e747fcf1c6ef5b20e27c84c56c25353aff082069b7340d  bodies.tsv (pre-swap body scan, tools/bodies.py)
795de6dc9e7a5727e73e1d9c0f21fea4afe3d3869701843854f8bd4a0987caf7  exe_g1/mashed_re.exe
80b4e002590de3141731f02db23e338d62b3d49a04e2c3b357e889913a2926b0  exe_g1/mashed_re_dev.asi
8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198  arms_g1/H_DEF.step.csv
ba407a169a8ec433f486492814e7e450e32c6c48be5f1a0e38e9c53074db1ceb  arms_g1/H_DEF.gates.csv
31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3  arms_g1/H_DEF.ptrace.log
1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457  arms_g1/B_DEF_K{0,1,26}.step.csv
8dd030119830441e7063f04b223a334786072bd793e662f0125aaa59e2592a7b  arms_g1/H_DEF.log
a8b84de4d871f5aa663cf7249267931414c79ddd66cab9e0097e16d97b0be88f  build1.log
98eda7c990c3c0f3e9eb0616fb259649a79c95b37c9249a12e56680c592e1253  gates_g1.log
```
Also there: `g2s/` (raw converter output for all 128), `dup_sources/` (14 duplicate pair files), `notes/`, `tools/`.

## HANDOFF / next

- Re-classify the 18 candidate rows to C4 (`match` route) once MATCH-RUBRIC is merged and this branch has passed
  the train. The rubric owner decides the register-ABI shims (6 rows) and the held static callees (4 new exe bodies).
- 0x0041b630: swap after MATCH-S03 lands (bind its 0x0041b340 callee to S03's body). 0x0041f360: decide the
  `g_dat_00636ad0` port-global read (swap binds it to 0x00636ad0).
- 10 residuals: 4 are one x87 slot from 100% (class a). 0x0041c410 and 0x0041d930 have port bodies, so a match
  would swap them; 0x0041e6c0 would unlock 0x0041e150/0x0041e170/0x0041e4b0, 0x0041c410 would unlock 0x0041cc50.
