# Mashed track world geometry — GRAPH*.BSP (RW 3.6 world stream)

Cracked 2026-06-10 (R3 opener). Tool: `re/tools/track_dump.py` (parse → OBJ +
top-down wireframe PNG). Validated on **13/13** TRACKS/*.piz (entry name varies:
`GRAPH.BSP` or `GRAPHICS.BSP`; the tool matches `GRAPH*.BSP`). Companions in the
same piz: `COLLISIONS.BSP`/`COLLIDE.BSP` (collision world, same container TBD),
`AI.BSP`/`AI1.BSP` (AI graph), `*.DFF` props, `TEXTURES.TXD`, `COURSE.LUA`/
`LAPDATA.LUA`, `*.MTS`/`*.SPL`/`*.ANM`/`*.UVA`.

Container: standard RenderWare binary chunk stream, version `0x1c02000a`
(the same RW 3.6 build the game's TXDs use). Chunk header = `[id u32]
[size u32][version u32]`.

```
WORLD (0x0B)
├─ STRUCT (0x01)  0x40 bytes — world header (offsets within the struct):
│    +0x00 u32   rootIsWorldSector
│    +0x04 f32x3 invWorldOrigin (x,y,z)
│    +0x10 u32   numTriangles          (validated: == Σ sector tris, 13/13)
│    +0x14 u32   numVertices           (validated: == Σ sector verts, 13/13)
│    +0x18 u32   numPlaneSectors       (validated: == tree inner nodes)
│    +0x1c u32   numWorldSectors       (validated: == tree leaves)
│    +0x20 u32   colSectorSize
│    +0x24 u32   format flags          (observed: 0x4001004d, 0x4001000d,
│                                       0x40020089, 0x400200c9)
│    +0x28 f32x6 bounding box (sup.xyz, inf.xyz)
├─ MATLIST (0x08)
│    STRUCT: i32 count, then count MATERIAL (0x07) children
│    (observed 13..63 materials per track)
├─ PLANESECTOR (0x0A) — binary tree, recursive:
│    STRUCT (0x01) 0x18 bytes:
│      +0x00 i32 type (axis), +0x04 f32 value,
│      +0x08 b32 leftIsAtomic, +0x0c b32 rightIsAtomic,
│      +0x10 f32 leftValue, +0x14 f32 rightValue
│    <left child>  PLANESECTOR (0x0A) or ATOMICSECTOR (0x09)
│    <right child> PLANESECTOR (0x0A) or ATOMICSECTOR (0x09)
└─ EXTENSION (0x03)

ATOMICSECTOR (0x09) leaf:
  STRUCT (0x01):
    +0x00 u32   matListWindowBase
    +0x04 u32   numTriangles (nt)
    +0x08 u32   numVertices  (nv)
    +0x0c f32x6 sector bounding box
    +0x24 u32   unused, +0x28 u32 unused          (fixed part = 0x2c bytes)
    then arrays (presence SOLVED per sector from payload size, then validated):
      f32x3 * nv   vertices                        (always)
      [f32x3 * nv  normals]                        (not seen in the 13 tracks)
      [u32   * nv  prelight RGBA]                  (present when fmt & 8-ish —
                                                    observed present everywhere)
      [f32x2 * nv  texcoords] * numUVsets          (1 set observed everywhere)
      u16x4  * nt  triangles
  EXTENSION (0x03)
```

## Triangle field order — determined empirically per sector

Both RW orders exist in the wild; the tool tests both against hard constraints
(every vertex index < nv AND every material id < numMaterials) and records the
fit. The Mashed tracks fit hypothesis **(v0,v1,v2,mat)** — e.g. Arctic's first
failing value under the (mat,v0,v1,v2) reading was a "mat id" of 15 with only
13 materials, while (v0,v1,v2,mat) fits all 16,480 triangles. 13/13 tracks fit
the same order.

## Validation results (13/13 GREEN)

| track | tris | verts | sectors (plane+world) | format | mats |
|---|--:|--:|--|--|--:|
| Arctic | 16,480 | 16,229 | 11+12 | 0x4001004d | 13 |
| City | 18,426 | 21,984 | 6+7 | 0x4001004d | 63 |
| dump | 26,152 | 28,083 | 24+25 | 0x4001004d | 40 |
| Egypt | 10,126 | 11,307 | 9+10 | 0x4001004d | 18 |
| Forest | 8,853 | 9,616 | 27+28 | 0x4001000d | 28 |
| Highway | 17,660 | 21,914 | 15+16 | 0x4001004d | 24 |
| Neustein | 12,596 | 13,286 | 9+10 | 0x4001004d | 29 |
| rouabout | 12,826 | 13,673 | 11+12 | 0x40020089 | 36 |
| sands | 13,152 | 13,328 | 44+45 | 0x40020089 | 25 |
| Storm | 10,346 | 11,430 | 27+28 | 0x4001004d | 31 |
| SuperG | 11,394 | 13,003 | 31+32 | 0x400200c9 | 24 |
| training | 11,469 | 12,767 | 41+42 | 0x400200c9 | 24 |
| Warzone | 23,496 | 26,965 | 15+16 | 0x400200c9 | 56 |

Proof artifact: `verify/r3/arctic_wireframe.png` — the Arctic arena top-down
wireframe (island terrain + the signature bridge span clearly visible) +
`verify/r3/arctic_world.obj` (loads in any OBJ viewer).

## Collision worlds — SAME format (cracked 2026-06-10)

`COLLISIONS.BSP` / `COLLIDE.BSP` / `COLL.BSP` (naming varies per track) is the
SAME RW world stream, validated **13/13** with the same parser (e.g. Arctic:
3,047 tris / 2,675 verts / 6 sectors, format 0x40000079; others 0x40000049).
Its MATLIST materials are untextured color-only entries = collision surface
types (3..25 per track).

## Material → texture binding (cracked 2026-06-10)

Each MATERIAL(0x07) carries STRUCT (RGBA at +0x04) + TEXTURE(0x06) whose
second subchunk is the STRING texture name. Names resolve into the track's
TXD — entry name varies: `TEXTURES.TXD` (Arctic…) or `<TRACK>.TXD` (CITY.TXD,
DUMP.TXD…); prefer TEXTURES.TXD else the largest .TXD. Track TXDs are the
SAME chunk-0x23 container as the menu TXDs but with root version 0x1c02000a
(TxdDecoder now accepts both encodings). Binding sweep 13/13: 21/24..24/24
bound per track; small MISSING tail (e.g. 'JetRanger', 'Van') = names living
in secondary TXDs / prop TXDs — multi-TXD lookup is an R4 detail.

## R4 opener (2026-06-10): rendered in mashed_re.exe

`Track/TrackWorld.{h,cpp}` (C++ twin of track_dump.py, same validations) +
`D3d9Render/TrackRenderer.{h,cpp}` (per-material batches, prelight as vertex
diffuse, PAL4/PAL8/32bpp texture expansion, hand-built LookAt/Perspective
matrices, auto-orbit fly-through) render Arctic textured in the standalone:
`verify/r4/arctic_fly_*.png` (island, dirt circuit, suspension bridge with
cables — baked dusk prelight). Env `MASHED_TRACK_VIEW=<piz|1>`.

## Open items (R3/R4 continuation)

- AI*.BSP graph format; `.SPL` splines; `.MTS` material scripts; `.ANM` anims.
- Multi-TXD texture lookup (props/vehicles referenced from world materials).
- format-flag bit semantics (0x4001004d vs 0x40020089 etc.) — currently solved
  per-sector empirically; bit meanings unconfirmed. [UNCERTAIN]
- Round-trip writer: not yet needed (no world rewriting use-case identified).

## AI.BSP — the AI path / lap gates (cracked 2026-06-10, R6 prep)

`AI.BSP` / `AI1.BSP` is the SAME world stream again: one sector of N vertical
4-vert "gate" quads spanning the track (Arctic 94, City 117). **The material's
RED byte is the gate index** — the numbers `LAPDATA.LUA`'s `Lap_Line()` /
`Safe_Start_Lines()` refer to (Arctic: rgba=(0,0,0,255), (1,0,0,255), ...).
Gate 0 = the start/finish line; race direction = toward gate 1. Runtime-
verified: spawning at gate-0's center places the car exactly on the rendered
start straight under the gantry on both tracks.

## .MTS — prop matrix sets (cracked 2026-06-10 via the text twin)

`*.MTS` ("ExportMatrices.mll" output; `CRATE02.MTS.TXT` in Arctic.piz is a
plain-text twin): `u32 count`, then per instance an RW chunk id 0x0d
(ver 0x1803FFFF) wrapping a STRUCT of `f32x9` column-major rotation +
`f32x3` translation (+ trailing u32). COURSE.LUA wires them:
`RWP_Object(i, "name", "X.dff", "Y.mts")` = instanced physics props (tyre
walls, crates), `Clump_Filename(i, "x.dff")` = identity-placed world clumps
(sea, freighter...), `Clump_Exclude_From_World(i)` = overlay exclusions.
Implemented in TrackRenderer (Arctic: 7 props/23 instances; City: 10/52).
