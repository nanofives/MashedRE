# Mashed track data formats — SPL / ANM / UVA / MTS / LAPDATA (WS-F)

Cracked + data-verified 2026-06-16 (WS-F, ROADMAP "Completion plan"). All four
binary formats are **standard RenderWare core chunk streams**; their chunk IDs
were cross-checked against
`re/prior_art/renderware/gta-reversed-modern/source/game_sa/RenderWare/rw/rwplcore.h`
(`MAKECHUNKID(rwVENDORID_CORE, n) == n`). LAPDATA.LUA is plain Lua text.

Tools:
- `re/tools/rw_track_data.py` — Python parser/validator (`dump` / `validate`).
- `mashedmod/src/mashed_re/Track/TrackData.{h,cpp}` — renderer-agnostic C++ twin
  (+ an HAnim keyframe sampler). Runtime-checked by
  `mashedmod/src/mashed_re/Track/tests/trackdata_test.cpp`.

Container: every binary format below opens with the RW chunk header
`[id u32][size u32][version u32]`. Two RW version stamps appear: `0x1803FFFF`
(older assets) and `0x1c02000a` (the RW 3.6/3.7 build the TXDs/DFFs use) — both
accepted; the parsers do not gate on version.

## Validation summary (`rw_track_data.py validate`)

**229 / 229 assets parse with exact byte consumption across all 13 track
pizzes; 0 failures.** Per-format gates:

| format | ext | count | gate |
|---|---|---|---|
| spline | `.SPL` | 20 | chunk id 0x0C + size == 12+32+8+N·12 |
| hanim | `.ANM` | 59 | chunk id 0x1B + scheme 1 + frame0 t=0 + lastT==duration + unit quats |
| uvanim | `.UVA` | 8 | chunk id 0x2B + struct + non-empty named entries |
| matrix | `.MTS` | ~129 | per-record 0x0D/0x01 headers + well-formed basis + valid type |
| lapdata | `LAPDATA.LUA` | 13 | ≥1 `Lap_Line` |

The 14 informational `twin-stale` notes are `.MTS` ↔ `.MTS.TXT` mismatches where
the human-readable export was not re-synced to the rebuilt binary (the binary is
authoritative — see F5).

---

## F1 — `.SPL` splines (rwID_SPLINE 0x0C)

Water/wave ripple paths (Arctic 10, Storm 9, rouabout 1). Layout:

```
[chunk 0x0C][size][ver]
  +0x00  32 bytes  name              copied to the spline object's +0x18 field
                                     (all-zero = empty in every shipped spline)
  +0x20  u32       numPoints
  +0x24  u32       spline type       (== 1 in every shipped spline; 1 and 2 accepted)
  +0x28  numPoints · (f32 x, y, z)   control points
```

Reader, resolved 2026-10-26 (D4-WSF-TRACKDATA-RT, `re/tools/decomp_pc.py`):
`FUN_004b3cc0` opens the file, finds chunk `0xc` (`FUN_004cc5e0(h, 0xc, ..)`) and calls
`FUN_00545260(h)`. `FUN_00545260` reads `0x28` bytes in one `FUN_004cbd30` call into a
stack block `{char[32]; int numPoints; int type}`, allocates `numPoints*0xc`, reads the
points with `FUN_004cc790`, builds the object with `FUN_00546380(numPoints, type, points)`,
then calls `RwGlobals+0xd0(obj+0x18, block, 0x20)` and sets `*obj = 0xc`.
`FUN_00546380` accepts type 1 (alloc `n*0xc+0x50`) or 2 (alloc `n*0xc+0x5c`), any other
type fails with error `0x102`/`1`, and it stores type at `obj+0xc`, `0` at byte
`obj+0x18`. The evaluator `FUN_00545340` branches on `obj+0xc` (1: `n-2` segments, 2:
`n-3`). The `RwGlobals+0xd0` slot is a 3-argument copy; whether it stops at a NUL is not
read here and does not change the result on the all-zero shipped names.

File size == `12 + 32 + 8 + numPoints·12` exactly (validated 20/20). The numbered
`WAVE1..9.SPL` are **constant-Y** (e.g. Arctic WAVE1 = 40 pts at Y=-3.618, the
frozen-bay water surface). Non-ripple splines (`WAVE.SPL`, `RAIL.SPL`,
Storm `WAVE7.SPL`) are general 3D paths — constant-Y is a property of the ripple
set, not a format invariant.

## F2 — `.ANM` animations (rwID_HANIMANIMATION 0x1B)

Helicopter / copter / cameraman flight paths (`H_ANIM_*`, `KTC1`, etc.). The
inner stream is a standard RtAnim animation with the HAnim std keyframe scheme:

```
[chunk 0x1B][size][ver]
  +0x00  u32  version       (0x100)
  +0x04  u32  keyFrameScheme(1 = HAnim std keyframe)
  +0x08  u32  numFrames
  +0x0C  u32  flags         (0)
  +0x10  f32  duration      (seconds)
  +0x14  numFrames · 36-byte keyframe:
            +0x00 f32   time
            +0x04 f32x4 q        (rotation quaternion x,y,z,w — unit)
            +0x14 f32x3 t        (translation x,y,z)
            +0x20 u32   prevFrameOffset  = (frameIndex-1)·36  [runtime linkage,
                                            unused by the sampler]
```

File size == `12 + 20 + numFrames·36` exactly (validated 59/59). Verified:
frame[0].time == 0, frame[last].time == duration, every `|q| ≈ 1.0`. Arctic
`H_ANIM_1` = 31 frames over 39.98 s, a closed helicopter loop (frame0.t ≈
frame30.t). `KTC1` = 51 frames / 29.98 s, the `KTC_Apache` copter from
`KTCSCRIPT.LUA` (path passes the script's pickup positions ~(-26, 2.4, 17)).

`HAnim::Sample(t, pos, q)` (C++) linearly interpolates translation and nlerps the
quaternion (shortest-arc) between the bracketing keyframes, looping on duration.

**Wiring status (F2): WIRED 2026-06-16.** `TrackRenderer::LoadCopters` parses the
copter wiring from the track's `COURSE.LUA` — `General_Anim_Filename( slot ,"x.anm" )`
registers a flight path into a slot, and `SetCopter( "Copter"|"JetRanger" , slot )`
puts a scenery copter on that path — plus `KTCSCRIPT.LUA`'s
`KTC_NewCopter( "KTC_Apache", slot )` (the shoot-for-weapons gameplay copter). It
loads the bound `.ANM` paths from the track piz and the model DFFs
(`COPTER.DFF` / `JETRANGER.DFF` / `KTC_APACHE.DFF`) + `COPTERS.TXD` from the shared
`Common/Perm.piz` (entries under `TOASTART/COMMON/PERM/COPTERS/`; the path is derived
from the track piz exactly as `LED.piz` is). `Render` draws each copter under the
per-frame `HAnim::Sample(t)` transform — unit quaternion → rotation + translation in
the row-vector convention the MTS/prop path already feeds to `D3DTS_WORLD` — looped on
the race clock `t`. Verified on **Arctic** (6 flying = 5 `SetCopter` slots 0-4 + 1
`KTC_NewCopter` slot 5, 3 unique models) and **Warzone** (5 flying) via the load-log
line plus an A/B render diff (`MASHED_NO_COPTERS` toggle) that isolates exactly the
copter pixels — a helicopter model airborne over the track and scattered surface
copters, with the track/sky otherwise pixel-identical (`verify/_f2_diff_highlight.png`).

The **cameraman is NOT animated** by any shipped track: `Camera_Anim_Filename` is the
commented-out template on every `COURSE.LUA` (active count 0/13 tracks), and `CamMan.dff`
is a static `Clump_Filename` prop — so it correctly renders under its world/identity
matrix, not an HAnim. [UNCERTAIN] the original's exact copter playback cadence/phase is
not RE'd from the binary; since the `.ANM` durations are in seconds and the paths are
closed loops (frame0 ≈ frameLast), playback is looped at 1× on the race clock — the
faithful reading of the data, pending an engine-side copter-update RE. Orientation uses
the data-stated `q=(x,y,z,w)` order; nose-orientation is not bit-verified vs the original.

## F3 — `.UVA` UV-animation dictionaries (rwID_UVANIMDICT 0x2B)

Scrolling sea/sky textures. A dictionary of named UV anims; each entry reuses the
RtAnim chunk (0x1B) with the UV keyframe scheme `0x1C1`:

```
[chunk 0x2B][size][ver]
  [chunk 0x01 STRUCT][4][ver]  u32 numAnims
  numAnims · [chunk 0x1B][size][ver]:
     +0x00  u32  version       (0x100)
     +0x04  u32  scheme        (0x1C1 = UV keyframe)
     +0x08  u32  numFrames     (2)
     +0x0C  u32  flags
     +0x10  f32  duration      (33.0 s for Arctic sea/sky)
     +0x14  u32  (read, discarded)  (0 in all 12 entries)
     +0x18  char name[32]      ("bmp_Sea_M", "bmp_Sky_M")
     +0x38  u32  outSlot[8]    per-node output-matrix index (see below)
     +0x58  numFrames · 32-byte keyframe:
              +0x00 f32   time
              +0x04 f32x6 uv   uv[0] angle (radians), uv[1] right.x, uv[2] up.y,
                               uv[3] right.y, uv[4] pos.x, uv[5] pos.y
              +0x1C u32   prev keyframe INDEX  (stored as base + idx*0x20)
```

Reader and consumer, resolved 2026-10-26 (D4-WSF-TRACKDATA-RT, `re/tools/decomp_pc.py`,
static words read from `MASHED.exe.unpatched`):

- `FUN_0042a740` opens the file, finds chunk `0x2b` and calls
  `FUN_00558df0(&schema_0x00623df0, h)`. The schema words are `+4 0x2b`, `+8 0x1b`,
  `+0xc 0x36000`, `+0x28 0x0052daf0` (entry stream-read).
- `FUN_0052daf0` reads `version` (must be `0x100`), `scheme`, `numFrames`, `flags`,
  `duration` with `FUN_004cc790` (the `+0x00..+0x13` words), looks `scheme` up in the
  runtime table `0x00912160` (stride `0x30`) and calls the entry's `+0x20` callback.
- Scheme `0x1c1` registers from the static info at `0x00623e54`: interp/anim keyframe
  size `0x20`/`0x20`, apply `0x00552050`, blend `0x00552190`, interpolate `0x005520f0`,
  stream-read `0x00552340`, stream-write `0x005523f0`, get-size `0x005524a0`, custom
  data size `0x44`. Scheme `0x1c0` (`0x00623e24`) shares the stream callbacks, apply
  `0x005524c0`. No shipped `.UVA` uses `0x1c0`.
- `0x00552340` (Ghidra-missed, `--create`): reads 4 bytes (`+0x14`) into a stack local
  that nothing reads, then `FUN_00551c40(h)` for the custom data, then per keyframe
  `FUN_004cc790(h, kf+4, 0x1c)` (time + `uv[6]`) and a 4-byte index `i`, stored as
  `kf+0 = frames + i*0x20`.
- `FUN_00551c40` reads `0x20` raw bytes (`+0x18` name) to custom `+0x00`, then
  `FUN_004cc790` `0x20` bytes (`+0x38`, 8 int32) to custom `+0x20`, and sets custom
  `+0x40 = 1`.
- `FUN_00552050` (apply, 0x1c1) builds the UV matrix from the interpolated frame
  (`+8..+0x1c` = `uv[0..5]`): `right = (uv[1], uv[3], 0)`, `up = (0, uv[2], 0)`,
  `pos = (uv[4], uv[5], 0)`, `at = 0`, then `FUN_004c45f0`, `RwMatrixTranslate(-0.5, -0.5, 0)`
  (`0x005e488c`), `RwMatrixRotate((0,0,1), uv[0] * 57.2957763671875)` (`0x005cd100`),
  `RwMatrixTranslate(0.5, 0.5, 0)` (`0x005e4898`), each with combine arg `2`.
  `FUN_00551d40` carries an inline copy of the same element mapping.
- `0x005520f0` (interpolate) lerps `uv[1..5]` and lerps `uv[0]` after wrapping the
  delta into `[-3.1415927, 3.1415927]` by `6.2831855` (`0x005e48a4`, `0x005ce2f4`,
  `0x005cd2c0`).
- `FUN_00551d40` (per-material update, reached from `FUN_00474f50` <- `FUN_00475010` <-
  `FUN_00426780`) reads `outSlot[node] = *(u32*)(custom + 0x20 + node*4)` for
  `node < interp+0x2c`. A value below 2 writes that node's matrix into the material's
  UV matrix slot `0` or `1` (`FUN_004c52f0`). Other values skip the node.

Shipped values (12 entries in 8 files): every `uv[0]` is -0.0, `uv[1]`/`uv[2]` are 1.0,
`uv[3]` is 0.0, so only `pos.x`/`pos.y` animate, the Arctic rates above stand. `+0x38`
is **not** all zero: `outSlot[0]` is 0 everywhere, Storm `MUL_watertest_M` has
`outSlot = (0, 1, 0, ..)` with two t=0 keyframes, and 5 single-node entries (City
`BMP_Arrows_M`, `Fountain_M`, Forest `Waterfroth_M`, sands `Animated_Water_M`, training
`Animated_Water_M`) carry non-zero words in `outSlot[1..7]`. [UNCERTAIN] whether those
words are read depends on the node count in `interp+0x2c`, whose writer was not read in
this step (missing: the interpolator create call for a material UV anim, and what node
count it derives from the animation).

Arctic `SEA.UVA`: 2 entries. `bmp_Sea_M` translates the sea UV ty 0→6 over 33 s →
**dv/dt = 0.1818 u/s**; `bmp_Sky_M` ty 0→-1 → **dv/dt = -0.0303 u/s** (du/dt = 0
for both — pure vertical scroll). These rates are byte-exact. The element order
is resolved above (`FUN_00552050`): `uv[5]` is `pos.y`.

**Wiring status (F3): WIRED — extension-based 2026-06-16.** The name-match heuristic
is replaced by the original's real binding. `TrackWorld`/`DffModel` now parse the RW
**UVAnim material extension** (`rwID_UVANIMPLUGIN` 0x135 = `MAKECHUNKID(CRITERIONTK,
0x35)`) inside each material's `EXTENSION` (0x03) chunk — librw `readUVAnim` layout: a
`STRUCT` (0x01) holding `u32 mask`, then a 32-byte name per set bit; the lowest set
bit's name (e.g. "bmp_Sea_M") binds the material to that `.UVA` dict entry. The parsed
name is stored in `Material::uv_anim` / `DffMaterial::uv_anim`. `TrackRenderer` maps
each `.UVA` entry name → its keyframe scroll rate and applies it (a `D3DTS_TEXTURE0`
translation, `D3DTTFF_COUNT2`) to the bound material — for **world** geometry, **DFF
props** (Arctic's sea = `SEA.DFF`) AND the **sky** clump (`SKY.DFF`), the cases the
old world-only name-match missed.

Data-verified (byte scan of the Arctic assets): `GRAPH.BSP` carries **no** UVAnim
extension; `SEA.DFF` has one (`mask=0x1`, name `bmp_Sea_M`, off 0x2240) and `SKY.DFF`
one (`bmp_Sky_M`) — i.e. on Arctic the UV-scroll lives entirely on the prop/sky clumps,
so the old heuristic (world materials only) bound nothing visible. Standalone verify:
load log `F3 UV-anim: 2 .UVA rate(s); scrolling materials world=0 props=1 sky=1`, and an
A/B render diff (`MASHED_NO_UVSCROLL` toggle) localises the moving texels to the
frozen-bay sea surface (`verify/_f3_scroll_highlight.png`).

## F5 — `.MTS` matrices (count + N · rwID_MATRIX 0x0D)

**Audit finding: `.MTS` is NOT a "material script"** (the WS-F label is a
misnomer). It is an `ExportMatrices.mll` dump of **instance-placement matrices** —
the per-instance transforms for scattered props (crates, lights, scaled trees and
foliage). Layout:

```
u32 count
count · record (76 bytes each):
  [chunk 0x0D MATRIX ][size 0x40][ver]
    [chunk 0x01 STRUCT][size 0x34][ver]
      f32x12  right(3), up(3), at(3), pos(3)   RW row order (4x3)
      u32     type   (3 = rwMATRIXTYPEORTHONORMAL; 0 = arbitrary/scaled)
```

File size == `4 + count·76` exactly (validated ~129/129). Cross-checked against
the `.MTS.TXT` twin where synced: Arctic `CRATE.MTS` count 5 == 5 crates, every
`pos` matches the txt `Translation:` exactly, and the matrix basis reconstructs
the txt `Rotation:` (Y-Euler 43.7175° → cos/-sin). **type 3** props (crates,
lights) are orthonormal; **type 0** props (trees/foliage) carry a uniform scale in
the basis (Forest `TREES_1` scale 5.644 == the txt `Scale: [5.64389,...]`).

The renderer already consumes `.MTS` inline (`TrackRenderer.cpp`, RWP_Object prop
placement, 76-byte stride) — `MtxList` formalizes/documents it.

**`.MTS.TXT` is a stale twin** on several tracks (it orders matrices differently
than the binary, an occasional prop is nudged, and on City/Warzone/Neustein it has
a wholly different count — the binary was rebuilt without re-exporting the txt).
The binary is authoritative; the validator gates on the txt-independent
well-formedness check and reports txt divergence as informational only.

## F4 — `LAPDATA.LUA` (text)

Lap lines, split sectors, safe-start ranges. Present in **all 13** tracks. Calls
(comments `-- …` stripped; two terminator dialects accepted —
`Lap_Line(-1)` and `Lap_Line_End()`):

```
Lap_Variations(n)            number of route variations (1 for the shipped tracks)
Lap_Line(gate)               an AI gate index that is a lap line (start/finish)
Lap_Line(-1) | Lap_Line_End()   terminator
Safe_Start_Lines(a, b)       gate range that is "safe" at the start (grace zone)
Split_Sector(splitId, gate)  split-time checkpoint at a gate
```

The numbers index the AI gate ribbon (`AI*.BSP`, material RED byte = gate index;
gate 0 = start line). Arctic: `Lap_Line(0)`, `Lap_Line(93)`,
`Split_Sector(0,26)`, `Split_Sector(1,55)`, 7 `Safe_Start_Lines`.

**Wiring status (F4): SEQUENCE + SPLITS WIRED 2026-06-16 (algorithm-faithful).**
After RE'ing the original lap FUN (`FUN_00408610` crossing rule + lap-line list
`FUN_00426cf0` → `DAT_0066d6e4[6]` + Time-Trial intermediate recorder
`FUN_00411600`; full subsystem map in
`re/analysis/lap_progress_subsystem_2026-06-16.md`), `UpdateRace` now uses the
multi-`Lap_Line` **crossing sequence** rather than a single finish gate.
`Track::LapLineStep` (`Track/LapLogic.h`, shared verbatim with the test) flags
each declared `Lap_Line` gate as the car advances forward onto it and completes a
lap only when the primary line is reached with EVERY declared line crossed since
the last lap — the "why two lap lines" anti-shortcut (Arctic must cross gate 93
then gate 0). `Split_Sector` gates record the race clock per lap for the player
car, mirroring `FUN_00411600`'s intermediate timing (`race_time_`/`split_time_`).

**NOT bit-identical / not C4:** the original derives its gate index from a spline
racing-line projection (`FUN_00407e20` family) the standalone doesn't port — it
advances gates by proximity — so the diff lane can't bit-match `FUN_00408610`.
Verified: `Track/tests/lap_test.cpp` drives the real `LapLineStep` (one loop = 1
lap; anti-shortcut reach-primary-without-other = 0 laps; 3-line sequence; single-
line; no-LAPDATA → gate-0 fallback — ALL PASS) plus an integration smoke (the
race runs the per-car step without crash). The remaining verbatim/C4 path is the
racing-line-projection subsystem port.

---

## Provenance / no-guessing notes

- Chunk IDs: authoritative (rwplcore.h). Field offsets: derived from the bytes and
  confirmed by exact whole-file size arithmetic on every asset.
- `.MTS` semantics additionally confirmed against the `.MTS.TXT` exports (pos +
  rotation + scale) where the twin is synced.
- 2026-10-26 (D4-WSF-TRACKDATA-RT): SPL `+0x00` name / `+0x24` type, UVA `+0x14`,
  UVA `+0x38` meaning and the UVA `uv[6]` element order are resolved from the
  original's readers (cited in F1 / F3). Open, each with its missing evidence:
  (1) UVA `outSlot[1..7]` reads depend on `interp+0x2c` (writer not read);
  (2) F2 copter playback cadence/phase and nose orientation: the copter per-frame
  update and the delta it passes into `RtAnimInterpolatorAddAnimTime` `0x0052e0e0`
  are not found. `0x0052e0e0`'s callers are `FUN_004b47c0` (hier `+0x20`, then
  `FUN_00533ec0`), `FUN_0052df70`, `FUN_0052e2c0`, `FUN_00551fe0` (UV). The two
  `FUN_004b47c0` call sites `0x00451ee7` / `0x00452048` are in Ghidra-missed code in
  the `0x00451xxx` block that holds `WarzonePontoonAnimLoopCb` `0x00451690`, and both
  write `_DAT_007f0fb4` from `[ebx+200]`. Neither is shown to be the copter path.
  Missing: the function the `SetCopter` / `KTC_NewCopter` Lua bindings register for
  per-frame update, and its delta argument. No extracted value in this note depends on
  either open item.
