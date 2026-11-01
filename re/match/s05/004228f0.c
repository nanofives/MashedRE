// FLAGS: /O2 /GS /Ob1
/* MATCH-S05 0x004228f0 (399 B): best 79.2% exact (regs 79.2, ops 95.8) by hand. Raw g2s 46.3%.
   Shape: record r = &DAT_006403e8[i] (0xf40 stride: 64 triangles of 0x3c at +0, 4 texture pointers at 0xf00,
   RGBA bytes at 0xf28, count at 0xf30, texture index at 0xf34, f3c written here). Copies count triangles into a
   local array of 0x24-byte vertices (pos, unwritten 3 dwords, packed colour a<<24|r<<16|g<<8|b, u, v), then
   renderStateSet(1, raster) if any, (0xc,1), (2,3), FUN_004cd070(verts, count*3, 0, 0x19) and on nonzero
   FUN_004cd2d0(3) + FUN_004cd140(), then (0xc,0). The cam+0x68 pair is copied to a local 8-byte struct first.
   What moved it: the u/v copies are fld/fstp in the original while positions are integer moves; only passing
   u/v by value to an __inline helper (SetV) produced that (direct copies, (double) casts, *1.0f, +0.0f all fold
   to integer moves). An index loop with the colour computed inside the loop (hoisted by the optimizer) fixed
   the prologue/count reload (73 -> 79%). Tried: pointer-walk loop, colour after the three vertices, other
   statement orders inside SetV, a 3-iteration inner loop (27.6%), match_permute (2 runs, no gain).
   Residual (b): the loop body store schedule (colour stores and the pointer increments land earlier than in
   the original); the right helper/macro shape for the vertex fill is not found yet. */
/* ---- MATCH-S05 preamble (same text in every re/match/s05 source; not part of any function body) ----
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
typedef struct { float x, y, z; } V3_8f0;
typedef struct { float x, y; } V2_8f0;
typedef struct { V3_8f0 p[3]; float uv[3][2]; } Tri_8f0;          /* 0x3c */
typedef struct { V3_8f0 pos; V3_8f0 nrm; unsigned int color; float u, v; } Im3DVertex_8f0;  /* 0x24 */
typedef struct { void *raster; } Tex_8f0;
typedef struct {
  Tri_8f0 tri[64];             /* 0x000 */
  Tex_8f0 *tex[4];             /* 0xf00 */
  float f10, f14, f18, f1c, f20, f24;
  unsigned char col[4];    /* 0xf28 r,g,b,a */
  int f2c;
  int count;               /* 0xf30 */
  int texIdx;              /* 0xf34 */
  int f38;
  float f3c;
} Rec_8f0;
struct RwGlobals_8f0 { char pad[0x20]; int (*renderStateSet)(int, void *); };
MGLOBALA(Rec_8f0, DAT_006403e8, 4, 0x006403e8);
MGLOBAL(float, DAT_005cc320, 0x005cc320);
MGLOBAL(float, DAT_005cc328, 0x005cc328);
MGLOBAL(struct RwGlobals_8f0 *, DAT_007d3ff8, 0x007d3ff8);
int FUN_004cd070(void *verts, int n, void *ltm, int flags);
int FUN_004cd2d0(int prim);
int FUN_004cd140(void);
static __inline void SetV_8f0(Im3DVertex_8f0 *d, V3_8f0 *p, unsigned int c, float u, float w) { d->pos.x = p->x; d->pos.y = p->y; d->pos.z = p->z; d->u = u; d->v = w; d->color = c; }
// MATCH: 0x004228f0 FUN_004228f0
void FUN_004228f0(int i, char *cam)
{
  Rec_8f0 *r = &DAT_006403e8[i];
  void *raster = 0;
  V2_8f0 t;
  int k;
  Im3DVertex_8f0 verts[192];
  if (r->count != 0) {
    t = *(V2_8f0 *)(cam + 0x68);
    r->f3c = ((DAT_005cc320 - t.x) + DAT_005cc320) * DAT_005cc328;
    if (r->tex[r->texIdx] != 0)
      raster = r->tex[r->texIdx]->raster;
    for (k = 0; k < r->count; k++) {
      unsigned int c = ((unsigned int)r->col[3] << 24) | ((unsigned int)r->col[0] << 16) |
                       ((unsigned int)r->col[1] << 8) | r->col[2];
      SetV_8f0(&verts[k * 3 + 0], &r->tri[k].p[0], c, r->tri[k].uv[0][0], r->tri[k].uv[0][1]);
      SetV_8f0(&verts[k * 3 + 1], &r->tri[k].p[1], c, r->tri[k].uv[1][0], r->tri[k].uv[1][1]);
      SetV_8f0(&verts[k * 3 + 2], &r->tri[k].p[2], c, r->tri[k].uv[2][0], r->tri[k].uv[2][1]);
    }
    if (raster != 0)
      DAT_007d3ff8->renderStateSet(1, raster);
    DAT_007d3ff8->renderStateSet(0xc, (void *)1);
    DAT_007d3ff8->renderStateSet(2, (void *)3);
    if (FUN_004cd070(verts, r->count * 3, 0, 0x19)) {
      FUN_004cd2d0(3);
      FUN_004cd140();
    }
    DAT_007d3ff8->renderStateSet(0xc, (void *)0);
  }
}
