// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL05 0x0055ae70: hand-matched (h1). 100%. Callee ushort args (no CONCAT22), shift count as (unsigned)ushort & 0x1f. */
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
int FUN_00564c80(int p0, unsigned short p1, float * p2);
void FUN_00565200(int p0, unsigned short p1, float * p2);
void FUN_00565260(int p0, unsigned short p1);
void FUN_00565570(int p0, unsigned short p1, int p2);
// MATCH: 0x0055ae70 FUN_0055ae70
int * FUN_0055ae70(int *param_1,int param_2,int param_3)
{
  unsigned int *puVar1;
  unsigned short uVar2;
  float *puVar3;
  float local_20[8];

  FUN_00565570(*param_1,*(unsigned short *)(param_2 + 0x20),param_3);
  uVar2 = *(unsigned short *)(param_2 + 0x20);
  puVar3 = 0;
  if ((*(unsigned int *)(param_1[0x1a] + (unsigned int)(uVar2 >> 5) * 4) & 1 << ((unsigned int)uVar2 & 0x1f)) != 0) {
    FUN_00565200(*param_1,uVar2,local_20);
    puVar3 = local_20;
  }
  uVar2 = *(unsigned short *)(param_2 + 0x20);
  if ((*(unsigned int *)(param_1[0x1a] + (unsigned int)(uVar2 >> 5) * 4) & 1 << ((unsigned int)uVar2 & 0x1f)) != 0) {
    FUN_00565260(*param_1,uVar2);
  }
  if (puVar3 != 0) {
    FUN_00564c80(*param_1,*(unsigned short *)(param_2 + 0x20),puVar3);
    puVar1 = (unsigned int *)(param_1[0x1a] + (unsigned int)(*(unsigned short *)(param_2 + 0x20) >> 5) * 4);
    *puVar1 = *puVar1 | 1 << ((unsigned int)*(unsigned short *)(param_2 + 0x20) & 0x1f);
  }
  return param_1;
}
