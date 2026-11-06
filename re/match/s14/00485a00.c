// FLAGS: /O2 /GS /Ob1
/* MATCH-S14 h3 0x00485a00: 100.0% (regs 100%). From raw g2s (93.8%): return type void -> undefined4*
   returning param_3 (the caller 0x004850e0 uses the return value; keeping eax live moved the last load to ecx).
   relocheck OK. */
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
// MATCH: 0x00485a00 FUN_00485a00
undefined4 * FUN_00485a00(int param_1,undefined4 param_2,undefined4 *param_3)

{
  undefined4 uVar1;
  
  uVar1 = *(undefined4 *)(param_1 + 4);
  *param_3 = param_2;
  param_3[1] = uVar1;
  param_3[2] = 0;
  param_3[4] = 0;
  param_3[6] = 0;
  param_3[7] = 0;
  param_3[8] = 0;
  param_3[9] = 0;
  param_3[10] = 0;
  param_3[3] = 0x3f800000;
  param_3[5] = 0x3c23d70a;
  *(undefined4 *)(param_3[0x18] + 0x3c) = 0;
  *(undefined4 *)(param_3[0x18] + 0x40) = 0;
  *(undefined4 *)(param_3[0x18] + 0x44) = 0;
  *(undefined4 *)(param_3[0x18] + 0x38) = 1;
  *(undefined4 *)(param_3[0x18] + 0x48) = 0;
  *(undefined4 *)(param_3[0x18] + 0x50) = 1;
  return param_3;
}
