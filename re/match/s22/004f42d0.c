// FLAGS: /O2 /Ob1 /G5
/* MATCH-S22 0x004f42d0 (h3, by hand): 100% (regs 100%) under /O2 /Ob1 /G5. Raw converter 75.0% (goto form, FUN_004f4310 as MGLOBAL).
   Typed callback pointer, FUN_004f4310 declared as a function, result as a && ternary. */
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
typedef void (*S22VoidFunc)(void);
MGLOBAL(S22VoidFunc, DAT_007d7294, 0x007d7294);
void FUN_004cbc80(S22VoidFunc p0);
S22VoidFunc FUN_004cbc90(void);
int FUN_004f4510(void);
int FUN_004f8580(void);
void FUN_004f4310(void);
// MATCH: 0x004f42d0 FUN_004f42d0
int FUN_004f42d0(void)
{
  int result;

  result = (FUN_004f4510() && FUN_004f8580()) ? 1 : 0;
  DAT_007d7294 = FUN_004cbc90();
  FUN_004cbc80(FUN_004f4310);
  return result;
}
