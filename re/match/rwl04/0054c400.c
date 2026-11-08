// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL04 0x0054c400 (h2): 87.5% (regs 88.8, ops 95). Float global (fdivr), raw loop structure kept (iVar14 = n - i countdown, iVar13 count, j), >= 0 tests. Residue: orig hoists the inner count differently (mov esi,ebp; lea edx,[esi+1] in-block, dec esi at tail) vs ours lea edx,[ebp+1]/lea esi,[edx-1] up front. permuter 336 compiles no real gain. class (b). */
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
MGLOBAL(float, DAT_005cc320, 0x005cc320);
// MATCH: 0x0054c400 FUN_0054c400
float * FUN_0054c400(float *param_1,int param_2,float *param_3,int param_4)
{
  float fVar1, fVar2, fVar3, fVar4, fVar5, fVar6, fVar7, fVar8, fVar9, fVar10;
  int iVar11;
  float *pfVar12;
  int iVar13;
  int iVar14;
  int j;
  int i;

  fVar8 = DAT_005cc320 / (float)param_4;
  fVar1 = *param_3;
  fVar2 = param_3[1];
  fVar3 = param_3[2];
  fVar4 = param_3[3];
  fVar5 = param_3[4];
  fVar6 = param_3[5];
  i = 0;
  pfVar12 = param_1;
  iVar14 = param_4;
  if (param_4 >= 0) {
    do {
      iVar11 = i;
      fVar7 = (float)i * fVar8;
      if (iVar14 >= 0) {
        iVar13 = iVar14 + 1;
        j = iVar14;
        do {
          fVar10 = (float)j * fVar8;
          fVar9 = DAT_005cc320 - (fVar7 + fVar10);
          *pfVar12 = fVar10 * fVar1 + fVar5 * fVar9 + fVar7 * fVar3;
          pfVar12[1] = fVar10 * fVar2 + fVar9 * fVar6 + fVar7 * fVar4;
          pfVar12 = (float *)((int)pfVar12 + param_2);
          j = j + -1;
          iVar13 = iVar13 + -1;
        } while (iVar13 != 0);
      }
      i = iVar11 + 1;
      iVar14 = iVar14 + -1;
    } while (i <= param_4);
  }
  return param_1;
}
