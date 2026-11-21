# RESULT PORT-OT-RACE-BIND-PROV4 (run 53, 2026-11-21)

Branch `orch/PORT-OT-RACE-BIND-PROV4` from `orch/integration` `b24bba46c` (still the integration head at the end of the
row, so no rebase). Row kind: top-down / race-path. Worker claude2, Opus 5.5 medium, lead cmwpwu9qr5v1xnz1c1zq4o204.
PREREG `verify/ro_20261116/PREREG_PORT-OT-RACE-BIND-PROV4.md`, committed alone at `124759e23`, before any arm.
No G-CTRL, no B, no 12x4: not run, top-down row. No tracker edit, no promotion, no default change, nothing enqueued,
merged or pushed. One attempt.

## 0. Verdict: BLOCKED at 0x0047b860 (the next named stop, inside 0x0047a020's call to 0x0047b9b0). Load length not measured

| DONE item | result |
|---|---|
| the four provisional sources bound in place of their stops | yes: 0x0047a020, 0x00479330, 0x00478660, 0x00462950 are `bind` (kind `provisional`) in `rltools/plan_rl.json`, one ORIGTOP-only TU each (`Match/PortTop_<rva>.cpp`) |
| the course load gets past 0x0047a020 | **the 0x0047a020 stop is gone**: the body runs and reaches its callee 0x0047b9b0 (source call at orig 0x0047a0ca, `PROVISIONAL.tsv` row), whose callee 0x0047b860 is the first stop reached. Stops-exit arm exit code 4700256 = `0x0047B860` (checkpoint 3: `0x0047A020`). In count mode the load continues past 0x0047a020 into 0x00479330 (its callee 0x00478200 runs) |
| load length under `MASHED_ORIG_TOP` against ref1's 108 | **not measured**: no knob-on arm reaches the race anchor (`race.reached` false, `race.n` 0 in both meta.json). Knob-off control: 1. ref1: 108. Checkpoint 1: 3 |
| first divergence against ref1 | unchanged on all three arms: **record 0 (tick 1), `0x00604604` ref `0x00000004` exe `0x00000000`**, 118 counted blocks / 76 raw words, IDENTICAL-FOR-N 17 (of 600 / 300 / 283) |
| self-test | knob-off arm `[REPLAY]` PASS 0/666, `[TRACE]` PASS 0/33294, `[OT_TIMER]` PASS 0/5990; stops-exit arm `[OT_TIMER]` PASS 0/2990; every arm `[portstate-image]` PASS 0/8, `[OT_CODEPTR]` PASS 0/367; knob boot the same two plus `WinMain 0x00492370 returned 0 after 3000 frames` |

Unfinished scope: the load length (target above 3). It needs the stops after 0x0047a020 in call order, section 5. Those
rvas are not owned by this row.

## 1. Change (knob `MASHED_OT_RACELOAD=1`, default OFF, exe only, ORIG_TOP only)

- `verify/ro_20261116/rltools/rl_bind_gen.py`:
  - `PROV_BIND` (the audio generator's set, read by `msrc()`) extended in place with the four rvas, so they bind their
    `re/match/PROVISIONAL.tsv` sources (`s12/0047a020.c` 92.2%, `s12/00479330.c` 97.2%, `s12/00478660.c` 79.1%,
    `s10/00462950.c` 88.8%, all NOT-PROVEN, RACE-COURSELUA-BODIES). L-126: provisional, knob path only, never C4. No new
    PROVISIONAL.tsv row and no admission (the four rows already exist).
  - `NOT_LOCAL += (0x00478660, 0x004785e0)`: `re/match/s12/00478660.c` lines 55-77 hold a HELD static copy of 0x004785e0
    under `#ifndef MATCH_PORT`; on the port it is external, and 0x004785e0 (C2, no matched source) is a named stop.
  - `FORCE_STOP 0x004790e0`: a register-arg callee. Disassembly of the anchored `original/MASHED.exe.unpatched`:
    0x00479e28 `mov eax, dword ptr [ebx + 0x105d4]`, 0x00479e2e `call 0x4790e0`; 0x004790e0 `push 0`, 0x004790e2
    `push 0x479030`, 0x004790e7 `push eax` (EAX read with no write before it), 0x004790e8 `call 0x4e5c70`. The port
    declaration in `00479330.c` is `void FUN_004790e0(void)` and the source defines it `static`, so no bound body
    receives EAX: the rva stays a named stop (first build: LNK2019 on `_PortTop_Bound_004790e0`).
  - `NAME_DEF 0x004e5c30 -> RpWorldForAllAtomics` (`re/match/rwl02/004e5c30.c` line 34 names the function so; first
    build: LNK2019 on `_PortTop_Bound_004e5c30`).
  - `EXTRA_DEF 0x0042a740: _strchr -> MashedCrt_strchr_004a2fd0` (the exe's binding of the original CRT strchr
    0x004a2fd0, `Match/PortTop_CrtString.cpp`, BIND-CRT-STRING; first build: LNK2019 on `__strchr`).
- `rltools/exeidx_drop_rl.py` (new): drops this generator's own wrappers from `exeidx.json` before planning. Without it
  `exe_body_index.py` sees checkpoint 2/3's `PortTop_<rva>.cpp` as `match-include` bodies, plans them as `reuse`, and
  `--write` deletes them. Checkpoint 3's plan is reproduced exactly with it (dry-run diff: additions only).
- `rltools/cpp_unclean.txt` +1: 0x00451730 (`C2664 'float *RwCore_004c1740(float *,float *,int)': argument 1 from
  'undefined4'`), a named stop.
- Plan `00426e10,004264d0`: **139 bind (135 match, 4 provisional), 75 reuse, 46 stops** (checkpoint 3: 92 / 45 / 33).
  New binds: the 4 provisional + 43 matched (C4 by match) closure rvas, listed in `rltools/provbind4_newbinds.txt`
  (49 lines, two of which, 0x00451730 and 0x004790e0, became stops as above).
- Ownership (L0, `log/orch/run53_bindlists/`): all 49 planned new binds are in `race_owned.txt`; none is in
  `audio_owned.txt` or `bulk_cp1.tsv`. 0x00458c70 (RACE-RWGLOBALS-FRAMELIST) stays the counting stand-in (ALIAS,
  unchanged). Existing audio bodies 0x005a6710 / 0x005a6dc0 / 0x005a7aa0 / 0x005a7af0 are reused by name, not bound.
- `mashedmod/exe_sources.rsp` +47 TUs, `re/tools/dual_copy_allowlist.txt` +10 `ORIGTOP-ONLY` lines (`rl_sync.py`).
  rva-lint 391 known, NEW 0. `Match/PortTop_RaceLoadStops.cpp` regenerated (46 stops), `Match/PortTop_00426e10.cpp`
  regenerated (its four callees now `PortTop_Bound_<rva>`).

## 2. Builds and checks

Lock / slot status printed before each build and arm run: `mashed_lock.py status` = `free 'mashed_machine'` every time;
`test-slot --status` memory lines 25.0 GB (first build, printed while it ran: the slot rule arrived after it started;
only this row's build.bat was running, checked by command line), 24.8, 24.0, 23.9 GB before the later builds and the
check, 24.4 GB before the arms. The knob boot and the arms waited about 10 min each on the test-slot memory gate.

| check | result |
|---|---|
| VS2022 build (`MASHED_NO_DEPLOY=1`) | `=== Build OK ===` on the third attempt (1: C2664 in 0x00451730; 2: four LNK2019, section 1). `mashed_re.exe` SHA-256 `ab39e1d6...` |
| object build (`MASHED_DDKOBJ=1`) | `=== Build OK ===`, `mashed_re_ddk.exe` SHA-256 `70aa9304...` |
| `ddkobj.py check` | IDENTICAL 1803/1855, DIFFERENT 28, not-in-map 24, units linked 1802/2038, refusals 11 (externalized-static-register-convention 7, wrapper-redefines-globals 4), provisional kept VS2022 225 units + 5 fns (integration head per PORT-TOP-AUDIO-5AEF30-KNOB: 221 + 5; the +4 are this row's provisional units) |
| newly DIFFERENT units | **none.** The 28 DIFFERENT = the base's 25 (`D:/MashedWT/PORT-TOP-AUDIO-5AEF30-KNOB_data/bytecheck0_base.tsv`, fresh check of 0917bc3d0) + 0x005ae3a0, 0x005bbdb0, 0x005bce80, all classified in `RESULT_PORT-TOP-AUDIO-5AEF30-KNOB.md` (the integration head). `comm` of the two lists: no rva on one side only. None of this row's binds is DIFFERENT |
| this row's matched binds in `BYTECHECK.tsv` | 43 IDENTICAL; 5 `not-in-map` (0x00479030, 0x00543bb0, 0x00543da0, 0x00543dc0, 0x00543e30: no symbol in `mashed_re_ddk.map`, so no linked DDK body to compare). [UNCERTAIN] why the linker drops them: 0x00479030 is passed by address only from 0x004790e0 (now a stop, 0x004790e2), the 0x00543xxx reason is not checked; test: `/VERBOSE:REF` link of the object build. Rows: `provbind4_bytecheck_rows.txt` |
| the four provisional units | not in `BYTECHECK.tsv`: kept on their VS2022 bodies by `ddkobj.py load_provisional` (prediction 6) |
| match.py / relocheck on the sources the exe includes | not re-run: the 43 matched sources carry `c4=match` (BYTE-IDENTICAL + relocheck OK) in `hooks.csv`, and the object build byte-checks every linked one (43 IDENTICAL); the four provisional sources are unchanged from their `PROVISIONAL.tsv` rows (no source edit in this row) |
| knob boot (VS2022 exe, `MASHED_ORIG_TOP=1 MASHED_OT_RACELOAD=1 MASHED_DET_FRAMES=3000`) | `knob_rl_on: exited rc=0 (0x00000000) after 37.8 s`, `WinMain 0x00492370 returned 0 after 3000 frames` |
| menu walk | not run: no menu code changed |

## 3. Arms (object build, root `D:/MashedWT/PORT-OT-RACE-BIND-PROV4_root` = robocopy of `PORT-OT-RACE_root`, data `D:/MashedWT/PORT-OT-RACE-BIND-PROV4_data`)

`A4_RO_CKPT=0,1,17,18,299,301,600`, replay json unchanged (no diff on the branch).

| | ref1 | `RO_ROIMG-pr1` (knob off) | `RO_ROLOAD-pr1` (stops exit) | `RO_ROLOADC-pr1` (stops count) |
|---|---|---|---|---|
| exit | | 0, 15 s | 4700256 (`0x0047B860`), 5 s | 3221225477 (`0xC0000005`), 5 s |
| ticks recorded (meta) | | 600 | 300 | 300 |
| `race.dn_warp` (load length) | 108 | 1 | not reached | not reached |
| first divergent record vs ref1 | | 0, `0x00604604` | 0, `0x00604604` | 0, `0x00604604` |
| detorig_ticks verdict | | IDENTICAL-FOR-N 17 of 600 | 17 of 300 | 17 of 283 |

Knob-on log (`provbind4_log_extract.txt`):
- Exit arm: `PortTop_Bound_00426e10(track 30)` entered, COURSE.LUA found (0x00550b00 `("COURSE.LUA")` = 1), then
  `rva 0x0047B860: race-load stop ... callers 0x0047b9b0, return 0004B416, exits` and `[OT_TIMER] STOP ... 0x0047B860`.
  `0x0004B416` = `_PortTop_Bound_0047b9b0+0x6` in `mashed_re_ddk.map` (symbol at `0x0004b410`, DDK object
  `ddk_s13_0047b9b0`). The 0x0047a020 stop line of checkpoints 2 / 3 does not appear.
- Count arm, stops in order: 0x0047b860, 0x00440bc0 (callers 0x0047a020 / 0x0047a0f0 / 0x0047a130), 0x0047b8d0,
  0x004b7480, then 0x00478200 (a bound callee of 0x00479330) searches `.bsp` with an empty name in pc / xbox / ps2 /
  gamecube (`cfglog: Unable to find .bsp in piz`) and calls 0x004a332b (`CrtExitNoReturn_j5`, a stop; return
  `0x00051509` = `_PortTop_Bound_00478200+0x19`), which returns 0 in count mode, then the fault. [UNCERTAIN] the empty
  name: the COURSE.LUA handlers (0x00440bc0 registration, 0x0047b860 / 0x0047b8d0) are count stops that return 0, so the
  script sets no track name; not measured, test = the same arm after those three bind (section 5). The fault after a
  count-mode exit stop is a count-mode consequence, not a finding about a bound body (checkpoint 2 / 3 rule).
- 0x00478660 and 0x00462950 are not reached on either knob-on arm (no stop or entry line beyond 0x00478200).

## 4. Prediction check (PREREG)

1 holds. 2 holds (0x0047a020 line gone, later stop 0x0047b860). 3: not reached, so the reached stop names the next body.
4 holds. 5 holds for every self-test printed. 6 holds (no newly DIFFERENT unit, four provisional units kept VS2022).

## 5. Next (not this row's rvas; all in `race_owned.txt`)

1. 0x0047b860 (C4 by match, cpp-unclean: `C2660 'PortTop_RLStop_004b7330': takes no 1 argument`), then 0x00440bc0
   (cpp-unclean C2632 `char` followed by `bool`), 0x0047b8d0 (cpp-unclean C2664 on `Orig_004b6520`), 0x004b7480 (C2):
   the COURSE.LUA handler chain under 0x0047a020 / 0x0047b9b0. Making the three cpp-unclean sources compile under
   `MATCH_PORT` (DDK bytes unchanged, match.py check) is checkpoint 2's next item 2.
2. 0x004790e0: an exe body that takes the world in EAX (a register-arg adapter), then 0x00479030 links.
3. Then the load length becomes measurable on the exit arm.

## 6. Evidence

Committed (`verify/ro_20261116/`): this RESULT, the PREREG, `provbind4_log_extract.txt`, `provbind4_fd_ref1_roload.md`,
`provbind4_fd_ref1_roloadc.md` (firstdiv reports), `provbind4_bytecheck_rows.txt`, `raw_sha256_PORT-OT-RACE-BIND-PROV4.txt`
(SHA-256 of every raw file cited and of both exes), `rltools/provbind4_newbinds.txt`, the generator edits,
`rltools/exeidx_drop_rl.py`.

Raw (not committed): `D:/MashedWT/PORT-OT-RACE-BIND-PROV4_data` (three arm dirs, arm logs, `knob_rl_on.log`,
`runs.log`, `fd_ref1_<arm>.out`), `D:/MashedWT/PORT-OT-RACE-BIND-PROV4_build_vs.log`, `_build_ddk.log`, `_ddkcheck.log`.

Commands:
- Index + plan: `py -3.12 verify/portorigtop_20261113/tools/exe_body_index.py . verify/ro_20261116/rltools/exeidx.json`,
  `py -3.12 verify/ro_20261116/rltools/exeidx_drop_rl.py . verify/ro_20261116/rltools/exeidx.json`,
  `py -3.12 verify/ro_20261116/rltools/rl_bind_gen.py . 00426e10,004264d0 --write`, `py -3.12 verify/ro_20261116/rltools/rl_sync.py`.
- Builds: `cmd /c "set MASHED_NO_DEPLOY=1&& mashedmod\build.bat"`, the same with `set MASHED_DDKOBJ=1&&`;
  `py -3.12 re/tools/ddkobj/ddkobj.py check`.
- Knob boot: `PORTTOP_OUT=D:/MashedWT/PORT-OT-RACE-BIND-PROV4_data test-slot bash -c 'py -3.12 <orch-wt>/re/orchestrator/mashed_lock.py run --label PORT-OT-RACE-BIND-PROV4 -- py -3.12 verify/porttop_20261108/knob_boot.py knob_rl_on D:/MashedWT/PORT-OT-RACE-BIND-PROV4_root 300 MASHED_ORIG_TOP=1 MASHED_OT_RACELOAD=1 MASHED_DET_FRAMES=3000'`
- Arms: `A4_OUT=D:/MashedWT/PORT-OT-RACE-BIND-PROV4_data A4_ROOT=D:/MashedWT/PORT-OT-RACE-BIND-PROV4_root A4_EXE=<wt>/mashedmod/build/ddkobj/mashed_re_ddk.exe A4_RO_CKPT=0,1,17,18,299,301,600 test-slot bash -c 'py -3.12 <orch-wt>/re/orchestrator/mashed_lock.py run --label PORT-OT-RACE-BIND-PROV4 -- py -3.12 verify/savelink_20261023/run_arms.py RO_ROIMG-pr1 RO_ROLOAD-pr1 RO_ROLOADC-pr1'`
- First divergence: as RESULT_PORT-OT-RACE section 4, per arm, into `D:/MashedWT/PORT-OT-RACE-BIND-PROV4_data/fd_ref1_<arm>.out`.
- Disassembly of 0x00479e20 / 0x004790e0: capstone over `original/MASHED.exe.unpatched` (scratch script, output quoted in section 1).
