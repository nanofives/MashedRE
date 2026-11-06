// FLAGS: /O2 /GS /Ob1
/* 0x00478220: 100% (regs 100%), hand pass h9 (MATCH-S12). RwIm3DVertex[6] + ushort idx[8] locals, the
   debug-box record (0x4c B, base param_1+0xc1c0, count at +0x16240) copied component by component (struct
   assignment of RwV3d moves the base register and uses lea+copy). Residual: none. */
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
typedef struct { float x, y, z; } RwV3d;
typedef struct { RwV3d objVertex; RwV3d objNormal; unsigned int color; float u, v; } RwIm3DVertex;
typedef struct { RwV3d a; RwV3d b; RwV3d p[4]; int pad; } DbgBox;
typedef int (*RwRenderStateSetFn)(int, int);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
int FUN_004cd070(RwIm3DVertex *verts, int n, void *mat, unsigned int flags);
int FUN_004cd140(void);
int FUN_004cd170(int type, unsigned short *idx, int n);
// MATCH: 0x00478220 FUN_00478220
void FUN_00478220(int param_1)
{
  unsigned short idx[8];
  RwIm3DVertex v[6];
  DbgBox *b;
  int i;

  idx[0] = 0; idx[1] = 1; idx[2] = 1; idx[3] = 2;
  idx[4] = 2; idx[5] = 3; idx[6] = 4; idx[7] = 5;
  for (i = 0; i < *(int *)(param_1 + 0x16240); i++) {
    b = (DbgBox *)(param_1 + 0xc1c0) + i;
    v[0].objVertex.x = b->p[0].x; v[0].objVertex.y = b->p[0].y; v[0].objVertex.z = b->p[0].z;
    v[1].objVertex.x = b->p[1].x; v[1].objVertex.y = b->p[1].y; v[1].objVertex.z = b->p[1].z;
    v[2].objVertex.x = b->p[2].x; v[2].objVertex.y = b->p[2].y; v[2].objVertex.z = b->p[2].z;
    v[3].objVertex.x = b->p[3].x; v[3].objVertex.y = b->p[3].y; v[3].objVertex.z = b->p[3].z;
    v[4].objVertex.x = b->b.x; v[4].objVertex.y = b->b.y; v[4].objVertex.z = b->b.z;
    v[5].objVertex.x = b->a.x + b->b.x;
    v[5].objVertex.y = b->a.y + b->b.y;
    v[5].objVertex.z = b->a.z + b->b.z;
    v[0].color = 0xff40ff40;
    v[1].color = 0xff4040ff;
    v[2].color = 0xffffff40;
    v[3].color = 0xff40ffff;
    v[4].color = 0xff000000;
    v[5].color = 0xffffffff;
    (*(RwRenderStateSetFn *)(DAT_007d3ff8 + 0x20))(1, 0);
    if (FUN_004cd070(v, 6, 0, 2)) {
      FUN_004cd170(1, idx, 8);
      FUN_004cd140();
    }
  }
}
