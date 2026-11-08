// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL06 h1 0x00578f20: 64.2% (regs 71.6%), class b. loop counter kept in memory, register pressure differs. Tried: uint compare, sum order, double types, permuter. cxx:ok. */
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
MGLOBAL(float, DAT_005e5050, 0x005e5050);
float *FUN_0055c000(int p0, int p1, float *p2, void *p3);
// MATCH: 0x00578f20 FUN_00578f20
float *FUN_00578f20(int *param_1,float *param_2,float *param_3)
{
  float fVar1;
  float fVar2;
  float *pfVar3;
  int iVar4;
  float local_14;
  uint local_10;
  undefined1 local_c [12];

  iVar4 = 0;
  local_14 = -3.4028235e+38f;
  local_10 = 0;
  if ((uint)param_1[1] > 0) {
    do {
      pfVar3 = FUN_0055c000(*param_1 + iVar4,*param_1 + iVar4,param_2,local_c);
      if (pfVar3 != (float *)0) {
        fVar1 = *(float *)(*param_1 + 0x4c + iVar4);
        fVar2 = pfVar3[1] * param_2[1] + pfVar3[0] * param_2[0] + pfVar3[2] * param_2[2] + fVar1;
        if (fVar2 > local_14) {
          local_14 = fVar2;
          *param_3 = fVar1 * *param_2 + *pfVar3;
          param_3[1] = fVar1 * param_2[1] + pfVar3[1];
          param_3[2] = fVar1 * param_2[2] + pfVar3[2];
        }
      }
      local_10 = local_10 + 1;
      iVar4 = iVar4 + 0x60;
    } while (local_10 < (uint)param_1[1]);
    if (local_14 > DAT_005e5050) {
      return param_3;
    }
  }
  return (float *)0;
}
