// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL04 0x00546890: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl04_20261107/results.tsv. */
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
typedef unsigned char undefined1;
typedef unsigned int undefined4;
typedef unsigned int uint;
typedef int *(*PFN_al)(int,int);
typedef void (*PFN_fr)(void *);
void *__cdecl memset(void *, int, unsigned int);
#pragma intrinsic(memset)
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
undefined4 FUN_004d7ff0(undefined4 p0, ...);
undefined4 * FUN_004d8480(undefined4 * p0);
// MATCH: 0x00546890 FUN_00546890
float * FUN_00546890(uint param_1)
{
  int *piVar1;
  int iVar2;
  float fVar3;
  uint uVar4;
  uint uVar5;
  float *pfVar6;
  int *piVar7;
  int iVar8;
  int *piVar9;
  int iVar10;
  float *pfVar11;
  float *pfVar12;
  uint uStack_14;
  uint nn;
  undefined4 e[2];

  uVar4 = param_1;
  uVar5 = param_1 * param_1;
  nn = uVar5;
  pfVar6 = (float *)(**(PFN_al *)(DAT_007d3ff8 + 0x108))(uVar5 * 4,0x30102);
  if (pfVar6 == (float *)0x0) {
    e[0] = 0x102;
    e[1] = FUN_004d7ff0(0x80000013,uVar5 * 4);
    FUN_004d8480(e);
    return (float *)0x0;
  }
  memset(pfVar6,0,uVar5 * 4);
  iVar8 = (param_1 - 1) * 4;
  piVar7 = (int *)(**(PFN_al *)(DAT_007d3ff8 + 0x108))(iVar8,0x30102);
  if (piVar7 == (int *)0x0) {
LAB_fail:
    e[0] = 0x102;
    e[1] = FUN_004d7ff0(0x80000013,iVar8);
    FUN_004d8480(e);
    (**(PFN_fr *)(DAT_007d3ff8 + 0x10c))(pfVar6);
    return (float *)0x0;
  }
  *piVar7 = 1;
  piVar7[1] = 4;
  if (2 < (int)(param_1 - 1)) {
    iVar8 = param_1 - 3;
    piVar9 = piVar7;
    do {
      piVar9[2] = piVar9[1] * 4 - *piVar9;
      piVar9 = piVar9 + 1;
      iVar8 = iVar8 + -1;
    } while (iVar8 != 0);
  }
  iVar2 = piVar7[param_1 - 2];
  (**(PFN_fr *)(DAT_007d3ff8 + 0x10c))(piVar7);
  iVar8 = param_1 * 4;
  piVar7 = (int *)(**(PFN_al *)(DAT_007d3ff8 + 0x108))(iVar8,0x30102);
  if (piVar7 == (int *)0x0) goto LAB_fail;
  *piVar7 = 1;
  piVar7[1] = 2;
  uVar5 = param_1;
  if (2 < (int)param_1) {
    iVar8 = param_1 - 2;
    piVar9 = piVar7;
    do {
      iVar8 = iVar8 + -1;
      piVar9[2] = piVar9[1] * 4 - *piVar9;
      piVar9 = piVar9 + 1;
    } while (iVar8 != 0);
  }
  while (uVar5 = uVar5 - 2, -1 < (int)uVar5) {
    piVar7[uVar5] = -piVar7[uVar5];
  }
  iVar8 = 0;
  if (0 < (int)param_1) {
    piVar9 = piVar7 + (param_1 - 1);
    pfVar12 = pfVar6;
    do {
      iVar10 = 1;
      *pfVar12 = (float)*piVar9 / (float)iVar2;
      pfVar11 = pfVar12;
      if (0 < iVar8) {
        do {
          piVar1 = piVar7 + iVar10;
          iVar10 = iVar10 + 1;
          pfVar11[1] = (float)(int)(*piVar1 * *piVar9 * ((-(uint)((param_1 & 1) != 0) & 4) - 2))
                       / (float)iVar2;
          pfVar11 = pfVar11 + 1;
        } while (iVar10 <= iVar8);
      }
      iVar8 = iVar8 + 1;
      piVar9 = piVar9 + -1;
      pfVar12 = pfVar12 + param_1;
    } while (iVar8 < (int)param_1);
  }
  iVar8 = 0;
  pfVar6[nn - 1] = *pfVar6;
  if (0 < (int)param_1) {
    param_1 = 0;
    uStack_14 = nn;
    do {
      iVar2 = uVar4 - 1;
      if (iVar8 < iVar2) {
        iVar10 = iVar2 - iVar8;
        pfVar11 = pfVar6 + param_1 + iVar2;
        pfVar12 = pfVar6 + (uStack_14 - iVar2) + -1;
        do {
          fVar3 = *pfVar12;
          pfVar12 = pfVar12 + 1;
          *pfVar11 = fVar3;
          pfVar11 = pfVar11 + -1;
          iVar10 = iVar10 + -1;
        } while (iVar10 != 0);
      }
      iVar8 = iVar8 + 1;
      param_1 = param_1 + uVar4;
      uStack_14 = uStack_14 + -uVar4;
    } while (iVar8 < (int)uVar4);
  }
  (**(PFN_fr *)(DAT_007d3ff8 + 0x10c))(piVar7);
  return pfVar6;
}
