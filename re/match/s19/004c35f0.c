// FLAGS: /O2 /GS /Ob1 /G5
/* MATCH-S19 0x004c35f0 (h2 hand form, h6 flags): 100.0%, regs 100.0%, relocheck OK, cxx ok. Four default-or-param stores, RwGlobals pointer first in each address. The 96.7% residual (test eax placement) was the flag set: /G5 fixes it with the source unchanged. */
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
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_007d3ffc, 0x007d3ffc);
typedef int (*fn_t)(void);
int FUN_004c3730(void);
int FUN_004c3670(void);
int FUN_004c3880(void);
int FUN_004c37d0(void);
// MATCH: 0x004c35f0 FUN_004c35f0
int FUN_004c35f0(fn_t param_1, fn_t param_2, fn_t param_3, fn_t param_4)
{
  if (param_1 == 0) {
    param_1 = FUN_004c3730;
  }
  *(fn_t *)(DAT_007d3ff8 + DAT_007d3ffc + 8) = param_1;
  if (param_2 == 0) {
    param_2 = FUN_004c3670;
  }
  *(fn_t *)(DAT_007d3ff8 + DAT_007d3ffc + 0xc) = param_2;
  if (param_3 == 0) {
    param_3 = FUN_004c3880;
  }
  *(fn_t *)(DAT_007d3ff8 + DAT_007d3ffc + 0x10) = param_3;
  if (param_4 == 0) {
    param_4 = FUN_004c37d0;
  }
  *(fn_t *)(DAT_007d3ff8 + DAT_007d3ffc + 0x14) = param_4;
  return 1;
}
