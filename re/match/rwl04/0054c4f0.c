// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL04 0x0054c4f0 h1: best 54.1% (regs 55.1%, ops 77.6%); structure only (888 B forward-difference patch evaluator). Ghidra's 60 float locals folded into one float L[0x8c] stack array (frame 0x234 matches). Orig also pushes ebx (more registers live) and keeps the 9 inner-loop floats on the FPU/regs differently. Class (b). */
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
void FUN_0054d4d0(float *p0, int p1);
void FUN_0054e520(float *p0, float *p1, float p2, float p3);
// MATCH: 0x0054c4f0 FUN_0054c4f0


void FUN_0054c4f0(float *param_1,int param_2,int param_3,int param_4)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  int iVar8;
  int iVar9;
  float L[0x8c];
  
  fVar1 = DAT_005cc320 / (float)param_4;
  FUN_0054d4d0((&L[0x4c]),param_3);
  FUN_0054e520(&L[0],(&L[0x4c]),fVar1,fVar1);
  iVar9 = param_4 + 1;
  for (;;) {
    L[71] = L[9];
    L[72] = L[10];
    iVar8 = param_4 + 1;
    fVar1 = L[0];
    fVar2 = L[4];
    fVar3 = L[8];
    fVar4 = L[1];
    fVar5 = L[5];
    fVar6 = L[2];
    fVar7 = L[6];
    for (;;) {
      *param_1 = fVar1;
      param_1[1] = fVar4;
      param_1[2] = fVar6;
      param_1 = (float *)((int)param_1 + param_2);
      iVar8 = iVar8 + -1;
      if (iVar8 == 0) break;
      fVar1 = fVar2 + fVar1;
      fVar4 = fVar5 + fVar4;
      fVar6 = fVar7 + fVar6;
      fVar2 = fVar3 + fVar2;
      fVar5 = L[71] + fVar5;
      fVar7 = L[72] + fVar7;
      fVar3 = fVar3 + L[12];
      L[71] = L[13] + L[71];
      L[72] = L[14] + L[72];
    }
    iVar9 = iVar9 + -1;
    if (iVar9 == 0) break;
    L[0] = L[16] + L[0];
    L[1] = L[17] + L[1];
    L[2] = L[18] + L[2];
    L[4] = L[20] + L[4];
    L[5] = L[21] + L[5];
    L[6] = L[22] + L[6];
    L[8] = L[24] + L[8];
    L[9] = L[25] + L[9];
    L[10] = L[26] + L[10];
    L[12] = L[28] + L[12];
    L[13] = L[29] + L[13];
    L[14] = L[30] + L[14];
    L[16] = L[32] + L[16];
    L[17] = L[33] + L[17];
    L[18] = L[34] + L[18];
    L[20] = L[36] + L[20];
    L[21] = L[37] + L[21];
    L[22] = L[38] + L[22];
    L[24] = L[40] + L[24];
    L[25] = L[41] + L[25];
    L[26] = L[42] + L[26];
    L[28] = L[44] + L[28];
    L[29] = L[45] + L[29];
    L[30] = L[46] + L[30];
    L[32] = L[48] + L[32];
    L[33] = L[49] + L[33];
    L[34] = L[50] + L[34];
    L[36] = L[52] + L[36];
    L[37] = L[53] + L[37];
    L[38] = L[54] + L[38];
    L[40] = L[56] + L[40];
    L[41] = L[57] + L[41];
    L[42] = L[58] + L[42];
    L[44] = L[60] + L[44];
    L[45] = L[61] + L[45];
    L[46] = L[62] + L[46];
  }
  return;
}
