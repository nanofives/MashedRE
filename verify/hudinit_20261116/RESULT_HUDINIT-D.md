# RESULT HUDINIT-D (run 51, top-down row): GREEN for the row acceptance

Branch `orch/HUDINIT-D` from `orch/integration` 59935c224. Opus 5.5 medium, one attempt, claude2.

**Verdict.** Under the frame route (`MASHED_ORIG_TOP=1 MASHED_RWC_CORE=1 MASHED_RWC_FRAME=1`) all four calls AppDestroy's
0x00427620 makes to 0x00556e40 now run the matched RW core body `RwCore_00556e40` instead of the stand-in, on both builds.
Both frameT boots end `WinMain 0x00492370 returned 0 after 600 frames`, rc 0. **Self-test: PASS** on every arm (section 3).
G-CTRL / B / 12 captures: not run (top-down row, the knob is off for those hashes).

## 0. Base note: HUDINIT-C is cherry-picked
The task said HUDINIT-C (b4d380496) was merged. It is not: at 59935c224 `orch/integration` does not contain it (ledger
stage `merge-queued`). This row edits the same selector file, so the branch's first commit is a cherry-pick of b4d380496
(b69feb021, same patch). When the train merges both branches the shared hunk is identical on both sides. If the train
merges HUDINIT-D first, HUDINIT-C becomes a no-op merge.

## 1. Change
- `verify/rwccorerun_20261111/gen_rwcore_tus.py`: `'00556e40'` added to `ROOTS`. The generator was run once. Its closure
  added exactly one row (`RwCore_Bodies.tsv`: `0x00556e40 match100 re/match/s24/00556e40.c 100.0 RwCore_00556e40`) and one
  TU, `mashedmod/src/mashed_re/RwCore/RwCore_00556e40.cpp` (callee 0x004c5a60 binds to the existing `RwCore_004c5a60`).
  **GEN-RWCORE-PROVLIST-DRIFT handled:** the regeneration re-added the 4 rows HUDINIT-C dropped from `PROVISIONAL_RWC.tsv`
  (0x004d8480, 0x00554390, 0x005c4ad0, 0x005c4c60), and moved the whole generated block of `exe_sources.rsp` to the file
  end, and rewrote every other generated file with LF endings only. All of that was reverted to HEAD. Kept: the ROOTS line,
  the one `RwCore_Bodies.tsv` row, the new TU, and one hand-added line `"RwCore\RwCore_00556e40.cpp"` inside the existing
  generated block of `exe_sources.rsp`. `PROVISIONAL_RWC.tsv` is unchanged by this row (0x00556e40 is match100, no row).
- `mashedmod/src/mashed_re/Match/PortTop_00427620.cpp`: `PortTop_00427620_Call00556e40` calls `RwCore_00556e40` when
  `RwCore_FrameRoute()` holds and the handle is non-zero, the stand-in on a zero handle (the body reads `*(h + 0x74)` with
  no null check, re/match/s24/00556e40.c). Every frame-route call is logged (cap 8). Off the frame route nothing changes.
  No librw binding added (L-124).
- `re/tools/dual_copy_allowlist.txt`: one `ORIGTOP-ONLY DUP-IN-TARGET:00556e40` entry (Match/PortTop_00556e40.cpp and
  RwCore/RwCore_00556e40.cpp include the same source), the same form as the 0x00555830 entry above it. The rva-lint
  refused the build without it.
- `re/orchestrator/UNCERTAIN_REGISTER.md`: one line, HUDINIT-D-ENTRY-RELEASE-STATE.

## 2. Static checks on the source the exe includes (re/match/s24/00556e40.c)
- `match.py --bytes`: `0x00556e40 _FUN_00556e40: 100.0% (regs 100.0%, ops 100.0%) 26/26 ins flags=/O2 /Ob1 /G5 tc=ddk`,
  `67 B whole symbol, 0 differing unmasked bytes -> BYTE-IDENTICAL`.
- `relocheck.py`: `0x00556e40 OK checked=3 unchecked=0`.
- hooks.csv already has 0x00556e40 at C4 (route match). No promotion file.

## 3. Runs (private root D:/MashedWT/HUDINIT-D_root, test-slot outside, mashed_lock inside, verify/rwcframemovie_20261116/run_arms.sh with RWC_ROOT / RWC_DATA)
| arm | build | exit | 0x00556e40 lines | self-tests |
|---|---|---|---|---|
| vs_frameT | VS2022 | rc 0, 18.4 s, 600 frames | 4 x `RW core body RwCore_00556e40 (frame route) (HUDINIT-D)` on 0x09070180 / 0x09070200 / 0x09070280 / 0x09070300 | portstate-image 0/8, OT_CODEPTR 0/367, RWC_CORE binding 0/11, RWC-DEVICE 0 fail, OT_TIMER 0/6000: PASS |
| ddk_frameT | object (`MASHED_DDKOBJ=1`) | rc 0, 16.2 s, 600 frames | 4 x same on 0x0185D6F0 / 0x0185D770 / 0x0185D7F0 / 0x0185D870 | same set, PASS |
| vs_off | VS2022, no frame route | rc 0, 8.0 s, 600 frames | `(0x00000000): stand-in (RWMOD off)`, as in HUDINIT-C's vs_off | portstate-image 0/8, OT_CODEPTR 0/367: PASS |

Both frameT arms: 0x00555830 still on `RwCore_00555830` (HUDINIT-C), `live clumps at RwEngineStop: 1` (the known
RWC-FRAME-BOMB-DESTROY clump), no `AV at`, no `FAIL` in any of the three logs. The test slot waited 600 s for memory, then
ran local (no hub dispatch line).

## 4. Builds and object build
Both builds `=== Build OK ===` (`MASHED_NO_DEPLOY=1`), the object build took 14 min. `ddkobj.py check`:
**IDENTICAL 1562/1600, DIFFERENT 22, not-in-map 16**, units linked 1550/1762, refusals 11
(externalized-static-register-convention 7, wrapper-redefines-globals 4), provisional kept VS2022 201 units + 5 fns.
Against HUDINIT-C's check (IDENTICAL 1561/1599, units 1549/1761): +1 unit, the new `_RwCore_00556e40` (IDENTICAL, 80 B;
`_Orig_00556e40` IDENTICAL too), and the DIFFERENT list is the same 22 RVAs
(0x004332a0 ... 0x00554200 ... 0x005aea60). No newly DIFFERENT unit.
The comparison is with HUDINIT-C's check on its base 7630e53ca, not a fresh object build of 59935c224 (that costs another
14 min build). A unit that turned DIFFERENT here would show as a new RVA in that list, and none does.

## 5. Open scope
- [UNCERTAIN] HUDINIT-D-ENTRY-RELEASE-STATE (register line): the four entry releases are not compared with the
  original's teardown state.
- `[PORT-TOP] rva 0x004d8480: stand-in, not run (PORT-TOP-04C)` and 0x00552b90 (FontSys_Shutdown) are still stand-ins at
  AppDestroy on the frame route. Not this row.

## 6. Raw data (D:/MashedWT/HUDINIT-D_data, not committed), sha256
```
e26f4645ac0a73c5a554a89e7a30189ab0785d33f8dab772137d5c0a37a707d7 vs_frameT.log
146e4ad75666cc055bf836a407fdf47c89c14f1f10deb1dfea39603b899d9321 ddk_frameT.log
235a4bbe090e288b65e849a764d50b2b43e972dcb8652be50003682b1dd1ba1d vs_off.log
3a4747f95cee300c1761aa4bd99b17b7efc37f96d92b4408e4812f9299a325c2 run_arms.txt
7c01a45db373271834a733b2a26881278fadbc47fd6e1b26d870d9e8b8c33659 build_vs.log
cf35a80db762896216627e35c706f98dfe8f002cb695ddf871d37a1faa604275 build_ddk.log
fcdd3e9bd01dfae22af56aa47c2dd5e9ba86261cb8774ab8b335364ea4e845fe ddkobj_check.txt
22b22515300bb1c21c64de634432e8ffb336fe06b48ec0e6c99b4bbb2a69967a match_00556e40.txt
3e505bf692d7bc3646a7623f6e051b437d24442afe700753e28cf1cadb48e375 relocheck_00556e40.txt
```
