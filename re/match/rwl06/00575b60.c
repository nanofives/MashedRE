// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL06 0x00575b60: hand-matched (h2). 79.8% regs 79.8% ops 90.4%. Ghidra's float param_1 / byte** args were stack slots: rewritten as int param_1 and two float locals (lo, hi) whose addresses go to 0x0055c2d0. Residual: in dot products 2 and 3 the original sums (z + x) + y, ours z + y + x for every source order of the three terms (6 permutations tried). x87 add-chain order, class a. */
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
typedef unsigned char byte;
void FUN_0055c2d0(int p0, float * p1, float * p2, float * p3, float * p4);
// MATCH: 0x00575b60 FUN_00575b60
void FUN_00575b60(int param_1,int param_2,float *param_3,byte *param_4)
{
  short sVar1;
  int iVar2;
  float *pfVar3;
  int iVar4;
  float fVar5;
  float lo;
  float hi;

  iVar2 = *(int *)(param_1 + 8);
  sVar1 = *(short *)(iVar2 + 0x48);
  *(short *)param_4 = sVar1;
  if (sVar1 != 0xe) {
    pfVar3 = *(float **)(param_1 + 0x10);
    *param_3 = *pfVar3 * *(float *)(iVar2 + 0x30) +
               pfVar3[4] * *(float *)(iVar2 + 0x34) + pfVar3[8] * *(float *)(iVar2 + 0x38);
    iVar4 = *(int *)(param_1 + 0x10);
    param_3[1] = *(float *)(iVar4 + 0x24) * *(float *)(iVar2 + 0x38) + *(float *)(iVar4 + 4) * *(float *)(iVar2 + 0x30) + *(float *)(iVar4 + 0x14) * *(float *)(iVar2 + 0x34);
    iVar4 = *(int *)(param_1 + 0x10);
    param_3[2] = *(float *)(iVar4 + 0x28) * *(float *)(iVar2 + 0x38) + *(float *)(iVar4 + 8) * *(float *)(iVar2 + 0x30) + *(float *)(iVar4 + 0x18) * *(float *)(iVar2 + 0x34);
    if ((*param_4 & 1) == 0) {
      FUN_0055c2d0(*(int *)(param_2 + 8),*(float **)(param_2 + 0x10),param_3,&lo,
                   &hi);
      iVar4 = *(int *)(param_1 + 0x10);
      fVar5 = *(float *)(iVar4 + 0x38) * param_3[2] +
              *(float *)(iVar4 + 0x30) * *param_3 + *(float *)(iVar4 + 0x34) * param_3[1] +
              *(float *)(iVar2 + 0x18) * *(float *)(iVar2 + 0x38) +
              *(float *)(iVar2 + 0x14) * *(float *)(iVar2 + 0x34) +
              *(float *)(iVar2 + 0x10) * *(float *)(iVar2 + 0x30);
      if (hi - fVar5 < fVar5 - lo) {
        *param_3 = -*param_3;
        param_3[1] = -param_3[1];
        param_3[2] = -param_3[2];
      }
    }
  }
}
