# RESULT PORT-TOP-04F: 0x00402750 loader families off their stand-ins under MASHED_ORIG_TOP (L-96 top-down lane)

Branch `orch/PORT-TOP-04F` from `orch/PORT-TOP-04E` @ `b442a5254`, with `orch/integration` @ `5873e911e` merged in
(`790417bf4`, `git -c merge.autoStash=false`). Code commits `dffe29985` (bindings + fonts) and `44e67f784` (pc.txd +
perm). Evidence exe `0731b6c198640dbb` (sha256/16), built from `44e67f784`. The fonts commit was checked on its own exe
`623c5198f64f4aeb`.

Verdict: GREEN on what landed. Four loaders now run their matched bodies on the knob path: 0x004283a0, 0x004274e0,
0x004b3d80 and 0x00425bc0. The RW stream / texture / TXD bindings they need come first. The other families, the
two sprite lookups and the six teardown callees are handed back (below). Knob-ON boot exits 0. The 12 nav-demo captures
are byte-identical knob ON vs OFF after each family, and they are the same files as 04B..04E. The default gates are
unchanged.

## Scope landed vs handed back

| item from the brief | state |
|---|---|
| RW stream + texture bindings first | **done**: RwStreamOpen / Read / FindChunk / Close, RwTextureRead, the chunk-0x23 dictionary read 0x0054f8d0 (below) |
| fonts (0x004283a0, 0x00427ca0, 0x004275d0, 0x004274e0) | **2 of 4**: 0x004283a0 and 0x004274e0 matched. 0x00427ca0 / 0x004275d0 stay stand-ins: they sit on the RW Rt2d font toolkit (0x00552b60, 0x00552920, 0x00553cf0, 0x00554390, 0x00556d70, 0x00556e90), which has no exe body. The exe copy of 0x00427ca0 (Match/MatchS06_00427ca0.cpp) forwards to those raw addresses, so calling it would jump into unmapped original code |
| pc.txd (0x004b3d80) | **done** |
| perm (0x00425bc0, 0x004025f0) | **1 of 2**: 0x00425bc0 matched. 0x004025f0 (perm clumps) stays a stand-in: DFF reads through RpClumpStreamRead 0x004e7420 and geometry builders with no binding |
| sprite dictionaries (0x0040bbb0, 0x0040bb50, 0x0040bb30, 0x004881d0) | **handed back**. 0x0040bb50 / 0x0040bb30 were built as exe TUs of their matched sources and rva-lint refused them with CROSS-TARGET: the asi carries different bodies (Frontend/SpriteCluster.cpp, Util/PromoLoop_sessionB.cpp). One TU in both targets means replacing those asi bodies. Reverted, not committed. 0x0040bbb0 loads four TXDs from sfx.piz and calls about 20 more; 0x004881d0 builds RW frame / atomic objects |
| panels (nine Panel.piz callees + 0x0041eaa0 / 0x00420d00), powerups (0x0045bae0, 0x00418980) | **handed back**: their trees go through 0x004b3fc0 (no source), 0x004b5190 / 0x004b51d0 / 0x00543d40.. (element arrays), 0x0042a5d0 / 0x0042a6b0 (exe copies with raw forwarders) and RW frame / clump calls |
| six AppDestroy teardown callees | **handed back**: each reads state a loader still on a stand-in creates (0x004014b0: perm clumps of 0x004025f0; 0x00489250: 0x004881d0's objects; 0x0041e0d0 / 0x0041ffb0 / 0x004196f0: panels; 0x00412010: under 0x0045b930). None belongs to a family moved here |
| 0x004924f0 | left on its stand-in (row PORT-TOP-04D-WALK04) |

0x00402750's 58 callees now: matched 12 (was 8), bound 6, stand-in 40 (was 44). The stand-in count in
Match/PortTop_00402750_Pieces.cpp is 33 (was 37), and 31 of them are reached in the knob boot (was 35).

## Bindings (exe only, knob path only)

**Match/PortTop_RwStream04F.cpp.** In the original, RwStreamOpen rva 0x004cc230 with type 2 opens the name through the
RenderWare file system (RtFSManager), which the piz mounts feed. The exe has no RtFSManager (RtFSManagerOpen is a shim
since PORT-TOP-03). The open binding reads the whole file once, from the piz mount stack of 04E (newest mount first) or
else from the disk under `original/`. It returns an RW memory stream over that copy: [0] type 3, [1] access 1,
[3] position, [4] size, [5] base. That is the word layout of the case-3 branch of RwStreamRead (Render/RwStreamCore.h).
Declared deviation: a type-2 open gives a type-3 stream. Every reader on the knob path goes through RwStreamRead /
RwStreamSkip, which branch on the type word, so no reader sees a file handle.
- RwStreamRead rva 0x004cbd30 -> the exe body `mashed_re::Render::RwStreamCoreRead`.
- RwStreamFindChunk rva 0x004cc5e0 -> the exe body `mashed_re::D3d9Render::Rwl_StreamFindChunk`.
- RwStreamClose rva 0x004cc160: the matched body (re/match/s20/004cc160.c) returns the record to the RwGlobals +0x11c
  free list, which the exe's RwGlobals record does not carry. The binding frees the record and the copy (shim).
- `PortTop_PizFs_Find` now compares names case-insensitively with backslash and `/` equal. perm.piz holds
  `TOASTART/COMMON/PERM/COPTERS/COPTERS.TXD`, and 0x00425bc0 asks for `ToastArt/common/perm/Copters/Copters.txd`.

**LibRw/RwApiBind.cpp.**
- RwTextureRead rva 0x004c5cb0 (re/match/s19/004c5cb0.c): the find callback first (default 0x004c7490, the current
  dictionary only). A hit gets its count raised and is returned. Else the read callback (default 0x004c61b0, image
  files) runs, and a texture it reads is linked into the current dictionary when there is one. librw keeps the same
  current dictionary and find callback (`rw::Texture::findCB`). The read side is the exe's: `<name>.png` then
  `<name>.bmp` from the piz mount stack or the disk, decoded by the WIC decoder (D3d9Render/PngLoader.h) into a C8888
  `rw::Texture` named `<name>`. [UNCERTAIN] the original's extension order: 0x004c61b0 tries image formats registered
  at run time, and no registration list is recorded. The four font36.piz logos exist in one format each
  (MASHEDNEWLOGO.PNG, BIGE.BMP, PROLOGICII.BMP, DOLBYDIG.BMP), so the order picks the same file here.
- rva 0x0054f8d0 (re/match/rwl04/0054f8d0.c, 54.9%, not a matched body): reads the chunk-0x23 texture dictionary whose
  header RwStreamFindChunk has just consumed. The binding decodes from that header with the exe's decoder of the
  format (Txd/TxdDecoder.h, written from 0x0054f8d0) and builds the dictionary on librw, then moves the stream position
  past the chunk (shim). The decode-and-build step is now shared with 0x004b3eb0 (`DictFromTxdBytes`). The
  0x004b3eb0 load-icon dictionary is unchanged: `33932 bytes -> 1 of 1 textures`, as before.

**rdata seed.** The exe maps the original data range zero-filled. Matched bodies that read an original string get it
from Match/PortTop_RdataSeed.inc, written under the knob only. The first knob boot of the fonts build found two misses:
`RwTextureRead("")` for 0x005cd640 and `0x004cc230(2, 1, "")` for english.dat. `gen_rdata_seed.py` now also seeds a
single-char `MGLOBAL(char, ...)` the source passes by address as a string (up to its NUL, read-only sections only),
and it was re-run with 004283a0.c and 004274e0.c added. That adds 7 rows: 0x005cd5ac `USA.dat`, 0x005cd5b4
`Italian.dat`, 0x005cd5c0 `Spanish.dat`, 0x005cd5cc `German.dat`, 0x005cd5d8 `french.dat`, 0x005cd5e4
`english.dat`, 0x005cd640 `bigE`. The 27 earlier rows are unchanged.

## The four matched bodies

| rva | TU | source (match.py --bytes / relocheck, hooks.csv notes) | knob boot |
|---|---|---|---|
| 0x004283a0 | Match/PortTop_004283a0.cpp (new, exe only) | re/match/s06/004283a0.c BYTE-IDENTICAL 92 B, OK 15/15 | `MashedNEWLogo.png` 256x128, `bigE.bmp` 32x32, `proLogicII.bmp` 128x64, `DolbyDig.bmp` 128x64 from font36.piz -> DAT_0067d964 / 70 / 68 / 6c. AppDestroy's matched 0x00428400 destroys the four (`RwTextureDestroy(0x0...) -> rw::Texture::destroy` x4, where 04E logged `null` x4) |
| 0x004274e0 | Match/MatchS06_004274e0.cpp, now in both targets | re/match/s06/004274e0.c BYTE-IDENTICAL 148 B, OK 12/12 | `english.dat` 33626 bytes from font36.piz into DAT_0066d828, DAT_0067d84c = 1 |
| 0x004b3d80 | Match/PortTop_004b3d80.cpp (new, exe only) | re/match/s18/004b3d80.c BYTE-IDENTICAL 82 B, OK 4/4 | `toastart\pc\pc.txd` 33272 bytes from disk -> 4 of 4 textures -> DAT_00636ac8. AppDestroy 0x00402a40 destroys it |
| 0x00425bc0 | Match/PortTop_00425bc0.cpp (new, exe only) | re/match/s05/00425bc0.c BYTE-IDENTICAL 46 B, OK 5/5 | Copters.txd 151840 bytes from perm.piz -> 4 of 4 textures -> DAT_00644150. AppDestroy's matched 0x00425ed0 destroys it |

- Match/MatchS06_004274e0.cpp keeps one body for both targets. Under `MASHED_STANDALONE` its three RW stream
  callees bind to the stream bindings instead of raw-address forwarders, and the knob gate `LocalizationFileLoad`
  (MASHED_LOC_USA_DAT, index 5) is the entry 0x00402750 calls. The shadow A/B wrapper and the install stay asi-only.
- 0x00425bc0's source declares 0x004b3d80's first parameter `undefined4` and passes a string literal. C accepts that,
  C++ does not. The source text is unchanged: the TU declares a C++ overload taking `const char*`, the call resolves to
  it, and it forwards to the one C-linkage body.
- Exe readers of the new globals: the exe readers of 0x00771964 (logo) and 0x00636ac8 (pc.txd) are in
  Frontend/SkeletonAndScatter_t6.cpp and HUD/HudBatch.cpp, both asi-only. The hand frontend reads its own string table
  (g_menu_str, exe_main.cpp), not DAT_0066d828. The captures below are unchanged.

## Knob-ON boot (`MASHED_ORIG_TOP=1 MASHED_DET_FRAMES=3000`)

Fonts exe `623c5198` (b1b) and evidence exe `0731b6c1` (b2): exit 0 (28.3 s / 27.7 s),
`0x00402750 AppInitialiseOnBootup (matched) returned 1`, `WinMain 0x00492370 returned 0 after 3000 frames`. Every
dictionary created is destroyed in teardown (b2 lines 332 pc.txd, 386 Copters, 526 load icon).

## Captures

`MASHED_NAV_DEMO=1 MASHED_DETERMINISTIC=1`, runs knob ON, OFF, OFF, ON in root `D:/MashedWT/PORT-TOP-04F_rootK`, with the
04E settings baseline restored before each. All runs exit 0 with `NAV_DEMO phase=28 done ok=1`. **12 / 12 captures are
byte-identical across the four runs, after each family (b1b and b2), and equal to the 04E list** (`cmp_caps.py`):
walk_01 `83c8cbb913899494`, 02 `24249b644e3e8057`, 03 `feb2c803554cd028`, 04 `2871487f1fcef51c`, 05 `9f02cb91d09e0500`,
06 `b1f095c07072b28d`, 07 `3465159db8526481`, 08 `843e3fc49d166754`, 09 `5db0164fb2c6df8c`, 10 `ca2f64dc5e7a9407`,
11 `536f710d21e50619`, 12 `6d41bc3ac8ad8d8a`.

## Default gates (knob OFF)

`run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`, `A4_ROOT=D:/MashedWT/PORT-TOP-04F_root`, exe `0731b6c1`. All four arms
exit 0 and self-exit (H_DEF 119 s, B 12-13 s).

| gate | `0731b6c1` | merge_train.json ref |
|---|---|---|
| G-CTRL | `8284be46/67931834/31fe043e` | same |
| B | `1d5bf4f9` x3 | same |

## Lint, build, machine rules

`cmd /c "set MASHED_NO_DEPLOY=1&& mashedmod\build.bat"`, both targets OK, rva-lint `OK -- no NEW duplicate bodies (1
known)`, asm-lint OK, on both evidence exes. Three builds failed and were fixed before any evidence:
1. rva-lint DUP-IN-TARGET 0x00425bc0: a comment line in the PizFs_Find header started with `// 0x00425bc0`. Reworded
   to `// rva 0x00425bc0`.
2. C2664 in 0x00425bc0's source (string literal to `undefined4`): the overload above.
3. rva-lint CROSS-TARGET 0x0040bb50 / 0x0040bb30: the sprite lookups, reverted (scope table).

Every build ran with `mashed_lock.py status` showing no PRIORITY-HOLD or SOLO. The waits were for train passes
`20261109_115037` and `20261109_124725`. Every game launch ran through `D:/MashedWT/PORT-TOP-04F_data/run_all_04f.py`
inside `mashed_lock.py run --label PORT-TOP-04F[-gates]`, inside `test-slot`.

## Port column (scripts/backfill_exe_file.py, then scripts/backfill_port.py --allow-stale)

`--allow-stale`: the sources were restored by `git checkout` after the reverted sprite build, so their mtime is newer
than `mashed_re.map`. The content equals the `0731b6c1` build. **11 rows changed**:

- from this row (8): 0x004283a0 none -> called (Match/PortTop_004283a0.cpp), 0x004b3d80 none -> called
  (Match/PortTop_004b3d80.cpp), 0x00425bc0 none -> called (Match/PortTop_00425bc0.cpp), 0x004274e0 none -> body
  (Match/MatchS06_004274e0.cpp; backfill_port records body, not called. 0x00402750 calls the knob gate
  `LocalizationFileLoad`, which calls the body symbol `LocalizationFileLoadBody`. [UNCERTAIN] why the script does not
  follow that one level: its call-reach rule was not read in this row), and the RW stream bodies the new bindings reach: 0x004cbd30
  RwStreamRead, 0x004cc050 RwStreamSkip, 0x004cc400 RwStreamReadChunkHeader, 0x004cc5e0 RwStreamFindChunk, all
  body -> called.
- from the orch/integration merge, not this row's code (3): 0x00476ae0 and 0x00476b30 body -> called
  (Match/MatchS12_*.cpp, PORT-STATE-02 `6fa18d03c`), 0x0046dd80 none -> called (Match/MatchS11_0046dd80.cpp). 04F does
  not touch these TUs.

The RW binding rows (0x004cc230, 0x004cc160, 0x004c5cb0, 0x0054f8d0) get no exe_file: they are bindings, not bodies.
BINDINGS.tsv: `update_bindings_top04f.py` (runs the 04E updater, then writes 10 rows), 378 rows: matched 121,
stand-in 92, bound 40, port 40, rw 25, shim 15, under-stand-in 31, missing 12, crt 2.

## Handed back (for PORT-TOP-04G onward)

1. **Fonts, Rt2d part.** 0x00427ca0 / 0x004275d0 need the RW Rt2d font toolkit (0x00552b60, 0x00552920, 0x00553cf0,
   0x00553e80, 0x00554390, 0x00556cc0, 0x00556d70, 0x00556e90). librw has no Rt2d, so these are port bodies or shims
   first. The exe copy of 0x00427ca0 forwards to raw addresses and must not be called as is.
2. **Sprite lookups 0x0040bb50 / 0x0040bb30.** Replace the asi hand bodies with the matched-source TU in both targets
   (CROSS-TARGET otherwise), then bind. Their dictionaries stay 0 until 0x0040bbb0 (sfx.piz, four TXDs) moves.
3. **0x0040bbb0, 0x004881d0, panels, powerups, 0x004025f0**: RW frame / atomic / clump bindings (RpClumpStreamRead
   0x004e7420, RpClumpForAllAtomics 0x004e66d0) and the element-array helpers 0x004b3fc0 / 0x004b5190 first.
4. **Six teardown callees**: after their loaders (scope table).
5. Still open: 0x004caea0 SetRenderState through librw's cache, 0x004d41e0 identity, 0x004924f0 (row
   PORT-TOP-04D-WALK04), the save restore of 0x007f0a40.

## Raw data (D:/MashedWT/PORT-TOP-04F_data)

| file | sha256 |
|---|---|
| b1b/knob_on_1.log (fonts exe) | 87bc38f73aa3dc9a48e81f31f61e08eae5ddf48a58ac3f02b05a448be2746c48 |
| b1b_run.log | a29bdc565207e1e1d93e7a10ffaf37c96cf96f4e75fa5683016c6a71d2172e82 |
| b2/knob_on_1.log | e891ecd7e5a7ea2397f53901a84d54617db25b7d0274cb30b421a7050739ab6d |
| b2/navdemo_knob1.log / b2/navdemo_knob2.log | c23e4311c89268908f42bdd045d3ac29044656642f67b7c8c1a30d1b8b1186df / cdc839b3ddb49e0cd6461242d271b381600e10ef39e374e835c0206de6c490e9 |
| b2/navdemo_hand1.log / b2/navdemo_hand2.log | 94d983db1844e8efafe1ac971857c20f0996b29a9c506a1b13fdc660c46f9f51 / 7704e3c7f7ef4cc2aa22a11f9ef1ff77464679c01d8b69a174f4f58cd61714cb |
| b2_run.log | 2cefea4363714420fde958d2938ed29afd073fd5157ea355fb747fb2f562222f |
| b2/arms/H_DEF.step.csv | 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198 |
| b2/gates.log | 4821c597892c11aeec36a812bdf983031c5309301b6330fe50275a0c6bf443ca |
| build1b.log / build2.log | c0d79efbe3a7c4fd26b81e84a1cb5c78776d768025f8eff26206eace28f355a7 / 79a23ea1581fd61b68d8839ddaa4574a58879bc01cfb8eb74c7c1a9e00652671 |
| run_all_04f.py (driver) / cmp_caps.py (capture compare) | f5c16279739092a824933f9bbe7b034d120ed6894a5c7f9ee7b076ce204e563b / 97016dc8612968fb38875d264f446dfccba5e81dd03272f046da0e34929783b3 |
