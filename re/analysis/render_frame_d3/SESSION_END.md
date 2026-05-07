# SESSION_END — render_frame_d3-20260503

**Session ID**: render_frame_d3-20260503  
**Bucket**: render_frame_d3  
**Date**: 2026-05-03  
**Slot**: Mashed_pool13 (pool5 was orphan-locked; .lock~ held by another process)  
**Binary anchor**: MASHED.exe SHA-256 = `BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E` ✓

---

## Functions mapped (C0→C1)

| RVA | Name | Notes |
|-----|------|-------|
| 0x00426670 | WorldRenderDispatch_Begin | Gates DAT_0066d704; selects world_A/B; calls FUN_004e4320 |
| 0x004266b0 | WorldRenderDispatch_End | Mirror of 00426670; calls FUN_004e4350 |
| 0x0040de30 | MinimapCameraOrthoSetup | Saves frame+proj+vw; identity+ortho 0.6×0.45; calls WorldRenderDispatch_Begin |
| 0x0040df20 | MinimapCameraRestore | Calls WorldRenderDispatch_End; restores proj/vw/frame from save buffers |
| 0x00492440 | RenderStatsAccumulate | 60-frame rolling avg leaf (sum+avg+max+counter per 2 values) |
| 0x00404320 | PerModeRenderMachine | Large state machine modes 5/8/9/10; EndUpdate→SetVW→BeginUpdate→dispatch |
| 0x00410b30 | InGameRenderDispatcher | Main in-game render: player cam + 4-player loop + world + 20+ effects/HUD |
| 0x00433f40 | RaceEndFadeOverlay | Guards DAT_0067eab0/b8; alpha=(b8*3)>>2; 3 panels + text; track 0x27b special |
| 0x00426030 | WorldRenderPrePass | Calls sub_0041e9b0; predicate FUN_0041ea10; sets DAT_005f616c=1 |
| 0x0040df60 | ConditionalRenderSubPass | Three-gate guard (ba8c∈{5,6}, FUN_0042f6a0∈{3,4,5}, fd0∈{4,7,8,9,10}) |
| 0x00492e60 | SetDefaultViewWindow | Calls FUN_004671a0(0xffffffff) + RwCameraSetViewWindow(0.8, 0.8) |
| 0x0042d390 | GetRaceStateField | Trivial getter returning DAT_0067ea6c |
| 0x0042f530 | ViewportSetup | Computes viewport rect; copies camera rasters via FUN_004c7760 |
| 0x0042a9f0 | GetFadeAlpha | Trivial getter returning (byte)DAT_0067eca8 |

14 functions promoted C0→C1.  
D-0891 (FUN_00403050) was already C1/mapped (loading_screen session); skipped.

---

## Deferred entries filed

- **D-5020..D-5060** (41 entries) in DEFERRED.md, bucket `render_frame_d3-cont1`
  - D-5020: FUN_004e4320 (world render begin)
  - D-5021: FUN_004e4350 (world render end)
  - D-5022: FUN_0041ea10 (world predicate)
  - D-5023: FUN_0041e8f0 (world render pre-pass body)
  - D-5024: FUN_00401f10 (conditional render sub-pass body)
  - D-5025..D-5029: PerModeRenderMachine callees (FUN_00403d30, FUN_00403fa0, FUN_0042c010, FUN_004041c0, FUN_004037d0)
  - D-5030..D-5034: RaceEndFadeOverlay callees
  - D-5035: FUN_004671a0 (viewport/raster helper, multi-arg)
  - D-5036: already mapped (FUN_004270f0 = CourseRenderFrame)
  - D-5037..D-5060: InGameRenderDispatcher depth-3 callees (bulk; 24 entries)

---

## Tracker mutations

| Tracker | Change |
|---------|--------|
| hooks.csv | 14 rows C0→C1; 40 new C0/deferred rows appended (D-5020..D-5060, minus already-mapped) |
| DEFERRED.md | D-0880..D-0894 CLEARED (minus D-0891); D-5020..D-5060 appended |
| STUBS.md | S-1700..S-1712 appended (13 entries) |
| UNCERTAINTIES.md | U-1707..U-1716 appended (10 entries) |
| re/analysis/CHANGELOG.md | 14 individual entries + BATCH summary |

---

## Uncertainties opened

U-1707: world selector global DAT_0066d700 semantics  
U-1708: world pointer types at DAT_00656ee8 / DAT_0065742c  
U-1709: camera field at +0x14 (frame matrix start or header?)  
U-1710: RenderStats struct fields at +0x20..+0x40  
U-1711: mode-10 render table at 0x00636b88 (6 entries stride 20)  
U-1712: DAT_005f616c role (enable flag for world pre-pass)  
U-1713: DAT_0063ba8c full enum semantics (known: 5=single, 6=multi)  
U-1714: FUN_004671a0 calling convention (stdcall vs custom, multiple callers)  
U-1715: DAT_0067ea6c race-state field semantic meaning  
U-1716: DAT_0067ed68 viewport X-offset purpose (letterbox / splitscreen?)  

---

## ID ranges consumed

- U-1707..U-1716 of U-1707..U-1726 pre-assigned (10 of 20 used)
- S-1700..S-1712 of S-1700..S-1719 pre-assigned (13 of 20 used)
- D-5020..D-5060 of D-5020..D-5079 pre-assigned (41 of 60 used)

---

## Pool / MCP

- Slot Mashed_pool13 used (pool5 abandoned — orphaned .lock~ file, device busy)
- MCP session `fbd33b856186490085e6f995c02cc052` — to be closed post-session
