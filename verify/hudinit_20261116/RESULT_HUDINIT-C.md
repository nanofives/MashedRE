# RESULT HUDINIT-C (run 51, top-down row): GREEN for the row acceptance

Branch `orch/HUDINIT-C` from `orch/integration` 7630e53ca. Opus 5.5 medium, one attempt, claude3.

**Verdict.** Acceptance met on both builds: under the frame route (`MASHED_ORIG_TOP=1 MASHED_RWC_CORE=1
MASHED_RWC_FRAME=1`) AppDestroy's 0x00427620 calls 0x00555830 with a non-zero `DAT_0067d838` (vs 0x09069794, object
build 0x092F3F6C), and that call now runs the matched RW core body `RwCore_00555830` instead of the stand-in. Both
boots end `WinMain 0x00492370 returned 0 after 600 frames`, rc 0. **Self-test: PASS** on every arm (section 3).
G-CTRL / B / 12 captures: not run (top-down row, the knob is off for those hashes).

## 1. Prerequisite check (register line T1-RWCORERUN-STAGE)
The ledger note said acceptance needs RWC-CORE-RUN (blocked since run 38). Answer: **met, through the RWC-FRAME-* rows.**
- RESULT_RWC-FRAME-CLUMP section 6 (d11, d12): under `MASHED_RWC_FRAME` the core's RwGlobals is installed at
  `0x007d3ff8`, 0x00427ca0 runs the matched body, and its font loader 0x00427880 runs the exe body
  `FUN_00427880` with the font read on `RwCore_00554390` (`Match/MatchS06_00427ca0.cpp` selector `MatchS06_00427ca0_Sel00427880`).
- The run-46/49 frameT logs of that row (`D:/MashedWT/RWC-FRAME-CLUMP_data/vs_frameT.log`, `ddk_frameT.log`) already
  showed `[ORIG_TOP] rva 0x00555830(0x08FB91FC)` / `(0x08EEDA3C)`: a non-zero handle, but the call took
  `stand-in (RWMOD off)`, because the 0x00427620 selector chose between the librw-bound `Orig_00555830` (RWMOD) and the stand-in only.
- The VS2022 frameT log (section 3) shows `RwGlobals 018B2130` installed by the core's RwTexDictionaryCreate binding.

So RWC-CORE-RUN's own exit (first menu frame drawn by the core, compared) is covered by RWC-FRAME-CLUMP (80 of 82
draws per frame with `--tol-anim 8`), and the RWC-CORE-RUN ledger stage is stale. Not changed here (lead/L0 call).

## 2. Change
- `mashedmod/src/mashed_re/Match/PortTop_00427620.cpp`: `PortTop_00427620_Call00555830` calls `RwCore_00555830`
  (`RwCore/RwCore_00555830.cpp`, `re/match/s24/00555830.c`, match100) when `RwCore_FrameRoute()` holds. Its callees there
  are `RwCore_00553e80`, `RwCore_004c5a60`, `RwCore_005c4d50`, `Orig_005c4d30` and RwGlobals +0x10c / +0x11c through the
  core's `DAT_007d3ff8`. Off the frame route nothing changes. No librw binding added (L-124).
- `verify/rwccorerun_20261111/PROVISIONAL_RWC.tsv`: 4 stale chain rows dropped with
  `prov_drop.py --only 0x004d8480,0x00554390,0x005c4ad0,0x005c4c60 --apply` (MATCH-PROV-DROP-RULE): each is
  BYTE-IDENTICAL under match.py `--bytes`, relocheck OK, and included by an exe TU. The other 99 DROP rows of the
  full dry run were left alone (out of scope). [UNCERTAIN] the generator re-adds them on a regeneration (register
  line GEN-RWCORE-PROVLIST-DRIFT, already there).
- `re/orchestrator/UNCERTAIN_REGISTER.md`: one line, HUDINIT-C-FONT-TEARDOWN-STATE.

## 3. Runs (private root D:/MashedWT/HUDINIT-C_root, test-slot outside, mashed_lock inside)
| arm | build | exit | 0x00555830 line | self-tests |
|---|---|---|---|---|
| vs_frameT | VS2022 | rc 0, 20.0 s, 600 frames | `(0x09069794): RW core body RwCore_00555830 (frame route) (HUDINIT-C)` | portstate-image 0/8, OT_CODEPTR 0/367, binding 0/11, RWC-DEVICE 0 fail, OT_TIMER 0/6000: PASS |
| ddk_frameT | object (`MASHED_DDKOBJ=1`) | rc 0, 52.6 s, 600 frames | `(0x092F3F6C): RW core body RwCore_00555830 (frame route) (HUDINIT-C)` | same set, PASS |
| vs_off | VS2022, no frame route | rc 0, 71.2 s, 600 frames | `(0x00000000): stand-in (RWMOD off)` (unchanged) | portstate-image, OT_CODEPTR PASS |

Both frameT arms: `live clumps at RwEngineStop: 1` (the known Bomb.dff clump, row RWC-FRAME-BOMB-DESTROY), no `AV at`, no `FAIL`.

## 4. Builds and object build
Both builds `=== Build OK ===` (`MASHED_NO_DEPLOY=1`). `ddkobj.py check` before and after the drop (same base, the
object build rebuilt in between): identical output, **IDENTICAL 1561/1599, DIFFERENT 22, not-in-map 16**, units linked
1549/1761, refusals 11 (externalized-static-register-convention 7, wrapper-redefines-globals 4). No newly DIFFERENT
unit: this row's only code change is a selector in a TU outside the object-build unit set, and the 22 DIFFERENT units are
the base's (`0x00554200` among them, a 99.3% source, not a 100% unit). Chain units in BYTECHECK.tsv, all IDENTICAL:
0x00427880, 0x00554390, 0x00553e80, 0x005551d0, 0x005c4c60, 0x005c4ad0, 0x004cc790, 0x004c5ca0, 0x004d8480, 0x00555830
(both `Orig_` and `RwCore_` copies). The four dropped rows were already linked and IDENTICAL before the drop.

## 5. Chain status (match.py `--bytes` + relocheck.py on the source the exe includes, this branch)
| rva | source | score | bytes | relocheck | hooks.csv |
|---|---|---|---|---|---|
| 0x00427880 | re/match/s06/00427880.c | 100% | BYTE-IDENTICAL 67 B | OK | C4 |
| 0x00554390 | re/match/rwl05/00554390.c | 100% (ledger said 33.5%) | BYTE-IDENTICAL 1110 B | OK | C4 |
| 0x00553e80 | re/match/s24/00554200.c | 100% | BYTE-IDENTICAL 100 B | OK | C4 |
| 0x00554200 | re/match/s24/00554200.c | 99.3% | 6 bytes differ at +0x9d | OK | C2 |
| 0x00554940 | re/match/s24/00554940.c | 12.5% | 1220 of 1417 B differ | not run | C2 |
| 0x005551d0 | re/match/rwl05/005551d0.c | 100% | BYTE-IDENTICAL 162 B | OK | C4 |
| 0x005c4c60 | re/match/s28/005c4c60.c | 100% (ledger said 93%) | BYTE-IDENTICAL 186 B | OK | C4 |
| 0x005c4ad0 | re/match/s28/005c4ad0.c | 100% | BYTE-IDENTICAL 205 B | OK | C4 |
| 0x004cc790 | re/match/s20/004cc790.c | 100% | BYTE-IDENTICAL 80 B | OK | C4 |
| 0x004c5ca0 | re/match/s19/004c5ca0.c | 100% | BYTE-IDENTICAL 16 B | OK | C4 |
| 0x004d8480 | re/match/s21/004d8480.c | 100% (ledger said 85.7%) | BYTE-IDENTICAL 86 B | OK | C4 |
| 0x00555830 | re/match/s24/00555830.c | 100% | BYTE-IDENTICAL 215 B | OK | C4 |
| 0x005547f0 | lifted (RwCoreLift_005547f0) | - | - | - | C1 |

No promotion file: every 100% row is already C4 by match in hooks.csv. Ledger percentages in the row title were stale.

## 6. Open scope (does not block this row's acceptance)
- **0x00556e40** (the sibling call in 0x00427620): still the stand-in on the frame route with a non-zero handle
  (vs 0x090606F0). C4 by match but no RwCore_ TU. **New row HUDINIT-D** (`roadmap.ps1 add`).
- 0x00554200 99.3%: row MATCH-RT2D-554200, parked by L-146 (Fable RED). Runs as a provisional body on the knob path.
- 0x00554940 12.5% (provisional, knob path) and 0x005547f0 (lifted callback pointer): no row. They run, they are not
  stand-ins, and they do not block acceptance. A match row is the lead's call.
- [UNCERTAIN] HUDINIT-C-FONT-TEARDOWN-STATE: the matched 0x00555830 now frees the font on the core, not compared with
  the original's teardown state (register line).

## 7. Raw data (D:/MashedWT/HUDINIT-C_data, not committed), sha256
```
5ebc8153c905d12c3088966cb35c1b1be67a1711169788dac0366f6c962d8541 vs_frameT.log
ea7dc6756de546e4cbda3e8d958bdc95246f2d8cc0af440dd75f1f2791e1485a ddk_frameT.log
2d022730b62771c81a92c8c0e75d897e029337ba31fc19152665c86d8c79380e build_vs.log
14cfcff59f3f028ab2aaed7163822340c52df06197118083dfa53ab793932bc4 build_ddk.log (= build_ddk2.log)
d1d3e0c1934c5fe4e4a8ca72b4a0ffae5ff029fcc9b4b2c67c4e7c830e109ecd ddkobj_check_pre.txt (= ddkobj_check_post.txt)
c75595c41958b5ba61d24a96d8ec9bddd353a1e19c7a1f00ee96be08d21b300a BYTECHECK_pre.tsv
317818323b751d1e3bd717aa3cda3c935e9e800eb0630268c56c0b692809e1ff match_chain.txt
51d43df7c75d735b3398eabd842b5886337362205c11e043e47de84ad2d01778 relocheck_chain.txt
fab5be8768a617643b3265b07e82dc724f22a371c2a584ecdef64e8ded4439b2 provdrop_apply.tsv
```
vs_off.log is in the same folder.
