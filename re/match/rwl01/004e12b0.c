// FLAGS: /O2 /Ob1 /G5
/* h3 hand-match: best 67.7% (regs 84.3%), class b. Freelist-block coalesce (header at param_2-0x20: +0 prev, +4 next, +8 size, +0xc link). Written with everything relative to piVar1, int-valued flags (mov r,1 / xor r,r like the original). Residual: original loads the param before the pushes into eax and keeps iVar2 in eax, we keep it in ecx / use ebx+ebp; flags in the file were /GS before, now plain G5 (no change in score class). Permuter 74% variant corrupts an address expression (*(int*)4): rejected. */
/* MATCH-RWL01 0x004e12b0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl01_20261107/results.tsv. */
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
typedef unsigned int undefined4;
#ifndef __cplusplus
#define true 1
#endif
#ifndef __cplusplus
#define false 0
#endif
int FUN_004e13f0(int p0);
// MATCH: 0x004e12b0 FUN_004e12b0
void FUN_004e12b0(int param_1,int param_2)

{
  int *piVar1;
  int iVar2;
  int iVar3;
  int *piVar4;
  int bVar5;
  int bVar6;
  undefined4 *puVar7;
  
  piVar1 = (int *)(param_2 + -0x20);
  iVar2 = *(int *)(param_2 + -0x20);
  if ((iVar2 == 0) || (*(int *)(iVar2 + 0xc) == 0)) {
    bVar5 = 0;
  }
  else {
    bVar5 = 1;
  }
  iVar3 = piVar1[1];
  if ((iVar3 == 0) || (*(int *)(iVar3 + 0xc) == 0)) {
    bVar6 = 0;
  }
  else {
    bVar6 = 1;
  }
  if (bVar5) {
    if (bVar6) {
      puVar7 = *(undefined4 **)(iVar3 + 0xc);
      iVar2 = *(int *)(param_1 + 0x14);
      iVar3 = *(int *)(param_1 + 0xc);
      if ((undefined4 *)(iVar3 + -8 + iVar2 * 8) != puVar7) {
        *puVar7 = *(undefined4 *)(iVar3 + -8 + iVar2 * 8);
        iVar2 = *(int *)(iVar3 + -4 + iVar2 * 8);
        puVar7[1] = iVar2;
        *(undefined4 **)(iVar2 + 0xc) = puVar7;
      }
      *(int *)(param_1 + 0x14) = *(int *)(param_1 + 0x14) + -1;
      *(int *)(*piVar1 + 8) =
           *(int *)(*piVar1 + 8) +
           piVar1[2] + 0x40 + *(int *)(piVar1[1] + 8);
      **(undefined4 **)(*piVar1 + 0xc) = *(undefined4 *)(*piVar1 + 8);
      *(undefined4 *)(*piVar1 + 4) = *(undefined4 *)(piVar1[1] + 4);
      piVar4 = *(int **)(piVar1[1] + 4);
      if (piVar4 != (int *)0x0) {
        *piVar4 = *piVar1;
        return;
      }
    }
    else {
      *(int *)(iVar2 + 8) = *(int *)(iVar2 + 8) + piVar1[2] + 0x20;
      **(undefined4 **)(*piVar1 + 0xc) = *(undefined4 *)(*piVar1 + 8);
      *(undefined4 *)(*piVar1 + 4) = *(undefined4 *)&piVar1[1];
      if (*(int **)&piVar1[1] != (int *)0x0) {
        **(int **)&piVar1[1] = *piVar1;
        return;
      }
    }
  }
  else if (bVar6) {
    piVar1[2] = piVar1[2] + *(int *)(iVar3 + 8) + 0x20;
    *(undefined4 *)&piVar1[3] = *(undefined4 *)(piVar1[1] + 0xc);
    *(int **)(*(int *)(piVar1[1] + 0xc) + 4) = piVar1;
    **(undefined4 **)(piVar1[1] + 0xc) = *(undefined4 *)&piVar1[2];
    piVar4 = *(int **)(piVar1[1] + 4);
    *(int **)&piVar1[1] = piVar4;
    if (piVar4 != (int *)0x0) {
      *piVar4 = (int)piVar1;
      return;
    }
  }
  else {
    puVar7 = (undefined4 *)FUN_004e13f0(param_1);
    if (puVar7 != (undefined4 *)0x0) {
      puVar7[1] = (undefined4)piVar1;
      *puVar7 = *(undefined4 *)&piVar1[2];
      *(undefined4 **)&piVar1[3] = puVar7;
    }
  }
  return;
}
