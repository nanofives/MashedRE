// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 0x00442440: best 73.8% (regs 74.1%, ops ~92%), 122/122 ins, by hand + permuter (raw 13.1%).
   Shape: the source matrix (FUN_0046d4a0 of FUN_0040dc90()) struct-copied into +0x4c (rep movsd); offset
   a = right*k0 + up*k1 + at*k2 built with member-wise copies into locals a/b (the original keeps x,y on the FPU
   stack and the z copy in memory, which member-wise copies reproduce), a copy of up kept as the rotation axis,
   translate by a, then translate by -pos, rotate about up by +0xb0, translate back by pos (-1.0f is the literal
   at 0x005cc33c). Tried: struct copies, up copied before/after, static __inline scale/add helpers (35-51%).
   Residual (b): the original evaluates the first a+b sum later and pushes the translate args after the last
   fadd; 9176 here hoists the call-argument pushes into the sum and so shifts every later esp offset. */
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
typedef struct { float x, y, z; } V3_00442440;
typedef struct {
  V3_00442440 right; unsigned int flags;
  V3_00442440 up; unsigned int pad1;
  V3_00442440 at; unsigned int pad2;
  V3_00442440 pos; unsigned int pad3;
} Mtx_00442440;
typedef struct {
  char pad0[0x3c];
  float k0, k1, k2;
  int pad48;
  Mtx_00442440 mtx;
  char pad8c[0x24];
  float angle;
  char padb4[0xc];
  int c0;
  int c4;
  int c8;
} Obj_00442440;
int FUN_0040dc90(void);
int FUN_0046d4a0(Mtx_00442440 **p0, int p1);
int FUN_004c4d20(Mtx_00442440 *p0, V3_00442440 *p1, float p2, int p3);
void FUN_004c51a0(Mtx_00442440 *p0, V3_00442440 *p1, int p2);
// MATCH: 0x00442440 FUN_00442440
void FUN_00442440(Obj_00442440 *param_1)
{
  Mtx_00442440 *src;
  V3_00442440 a;
  V3_00442440 b;
  V3_00442440 up;
  Mtx_00442440 *m;

  FUN_0046d4a0(&src, FUN_0040dc90());
  m = &param_1->mtx;
  *m = *src;
  a.x = m->right.x;
  a.y = m->right.y;
  a.z = m->right.z;
  a.x = a.x * param_1->k0;
  a.y = a.y * param_1->k0;
  a.z = a.z * param_1->k0;
  up.x = m->up.x;
  up.y = m->up.y;
  up.z = m->up.z;
  b.x = m->up.x;
  b.y = m->up.y;
  b.z = m->up.z;
  b.x = b.x * param_1->k1;
  b.y = b.y * param_1->k1;
  b.z = b.z * param_1->k1;
  a.x = a.x + b.x;
  a.y = a.y + b.y;
  a.z = a.z + b.z;
  b.x = m->at.x;
  b.y = m->at.y;
  b.z = m->at.z;
  b.x = b.x * param_1->k2;
  b.y = b.y * param_1->k2;
  a.x = a.x + b.x;
  a.y = a.y + b.y;
  b.z = b.z * param_1->k2;
  a.z = a.z + b.z;
  FUN_004c51a0(m, &a, 2);
  param_1->c8 = 0;
  param_1->c0 = param_1->c4;
  a = m->pos;
  b.x = a.x * -1.0f;
  b.y = a.y * -1.0f;
  b.z = a.z * -1.0f;
  FUN_004c51a0(m, &b, 2);
  FUN_004c4d20(m, &up, param_1->angle, 2);
  FUN_004c51a0(m, &a, 2);
}
