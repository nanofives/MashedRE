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

{'unchanged': 49, 'body-changed': 7, 'callee-changed': 2} rows 58
