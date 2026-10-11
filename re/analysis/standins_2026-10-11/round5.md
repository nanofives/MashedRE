# Stand-in promotion round 5 (2026-10-11): 0x00401fe0, 0x004022a0

Port: `mashedmod/src/mashed_re/Render/PromoLoop_standins_r5.cpp` (.asi only, exe_file empty), one shared helper
(GeomBuild) for both bodies; every address is cited in the port header.

## 0x00401fe0 Geom3Vert1TriCreate, 0x004022a0 Geom4Vert2TriCreate (were FUN_00401fe0 / FUN_004022a0)

**Purpose.** Create a material (0x004e7fa0) and set its colour dword +4 to 0xffffffff, create a geometry
(0x004e8c70(nVerts, nTris, 0x4b)) and return NULL when that fails (the material is not released on that path). Set
the material's +0xc / +0x10 / +0x14 to 0x3e99999a / 0 / 0x3f333333, write the vertex positions into the geometry's
morph target (+0x5c, vertices at +0x14), and for each vertex the normal (0, 1.0f, 0), prelit bytes ff ff ff ff and uv
(1.0f, 1.0f) whenever those arrays exist (morph +0x18, geometry +0x30 / +0x34). Set each triangle's indices
(0x004e8910) and material (0x004e8940), compute the bounding sphere (0x004e8640) into morph +4..+0x10, unlock
(0x004e8a10), release the material (0x004e8030) and return the geometry.

| row | vertices | triangles | positions |
|---|---|---|---|
| 0x00401fe0 | 3 | (0,1,2) | (1,0,-1) (0,0,2) (-1,0,-1) |
| 0x004022a0 | 4 | (0,1,3) (3,1,2) | (-1,0,-4) (-1,0,4) (1,0,4) (1,0,-4) |

- Callers: 0x00402240 / 0x00402590 (both C3 since round 3). Callees: 0x004e7fa0, 0x004e8c70, 0x004e8910, 0x004e8940,
  0x004e8640, 0x004e8a10, 0x004e8030 (C2, renderware band).
- Fixture as round 3 (`exhaust: 'null'`). path1 GREEN 4/4 each, self-baseline GREEN 4/4 each: two full builds and two
  scheduled failures inside the geometry create (NULL exit). path2 PASS 1/1 each.
- Mutation control: 0x004022a0 built with the second triangle as (3,2,1) read RED 2/4 (the two full builds; the
  failure vectors exit before the triangles). Reverted, rebuilt, GREEN again. Evidence `negctl_geom_4vert_2tri_triorder.csv`.
