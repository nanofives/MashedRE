// FLAGS: /O2 /GS /Ob1
/* a1 0x00401fe0 (600 B). Best: 46.4% (start g2s 21.2%). RpMaterial + RpGeometry(3 verts, 1 tris, 0x4b) builder,
   RW 3.x layouts: material +4 colour, +0xc surface props; geometry +0x2c triangles, +0x30 prelit, +0x34 uv,
   +0x5c morph target (+4 sphere, +0x14 verts, +0x18 normals). Fixed: typed RW structs, struct copies for
   colour / surface props / sphere, one vertex loop over a local pos[] (9176 fully unrolls it), block-free
   function-scope locals give the original 0x4c/0x58 frame exactly (prologue, frame layout, tail identical).
   Residual (b): register allocation of the unrolled vertex body. Original keeps verts in a scratch
   register and uses ebx as the pos copy temp, with the long-lived value (mt in 0x00401fe0, tris in
   0x004022a0) in ebp spilled/reloaded around the normal copy; 9176 here puts verts in ebx and spills it.
   Tried: load order of the geometry fields, mt vs re-reading geo->morphTarget, *verts++ vs verts[i] vs
   per-component copy, normal init inside/outside the loop, declaration orders; permuter 860 compiles
   (only semantics-breaking variants scored higher, discarded). */
typedef struct { float x, y, z; } V3;
typedef struct { unsigned char r, g, b, a; } RGBA;
typedef struct { float ambient, specular, diffuse; } SurfProps;
typedef struct { float u, v; } TexCoords;
typedef struct { V3 center; float radius; } Sphere;
typedef struct { unsigned short vi[3]; unsigned short matIndex; } Triangle;
typedef struct RpMaterial { int texture; RGBA color; int pipeline; SurfProps surfaceProps; } RpMaterial;
typedef struct MorphTarget { void *parentGeom; Sphere boundingSphere; V3 *verts; V3 *normals; } MorphTarget;
typedef struct RpGeometry {
  int pad[11];
  Triangle *triangles;   /* +0x2c */
  RGBA *preLitLum;       /* +0x30 */
  TexCoords *texCoords;  /* +0x34 */
  int pad2[9];
  MorphTarget *morphTarget; /* +0x5c */
} RpGeometry;
RpMaterial *FUN_004e7fa0(void);
int FUN_004e8030(RpMaterial *m);
MorphTarget *FUN_004e8640(MorphTarget *mt, Sphere *s);
RpGeometry *FUN_004e8910(RpGeometry *g, Triangle *t, unsigned short a, unsigned short b, unsigned short c);
RpGeometry *FUN_004e8940(RpGeometry *g, Triangle *t, RpMaterial *m);
RpGeometry *FUN_004e8a10(RpGeometry *g);
RpGeometry *FUN_004e8c70(int nv, int nt, unsigned int flags);
// MATCH: 0x00401fe0 FUN_00401fe0
RpGeometry *FUN_00401fe0(void)
{
  RGBA col;
  SurfProps sp;
  V3 n;
  Sphere s;
  V3 pos[3];
  RpMaterial *mat;
  RpGeometry *geo;
  Triangle *tri;
  RGBA *pre;
  TexCoords *tex;
  MorphTarget *mt;
  V3 *verts;
  V3 *nrm;
  int i;
  col.r = 255; col.g = 255; col.b = 255; col.a = 255;
  pos[0].x = 1.0f; pos[0].y = 0.0f; pos[0].z = -1.0f;
  pos[1].x = 0.0f; pos[1].y = 0.0f; pos[1].z = 2.0f;
  pos[2].x = -1.0f; pos[2].y = 0.0f; pos[2].z = -1.0f;
  mat = FUN_004e7fa0();
  mat->color = col;
  geo = FUN_004e8c70(3, 1, 0x4b);
  if (geo == 0) return 0;
  {
    sp.ambient = 0.3f; sp.specular = 0.0f; sp.diffuse = 0.7f;
    mat->surfaceProps = sp;
  }
  tri = geo->triangles;
  pre = geo->preLitLum;
  tex = geo->texCoords;
  mt = geo->morphTarget;
  verts = mt->verts;
  nrm = mt->normals;
  for (i = 0; i < 3; i++) {
    verts[i] = pos[i];
    n.x = 0.0f; n.y = 1.0f; n.z = 0.0f;
    if (nrm) { *nrm = n; nrm++; }
    if (pre) { pre->r = 255; pre->g = 255; pre->b = 255; pre->a = 255; pre++; }
    if (tex) { tex->u = 1.0f; tex->v = 1.0f; tex++; }
  }
  FUN_004e8910(geo, tri, 0, 1, 2);
  FUN_004e8940(geo, tri, mat);
  {
    FUN_004e8640(mt, &s);
    mt->boundingSphere = s;
  }
  FUN_004e8a10(geo);
  FUN_004e8030(mat);
  return geo;
}
