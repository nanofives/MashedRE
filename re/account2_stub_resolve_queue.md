# account2 Lane-A queue — stubs whose stubbed call site is already implemented

Regenerated 2026-07-23. **Corrected**: joins on STUBS.md **col-2** (the stubbed call
site RVA), not col-3 (col-3's leading RVA is the *parent* function in some rows —
the format is inconsistent). Each row below: the stubbed callee is already
`impl/ported/verified` in hooks.csv, so the stub is a **candidate for resolution**.

## ⚠️ Rewire lane — reliability caveats (learned 2026-07-23)

The **reliable** signal for a rewire is *source truth*: a live trampoline
(`reinterpret_cast<Fn>(0xRVA)(...)`) whose target is now `impl/ported` in hooks.csv.
The STUBS col-position join below OVER-counts and must not be trusted alone. Four traps:

1. **STUBS.md mixes column order** — some rows `| S | callee | parent |`, others
   `| S | parent | callee |`. Don't infer the callee from column position.
2. **Commented-out `RH_ScopedInstall` lines** match naive container-detection regex →
   wrong container attribution. Exclude commented lines.
3. **In-source confidence comments are stale** (e.g. `ScreenHeightGet C3` when hooks.csv
   says C4). Trust hooks.csv, never the inline `// FUN_x  Name  C?` comment.
4. **VERBATIM-THUNK TUs must be EXCLUDED.** Files like `VehicleCouplingBridge.cpp`,
   `PhysicsChainHooks.cpp`, `RwpSolverCore*.cpp`, `BatchAA_s1.cpp`,
   `VehicleVelocityWorldGet.cpp` dispatch *every* callee through its real RVA **by design**
   — the B5c/B5d inline-JMP routing makes those RVA calls the bit-identity C4 harness.
   Rewiring them to direct calls BREAKS the verbatim diff. Detect + skip via header markers:
   `verbatim | thunk | routes into our ports | dispatched to its real RVA | inline-JMP | B5c | B5d | bit-identit`.

**Safe rewire = C4 callee, container <C4, 0 verbatim-markers, per-file verified** (as done
for `IntroSplash.cpp` and `SpriteCluster.cpp`, 2026-07-23). Genuine-scaffold candidates still
pending per-file verification: `Frontend/BatchAA_s3`, `Frontend/Cluster_v3`,
`Frontend/FrontendLeaves_ad1`, `Frontend/MenuMenusB`, `Gameplay/Thresholds_ah4`,
`Render/D3D9Helpers_q5`, `Render/RwStreamWriteBytes`.

## Stale/rewire stub strikes

Two resolution types (must read the caller source to tell them apart):
- **stale** — the reimplementation already calls the ported symbol directly → strike via re-classify (no code change).
- **rewire** — caller still trampolines to the original RVA → point it at the ported symbol, build, then strike.

**378 resolvable stub rows** across 279 unique call sites.

| subsystem | resolvable stubs |
|---|---:|
| render | 87 |
| frontend | 70 |
| util | 62 |
| audio | 31 |
| vehicle | 30 |
| gameplay | 25 |
| hud | 13 |
| boot | 11 |
| third-party-library[RenderWare-Physics-3.7] | 11 |
| save | 9 |
| ai | 9 |
| input | 9 |
| particle | 6 |
| third-party-library[renderware] | 3 |
| camera | 1 |
| physics | 1 |

## Rows

| stub | subsystem | col2 RVA | target status/conf | target name |
|---|---|---|---|---|
| S-0400 | ai | 00416a30 | impl/C3 | FUN_00416a30 |
| S-0401 | ai | 00416250 | impl/C3 | FUN_00416250 |
| S-0428 | ai | 00423480 | impl/C3 | AiFilenameBuild |
| S-1842 | ai | 0040e350 | impl/C3 | GetRenderSubMode |
| S-2600 | ai | 00429840 | impl/C3 | RaceStateLatchSet |
| S-2603 | ai | 00429860 | impl/C3 | RaceStateFlagGet |
| S-2605 | ai | 0041da90 | impl/C3 | DeltaTimeOutGet |
| S-3208 | ai | 00423480 | impl/C3 | AiFilenameBuild |
| S-3660 | ai | 00429860 | impl/C3 | RaceStateFlagGet |
| S-0340 | audio | 005a9e10 | impl/C3 | AudioSubStructTwoCallInit |
| S-0341 | audio | 005aee20 | verified/C4 | AudioBitScanForward |
| S-0625 | audio | 005baf00 | impl/C3 | MusicGroupVolumeSet |
| S-0627 | audio | 0042f760 | impl/C3 | Global67f19cGet |
| S-0628 | audio | 0042f770 | impl/C3 | Global67f1a0Get |
| S-0629 | audio | 0042f780 | impl/C3 | Global67f1a4Get |
| S-0630 | audio | 00432230 | impl/C3 | RecEq231 |
| S-0631 | audio | 00432260 | impl/C3 | RecEq232 |
| S-0903 | audio | 005a7af0 | impl/C3 | FUN_005a7af0 |
| S-1363 | audio | 005baf40 | impl/C3 | AudioRendererField3cSet |
| S-1369 | audio | 005aeea0 | impl/C3 | AudioSemaphoreCreate |
| S-1370 | audio | 005aef00 | impl/C3 | AudioThreadDescInit |
| S-1720 | audio | 005aa1e0 | impl/C3 | FUN_005aa1e0 |
| S-3565 | audio | 005aad40 | impl/C3 | FUN_005aad40 |
| S-3566 | audio | 005aa030 | impl/C3 | FUN_005aa030 |
| S-3688 | audio | 005be190 | impl/C3 | AudioRwsSubZeroInit |
| S-5500 | audio | 005aeda0 | impl/C3 | AudioShiftAddMul64 |
| S-5501 | audio | 005ab370 | impl/C3 | FUN_005ab370 |
| S-5508 | audio | 005ae030 | impl/C3 | AudioSubStructCleanup |
| S-5510 | audio | 005adf30 | impl/C3 | AudioFmtKeyCompare |
| S-5511 | audio | 005ac9e0 | impl/C3 | AudioFmtEntryMatch |
| S-5512 | audio | 005ac980 | impl/C3 | AudioFmtDescCopy |
| S-5513 | audio | 005ac5f0 | impl/C3 | AudioFmtDescEqual |
| S-5515 | audio | 005ac740 | impl/C3 | AudioSubStructBufCleanup |
| S-5518 | audio | 005a9e10 | impl/C3 | AudioSubStructTwoCallInit |
| S-5519 | audio | 005aee20 | verified/C4 | AudioBitScanForward |
| S-5522 | audio | 005b0f10 | impl/C3 | Init5b0f10 |
| S-5526 | audio | 005b0ec0 | impl/C3 | FUN_005b0ec0 |
| S-5528 | audio | 005b0f90 | impl/C3 | FUN_005b0f90 |
| S-5529 | audio | 005b11d0 | impl/C3 | FUN_005b11d0 |
| S-5530 | audio | 005aeea0 | impl/C3 | AudioSemaphoreCreate |
| S-0001 | boot | 00499730 | impl/C3 | BootPtr773818Get |
| S-0030 | boot | 004a5984 | impl/C2 | CrtSehProlog |
| S-0031 | boot | 004a59bf | impl/C2 | CrtSehEpilog |
| S-0038 | boot | 004a3440 | impl/C3 | CrtStackProbe |
| S-0326 | boot | 004a332b | impl/C3 | CrtExitNoReturn_j5 |
| S-1480 | boot | 005c4d30 | impl/C3 | CondGet5c4d30 |
| S-3696 | boot | 005c4d30 | impl/C3 | CondGet5c4d30 |
| S-3927 | boot | 005c4d30 | impl/C3 | CondGet5c4d30 |
| S-4062 | boot | 0040bb30 | impl/C3 | FUN_0040bb30 |
| S-4101 | boot | 0040bb30 | impl/C3 | FUN_0040bb30 |
| S-4327 | boot | 005c4d30 | impl/C3 | CondGet5c4d30 |
| S-1423 | camera | 00426bb0 | impl/C3 | CameraPath::GetCount |
| S-0172 | frontend | 004c1bb0 | impl/C3 | IntroSplashRenderState |
| S-0173 | frontend | 004c1a00 | impl/C3 | IntroSplashVtableSlot6 |
| S-0174 | frontend | 004c19f0 | impl/C3 | RwVtableSlot07Call |
| S-0178 | frontend | 004c5c00 | impl/C3 | FUN_004c5c00 |
| S-0380 | frontend | 0046c5c0 | impl/C3 | VehicleSlotInit |
| S-0381 | frontend | 0046c790 | impl/C3 | VehicleSlotFieldSet |
| S-0382 | frontend | 004215c0 | impl/C3 | FUN_004215c0 |
| S-0383 | frontend | 0045ba00 | impl/C3 | RaceResultIndexedStore |
| S-0431 | frontend | 00425b70 | impl/C3 | SlotFieldSetter |
| S-0454 | frontend | 00473ee0 | verified/C4 | LogoOverlayDraw |
| S-0493 | frontend | 0040e470 | impl/C3 | CarSlotStateGet |
| S-0549 | frontend | 0042b930 | verified/C4 | MenuAlphaGet |
| S-0801 | frontend | 00493f70 | impl/C4 | VideoStateFlagGet |
| S-0802 | frontend | 00493f80 | impl/C4 | IntroVideoDimGetter |
| S-0803 | frontend | 00493fc0 | impl/C4 | AspectRatioGlobalGet |
| S-0810 | frontend | 004c1a00 | impl/C3 | IntroSplashVtableSlot6 |
| S-0811 | frontend | 004c1bb0 | impl/C3 | IntroSplashRenderState |
| S-0814 | frontend | 004c75e0 | impl/C3 | ViewportOriginGetter |
| S-1003 | frontend | 00424920 | impl/C3 | EndOfRoundAccumulator |
| S-1043 | frontend | 00424920 | impl/C3 | EndOfRoundAccumulator |
| S-1135 | frontend | 0042ae10 | impl/C4 | MenuReadinessCheckA |
| S-1136 | frontend | 0042aeb0 | impl/C3 | MenuReadinessCheckB |
| S-1138 | frontend | 004298c0 | impl/C3 | Clear67d99c_x4 |
| S-1641 | frontend | 0042f7b0 | impl/C3 | FrontendCursorUpdate |
| S-1643 | frontend | 0042fa00 | impl/C3 | PlayerSlotEdgeAdjust |
| S-1644 | frontend | 00431f30 | impl/C3 | FrontendPageIdDispatch |
| S-1646 | frontend | 0042ad90 | impl/C3 | FUN_0042ad90 |
| S-1649 | frontend | 00432b30 | verified/C4 | MenuPromptStrip |
| S-2341 | frontend | 004d8c40 | impl/C3 | FUN_004d8c40 |
| S-2541 | frontend | 00401570 | impl/C3 | EntryTableScanByKey |
| S-2544 | frontend | 00426cf0 | impl/C3 | GetDat0066d6e4 |
| S-3211 | frontend | 00425b70 | impl/C3 | SlotFieldSetter |
| S-3420 | frontend | 0042bfb0 | impl/C3 | MenuStateParamStore |
| S-3564 | frontend | 00423b40 | impl/C3 | PlayerScoreAccA |
| S-3610 | frontend | 004260b0 | impl/C3 | GetDat0065743c |
| S-3611 | frontend | 004260a0 | impl/C3 | GetDat00657438 |
| S-3616 | frontend | 00432ad0 | impl/C3 | MenuDimOverlayFadeStep |
| S-3628 | frontend | 00426cf0 | impl/C3 | GetDat0066d6e4 |
| S-3651 | frontend | 0041e080 | impl/C3 | ScoreboardStateZeroInit |
| S-3657 | frontend | 00429b30 | impl/C3 | FUN_00429b30 |
| S-3658 | frontend | 004a2c48 | impl/C3 | FUN_004a2c48 |
| S-3661 | frontend | 0043aee0 | impl/C3 | MenuSlotFlagSetCurrent |
| S-3667 | frontend | 00431d80 | impl/C3 | TiebreakFlagGet |
| S-3668 | frontend | 0042fe80 | verified/C4 | GetRaceEndFlag |
| S-3671 | frontend | 0040e470 | impl/C3 | CarSlotStateGet |
| S-3815 | frontend | 00493f80 | impl/C4 | IntroVideoDimGetter |
| S-3816 | frontend | 00492d20 | impl/C3 | IntroSplashFrameTickShim |
| S-3818 | frontend | 004c1bb0 | impl/C3 | IntroSplashRenderState |
| S-3819 | frontend | 004c1a00 | impl/C3 | IntroSplashVtableSlot6 |
| S-3830 | frontend | 0042b930 | verified/C4 | MenuAlphaGet |
| S-3910 | frontend | 00473870 | impl/C3 | TextSpriteUVExplicit |
| S-3926 | frontend | 004a2c48 | impl/C3 | FUN_004a2c48 |
| S-3936 | frontend | 004c75e0 | impl/C3 | ViewportOriginGetter |
| S-3999 | frontend | 004282a0 | impl/C3 | MenuMenusBA |
| S-4000 | frontend | 0042aae0 | impl/C4 | MenuIm2DQuad |
| S-4004 | frontend | 004c19f0 | impl/C3 | RwVtableSlot07Call |
| S-4005 | frontend | 004c1a00 | impl/C3 | IntroSplashVtableSlot6 |
| S-4006 | frontend | 004a2c48 | impl/C3 | FUN_004a2c48 |
| S-4009 | frontend | 0042ae10 | impl/C4 | MenuReadinessCheckA |
| S-4010 | frontend | 0042aeb0 | impl/C3 | MenuReadinessCheckB |
| S-4011 | frontend | 0042af50 | impl/C3 | MenuReadinessCheckC |
| S-4017 | frontend | 004298c0 | impl/C3 | Clear67d99c_x4 |
| S-4019 | frontend | 0040b810 | impl/C3 | TimerGlobalsReset |
| S-4021 | frontend | 00422b30 | impl/C3 | TimerArrayClear |
| S-4022 | frontend | 0046dc00 | impl/C3 | EntityFieldSet |
| S-4027 | frontend | 004a2c48 | impl/C3 | FUN_004a2c48 |
| S-4028 | frontend | 004893d0 | impl/C3 | FUN_004893d0 |
| S-4060 | frontend | 004a2c48 | impl/C3 | FUN_004a2c48 |
| S-4106 | frontend | 004a2c48 | impl/C3 | FUN_004a2c48 |
| S-4247 | frontend | 004c5c00 | impl/C3 | FUN_004c5c00 |
| S-0352 | gameplay | 004725f0 | impl/C3 | FUN_004725f0 |
| S-0354 | gameplay | 00405400 | impl/C3 | Clear639d70x3 |
| S-0540 | gameplay | 0046d320 | impl/C3 | Idx2Wheel881790Get |
| S-0541 | gameplay | 0046d360 | impl/C3 | Idx2Wheel881738Get |
| S-0553 | gameplay | 0047ce80 | impl/C3 | Table6c9758Get |
| S-0554 | gameplay | 0047ce00 | impl/C3 | Table6c9438Get |
| S-1865 | gameplay | 00458f80 | impl/C3 | FUN_00458f80 |
| S-3120 | gameplay | 00412130 | impl/C3 | FUN_00412130 |
| S-3427 | gameplay | 0040ce80 | impl/C3 | PtrTable5f2770Get |
| S-3592 | gameplay | 004725f0 | impl/C3 | FUN_004725f0 |
| S-3593 | gameplay | 00405400 | impl/C3 | Clear639d70x3 |
| S-3603 | gameplay | 0041f330 | impl/C3 | FUN_0041f330 |
| S-3614 | gameplay | 0041ea70 | impl/C4 | GlobalField63d7e4_3c |
| S-3619 | gameplay | 0041e140 | impl/C3 | Global63d7e0Get |
| S-3626 | gameplay | 004173a0 | impl/C3 | Float89a360Get |
| S-3629 | gameplay | 00406370 | impl/C3 | Clear10x3_63a494 |
| S-3648 | gameplay | 00421080 | impl/C3 | Fill63e5a4 |
| S-3666 | gameplay | 00413fa0 | impl/C3 | FUN_00413fa0 |
| S-3676 | gameplay | 004190f0 | impl/C3 | ClearEax4190f0 |
| S-3677 | gameplay | 0040b890 | impl/C3 | FUN_0040b890 |
| S-3717 | gameplay | 0041a8b0 | impl/C3 | GhostSlotSet63c6f0 |
| S-3721 | gameplay | 0041a4a0 | impl/C3 | FUN_0041a4a0 |
| S-3832 | gameplay | 00471530 | impl/C3 | ClearTable471530 |
| S-4029 | gameplay | 00489450 | impl/C3 | ContRecSet450 |
| S-4030 | gameplay | 00489480 | impl/C3 | ContRecSet480 |
| S-0920 | hud | 00450b10 | impl/C3 | HudIm2DQuad |
| S-1480 | hud | 00552d10 | impl/C3 | FontMatrix_Push |
| S-1480 | hud | 00552df0 | verified/C4 | FontCtx_SetTranslation |
| S-1480 | hud | 00552da0 | impl/C3 | FontCtx_SetScale |
| S-1485 | hud | 00557110 | impl/C3 | FUN_00557110 |
| S-3125 | hud | 00427780 | verified/C4 | FontText_StringTableLookup |
| S-3129 | hud | 00427840 | verified/C4 | FontText_UTF16WidenCopy |
| S-3928 | hud | 00428610 | impl/C3 | ViewportScaledRectDraw |
| S-4238 | hud | 00427840 | verified/C4 | FontText_UTF16WidenCopy |
| S-4239 | hud | 00552d10 | impl/C3 | FontMatrix_Push |
| S-4362 | hud | 00557110 | impl/C3 | FUN_00557110 |
| S-4398 | hud | 0042b8b0 | impl/C3 | ScreenWidthGet |
| S-4399 | hud | 0042b8c0 | impl/C4 | ScreenHeightGet |
| S-0494 | input | 00497450 | impl/C3 | Pred497450 |
| S-1442 | input | 004b64e0 | impl/C3 | FUN_004b64e0 |
| S-2410 | input | 004b64e0 | impl/C3 | FUN_004b64e0 |
| S-3833 | input | 00495520 | impl/C3 | Get771e78 |
| S-3835 | input | 00495790 | impl/C3 | Global772facGet |
| S-3920 | input | 00495520 | impl/C3 | Get771e78 |
| S-3969 | input | 00495520 | impl/C3 | Get771e78 |
| S-4057 | input | 004b6480 | impl/C3 | BitArrayClear |
| S-4097 | input | 004b6480 | impl/C3 | BitArrayClear |
| S-0316 | particle | 00489290 | impl/C3 | FUN_00489290 |
| S-0317 | particle | 0048a460 | impl/C3 | StridedClear2_709238 |
| S-0318 | particle | 0048a830 | impl/C3 | FUN_0048a830 |
| S-3642 | particle | 0048ade0 | impl/C3 | FUN_0048ade0 |
| S-3643 | particle | 00487df0 | impl/C4 | Set703058_0 |
| S-3644 | particle | 00486f90 | impl/C3 | FUN_00486f90 |
| S-2643 | physics | 0047ce40 | impl/C3 | Search47ce40 |
| S-0060 | render | 004cbc60 | impl/C3 | RwGlobal7d4598Set |
| S-0061 | render | 004cbc70 | impl/C3 | RwGlobal7d4598Get |
| S-0062 | render | 004cbc80 | impl/C3 | RwGlobal7d459cSet |
| S-0063 | render | 004cbc90 | impl/C3 | RwGlobal7d459cGet |
| S-0209 | render | 004c2f30 | impl/C3 | RwEngineSetVideoMode |
| S-0220 | render | 004d7ff0 | impl/C3 | RwIdentityPassthrough |
| S-0221 | render | 004d8480 | impl/C3 | RwErrSlotWrite |
| S-0282 | render | 004cbd30 | impl/C3 | RwStreamRead |
| S-0327 | render | 004c5bc0 | impl/C3 | FUN_004c5bc0 |
| S-0328 | render | 004c5c80 | impl/C3 | FUN_004c5c80 |
| S-0335 | render | 0045b350 | impl/C3 | RwInitNullStub |
| S-0367 | render | 00418a00 | impl/C3 | FUN_00418a00 |
| S-0423 | render | 004cbd30 | impl/C3 | RwStreamRead |
| S-0427 | render | 0047cdc0 | impl/C3 | StoreDistSq47cdc0 |
| S-0446 | render | 00552d70 | impl/C3 | ViewportStackPop |
| S-0546 | render | 004c4d20 | verified/C4 | RwMatrixRotate |
| S-0548 | render | 004c39b0 | verified/C4 | RwV3dNormalize |
| S-0550 | render | 0042f510 | impl/C3 | Vehicle0HandleGet |
| S-0704 | render | 004d5480 | impl/C3 | FUN_004d5480 |
| S-0705 | render | 004d54f0 | impl/C3 | FUN_004d54f0 |
| S-0706 | render | 004d5570 | impl/C3 | D3d9State_SetSampler |
| S-0707 | render | 004d53b0 | impl/C3 | D3d9State_Flush |
| S-0826 | render | 004c2f30 | impl/C3 | RwEngineSetVideoMode |
| S-0827 | render | 004c2ed0 | impl/C3 | RwEngineGetModeInfo |
| S-1421 | render | 004c3bf0 | verified/C4 | Vec2Length |
| S-1422 | render | 004c39b0 | verified/C4 | RwV3dNormalize |
| S-1440 | render | 004c0e50 | impl/C3 | FUN_004c0e50 |
| S-1483 | render | 004c4a50 | verified/C4 | RwMatrixRotateInner |
| S-1701 | render | 004e4350 | impl/C3 | FUN_004e4350 |
| S-1702 | render | 0041ea10 | impl/C3 | GlobalField63d7e4_24 |
| S-1703 | render | 0041e8f0 | impl/C3 | TrackNodeDispatch24 |
| S-1705 | render | 00403d30 | impl/C3 | Render_00403d30 |
| S-2122 | render | 004cd140 | impl/C3 | RwRenderCommandBufferReset |
| S-2127 | render | 00552d70 | impl/C3 | ViewportStackPop |
| S-2484 | render | 004cbd30 | impl/C3 | RwStreamRead |
| S-2485 | render | 004d7ff0 | impl/C3 | RwIdentityPassthrough |
| S-2486 | render | 004d8480 | impl/C3 | RwErrSlotWrite |
| S-2623 | render | 004c39b0 | verified/C4 | RwV3dNormalize |
| S-2662 | render | 004c5830 | impl/C3 | FUN_004c5830 |
| S-2663 | render | 004c5850 | impl/C3 | FUN_004c5850 |
| S-3203 | render | 004cbd30 | impl/C3 | RwStreamRead |
| S-3207 | render | 0047cdc0 | impl/C3 | StoreDistSq47cdc0 |
| S-3577 | render | 0045b350 | impl/C3 | RwInitNullStub |
| S-3605 | render | 004c5c80 | impl/C3 | FUN_004c5c80 |
| S-3649 | render | 0041d910 | impl/C3 | Clear63d584Pair |
| S-3652 | render | 0041b520 | impl/C3 | FUN_0041b520 |
| S-3675 | render | 00418e50 | impl/C3 | Fill64_63c508 |
| S-3680 | render | 0041b720 | impl/C3 | FUN_0041b720 |
| S-3681 | render | 0041b770 | impl/C3 | FUN_0041b770 |
| S-3691 | render | 004c5ca0 | impl/C3 | FUN_004c5ca0 |
| S-3731 | render | 004c4d20 | verified/C4 | RwMatrixRotate |
| S-3732 | render | 004c45f0 | impl/C3 | FUN_004c45f0 |
| S-3748 | render | 004d7ff0 | impl/C3 | RwIdentityPassthrough |
| S-3749 | render | 004d8480 | impl/C3 | RwErrSlotWrite |
| S-3771 | render | 004d40c0 | impl/C3 | GetPipelinePtr |
| S-3826 | render | 004b65c0 | impl/C3 | FUN_004b65c0 |
| S-3831 | render | 0042f510 | impl/C3 | Vehicle0HandleGet |
| S-3903 | render | 0042b890 | impl/C3 | RenderWidthSet |
| S-3904 | render | 0042b8a0 | impl/C3 | RenderHeightSet |
| S-3930 | render | 0045b350 | impl/C3 | RwInitNullStub |
| S-3958 | render | 004cbb60 | impl/C3 | GetRenderStateBlockPtr |
| S-3960 | render | 00498bf0 | impl/C4 | DisplayActiveFlagGet |
| S-3962 | render | 00499710 | impl/C3 | Global7e9584Get |
| S-3963 | render | 0045b350 | impl/C3 | RwInitNullStub |
| S-3982 | render | 0045b350 | impl/C3 | RwInitNullStub |
| S-4051 | render | 00474d80 | impl/C3 | ByteFlag474d80 |
| S-4052 | render | 004c39b0 | verified/C4 | RwV3dNormalize |
| S-4059 | render | 004c39b0 | verified/C4 | RwV3dNormalize |
| S-4074 | render | 004b52f0 | impl/C3 | FUN_004b52f0 |
| S-4085 | render | 004769a0 | impl/C3 | ParticleEmitter_SetPosition |
| S-4086 | render | 004769f0 | impl/C3 | ParticleEmitter_SetColour |
| S-4087 | render | 004769d0 | impl/C3 | ParticleEmitter_SetVelocity |
| S-4088 | render | 00476a30 | impl/C3 | ParticleEmitter_SetScalar |
| S-4089 | render | 00476d00 | impl/C3 | FUN_00476d00 |
| S-4091 | render | 004c51a0 | ported/C2 | RwMatrixTranslate |
| S-4092 | render | 004c4d20 | verified/C4 | RwMatrixRotate |
| S-4093 | render | 004c5010 | verified/C4 | RwMatrixScale |
| S-4094 | render | 00476a10 | impl/C3 | FUN_00476a10 |
| S-4108 | render | 004769a0 | impl/C3 | ParticleEmitter_SetPosition |
| S-4109 | render | 004769d0 | impl/C3 | ParticleEmitter_SetVelocity |
| S-4110 | render | 004769f0 | impl/C3 | ParticleEmitter_SetColour |
| S-4111 | render | 00476a30 | impl/C3 | ParticleEmitter_SetScalar |
| S-4112 | render | 00476d00 | impl/C3 | FUN_00476d00 |
| S-4243 | render | 00552d70 | impl/C3 | ViewportStackPop |
| S-4321 | render | 004c5c80 | impl/C3 | FUN_004c5c80 |
| S-4322 | render | 004c5ca0 | impl/C3 | FUN_004c5ca0 |
| S-5520 | render | 004d7ff0 | impl/C3 | RwIdentityPassthrough |
| S-0202 | save | 00498950 | impl/C4 | ConfigLoad |
| S-0280 | save | 00550b00 | impl/C4 | VfsFileExists |
| S-0284 | save | 004cbe80 | impl/C4 | RwStreamWrite |
| S-2320 | save | 00550980 | impl/C3 | VfsStreamRead |
| S-3656 | save | 004099a0 | impl/C4 | Save::AutosaveTrigger |
| S-3811 | save | 004963e0 | impl/C4 | ConfigLogError |
| S-3812 | save | 00496400 | impl/C4 | ConfigLogDebug |
| S-3961 | save | 00496400 | impl/C4 | ConfigLogDebug |
| S-4116 | save | 004b3b70 | impl/C4 | FileReadWrapper_i3 |
| S-1482 | third-party-library[RenderWare-Physics-3.7] | 0055deb0 | ported/C3 | RwpWorldSolverHandle |
| S-2487 | third-party-library[RenderWare-Physics-3.7] | 0055deb0 | ported/C3 | RwpWorldSolverHandle |
| S-3724 | third-party-library[RenderWare-Physics-3.7] | 0055e200 | ported/C3 | RwpSolverContextSet |
| S-3725 | third-party-library[RenderWare-Physics-3.7] | 0055fe50 | ported/C2 | FUN_0055fe50 |
| S-3727 | third-party-library[RenderWare-Physics-3.7] | 00561e60 | ported/C2 | FUN_00561e60 |
| S-3729 | third-party-library[RenderWare-Physics-3.7] | 00561390 | ported/C2 | FUN_00561390 |
| S-3733 | third-party-library[RenderWare-Physics-3.7] | 0055dff0 | ported/C3 | RwpBodyRefreshGate |
| S-3735 | third-party-library[RenderWare-Physics-3.7] | 0055ab30 | ported/C2 | FUN_0055ab30 |
| S-3745 | third-party-library[RenderWare-Physics-3.7] | 00564c80 | ported/C2 | FUN_00564c80 |
| S-3747 | third-party-library[RenderWare-Physics-3.7] | 00565200 | ported/C2 | FUN_00565200 |
| S-4278 | third-party-library[RenderWare-Physics-3.7] | 0055deb0 | ported/C3 | RwpWorldSolverHandle |
| S-3714 | third-party-library[renderware] | 00546bf0 | ported/C2 | FUN_00546bf0 |
| S-3715 | third-party-library[renderware] | 00546c50 | ported/C2 | FUN_00546c50 |
| S-3716 | third-party-library[renderware] | 00546cb0 | ported/C2 | FUN_00546cb0 |
| S-0067 | util | 005c9d00 | impl/C2 | GetRaceEndTrigger |
| S-0304 | util | 004b6520 | impl/C3 | ZeroFillWrapper |
| S-0343 | util | 0041f320 | impl/C3 | Car::GetState |
| S-0355 | util | 00484c90 | impl/C3 | FUN_00484c90 |
| S-0374 | util | 00493b40 | impl/C3 | Ret3 |
| S-0460 | util | 004b6520 | impl/C3 | ZeroFillWrapper |
| S-0481 | util | 0042c2d0 | verified/C4 | GetDat0067ecb4 |
| S-0482 | util | 0042c2e0 | verified/C4 | GetDat0067ecb8 |
| S-0483 | util | 0042c2f0 | verified/C4 | SetDat0067ecb8 |
| S-0484 | util | 0042f500 | verified/C4 | GetDat0067ea64 |
| S-0485 | util | 0042f6a0 | impl/C3 | GetRaceSubMode |
| S-0490 | util | 005c9d00 | impl/C2 | GetRaceEndTrigger |
| S-0624 | util | 00432290 | impl/C3 | Trigger432290 |
| S-0671 | util | 00496920 | impl/C3 | TimerTable772ffcGet |
| S-1120 | util | 0042b950 | impl/C3 | Set7f1a0c_1000 |
| S-1122 | util | 00472640 | impl/C3 | Set86ecc8 |
| S-1126 | util | 00426c30 | verified/C4 | TimerDispatch30 |
| S-1127 | util | 00426c70 | verified/C4 | TimerDispatch70 |
| S-1128 | util | 00422b10 | impl/C3 | TimerArrayZero |
| S-1129 | util | 00426c10 | verified/C4 | TimerDispatch10 |
| S-1139 | util | 00413f90 | verified/C4 | TimerGetBasePtr |
| S-1600 | util | 00409930 | verified/C4 | LoadingState3Enter |
| S-1602 | util | 0042c1a0 | impl/C3 | StateAdvance2to3 |
| S-1603 | util | 00405420 | impl/C3 | ReplayCursorReset |
| S-1604 | util | 00430820 | impl/C3 | Global67ed6cGet |
| S-1605 | util | 00430790 | impl/C3 | GetDat0067f17c |
| S-1606 | util | 0045c810 | impl/C3 | FUN_0045c810 |
| S-1650 | util | 00492d10 | impl/C3 | Global771968Get |
| S-1652 | util | 0042c1f0 | impl/C3 | FUN_0042c1f0 |
| S-2225 | util | 0042f790 | impl/C3 | GhostMode::IsActive |
| S-2380 | util | 0040e170 | impl/C3 | Set63ba7c |
| S-2501 | util | 004430a0 | impl/C3 | Util897fe0Set |
| S-2602 | util | 0041e130 | impl/C3 | TimerStateSet |
| S-2604 | util | 00430820 | impl/C3 | Global67ed6cGet |
| S-3424 | util | 0042c1a0 | impl/C3 | StateAdvance2to3 |
| S-3540 | util | 004938e0 | impl/C3 | FUN_004938e0 |
| S-3574 | util | 0042f6a0 | impl/C3 | GetRaceSubMode |
| S-3575 | util | 0041f320 | impl/C3 | Car::GetState |
| S-3576 | util | 0041efc0 | impl/C3 | Car::GetLapProgress |
| S-3594 | util | 00484c90 | impl/C3 | FUN_00484c90 |
| S-3621 | util | 004295a0 | verified/C4 | HudDualLabelRender |
| S-3633 | util | 0040e340 | impl/C4 | GetLiveCarCount |
| S-3636 | util | 0048f260 | impl/C3 | Clear48f260 |
| S-3637 | util | 0048f680 | impl/C3 | StridedClear76a100 |
| S-3639 | util | 00475a60 | verified/C4 | PendingOpQueueFlush |
| S-3650 | util | 0042f6a0 | impl/C3 | GetRaceSubMode |
| S-3654 | util | 00430820 | impl/C3 | Global67ed6cGet |
| S-3663 | util | 0040e460 | impl/C3 | Flag63b908Set |
| S-3665 | util | 0041eda0 | impl/C3 | SlotBitSet |
| S-3679 | util | 004b6520 | impl/C3 | ZeroFillWrapper |
| S-3821 | util | 004944b0 | impl/C3 | FUN_004944b0 |
| S-3934 | util | 004938e0 | impl/C3 | FUN_004938e0 |
| S-3968 | util | 004b6520 | impl/C3 | ZeroFillWrapper |
| S-4008 | util | 0042c150 | impl/C3 | FUN_0042c150 |
| S-4013 | util | 0042b950 | impl/C3 | Set7f1a0c_1000 |
| S-4016 | util | 0040e360 | impl/C3 | RaceMode::Set |
| S-4018 | util | 00429aa0 | verified/C4 | GameStateSlotsFill |
| S-4020 | util | 00413f90 | verified/C4 | TimerGetBasePtr |
| S-4023 | util | 00493570 | impl/C3 | Set77196c_1 |
| S-4024 | util | 00493580 | impl/C3 | Set771970_1 |
| S-4118 | util | 004b6520 | impl/C3 | ZeroFillWrapper |
| S-5517 | util | 005c9d00 | impl/C2 | GetRaceEndTrigger |
| S-0357 | vehicle | 004114e0 | impl/C3 | ReplayCleanup |
| S-0368 | vehicle | 00418a30 | impl/C3 | FUN_00418a30 |
| S-0480 | vehicle | 0042c280 | impl/C3 | FUN_0042c280 |
| S-0487 | vehicle | 004331a0 | impl/C3 | RaceFinalizeOnce |
| S-1000 | vehicle | 0042bf30 | impl/C3 | Post0042bf30 |
| S-1001 | vehicle | 0042d3a0 | impl/C3 | RaceTransitionBufZero |
| S-1002 | vehicle | 004248b0 | impl/C3 | PerCarSnapshotInit |
| S-1040 | vehicle | 0042bf30 | impl/C3 | Post0042bf30 |
| S-1041 | vehicle | 0042d3a0 | impl/C3 | RaceTransitionBufZero |
| S-1042 | vehicle | 004248b0 | impl/C3 | PerCarSnapshotInit |
| S-1066 | vehicle | 0042c280 | impl/C3 | FUN_0042c280 |
| S-1068 | vehicle | 004331a0 | impl/C3 | RaceFinalizeOnce |
| S-1137 | vehicle | 0042bf30 | impl/C3 | Post0042bf30 |
| S-1840 | vehicle | 0046cbb0 | impl/C4 | CarStatePairGet |
| S-1841 | vehicle | 004922e0 | impl/C3 | CarEventTrigger |
| S-1860 | vehicle | 0046cbb0 | impl/C4 | CarStatePairGet |
| S-1861 | vehicle | 00405890 | impl/C4 | Pred405890 |
| S-2606 | vehicle | 00411d60 | impl/C3 | ReplayCheckTimer |
| S-2624 | vehicle | 00413c70 | impl/C3 | FUN_00413c70 |
| S-3580 | vehicle | 00404e00 | impl/C3 | IdxPtr404e00 |
| S-3581 | vehicle | 00404e20 | impl/C3 | IdxPtr404e20 |
| S-3653 | vehicle | 004117b0 | impl/C3 | ReplaySave |
| S-3655 | vehicle | 00483ca0 | impl/C3 | FUN_00483ca0 |
| S-3673 | vehicle | 00418a30 | impl/C3 | FUN_00418a30 |
| S-3980 | vehicle | 004840f0 | impl/C3 | FUN_004840f0 |
| S-3991 | vehicle | 00453f30 | impl/C3 | Fill6870b4 |
| S-4014 | vehicle | 0042bf30 | impl/C3 | Post0042bf30 |
| S-4046 | vehicle | 004b5240 | impl/C3 | FUN_004b5240 |
| S-4103 | vehicle | 00476cb0 | impl/C3 | Batch476cb0 |
| S-4119 | vehicle | 004b65a0 | impl/C3 | Global7d3e4cGetThunk |
