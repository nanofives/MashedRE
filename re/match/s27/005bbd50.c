// FLAGS: /O2 /GS /Ob1
/* MATCH-S27 h1: 45.7% (regs 48.6%, permuter best (150 s). Residual not classified beyond register/schedule differences. Class: b. */
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
typedef unsigned int uint;
#ifdef __cplusplus
typedef int code(...);
#else
typedef int code();
#endif
// MATCH: 0x005bbd50 FUN_005bbd50
uint FUN_005bbd50(int *param_1,int param_2)

{
  int iVar1;
  uint *puVar2;
  undefined4 local_28;
  uint local_24 [4];
  undefined4 local_14;
  
  puVar2 = local_24;
  for (iVar1 = 9; iVar1 != 0; iVar1 = iVar1 + -1) {
    puVar2 = puVar2 + 1;
    *puVar2 = 0;
  }
  local_24[2] = 0;
  local_28 = 0;
  local_24[1] = (uint)(-(param_2) & 0x10U | 1);
  local_14 = 0;
  puVar2 = local_24;
  iVar1 = (**(code **)(*param_1 + 0xc))(param_1,puVar2,&local_28,0);
  local_24[0] = 0x24;
  return (uint)puVar2 & (iVar1 < 0) - 1;
}
