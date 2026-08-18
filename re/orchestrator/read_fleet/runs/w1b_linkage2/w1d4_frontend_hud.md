I've now read every in-scope file, confirmed the build sets, established there are zero load-time-deref (class-C) constructs anywhere in Frontend/ or HUD/, and resolved the cross-TU externs. Here is the complete triage.

---

# D0.7 Triage Part 4 ÔÇö Frontend/ and HUD/ unlinked files

## Scope determination (from build.bat exe list + asi_sources.rsp)

- **Frontend/**: 67 `.cpp`, **47 already on the exe list** ÔåÆ **20 unlinked** in scope.
- **HUD/**: 10 `.cpp`, **6 already on the exe list** ÔåÆ **4 unlinked** in scope.
- **Total in scope: 24 files.**

**Confirmed "in NEITHER target":** `Frontend\MixedC3Sweep.cpp` and `Frontend\HudFrontendDispatchers_t4.cpp`. Neither is in build.bat's exe list; neither is in `asi_sources.rsp` (only `Audio\MixedC3Sweep.cpp` at rsp:155 ÔÇö a different file ÔÇö and `HudFrontendDispatchers_t4` appears nowhere). Both are intentionally-empty translation units (all-comment, zero symbols, zero `RH_ScopedInstall`). See `MixedC3Sweep.cpp:36-37` ("intentionally empty of implementationsÔÇª not compiled into the ASI") and `HudFrontendDispatchers_t4.cpp:104-105`.

## Class-C finding: ZERO

There is **no file-scope (column-0) initializer that dereferences an absolute address anywhere in Frontend/ or HUD/**. Grep for `^[A-Za-z_].*=\s*\*reinterpret_cast` and `^[A-Za-z_].*=\s*\*\(` across both directories returned **no matches**. Every `*reinterpret_cast<ÔÇª>(0x00ÔÇªÔÇª)` in these 24 files sits **inside a function body** (load-safe; faults only if called), never in a load-time initializer. This matches part 1/2: the only class-C offenders in the whole project are `Audio\AudioDSound.cpp:95-96` and `Audio\AudioRws.cpp:477-490`.

## (1) Per-file table

`RVA-free` = no MASHED address (0x00400000ÔÇô0x00b60000) in any non-comment code path. Count column = total address occurrences in the file (code + comment); for every class-B file I confirmed by reading that the code paths themselves carry absolute addresses, so all are `NO`.

| File | Class | RVA-free? | conf. | reached-by-name? | blocking construct / notes | cross-TU deps |
|---|---|---|---|---|---|---|
| **Frontend/ (20)** | | | | | | |
| MixedC3Sweep.cpp | **A** | n/a (empty TU; 5 addrs all in comments) | ÔÇö | no | Neither target; no symbols, no installs | none |
| HudFrontendDispatchers_t4.cpp | **A** | n/a (empty TU; 33 addrs all in comments) | ÔÇö | no | Neither target; no symbols, no installs | none |
| MenuAnimTickTwin.cpp | **A** | NO (19) | C3-twin | no | Pure diff-original harness: operates on MASHED live table 0x00898ac0, calls orig RVAs. Standalone port = MenuNavSM.cpp Anim_Tick (linked) | none |
| LogoOverlayTwin.cpp | **A** | NO (7) | C3-twin | no | Harness twin of FUN_00473ee0; standalone port = DrawQuadPrimitives.cpp LogoOverlayDraw (linked) | calls `LogoOverlayDraw` (extern, defined in linked DrawQuadPrimitives.cpp) |
| MenuDrawLoopTwin.cpp | **A** | NO (51) | C3-twin | no | Harness twin of FUN_0043c5b0; live-memory + orig-RVA calls | none |
| PromptStripTwin.cpp | **A** | NO (12) | C3-twin | no | Harness twin of FUN_00432b30; naked thunk + inline asm to orig | none |
| RaceResults.cpp | **B** | NO (24) | C3 | no | Tunnelled: `0x005f2770` double-deref (L35,61,90) | none (self-contained) |
| SpriteDispatch.cpp | **B** | NO (46) | C3 | no | Tunnelled: slot table 0x005cd838-898 (L60-71), 0x0067f17c/0x005f79d8 (L104-106) | calls FUN_0040bb90/70 via RVA |
| SlotZeroers_s2.cpp | **B** | NO (76) | C3 | no | Tunnelled: 0x00899a94 (L61), 0x007f1a18 (L127) | externs `GetRaceSubMode`(L44)ÔåÆ**linked** GameStateGetters.cpp; `GetDat0067ea64`(L45)ÔåÆ**linked** StateAccessors.cpp |
| SplashGameMode_t5.cpp | **B** | NO (89) | C3 (+2 impl-only) | no | Tunnelled: 0x00771a04 (L62), 0x008820b0 (L153) | none (defines VideoStateFlagGet/AspectRatioGlobalGet) |
| SkeletonAndScatter_t6.cpp | **B** | NO (135) | C3 (+deferred) | no | Tunnelled: 0x007719e8 (L105), 0x00771964+0x007d3ff8 (L316,325) | callees via RVA only |
| BatchAA_s2.cpp | **B** | NO (20) | C3 | no | Tunnelled: 0x00899f74 (L46) | externs `GetRaceSubMode`(L29)ÔåÆ**linked** |
| BatchAA_s5.cpp | **B** | NO (32) | C3 | no | Tunnelled: 0x005f6748/54 (L71,84) | calls FUN_0040ce80 via RVA |
| MenuMenusB.cpp | **B** | NO (131) | C3 (1 disabled) | no | Tunnelled: 0x0066d828 (L125), 0x008a94f0 (L403) | callees via RVA only |
| MenuMenusMixed.cpp | **B** | NO (29) | C3 | no | Tunnelled: 0x005f6748/50/54 (L66,79,85) | calls FUN_0040ce80 via RVA |
| RaceArmReset.cpp | **B** | NO (40) | C3 | no | Tunnelled: gate 0x0067eca4 (L64,67), calls 0x00402f80/0x0042d3a0 (L69,70) | callees via RVA only |
| FrontendLeaves_ad1.cpp | **B** | NO (35) | C3 | no | Tunnelled: 0x00899e80 (L78), 0x007f1a18 (L83) | externs `GetDat0067ea64`(L70)ÔåÆ**linked** StateAccessors.cpp |
| SpriteCluster.cpp | **B** | NO (173) | C3 (callees C4) | no | Tunnelled: DAT_0063b8fc, 0x00472c60 etc. | externs `ScreenHeightGet`ÔåÆ**linked**, `GetDat0067ea64`ÔåÆ**linked**, `SpriteSlotGate`/`HudSlotTypePlayer0-2`ÔåÆ**linked** SpriteGate.cpp |
| MenuMixed.cpp | **B** | NO (139) | C3 (2 deferred) | no | Tunnelled: 0x005cd794 (L136), 0x008a9640 (L619), 0x007f0fd0 (L706) | callees via RVA only |
| IntroSplash.cpp | **B** | NO (73) | C3 (orch. disabled) | no | Tunnelled: 0x007719ec (L140), 0x007d3ff8 (L217) | **externs `VideoStateFlagGet`+`AspectRatioGlobalGet` (L36-37) ÔåÆ defined ONLY in UNLINKED SplashGameMode_t5.cpp** |
| **HUD/ (4)** | | | | | | |
| FontCtx.cpp | **B** | NO (177) | C3 (2 disabled) | no | Tunnelled: 0x00912b04 (L57), 0x004c5010 (L40) | callees via RVA only |
| HudBatch.cpp | **B** | NO (97) | C3 | no | Tunnelled: 0x005cc35c-0x005cd238 (L84-88), 0x007d3ff8 (L243), inline-asm 0x7d3ff8 (L620,715) | callees via RVA only |
| ViewportDimsSet_ag.cpp | **B** | NO (6) | C3 | no | Tunnelled: 0x005cc320 (L20,26), calls 0x004c0e50 (L28) | none |
| TextCluster.cpp | **B** | NO (56) | C3 | no | Tunnelled: 0x00912b04 (L36), 0x00912a84 (L39), asm 0x0067d974/0x0042b8b0 (L275,199) | callees via RVA only |

**reached-by-name = NO for all 24, and this is provable, not asserted:** the standalone frontend call graph is `exe_main.cpp` + `MenuNavSM.cpp`. Every function here is an original-image `FUN_004xxxx`/`FUN_005xxxx` registered via `RH_ScopedInstall`, which in the exe resolves to the no-op in `Stubs\HookSystemNoOp.cpp` ÔÇö so they are dead exports. If any *linked* TU referenced one of their symbols by name, today's exe would already fail to link (unresolved external); since it links, no linked file reaches them. The four Twins' live standalone counterparts are already linked separately.

## (2) ORDERED ADD-BACK LIST of RVA-free class-B files

**EMPTY.** Zero of the 24 unlinked Frontend/HUD files is RVA-free. This is the headline: Frontend sits at 47/67 precisely because the RVA-free functions were *already* harvested into the exe ÔÇö every file left behind is a tunnelled reimplementation whose body dereferences MASHED addresses (data at 0x005f/0x0067/0x0077/0x0089/0x008a/0x0091ÔÇª, code at 0x0040-0x0055ÔÇª) that are unmapped in an exe based at 0x10000. None is linkable-to-functional as-is.

## (3) Porting backlog ÔÇö load-safe-but-tunnelled class-B files (18)

All 18 are load-safe (boot fine, no class-C) but AV if their bodies ever run standalone. Ranked by neutralization effort (ascending code-address footprint = least porting work first). Confidence is the file's dominant status.

| Rank | File | code-addr footprint | notes for porting |
|---|---|---|---|
| 1 | HUD/ViewportDimsSet_ag.cpp | 6 | one `0x005cc320` const + one `0x004c0e50` callee; smallest surface |
| 2 | BatchAA_s2.cpp | 20 | single accessor, one linked extern |
| 3 | RaceResults.cpp | 24 | one `0x005f2770` base to relocate |
| 4 | MenuMenusMixed.cpp | 29 | one table `0x005f6748` |
| 5 | BatchAA_s5.cpp | 32 | table twin of #4 |
| 6 | FrontendLeaves_ad1.cpp | 35 | score/team arrays; extern linked |
| 7 | RaceArmReset.cpp | 40 | 2 orig-RVA callees to port first |
| 8 | SpriteDispatch.cpp | 46 | slot ptr table + 2 orig callees |
| 9 | HUD/TextCluster.cpp | 56 | font ctx globals + x87 asm |
| 10 | IntroSplash.cpp | 73 | **must add SplashGameMode_t5 first** (see cross-TU); orchestrator install disabled (unresolved boot AV) |
| 11 | SlotZeroers_s2.cpp | 76 | per-player score arrays |
| 12 | SplashGameMode_t5.cpp | 89 | leaves + globals; **provides IntroSplash's externs** |
| 13 | HUD/HudBatch.cpp | 97 | render vtable via 0x007d3ff8 + inline asm |
| 14 | MenuMenusB.cpp | 131 | font/measure/draw chain, many callees |
| 15 | SkeletonAndScatter_t6.cpp | 135 | localization/PIZ + render vtable |
| 16 | MenuMixed.cpp | 139 | credits table + printf + FPU |
| 17 | HUD/FontCtx.cpp | 177 | full font matrix stack; 2 installs already disabled |
| 18 | SpriteCluster.cpp | 173 | largest; ProgressBar/Lobby renderers |

**Add-back ORDER constraint (the one real cross-TU-to-unlinked dep):** `IntroSplash.cpp` externs `VideoStateFlagGet` and `AspectRatioGlobalGet` (IntroSplash.cpp:36-37), which are defined **only** in the still-unlinked `SplashGameMode_t5.cpp` (SplashGameMode_t5.cpp:60,91). Adding IntroSplash without SplashGameMode_t5 = unresolved-external link failure. **SplashGameMode_t5 must precede or accompany IntroSplash.** All other externs in scope resolve to already-linked TUs (GameStateGetters, StateAccessors, MenuButtonDetect, SpriteGate, DrawQuadPrimitives).

## (4) Counts per class

| Class | Count | Files |
|---|---|---|
| **A** (unlinked by intent) | **6** | 2 empty TUs (MixedC3Sweep, HudFrontendDispatchers_t4) + 4 diff-original Twins (MenuAnimTick, LogoOverlay, MenuDrawLoop, PromptStrip) |
| **B** (safe-to-add, load-safe) | **18** | 14 Frontend + 4 HUD ÔÇö **all tunnelled, none RVA-free** |
| **C** (blocked) | **0** | no load-time-deref construct exists in Frontend/ or HUD/ |
| **Total** | **24** | |

**Bottom line for D0.7:** the Frontend/HUD unlinked set contains **no class-C loader hazards** and **no RVA-free files** ÔÇö so there is **nothing to bulk-add for a working feature**. All 18 class-B files are backlog: each needs its MASHED-address tunnels neutralized (porting work) before it ships anything, exactly the batch-1 bar in build.bat:137-140. The 6 class-A files should stay unlinked permanently (harness/empty).
