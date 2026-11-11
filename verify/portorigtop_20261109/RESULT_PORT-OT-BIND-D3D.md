# RESULT PORT-OT-BIND-D3D (L-111 step 2, 2026-11-11)

Verdict: **GREEN for the binding, with a split for the rest.**
- 0x004944c0 is bound on the exe's own DirectShow layer and device (`Match/PortTop_MovieBind.cpp`).
- The matched 0x00494a80 is compiled in the exe with that binding as its callee.
- The runnable set goes 1,027 -> **1,028**.
- The other 21 rows of the family are not bound. They are DirectShow base-class methods of the original's own renderer
  COM object, which the exe replaces instead of hosting. That needs a lead decision (section 4).

Branch `orch/PORT-OT-BIND-D3D` from orch/integration `25f36aeba`.

## 1. The family on this tree

PORT-ORIGTOP-SIZE counted 24 on `488fa1d9f`. On `25f36aeba`, `origtop_size.py` puts **22** `fam:d3d-dshow` rows in
reach. That is the `platform_candidates.csv` `signal=platform-api` rows plus the `directshow-baseclasses` library rows.
The edges below come from the same tool.

| row | identity, from the code | reached from |
|---|---|---|
| 0x004944c0 | DirectShow graph wrapper (section 2) | 0x00494a80 (matched) |
| 0x0049c6e0, 0x0049dc20 `CBaseVideoRenderer::NonDelegatingQueryInterface`, 0x0049dcb0, 0x0049de60, 0x0049de70, 0x0049de80, 0x0049e140, 0x0049e1f0 `CBaseRenderer::Stop`, 0x0049e2c0 `::Pause`, 0x0049e3b0 `::Run`, 0x0049ed70 `CBaseFilter::GetClassID`, 0x0049edb0 `::SetSyncSource`, 0x0049ee20 `::GetSyncSource`, 0x0049eed0 `::StreamTime`, 0x0049ef10, 0x004a0f50 `CBaseFilter::EnumPins`, 0x004a1100, 0x004a1120 `CUnknown::NonDelegatingRelease` | DirectShow base-class methods (hooks.csv `third-party-library[directshow-baseclasses]`) | address-taken only: the vtables the renderer constructors 0x00494ac0 / 0x00494b20 store, and the base constructors 0x0049ebc0 / 0x0049ec10 |
| 0x0049ebc0, 0x0049ec10 | base-class constructors of the same object | 0x00494b20 / 0x00494ac0 |
| 0x004a1790 | CComPtr destructor: `p = *p; if (p) p->Release()` (vtable +8) | 0x00494c80 only |

0x00494ac0 / 0x00494b20 / 0x00494c60 are reached only from **0x00494c80**. That is the small.mpg graph builder (C4,
100% byte match). On the knob path it is the PORT-TOP-04E stand-in (`PortTop_StandIn04E_00494c80`, returns 0). So 21
of the 22 rows sit behind 0x00494c80. 0x004944c0 is the one that gates a matched row directly.

I found no game logic in any of the 22. 0x004944c0's only game-side input is the movie index, which it stores into
0x00771a1c.

## 2. Identity of 0x004944c0 (L-116: from the code, not the filing)

Body: `re/match/s16/004944c0.c` (C2, 59.2%). GUIDs read from `original/MASHED.exe.unpatched`:

| address | GUID | interface | use |
|---|---|---|---|
| 0x005d0c4c | e436ebb3-524f-11ce-9f53-0020af0ba770 | CLSID_FilterGraph | CoCreateInstance -> 0x00771a30 |
| 0x005d0abc | 56a868a9-0ad4-11ce-b03a-0020af0ba770 | IID_IGraphBuilder | the same call |
| 0x005d0bcc | 56a868b1-... | IID_IMediaControl | QI -> 0x00771a34, Run (+0x1c) |
| 0x005d0bbc | 56a868b2-... | IID_IMediaPosition | QI -> 0x00771a2c |
| 0x005cfac4 | 56a868b6-... | IID_IMediaEvent | QI -> 0x00771a28 |
| 0x005cfad4 | 56a868b3-... | IID_IBasicAudio | QI -> 0x00771a20 |

The rest of the body:
- **Renderer.** operator new(0x170), then the renderer constructor 0x00493c00 (vtable 0x005cf9e8). Its IBaseFilter
  (obj + 0xc) goes into 0x00771a24, after the old one is released.
- **Graph build.** AddFilter "TEXTURERENDERER", AddSourceFilter on `[0x006147c4 + idx*4]`, FindPin "Output", Render.
- **Globals at the end.** 0x00771a00 = 0, 0x006147d8 = 1, return 0.
- **Texture side, inside Render.** The renderer's SetMediaType 0x00494820 (`re/match/s16/00494820.c`) creates two
  D3DFMT_X8R8G8B8 textures on the device at 0x00771a0c, into 0x00771a10 / 0x00771a14. It also writes the size floats
  0x007719ec..f8 and the format 0x007719fc.

The matched readers of that state:
- 0x004942b0 (event poll: GetEvent through 0x00771a28, put_CurrentPosition through 0x00771a2c).
- 0x00494320 (close: Stop and Release of 0x00771a34, Release of 0x00771a28 / 2c / 30 / 24 / 20, and an
  **unconditional** Release of 0x00771a10 and 0x00771a14).

## 3. The binding

**`PortTop_Bound_004944c0(idx)`** (`Match/PortTop_MovieBind.cpp`, exe only).

One owner. The exe owns the device (`PortTop_ExeDevice()` = exe_main `g_device`) and the graph (`MpegVideoTexture`:
FilterGraph -> SampleGrabber RGB32 -> NullRenderer). The original's renderer object is never constructed, and
0x00771a0c is neither read nor written.

The binding publishes **AddRef'd** pointers into the original's globals:
- graph -> 0x00771a30, IMediaControl -> a34, IMediaPosition -> a2c, IMediaEvent -> a28, IBasicAudio -> a20
- the SampleGrabber's IBaseFilter -> a24, after releasing the old one, as the original does
- the one texture, twice -> a10 / a14
- the floats 0x007719ec..f8 and format 0x16 -> 0x007719fc
- 0x00771a1c = idx, 0x00771a00 = 0, 0x006147d8 = 1

Each slot holds its own reference, so the matched 0x00494320 releases exactly what was published. The exe object keeps
its own references until it closes. It closes on the next open, or in the self-test.

Port choices, each a difference from the original:
- **Textures.** One texture in both slots, where the original double-buffers. The texture is D3DUSAGE_DYNAMIC in
  D3DPOOL_DEFAULT (the original: usage 0, MANAGED). The format is the same, 0x16.
- **Row order.** Rows stay bottom-up, as the original's renderer stores them. This is the new
  `MpegOpenOptions::flip = false`. The default `true` keeps every existing caller unchanged.
- **Failures** return 0x80004005 and log the reason, instead of the 0x00493f00 message box.
- **Frame upload** needs `PortTop_MovieBind_Pump()` on the render thread. The original's renderer copies frames on the
  DirectShow streaming thread. No knob-path caller of the pump exists yet (section 5).

**0x00494a80.** `Match/MatchS16_00494a80.cpp` is now also in `exe_sources.rsp`. Under MASHED_STANDALONE its callees
bind by name: 0x004944c0 -> the binding, 0x004c77c0 -> `MashedRw_RasterCreate_004c77c0`. The .asi keeps its
raw-address forwarders. On the knob path no row calls raw 0x004944c0 any more.

**BINDINGS** (`update_bindings_otbindd3d.py`, through `bindings_rows.apply`, 510 -> 512 rows):
- 0x004944c0 `bound`.
- 0x00494a80 `under-stand-in`. Its callers 0x00495350 (piece 2) and 0x0043dfd0 (under the 0x00492d30 stand-in) do not
  run it yet. I did not mark it `matched`, because that would count it as running.

**Self-test.** `MASHED_DBG_MOVIEBIND_SELFTEST=1` (default off) runs from piece 2 after the boot movies:
1. Matched 0x00494a80(0, 0, 0), which opens frontend.mpg through the binding.
2. 60 frames of pump plus GetEvent on the published IMediaEvent.
3. Matched 0x00494460 -> 0x00494320 close.
4. A reference check.

## 4. Decision: the other 21 (split)

None of the 21 has a target in the exe. They are virtual methods and constructors of the original's
`CBaseVideoRenderer` subclass. The exe does not host that object: its DirectShow layer uses SampleGrabber +
NullRenderer. Binding each method to "an exe function" would mean writing a delegating COM proxy inside the 0x170
bytes the matched 0x00494c80 allocates. I did not do that. Options for the lead:

| option | effect on the measurement | cost |
|---|---|---|
| (a) bind **0x00494c80** as a whole, as an exe small.mpg builder (same publish pattern into 0x00771a38..4c; `g_small_mpg`, built in piece 2 today) | the family leaves reach (0 rows). Runnable **1,023** (simulated on this tree: -5, the matched helpers 0x00493b50 / 0x00493bc0 / 0x00493f00 / 0x004944b0 / 0x00493b40 reachable only through the two builders leave reach) | replaces a 100% matched body with a binding |
| (b) vendor the DirectShow base classes (strmbase) so the matched 0x00494c80 / 0x00494ac0 run | the 21 bind by library | new dependency, needs Mariano's approval |
| (c) leave them behind the 0x00494c80 stand-in | 21 rows stay `fam:d3d-dshow`, alone +0 | none |

My recommendation is (c) now, and (a) if a later row needs 0x00402750's small.mpg call to run as matched code. The
measurement penalises (a) only because a bound node is not expanded.

## 5. Open, for the consumer row (not this row)

- **Frame upload.** The matched per-frame path 0x00494480 -> 0x004942b0 runs no upload. When 0x00495350 or 0x0043dfd0
  runs matched code, `PortTop_MovieBind_Pump()` needs a render-thread call point. Without one, the texture stays at its
  creation contents.
- **The draw** 0x00493fd0 (C0..C3) reads the published texture and floats. Once it runs, it puts v = max at the top,
  which matches `flip = false`.
- **Stale texture slots.** After 0x00494320, 0x00771a10 / 0x00771a14 hold the released texture (the original leaves
  them too). The binding owns the exe reference, so the texture stays alive until the exe object closes.

## 6. Gates (exe sha256 `21cb9c9938e4d49b4e8febca8a943a8486762c3428273a4d86509f6bd5b5e957`)

| check | result |
|---|---|
| build (`MASHED_NO_DEPLOY=1`) | `=== Build OK ===`, .asi not deployed |
| knob boot, RWMOD off | rc 0, 23.5 s, `WinMain 0x00492370 returned 0 after 3000 frames` |
| knob boot, RWMOD on | rc 0, 26.6 s, modules registered, 3000 frames |
| self-test, RWMOD off / on | rc 0 both (27.9 / 28.2 s), 3000 frames |
| 12 captures, RWMOD off (`capflake_runs.py nav1 OFF@dflt ON@dflt OFF@dflt ON@dflt`) | 4/4 `rc=0 pinned pos=55 hash=28538c74 class=E eqE=12` |
| 12 captures, RWMOD on (`nav_rwmod`) | 4/4 class E eqE=12 |
| walk_04 | `ba34030d1b7e2fe4` in all 8 rows. runs.tsv sha256 `f560646e…`, byte-identical to the POWERUPINIT / RWG-RASTER-BIND runs |
| G-CTRL (H_DEF) | `8284be46 / 67931834 / 31fe043e`, unchanged |
| B (K0 / K1 / K26) | `1d5bf4f9` x3, unchanged |
| rva-lint | OK, 35 known, 0 new |
| port column (`backfill_exe_file.py` + `backfill_port.py` on this build's map) | 0x00494a80 `none` -> `body` (exe_file `Match/MatchS16_00494a80.cpp`). The 9 rows 0x00474f90, 0x005aaaa0, 0x00487180, 0x004b4eb0, 0x00440bc0, 0x00562ac0, 0x005ac770, 0x005ac930, 0x0047f7d0 went from empty to `none` (regeneration of rows that had no value). 0x004944c0 stays `none`: the binding is not a body, and its comments are written so the scanner does not bind it as one. |

**Self-test lines** (`selftest_on.log`, identical shape in `selftest_off.log`):

```
[OT-BIND-D3D] 0x004944c0(0) original/toastart/pc/movies/frontend.mpg: graph 01695C00 ctrl 08EADF64 pos 08EAE078 event 08EADF80 audio 08EAE148 filter 08EAE21C texture 01694520 video 512x512 tex 512x512
[OT-BIND-D3D] self-test open: 0x00771a04=1 0x00771a18=016898b8 0x00771a1c=0 0x00771a00=0 0x006147d8=1 a10==a14 1 floats 512 512 512 512 fmt 0x16; refs graph 9 texture 3
[OT-BIND-D3D] self-test pump: 60 frames, 2 events (0 EC_COMPLETE), 4096 sampled texels nonzero
[OT-BIND-D3D] self-test close: 0x00771a04=0 0x00771a18=00000000 slots a10 01694520 a14 01694520 a20 00000000 a24 00000000 a28 00000000 a2c 00000000 a30 00000000 a34 00000000; refs graph 10 -> 5, texture 4 -> 2
```

How to read these lines:
- **0x00494a80.** The matched body ran its own stores: 0x00771a04 = 1, raster 0x00771a18 created.
- **0x00494460.** The matched body undid them: 0x00771a04 = 0, 0x00771a18 = 0.
- **0x00494320** cleared all six COM slots. The graph lost exactly the 5 published interface references (10 -> 5), and
  the texture lost its 2 (4 -> 2, the exe's own plus the test's).
- **Upload.** Frames reached the texture: 4096 of 4096 sampled texels nonzero, 4095 in the RWMOD-off run.
- **Earlier probe.** An earlier probe with movie 1 opened empire.mpg as the 167,414-byte skip_intro stub. It reached
  EC_COMPLETE within the 60 frames and left a black texture. That is why the self-test uses movie 0.

Launches: all inside `test-slot bash -c 'mashed_lock.py run --label PORT-OT-BIND-D3D -- ...'`. The build waited for the
merge train's hold. Roots: `D:/MashedWT/PORT-OT-BIND-D3D_root` (boots), `_rootK` (captures), `_rootA` (gates).

Raw files (not committed):
| file | sha256 |
|---|---|
| D:/MashedWT/PORT-OT-BIND-D3D_data/cap/runs.tsv | f560646e9825f27b38517ec0c4d851cd17931fc8944763b7fbaa1d1204796438 |
| D:/MashedWT/PORT-OT-BIND-D3D_data/boots/knob_off.log | b4f81f370693c8a589bcc094cf593e823df637cfdf230ec3d3761488ee0499d9 |
| D:/MashedWT/PORT-OT-BIND-D3D_data/boots/knob_on.log | d4f411f0f69959b530cb3c45994d53091a053c5952248f5550bafe4e40941035 |
| D:/MashedWT/PORT-OT-BIND-D3D_data/boots/selftest_off.log | 02dbcdaad31e8a41a5f93006073cdd59fbce2a504f784693f8f66e7f0c52b4f2 |
| D:/MashedWT/PORT-OT-BIND-D3D_data/boots/selftest_on.log | b303da59134028938ebc50f8e3a3cb3c72ecb2a1d5261a8cae72cf5bcc44bd2b |
| D:/MashedWT/PORT-OT-BIND-D3D_data/arms/H_DEF.step.csv | 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198 |

**Default path.** It changes in three places:
- `MpegOpenOptions::flip` (default true, same rows as before).
- Two accessor methods.
- `PortTop_ExeDevice()` in exe_main.

The self-test and the binding run only on MASHED_ORIG_TOP. The gates above are unchanged.

## 7. Re-measure (`origtop_size.py` on this branch)

| | before (25f36aeba) | after |
|---|---|---|
| runnable with no new family bound | 1,027 | **1,028** (+0x00494a80) |
| `fam:d3d-dshow` in reach | 22 | 21 |
| d3d family alone | +1 | +0 |
| cumulative with d3d bound | 1,047 | 1,047 |
| final (every binding family bound) | 1,236 | 1,236 |

SIZE.tsv is not committed. These numbers are reproducible by running the tool on this branch.
