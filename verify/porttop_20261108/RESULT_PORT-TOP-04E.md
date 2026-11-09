# RESULT PORT-TOP-04E: 0x00402750 AppInitialiseOnBootup runs matched under MASHED_ORIG_TOP (L-96 top-down lane)

Branch `orch/PORT-TOP-04E` from `orch/PORT-TOP-04D` @ `0f1ce4b75`. Code commits `a69549b31` (batch 1) and `5a5334269`
(batch 2). Evidence exe `cb722fb981ef822a` (sha256/16), built from `5a5334269`. Batch 1 exe `40b78e351261fcc0`, asi
`90c26107662712f1`.

Verdict: GREEN on what landed, with the loader-family moves and the six teardown callees handed back (below). The
matched 0x00402750 now runs on the knob path in place of the HandTop_InitRest stand-in. Knob-ON boot exits 0, the 12
nav-demo captures are byte-identical knob ON vs OFF (and the same files as 04B / 04C / 04D), and the default gates are
unchanged.

## Scope landed vs handed back

| item from the brief | state |
|---|---|
| 1. split HandTop_InitRest at 0x00402750's call points | **done**: five pieces (below), the default path runs them in the old order |
| 2. compile 0x00402750 matched, bind each callee | **done**: `Match/PortTop_00402750.cpp`, 58 callees bound by name, no raw-address call |
| 3. bind the piz file system the original loaders use | **done**: 0x004b6540 / 0x004b6560 / 0x00495280 / 0x004952f0 on the exe piz reader |
| 4. one loader family per commit, hand piece -> matched body | **partial**: batch 2 moved 0x004274d0 and 0x00431b40 (two leaves) to their matched bodies. The loader families (fonts, sprite dictionaries, panels, powerups, perm, pc.txd) are still stand-ins |
| 5. six teardown callees in AppDestroy | **handed back**: each reads state the loaders still on stand-ins create |
| 6. remaining RW rows | **done**: RwTextureDestroy 0x004c5a60, RwImageSetPath 0x004ce2d0, RpWorldDestroy 0x004e5700 on librw. 0x004d41e0 is a shim (identity not recorded) |
| 0x004924f0 | left on its stand-in (row PORT-TOP-04D-WALK04) |

## 0x00402750 matched body

`re/match/s01/00402750.c` got the MGLOBAL conversion (`verify/porttop_20261108/mglobal_convert.py`, 7 globals) so the
port can compile it as C++. Re-checked after the conversion: `match.py --bytes` 100.0% (187/187 ins), 739 B
BYTE-IDENTICAL; `relocheck.py` OK checked=93 unchecked=0. WinMain's `FUN_00402750` binds to `PortTop_Bound_00402750`
(Match/PortTop_00492370.cpp). It calls `Orig_00402750`, then installs the RwGlobals record and the no-focus-pause flag
as before. `MASHED_TOP04E_HANDINIT=1` runs the old one-call stand-in instead (diagnostic only).

### The five hand pieces (exe_main.cpp)

| piece | call point in 0x00402750 | hand work |
|---|---|---|
| 1 `HandTop_InitDataInput` | 0x00496e40 PCVS_Init | data wedge, boot chain, DirectInput keyboard |
| 2 `HandTop_InitMovies` | 0x00495350 HardwareShowIntroVideo | dsound.dll System32 guard, boot movies |
| 3 `HandTop_InitAudio` | 0x004669b0 sound engine init hub | exe audio, permdict.rws, menu music |
| 4 `HandTop_InitRasters` | 0x00403640 BackBuffRasterOpen wrapper | B14 probe, texture upload, B15 Im2D bridge, B16 RVA thunks |
| 5 `HandTop_InitFrontendAssets` | 0x00428390 FrontendStateSet(3) | B19 assets, settings, save restore, nav init, fonts, string table, B17 line |

The call points come in the hand init's own order, so the hand work runs in the same sequence on both paths.
HandTop_InitRest (default path) is now the five calls in a row. Piece 2's failure (no System32 dsound.dll) sets
`g_PortTop04E_InitFailed`: the later pieces do nothing and the wrapper returns 0, as the hand init returns false.

### Callee bindings (58)

- **matched / exe bodies (8):** 0x004d8560 UtilReturnOne4d8560, 0x00498bc0 VideoGetRenderWidth, 0x00498bd0
  VideoGetRenderHeight, 0x004671a0 BackBuffRasterOpen, 0x0045b350 RwInitNullStub, 0x00499ce0 (new
  Match/PortTop_00499ce0.cpp: identity matrix at 0x00773928, no other exe reader of 0x00773928..0x00773967), and from
  batch 2 0x004274d0 LangIndexSeedFromCli (DAT_007f0f60 = DAT_007719e8) and 0x00431b40 Set67eaa8 (DAT_0067eaa8 = 0.3f).
  Batch 2 added MatchS06_004274d0.cpp and MatchS07_00431b40.cpp to `exe_sources.rsp`.
- **bound (6):** the piz file system (4, below), RwEngineGetModeInfo 0x004c2ed0, RwEngineGetCurrentMode 0x004c2f00.
- **hand pieces (5)**, the log stand-in 0x004963e0 (PortTop_CfgLogDebug), and the **device stand-in** 0x004caea0 (no
  source): it returns a record whose vtable slot 57 (+0xe4) logs `SetRenderState(0x19, 5)` and `(0x18, 0)` and does not
  send them to the D3D9 device (librw keeps its own render-state cache for that device). DAT_00771a0c gets the record.
  No exe code reads 0x00771a0c.
- **stand-ins (37)** in Match/PortTop_00402750_Pieces.cpp: each logs its first call and returns 0, which leaves the
  global 0x00402750 writes from it at 0, the hand path's value (DAT_007f09e4, DAT_008aa318 / DAT_008aa314,
  DAT_00636ac8). 0x004841d0 returning 0 skips Rec18Create, so DAT_007f0a30 stays 0 (0x00484130 still returns at its
  guard). 0x00429290 is only passed as a pointer to 0x004b6560. 35 stand-ins are reached in the knob boot.

### Piz file system (exe piz reader)

0x004b6540 records the mode flag (0 on this run), 0x004b6560 records the callback
and its argument (the exe reader loads synchronously and never calls it). 0x00495280 maps `d:\toastart\...` to
`original/toastart/...` (the hand loaders' root, relative to MASHED_ROOT), loads the archive with
`mashed_re::Piz::Archive` and pushes it on an 8-deep mount stack. 0x004952f0 pops and frees the newest one.
`PortTop_PizFs_Find(entry, &len)` looks an entry up in the mounted archives, newest first, for the loaders that move
next. Knob boot:

```
[PIZ_FS] 0x004b6540 file system setup, mode flag 0 (exe piz reader, Piz/PizReader.h)
[PIZ_FS] 0x004b6560 callback 00168490 arg 0 recorded (not called by the exe reader)
[PIZ_FS] 0x00495280 mount d:\toastart\common\font36.piz -> original/toastart/common/font36.piz: OK, 26 entries (depth 1)
[PIZ_FS] 0x004952f0 unmount d:\toastart\common\font36.piz (depth 0)
[PIZ_FS] 0x00495280 mount d:\toastart\common\powerups\powerups.piz -> ...: OK, 36 entries (depth 1)
[PIZ_FS] 0x00495280 mount d:\toastart\common\Panel\Panel.piz -> ...: OK, 15 entries (depth 1)
[PIZ_FS] 0x00495280 mount d:\ToastArt\common\perm.piz -> original/ToastArt/common/perm.piz: OK, 26 entries (depth 1)
```

### RW rows (LibRw/RwApiBind.cpp)

- **RwTextureDestroy 0x004c5a60 -> rw::Texture::destroy.** The matched body (re/match/rwl01/004c5a60.c) decrements
  the count at +0x54. At <= 0 it runs the plugin destructors of registry 0x00618138 (0x004d8060), unlinks +8 / +0xc
  when +4 is set, destroys the raster at +0 (0x004c7650) and frees through RwGlobals +0x11c. Those are RwTexture's
  refCount / dict / lInDictionary / raster offsets, and librw's destroy does the same. The binding skips a null
  pointer. 0x00428400 passes four of them (DAT_0067d964..6c), all 0 on the knob path while the fonts family is a
  stand-in.
- **RwImageSetPath 0x004ce2d0 -> rw::Image::setSearchPath.** The matched body copies the string into the image
  module's path buffer. 0x00428400 passes 0x005cc4de, which is `""` in the anchored binary (bytes at 0x005cc4de are
  `00 00`, then `[%s] has %d` at 0x005cc4e0).
- **RpWorldDestroy 0x004e5700 -> rw::World::destroy.** Null skipped. Not reached on this run: 0x0042c2a0 calls it only
  when DAT_0067ea00 != 0.
- **0x004d41e0: shim.** [UNCERTAIN] it has no recorded RW catalog name, and librw has no counterpart for its record.
  0x00496ce0 passes DAT_00773094 / DAT_00773088, the two handles PCVS_Init 0x00496e40 creates. PCVS_Init is hand
  piece 1, which creates neither, so both are 0. A null record is skipped, a non-null one is logged and left alone.

New knob-exit lines: `0x004d41e0(0x00000000): shim, null` x2, `RwImageSetPath("")`, `RwTextureDestroy(0x00000000) ->
null` x4. The four `PORT-TOP-04C` stand-ins of these RVAs are deleted.

## Knob-ON boot (`MASHED_ORIG_TOP=1 MASHED_DET_FRAMES=3000`)

Batch 1 (exe `40b78e35`) and batch 2 (exe `cb722fb9`): exit 0 after 3000 frames (36.1 s / 26.5 s),
`0x00402750 AppInitialiseOnBootup (matched) returned 1`, `WinMain 0x00492370 returned 0 after 3000 frames`.

## Captures

`MASHED_NAV_DEMO=1 MASHED_DETERMINISTIC=1`, runs knob ON, OFF, OFF, ON in root `D:/MashedWT/PORT-TOP-04E_rootK`, settings
baseline `efd5da58...` restored before each. All exit 0 with `NAV_DEMO phase=28 done ok=1`. **12 / 12 captures
byte-identical across the four runs in each batch, and batch 2 equals batch 1.** They are the same files as 04B / 04C /
04D (`walk_02_mainmenu` = `24249b644e3e8057`, `walk_04_colourselect` = `2871487f1fcef51c`). The full list (sha256/16):
walk_01 `83c8cbb913899494`, 02 `24249b644e3e8057`, 03 `feb2c803554cd028`, 04 `2871487f1fcef51c`, 05 `9f02cb91d09e0500`,
06 `b1f095c07072b28d`, 07 `3465159db8526481`, 08 `843e3fc49d166754`, 09 `5db0164fb2c6df8c`, 10 `ca2f64dc5e7a9407`,
11 `536f710d21e50619`, 12 `6d41bc3ac8ad8d8a`.

## Default gates (knob OFF)

`run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`, `A4_ROOT=D:/MashedWT/PORT-TOP-04E_root`. All four arms exit 0 and
self-exit (H_DEF 118 s, B 12 s each), on both exes.

| gate | batch 1 `40b78e35` | batch 2 `cb722fb9` | merge_train.json ref |
|---|---|---|---|
| G-CTRL | `8284be46/67931834/31fe043e` | `8284be46/67931834/31fe043e` | same |
| B | `1d5bf4f9` x3 | `1d5bf4f9` x3 | same |

The first batch-1 gate attempt failed before any arm ran (the driver's `arms` output folder did not exist, run_arms.py
line 294). It was re-run with the folder created (`gates.log`).

## Lint, build, machine rules

`cmd /c "set MASHED_NO_DEPLOY=1&& mashedmod\build.bat"`, both targets OK, rva-lint `OK -- no NEW duplicate bodies (1
known)`, asm-lint OK. The first build failed rva-lint: two comment lines starting with an RVA (`// 0x00402750 ...` over
a hand piece, `// 0x00428400 ...` over the texture binding) made the scanner read them as second bodies. Both were
reworded. Every build ran with `mashed_lock.py status` showing no PRIORITY-HOLD or SOLO (the first waited for train
pass `20261109_080025` to clear). Every game launch ran through `D:/MashedWT/PORT-TOP-04E_data/run_all_04e.py` inside
`mashed_lock.py run --label PORT-TOP-04E`, inside `test-slot`.

## Port column (scripts/backfill_exe_file.py, then scripts/backfill_port.py --allow-stale)

**6 rows changed**: 0x00402750 none -> called (Match/PortTop_00402750.cpp), 0x00499ce0 none -> called
(Match/PortTop_00499ce0.cpp), 0x004274d0 none -> called (Match/MatchS06_004274d0.cpp), 0x00431b40 none -> called
(Match/MatchS07_00431b40.cpp), 0x00498bc0 body -> called, 0x00498bd0 body -> called.

A first pass also credited 0x00403640 to the pieces file. That was an overclaim: the piece is the hand stand-in, not
0x00403640's body. A comment line in the pieces file started with `// 0x00403640`, so the scanner took it as the body
tag. After the build, the line was reworded (comment only, in the RESULT commit) and both scripts were re-run from
the committed hooks.csv. `--allow-stale` is needed because that comment edit is newer than `mashed_re.map`. The four
RW rows are third-party library rows and get no exe_file. BINDINGS.tsv: `update_bindings_top04e.py` (it runs the 04D
updater first, then reads the 58 callee rows from the #define lines of Match/PortTop_00402750.cpp), 374 rows:
matched 117, stand-in 96, bound 37, under-stand-in 31. Callees of 0x00402750: matched 8, bound 6, stand-in 44.

## Handed back

1. **Loader families (step 4).** Each family's matched body pulls in deep trees that need RW stream / texture /
   raster bindings first. For example, 0x004274e0 streams english.dat through 0x004cc230, 0x004283a0 reads four
   textures through 0x004c5cb0, and 0x0040bbb0 loads four TXDs from sfx.piz and calls about 20 more. The next rows:
   fonts (0x004283a0 / 0x00427ca0 / 0x004275d0 / 0x004274e0), sprite dictionaries (0x0040bbb0 / 0x0040bb50 /
   0x0040bb30 / 0x004881d0), panels (the nine Panel.piz callees + 0x0041eaa0 / 0x00420d00), powerups (0x0045bae0 /
   0x00418980), perm (0x00425bc0 / 0x004025f0), pc.txd (0x004b3d80). Each has to keep the 12 captures identical.
   `PortTop_PizFs_Find` serves their piz reads.
2. **The six teardown callees** 0x004014b0, 0x0041e0d0, 0x00489250, 0x0041ffb0, 0x004196f0, 0x00412010 stay on their
   04C stand-ins. Each reads a pointer the loader stand-ins above leave at 0 (04C table), so they can move only after
   their loader.
3. **0x004caea0**: the device stand-in. Forwarding the two SetRenderState calls to the device has to go through
   librw's render-state cache, or librw's cached state no longer matches the device.
4. **0x004d41e0**: RW catalog identity not recorded ([UNCERTAIN] above).
5. Still open from 04D: the 0x004924f0 decision (row PORT-TOP-04D-WALK04) and the save restore of 0x007f0a40.

## Raw data (D:/MashedWT/PORT-TOP-04E_data)

| file | sha256 |
|---|---|
| knob_on_1.log (batch 1) | 870df547653976f3cdb984f504dbc594284d500a8f7e6daaf85605d535d83b69 |
| navdemo_knob1.log / navdemo_knob2.log (batch 1) | 5ec166a3817a3ad9a1784e52088f5f8d21e451b4bb50a69f823a84aec59223d8 / 8f85334ca47d8e5aae9a7b937137a85327c281def751c0ca3dbdb149307630a4 |
| navdemo_hand1.log / navdemo_hand2.log (batch 1) | 4a8d03a1b9019afc74158e243f994013638e8d4411dd1e99b17bbc9c9f57f1d7 / d61cd1b2c180f622fb0cfcb9f0941fe941a369d51dcf3e292afb11dfa19c793e |
| arms/H_DEF.step.csv (batch 1) = b2/arms/H_DEF.step.csv | 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198 |
| run_all.log / gates.log (batch 1) | 88c636c4c0838c8fb1232e8e00ded35365656c98324fbeb1cb128e10466864d1 / c7cfa2a454599105b9c5899127ce911746768281a53c22cd95d451d616f3a22e |
| b2/knob_on_1.log | c77f4eae9e2508b4e735ca14882d26f0282d8f169d79e47153ce47f046201443 |
| b2/navdemo_knob1.log / b2/navdemo_knob2.log | 1f6aecbe1f8562d8cbe234f21d050083171d25c65453d0886ca2e077c178eb65 / ce6a70e051c9476e9c3f43aed5d8aac8b59fd1a981ad2f3df49c04a39232ce79 |
| b2/navdemo_hand1.log / b2/navdemo_hand2.log | 1900eb111b7964d20decdc557762339713b43650d3833bb94bd7166bdba2da09 / 7a4d8082edd4bd1a113f379f91a9c76e9882a19b5a3a749dfd3cb1a4a441fdd2 |
| b2/run_all.log | 95055abcbe7052824ccc78db49fcf11183f95f93999e7afcb8b4bdb028426ba4 |
| build2.log / build3.log | 616de0b210c149eac64cbfd03923c61e8e67ce12b82a5f6b423c6473f655dfda / 1873ab4f992493a1b80cfd68401fb99e5f4ec96aa2eab2af46b76b2254bddf5d |
| run_all_04e.py (driver) | in the same folder |
