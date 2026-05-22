---
bucket: bucket_004c4270
session_date: 2026-05-19
session_id: batch-x-s3
opened_in_slot: Mashed_pool0
candidate_count: 80
address_span: 0x004c4270 .. 0x004cff20
size_bytes: 48304
avg_bytes_per_fn: 603
---

# Cluster summary

First-pass discovery bucket anchored at 0x004c4270, immediately after the
Lua-5.0 source band (which ends ~0x004c4000) and the C3-promoted leaf math
neighbors `Vec3Magnitude` (0x004c3ac0), `FastSqrt` (0x004c3b30), and
`FastInvSqrt` (0x004c3b90).

## Subsystem observed (NOT the working hypothesis)

The working hypothesis was "RW math core". Decomp showed the bucket is
actually a **mixed RenderWare core + D3D9 thin-wrapper renderer band**.
Five tight sub-clusters:

| Range | Sub-cluster |
|-------|-------------|
| 0x004c4270 .. 0x004c5680 | RwV3d field accessors (X/Y/Z), RwMatrix bbox / OrthoNormalize, axis-angle decompose. Calls into FastInvSqrt 0x004c3b90 and Vec3Magnitude 0x004c3ac0. |
| 0x004c5770 .. 0x004c5dd0 | RW resource registry dispatch (vtable at DAT_007d3ff8, DAT_007d4054). RwResourcesAllocateResEntry / FindResource shape. |
| 0x004c6140 .. 0x004c79f0 | RwTexture / RwImage subsystem: TextureRead, Raster lookup, mipmap chain builder, depth setters, sampler-state thunks. |
| 0x004c9fb0 .. 0x004cb840 | D3D9 device thin wrapper (IDirect3DDevice9* at DAT_007d4110): SetTexture(0x94/0x9c), SetTextureStageState(0x190), SetClipPlane(0x15c), SetGammaRamp(0x170), SetSamplerState(0x1ac), SetMaterial(0x1a0), SetLight(0xcc, 0x6c-byte stride). |
| 0x004cbad0 .. 0x004cbbd0 | Frustum cull plane test (6 planes, 5-float stride). |
| 0x004cc580 .. 0x004cc9e0 | D3D9 immediate-mode vertex buffer / chunk writer (Lock/Unlock pattern). |
| 0x004cd170 .. 0x004cdca0 | Render pipeline state machine (primitive type 1..5 dispatch). |
| 0x004cdca0 .. 0x004cff20 | RwImage core: Create/Destroy, AllocatePixels, FormatFindRaster, CopyImage, GammaCorrect, MakeMask, ReadImage (TGA/BMP-style loader with depth=4/8/0x20 palette/8bpp/32bpp dispatch). |

## C3-promotable leaf candidates flagged

Three small leaf accessors at the top of the bucket are direct
sister-functions to the existing C3 hooks and may be C3-promotable later
once their full byte sequences are read (Frida force-call A/B vs the live
process):

- `0x004c4270` — `RwV3dGetY`-shape: `MOV EAX, [ESP+4]; FLD [EAX+4]; RET`
  (4 bytes prologue confirmed via listing_code_unit_at).
- `0x004c42d0` — `RwV3dGetX`-shape companion.
- `0x004c4360` — `RwV3dGetZ`-shape companion.
- `0x004c4670` (thunk to 0x004c4680) — **RwMatrixOrthoNormalize** (calls
  `FastInvSqrt` thrice, writes RwMatrix flags |= 3). Strong C3 candidate.
- `0x004c5680` — major-axis selector for `OrthoNormalize` fallback path.

These are **NOT** promoted by this session. Plates note them as
"C3-candidate" but the session stays at C0->C1.

## Library-residue assessment

NOT library residue. Bucket has heavy game-string anchors via the
`DAT_007d3ff8` + `DAT_007d4054` + `DAT_007d4110` + `DAT_007d4628`
dispatch tables (these are Mashed's RW engine instance pointers,
already attested in adjacent hooks.csv rows for the render subsystem at
0x004c7a70 and 0x004c77c0). Average size 603 B/fn confirms game-code
density (CRT band is <150 B/fn).

## Cross-bucket calls observed

- Many callers route through `FUN_004d7ff0` (RwError build) +
  `FUN_004d8480` (RwError raise) — typical RW error reporting.
- Allocator path: `DAT_007d3ff8+0x108` (Malloc), `+0x110` (Realloc),
  `+0x10c` (Free), `+0x118` (PluginAlloc), `+0x11c` (PluginFree).
- RW pipeline calls `DAT_007d3ff8+0x68` (Stream open/find), `+0x88`
  (Raster create), `+0xa4` (Raster lock).

## Notes for ghidra-sweep

- All 80 plates use `confidence=C1`.
- 0 STUBs filed (no missing callees that block plate authoring).
- 8 [UNCERTAIN] markers filed — see individual plates. Most are about
  exact RW API mapping (Ghidra catalog has the API name but signature
  match needs the sweep's master-Ghidra struct inference).
- No struct extraction done in this session (RwMatrix layout is already
  attested via other plates; RwImage layout (4 ints + 4 ptrs + flags)
  has enough evidence in plates 0x004cdca0/0x004cdf20 to extract later).
