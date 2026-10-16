# RESULT d1r1: matched-pose in-race draw-list adjudication (D1-residue R1)

**Verdict: R1 closes as written, every row CITED.** On 9 frame-synced in-race frames (TRAINING
884/924/959, Arctic 1051/1091/1126, City 761/801/836) `re/tools/drawlist_diff.py` (new 3D mode)
reports 81 to 157 divergence rows per frame against the original, and every one of them hits a
citation (`cites_d1r1.json`, 24 entries): **0 RED**. No row is GREEN-by-fix on the default build:
the one fix this lane produced (F1, RW D3D9 alpha/cull state) failed pre-registered rules 1-2 on
Arctic and stays opt-in. The default build is unchanged: G-CTRL `991a5fff / be0d47ea / bc75b405`
before and after on the 118fb99d base. After the rebase onto `98705947` (d2settle's A3 LUT sqrt,
default ON, plus camribbon) the default gives `c2e4d60d / b50bd39b / ba90ed88`, the new default
trio, and `MASHED_NO_A3_LUT=1` gives the old one. All 18 replays re-run on the rebased build give
the same rows and verdicts (`scores.txt`). All numbers are in `scores.txt`; pre-registration
`PREREG_d1r1.md` (committed before the first measuring run, now `6280d66e` after the rebase) with
amendments 1 and 2. U-ids are U-9688..U-9696 after the rebase (camribbon filed U-9687).

What "cited" means here is uneven, and the reader should weigh it. Some citations are
architecture differences that rasterise nothing different: no culling (every such draw is
off-screen), shader fog, a no-op quad. Most are features the standalone does not have yet:
shadows, start display, weapons, FX, the gloss pass, the two-stage world, per-track placement.
Eight new UNCERTAINTIES rows (U-9689..U-9696) back the ones whose cause is not fully mapped. R1 asks
for adjudication, and the adjudication is what closes. The faithfulness work the rows describe stays
open; the U-rows are its list.

## 1. The instrument (what did not exist at the base)

- **Original side**, `re/frida/camera_probe.py` (R2's row-indexed replay probe):
  - `--drawrec`: every IDirect3DDevice9 draw of each shot frame (vtable 81-84, device
    `DAT_007d4110`, `mov eax,[0x7d4110]` at `0x004dbc31`; Present delimits frames). Per draw:
    texture names on stages 0 and 1 (RwTextureSetName `0x004c5ae0`, raster -> D3D texture through
    `DAT_00911ae4`, `0x004d6ceb`), 12 render states and 3 texture-stage states read back from the
    device, D3DTS_WORLD, FVF, stride, and the **geometry actually drawn**: non-degenerate triangles
    and their world AABB from a READONLY lock of the bound index/vertex buffers.
  - `--carframes`: each car's renderable frame LTM per row (`*(0x0063da18 + i*0x2ac)` -> `+4` ->
    `+0x50`). It equals the body draws' D3DTS_WORLD in the same frame on 4/4 cars.
  - `--track N`: writes `0x0063ba7c` / `0x0067f17c` at `FUN_0040d440` entry (`mov eax,[0x63ba7c]`
    at `0x0040d440`), so Quick Battle loads track N; the piz actually opened is recorded.
  - `--drawrec-bt REGEX`: return-address chains for matching draws (the render sites cited below).
- **Standalone side**: `D3d9Render/DrawRec.{h,cpp}` (`MASHED_DBG_D3DREC`, inert when unset) records
  the same fields, with librw's texture and world matrix handed over by two null-by-default hooks in
  `deps/librw/src/d3d/d3d9render.cpp` (MASHED_PATCHES.md). `MASHED_CAM_REPLAY_BASIS=1` renders from
  the trace row's `M_*` camera frame and view window, `MASHED_CAR_REPLAY=1` from its `F{i}_*` car
  frames. Both default OFF.
- **Differ**: `re/tools/drawlist3d.py`, auto-selected by `drawlist_diff.py`. Draws match on
  (texture, triangle count, world AABB), because RW's D3D9 instancing draws indexed strips and
  librw unshared lists, so call arguments never agree (amendment 1). Rows: STATE, TEX1, FOG, MOVED,
  SPLIT, MISSING, EXTRA, ORDER. MISSING/EXTRA rows say how many draws lie wholly outside the trace
  row's frustum.

## 2. Instrument gates (PREREG section 2)

| gate | result |
|---|---|
| I-COVER | PASS 9/9: recorder draws == the shim's `draw_calls` for the dumped frame (242/242 ... 444/444) |
| I-FRAME | PASS 9/9: no draw between the Present before D(K) and D(K) |
| I-NAME | PASS: original 98.3-100% of textured draws named (the 4 unnamed are one runtime 256x256 texture, U-9691); standalone 100% |
| I-SYNC | PASS 9/9: shot row == K, recorder armed, camera src 4 (transplant), max deviation from `M_*` 4.5e-8 |
| I-N0 | PASS: two TRAINING replays byte-identical on 3/3 frames |
| I-SELF | PASS 9/9 orig-vs-orig GREEN (after amendment 2's line-list key fix), stand-vs-stand GREEN |
| I-MSHIFT | PASS: original 884 vs standalone 914 turns the car geometry into MOVED rows (Advantage 44 draws, up to 5.4; Glass 8; Sky) that the matched pair does not have |

`gates.txt` has the per-frame values. The probe's car-Y self-check flags Arctic and City; their cars
sit at y = 0 (Arctic `F0_py` = 0 at the grid and while racing), so that check is TRAINING-specific.

## 3. Adjudication, per track (3 frames each, default build)

| track | rows (3 frames) | GREEN by fix | CITED | RED | matched draws | matched triangles |
|---|---:|---:|---:|---:|---|---|
| TRAINING | 256 | 0 | 256 | 0 | 194 / 191 / 191 of 242 / 257 / 233 | 90.4% / 89.1% / 90.5% |
| Arctic | 243 | 0 | 243 | 0 | 138 / 138 / 101 of 180 / 177 / 131 | 90.8% / 91.2% / 91.5% |
| City | 464 | 0 | 464 | 0 | 351 / 339 / 292 of 512 / 496 / 444 | 81.2% / 81.0% / 77.7% |

With F1 opt-in the row counts are 61/60/60, 68/68/68, 89/89/86, all CITED, 0 RED.

Citations, rows per track (TRAINING / Arctic / City). Full notes, with every RVA, in `cites_d1r1.json`:

| id | rows | cause |
|---|---|---|
| C01 CULL | 66 / 39 / 81 | librw draws world atomics, props, copters with no frustum test; every draw in the row lies wholly outside the view frustum |
| C01b | 3 / 0 / 0 | TRAINING Cliff: 20/21 off-screen, one AABB straddles the frustum (U-9694) |
| C02 CARLOD | 9 / 9 / 9 | standalone draws every non-hull car atomic incl. LOD duplicates; all 44 Advantage / 8 Glass original draws matched (U-9079) |
| C03 UNTEXTURED | 3 / 3 / 3 | per-car 44-tri mesh drawn depth-off before each body (`0x004ec3e1` <- `0x0041faa0` <- `0x004200c6`), start-display backings, a 20-line particle list (`0x0049a081`), a no-op full-screen Im2D quad (SRCBLEND ZERO / DESTBLEND ONE, `0x00450c7a`) |
| C04 SHADOW | 4 / 9 / 6 | car_shadow `0x0041ebdf`, RWObjShad `0x00487db7`, flatshad `0x0048e7f6` (RwIm3D); not drawn by the standalone |
| C05 PARTICLE | 9 / 6 / 8 | generated-sprite stand-in vs textured RwIm3D particles (exp_cloud2 `0x00489925`) |
| C06 PICKUP | 6 / 6 / 6 | pickup glow look (ROADMAP D1 stage 2): PUGlow `0x00458b3f`, SRCALPHA/INVSRCALPHA, vs additive generated sprite; triangle counts agree |
| C07 WEAPONS | 9 / 0 / 1 | mines, oil, mortar, missile, the power-up model: not rendered by the standalone |
| C08 STARTDISP | 18 / 18 / 21 | start light / scoreboard (`0x0041dad0`), depth-off, off-screen on all 9 frames |
| C09 FX | 0 / 10 / 10 | headlights (`0x00413b27`), beams (`0x0041ebeb`), lens flare (`0x0048921b`), vehicle icons |
| C10 | 3 / 0 / 0 | runtime 256x256 texture strips from `0x00422a5e` (U-9691) |
| C11 WATER | 6 / 0 / 0 | lake texture animation (water06..31) vs static Wet_Water00 |
| C12 CITYPLACE | 0 / 0 / 84 | City repeated props: 26 trees, 5 building copies, standalone one copy (U-9695) |
| C13 PROPPLACE | 3 / 3 / 0 | camera-man, Arctic parked vehicles at the world origin (U-9695) |
| C14 ANIM | 6 / 0 / 9 | copter path / rotor phase, City skyline, moved up to 0.9 |
| C15 SKY | 0 / 12 / 0 | Arctic sky pass (U-9696) |
| C16 TEX1-WORLD | 15 / 3 / 0 | two-stage world (NewShadow, wet/ice), librw stage 0 only (U-9692) |
| C17 CAR-GLOSS | 9 / 7 / 6 | car stage-1 gloss/env + CarGloss pass (U-9693) |
| C18 FOG | 3 / 3 / 3 | librw fogs in its pixel shader, so D3DRS_FOGENABLE 0 by construction |
| C19 STATE-LIBRW | 75 / 79 / 200 | librw alpha flag / alpha test / cull defaults vs the RW driver (section 4) |
| C20 STATE-SKY | 3 / 0 / 0 | legacy sky draw keeps depth test on; the original's sky cull varies per track (2 / 1) |
| C21 ORDER | 6 / 36 / 17 | blended-draw order (U-9694) |

## 4. Fix F1: RenderWare D3D9 alpha and cull state (opt-in, held OFF)

**What the original does**, every value cited to bytes:
- Per-raster alpha flag at ext `+8`, written from the raster format (`FUN_004d2980`: `ext+8 =
  DAT_005d8bf5[(fmt & 0xf) * 8]`). The TXD reader `FUN_0054f8d0` lets `FUN_004d5340` pick that
  format before RwRasterCreate `FUN_004c77c0`. Measured live, all 678 named rasters on three tracks
  are **16-bit**: 565 (flag 0) for 316, 4444 / 1555 (flag 1) for 362, and the flag equals "a used
  texel or palette entry has alpha < 255" on every texture once same-name textures in other pizzes
  are told apart (`texalpha_rule.txt`; the 32-bit-format rule fails 87-98 per track).
- Blend and alpha test switch together on stage-0 texture alpha OR vertex alpha (`FUN_004d6ce0` at
  `0x004d6d06`, `FUN_004d7100`), alpha test value `DAT_007d6bfc`.
- Device init `FUN_004d5bc0`: ALPHAFUNC 5 GREATER (`mov edx,5` `0x004d60ec`, `push edx; push 0x19`
  `0x004d6102`; `FUN_004d5480` touches only EAX/ECX), ALPHAREF 0 (`xor esi,esi` `0x004d611a`,
  `push esi; push 0x18` `0x004d6122`), CULLMODE 2 (`push 2; push 0x16` `0x004d6157`). The frame's
  world render `FUN_004492b0` sets cull 2 before the world and cull 1 after it.
- librw instead flags every raster alpha (the bridge creates C8888), and initialises ALPHAREF 10 /
  GREATEREQUAL / CULLMODE NONE (`d3ddevice.cpp:1787-1800`).

**Port** (`MASHED_LIBRW_RWSTATES=1`): `LibRw/RwRasterBridge.cpp` sets each raster's alpha flag from
its texels; `LibRw/RwRaceSubmit.cpp` sets ALPHATESTFUNC GREATER (new `rw::ALPHAGREATER`, librw
`rwrender.h` + `alphafuncMap`), ALPHATESTREF 0, CULLMODE CULLBACK per frame.

**Result against the pre-registered rules:**
- Rule 1 (fixed rows match everywhere): **fails on Arctic**.
- Rule 2 (no row that matched stops matching): **fails on Arctic**, 13-14 new rows per frame
  (`rowdelta_f1.txt`). TRAINING and City have 0 new rows, 25 and 66-68 rows gone per frame. The two
  Arctic causes are original behaviours the port lacks. (a) One sector's 8 meshes blend although 7
  of them have opaque textures and no vertex alpha of their own (U-9689). (b) The weather pass
  `FUN_00499d90` sets D3D CULLMODE 1 directly at `0x0049a084`, bypassing RW's cull cache
  `DAT_007d6b18`, so cars drawn after it run with cull NONE (U-9690). Before F1 the standalone
  blended and never culled everything, which matched those draws by accident.
- Rule 3 G-CTRL: unchanged (F1 off by default). Rule 4 no-op control: PASS 9/9. Rule 5 pixels: no
  frame rises (TRAINING -0.11 to -0.25, Arctic -0.01 to -0.04, City +0.01 to +0.03).

So F1 stays **default OFF**. Recommendation: wire it ON once U-9689 is ported (the rule is still
unlocated), and either port U-9690 or accept those Arctic cull rows as a cited original quirk.

## 5. Found on the way

1. **The original's frame is `FUN_004492b0`**: camera begin, depth test/write off, sky clump +
   `SkySecondaryDispatch`, cull 2, world, cull 1, depth back on, camera end
   (`evidence/decomp_sites.txt`).
2. **All original textures are 16-bit** (565 / 4444 / 1555). The standalone uploads 8888. That
   colour precision difference is outside any draw-list row.
3. **Per-car depth-off untextured mesh** drawn before each body (C03 a). The grey-chassis fix
   dropped the standalone's copies of the car's untextured hulls. The original draws one with depth
   test and write off, so it never covers the body. Belongs to the per-part code (U-9079).
4. Two standalone renderer facts: librw issues 12k-20k zero-primitive DrawIndexedPrimitive calls
   per race frame (one per empty material mesh), and draws every world-sector atomic every frame.
5. `decomp_pc.py` resolves the Ghidra pool relative to the script's own repo, so in a worktree it
   fails with "Could not find project". Running the main checkout's copy works.

## 6. Predictions (PREREG section 5)

- **Pr-1** (AI car textures differ): **refuted**. All four original cars draw texture `Advantage`,
  and all their body draws match.
- **Pr-2** (wheel matrices differ): **not testable** with this instrument. A wheel's AABB is
  invariant under rotation about its axle, and the wheel draws matched at 0.01.
- **Pr-3** (car bodies match only with the transplant): **confirmed**. 44/44 matched at K, and
  I-MSHIFT moves them at K+30.
- **Pr-4** (the original issues an in-race Im2D quad): **confirmed**. It is a full-screen quad with
  SRCBLEND ZERO / DESTBLEND ONE, which leaves the frame unchanged (C03 d).

## 7. Reproduce

```
py -3.12 re\frida\camera_probe.py --frame --early --shots-race 30,70,105 --drawrec --carframes --track 3 --out log\d1r1\orig_arctic\trace.csv --static-out log\d1r1\orig_arctic\static.json
py -3.12 verify\d1r1_20261016\run_replay3d.py <exe> log\d1r1\orig_arctic\stand --trace log\d1r1\orig_arctic\trace.csv --shots 1051,1091,1126 --track Arctic --frames 1450 [MASHED_LIBRW_RWSTATES=1]
py -3.12 re\tools\drawlist_diff.py log\d1r1\orig_arctic\orig_row_01051.draws.json log\d1r1\orig_arctic\stand\d1r2\stand_row_01051.draws.json --trace log\d1r1\orig_arctic\trace.csv --cite verify\d1r1_20261016\cites_d1r1.json
py -3.12 verify\d1r1_20261016\check_gates.py log\d1r1
```

`evidence/` holds the 9 original-side draw records, the 9 cited diffs, the per-texture alpha
dumps, the loaded-piz lists, three backtrace captures and the decomp extracts. The three original
traces (3.0-3.8 MB each, needed for the standalone replay and the frustum test) are not committed:
`*.csv.gz` is gitignored, so they stay at `log/d1r1/orig_{training,arctic,city}/trace.csv` (local
copies `evidence/*_trace.csv.gz`, ignored). Standalone records and BMPs are under
`log/d1r1/orig_*/stand*`.
