# RESULT PORT-OT-RACE (run 52, 2026-11-16), checkpoint 1: matched course load 0x0040d270 on ORIG_TOP

Branch `orch/PORT-OT-RACE` from `orch/integration` `810042e0e`. Row kind: top-down. Worker claude3, Opus 5.5, lead
cmwjaf65a3d7fp0153fg1nefc. PREREG `verify/ro_20261116/PREREG_PORT-OT-RACE.md`, committed alone at `b893f0ac1`.
No G-CTRL, no B, no 12x4: not run, top-down row (those hashes are computed with ORIG_TOP off). No tracker edit, no
promotion, no default change. Nothing enqueued, merged or pushed. T2-RACEPATH-CENSUS had not landed, so it is not used.

## 0. Verdict: GREEN for the DONE line of this run

| DONE item | result |
|---|---|
| the exe arm (object build, top-down route) runs a race load with load length > 1 | yes: `RO_ROLOAD-pr1` load length (`race.dn_warp`) **3**, against 1 on the knob-off control and 108 on ref1 |
| re-run RO-EXE-ARM-LOADALIGN against ref1 and report the first divergence | **record 0 (tick 1), VALUE dword `0x00604604`: ref `0x00000004`, exe `0x00000000`**, unchanged from RO-EXE-ARM-LOADALIGN |

What "race load" means here, stated exactly: the matched body of 0x0040d270 runs (byte-identical object, below), with
its two 0x004950b0 clock reads, its leaf callees and its tail writes. The course load proper under it (0x00426e10 via
0x0040d020) and six other subtrees are named counting stand-ins. No track piz is mounted and no COURSE.LUA runs.

## 1. Change (knob `MASHED_OT_RACELOAD=1`, default OFF, exe only, ORIG_TOP only)

- `Match/PortTop_0040d440.cpp`: the matched 0x0040d440 now calls `PortTop_Bound_0040d270` (was the PORT-TOP-05 stand-in).
- `Match/PortTop_StandInsRL.cpp`: `PortTop_Bound_0040d270`. Knob off: the old stand-in. Knob on and the table pointer
  passed from `[0x005f2770]` is 0 (no file-backed .data image): the old stand-in, logged. Knob on: `Orig_0040d270`.
  Also the named stand-ins and the 0x004ccde0 guard (below).
- `Match/PortTop_0040d270.cpp`: `re/match/top07/0040d270.c` (match.py 100%, C4 by match) as `Orig_0040d270`.
- `Match/PortTop_0040d020.cpp`: `re/match/s02/0040d020.c` as `Orig_0040d020`, its 0x00426e10 = stand-in.
- Five matched leaves had a .asi-only TU and no exe body. Their existing TUs are now in `exe_sources.rsp` too (one TU
  in both lists, so no dual copy): `MatchS10_00462510.cpp` (Set603868_0), `MatchS10_00462500.cpp` (Set603868_1),
  `MatchS07_0042f790.cpp` (GhostModeIsActive), `MatchS07_0042f7a0.cpp` (GhostFlagReset), `MatchS06_00426070.cpp`
  (TrackPhysWorld2Get).
- `re/tools/dual_copy_allowlist.txt`: one `ORIGTOP-ONLY CROSS-TARGET:0040d020` line (pair with the .asi's
  `Track/CourseLoadDispatch.cpp`). rva-lint: NEW 0.
- Stack `verify/savelink_20261023/stacks/ROLOAD.txt` = `@ROIMG` + `MASHED_OT_RACELOAD=1`.

Callee bindings of 0x0040d270 (all by name):

| callee | binding |
|---|---|
| 0x00426ba0, 0x00426c00, 0x0042fe30, 0x00426020, 0x004950b0, 0x004987b0 | existing matched exe bodies |
| 0x0040cf80, 0x0040cfd0 | existing `Orig_` PortTop bodies |
| 0x00462510, 0x00462500, 0x0042f790, 0x0042f7a0, 0x00426070 | the shared matched TUs above |
| 0x0040d020 | `Orig_0040d020` (this row) |
| 0x004ccde0 | `RwCore_004ccde0` when `[0x007d45cc]` != 0, else skipped and counted |
| 0x00426e10 (under 0x0040d020), 0x004264d0, 0x004074a0, 0x0040d110, 0x00411d90, 0x0040d040, 0x00426340, 0x00407800, 0x00458c70 | named counting stand-ins (log first call and argument) |

## 2. Builds and checks

| check | result |
|---|---|
| VS2022 build (`MASHED_NO_DEPLOY=1`) | `=== Build OK ===`, `mashed_re.exe` SHA-256 `4fac8c06...` (first attempt stopped at rva-lint with 7 NEW findings, all from this row; fixed as in section 1) |
| object build (`MASHED_DDKOBJ=1`) | `=== Build OK ===`, `mashed_re_ddk.exe` SHA-256 `4cbd37b2...` |
| `ddkobj.py check` | IDENTICAL 1579/1619, DIFFERENT 24, not-in-map 16, units linked 1569/1786, refusals 11 (externalized-static-register-convention 7, wrapper-redefines-globals 4). The DIFFERENT list holds none of this row's RVAs (0x004929d0 is in it, as on RO-EXE-ARM-LOADALIGN's build) |
| this row's units in `BYTECHECK.tsv` | IDENTICAL: 0x0040d270 (448 B), 0x0040d440, 0x0040d020, 0x00462510, 0x00462500, 0x0042f790, 0x0042f7a0, 0x00426070 |
| knob boot (VS2022 exe, `MASHED_ORIG_TOP=1 MASHED_OT_RACELOAD=1 MASHED_DET_FRAMES=3000`) | `knob_rl_on: exited rc=0 (0x00000000) after 141.4 s`, `WinMain 0x00492370 returned 0 after 3000 frames`. No race load in a menu boot (the phase word does not reach 2) |
| menu walk | not run: no menu code changed |

## 3. Arms (object build, root `D:/MashedWT/PORT-OT-RACE_root` = robocopy of `RO-EXE-ARM-LOADALIGN_root`)

`A4_RO_CKPT=0,1,17,18,299,301,600`, replay json `verify/replayoracle_20261113/replay_trace.json` (identical to the one
RO-EXE-ARM-LOADALIGN used: no diff between the branches on the replay inputs, masks, harness or knob code).

| | ref1 | `RO_ROIMG-pr1` (knob off) | `RO_ROLOAD-pr1` (knob on) |
|---|---|---|---|
| exit | | 0, self-exited, 19 s | 0, self-exited, 9 s |
| `race.dn_warp` (load length) | 108 | 1 | **3** |
| `race.n` at the anchor | 3632 | 903 | 905 |
| anchor tick | 300 | 300 (warp + 0) | 300 (warp + 0) |
| `[REPLAY]` self-test | | PASS (0 of 666) | PASS (0 of 666) |
| `[TRACE]` self-test | | PASS (0 of 33294) | PASS (0 of 33294) |
| `[OT_TIMER]` self-test | | PASS (0 of 5990) | PASS (0 of 5990) |
| 0x0040d270 route (log) | | `stand-in (PORT-TOP-05) course load` x1 | `Orig_0040d270(table 005F2728, row 0) entered, call 1` |

The +2 reads are the two 0x004950b0 calls of 0x0040d270 (entry, and the elapsed value before the print), both inside
the warp tick while `DAT_00771968` is 2, so the anchor read is the third.

Knob-on log (`portotrace_roload_log_extract.txt`): A path taken (0x0040d040 calls 0). Stand-in first-call arguments:
0x004264d0 `0x1E`, 0x00426e10 `0x1E`, 0x00411d90 `0x1E` (the value `**(0x005F2728 + 0 * 4)`), 0x0040d110 row 0.
0x004ccde0 skipped 3 times (`[0x007d45cc]` = 0). After the body: `DAT_0063ba8c` = 1, `DAT_00803324` = `0x44C30000`,
`DAT_0080332c` = 0. Prediction 2 of the PREREG named a `course LoadTime is %u` line: it does not appear in `mashed_re.log`.
Where 0x004987b0's exe body writes was not checked here.

## 4. First divergence (re-run of RO-EXE-ARM-LOADALIGN against ref1)

Command (each arm): `py -3.12 re/frida/detorig_firstdiv.py D:/MashedWT/RO-REF-NAME_data/ref1 D:/MashedWT/PORT-OT-RACE_data/<arm>
--masks re/frida/warp_normalize.tsv,re/frida/replay_masks.tsv --ignore 0x00614400-0x00614800,0x00771800-0x00771c00
--exe-map mashedmod/build/ddkobj/mashed_re_ddk.map`

| | `RO_ROIMG-pr1` | `RO_ROLOAD-pr1` |
|---|---|---|
| first divergent record | 0 (tick 1) | 0 (tick 1) |
| counted blocks / raw words at record 0 | 118 / 76 | 118 / 76 |
| VALUE dwords at tick 1 | 5275 | 5275 |
| first VALUE dword | `0x00604604` ref 4, exe 0 | `0x00604604` ref 4, exe 0 |
| detorig_ticks verdict | IDENTICAL-FOR-N 17 of 600, first NEW block `0x00898c00` at record 17 | the same |
| tick 301 counted blocks / raw words | 503 / 96 | 503 / 96 |

The same first divergence as RO-EXE-ARM-LOADALIGN (record 0, `0x00604604`, 5275 VALUE dwords, IDENTICAL-FOR-N 17), as
the PREREG predicted: the knob acts only at tick 300. Report: `portotrace_fd_ref1_roload.md`.

Words the load writes, tick 301 (`portotrace_words.py`, ref `ckpt205` = tick 301, run `ckpt05` = tick 301):

| va | ref1 | knob off | knob on |
|---|---|---|---|
| `0x00803324` | `0x44c30000` | 0 | `0x44c30000` (same as ref) |
| `0x0063ba8c` | 2 | 0 | 1 |
| `0x0080332c` | `0x0f392560` | 0 | 0 |
| `0x00603868` | 1 | 1 | 1 |
| `0x0067ea70` / `0x0063ba78` / `0x0063ba7c` | 0 / 0 / 0 | the same | the same |
| `0x00771968` | 3 | 3 | 3 |

- `0x00803324` = 1560.0 is the store at 0x0040d3d2. It now matches ref1.
- `0x0063ba8c`: 0x0040d270 stores 1 (the source's tail `DAT_0063ba8c = 1`). ref1 holds 2 at tick 301, so a later writer
  changed it on the original side between the load and tick 301. Which writer is not measured here.
- `0x0080332c` is the return of 0x00426070 (`[0x00656ee8]`). ref1 holds a heap pointer there. On the exe it is 0
  because the course load 0x00426e10 is a stand-in.
- The tick-301 block count does not move (503 both arms): the 2 words the knob writes sit in blocks that differ for
  other words anyway.

## 5. Next

1. Load length 3 against 108: the rest of the original's 108 clock calls come from inside the course load. Getting them
   needs 0x00426e10 under ORIG_TOP. Its 35 direct callees (hooks.csv `port` column): 9 have an exe body (`body` or
   `called`: 0x004053d0, 0x0041e870, 0x004260e0, 0x004262f0, 0x0042a530, 0x0042a8d0, 0x0042f510, 0x004671a0,
   0x004987b0) and 0x00495280 has the PIZ_FS mount binding (`PortTop_PizFs_Mount_00495280`), 4 are C2 with no admitted body (0x00462950, 0x00478660, 0x00479330, 0x0047a020), the rest are matched
   C4 sources with no exe TU. The T2-RACEPATH-CENSUS table is the input for that ordering. RO-EXE-ARM-LOADALIGN already
   measured that the exe piz reader (whole-archive load) has no per-read poll loop, so even a ported 0x00426e10 on the
   exe reader will not reproduce the original's poll count.
2. The other stand-ins of this checkpoint (0x004264d0, 0x004074a0, 0x0040d110, 0x00411d90, plus 0x0040d040 /
   0x00426340 / 0x00407800 off the path taken) are the next port units under the course load.
3. The first divergence stays the RO-ORACLE-GATE boot-write class at record 0 (`0x00604604`), independent of this row.
4. Then the race tick 0x004111c0 (still the PORT-TOP-05 counting stand-in) as MATCH-RACEPATH lands.

## Evidence

Committed (`verify/ro_20261116/`): this RESULT, the PREREG, `portotrace_fd_ref1_roload.md` (firstdiv report, knob on),
`portotrace_fd301_roload.md` (tick 301), `portotrace_roload_log_extract.txt` (the knob's log lines, warp / anchor /
self-test lines of both arms), `portotrace_words.py`, `raw_sha256_PORT-OT-RACE.txt` (SHA-256 of every raw file cited and
of both exes).

Raw (not committed): `D:/MashedWT/PORT-OT-RACE_data` (both arm dirs, logs, firstdiv json/md/out, knob boot log),
`D:/MashedWT/PORT-OT-RACE_build_vs.log`, `_build_ddk.log`, `_ddkcheck.log`.

Commands:
- Builds: `cmd /c "set MASHED_NO_DEPLOY=1&& mashedmod\build.bat"`, then the same with `set MASHED_DDKOBJ=1&&`;
  `py -3.12 re/tools/ddkobj/ddkobj.py check`.
- Knob boot: `PORTTOP_OUT=D:/MashedWT/PORT-OT-RACE_data test-slot bash -c 'py -3.12 <orch-wt>/re/orchestrator/mashed_lock.py run --label PORT-OT-RACE -- py -3.12 verify/porttop_20261108/knob_boot.py knob_rl_on D:/MashedWT/PORT-OT-RACE_root 300 MASHED_ORIG_TOP=1 MASHED_OT_RACELOAD=1 MASHED_DET_FRAMES=3000'`
- Arms: `A4_OUT=D:/MashedWT/PORT-OT-RACE_data A4_ROOT=D:/MashedWT/PORT-OT-RACE_root A4_EXE=<wt>/mashedmod/build/ddkobj/mashed_re_ddk.exe A4_RO_CKPT=0,1,17,18,299,301,600 test-slot bash -c 'py -3.12 <orch-wt>/re/orchestrator/mashed_lock.py run --label PORT-OT-RACE -- py -3.12 verify/savelink_20261023/run_arms.py RO_ROIMG-pr1 RO_ROLOAD-pr1'`
