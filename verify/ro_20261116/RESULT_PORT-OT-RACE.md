# RESULT PORT-OT-RACE (run 52, 2026-11-16), checkpoints 1, 2 and 3: matched 0x0040d270, 0x00426e10 and 0x004264d0 on ORIG_TOP

Branch `orch/PORT-OT-RACE` from `orch/integration` `810042e0e`. Row kind: top-down. Worker claude2 (account2), Opus 5.5,
lead cmwjaf65a3d7fp0153fg1nefc. (Corrected at checkpoint 2: this header said "claude3" at checkpoint 1; the worker ran on
claude2.) Checkpoint 2 is section 6, below the checkpoint-1 text, which is unchanged. PREREG `verify/ro_20261116/PREREG_PORT-OT-RACE.md`, committed alone at `b893f0ac1`.
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

## 6. Checkpoint 2 (stop 2): the matched course load 0x00426e10 and its closure

PREREG section "Checkpoint 2", committed alone at `07a4513b7` before any build or arm.

### 6.0 Verdict: BLOCKED at 0x0047a020 (C2, no matched or admitted body); the port of the course load's prefix is GREEN

| item | result |
|---|---|
| the matched 0x00426e10 runs on the exe arm | yes: `PortTop_Bound_00426e10(track 30)` entered, `[PIZ_FS] 0x00495280 mount c:\toastart\tracks\training.piz -> original/toastart/tracks/training.piz: OK, 43 entries` |
| every bound body byte-identical in the object build | yes: 85 of 85 bound rvas IDENTICAL in `BYTECHECK.tsv` (`portotrace_ck2_bytecheck_rows.txt`) |
| stops-exit arm (`RO_ROLOAD-pr1`) | ends at the first stop in the source's call order, **0x0047a020** (COURSE.LUA, line 111 of `re/match/s06/00426e10.c`), exit code 4694048 = `0x0047A020`, 300 ticks recorded |
| stops-count arm (`RO_ROLOADC-pr1`) | runs past 0x0047a020, 0x00479330, the LAPDATA.LUA read (0x0047a0f0) and 0x00478660, then faults (exit `0xC0000005`) after the stops under 0x00480340's subtree that returned 0 (last logged: 0x0055f800, caller 0x0055f450). 283 records written |
| load length | **not measured**: neither knob-on arm reaches the race anchor (`race.reached` false, `race.n` 0 in both meta.json). Knob-off control: 1. ref1: 108. Checkpoint 1 (stand-in course load): 3 |
| first divergence against ref1 | unchanged on all three arms: **record 0 (tick 1), `0x00604604` ref `0x00000004` exe `0x00000000`**, 5275 VALUE dwords, IDENTICAL-FOR-N 17. Audio lane, not chased |

The block: the course load cannot finish on the exe until 0x0047a020 has a body. It is C2 with no matched,
admitted or provisional source. 0x00479330, 0x00478660 and 0x00462950 are the same class and follow it in the call order.
Stops are this row's design (a reached stop names the next body to bind). Giving these rvas a body is matching / L-126
admission work, outside a port row, so the run stops here as the brief says for a blocked stop.

### 6.1 Change (knob `MASHED_OT_RACELOAD=1`, default OFF, exe only, ORIG_TOP only)

- `Match/PortTop_0040d020.cpp`: the matched 0x0040d020 calls `PortTop_RL_00426e10` (`Match/PortTop_StandInsRL.cpp`,
  logs, then `PortTop_Bound_00426e10`). The counting stand-in `PortTop_StandInRL_00426e10` stays defined and is no longer called.
- Generator `verify/ro_20261116/rltools/rl_bind_gen.py` (reuses the definitions of
  `verify/portorigtop_20261113/tools/audiotick_bind_gen.py`). Plan `rltools/plan_rl.json`, listing `rltools/plan_6e10.txt`.
  For root 0x00426e10: **85 bound** (all c4=match sources, one ORIGTOP-only TU each, `Match/PortTop_<rva>.cpp`),
  **37 reused** exe bodies, **32 named stops** (`Match/PortTop_RaceLoadStops.cpp`).
  - Reuse order: a PortTop match-include body, then the RW core's body `RwCore/RwCore_<rva>.cpp` (L-124), then a matched
    or admitted source to bind, then another match-include exe body. Hand-path bodies (RH only) are not bound: the rva is
    a stop. Leaf C4 rows keep the audio generator's rule (its `reuse`).
  - File layer aliases: 0x00495280 / 0x004952f0 -> `PortTop_PizFs_Mount_00495280` / `PortTop_PizFs_Unmount_004952f0`
    (PIZ_FS mount stack), 0x004cc230 / 0x004cc160 -> the PORT-TOP-04F RW stream open / close bindings, 0x00550b00 ->
    `RwCoreBind_FsFileExists_00550b00`.
  - `NOT_LOCAL`: `re/match/s13/0047bf70.c` holds a HELD copy of 0x0047be80 under `#ifndef MATCH_PORT`, so on the port
    0x0047be80 is an external callee (a stop: C2), not file-local.
- **24 matched sources do not compile as C++** under `MATCH_PORT` (C-only pointer / integer conversions, raw-converter
  `bool` / `wchar_t` typedefs). The exe build compiles `.cpp` TUs only (`mashedmod/build_objs.ps1` refuses other
  entries), so they are named stops with the first VS2022 error recorded: `rltools/cpp_unclean.txt`. Among them
  0x00409680 / 0x00409710 (direct callees of 0x00426e10, after 0x0047a020), 0x00440bc0 (the COURSE.LUA / LAPDATA.LUA
  handler registration), 15 Lua handlers 0x0047a1b0..0x0047b250, 0x0047b860, 0x0047b8d0, 0x00425bf0, 0x00458dd0,
  0x004c3d90 and 0x004e89a0.
- 8 sources declared their globals as plain `extern` data, which the exe cannot link. `rltools/guard_externs.py` puts
  each such line under `#ifndef MATCH_PORT` and binds the port to the original address (`#ifdef MATCH_PORT #define`).
  The DDK never defines MATCH_PORT; `re/tools/match/match.py --bytes` after the edit: all 8 BYTE-IDENTICAL
  (`portotrace_ck2_matchpy.txt`): `re/match/s01/00405780.c`, `s01/004095a0.c`, `s01/raw/00409680.c`,
  `s01/raw/00409710.c`, `pilot/a7_004260e0.c`, `pilot/a3_004715a0.c`, `pilot/a5_0047f4c0.c`, `tlr/00440bc0.c`.
- Stops: default a reached stop logs and exits with code = rva (`PortTop_AudioCalleeStop`). `MASHED_OT_RL_STOPS=count`:
  log the first call, return 0. Stops a caller declares float / double (0x0055b940) exit in both modes.
- `mashedmod/exe_sources.rsp` +86 TUs (`rltools/rl_sync.py`), `re/tools/dual_copy_allowlist.txt` +38 `ORIGTOP-ONLY`
  lines (pruned to the bound set, `rltools/prune_allowlist.py`). rva-lint NEW 0. Stack
  `verify/savelink_20261023/stacks/ROLOADC.txt` = `@ROLOAD` + `MASHED_OT_RL_STOPS=count`.

### 6.2 Builds and checks

| check | result |
|---|---|
| VS2022 build (`MASHED_NO_DEPLOY=1`) | `=== Build OK ===` after three fixes in this row (stop comments read as body markers by rva-lint, the 24 cpp-unclean TUs, the 0x0047be80 link error). `mashed_re.exe` SHA-256 `e4610cf7...` |
| object build (`MASHED_DDKOBJ=1`) | `=== Build OK ===`, `mashed_re_ddk.exe` SHA-256 `9b8f1bdc...` |
| `ddkobj.py check` | IDENTICAL 1667/1706 (checkpoint 1: 1579/1619), DIFFERENT 24 (the same 24 as checkpoint 1, none of this row's), not-in-map 15, units linked 1654/1871, refusals 11 |
| knob boot to the menu | not run: no menu code changed, and the race load is not reached in a menu boot (checkpoint 1) |

### 6.3 Arms (object build, root `D:/MashedWT/PORT-OT-RACE_root`, data `D:/MashedWT/PORT-OT-RACE_data2`)

| | ref1 | `RO_ROIMG-pr1` (knob off) | `RO_ROLOAD-pr1` (stops exit) | `RO_ROLOADC-pr1` (stops count) |
|---|---|---|---|---|
| exit | | 0, 11 s | 4694048 (`0x0047A020`), 6 s | `0xC0000005`, 7 s |
| ticks recorded | | 600 | 300 | 283 |
| `race.dn_warp` (load length) | 108 | 1 | not reached | not reached |
| first divergent record vs ref1 | | 0, `0x00604604` | 0, `0x00604604` | 0, `0x00604604` |
| detorig_ticks verdict | | IDENTICAL-FOR-N 17 of 600 | 17 of 300 | 17 of 283 |

Knob-on log, both arms (`portotrace_ck2_log_extract.txt`): 0x0040d270 entered, 0x004ccde0 skipped (`[0x007d45cc]` 0),
0x004264d0 stand-in (arg `0x1E`), `PortTop_Bound_00426e10(track 30)` entered, training.piz mounted (43 entries),
`cfglog: Searching piz for COURSE.LUA`, 0x00550b00 `("pc/COURSE.LUA")` 0 then `("COURSE.LUA")` 1 through the
RtFS-manager-0 shim. Exit arm: the 0x0047a020 stop line and `[OT_TIMER] STOP ... 0x0047A020`, `[OT_TIMER] self-test PASS
(0 of 2990)`. Count arm, stops hit in order: 0x0047a020, 0x00479330, 0x0047b860, 0x00440bc0, 0x0047b8d0, 0x004b7480,
0x00478660, 0x0057c4b0, 0x00559560, 0x005595d0, 0x0055f800, then the fault. [UNCERTAIN] The faulting instruction is not
captured (no exception logger on this path). The last stops returned 0 for results their callers declare `int *`
(0x005595d0), so the fault is a consequence of count mode, not a finding about a bound body (register line).

The `Opening piz file [%s]` print (0x004987b0 at line 107 of the source) is not in `mashed_re.log`, the same as the
`course LoadTime` print at checkpoint 1. Where the exe's `DbgPrintf_004987b0` writes was not checked.

### 6.4 Next (each a separate stop)

1. 0x0047a020 (COURSE.LUA run, C2), then 0x00479330, 0x00478660, 0x00462950: a body for each (match, L-126 admission
   or a listed provisional body) before the course load can finish on the exe. Load length becomes measurable again
   only then.
2. The 24 cpp-unclean sources: C++-compatible text under `MATCH_PORT` guards (DDK bytes unchanged, checked by match.py),
   then rebind. 0x00409680 / 0x00409710 are on the course load's own path after 0x0047a020.
3. Ready and independent of the block: the checkpoint-1 stand-in 0x004264d0 runs **before** the course load (it is on the
   path taken, arg `0x1E`), so its closure (dry run: 101 bound, 29 reused, 13 stops) is measurable on the exit arm. The
   other subtrees (0x004074a0, 0x0040d110, 0x00411d90 after the load; 0x0040d040, 0x00426340, 0x00407800 (C2),
   0x00458c70 off the path taken) bind statically but are not reached while 0x0047a020 exits.

### 6.5 Evidence

Committed (`verify/ro_20261116/`): this section, `portotrace_ck2_log_extract.txt`, `portotrace_ck2_fd_ref1_roload.md`,
`portotrace_ck2_fd_ref1_roloadc.md`, `portotrace_ck2_matchpy.txt`, `portotrace_ck2_bytecheck_rows.txt`,
`raw_sha256_PORT-OT-RACE_ck2.txt`, `rltools/` (generator, plan, listings, cpp_unclean list, helper scripts).

Commands:
- Plan + wrappers: `py -3.12 verify/ro_20261116/rltools/rl_bind_gen.py . 00426e10 --write`, then `py -3.12 verify/ro_20261116/rltools/rl_sync.py`.
- Builds: as checkpoint 1; `py -3.12 re/tools/ddkobj/ddkobj.py check`.
- Arms: as checkpoint 1 with `A4_OUT=D:/MashedWT/PORT-OT-RACE_data2` and arms `RO_ROIMG-pr1 RO_ROLOAD-pr1 RO_ROLOADC-pr1`.
- First divergence: as checkpoint 1, per arm, into `D:/MashedWT/PORT-OT-RACE_data2/fd_ref1_<arm>.out`.

## 7. Checkpoint 3 (stop 3): the matched 0x004264d0 and its closure

`orch/integration` `215d5bbb9` merged into the branch first (`9410ca739`, clean, the only file both sides touched was
`re/orchestrator/UNCERTAIN_REGISTER.md`). PREREG section "Checkpoint 3" committed alone at `e46a08bc5`; after the first
arms faulted, the lead chose (b) and the amendment was committed alone at `c40ea3f66`, before the re-run.
0x0047a020 / 0x00479330 / 0x00478660 / 0x00462950 (owned by RACE-COURSELUA-BODIES) are not touched: they stay named stops.

### 7.0 Verdict: GREEN for stop 3 as amended (the matched 0x004264d0 runs and returns; the exit arm ends at the 0x0047a020 stop again)

| item | result |
|---|---|
| the matched 0x004264d0 runs on the exe arm | yes: `PortTop_Bound_004264d0(0000001E) entered`, the 0x00458c70 stand-in line, `matched body returned`, then the 0x00426e10 entry |
| every bound unit byte-identical in the object build | yes: the 7 new bound rvas and the file-local 0x00426460 IDENTICAL (`portotrace_ck3_bytecheck_rows.txt`) |
| stops-exit arm (`RO_ROLOAD-pr1`) | ends at **0x0047a020**, exit code 4694048 = `0x0047A020`, 300 ticks, as at checkpoint 2 |
| stops-count arm (`RO_ROLOADC-pr1`) | the same stop sequence as checkpoint 2, then `0xC0000005` after 0x0055f800 |
| knob-off control (`RO_ROIMG-pr1`) | unchanged: exit 0, 600 ticks, load length 1 |
| load length | **not measured**: no knob-on arm reaches the race anchor (`race.reached` false, `race.n` 0). ref1: 108. Checkpoint 1: 3 |
| first divergence against ref1 | unchanged on all three arms: record 0 (tick 1), `0x00604604` ref 4 exe 0, IDENTICAL-FOR-N 17 |

What does not run: 0x00458c70 is a counting stand-in (7.2), and the body returns right after 0x0042fe30 on the exe
(no PIZ_FS mount line between entry and return), so its piz / clump / Lua branch (0x00495280, 0x0042a5d0, 0x00426460,
0x0042a470, 0x0047a130, 0x004952f0) is bound and byte-checked but not executed on this arm.

### 7.1 Change (knob `MASHED_OT_RACELOAD=1`, default OFF, exe only, ORIG_TOP only)

- `Match/PortTop_0040d270.cpp`: 0x004264d0 binds to `PortTop_RL_004264d0` (`Match/PortTop_StandInsRL.cpp`: logs entry with
  `DAT_0067ea74` and `[0x007d3ff8]`, calls `PortTop_Bound_004264d0` under an SEH filter that logs a fault and passes it on
  with `EXCEPTION_CONTINUE_SEARCH`, then logs the return). The stand-in `PortTop_StandInRL_004264d0` stays defined, no longer called.
- Generator `rl_bind_gen.py . 00426e10,004264d0 --write` (listing `rltools/plan_64d0.txt`): **92 bound, 45 reused,
  33 stops**. Against checkpoint 2: +7 bound (0x004264d0, 0x0042a5d0, 0x00458d00, 0x00458e00, 0x00458fd0, 0x00472690,
  0x0047a130), +1 stop (0x00458dd0, cpp-unclean: C2664 on `Orig_004e8090`), none of checkpoint 2's 85 bound lost.
  The dry run's "101 bound, 29 reused, 13 stops" was counted before checkpoint 2's TUs existed. Three generator rules added:
  - `ALIAS 0x0042fe30 -> RaceEndFlagIfEndMode` (`Match/MatchS07_0042fe30.cpp`): the exe already compiles that matched source
    once (callee 0x0042f6a0 = `GetRaceSubMode`). `reuse()` skipped it because the source is not a leaf.
  - `CALLEE_OVERRIDE (0x0042a5d0, 0x004b3bf0) -> MatchS06_0042a5d0_Call004b3bf0`: the source declares 0x004b3bf0 with 1
    argument, `Orig_004b3bf0` takes 3. The existing forwarder passes 0 / 0 (deviation declared in `Match/MatchS06_0042a5d0.cpp`).
    0x0042a5d0's source is the only bound source that declares 0x004b3bf0.
  - `ALIAS 0x00458c70 -> PortTop_StandInRL_00458c70` (lead decision (b), 7.2): the named counting stand-in (logs its first
    call, returns, no RW core call). The generated `PortTop_00458c70.cpp` and its rsp line are removed by the generator.
- `rltools/same_source_exe.py` (new): 7 planned binds have a source another exe TU already compiles under `FUN_<rva>`
  (`exe_body_index.py` does not see those). 6 were bound at checkpoint 2 (0x00423480, 0x00425c00, 0x004262f0, 0x0042a470,
  0x0042a530, 0x0042a8d0) and stay separate bound copies: a bound copy takes the race-load closure's callee bindings.
  0x0042a5d0 is bound the same way, with the override above.
- `exe_sources.rsp` +7 TUs. `re/tools/dual_copy_allowlist.txt` +5 `ORIGTOP-ONLY` lines: 0x004264d0, 0x00426460,
  0x00472690 (CROSS-TARGET against their .asi MatchS TUs), 0x0042a5d0 (DUP-IN-TARGET against `MatchS06_0042a5d0.cpp`),
  0x00458e00 (DUP-IN-TARGET against the hand-path `Gameplay/PickupPoolSpawn.cpp`). rva-lint NEW 0.
  `rltools/prune_allowlist.py` removed three lines whose rva is a file-local function, not a plan key (0x0047bc90,
  0x0047bcc0 from checkpoint 2, 0x00426460); they were put back (the first two from HEAD, the third by `rl_sync.py`). Its
  rule needs the file-local keys before it is used again.
- Stack `verify/savelink_20261023/stacks/ROLOADT.txt` = `@ROLOAD` + `MASHED_CODE_TRAP=1` (diagnostic, used once).

### 7.2 The fault that made 0x00458c70 a stand-in (first arms, before the amendment)

With 0x00458c70 bound (the matched body), both knob-on arms ended `0xC0000005` inside 0x004264d0, before any stop or
mount line (`D:/MashedWT/PORT-OT-RACE_data3`). The code trap arm logged no trap (the fault is outside 0x00401000..0x005cbfff).
The SEH filter run (`D:/MashedWT/PORT-OT-RACE_data3b/RO_ROLOAD-pr1.log` lines 827..830):

- fault `C0000005` at `exe+0x23415` = `_RwCore_004c15c0+0x85` (map preferred load address `0x00010000`, symbol at
  `0x00033390`), original **0x004c1645**. `RwCore_004c15c0` is IDENTICAL in `BYTECHECK.tsv` (192 B).
- access kind 1 (write), address `0x00000004`, ecx 0, edx `0x11292148`.
- at the fault `[0x007d3ff8]` = `0x01106060` and `[0x01106060 + 0xbc]` = **0**.
- The source line (`re/match/rwl01/004c15c0.c`): `*(int *)(*(int *)(DAT_007d3ff8 + 0xbc) + 4) = *(int *)(param_1 + 0xa0) + 8;`
  in the `(*(byte *)(*(int *)(param_1 + 0xa0) + 3) & 3) == 0` branch. The call is 0x00458c70's
  `FUN_004c15c0(*(int *)(r->clump + 4))`, first record (index 24).
- `0x01106060` is the librw RwGlobals record of this arm: `[RW_BIND] RwGlobals: 0x007d3ff8 00FF1C90 (B15 device) -> record
  01106060; ... strcpy / strcat at +0xcc / +0xd4, memory functions at +0x108..+0x114 on librw`. That record's +0xbc is 0.
  [UNCERTAIN] Whether +0xbc is the RW core's dirty-frame list head is read from 0x004c15c0's source only; no RW core
  RwGlobals is built on this arm to compare.
- The 25 clump fields of the pool at `0x0068b198` (stride 0x50) are non-zero on both sides at tick 299 (ref1 `ckpt204`,
  exe `ckpt04`, `portotrace_ck3_words_t299.txt`), so the fault is not a NULL clump.

A second difference in the same body, from the same dump: **`0x0067ea74` ref1 1, exe 0** at tick 299; `0x0067e9fc` 0 on
both. 0x0042fe30 returns `DAT_0067ea74` unless 0x0042f6a0 returns 0xb, so on the exe 0x004264d0 returns after 0x0042fe30
and the original takes the branch with the piz mount. Which writer sets `0x0067ea74` on the original is not measured.

### 7.3 Builds and checks (after the amendment)

| check | result |
|---|---|
| VS2022 build (`MASHED_NO_DEPLOY=1`) | `=== Build OK ===` (first attempt after the amendment stopped at rva-lint NEW 1, 0x00426460, the pruned line, fixed as in 7.1). `mashed_re.exe` SHA-256 `ad14d57e...` |
| object build (`MASHED_DDKOBJ=1`) | `=== Build OK ===`, `mashed_re_ddk.exe` SHA-256 `3bd2db82...` |
| `ddkobj.py check` | IDENTICAL 1675/1714 (checkpoint 2: 1667/1706; the merged integration adds units), DIFFERENT 24 (the same 24), not-in-map 15, units linked 1661/1878, refusals 11 |

### 7.4 Arms (object build, root `D:/MashedWT/PORT-OT-RACE_root`, data `D:/MashedWT/PORT-OT-RACE_data3c`)

| | ref1 | `RO_ROIMG-pr1` (knob off) | `RO_ROLOAD-pr1` (stops exit) | `RO_ROLOADC-pr1` (stops count) |
|---|---|---|---|---|
| exit | | 0, 11 s | 4694048 (`0x0047A020`), 5 s | `0xC0000005`, 6 s |
| ticks recorded (meta) | | 600 | 300 | 300 |
| `race.dn_warp` (load length) | 108 | 1 | not reached | not reached |
| first divergent record vs ref1 | | 0, `0x00604604` | 0, `0x00604604` | 0, `0x00604604` |
| detorig_ticks verdict | | IDENTICAL-FOR-N 17 of 600 | 17 of 300 | 17 of 283 |
| self-tests | | `[REPLAY]` PASS 0/666, `[TRACE]` PASS 0/33294, `[OT_TIMER]` PASS 0/5990 | `[OT_TIMER]` PASS 0/2990 | |

Knob-on log, both arms (`portotrace_ck3_log_extract.txt`): 0x004264d0 entered with `0x1E`, `DAT_0067ea74` 0,
`[0x007d3ff8]` `0x01106020`; the 0x00458c70 stand-in first call; 0x004264d0 returned; `PortTop_Bound_00426e10(track 30)`
entered. Exit arm: the 0x0047a020 stop (callers 0x00426e10) and `[OT_TIMER] STOP ... 0x0047A020`. Count arm, stops in
order: 0x0047a020, 0x00479330, 0x0047b860, 0x00440bc0, 0x0047b8d0, 0x004b7480, 0x00478660, 0x0057c4b0, 0x00559560,
0x005595d0, 0x0055f800, then the fault (the checkpoint-2 sequence). PREREG predictions 14, 15, 16 hold; prediction 13
holds for the amended set. Predictions 9 to 11 were stated for the bound 0x00458c70 and were overtaken by the fault in 7.2.

### 7.5 Next

1. The RW layer under the race load: 0x00458c70 (and every other RW core body the closure reaches) needs the RW core's
   RwGlobals on this arm, not the librw record (L-124). Until then 0x00458c70 stays a stand-in.
2. The writer of `0x0067ea74` on the exe: with it 0x004264d0 takes the piz / clump branch, whose stops (0x00458dd0,
   0x00440bc0, 0x0047b860, 0x004b7480, 0x0047b8d0) become reachable.
3. Unchanged from checkpoint 2: 0x0047a020 / 0x00479330 / 0x00478660 / 0x00462950 (RACE-COURSELUA-BODIES), and the
   24 cpp-unclean sources of `rltools/cpp_unclean.txt` (0x00458dd0 is one of them; this checkpoint's plan reaches it).

### 7.6 Evidence

Committed (`verify/ro_20261116/`): this section, PREREG sections "Checkpoint 3" and its amendment,
`portotrace_ck3_log_extract.txt`, `portotrace_ck3_fd_ref1_roload.md`, `portotrace_ck3_fd_ref1_roloadc.md`,
`portotrace_ck3_words_t299.txt` (ref1 `ckpt204` against the first fault arm's `ckpt04`), `portotrace_ck3_bytecheck_rows.txt`,
`raw_sha256_PORT-OT-RACE_ck3.txt`, `rltools/plan_64d0.txt`, `rltools/same_source_exe.py`, the generator edits.

Commands:
- Plan + wrappers: `py -3.12 verify/ro_20261116/rltools/rl_bind_gen.py . 00426e10,004264d0 --write`, then
  `py -3.12 verify/ro_20261116/rltools/rl_sync.py` (do not run `prune_allowlist.py` until it knows file-local keys).
- Builds and check: as checkpoint 1.
- Arms: as checkpoint 1 with `A4_OUT=D:/MashedWT/PORT-OT-RACE_data3c` and arms `RO_ROIMG-pr1 RO_ROLOAD-pr1 RO_ROLOADC-pr1`
  (fault arms: `_data3` with `RO_ROLOADT-pr1`, `_data3b` with `RO_ROLOAD-pr1`).
- Words: `py -3.12 verify/ro_20261116/portotrace_words.py D:/MashedWT/RO-REF-NAME_data/ref1/ckpt204.data.bin
  D:/MashedWT/PORT-OT-RACE_data3/RO_ROLOAD-pr1/ckpt04.data.bin <the 25 clump fields 0x0068b198 + i * 0x50>,0x0067e9fc,0x0067ea74,0x0068b9a8`.
- First divergence: as checkpoint 1, per arm, into `D:/MashedWT/PORT-OT-RACE_data3c/fd_ref1_<arm>.out`.
