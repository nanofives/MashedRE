// FLAGS: /O2 /Ob1 /G5
/* MATCH-S27 h1: 26.8% (regs 51.2%, permuter best (150 s). Residual not classified beyond register/schedule differences. Class: b. */
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
typedef unsigned char undefined1;
typedef unsigned int uint;
MGLOBAL(undefined1, DAT_00634478, 0x00634478);
MGLOBAL(undefined1, DAT_00634498, 0x00634498);
// MATCH: 0x005b6b00 FUN_005b6b00
void FUN_005b6b00(uint param_1,int *param_2,int *param_3)

{
  uint uVar1;
  int iVar2;
  
  uVar1 = (int)*(short *)(&DAT_00634498 + *param_3 * 2) * ((param_1 & 7) * 2 + 1) >> 3;
  if ((param_1 & 8) != 0) {
    uVar1 = -uVar1;
  }
  *param_2 = iVar2;
  iVar2 = *param_2 + uVar1;
  if (iVar2 < -0x8000) {
    *param_2 = -0x8000;
  }
  else if (0x7fff < iVar2) {
    *param_2 = 0x7fff;
  }
  iVar2 = *param_3 + (int)*(short *)(&DAT_00634478 + param_1 * 2);
  *param_3 = iVar2;
  if (iVar2 >= 0) {
    if (iVar2 >= 0x59) {
      *param_3 = 0x58;
    }
    return;
  }
  *param_3 = 0;
  return;
}
