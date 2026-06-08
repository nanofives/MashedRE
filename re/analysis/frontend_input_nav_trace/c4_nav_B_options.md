# C4 navigate batch — nav=[4, 4, 12, 12, 4], n=120, 13 trio-OK

| RVA | name | calls(post-nav) | rate | installed | survived | verdict | why |
|---|---|---|---|---|---|---|---|
| 0x0042e3a0 | MenuChromeShellA | 1482 | 180/s | True | True | **C4-trio-OK** |  |
| 0x0042d5a0 | MenusBodyA | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x00472f40 | TextGradientV0V1Override | 1482 | 180/s | True | True | **C4-trio-OK** |  |
| 0x004730b0 | TextGradientV2V3Override | 1482 | 180/s | True | True | **C4-trio-OK** |  |
| 0x0042aad0 | MenuDimSet | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x0042aae0 | MenuIm2DQuad | 1482 | 180/s | True | True | **C4-trio-OK** |  |
| 0x00472c60 | ChromeBaseDraw | 32604 | 3976/s | True | True | HOT | HOT~3976/s(behavioral-lane) |
| 0x00492d20 | IntroSplashFrameTickShim | 1482 | 180/s | True | True | **C4-trio-OK** |  |
| 0x004274d0 | LangIndexSeedFromCli | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x00493f80 | IntroVideoDimGetter | 1482 | 180/s | True | True | **C4-trio-OK** |  |
| 0x00493fc0 | AspectRatioGlobalGet | 2964 | 361/s | True | True | **C4-trio-OK** |  |
| 0x004c19f0 | RwVtableSlot07Call | 2964 | 361/s | True | True | **C4-trio-OK** |  |
| 0x004c1a00 | IntroSplashVtableSlot6 | 2964 | 361/s | True | True | **C4-trio-OK** |  |
| 0x004c1bb0 | IntroSplashRenderState | 1482 | 180/s | True | True | **C4-trio-OK** |  |
| 0x0042f0c0 | MenuSpriteDispatchA | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x0042fb70 | MenuSpriteDispatchB | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x0042fe90 | MenuSpriteDispatchC | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x00430760 | IsMultiplayerMode | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x004282a0 | MenuMenusBA | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x00427ad0 | MenuMenusBB | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x0042ac00 | MenuGroupCount | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x00473870 | TextSpriteUVExplicit | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x0042a940 | MenuTableSearch | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x00430830 | SplitScreenTrackAssignment | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x0040b460 | MenuMenusBD | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x0042d290 | MenusLapTimeFmt | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x0042d300 | TimeDiffDecompose | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x00429870 | LapTimeALessThanB | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x00429a30 | MenuMenusBE | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x00429a80 | LapLapsGetBySlot | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x00429a90 | LapSecsGetBySlot | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x00429a70 | LapFracGetBySlot | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x0040b7b0 | PlayerHotkeyTableGet | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x0040b6b0 | ModeScoreGetBySlot | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x00474e60 | DegToRad | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x00436810 | LocalPlayerSlotCheck | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x0042ee40 | FrontendModeDispatch | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x0042ef40 | VehicleUnlockFlagGet | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x00408a70 | FrontendC2RoundI | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x00408ad0 | RaceScoreFloatGetBySlot | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x00422fd0 | FrontendRaceResultsDispatch | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x00431d80 | TiebreakFlagGet | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x0046c700 | EntityScoreFieldAdd | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x0042b9e0 | FUN_0042b9e0 | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x004323c0 | MenuCursorBack | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x0042ae10 | MenuReadinessCheckA | 1482 | 180/s | True | True | **C4-trio-OK** |  |
| 0x0042aeb0 | MenuReadinessCheckB | 1482 | 180/s | True | True | **C4-trio-OK** |  |
| 0x0042af50 | MenuReadinessCheckC | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x00430910 | MenuOptionSlotGet | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x0040b810 | TimerGlobalsReset | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x0042f6b0 | MenuModeSync | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x004307a0 | ElapsedVsThresholdCheck | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x004309b0 | FrontendModeIndex | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x00430b60 | MenuSlotCount | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x00492340 | CarSlotInit | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x00414120 | CopyTable005f2a70To0089a384 | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x00422b30 | TimerArrayClear | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x0046dc00 | EntityFieldSet | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x00494f30 | AspectRatioSnapshot | 1481 | 180/s | True | True | **C4-trio-OK** |  |
| 0x0040e480 | CarSlotStateSet | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x00431d90 | FrontendPanelFlagAdvance | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x00431f30 | FrontendPageIdDispatch | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x00427c90 | GetDat0067d84c | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x00428390 | FrontendStateSet | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x0040d250 | DoubleDerefIndexedGetter | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x0042f0b0 | GetFrameCounterPlus73 | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x00401570 | EntryTableScanByKey | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x00426cf0 | GetDat0066d6e4 | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x0045ba00 | RaceResultIndexedStore | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x0046c5c0 | VehicleSlotInit | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x0046c790 | VehicleSlotFieldSet | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x004c75e0 | ViewportOriginGetter | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x0042d3e0 | MenuEntryArrayInit | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x0042ed70 | MenusLapTimeCmp | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x0042f7b0 | FrontendCursorUpdate | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x0042fa00 | PlayerSlotEdgeAdjust | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x0040e470 | CarSlotStateGet | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x0041e080 | ScoreboardStateZeroInit | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x00422a80 | SlotBlockZero | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x00422aa0 | SlotFieldSet | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x00423040 | FrontendDirInput | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x00423270 | TabCycler | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x00423320 | CursorMover | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x00423b40 | PlayerScoreAccA | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x00423b60 | PlayerScoreAccB | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x00423b80 | PlayerScoreAccC | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x00423ba0 | PlayerScoreAccD | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x00423bc0 | PlayerScoreTeamAccC | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x00423c40 | PlayerScoreTeamAccBase | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x00423cc0 | PlayerScoreTeamAccGatedB | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x00423d50 | PlayerScoreTeamAccBaseB | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x00423dd0 | PlayerScoreTeamAccGatedC | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x00423e60 | PlayerScoreTeamAccCumB | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x00423ee0 | PlayerBlock2Field00Get | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x00423f60 | PlayerBlock2Field04Get | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x00423ff0 | PlayerBlock2Field50Get | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x00424070 | PlayerBlock2Field08Get | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x00424100 | TeamBlockZeroGet | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x004241b0 | GetDat008994c0 | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x00424920 | EndOfRoundAccumulator | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x00425b70 | SlotFieldSetter | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x00425ee0 | SlotWordPtrGet | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x00425ef0 | ActiveSlotCount | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x00426020 | GlobalDat00646e58Get | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x00426080 | GlobalDat00656ed8Get | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x00426090 | GlobalDat0066ce58Get | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x004260a0 | GetDat00657438 | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x004260b0 | GetDat0065743c | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x00426bc0 | GetDat0066d6e0 | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x00426bd0 | GetTableEntry0066d658 | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x00426d00 | FrontendArraySlotGet | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x00426dc0 | FrontendRaycastForward | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x0042a980 | MenuTableSearchAlt | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x0042a9c0 | ModeCodeLookup | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x0042b920 | ConstantGetter22 | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x0042bde0 | HudRectEmitter5 | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x0042bfb0 | MenuStateParamStore | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x00431b70 | MenuFlagDat007f0f10Get | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x00432ad0 | MenuDimOverlayFadeStep | 0 | 0/s | True | True | HOLD | not-exercised |
| 0x0043aee0 | MenuSlotFlagSetCurrent | 0 | 0/s | True | True | HOLD | not-exercised |
