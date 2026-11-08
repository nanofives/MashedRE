// FLAGS: /O2 /Ob1 /G5
/* RESULT: pending */
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
typedef struct V3_ { int x, y, z; } V3_;
MGLOBAL(float, DAT_005d757c, 0x005d757c);
MGLOBAL(float, DAT_005cc574, 0x005cc574);
double FUN_005667c0(float *param_1, float *param_2);
uint FUN_00566d50(float *p0, float *p1, float *p2, float *p3, float p4);
uint FUN_00566ea0(float *p0, float *p1, float *p2, float *p3, float *p4, float p5, int p6, int p7);
// MATCH: 0x0057bf30 FUN_0057bf30
double FUN_0057bf30(float *param_1,float *param_2,float *param_3,float *param_4,float *param_5,
                    float param_6)
{
  float fVar1;
  float fVar2;
  int iVar4;
  double fVar6;
  double fVar7;
  float f[11];

  f[2] = *param_5 - *param_4;
  f[3] = param_5[1] - param_4[1];
  f[4] = param_5[2] - param_4[2];
  fVar2 = f[4] * f[4] + (f[3] * f[3] + f[2] * f[2]);
  FUN_005667c0(f + 2,f + 2);
  iVar4 = FUN_00566ea0(f,param_2,param_3,param_4,f + 2,param_6,0,0);
  if (0 < iVar4) {
    fVar6 = f[0] / f[1];
    f[7] = (float)(fVar6 * param_3[2] + param_2[2]);
    f[8] = (float)((fVar6 * param_3[0] + param_2[0]) - param_4[0]);
    fVar7 = (f[7] - param_4[2]) * f[4] + (f[8] * f[2] + ((fVar6 * param_3[1] + param_2[1]) - param_4[1]) * f[3]);
    fVar1 = (float)fVar7;
    if (fVar7 < DAT_005d757c) {
      *(V3_ *)param_1 = *(V3_ *)param_4;
    }
    else if (fVar1 * fVar1 > fVar2) {
      *(V3_ *)param_1 = *(V3_ *)param_5;
    }
    else {
      *param_1 = fVar1 * f[2] + *param_4;
      param_1[1] = fVar1 * f[3] + param_4[1];
      param_1[2] = fVar1 * f[4] + param_4[2];
      return fVar6;
    }
    iVar4 = FUN_00566d50(f,param_2,param_3,param_1,param_6);
    if (0 < iVar4) {
      return f[0] / f[1];
    }
  }
  return DAT_005cc574;
}
