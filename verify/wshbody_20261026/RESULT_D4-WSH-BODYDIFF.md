# RESULT D4-WSH-BODYDIFF: C4 stale-candidate triage by function body

Text-only. Branch `orch/D4-WSH-BODYDIFF` off `orch/integration` @ `c003971ad`. No build, no game run,
no Frida, no tracker edit. Source: `verify/d4scope_20261026/SCOPING_D4-WSH.md` Findings 4, `ROADMAP.md:2997`.

**Verdict: GREEN.** 58 rows checked. 7 body-changed, 2 callee-changed, 0 comment-only, 49 unchanged.
The file-date heuristic over-counted by 49 of 58: those rows sit in a TU that was committed after their
evidence, but the function itself did not change.

## Counts

| Verdict (evidence copy) | Rows |
|---|---|
| body-changed | 7 |
| callee-changed | 2 |
| comment-only | 0 |
| unchanged | 49 |
| **checked** | **58** |

Exe copy of dual-copy rows (no evidence of its own, handed to D4-WSH-EXEFILE): 4 of 5 changed after the
`.asi` evidence date (`0x004325c0`, `0x00432b30`, `0x0040b890`, `0x0040b8e0`). The fifth, `0x0042b8d0`, has a single body.

## The 9 rows that changed

| RVA | Function | Change commit(s) | Class | What changed |
|---|---|---|---|---|
| `0x0043c5b0` | MenuDrawLoopTwin (`Frontend/MenuDrawLoopTwin.cpp:141-288`) | `c30c5ed5d` 2026-08-27 (body + callee `StaticDraw:122-136`) | body-changed, logic | `StaticDraw(r, col_scratch)` became `StaticDraw(r)` with a constant `kStaticShadow`. The commit message says the earlier body drew a WHITE shadow on a screen with no preceding item row, which the 2026-06-10 nav-driven 20/20 diff did not cover. The row's notes also have no C4 clause, only C2->C3. |
| `0x0046c7b0` | VehicleSlotGetter (`Vehicle/VehicleState.cpp:71-77`) | `714a97e1c` 2026-10-07, callee `VehRecord` `8401ec880` 2026-10-07 | body-changed, logic | `DAT_008815a4[idx*0x341]` became `VehRecord(idx) + kOff_Alive` plus a `!rec -> 0` guard. On the `.asi` branch `VehRecord` = `0x008815a0 + idx*0xd04` (`VehicleRecordBase.h:27-28,44`), `kOff_Alive` = 4 (`:30`), so the address is `0x008815a4 + idx*0xd04`, the same as before. The guard cannot fire there. This equality comes from reading the code, so it is C2-grade (`re/CONFIDENCE.md`). |
| `0x0057c210` | RwpBodyTableLookup (`Collision/RwpIntegrator.cpp:123-127`) | `0ca7a4359` 2026-09-09 | body-changed, A/B wrapper | Hand-rolled uninstall/call/reinstall self-test replaced by `SHADOW_AB_COUNTER` + `ShadowAB::Run(ab, RwpBodyTableLookup_impl, key)`. `RwpBodyTableLookup_impl` unchanged since evidence. |
| `0x0055ac00` | RwpShapeActiveBitSet (`:223-246`) | `2879c4d1b` 2026-09-09 | body-changed, A/B wrapper | Uninstall block replaced by `ShadowAB::OriginalWindow` RAII. `_impl` (`:211-220`) unchanged. |
| `0x0055b800` | RwpBodyMatrixRefresh (`:273-328`) | `2879c4d1b` 2026-09-09 | body-changed, A/B wrapper | Same change as above. `_impl` (`:256-266`) unchanged. |
| `0x0055dff0` | RwpBodyRefreshGate (`:359-388`) | `2879c4d1b` 2026-09-09 | body-changed, A/B wrapper | Same change as above. `_impl` (`:342-353`) unchanged. |
| `0x004938c0` | SoftwareTidyUpBeforeExiting (`Boot/LaunchHandshake.cpp:152-158`) | `e62783abc` 2026-08-13 | body-changed, rename only | Export renamed from `EngineStopHelper` to `SoftwareTidyUpBeforeExiting`. The statements did not change. |
| `0x00404f50` | SaveWrite (`Save/GameSave.cpp:264-268`) | callee `gFileWrite` `7ecffe118` 2026-10-23 | callee-changed, exe build only | `BackupBeforeFirstWrite(path)` added to the `MASHED_STANDALONE` `gFileWrite` (`:181-188`). The `.asi` branch binds `gFileWrite` to `0x004b3bb0` (`:76`), so the evidence copy is not affected. |
| `0x00404f80` | SaveFileExists (`:285-289`) | callee `gFileExists` `549196601` 2026-10-05 | callee-changed, exe build only | `gFileExists` (`:193-198`) was added in the `MASHED_STANDALONE` branch. The `.asi` branch binds it to `0x00550b00` (`:77`). |

The 4 Rwp rows cite `log/phys_c4_b5c_ALL6_GREEN_20260724.log`. Commit `2879c4d1b` reports a re-run
("304 samples / 6 functions / 1 boot, 0 divergent ... 6 of 6 log PATCHBYTE ... A/B-IS-REAL"), but it committed
no log file, so that re-run is not citable evidence.

## Demotion candidates (listed only, no tracker change)

1. `0x0043c5b0` MenuDrawLoopTwin. This is the strongest candidate. The body was fixed after its evidence for a
   defect that evidence did not exercise, and the row has no C4 clause. Options: demote to C3, or re-run
   `re/frida/menu_drawloop_diff.py` on a screen with no item row before the static rows and cite the result.
2. `0x0046c7b0` VehicleSlotGetter. The body text changed. Re-run `canonical_c4_racediff.py` on the current body,
   or demote. Reading the code says the `.asi` behaviour is the same, but reading is not C4 evidence.
3. `0x0057c210`, `0x0055ac00`, `0x0055b800`, `0x0055dff0` (Rwp B5c). The installed wrapper changed and `_impl`
   did not. Re-run the B5c phase-3 subset on the current body and commit the log. Demote if that cannot be done.
4. `0x004938c0`. Not a candidate on body grounds. Check that the hooks registry and the frida_diff harness
   resolve the new export name `SoftwareTidyUpBeforeExiting`.
5. `0x00404f50`, `0x00404f80`. Not candidates for the `.asi` C4. The exe copy belongs to the save lane / D4-WSH-EXEFILE.

## Hand-offs

- **D4-WSH-EXEFILE**: exe copies that changed after the `.asi` evidence. `0x004325c0` `Frontend/MenuNavSM.cpp`
  `Anim_Tick:867-922` (`3836926ff` 2026-06-15). `0x00432b30` `MenuNavSM.cpp` `Nav:965-1021` (`edba95beb`
  2026-06-12, `a05b9c14a` 2026-10-12, callees `KvLookup` `19e95b2f2`, `LogoOverlayFadeSet` `cde0ee52b`,
  `NavCountItems` `a05b9c14a`, `PlaceCursor` `76916f8b0`/`4b341a0a2`). `0x0040b890` / `0x0040b8e0`
  `exe_main.cpp:4115-4128` inline arms (`e43c7dba1`, `d11c67bbf` 2026-09-02, `5acba6dac`, `014685ce6` 2026-09-03,
  `6d255c141`, `69bb1be99` 2026-09-04). The `.asi` copies of all four are unchanged.
- **D4-WSH-NOEVID / re-classify**: `0x0042b8d0`'s `file` cell names `Util/StateAccessors.cpp`. That file does not
  exist on this lineage (`git log --all` finds it only at side-branch commit `ca28ea718`). The installed body is
  `Boot/FrameDispatch.cpp:22-24` (`RH_ScopedInstall` at `:26`), and it is unchanged since `f9ec4178b` 2026-05-16.
- Re-run this step after the last D4-DC consolidation row merges (scoping note, "Notes for the lead").

## Method

1. `stale_list.py c2bf772f8 exe_or_file` rebuilds the scoping population: C4 rows whose body .cpp (`exe_file`,
   else `file`) has `git log -1 --format=%cs` later than the newest `YYYY-MM-DD`/`YYYYMMDD` date in
   `scenario + frida_diff + notes`. It gives 58 rows in 34 files at the scoping base and the same 58 RVAs at
   `c003971ad` (`stale58.tsv`, `stale_head.tsv`). The other two body choices give 38/19 (`exe`) and 51/32 (`file`), so
   `exe_or_file` is the scoping rule.
2. `bodydiff.py c003971ad stale58.tsv bodydiff.tsv --detail bodydiff_detail.txt` takes one copy per row, or both
   copies for the 5 rows where `file` and `exe_file` are different .cpp files. For each copy:
   - Function: the `RH_ScopedInstall(X, 0x<rva>)` name, else the hooks.csv name. Range: top-level definition from
     the signature line to the matching brace (comments, strings and namespace braces handled). For exe copies that
     are not a named function, `EXE_SITES` gives the range: `Anim_Tick`, `Nav`, or `exe_main.cpp:4115-4128`.
   - `git log -L<start>,<end>:<file> c003971ad` keeps commits with committer date > the evidence date. If a kept
     commit also touches one of the row's `frida_diff` paths, it is the evidence commit and is dropped. Each hunk
     is classed comment-only (the `-` and `+` lines are equal once comments and whitespace are stripped),
     created (the parent had none of the range), or body-changed. Net check: the range in the parent of the
     oldest kept commit is compared with the range at head. Net-equal means no change, which is how
     `LogoOverlayDraw`'s 2026-06-12 add-then-revert (`87f526736`, `67710b3fc`) folds to unchanged.
   - Callees (depth 1): identifiers called in the body that have exactly one definition under `mashedmod/src`,
     each run through the same `git log -L` + net check. Three callees have 2 definitions and were checked by hand:
     `vt20` (`MenuDrawLoopTwin.cpp:56`, last change `b37f8b312` 2026-06-11, the evidence commit) and `readU32at`
     (`GameSaveBuffer.cpp:141`, last change `6098fd23a` 2026-05-22). Both are unchanged after evidence. Calls to
     original RVAs through `reinterpret_cast` are not port code and are not followed.
3. `row_table.py bodydiff.tsv` folds the copies into one verdict per row (`row_table.md`). The verdict is the
   evidence copy's.
4. Same-day commits (committer date == evidence date) are not counted. The 5 same-day body-changed commits were
   checked against the timestamp of the commit that added the row's evidence file. All 5 come first:
   `9cde82cb2` 17:59 < `4ff1c03d0` 19:10 (0x00492270), `ea7a93afb` 04:52 < `78046cc22` 16:52 / `ec26d9444` 17:15
   (0x0043c000, 0x004295a0), `249aa785a` 04:38 < `78046cc22` 16:52 (0x0042b180). 0x0055b800's same-day
   commits (`63d10c5c4`, `cd1d5f143`, 2026-07-23) cannot be ordered this way, because its log was force-added
   later (`c00869311` 2026-10-16). The row is already body-changed through `2879c4d1b`.

Limits. The preprocessor is not modelled. Callee hits inside `#ifdef MASHED_STANDALONE` were sorted by hand (the
SaveWrite and SaveFileExists rows). The evidence date is the newest date anywhere in the row text, so it can be
later than the real evidence run. A late cut-off can hide a change, but it cannot invent one. So the error can
only push rows toward "unchanged". Header-only callees
were followed when they have one definition (`VehRecord`).

## Files

| File | Role |
|---|---|
| `stale_list.py` | rebuilds the 58-row population |
| `bodydiff.py` | per-function `git log -L` + net + callee check |
| `row_table.py` | one verdict per row |
| `stale58.tsv`, `stale_head.tsv` | population at `c2bf772f8` and `c003971ad` |
| `bodydiff.tsv` | 63 lines (58 rows + 5 exe copies) |
| `bodydiff_detail.txt` | every `git log -L` commit per range, the net result, and each callee |
| `row_table.md` | the 58-row table below |

## All 58 rows

| RVA | hooks.csv name | evidence copy (file:lines) | evidence date | verdict | exe copy |
|---|---|---|---|---|---|
| `0x00492270` | SubsystemInit | `Boot/SubsystemInit.cpp:207-220` | 2026-10-12 | unchanged | - |
| `0x004938c0` | SoftwareTidyUpBeforeExiting | `Boot/LaunchHandshake.cpp:152-158` | 2026-06-09 | body-changed; body e62783abc(2026-08-13,body-changed) | - |
| `0x004c2f00` | RwEngineGetCurrentMode | `Boot/VideoConfig.cpp:113-128` | 2026-06-06 | unchanged | - |
| `0x00404e50` | SaveLoad | `Save/GameSave.cpp:242-246` | 2026-10-11 | unchanged | - |
| `0x00404f50` | SaveWrite | `Save/GameSave.cpp:264-268` | 2026-10-11 | callee-changed; callee gFileWrite@7ecffe118 | - |
| `0x00404f80` | SaveFileExists | `Save/GameSave.cpp:285-289` | 2026-06-09 | callee-changed; callee gFileExists@549196601 | - |
| `0x004099e0` | SaveStatusClear | `Save/GameSave.cpp:221-223` | 2026-06-09 | unchanged | - |
| `0x00407a40` | Table8a9640Get | `Util/PromoLoop_round42.cpp:28-30` | 2026-06-13 | unchanged | - |
| `0x0043c5b0` | MenuDrawLoop | `Frontend/MenuDrawLoopTwin.cpp:141-288` | 2026-06-10 | body-changed; body c30c5ed5d(2026-08-27,body-changed); callee StaticDraw@c30c5ed5d | - |
| `0x0042e3a0` | MenuChromeShellA | `Frontend/MenuSpriteDispatch.cpp:537-591` | 2026-10-11 | unchanged | - |
| `0x0042b930` | MenuAlphaGet | `Frontend/MenuGetters.cpp:23-25` | 2026-06-05 | unchanged | - |
| `0x0042b8d0` | StatePhaseIsIdle | `Boot/FrameDispatch.cpp:22-24` | 2026-06-06 | unchanged | - |
| `0x00493f80` | IntroVideoDimGetter | `Frontend/IntroSplash.cpp:138-147` | 2026-10-11 | unchanged | - |
| `0x0042c2f0` | SetDat0067ecb8 | `Util/UtilBatch.cpp:47-49` | 2026-06-09 | unchanged | - |
| `0x0042b8c0` | ScreenHeightGet | `Frontend/MenuButtonDetect.cpp:482-485` | 2026-06-19 | unchanged | - |
| `0x00427780` | FontText_StringTableLookup | `HUD/HudBatch.cpp:300-306` | 2026-06-09 | unchanged | - |
| `0x0042f8d0` | MenuMenusBC | `Frontend/MenuMenusB.cpp:157-257` | 2026-10-12 | unchanged | - |
| `0x0040b7a0` | HotkeyStringBaseGet | `Frontend/MenuScoreGetters.cpp:54-56` | 2026-06-09 | unchanged | - |
| `0x00430a10` | HudSlotTypePlayer0 | `Frontend/SpriteGate.cpp:175-192` | 2026-06-09 | unchanged | - |
| `0x00430a60` | HudSlotTypePlayer1 | `Frontend/SpriteGate.cpp:210-227` | 2026-06-09 | unchanged | - |
| `0x00430ab0` | HudSlotTypePlayer2 | `Frontend/SpriteGate.cpp:245-262` | 2026-06-09 | unchanged | - |
| `0x0042a920` | Frontend::PostTrophyEvent | `Save/CareerEvents.cpp:41-44` | 2026-06-09 | unchanged | - |
| `0x0046d700` | VehicleVec3At9C8Get | `Vehicle/VehicleState.cpp:167-173` | 2026-07-31 | unchanged | - |
| `0x0043c000` | TimerSlotTickDispatcher | `Util/UtilMid.cpp:718-747` | 2026-10-12 | unchanged | - |
| `0x004325c0` | FUN_004325c0 | `Frontend/MenuAnimTickTwin.cpp:59-116` | 2026-06-10 | unchanged | body-changed; body 3836926ff(2026-06-15,body-changed) |
| `0x0042aff0` | MenuButtonDetectA | `Frontend/MenuNav.cpp:401-403` | 2026-10-12 | unchanged | - |
| `0x0042b180` | MenuButtonDetectB | `Frontend/MenuNav.cpp:408-410` | 2026-10-12 | unchanged | - |
| `0x00409930` | LoadingState3Enter | `Util/UtilBatch.cpp:89-93` | 2026-06-09 | unchanged | - |
| `0x004098b0` | LoadingState1Enter | `Util/UtilBatch.cpp:67-72` | 2026-06-09 | unchanged | - |
| `0x0046cbb0` | VehicleCarStateRead | `Vehicle/VehicleState.cpp:186-194` | 2026-10-16 | unchanged | - |
| `0x00552df0` | FontCtx_SetTranslation | `HUD/FontCtx.cpp:156-175` | 2026-06-09 | unchanged | - |
| `0x004295a0` | HudDualLabelRender | `Util/UtilMid.cpp:501-511` | 2026-10-12 | unchanged | - |
| `0x0046c7b0` | VehicleSlotGetter | `Vehicle/VehicleState.cpp:71-77` | 2026-08-15 | body-changed; body 714a97e1c(2026-10-07,body-changed); callee VehRecord@8401ec880 | - |
| `0x0042fe80` | GetRaceEndFlag | `Frontend/MenuInit.cpp:28-30` | 2026-06-09 | unchanged | - |
| `0x00550bc0` | VfsStreamGetType | `Save/VfsStream.cpp:109-116` | 2026-10-11 | unchanged | - |
| `0x00404ee0` | Save::SerializeToBuffer | `Save/GameSaveBuffer.cpp:189-227` | 2026-06-09 | unchanged | - |
| `0x00404e80` | Save::DeserializeFromBuffer | `Save/GameSaveBuffer.cpp:151-182` | 2026-06-09 | unchanged | - |
| `0x00485360` | DynObjListGetCount | `Util/PromoLoop_round17.cpp:83-86` | 2026-08-15 | unchanged | - |
| `0x0057c210` | RwpBodyTableLookup | `Collision/RwpIntegrator.cpp:123-127` | 2026-07-24 | body-changed; body 0ca7a4359(2026-09-09,body-changed) | - |
| `0x0055ac00` | RwpShapeActiveBitSet | `Collision/RwpIntegrator.cpp:223-246` | 2026-07-24 | body-changed; body 2879c4d1b(2026-09-09,body-changed) | - |
| `0x00498bc0` | VideoGetRenderWidth | `Boot/VideoConfig.cpp:52-55` | 2026-10-09 | unchanged | - |
| `0x00498bd0` | VideoGetRenderHeight | `Boot/VideoConfig.cpp:68-71` | 2026-10-09 | unchanged | - |
| `0x00492770` | MainLoopInit | `Boot/FrameDispatch.cpp:173-196` | 2026-10-11 | unchanged | - |
| `0x00431ae0` | FUN_00431ae0 | `Boot/BootLowRvaCluster.cpp:316-320` | 2026-06-09 | unchanged | - |
| `0x00431af0` | FUN_00431af0 | `Boot/BootLowRvaCluster.cpp:341-345` | 2026-06-09 | unchanged | - |
| `0x00431b00` | FUN_00431b00 | `Boot/BootLowRvaCluster.cpp:366-370` | 2026-06-09 | unchanged | - |
| `0x00405430` | Pred405430 | `Util/PromoLoop_sessionB.cpp:298-303` | 2026-08-15 | unchanged | - |
| `0x00407db0` | RangeTableGroupOffset | `Gameplay/RangeTable_ah1.cpp:201-224` | 2026-10-10 | unchanged | - |
| `0x0040b890` | FUN_0040b890 | `Gameplay/Thresholds_ah4.cpp:39-48` | 2026-07-24 | unchanged | body-changed; body 69bb1be99(2026-09-04,body-changed) 6d255c141(2026-09-04,created) 5acba6dac(2026-09-03,created) d11c67bbf(2026-09-02,body-changed) e43c7dba1(2026-09-02,body-changed) |
| `0x0040b8e0` | FUN_0040b8e0 | `Gameplay/Thresholds_ah4.cpp:55-64` | 2026-07-24 | unchanged | body-changed; body 69bb1be99(2026-09-04,body-changed) 6d255c141(2026-09-04,created) 5acba6dac(2026-09-03,created) d11c67bbf(2026-09-02,body-changed) e43c7dba1(2026-09-02,body-changed) |
| `0x0040cd90` | thunk_FUN_00425ef0 | `Gameplay/ScoreMasks_ah3.cpp:71-83` | 2026-06-08 | unchanged | - |
| `0x0040dc70` | FUN_0040dc70 | `Gameplay/ScoreMasks_ah3.cpp:13-15` | 2026-06-08 | unchanged | - |
| `0x0055b800` | RwpBodyMatrixRefresh | `Collision/RwpIntegrator.cpp:273-328` | 2026-07-23 | body-changed; body 2879c4d1b(2026-09-09,body-changed) | - |
| `0x0055dff0` | RwpBodyRefreshGate | `Collision/RwpIntegrator.cpp:359-388` | 2026-07-24 | body-changed; body 2879c4d1b(2026-09-09,body-changed) | - |
| `0x0045caf0` | EqPredicate7f1a18 | `Util/PromoLoop_round58.cpp:55-60` | 2026-06-13 | unchanged | - |
| `0x0046bda0` | FUN_0046bda0 | `Util/PromoLoop_sessionB.cpp:3849-3875` | 2026-06-15 | unchanged | - |
| `0x00473ee0` | LogoOverlayDraw | `Frontend/LogoOverlayTwin.cpp:30-38` | 2026-06-10 | unchanged | - |
| `0x00432b30` | MenuPromptStrip | `Frontend/PromptStripTwin.cpp:137-147` | 2026-06-11 | unchanged | body-changed; body a05b9c14a(2026-10-12,body-changed) edba95beb(2026-06-12,body-changed); callee KvLookup@19e95b2f2 LogoOverlayFadeSet@cde0ee52b NavCountItems@a05b9c14a PlaceCursor@76916f8b0/4b341a0a2 |
