# PREREG d1r1: D1-residue R1, matched-pose in-race draw-list adjudication

Committed before the first measuring run. Base: `race/first-frame-parity` 118fb99d (R2 merged).
Branch `work/d1r1`. Evidence dir `verify/d1r1_20261016/`, bulky captures under `log/d1r1/`.

## 0. What R1 asks, and what exists today

ROADMAP §D1 D1-residue (R1): "an original-side capture at a matched, frame-synced pose adjudicates
faithfulness (`drawlist_diff.py` GREEN or every row cited)".

Surveyed at base (no new RE):
- `re/tools/drawlist_diff.py` is Im2D only (0x1c screen-space vertices, `:43`). It cannot read a 3D draw.
- The original-side in-race draw record is aggregate only: the d3d9 shim's `<bmp>.draw3d.json` is one
  line of totals (`d3d9_shim.cpp:633-642`), no texture, state or matrix.
- The standalone's `MASHED_DBG_DRAWSTREAM3D` is per-category counts written only from legacy
  `TrackRenderer.cpp`; on the default librw build `world` is 0 and `cars` is absent (memory
  "drawstream3d cars is legacy-only").
- The frame sync that works is R2's row-indexed replay (`RESULT_d1r2.md` §6), exact to the frame on
  TRAINING. Only the camera is replayed. `camera_probe.py` has no track flag (Quick Battle always
  races TRAINING).

So R1 needs a new instrument on both sides plus a 3D mode in the differ. This file fixes the
instrument, the gates and the decision rules before any of them is run.

## 1. Instrument

**I-1 Original-side per-draw recorder** (`re/frida/camera_probe.py --drawrec`). Armed for the shot
row's frame only (two frames of Interceptor, then detached; not a hot-path trace). Device pointer
`DAT_007d4110` (cited at `0x004dbc34`, the D3D device COM pointer whose vtable `+0x144` is
DrawPrimitive). Hooks the device vtable slots 81 DrawPrimitive, 82 DrawIndexedPrimitive,
83 DrawPrimitiveUP, 84 DrawIndexedPrimitiveUP, and 17 Present as the frame delimiter. Per draw it
records: slot, primitive type, the call's counts, the stage-0 texture (device GetTexture(0)) resolved
to its RW name, a fixed render-state set read back from the device at the draw (ZENABLE, ZWRITEENABLE,
ZFUNC, ALPHATESTENABLE, ALPHAREF, ALPHAFUNC, ALPHABLENDENABLE, SRCBLEND, DESTBLEND, CULLMODE,
FOGENABLE, LIGHTING), D3DTS_WORLD, FVF, whether a vertex shader is bound, and the stream-0 stride.
Texture names: `RwTextureSetName` `0x004c5ae0` (hooks.csv C3) is hooked from boot (load-time only, a
few thousand calls), texture -> name; at arm time each texture's raster `+[DAT_00911ae4]` (the D3D9
raster extension offset, cited at `0x004d6cf4` in `FUN_004d6ce0`) gives the IDirect3DTexture9*, which
yields the d3dtex -> name map.

**I-2 Standalone per-draw recorder** (`MASHED_DBG_D3DREC=<dir>`, new, diagnostic only, default
build unchanged when unset). Same vtable slots on the standalone's own device, armed for the replay
shot frame, same record schema, same render-state set read back from the device. Texture names from
the textures the standalone created (librw rasters + any direct D3D9 textures), keyed by the
IDirect3DTexture9*.

**I-3 Pose transplant on the standalone** (diagnostic knobs, default unchanged when unset):
- `MASHED_CAM_REPLAY_BASIS=1` (with `MASHED_CAM_REPLAY`): the camera frame of the frame that consumed
  row K is the original's own `M_*` (RwCamera frame modelling matrix at director onLeave, which R2
  measured to be what renders) and its view window `rvwx/rvwy`. R1 adjudicates rendering, not the
  director: R2 already closed the director on TRAINING, and this removes any director residue on the
  other tracks from the comparison by construction.
- `MASHED_CAR_REPLAY=1`: each car's rendered root matrix is the original's, from new trace columns
  (the render root walk `*(0x0063da18 + i*0x2ac)` -> `+4` -> `+0xa0`, documented in
  `verify/d1_carproj/RESULT.md:13-14`). Which of modelling/LTM renders is decided by the same
  consecutive-row test R2 used for the camera, before any adjudication run, and recorded.

**I-4 Track choice on the original.** `camera_probe.py --track N` writes the engine and menu track
indices `0x0063ba7c` / `0x0067f17c` (the pair `scenario_launch.py:3290-3291` writes; index map
0=Training .. 12=City) inside the track-load entry, so the Quick Battle flow loads track N. The
loaded piz is verified from the original (CreateFile watch, as `race_draw_burst.py:53-96`), never
assumed.

**I-5 Differ** (`re/tools/drawlist_diff.py`, new 3D record shape, auto-detected). Two levels:
- *draw level*: draws matched as a multiset on the signature (texture name, primitive type, vertex
  count, primitive count, render-state tuple); for matched pairs with a non-identity world matrix on
  both sides, translation must agree to 0.01 world units and the 3x3 to 0.01.
- *group level*: per (texture name, state tuple) the draw count and primitive sum on each side, so a
  batching difference shows as a group with equal sums but different draw counts.
- A `--cite <json>` file maps row patterns to a cause string. Verdict: GREEN (no unmatched rows),
  CITED (every unmatched row hits a citation), RED otherwise. Exit codes 0 / 0 / 1.

## 2. Instrument gates (must pass before a track's adjudication counts)

| gate | pass condition |
|---|---|
| **I-COVER** | on the original, the recorder's draw count for the shot frame equals the shim's `draw_calls` in that frame's `draw3d.json` (the shim counts every draw between two Presents) |
| **I-FRAME** | the recorded draws all lie between the two Presents bracketing row K, and row K's director onLeave precedes the first of them (else the frame is not row K's) |
| **I-NAME** | at least 95% of textured draws resolve to a name on each side; the rest are listed by size/format |
| **I-SYNC** | the standalone's shot logs `actual_row == K`, and its librw camera frame equals the trace `M_*` of row K to 1e-5 (read back like R2's G-WIRE) |
| **I-N0** | two standalone replay runs of the same trace and shot give byte-identical draw records |
| **I-SELF** | the differ scores orig-vs-orig and stand-vs-stand(run 2) GREEN |
| **I-MSHIFT** (mutant) | the original's frame K against the standalone's frame K+30 is RED with at least one world (track geometry) row. A GREEN here means the differ is blind to pose and every verdict is void |

## 3. Adjudication

Tracks: **TRAINING plus two others** (planned Arctic `--track 3` and City `--track 12`; if a track
cannot be captured cleanly the substitute is named with the reason). At least one racing-row shot
per track (`--shots-race`), three where the capture allows.

Each unmatched row is resolved one of three ways:
- **FIXED**: the original's behaviour is cited to RVAs and disassembly, the change is in render code
  only, a re-run shows the row matched, and the collateral rules in §4 hold.
- **CITED**: the cause is stated with evidence (standalone code site and original RVA, or a capture
  fact), or it points at a new or existing UNCERTAINTIES row.
- otherwise it stays RED and R1 does not close.

R1 closes only if every row on every adjudicated frame is matched, FIXED or CITED.

## 4. Fix rules (decision pre-registered, not the diagnosis)

A fix is kept only if all of these hold. It is then wired default ON with an opt-out knob:
1. the fixed row matches on every adjudicated frame where it occurs;
2. no row that matched before the fix stops matching on any adjudicated frame;
3. **G-CTRL** on the default build is unchanged (`991a5fff / be0d47ea / bc75b405`), since a render fix
   must not touch the sim;
4. **no-op control**: the fix's opt-out arm reproduces the pre-fix draw record byte for byte;
5. the R2 pixel scores on the TRAINING shots (`imgdiff` against the original BMP) do not rise by more
   than 0.5.

Failing any of 1-4, the fix stays default OFF with a recommendation. Failing 5 only, it is kept and
the rise is reported with its cause.

## 5. Predictions (recorded to keep the instrument honest)

- **Pr-1** AI car bodies: texture names differ. The librw branch submits every AI car with
  `rw_car_model_` (`TrackRenderer.cpp:7429-7441`), while the port's own selection gives AI cars
  liveries 1..3 (`exe_main.cpp:9197-9200`, `FUN_0040d110`).
- **Pr-2** Wheels: if world matrices are comparable, wheel draws differ in matrix. On librw every
  wheel is baked under the body matrix at rest (one clump, survey 2026-10-16), and the original has
  per-wheel frames.
- **Pr-3** Car bodies before `MASHED_CAR_REPLAY`: matrices differ (yaw-only build at
  `TrackRenderer.cpp:7414-7424` against the original's full frame); after it, they match.
- **Pr-4** In-race Im2D: the original issues a transparent quad per driving frame
  (`race_hud_burst.py:45-53`). No prediction for the standalone.
- No prediction for sky, water, props, copters, shadows, particles or pickups.

## 6. Baseline already measured (before this file)

G-CTRL on the base build (`log/d1r1/base/mashed_re.exe`, built from 118fb99d): `991a5fff / be0d47ea /
bc75b405`, equal to the recorded default (`gctrl.txt`). That is a fingerprint, not a measurement of
anything this file predicts.

## 7. Amendments

Any change to the instrument or a gate after this commit is appended here with a timestamp and the
reason, before the next adjudication run on the affected track.

### Amendment 1 (2026-10-16 ~03:40, before any adjudication run)

Two instrument smoke runs on TRAINING (`log/d1r1/smoke_t0`, `smoke_t1`; 4 original shots, 4
standalone replays of them) showed that matching on call arguments cannot work, for structural
reasons that are facts of the two renderers, not findings about faithfulness:
- The original draws indexed **triangle strips** (`pt=5`, RW's D3D9 instancing). librw draws
  **triangle lists** with unshared vertices (`pt=4`). Primitive and vertex counts never agree.
- librw issues a DrawIndexedPrimitive for every **empty** material mesh (every sector and car
  atomic carries the full material list): 18 083 zero-primitive calls in one TRAINING frame. A
  zero-primitive call rasterises nothing.
- librw draws through vertex/pixel shaders, so FVF, LIGHTING and the texture-stage ops are not
  comparable with the original's fixed-function values.

Changes, all made before any adjudication run:
1. **Match on the geometry drawn.** Both recorders now also read, per draw, the non-degenerate
   triangle count and the world-space AABB of the vertices those triangles use (READONLY lock of
   the bound index/vertex buffers, or the UP pointers; positions transformed by the draw's world
   matrix). A draw's identity is (stage-0 texture, triangle count, AABB within 0.01 + 1e-4 x
   extent). The original recorder's lock path read 481 of 483 draws on `smoke_t1` with no error.
2. **Zero-primitive draws are dropped** on both sides and counted (`info.empty_dropped`).
3. **Stage-1 texture recorded** (`tex1`): the original's world meshes are two-stage (stage 1 =
   `NewShadow` / `NewShadow_Small`, COLOROP 13), its car bodies carry `CarGloss` or an unnamed
   128x128 texture there. A matched draw whose stage-1 texture differs is a TEX1 row.
4. **Key render states** = ZENABLE ZWRITE ZFUNC ATEST AREF AFUNC ABLEND SRC DST CULL, with
   don't-care fields zeroed (ZFUNC when Z is off, AREF/AFUNC when alpha test is off, SRC/DST when
   blending is off). FOG is reported as info on matched draws. LIGHTING, TSS ops, FVF and VS are
   recorded but not compared.
5. **Off-screen evidence.** MISSING/EXTRA rows count how many of their draws lie wholly outside
   the view frustum of the trace row (the original's `M_*` frame and `rvwx/rvwy`, near 0.1 / far
   360 from `verify/d1_lens`). A draw wholly outside rasterises nothing.
6. **Row kinds** are STATE, TEX1, MOVED, SPLIT, MISSING, EXTRA, ORDER (`re/tools/drawlist3d.py`
   header). MATRIX is dropped: the AABB is already in world space.
7. **I-MSHIFT restated**: the mutant must turn matched world/car geometry into MOVED/MISSING
   rows, which a camera-only shift does not do to identity-matrix world sectors. So the mutant
   pairs the original's row K with the standalone's row K+30 **including** the car transplant, and
   must show car-body MOVED rows that the matched pair does not have.

Gates I-COVER, I-FRAME, I-NAME, I-SYNC, I-N0, I-SELF and the §3/§4 rules are unchanged.
