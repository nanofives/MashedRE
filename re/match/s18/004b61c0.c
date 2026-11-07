// FLAGS: /O2 /GS /Ob1
/* MATCH-S18 0x004b61c0 (h5): best 95.9% (regs 95.9%, ops 99.5%), 219/218 ins. Raw g2s 61.5%.
   Wireframe draw of a triangle mesh: 6 line indices per triangle into DAT_007bbe48 (clamped to 0x2000 tris),
   positions + one RGBA colour into the RwIm3DVertex buffer DAT_00773e48 (clamped to 0x2000 verts), RwIm3DTransform
   (0x004cd070), line-list RwIm3DRenderIndexedPrimitive (0x004cd170), RwIm3DEnd (0x004cd140), render states through
   RwGlobals+0x20. Moved it: ushort counts into int locals in the order numVerts, numTris, verts, tris (prologue
   identical); `RpTriangle *t = &tri[i]` per iteration (gives the original's base+4 induction pointer and the
   v0, v2, v1 load order); index stores as buf[n++].
   The natural vertex loop `for (i = 0; i < numVerts; i++)` scores 70.2%: 9176 unrolls it x8, the original is x4
   (same finding as S01 0x00401690 and S16 0x00499d90). This file unrolls x4 by hand (counter j = i + 3, then a
   remainder loop), which reproduces the original body, scheduling and remainder loop exactly.
   Residual: one 2-byte `mov edi, edi` alignment pad 9176 puts before the hand-written x4 loop top (offset 0xfe);
   the original's loop came from the compiler's own unroller and is not aligned. Tried goto-form loop (same pad),
   permuter 234 variants (no gain). Class (a): compiler-version (unroll factor / loop-top alignment of the
   unroller's loop). Not relocheck'd as 100%. */
/* ---- MATCH-S11 preamble (same text in every re/match/s11 source; not part of any function body) ----
   C under the DDK (match.py), C++ under VS2022 when the port includes this file (MATCH_PORT). */
#ifndef MATCH_PREAMBLE
#define MATCH_PREAMBLE
typedef unsigned char undefined; typedef unsigned char undefined1; typedef unsigned short undefined2;
typedef unsigned int undefined3; typedef unsigned int undefined4; typedef unsigned __int64 undefined8;
typedef unsigned int uint; typedef unsigned short ushort; typedef unsigned char uchar; typedef unsigned char byte;
typedef unsigned long ulong; typedef __int64 longlong; typedef unsigned __int64 ulonglong; typedef signed char sbyte;
typedef unsigned short word; typedef unsigned int dword; typedef long double float10;
#ifndef __cplusplus
typedef unsigned char bool;
#endif
#ifdef __cplusplus
/* port: a global is a reference to its original address (the exe maps the original data range) */
#define MGLOBAL(T, n, a) static T &n = *(T *)(a)
#define MGLOBALA(T, n, N, a) static T (&n)[N] = *(T (*)[N])(a)
#else
#define MGLOBAL(T, n, a) extern T n
#define MGLOBALA(T, n, N, a) extern T n[N]
#endif
#endif
/* ---- end preamble ---- */
typedef struct RwRGBA { unsigned char red, green, blue, alpha; } RwRGBA;
typedef struct RwV3d { float x, y, z; } RwV3d;
typedef struct RwIm3DVertex { RwV3d objVertex; RwV3d objNormal; unsigned int color; float u, v; } RwIm3DVertex;
typedef struct RpTriangle { unsigned short vertIndex[3]; short matIndex; } RpTriangle;
typedef struct WireGeom {
  int unk0;
  RpTriangle *triangles;   /* +0x04 */
  RwV3d *verts;            /* +0x08 */
  char pad[0x82 - 0xc];
  unsigned short numVerts; /* +0x82 */
  unsigned short numTris;  /* +0x84 */
} WireGeom;
typedef int (*RwRenderStateSetFn)(int state, void *value);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBALA(RwIm3DVertex, DAT_00773e48, 0x2000, 0x00773e48);
MGLOBALA(unsigned short, DAT_007bbe48, 0x2000 * 6, 0x007bbe48);
void *FUN_004cd070(void *pVerts, int numVerts, void *ltm, unsigned int flags);
int FUN_004cd140(void);
int FUN_004cd170(int primType, unsigned short *indices, int numIndices);
#define RWRGBALONG(r, g, b, a) ((unsigned int)(((a) << 24) | ((r) << 16) | ((g) << 8) | (b)))
#define RenderStateSet(s, v) ((RwRenderStateSetFn)*(int *)(DAT_007d3ff8 + 0x20))((s), (void *)(v))
// MATCH: 0x004b61c0 FUN_004b61c0
void FUN_004b61c0(WireGeom *geom, RwRGBA *color)
{
  RwRGBA white;
  int numVerts = geom->numVerts;
  int numTris = geom->numTris;
  RwV3d *verts = geom->verts;
  RpTriangle *tri = geom->triangles;
  int numIndices = 0;
  unsigned int flags;
  int i;

  white.red = 0xff;
  white.green = 0xff;
  white.blue = 0xff;
  white.alpha = 0xff;
  flags = 0x10;
  if (numVerts == 0) {
    return;
  }
  if (numVerts > 0x2000) {
    numVerts = 0x2000;
  }
  if (numTris > 0x2000) {
    numTris = 0x2000;
  }
  if (color == 0) {
    color = &white;
  }
  if (color->alpha == 0xff) {
    flags = 0x12;
  }
  for (i = 0; i < numTris; i++) {
    RpTriangle *t = &tri[i];
    unsigned short v0 = t->vertIndex[0];
    unsigned short v1 = t->vertIndex[1];
    unsigned short v2 = t->vertIndex[2];
    DAT_007bbe48[numIndices++] = v0;
    DAT_007bbe48[numIndices++] = v1;
    DAT_007bbe48[numIndices++] = v1;
    DAT_007bbe48[numIndices++] = v2;
    DAT_007bbe48[numIndices++] = v2;
    DAT_007bbe48[numIndices++] = v0;
  }
#define VSET(k) \
    DAT_00773e48[i + k].objVertex.x = verts[i + k].x; \
    DAT_00773e48[i + k].objVertex.y = verts[i + k].y; \
    DAT_00773e48[i + k].objVertex.z = verts[i + k].z; \
    DAT_00773e48[i + k].color = RWRGBALONG(color->red, color->green, color->blue, color->alpha);
  i = 0;
  if (numVerts >= 4) {
    int j = 3;
    do {
      VSET(0) VSET(1) VSET(2) VSET(3)
      i += 4;
      j += 4;
    } while (j < numVerts);
  }
  for (; i < numVerts; i++) {
    VSET(0)
  }
  RenderStateSet(7, 1);
  RenderStateSet(1, 0);
  if (color->alpha != 0xff) {
    RenderStateSet(0xc, 1);
  }
  if (FUN_004cd070(DAT_00773e48, numVerts, 0, flags)) {
    FUN_004cd170(1, DAT_007bbe48, numIndices);
    FUN_004cd140();
  }
  if (color->alpha != 0xff) {
    RenderStateSet(0xc, 0);
  }
  RenderStateSet(7, 2);
}
#undef VSET
#undef RenderStateSet
#undef RWRGBALONG
