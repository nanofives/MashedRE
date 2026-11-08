# RESULT PORT-TOP-04B: DisplayInit 0x004921d0 + ViewportInit 0x00428590 under MASHED_ORIG_TOP

Verdict: GREEN for the step as scoped, with one listed missing node (0x00428450, the loading-disc draw: one-body rule, see below). Knob-ON boot to 3000 frames exits 0. The 12 nav-demo captures are identical knob ON vs OFF. Default gates unchanged.

Under MASHED_ORIG_TOP=1 SubsystemInit 0x00492270 now calls the matched DisplayInit 0x004921d0 and ViewportInit
0x00428590 (TOP-03 left both as missing stubs). Both run their exe bodies compiled from the 100% matched sources, with
every callee bound by name in the PLAN 7 order (matched exe body, existing port body, RW binding, listed stand-in). No
raw-address call on the path. One node is a listed missing stub: the loading-disc draw 0x00428450 (reason below).

Branch `orch/PORT-TOP-04B`, cut from `orch/PORT-TOP-04A` @ `a90ed1970`. Code commit `6e8b68a17`. The previous
version of this file was the handoff brief (`2d840afe7`); its callee tree and risk list are kept in the sections below,
with what each risk measured as.

## What runs now (BINDINGS.tsv, `update_bindings_top04b.py`)

DisplayInit `0x004921d0` (Match/MatchS15_004921d0.cpp, MASHED_STANDALONE branch):

| rva | kind | exe symbol | note |
|---|---|---|---|
| `0x004c2f00` / `0x004c2ed0` | bound (TOP-03) | `MashedRw_EngineGetCurrentMode_004c2f00` / `..GetModeInfo_004c2ed0` | librw video mode |
| `0x00498bc0` / `0x00498bd0` | stand-in (TOP-02) | `PortTop_Width` / `PortTop_Height` | DAT_00616028 / 2c are 0 in the exe; the reference gets 640x480 from fix_camera_res |
| `0x00467110` (+ static `0x004670a0`) | matched | `Orig_00467110` | new exe TU. Main camera into DAT_006905b0 (camera, frame, rasters 640x480 type 2 / 1), view window 0.8, near 0.1, far 180, clone into DAT_006905b4 with a new frame DAT_006905b8 |
| `0x00467020` | matched | `Orig_00467020` | new exe TU, failure path only |
| `0x0042b890` / `0x0042b8a0` | matched | `RenderWidthSet` / `RenderHeightSet` | wrappers now also in the exe list |
| `0x0042d560` -> `0x0042d420`, `0x0042d4a0` | matched | `Orig_0042d560` ... | new exe TUs. Camera DAT_0067f18c, 0 x 0 rasters made sub rasters of the main camera's |
| `0x0042f660` -> `0x0042d420`, `0x0042f530` | matched | `DefaultViewportCameraInit`, `Orig_0042f530` | new exe TUs (0x0042f530 is the C2 source, 96.9%, the only one). Camera DAT_0067f190 |
| `0x0042b8b0` / `0x0042b8c0` | matched | `ScreenWidthGet` / `ScreenHeightGet` | exe bodies already |
| `0x004671a0` | matched | `BackBuffRasterOpen` | wrapper now also in the exe list; binds `0x0042b930` MenuAlphaGet, `0x0042f510` Vehicle0HandleGet |
| `0x004997b0` -> `0x00499720` | matched | `Win32ResourceLoader` -> `GetInputHinst` | wrappers now also in the exe list. DAT_007e9580 = the exe module (set by the matched 0x00499ba0), so FindResourceA finds the RWTEXDICTIONARY 404 copy that assets/mashed_re.rc embeds |
| `0x004b3eb0` | port | `PortTop_Port_004b3eb0` | no matched source. Decomp (decomp_pc.py, 2026-11-08): RW memory stream `0x004cc230(3, 1, &{data, size})`, find chunk 0x23 `0x004cc5e0`, read `0x0054f8d0`, close `0x004cc160`. Port: the exe's chunk-0x23 decoder (Txd/TxdDecoder.h, written from 0x0054f8d0) + rw::TexDictionary / rw::Texture (RwRasterBridge) |
| `0x004c5c00` | bound (TOP-03) | `MashedRw_TexDictionaryFindNamedTexture_004c5c00` | now reached |

ViewportInit `0x00428590` (Match/MatchS06_00428590.cpp, MASHED_STANDALONE branch):

| rva | kind | exe symbol | note |
|---|---|---|---|
| `0x004671a0` | matched | `BackBuffRasterOpen` | returns DAT_006905b0 |
| `0x004c1bb0` | bound | `MashedRw_CameraClear_004c1bb0` | the matched body calls RwGlobals +0x9c, absent in the exe, so the brief's "MatchS19_004c1bb0 joins the exe list" does not hold: it stays .asi-only |
| `0x004c1a00` | matched | `IntroSplashVtableSlot6` | wrapper now also in the exe list; calls the camera's +0x18 |
| `0x00428450` | missing | `PortTop_Missing_00428450` | see "The one missing node" |
| `0x004c19f0` | matched | `RwVtableSlot07Call` | exe body already; calls the camera's +0x1c |
| `0x004c1be0` -> `0x004c7730` | matched -> bound | `Orig_004c1be0` -> `MashedRw_RasterShowRaster_004c7730` | new exe TU |

RW core bound in `LibRw/RwApiBind.cpp` (identity read from each matched source, cited in the source comment):
`0x004c1d30` RwCameraCreate, `0x004c2bf0` RwCameraClone, `0x004c1cf0` RwCameraDestroy, `0x004c1c80`
RwCameraSetViewWindow, `0x004c1a40` / `0x004c1b10` RwCameraSetNear / FarClipPlane, `0x004c1bb0` RwCameraClear,
`0x004c2bb0` / `0x004c2b80` the camera's begin / end update functions (stored at +0x18 / +0x1c by 0x004c1d30; no
source, no hooks.csv row), `0x004c0b30` RwFrameCreate, `0x004c0c20` RwFrameDestroy, `0x004c0740`
_rwObjectHasFrameSetFrame, `0x004c77c0` RwRasterCreate, `0x004c7650` RwRasterDestroy, `0x004c7760` RwRasterSubRaster,
`0x004c7730` RwRasterShowRaster. `0x004c2bf0` is RwCameraClone (re/match/rwl01/004c2bf0.c creates a camera, takes the
source's frame, copies +0x18..+0x88, +0x60, +0x64, +0x14 and runs the plugin copy constructors); the brief had it as
"to identify". `0x004c7760` is RwRasterSubRaster (re/match/s19/004c7760.c).

### Cameras: RW-layout records over librw cameras

The matched game bodies touch RwCamera fields at fixed offsets themselves: 0x004670a0 and 0x0042d420 store the rasters
at +0x60 / +0x64 and test the frame at +0x04, 0x0042d4a0 / 0x0042f530 read +0x60 / +0x64, 0x004c1be0 reads +0x60,
0x004c1a00 / 0x004c19f0 call +0x18 / +0x1c. librw's rw::Camera has another layout (beginUpdateCB at +0x14, frameBuffer
well past +0x60), so a librw Camera* cannot be handed to them. The RwCameraCreate binding returns an RW 3.x-layout record
(the fields 0x004c1d30 writes, up to +0x94, static_asserted) whose exe-only tail holds the librw camera, with +0x18 /
+0x1c pointing at the begin / end update bindings. Every camera binding copies the fields the game code can write
(rasters, projection) to the librw camera before acting. Frames and rasters are librw objects as they are. librw's
camera defaults equal the ones 0x004c1d30 writes (window 1, offset 0, near 0.05, far 10, fog 5, perspective).

Two librw paths are deliberately not used:
- `rw::Camera::showRaster` / the d3d driver's `showRaster` Resets the device when the vsync flag differs from the
  PresentationInterval (`d3ddevice.cpp` showRaster). Under adoption that is the exe's device, and a Reset drops its
  default-pool resources. The RwRasterShowRaster binding presents on the adopted device directly.
- librw's begin / end update issue no BeginScene / EndScene under adoption (MASHED_PATCHES.md P3). ViewportInit's update
  runs at boot outside every exe frame, so the begin / end bindings bracket it themselves, as RW's D3D9 driver does.

### The one missing node: 0x00428450 (loading-disc draw)

Its only port body is the .asi-only naked-asm `HudSpinCoinAnim` (HUD/TextCluster.cpp, raw-address calls and raw .rdata
reads), and its matched source is 84.6% (re/match/s06/00428450.c). Compiling that source in an exe-only TU gives the
RVA a second body (rva-lint CROSS-TARGET, fail on new), and swapping the .asi body to a non-identical source is not a
matched-body swap. So ViewportInit clears, opens and closes the camera update and presents, without the disc quad. Its
callee `0x00450b10` (HudIm2DQuad) has an exe body and would also need the fake RW device that HandTop_InitRest installs
after SubsystemInit. To close it: a byte-identical 0x00428450 source (then one shared TU replaces TextCluster's asm),
plus the fake RW device installed before ViewportInit and the LoadIcon raster registered as a bridge handle.

### Lint fix in an .asi TU

`Frontend/IntroSplash.cpp` had `// 0x004c1be0  FUN_004c1be0 ...` above a raw call-through (`OrigRenderTargetHelper`),
which rva-lint binds as a body, so the new exe body of 0x004c1be0 failed CROSS-TARGET. The comment now reads like its
neighbour's (`Call-through to the ORIGINAL FUN_004c1be0 ... (not a body)`). No code change.

### .rdata

DisplayInit reads `s_RWTEXDICTIONARY_005cf6a4` and `s_LoadIcon_005cf698`. `gen_rdata_seed.py` now also takes
re/match/s15/004921d0.c: 27 rows (`rdata seed: 27/27 strings written` on the knob run).

## Knob-ON boot (`knob_on_1`, `MASHED_ORIG_TOP=1 MASHED_DET_FRAMES=3000`)

Exit 0 after 3000 frames, 28.6 s. Trace (`[RW_BIND]` / `[ORIG_TOP]`, in order, after the TOP-03 bring-up):

```
RwCameraCreate -> rw::Camera::create: record 08D32E50, librw camera 08E4D130
RwRasterCreate(640, 480, 0, 0x2) -> rw::Raster::create 016535D0
RwRasterCreate(640, 480, 0, 0x1) -> rw::Raster::create 016536B8
RwCameraClone(08D32E50) -> rw::Camera::clone: record 08D32F90
RwCameraCreate ... RwRasterCreate(0, 0, 0, 0x2) ... (0, 0, 0, 0x1)        (DAT_0067f18c)
RwCameraCreate ... RwRasterCreate(0, 0, 0, 0x2) ... (0, 0, 0, 0x1)        (DAT_0067f190)
0x004b3eb0: TXD from memory (33932 bytes) -> rw::TexDictionary 08D3C598 with 1 of 1 textures
RwCameraClear(08D32E50, 000000ff, 3) -> rw::Camera::clear
camera begin update 08D32E50 -> rw::Camera::beginUpdate, BeginScene ok
0x00428450 loading-disc draw (32, -32): missing body, not drawn
camera end update 08D32E50 -> EndScene, rw::Camera::endUpdate
RwRasterShowRaster(016535D0, flags 1) -> Present on the adopted device: 0x00000000
0x00492270 SubsystemInit (matched) returned 1
... AppInitialiseOnBootup stand-in, MainLoopInit returned 1, 3000 frames ...
0x00402a40 AppDestroy: stand-in HandTop_AppDestroy
RwTexDictionaryDestroy(0x08D3C598) -> rw::TexDictionary::destroy      (WinMain, first time reached)
RwEngineStop / Close / Term, HardwareExitApplication returned 0
WinMain 0x00492370 returned 0 after 3000 frames
```

The three cameras and their rasters are not destroyed at exit: in the original that is AppDestroy 0x00402a40's subtree,
still a stand-in (PORT-TOP-04C). librw's Engine::stop / close / term ran with them alive and the process exited 0.

## Captures

`MASHED_NAV_DEMO=1 MASHED_DETERMINISTIC=1`, four runs knob ON, OFF, OFF, ON in root `D:/MashedWT/PORT-TOP-04B_rootK`,
`mashed_re_settings.bin` restored from the TOP-03 baseline (`efd5da58...`) before each. All 13 NAV_DEMO steps on every
run (`NAV_DEMO phase=28 done ok=1 depth=0 screen=1 cur=2`; `knob_boot.py` prints only 60 lines, so the knob runs' `done`
line is in the logs, not in `run_all.log`), exit 0. Both knob runs show the ViewportInit present
(`RwRasterShowRaster` once per knob log, 0 in the hand logs). **12 / 12 captures byte-identical across the four runs**;
`walk_02_mainmenu.bmp` = `24249b64...`, the same file as TOP-02 / 03 / 04A.

| capture | sha256 (16) |
|---|---|
| walk_01_title | 83c8cbb913899494 |
| walk_02_mainmenu | 24249b644e3e8057 |
| walk_03_singleplayer | feb2c803554cd028 |
| walk_04_colourselect | 2871487f1fcef51c |
| walk_05_abilityselect | 9f02cb91d09e0500 |
| walk_06_challengeselect | b1f095c07072b28d |
| walk_07_back_mainmenu | 3465159db8526481 |
| walk_08_options_highlighted | 843e3fc49d166754 |
| walk_09_options | 5db0164fb2c6df8c |
| walk_10_sound | ca2f64dc5e7a9407 |
| walk_11_sound_adjusted | 536f710d21e50619 |
| walk_12_back_mainmenu2 | 6d41bc3ac8ad8d8a |

## Default gates (knob OFF)

Exe `94e87c9b` (commit `6e8b68a17`), `run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26` with `A4_ROOT=D:/MashedWT/PORT-TOP-04B_root`,
under `test-slot` (outside) + `mashed_lock.py run --label PORT-TOP-04B` (inside). All four arms exit 0 and self-exited
(H_DEF 118 s, each B arm 12 s).

| gate | this branch | merge_train.json ref |
|---|---|---|
| G-CTRL | `8284be46/67931834/31fe043e` | same |
| B | `1d5bf4f9/1d5bf4f9/1d5bf4f9` | same |

Build: `mashedmoduild.bat` with MASHED_NO_DEPLOY=1, both targets OK, rva-lint "OK -- no NEW duplicate bodies (1 known)",
asm-lint OK. Every game launch of this step (the knob boot, the gate arms, the four nav-demo runs) ran under
`mashed_lock.py` inside `test-slot`. The two builds ran before the lead's 2026-11-08 build rule (no build during a
merge-train PRIORITY-HOLD / SOLO) and were not checked against it.

## Risks from the brief, as measured

1. librw state after ViewportInit's frame: the 12 captures are byte-identical knob ON vs OFF, so the librw clear / camera update and the boot Present leave nothing the menu frames show. The clear touches the device viewport, render target and depth through librw, and engine->currentCamera stays the main camera after ViewportInit; the hand path's librw submit sets its own camera each frame (RaceSubmit) and P4 re-pushes librw's cached state. The run that skips only the present was not needed (nothing moved).
2. Extra Present before the title: one Present at boot (`RwRasterShowRaster ... 0x00000000`). The deterministic nav demo counts its own frames: all 13 steps at the same phases knob ON and OFF, identical captures. The knob-ON 3000-frame run took 28.6 s against 54.0 s on TOP-04A's run; that is machine load (the frame count is fixed and the exit is the same).
3. Camera rasters at render size: the main camera's rasters take librw's default render target and the exe's depth
   surface (d3d.cpp rasterCreateCamera / P5), the other four are 0 x 0 (not allocated) sub rasters. No new surface.
4. Exe readers of the new non-zero globals: DAT_006905b0 / b4 / b8, DAT_0067f18c / 90 and DAT_0077195c / 60 are read in
   the exe only by bodies that are `port=body` (not called): GetOverlayCamera 0x004671c0, 0x004671d0, 0x00467210,
   Vehicle0HandleGet 0x0042f510, plus the knob path's own nodes. DAT_0077195c's reader is WinMain's destroy (above).
   The cameras in these globals are RW-layout records, not librw cameras: a later port of their readers must go
   through the same bindings.

## Port column

`backfill_exe_file.py` then `backfill_port.py`: 20 rows change. none -> called: `0x0042b890 0x0042b8a0 0x0042d420
0x0042d4a0 0x0042d560 0x0042f530 0x0042f660 0x00467020 0x004670a0 0x00467110 0x004671a0 0x00499720 0x004997b0
0x004c1a00 0x004c1be0`. body -> called: `0x00428590 0x004921d0 0x0042b8b0 0x0042b8c0 0x004c19f0`. `0x004b3eb0`'s port
body carries a `// rva` comment (as TOP-04A's port bodies), so it is not counted as a body. No tracker C-level changes.

## Merge state

`orch/PORT-TOP-04A` (`a90ed1970`) is not on `orch/integration` yet (`04a6cc060` at hand-in), so no merge was made. When it lands: `git -c merge.autoStash=false merge orch/integration`, then `backfill_exe_file.py` and `backfill_port.py`, rebuild, gates, knob boot, captures.

## Files, raw data

Committed: this file, `gen_top04b_tus.py`, `update_bindings_top04b.py`, `BINDINGS.tsv`, `hooks.csv`, the code above.
Raw data in `D:/MashedWT/PORT-TOP-04B_data`:

| file | sha256 |
|---|---|
| mashed_re_gated.exe (commit 6e8b68a17) | 94e87c9ba3cea8a007aec10290bea119102d8cdeb1b2d3532dced5b828b3b1c1 |
| build1.log | 0d61acb034fabb4c9f134bb1a2f7398f2d710c7fb9ceabd31e8b735462251210 |
| knob_on_1.log | d9aa3d38f5f794dbe9976bf5c0f480b111729ab11d32c55f8582c1090bb5405c |
| run_all.log (gates + captures) | 2057c7b7e913a4cee7b8086bb074de43e00286592b85295f697728de331efcdf |
| navdemo_knob1.log / navdemo_knob2.log | 57a61f88c436c7de023447cb9f5e175707eee8a1b533eec1e61b05c44a33fb9d / 3df9cb4e34394721ad01966fef921b0dbabe80f22989ad0502997c1a8c0f29fc |
| navdemo_hand1.log / navdemo_hand2.log | d8fdc5ec0a9f61d5f9a8389f248365e935532b35d9cf5e4e829e2eb0d94b0e71 / ccb7cddbcfcc99b100d789f85a341f35d225a015255737840f71966d714bb41e |
| navdemo_*/walk_02_mainmenu.bmp (all four) | 24249b644e3e8057271f3f1c252514c7820abec3f3da15d316f39dbf156a9066 |
| arms/H_DEF.step.csv / .gates.csv / .ptrace.log | 8284be46b3c692f3... / 6793183492e0c082... / 31fe043e261d2df9... |
| arms/B_DEF_K0 / K1 / K26.step.csv | 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 |
| settings_baseline.bin (copy of TOP-03's) | efd5da5862222448af3ee58c255d583f88b5df28e26ae8ec2fe5a837572c1590 |

## Next

- PORT-TOP-04C (AppDestroy 0x00402a40): should destroy the three cameras through `0x00467020` (bound here).
- 0x00428450: a byte-identical source, then one shared TU for both targets, the fake RW device installed before
  ViewportInit and the LoadIcon raster registered as a bridge handle (or HudIm2DQuad given a librw path).
- PORT-TOP-04D (AppInitialiseOnBootup).
