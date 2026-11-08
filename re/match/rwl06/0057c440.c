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
typedef struct M64 { int v[16]; } M64;
void FUN_004c45f0(M64 *param_1);
void FUN_004c51a0(M64 *param_1, float *param_2, int param_3);
// MATCH: 0x0057c440 FUN_0057c440
int FUN_0057c440(int param_1)
{
  int *piVar1;
  M64 *puVar2;
  float loc[3];

  piVar1 = *(int **)(*(int *)(param_1 + 0x10) + 4);
  loc[0] = -((float *)piVar1)[2];
  loc[1] = -((float *)piVar1)[3];
  loc[2] = -((float *)piVar1)[4];
  puVar2 = *(M64 **)(*(int *)(param_1 + 0x10) + 8);
  *puVar2 = ((M64 *)(**(int **)(*piVar1 + 0x10)))[piVar1[1]];
  FUN_004c51a0(puVar2,loc,1);
  FUN_004c45f0(puVar2);
  return param_1;
}
