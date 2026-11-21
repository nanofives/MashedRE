# RESULT RACE-RWGLOBALS-FRAMELIST (run 53, 2026-11-21)

Branch `orch/RACE-RWGLOBALS-FRAMELIST`, rebased onto `orch/integration` `a9e5b4c8a` (PORT-OT-RACE-BIND-PROV4 and BIND-TOOL
merged) before the last builds and arms. Row kind: top-down / race path. Worker claude2, Opus 5.5, lead
cmwpwu9qr5v1xnz1c1zq4o204. PREREG `verify/ro_20261116/PREREG_RACE-RWGLOBALS-FRAMELIST.md`, committed alone before any build
(`1dd09ce13` after the rebase), amendment 1 committed alone after the two faulting knob boots and before any arm (`172a39ab4`).
No G-CTRL, no B, no 12x4: not run, top-down row. No tracker edit, no promotion, no default change. Nothing enqueued,
merged or pushed. One attempt.

## 0. Verdict: GREEN for the binding (0x00458c70 runs on the RW core frame list); load length not measured

| DONE item | result |
|---|---|
| the RW core frame list exists on the record the race load uses | yes: under `MASHED_OT_RACELOAD=1` the matched RW core frame-module open 0x004c07b0 runs once on the librw RwGlobals record (offset 0x164). Log on every knob-on run: `-> record; +0xbc = +0xc0 = record + 0xbc, +0x164 007D3E78, DAT_007d3e70 00000164, DAT_007d3e74 00000001` |
| 0x00458c70 bound | yes: `PortTop_Bound_00458c70` (the matched source) runs on both knob-on arms: `entered, [+0xbc] = record + 0xbc`, `returned, [+0xbc] = 112427A0` (25 frame roots linked), then 0x004264d0 returns and 0x00426e10 is entered. No fault (checkpoint 3: `0xC0000005` at RwCore_004c15c0+0x85) |
| load length under `MASHED_ORIG_TOP` against ref1's 108 (target above 3) | **not measured**: neither knob-on arm reaches the race anchor (`race.reached` false, `race.n` 0 in both meta.json). Stops-exit arm ends at the 0x0047B860 stop (exit 4700256), as on PORT-OT-RACE-BIND-PROV4. Knob-off control: 1. ref1: 108 |
| first divergence against ref1 | unchanged on all three arms: **record 0 (tick 1), `0x00604604` ref `0x00000004` exe `0x00000000`**, 118 counted blocks / 76 raw words, IDENTICAL-FOR-N 17. VALUE dwords at tick 1: 5275 knob off, **5269** knob on (section 4) |
| self-test | knob-off arm `[REPLAY]` PASS 0/666, `[TRACE]` PASS 0/33294, `[OT_TIMER]` PASS 0/5990; stops-exit arm `[OT_TIMER]` PASS 0/2990; every arm `[portstate-image]` PASS 0/8, `[OT_CODEPTR]` PASS 0/367; knob boot (VS2022) `WinMain 0x00492370 returned 0 after 3000 frames`, rc 0 |
| object build | 0x00458c70 and both copies of 0x004c07b0 IDENTICAL; DIFFERENT 28, the same 28 as the integration head's check: no newly DIFFERENT unit |

Unfinished scope: the load length (target above 3). It is blocked behind the course-load stops after 0x0047a020
(first: 0x0047b860, a callee of 0x0047b9b0), which are not this row's RVAs. The audio lane's `[RwGlobals+0xf4]`: not
changed by this row. +0xf4 is still set only by `MASHED_ORIG_TOP_RWMOD=1` (`RegisterRwEarlySlots`). The frame list does
not touch it, so audio arms keep that knob (section 6).

## 1. Evidence the change rests on

- Fault (RESULT_PORT-OT-RACE.md 7.2): `RwCore_004c15c0` writes `[[RwGlobals+0xbc]+4]` (original 0x004c1645). On the ORIG_TOP arm
  `[0x007d3ff8]` is the librw record (`LibRw/RwApiBind.cpp` `MashedRw_InstallRwGlobals`), +0xbc = 0.
- Writer of +0xbc / +0xc0 in the RW core: `re/match/s19/004c07b0.c` (0x004c07b0, C4 c4=match): `DAT_007d3e70 = offset`,
  `[RwGlobals + offset] = FUN_004cc9e0(DAT_00617f78, DAT_00617f90, 4, DAT_00617f94, &DAT_007d3e78, 0x4000e)`, then
  `[RwGlobals+0xbc] = [RwGlobals+0xc0] = RwGlobals + 0xbc`, `DAT_007d3e74++`.
- The original's record (original-side probe `verify/portorigtop_20261109/ORIG_RWG.json`, RwGlobals `0x02b499a8`, 33.39 s):
  +0xbc = +0xc0 = `0x02b49a64` = RwGlobals + 0xbc, +0x164 = `0x007d3e78`. So the frame module's offset is 0x164.
  ref1 at tick 1 agrees: `0x007d3e70` = 0x164, `0x007d3e74` = 1 (section 4).
- This also answers register line PORT-OT-RACE-RWCORE-FRAMELIST: the matched 0x004c07b0 is the +0xbc writer, and the
  original's +0xbc is a self-linked head (probe). Measured: with that head, 0x00458c70 runs.

## 2. Change (knob `MASHED_OT_RACELOAD=1`, default OFF, exe only, ORIG_TOP only, no new librw binding)

- `LibRw/RwApiBind.cpp`: `RaceLoadFrameOpen()` runs once, at the first install of the librw record at 0x007d3ff8, under
  `MASHED_OT_RACELOAD=1`. It calls `PortTop_RL_FrameOpenSeh(record)` and logs the record words before and after.
- `Match/PortTop_StandInsRL.cpp`:
  - `PortTop_RL_FrameOpenSeh` = `PortTop_Bound_004c07b0(record, 0x164, 4)` under an SEH filter that logs a fault and
    passes it on (RwApiBind.cpp has no windows.h).
  - `PortTop_RL_00458c70`: when `[[0x007d3ff8]+0xbc]` = 0 it keeps the counting stand-in and logs it, else it logs and
    calls `PortTop_Bound_00458c70`.
- `Match/PortTop_004c07b0.cpp` (new): `re/match/s19/004c07b0.c` as `PortTop_Bound_004c07b0`, callee 0x004cc9e0 =
  `Orig_004cc9e0` (`Match/PortTop_004cc9e0.cpp`), which jumps to the exe's freelist-create binding
  `PortTop_RwFreeListCreate_004cc820` (`Match/PortTop_004cc820.cpp`, HUDINIT-A, L0 ruling 2026-11-10). Why not
  `RwCore_004c07b0` (amendment 1):
  - The first two knob boots faulted inside it: `knob_rl_on2` logged a write to `0x00000004` at exe+0x1F2F0C, eax 0,
    ecx `0x007D45CC`, edi `0x007D3E78`. That is the tail of `RwCore_004cc820`, which links the new freelist into the RW core
    list at 0x007d45cc (`re/match/rwl01/004cc820.c`, `*(DAT_007d45cc + 4) = param_5 + 7`). That head is 0 on this path:
    its writer, the freelist-module open 0x004cce20, does not run here.
  - `RwCore_004cc820` is a 74.5% source listed provisional only for `MASHED_RWC_CORE`
    (`verify/rwccorerun_20261111/PROVISIONAL_RWC.tsv`), so it may not run on this knob path.
  - 0x004ccde0, the list walker, is in `bulk_cp1.tsv` (another row's RVA): not touched.
  - The binding's declared deviations: no block preallocated, header +0x1c / +0x20 not linked into 0x007d45cc.
  `RwCore/RwCore_004c07b0.cpp` stays the `MASHED_RWC_CORE` copy.
- `Match/PortTop_00458c70.cpp` (new, hand-written): `re/match/s09/00458c70.c` as `PortTop_Bound_00458c70`, callee 0x004c15c0 =
  `RwCore_004c15c0` (RW core body, L-124).
- Callers: `Match/PortTop_0040d270.cpp` and `Match/PortTop_004264d0.cpp` define `FUN_00458c70` as `PortTop_RL_00458c70`.
  `verify/ro_20261116/rltools/rl_bind_gen.py`: `ALIAS['00458c70'] = PortTop_RL_00458c70`, so a regeneration writes the same
  define. The generator never removes the hand-written TU (its header is not the generator's).
- `exe_sources.rsp` +2 TUs. `re/tools/dual_copy_allowlist.txt` +1 line `ORIGTOP-ONLY DUP-IN-TARGET:004c07b0`: the RwCore copy
  carries the marker, the PortTop copy does not, as with 0x004cc9e0. rva-lint NEW 0.
- Ownership: 0x004c07b0, 0x004cc9e0, 0x004cc820 are in none of `race_owned.txt`, `audio_owned.txt`, `bulk_cp1.tsv`.
  0x00458c70 and 0x004c15c0 are in `race_owned.txt`. No body of another row's RVA is bound or changed (0x004cc9e0 and
  0x004cc820 are called through their existing exe bodies, unchanged).

## 3. Builds and checks

| check | result |
|---|---|
| VS2022 build (`MASHED_NO_DEPLOY=1`), final tree `b945ce6c2` | `=== Build OK ===`, rva-lint 392 findings, allowlisted 392, NEW 0. `mashed_re.exe` SHA-256 `f21a686d...` |
| object build (`MASHED_DDKOBJ=1`) | `=== Build OK ===`, `mashed_re_ddk.exe` SHA-256 `665b7044...` |
| `ddkobj.py check` | IDENTICAL 1805/1857, DIFFERENT 28, not-in-map 24, units linked 1804/2040, refusals 11 (externalized-static-register-convention 7, wrapper-redefines-globals 4), provisional kept VS2022 225 units + 5 fns |
| newly DIFFERENT units | **none**. Base: the check of the tree merged into the integration head as `983b5e810` (`D:/MashedWT/PORT-OT-RACE-BIND-PROV4_ddkcheck.log`: IDENTICAL 1803/1855, DIFFERENT 28). `git diff 983b5e810^2 a9e5b4c8a -- mashedmod re/match re/tools/ddkobj` is empty (CENSUS-ROUTE-TABLE `12998c278` and BIND-TOOL `a110f6c80` change no build input), so that check is of the integration head's build inputs. The two DIFFERENT sets are equal (28 = 28, none new, none gone). +2 units, both IDENTICAL: `_PortTop_Bound_00458c70` 144 B, `_PortTop_Bound_004c07b0` 128 B (`framelist_bytecheck_rows.txt`) |
| match.py `--bytes` / relocheck on the included sources | `re/match/s09/00458c70.c` 100.0%, 136 B BYTE-IDENTICAL, relocheck OK checked=4; `re/match/s19/004c07b0.c` 100.0%, 124 B BYTE-IDENTICAL, relocheck OK checked=13 (`framelist_matchpy.txt`) |
| knob boot (VS2022, `MASHED_ORIG_TOP=1 MASHED_OT_RACELOAD=1 MASHED_DET_FRAMES=3000`, final exe) | `knob_rl_final: exited rc=0 after 30.3 s`, frame open line as predicted, `WinMain 0x00492370 returned 0 after 3000 frames` |
| menu walk | not run: no menu code changed |

Earlier builds and knob boots on the way (all in `D:/MashedWT/RACE-RWGLOBALS-FRAMELIST_data`): `knob_rl_on` (no SEH log) and
`knob_rl_on2` (SEH log, above) faulted inside the frame open with `RwCore_004c07b0`. `knob_rl_on3` (binding route,
pre-rebase) passed. One build attempt stopped at rva-lint (`ORIGTOP-ONLY` marker on both copies, fixed as in section 2).

**Rule break, stated:** the build at 02:53:57 (`_build_vs2.log`, the SEH log build) started while the merge train
`20261121_022308` held PRIORITY-HOLD and SOLO on `mashed_machine`: my wait loop checked only for other builds, not the
lock. It finished `Build OK` in a few minutes. Every later build and run waited for no PRIORITY / SOLO. Lock and slot
status lines: `D:/MashedWT/RACE-RWGLOBALS-FRAMELIST_status_{vs,ddk,boot,final}.txt`. Final builds: `HELD 'mashed_machine'
label=PROV-INSITU-REF1` (a game run, no hold), memory 24.2 / 24.0 GB committed of 27.4 GB (below the 26 GB hard hold).
Arms: `HELD ... label=RACE-WRITER-0067EA74`, then my lock.

## 4. Arms (object build `665b7044`, root `D:/MashedWT/RACE-RWGLOBALS-FRAMELIST_root` = robocopy of `PORT-OT-RACE_root`)

`A4_RO_CKPT=0,1,17,18,299,301,600`, replay json unchanged.

| | ref1 | `RO_ROIMG-pr1` (knob off) | `RO_ROLOAD-pr1` (stops exit) | `RO_ROLOADC-pr1` (stops count) |
|---|---|---|---|---|
| exit | | 0, 13 s | 4700256 (`0x0047B860`), 5 s | `0xC0000005`, 5 s |
| ticks recorded | | 600 | 300 | 300 (283 compared) |
| `race.dn_warp` (load length) | 108 | 1 | not reached | not reached |
| 0x00458c70 | | not reached | bound body, returned | bound body, returned |
| first divergent record vs ref1 | | 0, `0x00604604` | 0, `0x00604604` | 0, `0x00604604` |
| VALUE dwords at tick 1 | | 5275 | 5269 | 5269 |
| detorig_ticks verdict | | IDENTICAL-FOR-N 17 of 600 | 17 of 300 | 17 of 283 |

Count arm, stops in order after 0x00426e10: 0x0047b860, 0x00440bc0, 0x0047b8d0, 0x004b7480, 0x004a332b, then
`Unable to find .bsp in piz` and the fault. That is the PORT-OT-RACE-BIND-PROV4 count-mode sequence (empty .bsp name, its
register line). [UNCERTAIN] The faulting instruction of the count arm is not captured. It comes after count stops that
return 0 and is not attributed to this row's change.

Frame-module words at tick 1 (`portotrace_words.py`, ref1 `ckpt201` against `ckpt01`):

| va | ref1 | knob off | knob on |
|---|---|---|---|
| `0x007d3e70` (globalsOffset) | 0x164 | 0 | 0x164 (same) |
| `0x007d3e74` (numInstances) | 1 | 0 | 1 (same) |
| `0x007d3e78` header +0x0 (entry size) | 0xb4 | 0 | 0xa4 (register line: which writer raises DAT_00617f78 on the original) |
| `0x007d3e7c` / `0x007d3e80` / `0x007d3e84` | 0x32 / 7 / 4 | 0 | the same as ref |
| `0x007d3e88` / `0x007d3e8c` (block list) | heap `0x0f2ae950` / `0x0482c6c8` | 0 | `0x007d3e88` / `0x007d3e88` (no prealloc: declared) |
| `0x007d3e90` (+0x18 flags) | 3 | 0 | 3 (same) |
| `0x007d3e94` / `0x007d3e98` (+0x1c / +0x20) | `0x007d4020` / `0x007d45bc` | 0 | 0 / 0 (not linked into 0x007d45cc: declared) |

Those six words that now match are the 6 VALUE dwords fewer at tick 1 (5275 -> 5269).

## 5. Known limit (register lines)

- The 25 frames 0x004c15c0 marks are librw frames. They sit in the record's RW core dirty list, which no librw sync walks
  (`UNCERTAIN_REGISTER.md`, "RACE-RWGLOBALS-FRAMELIST (librw frames in the core list)"). Not measured: no arm reaches a race
  frame.
- Entry size 0xb4 on ref1 against 0xa4 (file-backed `DAT_00617f78`): the original writer is not measured (register line
  "frame entry size"). No reader of the frame freelist runs on this path (frame creates are librw bindings).

## 6. Next

1. Load length: the course-load stops after 0x0047a020 in call order (0x0047b860 first, then 0x00440bc0 / 0x0047b8d0 /
   0x004b7480 / 0x004a332b on the count arm). Not this row's RVAs.
2. The audio lane's `[RwGlobals+0xf4]` stays on `MASHED_ORIG_TOP_RWMOD=1`. This row puts only the frame module on the
   record. Making +0xf4 independent of RWMOD is a separate decision (the string slots are librw-record fields, not RW core
   module state).
3. RW core list 0x007d45cc: initialising it needs 0x004cce20 (C4 match), which also installs RwGlobals +0x108..+0x114 and
   the freelist of freelists. That would let 0x004ccde0 run, but 0x004ccde0 is another row's RVA (bulk_cp1).

## 7. Record layout and who owns +0xbc / +0xc0 / +0x164 (added after review, L0 condition)

The record at 0x007d3ff8 on this path is not a librw object. It is the port's own `MashedRwGlobals s_rwGlobals`
(`mashedmod/src/mashed_re/LibRw/RwApiBind.cpp:1782`..`:1807`, size 0x440, `static_assert` at `:1806`). librw's engine keeps
its state in its own globals and never reads this record. The record's fields:

| range | field (source line) | written by |
|---|---|---|
| +0x00..+0x33 | `dev[0x34]` (`:1783`) | `memcpy` from the B15 device (`:2134`) |
| +0x34..+0xcb | `pad34` (`:1784`), no named field | `Put(0x34)`, `Put(0x60)` under RWMOD (`:2001`, `:2002`). Nothing else |
| +0xcc / +0xd4 | `rwstrcpy` / `rwstrcat` (`:1785`..) | the installer, every install |
| +0xc4, +0xd0, +0xdc, +0xe8, +0xf4, +0xf8, +0xfc | string slots | `Put` under RWMOD (`:1977`..`:1983`) |
| +0x108..+0x11c | memory and freelist functions | the installer; +0x118 / +0x11c under RWMOD |
| +0x120..+0x43f | `mod[]` (`:1797`) | `Put(kMod... )` under RWMOD: vector +0x134..+0x148, matrix +0x14c..+0x160, image +0x388 / +0x38c, texture +0x3f4..+0x410 (`:2006`..`:2037`) |

- **+0xbc and +0xc0** lie in `pad34`. No binding code writes them: the only `Put` offsets inside `pad34` are 0x34 and 0x60.
  Owner: the RW core frame module (0x004c07b0 writes them).
- **+0x164** = matrix module + 0x18, inside `mod[]`. `RegisterRwModules` leaves it 0 on purpose (`:2014`: "+0x18 0x007d3e78 / +0x1c
  0x007d45a0 / +0x20 0x007d3e9c stay 0: no reader of them on the knob path found"). The original holds 0x007d3e78 there
  (`ORIG_RWG.json`), the frame freelist 0x004c07b0 stores. Owner: the RW core frame module.
- Readers, by source search: of `0x007d3ff8` + 0xbc / + 0xc0 / + 0x164 in `mashedmod/src` outside `RwCore/`, only this row's log
  lines and one comment (`Gameplay/PickupPoolSpawn.cpp:91`). Of `DAT_007d3ff8 + 0xbc`, `+ 0xc0` or `+ DAT_007d3e70` in `re/match`,
  only RW core sources (re/match/s19, rwl01, rwl03 and 004d80d0): 004c07b0, 004c0830, 004c0870, 004c0910, 004c0b30, 004c0c20, 004c0d70, 004c0de0, 004c0e50,
  004c0f00, 004c1040, 004c1210, 004c1340, 004c13e0, 004c1480, 004c1520, 004c15c0, 004c1680, 004d80d0, 00533ec0.
- [UNCERTAIN] A text search cannot exclude a reader that reaches these offsets through a computed pointer. Test (register line
  FRAMELIST-LIBRW-RECORD-OVERLAP): read back the whole record before and after the open, and again at the end of a knob-on arm,
  and diff the words other than +0xbc / +0xc0 / +0x164.

## Evidence

Committed (`verify/ro_20261116/`): this RESULT, the PREREG with amendment 1, `framelist_log_extract.txt` (frame-open,
0x00458c70, stop and self-test lines of the knob boots and the three arms), `framelist_fd_ref1.md` (head of the three
firstdiv reports), `framelist_bytecheck_rows.txt`, `framelist_matchpy.txt`, `raw_sha256_RACE-RWGLOBALS-FRAMELIST.txt` (SHA-256 of
both exes, BYTECHECK.tsv, build / check logs, the base check log, every arm log, meta.json, ticks.bin, the tick-1
checkpoints and the firstdiv outputs).

Raw (not committed): `D:/MashedWT/RACE-RWGLOBALS-FRAMELIST_data` (arm dirs, logs, knob boot logs, `fd_ref1_<arm>.out`),
`D:/MashedWT/RACE-RWGLOBALS-FRAMELIST_build_{vs,vs2,vs3,ddk}*.log`, `_build_vs_final.log`, `_build_ddk_final.log`,
`_ddkcheck_final.log`, `_status_*.txt`.

Commands:
- Builds: `cmd /c "set MASHED_NO_DEPLOY=1&& mashedmod\build.bat"`, then with `set MASHED_DDKOBJ=1&&`; `py -3.12 re/tools/ddkobj/ddkobj.py check`.
- Knob boot: `PORTTOP_OUT=D:/MashedWT/RACE-RWGLOBALS-FRAMELIST_data test-slot bash -c 'py -3.12 <orch-wt>/re/orchestrator/mashed_lock.py run --label RACE-RWGLOBALS-FRAMELIST -- py -3.12 verify/porttop_20261108/knob_boot.py knob_rl_final D:/MashedWT/RACE-RWGLOBALS-FRAMELIST_root 400 MASHED_ORIG_TOP=1 MASHED_OT_RACELOAD=1 MASHED_DET_FRAMES=3000'`
- Arms: `A4_OUT=D:/MashedWT/RACE-RWGLOBALS-FRAMELIST_data A4_ROOT=D:/MashedWT/RACE-RWGLOBALS-FRAMELIST_root A4_EXE=<wt>/mashedmod/build/ddkobj/mashed_re_ddk.exe A4_RO_CKPT=0,1,17,18,299,301,600 test-slot bash -c 'py -3.12 <orch-wt>/re/orchestrator/mashed_lock.py run --label RACE-RWGLOBALS-FRAMELIST -- py -3.12 verify/savelink_20261023/run_arms.py RO_ROIMG-pr1 RO_ROLOAD-pr1 RO_ROLOADC-pr1'`
- First divergence: `py -3.12 re/frida/detorig_firstdiv.py D:/MashedWT/RO-REF-NAME_data/ref1 D:/MashedWT/RACE-RWGLOBALS-FRAMELIST_data/<arm> --masks re/frida/warp_normalize.tsv,re/frida/replay_masks.tsv --ignore 0x00614400-0x00614800,0x00771800-0x00771c00 --exe-map mashedmod/build/ddkobj/mashed_re_ddk.map`
- Words: `py -3.12 verify/ro_20261116/portotrace_words.py D:/MashedWT/RO-REF-NAME_data/ref1/ckpt201.data.bin D:/MashedWT/RACE-RWGLOBALS-FRAMELIST_data/<arm>/ckpt01.data.bin 0x007d3e70,...,0x007d3e98`
