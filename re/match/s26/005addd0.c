// FLAGS: /O2 /Ob1 /G5
/* MATCH-S26 h1 0x005addd0 (49 B). Best 88.2% (regs 88.2%), residual class (b). only the null-check layout differs (orig jne body; ret; body vs ours je end); tried if/return, if-block, else forms, permuter 150s. */
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
unsigned int FUN_005ae800(int * p0, int p1);
MGLOBAL(unsigned char, DAT_009146c0, 0x009146c0);
// MATCH: 0x005addd0 FUN_005addd0
void FUN_005addd0(int *param_1,int param_2)
{
  int *n = (int *)FUN_005ae800((int *)&DAT_009146c0,0x30804);
  int t;
  if (n == 0) return;
  n[2] = param_2; t = *param_1;
  n[1] = (int)param_1;
  *n = t;
  *(int **)(*param_1 + 4) = n;
  *param_1 = (int)n;
}
