// FLAGS: /O2 /Ob1 /G5
/* MATCH-S20 0x004d07b0 (h2): 77.0% (regs 77.1%). Raw 35.5% (Ghidra lost track of the stack: unaff_retaddr). Hand-
   written from odis: camera-raster to texture copy. Walk to the top raster, lock the camera raster (0x004cafd0) or
   set the render surface (0x004c7a20), set the D3D viewport (+0xbc); when 0x004c9cd0 succeeds, fill the four Im2D
   vertices at 0x007d4658 (positions from the rect or the raster size, uv = offset / top size through fidiv), save 8
   + 2 render states, draw the 4-vertex strip (0x004dba60), restore them; then StretchRect (+0x88) from the render
   surface into the texture surface with the caps-selected filter, or restore the target. Permuter moved two
   independent stores (+1.9). Residual (a): every instruction and operand matches except stack offsets. The original
   keeps 4 locals (the fild temp, saved states 10/7/0x16 and the surface pointer) in the dead homes of
   param_1..param_3 with a 0x58 frame. 9176 gives a 0x68 frame with separate slots. This is the same packing gap as
   0x004cc5e0 and 0x004d3510. */
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
typedef unsigned int undefined4;
typedef unsigned int uint;
typedef unsigned char uchar;
typedef struct RwRaster RwRaster;
struct RwRaster {
  RwRaster *parent; uchar *cpPixels; uchar *palette; int width; int height; int depth; int stride;
  short nOffsetX; short nOffsetY; uchar cType; uchar cFlags; uchar privateFlags; uchar cFormat;
};
typedef struct {
  int *texture; void *palette; uchar alpha; uchar cube : 4; uchar face : 4; uchar automipmapgen : 4;
  uchar compressed : 4; uchar lockedMipLevel; int *lockedSurface;
} RasExt;
typedef struct { int x, y, w, h; } RwRect;
typedef struct { long left, top, right, bottom; } RECT_;
typedef struct { uint X, Y, Width, Height; float MinZ, MaxZ; } VIEWPORT_;
typedef struct { float x, y, z, rhw; uint color; float u, v; } Im2DVertex;
typedef long (__stdcall *setvp_fn)(int *, VIEWPORT_ *);
typedef long (__stdcall *getsurf_fn)(int *, uint, int **);
typedef long (__stdcall *getcube_fn)(int *, uint, uint, int **);
typedef long (__stdcall *stretch_fn)(int *, int *, RECT_ *, int *, RECT_ *, uint);
typedef long (__stdcall *com0_fn)(int *);
MGLOBAL(int, DAT_00911ae4, 0x00911ae4);
MGLOBAL(RwRaster *, DAT_007d46c8, 0x007d46c8);
MGLOBAL(int *, DAT_007d4118, 0x007d4118);
MGLOBAL(int *, DAT_007d4110, 0x007d4110);
MGLOBAL(uint, DAT_00912094, 0x00912094);
MGLOBAL(int, DAT_007d4114, 0x007d4114);
MGLOBALA(Im2DVertex, DAT_007d4658, 4, 0x007d4658);
int FUN_004c79f0(int p0);
int FUN_004c7a20(int p0, void *p1);
int FUN_004c9cd0(void);
int FUN_004cafd0(int p0, RwRaster *p1);
int FUN_004d5480(int p0, int p1);
int FUN_004d54d0(int p0, int *p1);
int FUN_004d6910(int p0, int *p1);
int FUN_004d6c40(int p0);
int FUN_004d6c90(int p0);
int FUN_004d6ce0(int p0, int p1);
int FUN_004d7100(int p0);
int FUN_004d7480(int p0, int p1);
int FUN_004dba60(int p0, Im2DVertex *p1, int p2);
#define EXT(r) ((RasExt *)((uchar *)(r) + DAT_00911ae4))
// MATCH: 0x004d07b0 FUN_004d07b0
void FUN_004d07b0(RwRaster *param_1,RwRect *param_2,int param_3,int param_4)
{
  int locked;
  RwRaster *top;
  RasExt *ext;
  VIEWPORT_ vp;
  int s10, s11, s12, s14, s9, s1, s3, s4, s7, s16;
  int *surface;
  RECT_ srcRect;
  RECT_ dstRect;
  uint filter;

  locked = 0;
  top = param_1;
  do {
    top = top->parent;
  } while (top != top->parent);
  ext = EXT(top);
  if (DAT_007d46c8->cType != 2) {
    locked = FUN_004cafd0(0, DAT_007d46c8);
    if (locked) goto locked_ok;
  }
  FUN_004c7a20(0, DAT_007d4118);
locked_ok:
  FUN_004c79f0(0);
  vp.X = DAT_007d46c8->nOffsetX;
  vp.Y = DAT_007d46c8->nOffsetY;
  vp.Width = DAT_007d46c8->width;
  vp.Height = DAT_007d46c8->height;
  vp.MinZ = 0.0f;
  vp.MaxZ = 1.0f;
  (*(setvp_fn *)(*DAT_007d4110 + 0xbc))(DAT_007d4110, &vp);
  if (FUN_004c9cd0()) {
    DAT_007d4658[0].x = (float)param_2->x;
    DAT_007d4658[0].y = (float)param_2->y;
    DAT_007d4658[1].x = (float)param_2->x;
    if (param_3) {
      DAT_007d4658[1].y = (float)(param_2->h + param_2->y);
      DAT_007d4658[2].x = (float)(param_2->w + param_2->x);
      DAT_007d4658[2].y = (float)param_2->y;
      DAT_007d4658[3].x = (float)(param_2->w + param_2->x);
      DAT_007d4658[3].y = (float)(param_2->h + param_2->y);
    } else {
      DAT_007d4658[2].y = (float)param_2->y;
      DAT_007d4658[1].y = (float)(param_1->height + param_2->y);
      DAT_007d4658[2].x = (float)(param_2->x + param_1->width);
      DAT_007d4658[3].x = (float)(param_2->x + param_1->width);
      DAT_007d4658[3].y = (float)(param_1->height + param_2->y);
    }
    DAT_007d4658[0].z = 0.0f;
    DAT_007d4658[0].rhw = 1.0f;
    DAT_007d4658[0].color = 0xffffffff;
    DAT_007d4658[0].u = (float)param_1->nOffsetX / top->width;
    DAT_007d4658[0].v = (float)param_1->nOffsetY / top->height;
    DAT_007d4658[1].z = 0.0f;
    DAT_007d4658[1].rhw = 1.0f;
    DAT_007d4658[1].color = 0xffffffff;
    DAT_007d4658[1].u = (float)param_1->nOffsetX / top->width;
    DAT_007d4658[1].v = (float)(param_1->nOffsetY + param_1->height) / top->height;
    DAT_007d4658[2].z = 0.0f;
    DAT_007d4658[2].rhw = 1.0f;
    DAT_007d4658[2].color = 0xffffffff;
    DAT_007d4658[2].u = (float)(param_1->nOffsetX + param_1->width) / top->width;
    DAT_007d4658[2].v = (float)param_1->nOffsetY / top->height;
    DAT_007d4658[3].z = 0.0f;
    DAT_007d4658[3].rhw = 1.0f;
    DAT_007d4658[3].color = 0xffffffff;
    DAT_007d4658[3].u = (float)(param_1->nOffsetX + param_1->width) / top->width;
    DAT_007d4658[3].v = (float)(param_1->nOffsetY + param_1->height) / top->height;
    FUN_004d6910(10, &s10);
    FUN_004d6910(0xb, &s11);
    FUN_004d6910(0xc, &s12);
    FUN_004d6910(0xe, &s14);
    FUN_004d6910(9, &s9);
    FUN_004d6910(3, &s3);
    FUN_004d6910(4, &s4);
    FUN_004d6910(1, &s1);
    FUN_004d54d0(7, &s7);
    FUN_004d54d0(0x16, &s16);
    FUN_004d6ce0((int)top, 0);
    if (param_4 && ext->alpha) {
      FUN_004d7100(1);
      FUN_004d6c40(5);
      FUN_004d6c90(6);
    } else {
      FUN_004d7100(0);
    }
    FUN_004d7480(0xe, 0);
    FUN_004d7480(9, (param_3 != 0) + 1);
    FUN_004d7480(2, 3);
    FUN_004d5480(7, 0);
    FUN_004d5480(0x16, 1);
    FUN_004dba60(4, DAT_007d4658, 4);
    FUN_004d5480(7, s7);
    FUN_004d5480(0x16, s16);
    FUN_004d7480(10, s10);
    FUN_004d7480(0xb, s11);
    FUN_004d7480(0xc, s12);
    FUN_004d7480(0xe, s14);
    FUN_004d7480(9, s9);
    FUN_004d6ce0(s1, 0);
    FUN_004d7480(3, s3);
    FUN_004d7480(4, s4);
  }
  if (DAT_007d46c8->cType != 2) {
    if (locked == 0) {
      if (ext->cube) {
        (*(getcube_fn *)(*ext->texture + 0x48))(ext->texture, ext->face, 0, &surface);
      } else {
        (*(getsurf_fn *)(*ext->texture + 0x48))(ext->texture, 0, &surface);
      }
      srcRect.left = DAT_007d46c8->nOffsetX;
      srcRect.top = DAT_007d46c8->nOffsetY;
      srcRect.right = DAT_007d46c8->width + srcRect.left;
      srcRect.bottom = DAT_007d46c8->height + srcRect.top;
      dstRect.left = param_1->nOffsetX;
      dstRect.top = param_1->nOffsetY;
      dstRect.right = param_1->width + dstRect.left;
      dstRect.bottom = param_1->height + dstRect.top;
      filter = 0;
      if (param_3) {
        if (DAT_00912094 & 0x2000200) filter = 2;
      } else {
        if (DAT_00912094 & 0x1000100) filter = 1;
      }
      (*(stretch_fn *)(*DAT_007d4110 + 0x88))(DAT_007d4110, DAT_007d4118, &srcRect, surface, &dstRect, filter);
      (*(com0_fn *)(*surface + 8))(surface);
      return;
    }
    FUN_004c7a20(0, DAT_007d4118);
    FUN_004c79f0(DAT_007d4114);
  }
}
