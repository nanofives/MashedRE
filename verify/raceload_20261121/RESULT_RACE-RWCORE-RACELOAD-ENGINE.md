# RESULT RACE-RWCORE-RACELOAD-ENGINE, step 1 (run 54, 2026-11-21): first stop on the core engine = 0x00458dd0, a GAME function (not a library function)

Branch `orch/RACE-RWCORE-RACELOAD-ENGINE` from `orch/integration` `77dedfece`. PREREG
`verify/raceload_20261121/PREREG_RACE-RWCORE-RACELOAD-ENGINE.md`, committed alone (`c36382b96`) before any build or arm.
Row kind: race path. Worker claude2, Opus 5.5 high, lead "Mashed run 54 RACE lead". Step 1 only: one arm, nothing
bound, no source change, no default change, no tracker edit, no promotion. G-CTRL / B / 12x4: not run (race-path arm on
knobs, the knob-off hashes cannot see it; no default-path code changed).

## 0. Verdict

| item | result |
|---|---|
| G2 done-means | **met by the second form**: the first stop on the RW core engine is named, with address and class |
| first stop | **0x00458dd0** (`FUN_00458dd0`, hooks.csv `gameplay`, C4 `c4=match`, `re/match/s09/00458dd0.c` 100.0%), the race-load stop `PortTop_RLStop_00458dd0` (`Match/PortTop_00458e00.cpp:11`, `Match/PortTop_RaceLoadStops.cpp:62`), called from `PortTop_Bound_00458e00+0xB7` (return 0x0004E427 = exe+0x3E427, `mashed_re_ddk.map`). Log line 456 `rva 0x00458DD0: race-load stop (PORT-OT-RACE), call 1, callers 0x00458e00, return 0004E427, exits` |
| class | **game** (gameplay pickup pool, not RW, not CRT / Lua / zlib / libpng / qhull). Its source is a 100% byte match but compiles C++-unclean in the PortTop TU (`C2664 'int *Orig_004e8090(int *,int)'`, `PortTop_RaceLoadStops.cpp:61`), which is why it is a stop. Callees: 0x00458630 (frontend, C4), 0x004b4080 (render, C4), 0x004e8090 (RW, `RwCore_004e8090` match100) |
| load length | **not measured: the race is not reached** (`meta.json` `race.reached` false, `race.n` 0; ref1: 108). The arm exits at the stop, exit code 0x00458DD0, 6 s |
| the RACE-RWLAYER fault | **not hit on the core engine.** RACE-RWLAYER's librw arm faulted at `RwCore_00543d40+0xd` inside 0x004b5190. Here the race load gets past the DFF read and past 0x004b5190 to 0x00458fd0 -> 0x00458e00 (section 2; [UNCERTAIN] on the exact call path, register line "RACE-RWCORE-RACELOAD-ENGINE (core clump branch)") |
| self-tests | all PASS: `[portstate-image]` 0 of 8, `[OT_CODEPTR]` 0 of 367 (image slots 107 written, 0 refused), `[REPLAY]` init 0 of 66, `RwTexDictionaryCreate` self-test 0 of 11 (calls 1 and 2), `[RWC-DEVICE]` 0 fail, `[OT_TIMER]` 0 of 2990, trace self-test 22196 checks 0 fails (`meta.json`) |
| first divergence vs ref1 | record 0 (tick 1), `0x00604604` ref `0x00000004` exe `0x00000000` (the same first word as RACE-RWLAYER), 85 counted blocks / 68 raw words, IDENTICAL-FOR-N 1 of 300, first NEW block `0x007f0c00` at record 1 (`fd_ref1_RO_ROLOADRWC-pr1.md`). Informational only: the arm stops inside the warp, before any race tick |
| object build | IDENTICAL 1976/2035, DIFFERENT 32, not-in-map 27, refusals 12, dual body 32: `BYTECHECK.tsv` byte-identical to `log/orch/run54_headcheck/BYTECHECK.tsv` (2036 rows). **No newly DIFFERENT unit**, no listed unit -> 100% neighbour fallback pair new against the head (no source changed) |

## 1. Arms

One arm, `RO_ROLOADRWC-pr1`: stack `verify/savelink_20261023/stacks/ROLOADRWC.txt` = `@ROLOAD` (= `@ROIMG` +
`MASHED_OT_RACELOAD=1`) + `MASHED_RWC_CORE=1` + `MASHED_RWC_FRAME=1` + `MASHED_RWC_FRAME_N=100000` (FRAME_N lifts the
default end after the first present, `RwCore/RwCore_Knob.cpp:437`, as on the run-52 frameT arms). Object build
`mashed_re_ddk.exe` `e78a5d91`, root `D:/MashedWT/RACE-RWCORE-RACELOAD-ENGINE_root`, test-slot outside and mashed_lock
inside, `A4_RO_CKPT=0,1,17,18,299,301,600`. `mashed_lock.py status` before each build and launch: no PRIORITY-HOLD or SOLO
(free, or HELD by a plain game run of PROV-INSITU-REF1-WS2 / REPLAY-FALLBACK-8, then queued).

**First launch invalid (harness), re-run once.** The first launch ran without `original\mashed_image.bin` in the fresh
root: `[portstate-image] REFUSED(missing) mashed_image.bin: nothing written; the per-address seed runs instead` (log
line 2). .data stayed mostly zero (0x005EA000..0x005F2770 all zero at checkpoint 0, 6,270 dwords there non-zero on
RACE-RWLAYER's arm and on ref1), so 0x0040d270 faulted at 0x0040d2cc (`Orig_0040d270+0x5C`, `mov eax,[edx]` with
`edx = [0x005F2728 + row*4]` = 0; static image value `0x005F2570`). That is the missing image, not an RW core finding,
and the PREREG arm (`@ROIMG`) had not run. Image staged with `re/tools/image/extract_image.py` (sha256 `81CC7775...`, byte
for byte RACE-RWLAYER's root copy, `image_check.py` 392 of 392 PASS), the same arm re-run once. Raw data of the invalid
launch kept as `RO_ROLOADRWC-pr1_noimage*` (extract at the end of `raceload_log_extract.txt`).

## 2. What the valid arm shows (`raceload_log_extract.txt`, line numbers of `RO_ROLOADRWC-pr1.log`)

1. Engine on the core: data seed, `RwEngineInit 0x004c32b0 = 1`, `RenderwareAttachPlugins 0x00493640() = 1 on the RW core`,
   `RwEngineOpen 0x004c30b0 = 1`, `RwEngineStart 0x004c2fb0() = 1`, `MASHED_RWC_FRAME: ... the top-down path continues
   with the bindings on the core`; `[RW_BIND] RwGlobals: MASHED_RWC_FRAME: 0x007d3ff8 01731518 -> the RW core's
   RwGlobals 01731518 (librw record not installed)` (line 355).
2. 300 frames on the core (presents 1..300, each frame `rendered by 0x00492e90 (matched, RW core) = 1`), then the replay
   warp at tick 300 (`track 0 mode 10 rule 0 car 0 team 0 cars 4`).
3. Race load: `0x0040d270` matched body entered (table 005F2728, row 0), `0x004264d0` entered with `DAT_0067ea74 = 1`,
   `0x00458c70` runs and returns, training.piz mounted (43 entries), `powerups_gold.dff` 11902 bytes from piz into a
   memory stream (`[RW_FS] 0x004cc230(2, 1, ...)`).
4. **The clump read runs on the core.** The log has **no** `[RW_BIND] RpClumpStreamRead` line in the whole run (the librw
   binding logs every call; RACE-RWLAYER's arm has 11+ of them, `RO_ROLOADRW-pr1.log:239..` and line 856 for this read).
   `MashedRw_ClumpStreamRead_004e7420` takes `RwCore_004e7420` as its first statement under `RwCore_FrameRoute()`
   (`LibRw/RwApiBind.cpp:981`), and the binding has no log line on that branch.
5. Stop at 0x00458dd0 from 0x00458e00 (lines 456..457), then `[OT_TIMER]` self-test PASS and exit 0x00458DD0.

Path from the DFF read to the stop, read from the matched sources: 0x004264d0 `clump = FUN_0042a5d0("powerups_gold.dff")`;
`clump != 0` -> `FUN_00426460(clump)` -> per atomic `FUN_004b5190(list[i], 0, 0)` then `FUN_00458fd0(obj, v & 0xff, ...)`
(`re/match/s06/004264d0.c`); 0x00458fd0 = `FUN_00458fa0(FUN_00458e00(p, type), v)` (`re/match/s09/00458fd0.c:17`);
0x00458e00 calls 0x00458dd0. The other branch (`clump == 0`) searches `powerups_gold.lua` / `powerups.lua` and runs Lua
0x0047a130; no such search line is in the log. **[UNCERTAIN]**: that the clump branch ran and 0x004b5190 (with
0x00543d40) returned for at least one atomic rests on these sources plus absent log lines; the 0x00458e00 caller and the
clump pointer are not logged. Test: log the `RwCore_004e7420` return value and the 0x00458e00 return address on this arm.
Register line "RACE-RWCORE-RACELOAD-ENGINE (core clump branch)" in `re/orchestrator/UNCERTAIN_REGISTER.md`.

## 3. Builds and checks

| check | result |
|---|---|
| VS2022 build (`MASHED_NO_DEPLOY=1`) | `=== Build OK ===`, rva-lint 465 / 465 allowlisted, NEW 0. `mashed_re.exe` `e68947e9...` |
| object build (`MASHED_DDKOBJ=1`) | `=== Build OK ===`, `mashed_re_ddk.exe` `e78a5d91...` |
| `ddkobj.py check` | IDENTICAL 1976/2035, DIFFERENT 32 (the head's 32), not-in-map 27, units linked 1966/2231, refusals 12 (externalized-static-register-convention 8, wrapper-redefines-globals 4), provisional kept VS2022 253 units + 6 functions, dual body 32 |
| against `log/orch/run54_headcheck/` | `BYTECHECK.tsv` equal row for row (2036 = 2036). No newly DIFFERENT unit to classify, no new fallback pair |
| match.py / relocheck | not run: no source bound |
| knob boot / menu walk | not run: no code change; the arm itself is the knob boot of this row and its self-tests are in section 0 |

## 4. Next (not started: step 1 only, one attempt)

1. **Bind 0x00458dd0** on the race-load path: the 100% source `re/match/s09/00458dd0.c` (C4 by match) fails only the C++
   compile in the PortTop TU (`C2664`, argument 1 of `Orig_004e8090` from `undefined4`). Binding row, data only (RULE 1).
   Owner: not in the run-54 ownership table; the lead assigns it. Then re-run this arm (`RO_ROLOADRWC-pr1`) for the next
   stop, or with `MASHED_OT_RL_STOPS=count` (stack `ROLOADC` form) to list every race-load stop on the core in one arm.
2. The [UNCERTAIN] line above: one logging arm settles the clump branch and 0x004b5190 on the core.
3. Harness note for the next race-load row on a fresh root: stage `original\mashed_image.bin` with
   `re/tools/image/extract_image.py` before the first launch (`[portstate-image] REFUSED(missing)` otherwise, and the arm
   faults in 0x0040d270 for a reason that is not the row's).

## Evidence

Committed (`verify/raceload_20261121/`): this RESULT, the PREREG, `raceload_log_extract.txt`, `fd_ref1_RO_ROLOADRWC-pr1.md`,
`raw_sha256_RACE-RWCORE-RACELOAD-ENGINE.txt`, `mapsym.py` (map resolver), stack `verify/savelink_20261023/stacks/ROLOADRWC.txt`,
one line in `re/orchestrator/UNCERTAIN_REGISTER.md`.

Raw (not committed, SHA-256 in `raw_sha256_RACE-RWCORE-RACELOAD-ENGINE.txt`): `D:/MashedWT/RACE-RWCORE-RACELOAD-ENGINE_data`
(`RO_ROLOADRWC-pr1/` meta.json `fd5e83b8...`, ticks.bin, ckpt*.data.bin; `RO_ROLOADRWC-pr1.log` `13436fc7...`;
`fd_ref1_RO_ROLOADRWC-pr1.out`; `arms.out`; `RUNLOG.txt`; the invalid launch `RO_ROLOADRWC-pr1_noimage*`), the build logs
`D:/MashedWT/RACE-RWCORE-RACELOAD-ENGINE_build_{vs,ddk}.log`, the root's `original/mashed_image.bin`.

Commands:
- Builds: `cmd /c "set MASHED_NO_DEPLOY=1&& mashedmod\build.bat"`, then with `set MASHED_DDKOBJ=1&&`; `py -3.12 re/tools/ddkobj/ddkobj.py check`.
- Image: `py -3.12 re/tools/image/extract_image.py --exe <main>/original/MASHED.exe.unpatched --out D:/MashedWT/RACE-RWCORE-RACELOAD-ENGINE_root/original/mashed_image.bin`
- Arm: `A4_OUT=D:/MashedWT/RACE-RWCORE-RACELOAD-ENGINE_data A4_ROOT=D:/MashedWT/RACE-RWCORE-RACELOAD-ENGINE_root A4_EXE=<wt>/mashedmod/build/ddkobj/mashed_re_ddk.exe A4_RO_CKPT=0,1,17,18,299,301,600 test-slot bash -c 'py -3.12 <wt>/re/orchestrator/mashed_lock.py run --label RACE-RWCORE-RACELOAD-ENGINE -- py -3.12 verify/savelink_20261023/run_arms.py RO_ROLOADRWC-pr1'`
- First divergence: `py -3.12 re/frida/detorig_firstdiv.py D:/MashedWT/RO-REF-NAME_data/ref1 D:/MashedWT/RACE-RWCORE-RACELOAD-ENGINE_data/RO_ROLOADRWC-pr1 --masks re/frida/warp_normalize.tsv,re/frida/replay_masks.tsv --ignore 0x00614400-0x00614800,0x00771800-0x00771c00 --exe-map mashedmod/build/ddkobj/mashed_re_ddk.map`
- Map: `py -3.12 verify/raceload_20261121/mapsym.py mashedmod/build/ddkobj/mashed_re_ddk.map 0x3E427`
