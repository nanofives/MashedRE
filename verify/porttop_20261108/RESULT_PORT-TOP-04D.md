# RESULT PORT-TOP-04D: RW memory functions on librw, AppDestroy's RW-free callers run, 0x004924f0 compiled (L-96 top-down lane)

Branch `orch/PORT-TOP-04D` from `orch/PORT-TOP-04C` @ `7db26fe47`, with `orch/integration` @ `04fc75cca` merged in
(`63444d543`: 04A, STATE-03, STATE-03-FPU). Code commits `b506fad14` + `0e80454f4`. Exe `0d0c0eb3c9751b03` (sha256/16), asi `f9f4b46e848879ef`, built from `0e80454f4`.

Verdict: HANDOFF. The parts that fit this step landed green: the RW memory functions behind 0x007d3ff8 run on librw,
0x004114c0 and 0x00484130 run matched in AppDestroy, and the trace has one line per destroyed camera. Knob-ON boot exits
0, the 12 captures are identical knob ON vs OFF, and the default gates are unchanged. The matched 0x004924f0 is compiled
into the exe, but running it changes one capture, so WinMain still calls the stand-in (a decision for the lead, below).
0x00402750 and the six teardown callees that need its state are handed back with a proposed split.

## Scope landed vs handed back

| item from the brief | state |
|---|---|
| RW free behind `*(0x007d3ff8)` on librw, not through the B15 device | **done**: RwGlobals record (below) |
| 0x004114c0 and 0x00484130 run matched | **done**: both run in AppDestroy (0x004114c0 logs its free; 0x00484130 receives DAT_007f0a30 = 0 and returns at its guard) |
| one log line per destroyed camera | **done**: 4 lines per knob exit |
| 0x004924f0 off its stand-in | **compiled, not run by default**: running it changes one capture (below), so WinMain still calls the stand-in. `MASHED_TOP04D_DATAZERO=1` runs the matched body |
| 0x00402750 AppInitialiseOnBootup matched | **handed back** (below) |
| the six teardown callees that need 0x00402750's state | **handed back** with 0x00402750 |
| remaining unbound RW rows | **handed back**: RpWorldDestroy 0x004e5700, 0x004ce2d0, 0x004c5a60, 0x004d41e0 are still stand-ins |

## RwGlobals record (LibRw/RwApiBind.cpp)

Matched game bodies call the RW memory functions through the pointer at 0x007d3ff8: free at +0x10c (0x004114c0,
0x00484130; `re/analysis/boot_app_init_d3/0x004114c0.md` "free at DAT_007d3ff8+0x10c"), alloc at +0x108
(`re/analysis/bucket_004ddfb0/0x004e8750.md`), realloc at +0x110 (`re/analysis/bucket_004c4270/0x004ce2d0.md`). On the
knob path 0x007d3ff8 held the B15 Im2D fake device (D3d9Render/RwIm2DBridge.cpp, 0x34 bytes), so +0x10c lay past it
(the 04C batch-1 0xC0000005).

`MashedRwGlobals` is an RW-layout record: bytes +0x00..+0x33 are copied from the B15 device (its three used fields: +0x18
near-screen Z, +0x20 render-state set, +0x30 Im2D primitive), +0x108 / +0x10c / +0x110 / +0x114 hold malloc / free /
realloc / calloc wrappers over librw's `rw::Engine::memfuncs`, the rest is zero. `MashedRw_InstallRwGlobals()` points
0x007d3ff8 at it once, from the 0x00402750 stand-in, right after the hand init installed the B15 bridge. The B15
device's fields are written only at its install (RwIm2DBridge.cpp:277-279), so the copy cannot go stale. The free
wrapper logs each call. Other readers of 0x007d3ff8 offsets beyond +0x34 (IntroSplash +0x9c, the 0x004d8c40 list splice,
HudBatch +0x118 through DAT_007d4028) are .asi-only bodies.

The default path never calls `MashedRw_InstallRwGlobals` (it is reached only from Match/PortTop_00492370.cpp).

## 0x004924f0: compiled, not run by default

The matched source `re/match/topm01/004924f0.c` (464 B byte-identical, relocheck OK) is the exe body of 0x004924f0, in
Boot/SubsystemInit.cpp under `MASHED_STANDALONE`. The .asi keeps its raw passthrough, so there is one TU and one body.
Callees bind by name: 0x00431ae0 / 0x00431af0 / 0x00431b00 / 0x00431d00 / 0x0045b350 to their exe bodies, 0x00431b10 to
Match/MatchS07_00431b10.cpp (added to the exe list).

Batch 1 ran it from WinMain (exe `2fe7b988`, commit `b506fad14`). Knob ON, **walk_04_colourselect differs** from knob OFF
(2188 px in box 62,234 to 417,286): player 1's keyboard icon sits on the right and highlighted instead of on the left in
white. The other 11 captures were identical. Both knob runs matched each other and both hand runs matched each other,
so the difference is deterministic. Batch 2 confirms the cause. The default run calls the stand-in, and all 12 are
identical. A diagnostic run with `MASHED_TOP04D_DATAZERO=1` (the matched body) gives walk_04 = `da29bdef4b0bc8f1`,
the same file as batch 1's knob runs. The RwGlobals record is in both batches, so it is not the cause.

Mechanism (static): the hand boot chain (exe_main.cpp ExecuteFrontendBootChain step 1) zeroes only 0x007f0f60..
+0x373a4 afterwards. 0x004924f0's writes outside that range survive: the 0x007f0a40 table defaults (`ctl[13][5] = -1`,
`b = 9`, `c = 0x3b`, `d = 0.99f`, `on[13][12] = 1`, `opt[13][12]`), 0x007f0f2c..0x007f0f3c = 0, and 0x007f0f10 = 2
(0x00431b10). The port's save path does not restore 0x007f0a40 into the image (exe_main.cpp:189), so the hand path keeps
zeros there. Which of the two colour-select states the original shows is not established here: no original-side capture
of that screen was taken.

**Decision for the lead:** run 0x004924f0 matched (knob path then shows the defaults, and walk_04 changes knob ON vs OFF)
or keep the stand-in until the save restore writes 0x007f0a40 the way the original does. Evidence: the two walk_04
files (`2871487f1fcef51c` hand, `da29bdef4b0bc8f1` matched). Recommendation: keep the stand-in, and take an original-side
walk_04 capture before deciding.

## Knob-ON boot (`knob_on_1`, `MASHED_ORIG_TOP=1 MASHED_DET_FRAMES=3000`, exe `0d0c0eb3`)

Exit 0 after 3000 frames, 27.2 s. New lines against 04C:

```
[ORIG_TOP] 0x004924f0 data init: stand-in (hand boot chain steps 1-3, run inside the 0x00402750 stand-in)
[RW_BIND] RwGlobals: 0x007d3ff8 00F1C030 (B15 device) -> record 00FEE0F8; Im2D +0x18/+0x20/+0x30 copied, memory functions at +0x108..+0x114 on librw
[ORIG_TOP] 0x00402a40 AppDestroy (matched) enter: cameras ... DAT_007f0a30=00000000, DAT_008a94a8=00000000
[RW_BIND] RwGlobals +0x10c free(00000000) -> rw::Engine::memfuncs.rwfree                      (0x004114c0, matched)
[RW_BIND] RwCameraDestroy(08E405C8) -> rw::Camera::destroy: librw camera 08D4C138, rasters 0181DC20 / 0181E190   (0x00467020: DAT_006905b0)
[RW_BIND] RwCameraDestroy(08E408E8) -> rw::Camera::destroy: librw camera 08D4BF10, rasters 0181DC20 / 0181E190   (0x00467070: clone DAT_006905b4)
[RW_BIND] RwCameraDestroy(08E403E8) -> rw::Camera::destroy: librw camera 08D4F4F8, rasters 00000000 / 00000000   (0x004b4880: DAT_0067f18c)
[RW_BIND] RwCameraDestroy(08E40CA8) -> rw::Camera::destroy: librw camera 08D4CC00, rasters 00000000 / 00000000   (0x004b4880: DAT_0067f190)
[ORIG_TOP] 0x00402a40 AppDestroy (matched) returned; then HandTop_AppDestroy for the exe audio / DirectInput / small.mpg
[ORIG_TOP] WinMain 0x00492370 returned 0 after 3000 frames
```

The clone shares its source's rasters (`RwCameraClone`), and 0x00467070 destroys only its frame and the camera. 0x004b4880
zeroes +0x60 / +0x64 before RwRasterDestroy, so its two cameras log zero rasters at destroy time. 0x00484130 receives
DAT_007f0a30 = 0 and returns at its guard, so it makes no free call. The stand-ins `0x004114c0` / `0x00484130` from 04C
no longer appear. The port-side shutdown order did not need to change: HandTop_AppDestroy still runs after the matched
body, and the boot exits 0.

## Captures

`MASHED_NAV_DEMO=1 MASHED_DETERMINISTIC=1`, runs knob ON, OFF, OFF, ON in root `D:/MashedWT/PORT-TOP-04D_rootK`,
settings baseline `efd5da58...` restored before each, exe `0d0c0eb3`. All exit 0 with `NAV_DEMO phase=28 done ok=1`. Both
knob logs have 4 `RwCameraDestroy` lines, the hand logs none. **12 / 12 captures byte-identical across the four runs**,
the same files as TOP-04B / 04C (`walk_02_mainmenu` = `24249b644e3e8057`, `walk_04_colourselect` = `2871487f1fcef51c`).
The fifth run (`navdemo_knob_dz`, `MASHED_TOP04D_DATAZERO=1`) is the diagnostic above: 11 / 12 identical, walk_04 differs.

## Default gates (knob OFF)

Exe `0d0c0eb3` (commit `0e80454f4`), `run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`, `A4_ROOT=D:/MashedWT/PORT-TOP-04D_root`.
All four arms exit 0, self-exited (H_DEF 123 s, B 12 s each).

| gate | this branch | merge_train.json ref |
|---|---|---|
| G-CTRL | `8284be46/67931834/31fe043e` | same |
| B | `1d5bf4f9/1d5bf4f9/1d5bf4f9` | same |

Batch 1 (exe `2fe7b988`) gave the same gate hashes.

## 0x00402750 AppInitialiseOnBootup: handed back

0x00402750 is the whole asset bring-up: piz file system (0x004b6540 / 0x004b6560, mounts 0x00495280 / unmounts 0x004952f0
for font36, powerups, Panel, perm), PCVS_Init 0x00496e40, the RW D3D device (0x004caea0, no source) with two
SetRenderState calls through its vtable, the intro video 0x00495350, fonts, sprite lookups (0x0040bb50 "SemiC" /
"SemiC2", 0x0040bb30 "lensflare-fs8"), panels, powerups, pc.txd (0x004b3d80), then FrontendStateSet 0x00428390(3) and
0x004026d0(4). Its 58 direct callees: 13 with an exe body, 13 with only an .asi TU, 29 matched with no body, 2 with no
source (0x004caea0, 0x00558240), 1 RW binding. Its tree is 643 functions to depth 6 (`survey_tree.py`).

Running it matched means its loaders replace HandTop_InitRest's port loaders, and they write the original globals that
the port's frontend (matched exe bodies) reads. 0x004924f0 shows the cost of one such write in this step: its table
defaults alone moved an icon on one screen. One step cannot carry that and keep the 12 captures identical. Proposed
split:

1. **04E**: split HandTop_InitRest into the pieces that match 0x00402750's call points (piz mounts, PCVS / device, intro
   video, audio, frontend assets). Then run 0x00402750's matched body with each callee bound to its hand piece, and bind
   the piz file system (0x004b6540 / 0x004b6560 / 0x00495280 / 0x004952f0) to the exe's piz loader. Captures should
   stay identical.
2. **04F..**: one loader family per step (fonts, sprite dictionaries, panels / particle emitters, powerups, perm,
   pc.txd), each swapped to its matched body with a capture check.
3. With those in place, promote the six teardown callees (0x004014b0, 0x0041e0d0, 0x00489250, 0x0041ffb0, 0x004196f0,
   0x00412010). Each reads state 0x00402750 creates with no null check.
4. Resolve the 0x004924f0 decision above, and the save restore of 0x007f0a40.

RW rows still unbound on the knob path: RpWorldDestroy 0x004e5700, 0x004ce2d0 (RwImageSetPath by rw catalog match),
0x004c5a60, 0x004d41e0. 0x004d7ff0 / 0x004d8480 have port bodies only in the .asi-only Render/HighAB3Helpers_p6.cpp.

## Lint, build, machine rules

Builds: `cmd /c "set MASHED_NO_DEPLOY=1&& mashedmod\build.bat"`, both targets OK, rva-lint `OK -- no NEW duplicate bodies (1
known)`, asm-lint OK. One compile fix (`%p` for the raster pointers). `mashed_lock.py status` was `free` right before
each of the three builds. The third build waited for merge-train pass `20261109_031636` (PRIORITY-HOLD, then SOLO) to
clear. Every game launch (two batches) ran as one `D:/MashedWT/PORT-TOP-04D_data/run_all_04d.py` inside
`mashed_lock.py run --label PORT-TOP-04D`, inside `test-slot`. The driver stops after the knob boot when its log lacks
`WinMain 0x00492370 returned 0`.

## Port column (scripts/backfill_exe_file.py, then scripts/backfill_port.py --allow-stale)

Against this branch's hooks.csv after the integration merge: **8 rows changed**.
- From this step: 0x004924f0 body -> called (the column is static: the stand-in calls DataZeroFill under the switch),
  0x004114c0 body -> called, 0x00484130 none -> called, 0x00431b10 none -> called, 0x00431b80 body -> called, 0x00431d00
  body -> called.
- From the merged PORT-STATE-03 code, not this step: 0x0042ac50 body -> called, 0x0042ad10 none -> body.

`--allow-stale`: after the build, `git checkout` restored 31 regenerated TUs that differed only in line endings. That
made their mtimes newer than `mashed_re.map`, but their content is the content that was built. BINDINGS.tsv:
`update_bindings_top04d.py` (it runs the 04C updater first), 373 rows, matched 110, stand-in 60.

## Risks

1. The RwGlobals record copies the B15 device's first 0x34 bytes once. A future change that rewrites the B15 device's
   fields after install would not reach the record (today they are written only at install).
2. The record's other bytes are zero. A knob-path reader of a 0x007d3ff8 offset other than +0x18 / +0x20 / +0x30 /
   +0x108..+0x114 now reads 0 where it read B15 bytes or the statics after them. The readers found are .asi-only.
3. On the knob path the RW free goes to librw's free(). Today it only ever receives 0. Once 0x00402750 runs matched, the
   pointers it frees must come from the +0x108 alloc of the same record.

## Merge state

`orch/integration` @ `04fc75cca` merged at the start. 04B / 04C ride the next train pass. If integration moves first:
`git -c merge.autoStash=false merge orch/integration`, both backfill scripts, the BINDINGS updater, rebuild, re-run.

## Raw data (D:/MashedWT/PORT-TOP-04D_data)

| file | sha256 |
|---|---|
| knob_on_1.log | 0bfca80a68fbee166cf72ed095bcb7a4ab88738f330f2748210f7ab6058c793f |
| navdemo_knob1.log / navdemo_knob2.log | 11a8652780398bca43a4f7d786cf8e31456c25f8ad407f9af6081d7da196a6af / 7c73d342250d520c3bac79391ecf5a145679c59d20d638d22d5664ef348e1ee2 |
| navdemo_hand1.log / navdemo_hand2.log | 2c66c492a1eb7bf2ef554b3e2dbf750a835e3e67262c357aa2a32b32052535e3 / 5a3cbf323dfcdca031a4153ed584a1c0722431b2a98132eaf182b07df3394d9e |
| navdemo_knob_dz.log (diagnostic) | 3bbdff84b067254dce9eb74c28b511eceb9a49bc15b5d8c44a96bdbb5cb8c8dd |
| navdemo_knob_dz/walk_04_colourselect.bmp = batch1/navdemo_knob1/walk_04_colourselect.bmp | da29bdef4b0bc8f1 (sha256/16) |
| navdemo_hand1/walk_04_colourselect.bmp | 2871487f1fcef51c (sha256/16) |
| arms/H_DEF.step.csv | 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198 |
| run_all.log | 4f6bb822169fa403c056f15d7829a676eae0d941611f0b8196cd87bde9731267 |
| batch1/ (exe 2fe7b988, matched 0x004924f0 from WinMain) | knob boot exit 0, gates same, walk_04 differs |
