// FLAGS: /O2 /GS /Ob1
/* MATCH-S18 h2 0x004b5af0 (661 B): by hand, best 85.7% (regs 85.7%, ops 95.6%). Draws a matrix as three Im3D
   lines: 6 vertices, colours from param_2 (all six) or the local red/green/blue table (loop, 2 per axis), positions
   pos and pos + axis * param_3 (tmp then copy), render state 0xc set when param_2 alpha != 0xff (flags 0x12 -> 0x10),
   FUN_004cd070(verts, 6, 0, flags), three FUN_004cd430 lines, FUN_004cd140.
   What moved it: the loop must survive (colour computed twice in the body, 24.7% -> 31.7%); axis/pos pointer
   locals initialised at the declarations load param_1 into eax before the colour code, so flags spills around the
   loop and ebp is shrink-wrapped into it as in the original (46.2% -> 85.7%).
   Residual (b): the y/z axis products load the axis first in the original (fld [ax+4]; fmul s), 9176 loads s
   first; tried operand order, __inline scale/add helpers, direct/tmp/struct-copy forms, pos-copy placement,
   /G5 /G7 / no /GS, permuter (455 compiles): no move. */
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
typedef struct { float x, y, z; } V3_4b5af0;
typedef struct { V3_4b5af0 right; uint flags; V3_4b5af0 up; uint pad1; V3_4b5af0 at; uint pad2; V3_4b5af0 pos; uint pad3; } Mat_4b5af0;
typedef struct { V3_4b5af0 objVertex; V3_4b5af0 objNormal; uint color; float u; float v; } Im3DV_4b5af0;
typedef struct { uchar red; uchar green; uchar blue; uchar alpha; } RGBA_4b5af0;
typedef int (*RenderStateSetFn_4b5af0)(int, void *);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
void *FUN_004cd070(Im3DV_4b5af0 *p0, int p1, void *p2, uint p3);
int FUN_004cd140(void);
int FUN_004cd430(int p0, int p1);
#define RGBALONG_4b5af0(c) ((uint)(((c).alpha << 24) | ((c).red << 16) | ((c).green << 8) | (c).blue))
#define RSSET_4b5af0(s, v) ((*(RenderStateSetFn_4b5af0 *)(DAT_007d3ff8 + 0x20))((s), (void *)(v)))
// MATCH: 0x004b5af0 FUN_004b5af0
void FUN_004b5af0(Mat_4b5af0 *param_1, RGBA_4b5af0 *param_2, float param_3)
{
  V3_4b5af0 *right = &param_1->right;
  V3_4b5af0 *up = &param_1->up;
  V3_4b5af0 *at = &param_1->at;
  V3_4b5af0 *pos = &param_1->pos;
  RGBA_4b5af0 cols[3] = { { 255, 0, 0, 255 }, { 0, 255, 0, 255 }, { 0, 0, 255, 255 } };
  uint flags;
  Im3DV_4b5af0 verts[6];
  V3_4b5af0 tmp;
  uint c;
  int i;

  flags = 0x12;
  if (param_2 != 0) {
    c = RGBALONG_4b5af0(*param_2);
    verts[0].color = c;
    verts[1].color = c;
    verts[2].color = c;
    verts[3].color = c;
    verts[4].color = c;
    verts[5].color = c;
  } else {
    for (i = 0; i < 3; i++) {
      verts[i * 2].color = RGBALONG_4b5af0(cols[i]);
      verts[i * 2 + 1].color = RGBALONG_4b5af0(cols[i]);
    }
  }
  verts[0].objVertex.x = pos->x;
  verts[0].objVertex.y = pos->y;
  verts[0].objVertex.z = pos->z;
  verts[2].objVertex.x = pos->x;
  verts[2].objVertex.y = pos->y;
  verts[2].objVertex.z = pos->z;
  verts[4].objVertex.x = pos->x;
  verts[4].objVertex.y = pos->y;
  verts[4].objVertex.z = pos->z;
  tmp.x = param_3 * right->x;
  tmp.y = right->y * param_3;
  tmp.z = right->z * param_3;
  tmp.x = tmp.x + pos->x;
  tmp.y = tmp.y + pos->y;
  tmp.z = tmp.z + pos->z;
  verts[1].objVertex.x = tmp.x;
  verts[1].objVertex.y = tmp.y;
  verts[1].objVertex.z = tmp.z;
  tmp.x = param_3 * up->x;
  tmp.y = up->y * param_3;
  tmp.z = up->z * param_3;
  tmp.x = tmp.x + pos->x;
  tmp.y = tmp.y + pos->y;
  tmp.z = tmp.z + pos->z;
  verts[3].objVertex.x = tmp.x;
  verts[3].objVertex.y = tmp.y;
  verts[3].objVertex.z = tmp.z;
  tmp.x = param_3 * at->x;
  tmp.y = at->y * param_3;
  tmp.z = at->z * param_3;
  tmp.x = tmp.x + pos->x;
  tmp.y = tmp.y + pos->y;
  tmp.z = tmp.z + pos->z;
  verts[5].objVertex.x = tmp.x;
  verts[5].objVertex.y = tmp.y;
  verts[5].objVertex.z = tmp.z;
  if (param_2 != 0 && param_2->alpha != 255) {
    RSSET_4b5af0(0xc, 1);
    flags = 0x10;
  }
  RSSET_4b5af0(1, 0);
  if (FUN_004cd070(verts, 6, 0, flags) != 0) {
    FUN_004cd430(0, 1);
    FUN_004cd430(2, 3);
    FUN_004cd430(4, 5);
    FUN_004cd140();
  }
  if (param_2 != 0 && param_2->alpha != 255) {
    RSSET_4b5af0(0xc, 0);
  }
}
