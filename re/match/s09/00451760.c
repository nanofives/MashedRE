// FLAGS: /O2 /GS /Ob1
/* MATCH-S09 0x00451760 (h1): 100% (regs 100%). Raw converter 22.5%: typed the object as a struct (V3 copy at +0x38, 64-byte struct copy at +0x54 through the +0x94 pointer), callees at real arity (0x004b4790 2 args, 0x0052df20 3 args), callback context as int[2] local. Residual: none. */
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
typedef unsigned int uint;
typedef struct { float x, y, z; } V3_00451760;
typedef struct { undefined4 m[16]; } M16_00451760;
typedef struct {
  int clump;              /* 0x00 */
  int a4, a8, ac;         /* 0x04..0x0c */
  int f10;                /* 0x10 */
  char a14[0x24];         /* 0x14 */
  V3_00451760 v38;        /* 0x38 */
  int f44, f48, f4c, f50; /* 0x44..0x50 */
  M16_00451760 m54;       /* 0x54 */
  M16_00451760 *p94;      /* 0x94 */
  char a98[0x2c];         /* 0x98 */
  int fc4;                /* 0xc4 */
} S_00451760;
undefined4 FUN_00451690(undefined4 param_1,int param_2);
undefined4 FUN_004516d0(undefined4 param_1,int *param_2);
undefined4 FUN_00451730(undefined4 p0, int p1);
int FUN_004b4790(int p0, int p1);
int FUN_0052df20(int p0, void *p1, void *p2);
undefined4 FUN_0052e2c0(int p0, float p1);
undefined4 FUN_00533ec0(uint * p0);
int FUN_00550bc0(int p0);
int FUN_004e66d0(int p0, void *p1, void *p2);
// MATCH: 0x00451760 FUN_00451760
void FUN_00451760(S_00451760 *param_1,int param_2,int param_3)
{
  int iVar1;
  int local[2];

  param_1->clump = param_2;
  param_1->f4c = param_3;
  param_1->f44 = 0;
  param_1->f50 = FUN_004b4790(param_2,param_3);
  param_1->f48 = 0;
  FUN_0052df20(*(int *)(param_1->f50 + 0x20),(void *)FUN_00451690,param_1);
  local[0] = 0;
  local[1] = (int)param_1;
  FUN_004e66d0(param_1->clump,(void *)FUN_004516d0,local);
  param_1->f10 = *(int *)(param_1->a8 + 4);
  FUN_00451730((undefined4)&param_1->a14,param_1->a8);
  FUN_0052e2c0(*(int *)(param_1->f50 + 0x20),0);
  FUN_00533ec0((uint *)param_1->f50);
  iVar1 = FUN_00550bc0(param_1->f50);
  param_1->v38 = *(V3_00451760 *)(iVar1 + 0x70);
  param_1->p94 = (M16_00451760 *)(iVar1 + 0x40);
  param_1->m54 = *param_1->p94;
  param_1->fc4 = 0;
}
