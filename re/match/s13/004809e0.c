// FLAGS: /O2 /GS /Ob1
/* MATCH-S13 0x004809e0 (h9, by hand): 98.4% (regs 98.4%). Car respawn-place (car array 0x008815b4, stride
   0xd04; 300.0 @0x005cd8fc is a literal, a global float costs 4 points). Vertex read through a pointer to
   the element (direct indexing 75%). Residual (a): the second fld v.y is scheduled before lea/push of the
   FUN_0046d4a0 args in the original, after them in ours. Tried: permuter, global vs literal constant,
   operand order, inline scale helper / macro, pointer to car pos, local order. */
/* ---- MATCH-S13 preamble (same text in every re/match/s13 source; not part of any function body) ----
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
typedef struct V3 { float x, y, z; } V3;
typedef struct Mat { V3 right; int f0c; V3 up; int f1c; V3 at; int f2c; V3 pos; int f3c; } Mat;
typedef struct Vtx { V3 p; char pad[0x14]; } Vtx;   /* stride 0x20 */
typedef struct Car9e0 {
  int active;           /* +0x000 (0x008815b4) */
  int timer;            /* +0x004 */
  char pad08[0x11c];
  V3 off;               /* +0x124 (0x008816d8) */
  char pad130[0x86c];
  V3 pos;               /* +0x99c (0x00881f50) */
  char pad9a8[0x35c];
} Car9e0;               /* stride 0xd04 */
MGLOBALA(Car9e0, DAT_008815b4, 16, 0x008815b4);
MGLOBALA(int, DAT_006c9a78, 16, 0x006c9a78);
int FUN_0057c210(int p0);
int FUN_00426c00(void);
void FUN_0055ae70(int p0, int p1, int p2);
void FUN_0046d4a0(Mat **p0, int p1);
void FUN_004c3df0(V3 *p0, V3 *p1, int p2, Mat *p3);
void FUN_004c45f0(Mat *p0);
void FUN_004c0e50(int p0);
void FUN_0055dff0(int p0, int p1);
void FUN_0055ac00(int p0, int p1, int p2);
// MATCH: 0x004809e0 FUN_004809e0
int FUN_004809e0(int i)
{
  int obj;
  int *p;
  Mat *m;
  int obj2;
  V3 o;
  V3 v;
  Mat *dst;

  if (i >= 16) {
    return 0;
  }
  if (DAT_008815b4[i].active == 0) {
    return 0;
  }
  obj = FUN_0057c210(DAT_006c9a78[i]);
  if (FUN_00426c00() != 0x26) {
    FUN_0055ae70(*(int *)(obj + 0x24), obj, 1);
  }
  p = *(int **)(*(int *)(obj + 0x10) + 4);
  if (p != 0) {
    { Vtx *t = &(*(Vtx **)(*(int *)(*p + 0x10) + 8))[p[1]]; v = t->p; }
    DAT_008815b4[i].pos.x = v.x * 300.0f;
    DAT_008815b4[i].pos.y = v.y * 300.0f;
    DAT_008815b4[i].pos.z = v.z * 300.0f;
    FUN_0046d4a0(&m, i);
    obj2 = FUN_0057c210(DAT_006c9a78[i]);
    dst = *(Mat **)(*(int *)(obj2 + 0x10) + 8);
    *dst = *m;
    o = DAT_008815b4[i].off;
    FUN_004c3df0(&o, &o, 1, dst);
    dst->pos.x -= o.x;
    dst->pos.y -= o.y;
    dst->pos.z -= o.z;
    FUN_004c45f0(dst);
    FUN_004c0e50(*(int *)(DAT_006c9a78[i] + 4));
    FUN_0055dff0(obj2, 0);
    FUN_0055ac00(*(int *)(obj2 + 0x24), obj2, 1);
  }
  DAT_008815b4[i].active = 0;
  DAT_008815b4[i].timer = 600;
  return 1;
}
