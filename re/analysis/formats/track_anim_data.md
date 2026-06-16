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
  +0x00  32 bytes  sub-header        [UNCERTAIN semantics] — all-zero in every
                                     shipped spline (bbox/reserved); the point
                                     data below is byte-exact regardless.
  +0x20  u32       numPoints
  +0x24  u32       flag              (== 1 in every shipped spline)
  +0x28  numPoints · (f32 x, y, z)   control points
```

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

**Wiring status (F2):** the sampler is data-verified. Rendering the animated
copter/cameraman prop (bind the animated frame transform to its DFF — `CAMMAN.DFF`
and the heli model — under `D3DTS_WORLD` each frame) is the remaining hookup; the
renderer's prop path (`props_`) draws under a static matrix today.

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
     +0x14  u32  ?             [UNCERTAIN] (0)
     +0x18  char name[32]      ("bmp_Sea_M", "bmp_Sky_M")
     +0x38  u32  pad/nodeToUV[8] [UNCERTAIN] (all 0)
     +0x58  numFrames · 32-byte keyframe:
              +0x00 f32   time
              +0x04 f32x6 uv   2x3 affine UV transform [UNCERTAIN exact element
                                order — but uv[4],uv[5] are the animated tx,ty]
              +0x1C u32   prevFrameOffset  [runtime linkage]
```

Arctic `SEA.UVA`: 2 entries. `bmp_Sea_M` translates the sea UV ty 0→6 over 33 s →
**dv/dt = 0.1818 u/s**; `bmp_Sky_M` ty 0→-1 → **dv/dt = -0.0303 u/s** (du/dt = 0
for both — pure vertical scroll). These rates are byte-exact; the only
[UNCERTAIN] is the affine element ordering, which does not affect the extracted
scroll vector.

**Wiring status (F3):** `TrackRenderer::Load` parses the `.UVA` and binds each
entry's scroll rate to world materials whose `tex_name` carries the entry's stem
("bmp_Sea_M" → "Sea"); `Render` scrolls them via a `D3DTS_TEXTURE0` translation
with `D3DTTFF_COUNT2`. **Binding heuristic** — the original binds a UV anim to a
material through a per-material UV-anim extension chunk; we don't parse that
extension yet, so we name-match. Materials drawn as **DFF props** (Arctic's sea is
`SEA.DFF`, the sky is a clump) aren't covered yet (props don't retain material
names); covering them + the extension-based binding is the faithful follow-on.

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

**Wiring status (F4):** `TrackRenderer::Load` parses `LAPDATA.LUA`;
`UpdateRace` now counts a lap when a car crosses `lap_data_.finish_gate()` (the
primary `Lap_Line`, data-driven) instead of the hardcoded gate 0. The parsed
split sectors and safe-start ranges are carried for split-timing / start-grace.
**Deferred (needs Ghidra):** the multi-`Lap_Line` crossing-*sequence* semantics
(why a track lists two lap lines) and the exact line-plane crossing test are the
original's lap FUN, out of this no-Ghidra workstream's scope; the data-driven
finish gate is a strict improvement over the hardcoded constant.

---

## Provenance / no-guessing notes

- Chunk IDs: authoritative (rwplcore.h). Field offsets: derived from the bytes and
  confirmed by exact whole-file size arithmetic on every asset.
- `.MTS` semantics additionally confirmed against the `.MTS.TXT` exports (pos +
  rotation + scale) where the twin is synced.
- `[UNCERTAIN]` markers above are limited to fields that are **constant (zero) in
  all shipped assets** (SPL sub-header, UVA `+0x14`/`+0x38`) or to an internal
  element *ordering* that does not change any extracted quantity (UVA affine). No
  extracted value depends on an unresolved field.
