// FLAGS: /O2 /GS /Ob1
/* 0x00477be0: 93.2% (regs 93.2%), hand pass h9 (MATCH-S12). Trail record (0x2c0 B x 256 at 0x00693198,
   live bitmask at 0x006bf198): RwIm3DVertex[16], RwV3d pts[8], last, dist, count. The 0.5 is a literal (0.5f;
   the compiler's constant lands at 0x005cc32c, as in S11's 0046b1c0). Residual (b): the `t = *pos` copy
   stores are scheduled after the first fadd, the second fld/fchs one slot late, and `count++` comes out as
   load/inc/store with the epilogue pops after it (original: inc dword ptr [esi+0x2b0], pops before the last
   fstp). Tried: separate diff local (76%), scalar copies (77%), copy order (73%), t -= w*dir (90.6%),
   DAT_00693198[i]. form (67%), ++ as expression/unsigned count, /G5../G7, permuter 3 rounds (its only gain
   moved count++ above the stores, a semantic change, rejected). */
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
typedef struct {
  RwIm3DVertex verts[16];
  RwV3d pts[8];
  RwV3d last;
  float dist;
  int count;
  int pad[3];
} Trail;
MGLOBAL(RwV3d, DAT_006132d0, 0x006132d0);
MGLOBALA(Trail, DAT_00693198, 256, 0x00693198);
MGLOBALA(unsigned char, DAT_006bf198, 32, 0x006bf198);
float FUN_004c3ac0(RwV3d *v);
// MATCH: 0x00477be0 FUN_00477be0
int FUN_00477be0(int param_1, RwV3d *param_2, RwV3d *param_3, float param_4, float param_5)
{
  RwV3d t;
  Trail *tr;
  int i;
  float s;
  float len;

  i = param_1 - 1;
  if ((DAT_006bf198[i >> 3] & (1 << (i & 7))) == 0)
    return 0;
  if (param_3 == 0)
    param_3 = &DAT_006132d0;
  tr = &DAT_00693198[i];
  if (tr->count == 8)
    return 0;
  if (tr->count == 0) {
    tr->last = *param_2;
    tr->dist = 0.0f;
  } else {
    t.x = tr->last.x - param_2->x;
    t.y = tr->last.y - param_2->y;
    t.z = tr->last.z - param_2->z;
    len = FUN_004c3ac0(&t);
    if (len > param_5)
      return 0;
    tr->dist = len;
  }
  s = param_4 * 0.5f;
  tr->pts[tr->count] = *param_2;
  t = *param_2;
  t.x += s * param_3->x;
  t.y += s * param_3->y;
  t.z += s * param_3->z;
  tr->verts[tr->count * 2].objVertex.x = t.x;
  tr->verts[tr->count * 2].objVertex.y = t.y;
  tr->verts[tr->count * 2].objVertex.z = t.z;
  tr->verts[tr->count * 2].u = 0.0f;
  tr->verts[tr->count * 2].v = (float)tr->count;
  param_4 = -param_4;
  t.x += param_4 * param_3->x;
  t.y += param_4 * param_3->y;
  t.z += param_4 * param_3->z;
  tr->verts[tr->count * 2 + 1].objVertex.x = t.x;
  tr->verts[tr->count * 2 + 1].objVertex.y = t.y;
  tr->verts[tr->count * 2 + 1].objVertex.z = t.z;
  tr->verts[tr->count * 2 + 1].u = 1.0f;
  tr->verts[tr->count * 2 + 1].v = (float)tr->count;
  tr->count++;
  return param_1;
}
