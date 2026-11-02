// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 0x00441b30: best 86.7% (regs 86.7%, ops 100%), 90/90 ins, by hand (raw 46.7%). Every read of
   *param_2 goes to a local first (all reads precede all stores in the original), pos copied member-wise, the
   three float->int conversions are _ftol (FUN_004a2c48), the matrix at +0xc set by the RwMatrixSetIdentity
   chained-assignment form (rightmost stored first), m0 = 1 stored before m1/m2 (permuter). Residual (b): stack
   slots of the float locals. Original: f10 +0, f11 +4, f3 +8, f4 +0xc, f5 +0x10, pos +0x14, f6 in param_2's
   incoming slot; 9176 here gives f3 the param slot and f4..f11 ascending. Declaration order does not move it;
   read order of the locals made it worse (6 orders tried). */
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
typedef struct { float x, y, z; } V3_00441b30;
typedef struct {
  V3_00441b30 pos;
  float f3, f4, f5, f6;
  float f7, f8, f9;
  float f10, f11;
} Src_00441b30;
typedef struct {
  V3_00441b30 right; unsigned int flags;
  V3_00441b30 up; unsigned int pad1;
  V3_00441b30 at; unsigned int pad2;
  V3_00441b30 pos; unsigned int pad3;
} Mtx_00441b30;
typedef struct {
  int m0, m1, m2;
  Mtx_00441b30 mtx;
  int pad13[0x10];
  V3_00441b30 v23;
  V3_00441b30 v26;
  float f29;
  int pad2a;
  float f2b, f2c, f2d;
  float f2e, f2f, f30;
  float f31;
  int pad32;
  int m33;
  float f34, f35;
} Cam_00441b30;
void FUN_004c51a0(Mtx_00441b30 *p0, V3_00441b30 *p1, int p2);
// MATCH: 0x00441b30 FUN_00441b30
void FUN_00441b30(Cam_00441b30 *param_1, Src_00441b30 *param_2)
{
  V3_00441b30 pos;
  float f3, f4, f5, f6, f10, f11;
  int i7, i8, i9;

  pos.x = param_2->pos.x;
  pos.y = param_2->pos.y;
  pos.z = param_2->pos.z;
  f3 = param_2->f3;
  f4 = param_2->f4;
  f5 = param_2->f5;
  f6 = param_2->f6;
  i7 = (int)param_2->f7;
  i8 = (int)param_2->f8;
  i9 = (int)param_2->f9;
  f10 = param_2->f10;
  f11 = param_2->f11;
  param_1->m0 = 1;
  param_1->m1 = i7;
  param_1->m2 = i8;
  param_1->mtx.right.x = param_1->mtx.up.y = param_1->mtx.at.z = 1.0f;
  param_1->mtx.right.y = param_1->mtx.right.z = param_1->mtx.up.x = 0.0f;
  param_1->mtx.up.z = param_1->mtx.at.x = param_1->mtx.at.y = 0.0f;
  param_1->mtx.pos.x = param_1->mtx.pos.y = param_1->mtx.pos.z = 0.0f;
  param_1->mtx.flags = param_1->mtx.flags | 0x20003;
  FUN_004c51a0(&param_1->mtx, &pos, 0);
  param_1->f2b = f3;
  param_1->f2c = f4;
  param_1->v23.x = param_1->v23.y = param_1->v23.z = 0.0f;
  param_1->v26.x = param_1->v26.y = param_1->v26.z = 0.0f;
  param_1->f29 = 12.0f;
  param_1->f2d = f5;
  param_1->f2e = param_1->f2f = 0.0f;
  param_1->f30 = 1.0f;
  param_1->f31 = f6;
  param_1->m33 = i9;
  param_1->f34 = f10;
  param_1->f35 = f11;
}
