// FLAGS: /O2 /Ob1 /G5
/* MATCH-S25 h3 0x005aacf0: by hand. 100%: uVar1 is uint (zero-extended ushort load), plain for loop. */
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
void FUN_005aac20(int * p0, uint p1, int p2);
// MATCH: 0x005aacf0 FUN_005aacf0
void FUN_005aacf0(int *param_1,int param_2)
{
  uint uVar1; uint uVar2;
  if (param_2 == 1) { uVar1 = *(ushort *)(*(int *)(*param_1 + 0x4c) + 0xc); } else { uVar1 = *(ushort *)(*(int *)(*param_1 + 0x4c) + 0xe); }
  for (uVar2 = 0; uVar2 < uVar1; uVar2++) { FUN_005aac20(param_1,uVar2,param_2); }
}
