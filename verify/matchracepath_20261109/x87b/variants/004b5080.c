// FLAGS: /O2 /GS /Ob1
/* MATCH-S18 0x004b5080 (h3): hand, best 85.4% (regs 85.4%). RwMatrix-shaped param_1 (right +0, up +0x10,
   at +0x20, pos +0x30), three-point triangle param_2. Tried: g2s float* form (52.8%), struct form, up-cross
   statement orders x pos-block positions (24 variants, best = up.y, up.x, up.z, pos), cross product as an inline
   helper (82%), local at pointer (85.4%, at.y/z then go through edi), /G5 (79.8%), permuter (86.5% only by moving
   up.y before the at normalize = wrong semantics, rejected). Residual (b): x87 operand order inside the up cross
   products (original loads at.y before right.z) and one fstp placement before the first call. */
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
typedef struct { float x, y, z; } V3_4b5080;
typedef struct { V3_4b5080 right; unsigned int flags; V3_4b5080 up; unsigned int pad1; V3_4b5080 at; unsigned int pad2; V3_4b5080 pos; unsigned int pad3; } M_4b5080;
float FUN_004c39b0(V3_4b5080 *out, const V3_4b5080 *in);
void FUN_004c45f0(M_4b5080 *m);
void FUN_004c51a0(M_4b5080 *m, const void *m2, int op);
// MATCH: 0x004b5080 FUN_004b5080
void FUN_004b5080(M_4b5080 *param_1, V3_4b5080 *param_2, const void *param_3)
{
  V3_4b5080 a;
  V3_4b5080 b;
  V3_4b5080 c;

  a.x = param_2[0].x - param_2[1].x;
  a.y = param_2[0].y - param_2[1].y;
  a.z = param_2[0].z - param_2[1].z;
  b.x = param_2[2].x - param_2[1].x;
  b.y = param_2[2].y - param_2[1].y;
  b.z = (float)(param_2[2].z - (double)(param_2[1].z));
  FUN_004c39b0(&param_1->right, &a);
  c.x = b.z * a.y - b.y * a.z;
  c.y = b.x * a.z - b.z * a.x;
  c.z = (float)(b.y * a.x - (double)(b.x * a.y));
  FUN_004c39b0(&param_1->at, &c);
  param_1->up.x = param_1->at.y * param_1->right.z - param_1->at.z * param_1->right.y;
  param_1->up.y = param_1->at.z * param_1->right.x - param_1->at.x * param_1->right.z;
  param_1->up.z = param_1->at.x * param_1->right.y - param_1->at.y * param_1->right.x;
  param_1->pos.x = 0.0f;
  param_1->pos.y = 0.0f;
  param_1->pos.z = 0.0f;
  FUN_004c45f0(param_1);
  if (param_3 != 0) {
    FUN_004c51a0(param_1, param_3, 2);
  }
}
