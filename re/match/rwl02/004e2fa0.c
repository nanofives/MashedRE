// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL02 h3 0x004e2fa0: hand-written. */
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
// MATCH: 0x004e2fa0 FUN_004e2fa0
int FUN_004e2fa0(unsigned int *param_1, int param_2)
{
  unsigned int *puVar1;
  int iVar2;

  puVar1 = (unsigned int *)((int)param_1 + 0x27U & 0xffffffe0);
  iVar2 = ((param_2 + (int)param_1 & 0xffffffe0U) - (int)puVar1) + -0x20;
  if (iVar2 < 0x20) {
    return 0;
  }
  *puVar1 = (unsigned int)param_1;
  puVar1[1] = 0;
  puVar1[2] = 0;
  puVar1[4] = 0;
  puVar1[3] = iVar2;
  *param_1 = (unsigned int)puVar1;
  param_1[1] = (unsigned int)puVar1;
  return 1;
}

