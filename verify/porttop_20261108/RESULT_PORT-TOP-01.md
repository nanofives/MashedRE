# RESULT PORT-TOP-01 (closes MATCH-S15-FU-MAINLOOP)

Lane: D4 port axis, L-96 top-down (verify/portplan_20261108/PLAN.md section 7). Branch `orch/PORT-TOP-01` off
`orch/integration` @ `ea666edab`.

**Verdict: GREEN.** In `mashed_re.exe` the matched MainLoopInit `0x00492770` now returns: the boot log prints
`[7] MainLoopInit returned 1` (was `[7] MainLoopInit AV'd`). Each of its 5 callees has an exe binding by name, no
absolute-address call is left in the exe build of `Match/MatchS15_00492770.cpp`. Gates: unchanged (G-CTRL `8284be46/67931834/31fe043e`, B `1d5bf4f9` x3), so no flip candidate.

## Per-callee binding (one commit each)

| callee | commit | exe binding | kind | why |
|---|---|---|---|---|
| `0x0042b920` | `cf7c43645` | `ConstantGetter22` (`Match/MatchS07_0042b920.cpp`, already in the exe) | matched | only the `#define` changed |
| `0x0042b950` | `8b5083516` | `Set7f1a0c_1000` (`Match/MatchS07_0042b950.cpp`, was .asi-only, added to `exe_sources.rsp`) | matched | |
| `0x00433240` | `775cbb5fc` | `RaceArmReset` (`Match/MatchS08_00433240.cpp`, was .asi-only, added) | matched | its callees `CupFloatInit` 0x00402f80 and `RaceTransitionBufZero` 0x0042d3a0 were already exe bodies bound by name |
| `0x00495110` | `4d4a0de2d` | `Store7f1030_Qpc3Mhz` (new wrapper `Match/MatchS16_00495110.cpp`, both targets; pulls `MatchS16_004950b0.cpp` + `MatchS16_00495120.cpp` into the exe) | matched, with a **stand-in** for its callee 0x004950b0 in the exe | see below |
| `0x004c57a0` | `68c975043` | `MashedRw_MatrixCreate_004c57a0` (`LibRw/RwBridge.cpp`, `rw::Matrix::create`) | port (librw, L-98) | see below |

Full table: `BINDINGS.tsv`. Match / relocheck of every source the exe now compiles for this chain: `swaps.tsv`,
`relocheck.tsv` (8 sources, all 100% insn, relocheck OK).

**0x00495110 stand-in (debt).** The matched `0x004950b0` computes `QPC * 3000000 / [0x00771e70]` in signed
`__int64` with no zero guard (`re/match/s16/004950b0.c`). The original fills `0x00771e70/74` at `0x00495120`
(TimerQpfStore), called from `0x00493540` earlier in its boot than `0x00492770`. The exe runs neither (`0x00493540` port
`body`) and maps the original data range zero-filled, so the divisor is 0 and the matched body raises #DE. In the exe,
`0x00495110`'s `FUN_004950b0` is `MatchS16_00495110_Call004950b0`: it runs the matched `TimerQpfStore` once while
`0x00771e70/74` are both 0, then the matched `QpcTimeScaledTo3Mhz`. Both bodies are matched sources; only the call order
is the exe's. The .asi binds `QpcTimeScaledTo3Mhz` directly (MASHED.exe already stored the frequency). Retire the
stand-in when the exe runs the original boot order (PORT-TOP-02 / PORT-TOP-04).

**0x004c57a0 librw (port binding, not the matched body).** The matched body (`re/match/rwl01/004c57a0.c`) calls
`[[0x007d3ff8] + 0x118]([0x007d3ff8] + [0x007d4028], 0x3000d)` (RW globals freelist alloc) and writes an identity
matrix with flags `0x20003`. `0x007d3ff8` is zero in the exe, so the body would fault. Under L-98 the RW core stays
librw: `rw::Matrix::create` allocates the same 64-byte matrix and sets identity with `TYPEORTHONORMAL (3) | IDENTITY
(0x20000)` = `0x20003`. librw's engine is started by `RaceSubmit_Init` before `ExecuteFrontendBootChain`
(`exe_main.cpp` WinMain). The pointer goes to `0x007e9dc0`, which nothing in the exe reads. The .asi keeps its
raw-address call. This row's `port` stays `none` (the matched body is not in the exe).

## Behaviour

`ExecuteFrontendBootChain` (`exe_main.cpp:9364`) only logs MainLoopInit's return value, so returning 1 does not open
any path the chain used to skip. What is new is the side effect of the body running to the end (before, it faulted
inside its first callee `0x00495110`, after writing `0x00828300 = 0`): writes to `0x007f1030` (QPC/3 MHz), `0x007f1000`,
`0x007f101c`, `0x007f1020`, `0x00771968 = 1`, RaceArmReset's latch (`0x0067eca4 = 1` and `0x0067ecb0 / e844 / e9f8 /
e914 / ecac / ea84`, plus CupFloatInit and RaceTransitionBufZero), `0x007f1a0c = 0x1000`, `0x007e9dc0`, and the QPF
store at `0x00771e70/74`. Several exe bodies read those addresses (e.g. `0x0067eca4` by RaceEndCheckFinish 0x00432080
and RaceFinalizeOnce 0x004331a0, `0x007f1a0c` by the menu button code), which is why the gates below are the check.

Result: the default gates did not move (table below), so these writes do not reach the G-CTRL race or the B arms on
this exe. Nothing was gated behind a knob because no hash changed and the chain's control flow is unchanged.

## Port column (scripts/backfill_exe_file.py then scripts/backfill_port.py, commit `2aee96c55`)

| rva | name | before | after |
|---|---|---|---|
| 0x00492770 | MainLoopInit | called | called (now returns) |
| 0x00495110 | FUN_00495110 | none | called |
| 0x0042b920 | ConstantGetter22 | called | called |
| 0x0042b950 | Set7f1a0c_1000 | none | called |
| 0x00433240 | FUN_00433240 (RaceArmReset) | none | called |
| 0x004c57a0 | FontCtxMatrix_AllocInit | none | none (librw binding, see above) |
| 0x004950b0 | QpcTimeScaledTo3Mhz | none | called |
| 0x00495120 | TimerQpfStore | none | called |
| 0x00402f80 | CupFloatInit | body | called |

Totals after: none=5306 body=586 called=259. 6 rows changed.

## Gates

Build: `cmd /c "set MASHED_NO_DEPLOY=1&& mashedmod\build.bat"`, both targets OK. rva-lint: `OK -- no NEW duplicate
bodies (1 known, tracked)`, same allowlist (no key added or removed, so `dual_copy_allowlist.txt` is unchanged).

Exe `mashedmod/build/mashed_re.exe` sha256 `926624178c4e1b536b1f93a32a1a4dfd87fe839a4c28dcffed44bb90ba962be9`
(commit `68c975043`, code tip; later commits are tracker/evidence only). Command:
`test-slot bash -c 'A4_ROOT=D:/MashedWT/PORT-TOP-01_root A4_OUT=D:/MashedWT/PORT-TOP-01_data/arms py -3.12
<orch-wt>/re/orchestrator/mashed_lock.py run --label PORT-TOP-01 -- py -3.12 verify/savelink_20261023/run_arms.py
H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26'`. (A first run lost its outputs because the `arms` folder did not exist and the
runner does not create it, a copy error after the H_DEF game had exited 0. Re-run in full after `mkdir`.)

| arm | exit | secs | file | lines | sha8 | ref (merge_train.json) |
|---|---|---|---|---|---|---|
| H_DEF | 0, self-exited | 277 | H_DEF.step.csv | 13962 | 8284be46 | 8284be46 |
| | | | H_DEF.gates.csv | 54085 | 67931834 | 67931834 |
| | | | H_DEF.ptrace.log | 27042 | 31fe043e | 31fe043e |
| B_DEF_K0 | 0, self-exited | 31 | B_DEF_K0.step.csv | 762 | 1d5bf4f9 | 1d5bf4f9 |
| B_DEF_K1 | 0, self-exited | 22 | B_DEF_K1.step.csv | 762 | 1d5bf4f9 | 1d5bf4f9 |
| B_DEF_K26 | 0, self-exited | 21 | B_DEF_K26.step.csv | 762 | 1d5bf4f9 | 1d5bf4f9 |

Every one of the four boots logs `[7] MainLoopInit returned 1` (per-arm exe logs `H_DEF.log`, `B_DEF_K*.log`), none
logs `AV'd`. H_DEF boots, runs the 14400-frame G-CTRL race and exits on its own. No render gate was run (not in this
row's gate list).

Boot chain extract (H_DEF boot, private root `D:/MashedWT/PORT-TOP-01_root`): `bootchain_H_DEF.txt`.

## Tracker leg

`py -3.12 re/tools/c4_reclass.py batch --tsv verify/porttop_20261108/c4_batch.tsv` (dry run) prints
`[1/1] 0x00492770 match SKIP already C4 (no write)`. The row is C4 on the frida route and the batch tool skips C4 rows,
so the `c4=match` claim the MATCH-S15-FU-MAINLOOP text asks for cannot be written through the sanctioned tool. Not
hand-edited (WORKER_RULES). The row's notes still say "match route NOT claimed ... claim it when the callees have exe
bodies"; the evidence for that claim is this RESULT. The C-level of the 5 callees does not change.

## Raw data (not committed)

`D:/MashedWT/PORT-TOP-01_data/arms/` (sha256):

```
8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198  H_DEF.step.csv
6793183492e0c0822570ad71254f9fdf27568b77c81a25c56347f1616863d58c  H_DEF.gates.csv
31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3  H_DEF.ptrace.log
13c1d4520ef1bffcd323a7dc8bcf55c58482e78fd16dfa29bcfcb54a92270247  H_DEF.log
1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457  B_DEF_K0.step.csv (= K1 = K26)
a3fd3d40fdbb613e9da07c6d2ff1dd210fab2d5e27c9bb6cfb2ff5dac2a4f814  B_DEF_K0.log
6a3dba1e521df63b3b7145c4231ba27809b6a20f2131a893c58f379e6b1afaac  B_DEF_K1.log
5ec02fbcc43eb8cfe002849b0f6087b94823601a6e4ae2385d6d70c8ed9a03ef  B_DEF_K26.log
df16914ab55149164c2b8387c7e467b8a09c44cdd78256b92d3239833e419c7e  RUNLOG.txt
```
Build log: `log/porttop01_build.log` in the worktree (not committed).
