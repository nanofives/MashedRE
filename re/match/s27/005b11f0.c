// FLAGS: /O2 /Ob1 /G5
/* MATCH-S27 0x005b11f0: 100.0% (regs 100.0%). Hand: Ghidra data globals FUN_005b1240/50 are functions; declared as void(void) and stored via (undefined4) cast. */
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
void FUN_005b1240(void);
void FUN_005b1250(void);
// MATCH: 0x005b11f0 FUN_005b11f0
void FUN_005b11f0(undefined4 *param_1)
{
  *param_1 = (undefined4)FUN_005b1240;
  param_1[1] = (undefined4)FUN_005b1250;
  param_1[2] = 0;
  param_1[0xf] = 0;
  param_1[0x10] = 0;
  param_1[4] = 0;
  param_1[6] = 0;
  param_1[7] = 0;
  param_1[5] = 0;
  param_1[10] = 0;
  param_1[0xb] = 0;
  param_1[0xd] = 0;
  param_1[0xe] = 0;
  param_1[8] = 0;
  param_1[9] = 0;
  param_1[0xc] = 0;
  param_1[3] = 0;
}
