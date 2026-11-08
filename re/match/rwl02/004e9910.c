// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL02 h1 0x004e9910: best 88.0% (regs 88.0%), residual class a. same residual as 0x004e5ef0 (early-return layout), 88. */
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

MGLOBAL(undefined4, DAT_007d7270, 0x007d7270);
MGLOBAL(undefined4, DAT_007d727c, 0x007d727c);
int FUN_004cc790(undefined4 p0, undefined4 *p1, int p2);
// MATCH: 0x004e9910 FUN_004e9910
undefined4 FUN_004e9910(undefined4 param_1,int param_2)
{
  int iVar1;
  iVar1 = FUN_004cc790(param_1,&DAT_007d727c,4);
  if (iVar1 == 0) {
    return iVar1;
  }
  if (param_2 == 8) {
    iVar1 = FUN_004cc790(param_1,&DAT_007d7270,4);
    if (iVar1 == 0) {
      return iVar1;
    }
  }
  return param_1;
}
