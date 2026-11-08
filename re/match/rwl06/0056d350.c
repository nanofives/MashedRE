// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL06 0x0056d350: hand-matched (h2). 55.9% regs 94.9% ops 94.9% under FLAGS /O2 /Ob1 /G5 (raw file was /O1 at 77.0% with a frame-pointer prologue that the original lacks). Instruction stream identical; only the eax/ecx assignment differs (orig: iVar1 in ecx, param_3 loaded into eax after it; ours the reverse). Dot terms written in original x87 order (+0x18,+0x14,+0x10). Tried operand swaps, shift spellings, unsigned, permuter 166 compiles. Class b. */
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
// MATCH: 0x0056d350 FUN_0056d350
void FUN_0056d350(int *param_1,float *param_2,float *param_3,int param_4,int param_5,int param_6)
{
  int iVar1;
  int iVar2;

  iVar1 = (param_6 + (param_5 + param_4 * 4) * 2) * 0x20;
  *param_2 = *(float *)(*param_1 + iVar1) * param_3[3];
  param_2[1] = *(float *)(*param_1 + 4 + iVar1) * param_3[3];
  param_2[2] = *(float *)(*param_1 + 8 + iVar1) * param_3[3];
  iVar2 = *param_1 + iVar1;
  param_2[4] = *(float *)(iVar2 + 0x18) * param_3[2] +
               *(float *)(iVar2 + 0x14) * param_3[1] + *(float *)(iVar2 + 0x10) * *param_3;
  iVar2 = *param_1 + iVar1;
  param_2[5] = *(float *)(iVar2 + 0x18) * param_3[6] +
               *(float *)(iVar2 + 0x14) * param_3[5] + *(float *)(iVar2 + 0x10) * param_3[4];
  iVar1 = iVar1 + *param_1;
  param_2[6] = *(float *)(iVar1 + 0x18) * param_3[10] +
               *(float *)(iVar1 + 0x14) * param_3[9] + *(float *)(iVar1 + 0x10) * param_3[8];
}
