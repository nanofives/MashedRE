// FLAGS: /O2 /GS /Ob1
/* MATCH-S11 0x004721b0 (h7): 100%. Raw converter took the callback FUN_00472150 as a byte global;
   declared it as a function. Residual: none. */
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
void FUN_004b52c0(int p0, unsigned int p1, int p2);
int FUN_004e89a0(int p0, void *p1, undefined4 p2);
void *FUN_00472150(void *p0, void *p1);
// MATCH: 0x004721b0 FUN_004721b0
int FUN_004721b0(int param_1,undefined4 param_2)
{
  int uVar1;
  uVar1 = *(int *)(param_1 + 0x18);
  FUN_004b52c0(uVar1,0x40,1);
  FUN_004e89a0(uVar1,(void *)FUN_00472150,param_2);
  return param_1;
}
