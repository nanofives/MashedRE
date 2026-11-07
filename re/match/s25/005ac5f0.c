// FLAGS: /O2 /Ob1 /G5
/* MATCH-S25 0x005ac5f0 (h2): best 100.0% (regs 100.0%), class -. xor of zero-extended bytes masked with 0xfffffffd. relocheck. */
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
int FUN_005adf30(int p0, int p1);
// MATCH: 0x005ac5f0 FUN_005ac5f0

undefined4 FUN_005ac5f0(int *param_1,int *param_2)

{
  int iVar1;
  
  iVar1 = FUN_005adf30(param_1[1],param_2[1]);
  if ((((iVar1 == 0) && (*param_1 == *param_2)) && ((char)param_1[3] == (char)param_2[3])) &&
     ((*(char *)((int)param_1 + 0xd) == *(char *)((int)param_2 + 0xd) &&
      (((*(byte *)(param_1 + 6) ^ *(byte *)(param_2 + 6)) & 0xfffffffd) == 0)))) {
    return 1;
  }
  return 0;
}
