// FLAGS: /O2 /GS /Ob1
/* MATCH-S18 0x004b5d90 (h5): best 60.1% (regs 60.1%, ops 68.1%), 187/213 ins. Raw g2s 28.2%.
   Wireframe box: 8 RwIm3DVertex corners (+-halfSize, default {0.5,0.5,0.5}; vertex k has +x/+y/+z for bits
   0/1/2 of k), one RGBA colour (default white), identity RwMatrix (RwMatrixSetIdentity macro shape, flags |=
   0x20003) optionally transformed by ltm (0x004c52f0, combine 0), RwIm3DTransform (0x004cd070), 12
   RwIm3DRenderLine (0x004cd430) edges, RwIm3DEnd (0x004cd140); vertex alpha state 0xc set/cleared when the
   transform flags lack 0x2. Moved it: typed RwMatrix/RwIm3DVertex locals; colour/halfSize defaults and the
   0x10/0x12 flag in source order (prologue up to the first fld is identical); nx/ny/nz locals and float x/y locals
   for +x/+y (gives the original's 0x178 frame: two extra 4-byte slots); m.flags |= first in the identity block;
   the permuter (1164 compiles) moved the y local and the v[2]/v[6] stores (statement order only, same values).
   Residual: x87 scheduling of the 24 vertex position stores. The original duplicates each negated value with
   `fld st(0)` and routes some copies through int temps at [esp+8]/[esp+0xc]/[esp+0x10] (packed over the dead
   white colour slot) and keeps the matrix constant stores after the vertex block; 9176 keeps one copy with
   `fst`, hoists the matrix constants into the fp block and does not pack a third temp over the colour slot
   (x/y/z locals give a 0x17c frame). Tried: macro vs explicit stores, per-vertex colour, vertex-copy chains,
   corner loop over i&1/i&2/i&4, RwV3d neg local, x,y,z / z-only / y,x locals, vertex pointer, matrix pointer,
   matrix store orders, /G5 (movzx becomes byte loads, 47%). Class (b): local/temp shape not found (the temp
   packing may be compiler-version, unproven). */
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
typedef struct RwMatrix { RwV3d right; unsigned int flags; RwV3d up; unsigned int pad1; RwV3d at; unsigned int pad2; RwV3d pos; unsigned int pad3; } RwMatrix;
typedef struct RwIm3DVertex { RwV3d objVertex; RwV3d objNormal; unsigned int color; float u, v; } RwIm3DVertex;
typedef int (*RwRenderStateSetFn)(int state, void *value);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
RwMatrix *FUN_004c52f0(RwMatrix *matrix, const RwMatrix *transform, int combineOp);
void *FUN_004cd070(void *pVerts, int numVerts, RwMatrix *ltm, unsigned int flags);
int FUN_004cd140(void);
int FUN_004cd430(int vert1, int vert2);
#define RWRGBALONG(r, g, b, a) ((unsigned int)(((a) << 24) | ((r) << 16) | ((g) << 8) | (b)))
#define RenderStateSet(s, v) ((RwRenderStateSetFn)*(int *)(DAT_007d3ff8 + 0x20))((s), (void *)(v))
#define RwIm3DVertexSetPos(vert, imx, imy, imz) ((vert)->objVertex.x = (imx), (vert)->objVertex.y = (imy), (vert)->objVertex.z = (imz))
#define RwIm3DVertexSetRGBA(vert, r, g, b, a) ((vert)->color = RWRGBALONG(r, g, b, a))
// MATCH: 0x004b5d90 FUN_004b5d90
void FUN_004b5d90(RwV3d *halfSize, RwMatrix *ltm, RwRGBA *color)
{
  RwRGBA white;
  RwV3d defHalf;
  RwMatrix m;
  RwIm3DVertex v[8];
  unsigned int flags;
  unsigned int c;

  white.red = 0xff;
  white.green = 0xff;
  white.blue = 0xff;
  white.alpha = 0xff;
  defHalf.x = 0.5f;
  defHalf.y = 0.5f;
  defHalf.z = 0.5f;
  flags = 0x10;
  if (color == 0) {
    color = &white;
  }
  if (color->alpha == 0xff) {
    flags = 0x12;
  }
  if (halfSize == 0) {
    halfSize = &defHalf;
  }
  {
    float nx = -halfSize->x;
    float ny = -halfSize->y;
    float y = halfSize->y;
    float nz = -halfSize->z;
    float x = halfSize->x;
    c = RWRGBALONG(color->red, color->green, color->blue, color->alpha);
    RwIm3DVertexSetPos(&v[0], nx, ny, nz);
    RwIm3DVertexSetPos(&v[1], x, ny, nz);
    RwIm3DVertexSetPos(&v[3], x, y, nz);
    RwIm3DVertexSetPos(&v[2], nx, y, nz);
    RwIm3DVertexSetPos(&v[4], nx, ny, halfSize->z);
    RwIm3DVertexSetPos(&v[5], x, ny, halfSize->z);
    RwIm3DVertexSetPos(&v[7], x, y, halfSize->z);
    v[0].color = c; v[1].color = c; v[2].color = c; v[3].color = c;
    v[4].color = c; v[5].color = c; v[6].color = c; v[7].color = c;
    RwIm3DVertexSetPos(&v[6], nx, y, halfSize->z);
  }
  m.flags = m.flags | 0x20003;
  m.right.x = m.up.y = m.at.z = 1.0f;
  m.right.y = m.right.z = m.up.x = 0.0f;
  m.up.z = m.at.x = m.at.y = 0.0f;
  m.pos.x = m.pos.y = m.pos.z = 0.0f;
  if (ltm) {
    FUN_004c52f0(&m, ltm, 0);
  }
  if (!(flags & 2)) {
    RenderStateSet(0xc, 1);
  }
  RenderStateSet(1, 0);
  if (FUN_004cd070(v, 8, &m, flags)) {
    FUN_004cd430(0, 1);
    FUN_004cd430(1, 3);
    FUN_004cd430(3, 2);
    FUN_004cd430(2, 0);
    FUN_004cd430(4, 5);
    FUN_004cd430(5, 7);
    FUN_004cd430(7, 6);
    FUN_004cd430(6, 4);
    FUN_004cd430(0, 4);
    FUN_004cd430(2, 6);
    FUN_004cd430(1, 5);
    FUN_004cd430(3, 7);
    FUN_004cd140();
  }
  if (!(flags & 2)) {
    RenderStateSet(0xc, 0);
  }
}
#undef RwIm3DVertexSetRGBA
#undef RwIm3DVertexSetPos
#undef RenderStateSet
#undef RWRGBALONG
