# Mashed vehicle/prop models — *.DFF (RW 3.6 clump stream)

Cracked 2026-06-10 (R3). Tool: `re/tools/dff_dump.py` (parse → OBJ with frame
world-transforms applied). Validated on `VEHICLES/Advantag.piz::ADVANTAGE0.DFF`
(83 frames, 71 geometries, 71 atomics, 11,971 verts / 15,358 tris, material
"Advantage" bound; every count and index ranged-checked). Layout cross-checked
against the vendored librw (`re/prior_art/renderware/librw/src/{clump,frame,
geometry}.cpp`) — DFFs are fully standard RW 3.6 clump streams (ver
0x1c02000a), unlike the proprietary chunk-0x23 TXD container.

```
CLUMP (0x10): STRUCT { numAtomics i32 [, numLights i32, numCameras i32] }
  FRAMELIST (0x0E): STRUCT { numFrames i32;
      frames[numFrames] of 0x38 bytes each:
        f32x9 rotation (column-major 3x3), f32x3 position,
        i32 parentIndex (-1 = root), i32 flags }
      + one EXTENSION (0x03) per frame
  GEOMETRYLIST (0x1A): STRUCT { numGeometries i32 }, then per geometry
    GEOMETRY (0x0F):
      STRUCT { flags u32, numTriangles i32, numVertices i32,
               numMorphTargets i32;
               [surfProps f32x3 — only when stream version < 3.4; absent here]
               if !(flags & rpGEOMETRYNATIVE 0x01000000):
                 [flags & PRELIT 0x08: RGBA u8x4 * nv]
                 texcoord sets * (f32x2 * nv)
                   (sets = (flags>>16)&0xff, else TEXTURED(0x04)->1,
                    TEXTURED2(0x80)->2)
                 triangles u32x2 * nt:
                   v0 = hi16(word0), v1 = lo16(word0),
                   v2 = hi16(word1), mat = lo16(word1)
               morph targets * { sphere f32x4, hasVerts i32, hasNormals i32,
                                 [verts f32x3 * nv], [normals f32x3 * nv] } }
      MATLIST (0x08) — same shape as the world's (texture names within
        TEXTURE(0x06) STRING subchunks; resolves into the vehicle's TXD,
        e.g. ADVANTAGE.TXD)
  ATOMIC (0x14) * numAtomics: STRUCT { frameIndex i32, geoIndex i32,
      flags i32, unused i32 } — binds geometry to frame; render transform =
      the frame's parent-chain composition.
```

Observations (Advantage car): 6 damage-state DFFs per car (ADVANTAGE0..5, all
identical size — progressive damage variants); 71 atomics = body panels,
wheels, parts (the per-part frames drive damage/wheel animation); DEBRIS0..5
DFFs are the detached-part models.

Open: skin/anim plugins unexamined (EXTENSION payloads); per-part semantics
(which frame is which wheel) — needed by R5, not by parsing.
