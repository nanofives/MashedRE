# RESULT RACE-RWLAYER-00543D40 (run 53, 2026-11-21), checkpoint 2 of the RACE-RWGLOBALS-FRAMELIST worker

Branch `orch/RACE-RWLAYER-00543D40` = `orch/RACE-WRITER-0067EA74` `4946232ea` (the 0x004924f0 writer fix) with
`orch/RACE-RWGLOBALS-FRAMELIST` `9f6685512` merged (`f26fdac1e`, no conflict). Both sit on `orch/integration` `a9e5b4c8a`, which is
still the head. PREREG `verify/ro_20261116/PREREG_RACE-RWLAYER-00543D40.md`, committed alone (`e9f99c108`) before any build or
arm. Row kind: top-down / race path. Worker claude2, Opus 5.5, lead cmwpwu9qr5v1xnz1c1zq4o204. No G-CTRL, no B, no 12x4:
not run, top-down row. No code change, no binding, no tracker edit, no promotion, no default change. One attempt.

## 0. Verdict: BLOCKED. The fix is the race load's DFF read on the RW core engine, a move. Sized here, not started (L0 condition 2)

| item | result |
|---|---|
| who creates the atomic 0x00543d40 reads (condition 1) | **the librw binding** `MashedRw_ClumpStreamRead_004e7420` (`LibRw/RwApiBind.cpp:980`): `RpClumpStreamRead(...) -> rw::Clump::streamRead ... (11890 bytes, 17 atomics)`, both without RWMOD (RACE-WRITER-0067EA74 `RO_ROLOAD-pr1.log:836`) and with it (this row, `RO_ROLOADRW-pr1.log:856`). Not a call under 0x004b5190: the read is 0x004264d0 -> 0x0042a5d0("powerups_gold.dff") -> `Orig_004b3bf0` -> `FUN_004e7420` (`re/match/s18/004b3bf0.c:48`, `#define FUN_004e7420 MashedRw_ClumpStreamRead_004e7420` at `Match/PortTop_004b3bf0.cpp:13`). 0x004b5190 only receives the atomics afterwards (0x00426460: 0x004b3fc0 collects, then `FUN_004b5190(list[i], 0, 0)`) |
| RWMOD on the faulting arm | **off** (RACE-WRITER log line 133 `0x00493600: missing (MASHED_ORIG_TOP_RWMOD off ...)`, line 200 `0x00427ca0: RWMOD off`). The re-run with `MASHED_ORIG_TOP_RWMOD=1` (section 2) changes nothing on this path |
| route the creating call to the matched RW core | **not done: it is a move.** The core clump read needs a core engine with the RpWorld and game plugins registered (section 3). That engine does not exist on the knob path, where the engine is librw's. Sized in section 4 |
| load length under `MASHED_ORIG_TOP` | **not measured**: `race.reached` false, `race.n` 0 (`RO_ROLOADRW-pr1/meta.json`). ref1: 108 |
| first divergence against ref1 | record 0 (tick 1), `0x00604604` ref `0x00000004` exe `0x00000000`, 117 counted blocks / 76 raw words, VALUE 4858, IDENTICAL-FOR-N 16 of 283, first NEW block `0x0067e800` at record 16 (the same verdict as RACE-WRITER-0067EA74's knob-on arms) |
| self-test | `[portstate-image]` PASS 0/8, `[OT_CODEPTR]` PASS 0/367, `[REPLAY]` init self-test 0 of 66 failed. The arm faults before `[OT_TIMER]` and `[TRACE]` print (as on RACE-WRITER-0067EA74's knob-on arms) |
| object build | IDENTICAL 1805/1857, DIFFERENT 28: the same 28 as the integration head's check. **No newly DIFFERENT unit** (section 5) |

## 1. What 0x00543d40 reads, and why the librw atomic breaks it

- `re/match/s18/004b5190.c`: `uVar1 = *(atomic + 0x18); FUN_00543d40(uVar1); FUN_00543d70(uVar1, p2); FUN_0055deb0(...);
  FUN_00543df0(...)`. +0x18 is read as the atomic's geometry pointer.
- `re/match/rwl04/00543d40.c`: `n = *(DAT_007dc634 + geometry)`, then `*(DAT_007dc634 + 4 + geometry) + 0xc` walked in
  steps of 0x10. `DAT_007dc634` is a geometry plugin offset.
- Its only writer in `re/match` is 0x00543e50 (`re/match/rwl04/00543e50.c:74`, hooks.csv C4):
  `DAT_007dc634 = FUN_004e8f50(8, 0x11f, FUN_005440e0, FUN_00544100, FUN_005441f0)`. 0x00543e50 is in the closure of the
  plugin attach 0x00493640 (section 4). The plugin attach runs only on the RW core route (`MASHED_RWC_CORE=1`,
  RESULT_RWC-PLUGIN-ATTACH.md section 1).
- On the knob path the atomic and its geometry are librw objects built by `rw::Clump::streamRead`. No RW core plugin
  was registered, so nothing gave them RW core plugin data.
- [UNCERTAIN] Not measured at the fault: the value of `DAT_007dc634` and what librw holds at atomic + 0x18. Measured:
  fault at `RwCore_00543d40+0xd`, read of address 0, eax 0, ecx 0. Register line "RACE-RWLAYER-00543D40 (librw atomic
  layout)".

## 2. The RWMOD arm (L0 request, PREREG section "Arm requested by L0")

One arm, `RO_ROLOADRW-pr1` (stack `ROLOADRW` = `@ROLOAD` + `MASHED_ORIG_TOP_RWMOD=1`), object build `1edf3bfc`, root
`D:/MashedWT/RACE-RWLAYER-00543D40_root`, test-slot outside and mashed_lock inside, after `mashed_lock.py status` showed no
PRIORITY / SOLO (the first launch failed in the harness before the game started: the root copy had not run, so
`Popen(cwd=ROOT)` raised `NotADirectoryError`. The root was copied and the arm re-run once). The arm stops `0xC0000005` after
9 s, 300 ticks. In order (`rwlayer_log_extract.txt`):
- the RWMOD paths run: `0x00427ca0: RWMOD on: matched body` (line 199), the image formats via 0x00493600 (line 350), and
  the module registration (line 353);
- the FRAMELIST frame open (line 152), then 0x004264d0 enters with `DAT_0067ea74 = 1` (line 848). 0x00458c70's bound body
  runs and returns (lines 849, 850), and `powerups_gold.dff` is read from training.piz;
- the clump comes from the **librw binding**: `[RW_BIND] RpClumpStreamRead(0x11E8EAC0) -> rw::Clump::streamRead 1121A098
  (11890 bytes, 17 atomics)` (line 856);
- the fault is at exe+0x2E96D (`RwCore_00543d40+0xd`, `mashed_re_ddk.map`), read of 0, eax 0, ecx 0 (lines 857..859).

PREREG predictions 1, 2 and 3 hold. RWMOD does not route the clump read: `MashedRw_ClumpStreamRead_004e7420` sends the read
to `RwCore_004e7420` only under `RwCore_FrameRoute()` (`MASHED_RWC_CORE=1` + `MASHED_RWC_FRAME=1`, `LibRw/RwApiBind.cpp:981`).
So this is not a "which knobs the arms carry" row.

## 3. Why the fix is a move and not a re-route of one call

Routing the one call `FUN_004e7420` under 0x004b3bf0 to `RwCore_004e7420` makes the RW core read the frame list, the
geometries (materials, textures), the atomics and their plugin extensions. On the knob path:
- The atomic / geometry / clump module state of the RW core (freelists, plugin registry) is not on the librw record.
  RACE-RWGLOBALS-FRAMELIST put only the frame module there (RESULT section 7: the record is the port struct
  `MashedRwGlobals`. +0xbc / +0xc0 / +0x164 are the core frame module's, the rest belongs to the bindings).
- The plugin offsets (`DAT_007dc634` and the rest) are set by the plugin attach 0x00493640, which on the RW core route
  runs between RwEngineInit and RwEngineOpen (RESULT_RWC-PLUGIN-ATTACH.md section 1). [UNCERTAIN] That a registration has
  to precede RwEngineOpen to size live objects rests on RW 3.x plugin semantics, read and not tested here. Register line
  "RACE-RWLAYER-00543D40 (plugin attach order)".
- The texture lookups of the core material read resolve against the core's dictionary list. On the knob path the
  dictionaries are librw's (`RegisterRwModules`: the texture module list "starts empty ... The exe's dictionaries are
  librw's", `LibRw/RwApiBind.cpp:2027..2032`).

So the creating call can run on the core only when the race load runs on the RW core engine, the `MASHED_RWC_CORE` /
`MASHED_RWC_FRAME` route. Today that route ends the boot after the first frame (`RwCore/RwCore_Knob.cpp` header,
RESULT_RWC-PLUGIN-ATTACH.md verdict). Moving the race load there is the run-54 RACE row the lead named. It was not started.

## 4. Size of the move (from the RwCore TU bindings, `rltools/rwc_closure.py`)

Closure = each RwCore body plus every callee its TU binds by `#define FUN_<rva> <symbol>`, followed through RwCore bodies.
Kinds: `verify/rwccorerun_20261111/RwCore_Bodies.tsv`. Owner lists: `log/orch/run53_bindlists/`. Tables:
`rwlayer_closure_*.tsv`.

| root | nodes | match100 | provisional (RWC-only list) | lifted | missing stop | other | hooks.csv |
|---|---|---|---|---|---|---|---|
| 0x004e7420 RpClumpStreamRead (core) | 88 | 54 | 24 | 7 | 0 | 3 (1 exe binding `Orig_004d7ff0`, 1 `RwCoreCopy`, 1 `RwCoreCrt` qsort) | C4 72, C2 12, C3 1, C1 1, none 2 |
| 0x00493640 plugin attach (core) | 289 | 85 | 39 | 158 | 3 (0x0053ed20, 0x00551770, 0x005518f0) | 4 (3 exe bindings, 1 `RwCoreHand_004cca80`) | C4 184, C1 78, C2 19, C3 2, none 6 |
| consumers 0x004b5190, 0x004e6e00, 0x004e66d0 | 23 | 20 | 3 | 0 | 0 | 0 | C4 23 |
| **union** | **374** | **137** | **63** | **165** | **3** | **6** | C4 253, C1 79, C2 31, C3 3, none 8 |

- Every node already has an exe body on the RW core route: 137 match100 bodies, 63 provisional bodies listed in
  `PROVISIONAL_RWC.tsv` for `MASHED_RWC_CORE` only, and 165 lifted bodies. Using them on the race-load path needs either the
  RWC route itself or new `PROVISIONAL.tsv` listings for this path (L-126). Three are missing stops.
- 50 of the 374 are in other rows' lists (`audio_owned.txt`, `bulk_cp1.tsv`). Among them are 0x00543d40, 0x00543d70 and
  0x00543df0 themselves (`bulk_cp1.tsv`), and the RW stream bodies 0x004cbd30 / 0x004cc5e0 (`audio_owned.txt`). Per-node
  owners are in the tables.
- Not in the closure, and also needed: the RW core engine bring-up and the core texture dictionaries for the race-load
  textures. On the RWC route these are RwEngineInit / Open / Start on the core (RESULT_RWC-CORE-RUN.md) and the
  menu-to-race path on the core, which does not exist yet.

## 5. Builds and checks

Lock / slot status before each step (`D:/MashedWT/RACE-RWLAYER-00543D40_status.txt`): VS build and object build `free
'mashed_machine'`, memory 23.3 GB committed of 29.0 GB. Arm: `HELD 'mashed_machine' label=PROV-INSITU-REF1` (a game run, no
PRIORITY / SOLO), then my lock. No other Mashed build was running at either build.

| check | result |
|---|---|
| VS2022 build `9f94a9eec` | `=== Build OK ===`, rva-lint 392 / 392 allowlisted, NEW 0. `mashed_re.exe` `39df24cf...` |
| object build | `=== Build OK ===`, `mashed_re_ddk.exe` `1edf3bfc...` |
| `ddkobj.py check` | IDENTICAL 1805/1857, DIFFERENT 28, not-in-map 24, units linked 1804/2040 |
| newly DIFFERENT units | none. Base: the integration head's check (`D:/MashedWT/PORT-OT-RACE-BIND-PROV4_ddkcheck.log`, same build inputs as `a9e5b4c8a`). Equal sets (28 = 28). The branch's new units are IDENTICAL: `_DataZeroFill` 0x004924f0, `_CarSelectReset` 0x00431d00 (RACE-WRITER), `_PortTop_Bound_00458c70`, both copies of 0x004c07b0 (FRAMELIST) |
| match.py / relocheck | not run: this row binds no new source |
| knob boot | not run: no code change in this row (the merged code's knob boots are on RACE-WRITER-0067EA74 and RACE-RWGLOBALS-FRAMELIST) |

## 6. Next

1. Run 54 RACE row: the race load on the RW core engine. Prerequisites: the RWC route past its first frame (RwCore_Knob
   ends the boot there today), the menu-to-race path on the core engine, and core texture dictionaries for the race
   textures. Then the 374-node closure of section 4 runs as the RWC route's bodies, with its 3 missing stops
   (0x0053ed20, 0x00551770, 0x005518f0) and the owners in the tables.
2. Until then 0x004264d0's clump branch (`DAT_0067ea74 = 1`, the RACE-WRITER fix) cannot run 0x00426460 / 0x004b5190 /
   0x004e6e00 on the knob path.

## Evidence

Committed (`verify/ro_20261116/`): this RESULT, the PREREG, `rwlayer_log_extract.txt`, `rwlayer_fd_ref1.md`,
`rwlayer_closure_004e7420.tsv`, `rwlayer_closure_00493640.tsv`, `rwlayer_closure_004b5190_004e6e00_004e66d0.tsv`,
`rltools/rwc_closure.py`, `raw_sha256_RACE-RWLAYER-00543D40.txt`, and stack `verify/savelink_20261023/stacks/ROLOADRW.txt`.

Raw (not committed): `D:/MashedWT/RACE-RWLAYER-00543D40_data` (`RO_ROLOADRW-pr1/`, `RO_ROLOADRW-pr1.log`, `fd_ref1_RO_ROLOADRW-pr1.out`,
`arms.out`), `D:/MashedWT/RACE-RWLAYER-00543D40_build_{vs,ddk}.log`, `_ddkcheck.log`, `_status.txt`.

Commands:
- Builds: `cmd /c "set MASHED_NO_DEPLOY=1&& mashedmod\build.bat"`, then with `set MASHED_DDKOBJ=1&&`; `py -3.12 re/tools/ddkobj/ddkobj.py check`.
- Arm: `A4_OUT=D:/MashedWT/RACE-RWLAYER-00543D40_data A4_ROOT=D:/MashedWT/RACE-RWLAYER-00543D40_root A4_EXE=<wt>/mashedmod/build/ddkobj/mashed_re_ddk.exe A4_RO_CKPT=0,1,17,18,299,301,600 test-slot bash -c 'py -3.12 <orch-wt>/re/orchestrator/mashed_lock.py run --label RACE-RWLAYER-00543D40 -- py -3.12 verify/savelink_20261023/run_arms.py RO_ROLOADRW-pr1'`
- First divergence: `py -3.12 re/frida/detorig_firstdiv.py D:/MashedWT/RO-REF-NAME_data/ref1 D:/MashedWT/RACE-RWLAYER-00543D40_data/RO_ROLOADRW-pr1 --masks re/frida/warp_normalize.tsv,re/frida/replay_masks.tsv --ignore 0x00614400-0x00614800,0x00771800-0x00771c00 --exe-map mashedmod/build/ddkobj/mashed_re_ddk.map`
- Closure: `py -3.12 verify/ro_20261116/rltools/rwc_closure.py <absolute wt> 004e7420 --tsv <out>` (and `00493640`, `004b5190 004e6e00 004e66d0`).
