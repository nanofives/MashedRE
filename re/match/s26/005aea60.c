// FLAGS: /O2 /Ob1 /G5
/* MATCH-S26 0x005aea60 (h3): 41.7% exact, regs 79.2%. Residual class (b) callbacks are real functions now; original loads the global into ecx before the null test and the param in eax, ours swaps them; exact 41.7%, regs 79.2%, ops 95.8%. */
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
typedef unsigned int undefined4;
typedef int code();
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
void FUN_004ccef0(void);
void FUN_004a460d(void);
void FUN_005aead0(void);
void FUN_005aeaf0(void);
// MATCH: 0x005aea60 FUN_005aea60
undefined4 FUN_005aea60(int *param_1)
{
  int *d;
  d = (int *)(DAT_007d3ff8 + 0x108);
  if (param_1 != 0) {
    d[0] = param_1[0];
    d[1] = param_1[1];
    d[2] = param_1[2];
    d[3] = param_1[3];
    return 1;
  }
  d[0] = (int)FUN_004ccef0;
  *(code **)(DAT_007d3ff8 + 0x10c) = (code *)FUN_004a460d;
  *(code **)(DAT_007d3ff8 + 0x110) = (code *)FUN_005aead0;
  *(code **)(DAT_007d3ff8 + 0x114) = (code *)FUN_005aeaf0;
  return 1;
}
