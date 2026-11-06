// FLAGS: /O2 /GS /Ob1
/* MATCH-S09 0x00451d80 (h1): whole obj symbol BYTE-IDENTICAL (944 B incl. the 6-entry jump table at 0x00452118; match.py prints 7.6% only because pc_funcs.csv has 61 B, the code part scored with size 0x398 is 100% regs 100%). Raw converter 3.4%. What moved it: struct-typed object, param_2 unsigned (fild + 2^32 fix-up), literal floats 0.000333f/0.000666f/0.0f/2.0f (the .rdata values), case 2 stores f44 before fc8, case 5 copies the atomic bounding sphere then the 64-byte matrix then the centre per field, 0x004671a0 takes 1 arg and 0x004c1b40 2 (deferred add esp). Callees 0x00451820/0x00451ad0 are called with (p, dt). Residual: none. */
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
typedef struct { float x, y, z; } V3_00451d80;
typedef struct { V3_00451d80 center; float radius; } SPH_00451d80;
typedef struct { float m[16]; } M16_00451d80;
typedef struct { char a0[0x1c]; SPH_00451d80 sph; char a2c[0x20]; uint flags4c; } AT_00451d80;
typedef struct {
  int clump;              /* 0x00 */
  int a4;                 /* 0x04 */
  AT_00451d80 *a8;        /* 0x08 */
  char ac[0x2c];
  V3_00451d80 v38;        /* 0x38 */
  float f44;              /* 0x44 */
  int f48;                /* 0x48 */
  int f4c;                /* 0x4c */
  int f50;                /* 0x50 */
  M16_00451d80 m54;       /* 0x54 */
  M16_00451d80 *p94;      /* 0x94 */
  char a98[0x0c];
  float fa4;              /* 0xa4 */
  char aa8[0x20];
  float fc8;              /* 0xc8 */
} S_00451d80;
MGLOBAL(float, DAT_007f0fb4, 0x007f0fb4);
MGLOBAL(int, DAT_0088ff50, 0x0088ff50);
MGLOBAL(int, DAT_0088ff54, 0x0088ff54);
MGLOBAL(int, DAT_0088ff58, 0x0088ff58);
MGLOBAL(int, DAT_0088ff5c, 0x0088ff5c);
void FUN_00451820(S_00451d80 *p0, uint p1);
void FUN_00451ad0(S_00451d80 *p0, uint p1);
void FUN_00451cc0(S_00451d80 *p0);
void FUN_00451ce0(S_00451d80 *p0);
int FUN_004671a0(int p0);
undefined4 FUN_004b47c0(int p0, float p1);
int FUN_004c1b40(int p0, SPH_00451d80 *p1);
undefined4 FUN_004e5fc0(AT_00451d80 *p0);
undefined4 FUN_0052df70(int p0, float p1);
undefined4 FUN_0052e2c0(int p0, float p1);
undefined4 FUN_00533ec0(int p0);
// MATCH: 0x00451d80 FUN_00451d80
void FUN_00451d80(S_00451d80 *param_1,uint param_2)
{
  float f;
  SPH_00451d80 sph;

  if (param_1->f4c != 0) {
    switch (param_1->f48) {
    case 0:
      FUN_0052e2c0(*(int *)(param_1->f50 + 0x20),0);
      param_1->fc8 = 0;
      FUN_00533ec0(param_1->f50);
      FUN_00451ce0(param_1);
      param_1->m54 = *param_1->p94;
      FUN_00451820(param_1,param_2);
      param_1->fa4 = 1.0f;
      FUN_00451ad0(param_1,param_2);
      break;
    case 1:
      param_1->fa4 = param_1->fa4 - (float)param_2 * 0.000333f;
      if (param_1->fa4 < 0.0f) {
        param_1->f48 = 2;
      }
      param_1->m54 = *param_1->p94;
      param_1->f44 = 0;
      FUN_0052e2c0(*(int *)(param_1->f50 + 0x20),0);
      param_1->fc8 = 0;
      FUN_00533ec0(param_1->f50);
      FUN_00451ce0(param_1);
      FUN_00451ad0(param_1,param_2);
      break;
    case 2:
      param_1->m54 = *param_1->p94;
      f = (float)param_2 * 0.000666f;
      param_1->f44 = f;
      param_1->fc8 = f + param_1->fc8;
      FUN_004b47c0(param_1->f50,f);
      FUN_00451ce0(param_1);
      FUN_00451ad0(param_1,param_2);
      if (DAT_0088ff50 == 0 && DAT_0088ff54 == 0 && DAT_0088ff58 == 0 && DAT_0088ff5c == 0 &&
          param_1->fc8 > 2.0f) {
        param_1->f48 = 3;
      }
      break;
    case 3:
      param_1->m54 = *param_1->p94;
      f = (float)param_2 * 0.000666f;
      param_1->f44 = f;
      param_1->fc8 = param_1->fc8 - f;
      FUN_0052df70(*(int *)(param_1->f50 + 0x20),f);
      if (param_1->fc8 <= 0.0f) {
        FUN_00451cc0(param_1);
        param_1->fc8 = 0;
        FUN_0052e2c0(*(int *)(param_1->f50 + 0x20),0);
      }
      FUN_00533ec0(param_1->f50);
      FUN_00451ce0(param_1);
      FUN_00451ad0(param_1,param_2);
      break;
    case 4:
      param_1->m54 = *param_1->p94;
      f = (float)param_2 * 0.0f;
      param_1->f44 = f;
      FUN_004b47c0(param_1->f50,f);
      FUN_00451ce0(param_1);
      FUN_00451ad0(param_1,param_2);
      break;
    case 5:
      if (param_1->a8->flags4c & 2) {
        FUN_004e5fc0(param_1->a8);
      }
      sph = param_1->a8->sph;
      param_1->m54 = *param_1->p94;
      sph.center.x = param_1->v38.x;
      sph.center.y = param_1->v38.y;
      sph.center.z = param_1->v38.z;
      if (FUN_004c1b40(FUN_004671a0(0),&sph) == 0) {
        FUN_00451cc0(param_1);
        param_1->fc8 = 0;
      }
      FUN_00451ad0(param_1,param_2);
      break;
    }
  }
  DAT_007f0fb4 = param_1->fc8;
}
