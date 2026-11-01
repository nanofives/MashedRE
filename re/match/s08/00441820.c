// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 0x00441820: 100% by hand (raw did not compile). Moves: 90.0f / 180.0f as literals (they are
   compiler constants in .rdata at 0x005ccad0 / 0x005cd09c, a global reverses the fadd operand order); the
   vectors from FUN_00426cc0 and FUN_00426d00(.., 3) copied member by member (a struct assignment gives int movs
   and a different register pick); the matrix as float[16] (a char buffer adds a /GS cookie). */
/* ---- MATCH-S08 preamble (same text in every re/match/s08 source; not part of any function body) ----
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
typedef struct { float x, y, z; } V3_00441820;
MGLOBALA(float, DAT_006146f0, 3, 0x006146f0);
MGLOBALA(float, DAT_006146fc, 3, 0x006146fc);
void FUN_00409790(void);
V3_00441820 *FUN_004098a0(void);
V3_00441820 *FUN_00426cc0(int p0);
V3_00441820 *FUN_00426d00(int p0, int p1);
int FUN_004c3df0(V3_00441820 *p0, V3_00441820 *p1, int p2, void *p3);
int FUN_004c4d20(void *p0, float *p1, float p2, int p3);
// MATCH: 0x00441820 FUN_00441820
void FUN_00441820(int param_1, V3_00441820 *param_2, float *param_3)
{
  V3_00441820 *tab;
  V3_00441820 *e;
  V3_00441820 d;
  V3_00441820 up;
  V3_00441820 b;
  V3_00441820 *pb;
  float m[16];

  up.x = 0.0f;
  up.y = -1.0f;
  up.z = 0.0f;
  *param_3 = 0.0f;
  FUN_00409790();
  tab = FUN_004098a0();
  if (tab != 0) {
    e = &tab[param_1];
    if (e->z != -1.0f) {
      *param_3 = e->z;
    }
    if (e->x != -1.0f) {
      FUN_004c4d20(m, DAT_006146f0, 90.0f - e->x, 0);
      FUN_004c3df0(param_2, &up, 1, m);
      FUN_004c4d20(m, DAT_006146fc, e->y + 180.0f, 0);
      FUN_004c3df0(param_2, param_2, 1, m);
      return;
    }
  }
  pb = FUN_00426cc0(param_1);
  param_2->x = pb->x;
  param_2->y = pb->y;
  param_2->z = pb->z;
  d = *FUN_00426d00(param_1, 0);
  pb = FUN_00426d00(param_1, 3);
  b.x = pb->x;
  b.y = pb->y;
  b.z = pb->z;
  d.x = d.x - b.x;
  d.y = d.y - b.y;
  d.z = d.z - b.z;
  FUN_004c4d20(m, &d.x, -25.0f, 0);
  FUN_004c3df0(param_2, param_2, 1, m);
}
