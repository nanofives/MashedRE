# RESULT D4-DC-0510: 0x00410510 RuleEngine vs ScoringHooks, consolidated

**Verdict: GREEN.** 0x00410510 now has one body, `Race/EvalResultCore.cpp`, compiled into both targets. On the default
race path the exe reproduces every current reference exactly: G-CTRL, B, (b), (e), D2 3/3 and the 13-track table.
The allowlist line `CROSS-TARGET:00410510` is removed and rva-lint reports 0 NEW (5 allowlisted, 6 before).

Branch `orch/D4-DC-0510`, code commit `62e1470a0`, base `orch/integration` @ `91e8b9733`. DECISIONS W0-11 (P4).

## What changed

| file | change |
|---|---|
| `Race/EvalResultCore.{h,cpp}` (new, in `exe_sources.rsp` and `asi_sources.rsp`) | FUN_00410510's control flow, once. `EvaluateResultCore(EvalResultInputs&, Rule7Trace*)` |
| `Race/RuleEngine.cpp` | `EvaluateResult` is now a reader: `SnapshotInputs` over Cars/Persist, `*p0Won = (winner == 1)` |
| `Race/ScoringHooks.cpp` | `EvalResult_00410510` is now a reader: `LiveInputs` forwards to the original callees and globals, and performs the side effects |
| `re/tools/dual_copy_allowlist.txt` | `CROSS-TARGET:00410510` line removed |
| `mashedmod/run_tests.bat` | ruleengine_test links `EvalResultCore.cpp` |

**Input plumbing** (`EvalResultInputs`, one virtual per original read). The core calls them in the original's call
order, so the .asi reader runs the former `EvalResult_00410510` callee sequence. The exe reader returns the snapshot.

| input | .asi (live) | exe (snapshot) |
|---|---|---|
| RaceType | `FUN_0042f6a0()` | `p.timeAttack ? 2 : 0` |
| Rule / Participants / Teams | `DAT_007f0fd0` / `DAT_008a94d0` / `FUN_0042f500()` | `rule` / `c.participants` / `p.teams` |
| Score / SlotActive / Alive | `FUN_0040b6d0` / slot probe `+0x34+i*4` / `FUN_0046c7b0` | `c.score` / `c.active` / `c.alive` |
| FinishSlot / CollectDone | `FUN_00417740` / `FUN_00405890` | `(int)p.finishOrder` / `CollectAllDone(p)` |
| Rule7Count | `FUN_0040e340()` | `c.participants` |
| Metric / Motion | `FUN_00417730` / `FUN_0046cbb0` out1 | `c.metric` / `c.motion0` |
| Timer / TimerZero | `DAT_007f0fe4` / `DAT_005d757c` | `p.timer` / `0.f` |
| thresholds | `_DAT_005cc31c` / `_DAT_005cc9c8` / `_DAT_005cc574` | `kFinishMetric` / `kRule9Gap` / `kRule10Metric` |
| Rule7Keep | `true` (the original) | `p.rule7KeepWinner` (r7keep, TrackRenderer ON by default) |

**End-block side effects behind target thunks:** `OnEnter` (`DAT_0063b90c = 0`), `OnPlayer0Flag` (the `DAT_007f0fcc`
writes inside cases 9/10, before the end block) and `OnConclude` (LAB_0041062a: `FUN_0045bed0`, `FUN_0045bf30`,
`FUN_00458f80(0)`, `DAT_0063ba8c = 0xb`, `DAT_0063b90c..0063b91c`, `FUN_00448700`, `DAT_007f0fcc`, course 0x26
`FUN_0044df80(0)`). The .asi performs them. In the exe they are empty, as before the merge, so the race layer keeps
them (TrackRenderer reads the return value and p0Won).

## One audit claim corrected

The 2026-09-29 demotion note on the hooks.csv row says case 7's bound `c.participants` and the .asi's
`FUN_0040e340()` are "two different globals". The bytes say otherwise. `0x0040e340: A1 D0 94 8A 00 mov eax,[0x008a94d0]`
/ `0x0040e345: C3 ret` (`re/tools/disasm_fn.py 0x0040e340 0x0040e350` on `MASHED.exe.unpatched`), and `DAT_008a94d0` is
`Cars::participants`. The rule-7 loop at `0x0041075a..0x00410795` re-reads it on every pass (`call 0x40e340` at
`0x0041075c` and `0x00410787`), and the core does the same. The other two items in that note are now inputs or thunks,
not separate bodies: the LAB_0041062a effects sit in `OnConclude`, and the `won0 ||` disjunct only ever held when
winner == 1, which is what `p0Won = (winner == 1)` now encodes.

## Gates (exe `mashed_re.exe` sha256 `1e7288ff...f5ef45`, private root `D:/MashedWT/D4-DC-0510_root`, solo lock)

| gate | result | reference |
|---|---|---|
| G-CTRL H_DEF | `ef5f2e10/57f7ff83/b4f4b55a` | same (merge_train.json `gctrl.ref`) |
| B K0/K1/K26 | `c37d4c0c/0a8f9806/7013671d` | same (`b.ref`) |
| (b) vsum | 3/3 arms 3/3 (A/B/C PASS) | 3/3 |
| (e) ai_speed_env | 3/3 PASS, launch 1425.7 / 2052.5 / 2055.0 | same `e_launch` |
| D2 D2_DEF_1..3 | 3/3 in, drvmed 1970.58, 1636 lines each, no rerun | 3/3 |
| 13 tracks (run13, root13) | table identical to `verify/rpct_20261022/tracks_flip_score.txt`, INSTRUMENT GATES: PASS | identical |
| unit | `run_tests.bat` ruleengine GREEN, navsm GREEN, racemodes GREEN | |
| lint | rva-lint 0 NEW, 5 allowlisted | |

`run_tests.bat` menustr and badges FAIL. They are the asset-dependent tests and do not link any file this step touches
(MenuStringTable / TxdDecoder / PizReader). They were not run against the base.

Committed extracts: `arm_hashes.txt`, `vsum_b.txt`, `e_check.txt`, `score_d2.txt`, `tracks_score.txt`.

## .asi installed-hook observation: blocked by a pre-existing crash, not by this change

`re/frida/verify_scoring_hooks.py` (the WS-H2 canonical observation for the scoring trio) was run from the private root
through `verify/dc0510_20261025/vsh_private.py`, which repoints the script's ORIG/EXE to the private root and adds a
Frida exception handler. `original\mashed_re_dev.asi` was not touched. Only the private root's copy was swapped.

| run | asi | outcome |
|---|---|---|
| hooked x3 | this branch (`9b152466...`) | process dies between t+10 s and t+20 s of the 4p FFA Quick Battle, before the first scoring call |
| hooked x2 | base 91e8b9733 (`ebd06689...`, built in a throwaway worktree) | dies the same way (t+20 to t+30 s) |
| baseline (hooks off) | this branch | survives: 24 scoring calls, scores `[0,8,8,8]`, winner 2, `DAT_0063ba8c = 0xb` |

The access violation is the same in both builds (`av_new.jsonl`, `av_base.jsonl`): a write to `0x6e8b08dc` with
eax `0x420de0`, ecx `0x42480000`, inside `_Accum420de0` +0xf (`Util/PromoLoop_sessionB.cpp:2513`, installed at
0x00420de0). The fuzzy backtrace is `_Accum4215c0` <- `_FrontendRaceResultsDispatch` <- `MASHED.exe+0x1102a` <-
`GhostPlaybackTick_Entry` <- `TimeTrialRecordPlayback` <- `MASHED.exe+0x112d2`. The offset moves from `+0x4412f` (base)
to `+0x4436f` (branch) only because the .asi layout shifted. 0x00410510 is not on the faulting path. The baseline run
has its first EvalResult call at t+22 s, after both hooked runs had already died.

Consequence: no new behavioural evidence for the .asi body, so 0x00410510 stays **C2** (row file and exe_file now point at
`Race/EvalResultCore.cpp`). The E9 byte check in that script also reads E9 with hooks off, because its own
`Interceptor.attach` on the three RVAs writes the jump, so it does not prove the .asi installed.

Follow-ups (not done here): (1) the 0x00420de0 hook crash in a hooked 4p FFA race, a new finding with no tracker row
(only U-2868 / U-8077 mention FUN_00420de0, as a callee identity). (2) After that fix, re-run the hooked observation
and its baseline diff for the scoring trio, the route back to C3 for 0x00410510. (3) The installer check in
verify_scoring_hooks.py should read the bytes before it attaches Interceptor.

## Raw data (not committed), SHA-256

| file | sha256 |
|---|---|
| D:/MashedWT/D4-DC-0510_data/arms/H_DEF.step.csv | EF5F2E105F649CDBF1A900340EB128AC8317FCE986D36538248E3B686D1178AE |
| D:/MashedWT/D4-DC-0510_data/arms/H_DEF.gates.csv | 57F7FF835CD9B7D774B939F08E091CF4B5FAECA58CDCBD7FAD0B41563FE134C6 |
| D:/MashedWT/D4-DC-0510_data/arms/H_DEF.ptrace.log | B4F4B55A20E57E3A37B1643703BFA8E06D6973561E943BDAF369112AAEFEB28F |
| D:/MashedWT/D4-DC-0510_data/arms/B_DEF_K0.step.csv | C37D4C0CFBDD5843F946FEEC67C0D3559AC17613D2520CF86AC53F6F79935F84 |
| D:/MashedWT/D4-DC-0510_data/arms/B_DEF_K1.step.csv | 0A8F9806BE171E483C1626FC0F985716A6412DD803E33587F408024E3870F1EC |
| D:/MashedWT/D4-DC-0510_data/arms/B_DEF_K26.step.csv | 7013671DFEA95C72B49F484DDAE75CEFEED87CB0F95D34495A3BA59AEC6E6DC0 |
| D:/MashedWT/D4-DC-0510_data/arms/D2_DEF_{1,2,3}.motion_diag.log | B7B66701EBD6A5A924291FE388AC459EC90980DCEF1C23595C3F214EA86644D6 (all three) |
| D:/MashedWT/D4-DC-0510_data/tracks_score.txt | 2A44A74801EA6911382FA7CF6F5BE11F0BB6075FD88052D139D27D80D76ED0F6 |
| D:/MashedWT/D4-DC-0510_data/crash_new.jsonl | 2FF2018AE8FE3ED52AFE9B8AFE190CC372EE87DB9B81FC498B416486DCAAB24F |
| D:/MashedWT/D4-DC-0510_data/crash_base.jsonl | E13AA98C18E0A36E136364D6502C157035BD659BF2A6FAAE0946901A3BFDDDB0 |
| D:/MashedWT/D4-DC-0510_data/vsh_baseline.log | 88D402C44A3A02746578FC87C59DCDB324BA04630582AA50B284684FD0F84DB4 |
| D:/MashedWT/D4-DC-0510_data/vsh_hooked3.log | EF09DE78D3DC34B5FA7696D2EBEB7CFFDEB42DC630C50120A90CD175251F8345 |
| D:/MashedWT/D4-DC-0510_data/vsh_basehooked2.log | 985B8ACCA5405C2DD19C534D376CA29EF2876276EB66D17F85CDAD9B64B70CAE |
| mashedmod/build/mashed_re.exe (this branch) | 1E7288FFDCC4942A048B78D2E0D9B69700AE6F5DE0075BC9F42BEB4F52F5EF45 |
| mashedmod/build/mashed_re_dev.asi (this branch) | 9B15246675EA89DCAB2F4D6BA427465CA1E6EDD32A52553442468E656BACE7CC |
| base 91e8b9733 mashed_re_dev.asi | EBD06689B6280D876C2A6E5F130C1247D648A7C01C9F90225444651C2D0F9304 |
