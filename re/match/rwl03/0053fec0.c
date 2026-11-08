// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL03 0x0053fec0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl03_20261108/results.tsv. */
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
typedef unsigned short undefined2;
typedef unsigned int undefined4;
typedef unsigned int uint;
typedef unsigned char byte;
#define CONCAT31(a,b) ((uint)(((uint)(a)<<8)|(uchar)(b)))
MGLOBAL(int, DAT_007dc5dc, 0x007dc5dc);
MGLOBAL(undefined1, DAT_005cc320, 0x005cc320);
undefined4 FUN_004c5a60(int p0);
void FUN_004c5bc0(int p0, int p1);
int FUN_004c5c00(int p0, char * p1);
int FUN_004c5ca0(void);
int FUN_00543b30(int *p0, int *p1);
MGLOBAL(int, _DAT_005cc320, 0x005cc320);
// MATCH: 0x0053fec0 FUN_0053fec0
/* [C1 2026-05-20] Material/texture pair installer. Finds stage with `+0x14 == 1` and rebuilds its
   texture binding from
 */

int * FUN_0053fec0(int *param_1,int *param_2)

{
  char cVar1;
  int *piVar2;
  int uVar3;
  int *piVar4;
  int iVar5;
  char *pcVar6;
  byte bVar7;
  int iVar8;
  int iVar9;
  undefined4 *puVar10;
  int *local_28 [2];
  char local_20;
  undefined4 local_1f;
  
  bVar7 = 0;
  *(byte *)&local_28[0] = 0;
  do {
    if (*(int *)(*(int *)(DAT_007dc5dc + (int)param_1) + 0x14 + ((uint)local_28[0] & 0xff) * 0x18)
        == 1) {
      iVar8 = *(int *)(DAT_007dc5dc + (int)param_1) + ((uint)local_28[0] & 0xff) * 0x18;
      goto LAB_0053ff05;
    }
    bVar7 = bVar7 + 1;
    *(byte *)&local_28[0] = bVar7;
  } while (bVar7 < 2);
  iVar8 = 0;
LAB_0053ff05:
  if (*(int *)(iVar8 + 8) != 0) {
    FUN_004c5a60(*(int *)(iVar8 + 8));
    *(undefined4 *)(iVar8 + 8) = 0;
  }
  if (*(int *)(iVar8 + 4) != 0) {
    FUN_004c5a60(*(int *)(iVar8 + 4));
    *(undefined4 *)(iVar8 + 4) = 0;
    *(undefined4 *)(iVar8 + 0x10) = 0;
  }
  if (param_2 != (int *)0x0) {
    *(int **)(iVar8 + 8) = param_2;
    param_2[0x15] = param_2[0x15] + 1;
    piVar4 = (int *)*param_1;
    if (*(int *)(*param_2 + 0xc) == 0) {
      return param_1;
    }
    if ((piVar4 != (int *)0x0) && (*(int *)(*piVar4 + 0xc) == 0)) {
      return param_1;
    }
    local_20 = '\0';
    puVar10 = &local_1f;
    for (iVar5 = 7; iVar5 != 0; iVar5 = iVar5 + -1) {
      *puVar10 = 0;
      puVar10 = puVar10 + 1;
    }
    *(undefined2 *)puVar10 = 0;
    *(undefined1 *)((int)puVar10 + 2) = 0;
    iVar5 = 0;
    pcVar6 = &local_20;
    local_28[1] = param_2 + 4;
    if (piVar4 == (int *)0x0) {
      local_28[0] = param_2 + 4;
    }
    else {
      local_28[0] = piVar4 + 4;
    }
    do {
      if (((char)*local_28[0] == '\0') && ((char)*local_28[1] == '\0')) break;
      iVar9 = 0;
      do {
        piVar2 = local_28[iVar9];
        cVar1 = (char)*piVar2;
        if (cVar1 != '\0') {
          *pcVar6 = cVar1;
          pcVar6 = pcVar6 + 1;
          iVar5 = iVar5 + 1;
          local_28[iVar9] = (int *)((int)piVar2 + 1);
        }
        iVar9 = iVar9 + 1;
      } while (iVar9 < 2);
    } while (iVar5 < 0x1e);
    *pcVar6 = '\0';
    iVar5 = FUN_004c5ca0();
    *(undefined4 *)(iVar8 + 4) = 0;
    if (iVar5 != 0) {
      uVar3 = FUN_004c5c00(iVar5,&local_20);
      *(undefined4 *)(iVar8 + 4) = uVar3;
    }
    if (*(int *)(iVar8 + 4) == 0) {
      iVar9 = FUN_00543b30(piVar4,param_2);
      *(int *)(iVar8 + 4) = iVar9;
      if (iVar9 == 0) {
        return (int *)0x0;
      }
      if (iVar5 != 0) {
        FUN_004c5bc0(iVar5,iVar9);
        piVar4 = *(int **)(iVar8 + 4);
        goto LAB_0054005c;
      }
    }
    else {
      piVar4 = (int *)(*(int *)(iVar8 + 4) + 0x54);
      *piVar4 = *piVar4 + 1;
    }
    piVar4 = *(int **)(iVar8 + 4);
  }
  else {
    piVar4 = (int *)*param_1;
  }
LAB_0054005c:
  *(float *)(iVar8 + 0x10) = _DAT_005cc320 / (float)*(int *)(*piVar4 + 0xc);
  return param_1;
}
