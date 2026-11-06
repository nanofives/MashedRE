// FLAGS: /O2 /GS /Ob1
/* MATCH-S09 0x00451ce0 (h1): 100% (regs 100%). Raw converter 44.7%: typed structs (frame +0x40 matrix, V3 copy), and the scale is the literal 360.0f (the .rdata float at 0x005ccac4): through a MGLOBAL the compiler loads the global first (81.6%). Residual: none. */
/* ---- MATCH-S09 preamble (same text in every re/match/s09 source; not part of any function body) ----
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
typedef struct { float x, y, z; } V3_00451ce0;
typedef struct { float r[12]; V3_00451ce0 pos; float pw; } M16_00451ce0;
typedef struct { char a0[0x40]; M16_00451ce0 ltm; } F_00451ce0;
typedef struct {
  char a0[0x38];
  V3_00451ce0 v38;        /* 0x38 */
  char a44[0x0c];
  int f50;                /* 0x50 */
  char a54[0x30];
  V3_00451ce0 v84;        /* 0x84 */
  int f90;
  M16_00451ce0 *p94;      /* 0x94 */
  char a98[0x10];
  V3_00451ce0 va8;        /* 0xa8 */
} S_00451ce0;
F_00451ce0 *FUN_00550bc0(int p0);
// MATCH: 0x00451ce0 FUN_00451ce0
void FUN_00451ce0(S_00451ce0 *param_1)
{
  F_00451ce0 *f;

  f = FUN_00550bc0(param_1->f50);
  param_1->v38 = f->ltm.pos;
  param_1->p94 = &f->ltm;
  param_1->va8.x = param_1->p94->pos.x - param_1->v84.x;
  param_1->va8.y = param_1->p94->pos.y - param_1->v84.y;
  param_1->va8.z = param_1->p94->pos.z - param_1->v84.z;
  param_1->va8.x = param_1->va8.x * 360.0f;
  param_1->va8.y = param_1->va8.y * 360.0f;
  param_1->va8.z = param_1->va8.z * 360.0f;
}
