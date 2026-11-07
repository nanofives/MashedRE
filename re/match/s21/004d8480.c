// FLAGS: /O2 /Ob1 /G5
/* MATCH-S21 0x004d8480 (h4): 85.7% (regs 85.7%, ops 92.9%). Moved from the raw 67.9% by reading param_1[1] into a block local (gives the original's push edi / mov edi / test esi,edi / pop edi) and by testing the set bit first (if (hi & 0x80000000) {0} else {*p}). __int64 forms of either test gave 20%. Residual: ours shrink-wraps push esi past the first (lo != 0) early return, the original pushes it in the prologue and pops on both paths. That is the S19 shrink-wrap class, so no permuter run. Class (a). */
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
MGLOBAL(int, DAT_007d6c5c, 0x007d6c5c);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
// MATCH: 0x004d8480 FUN_004d8480
int *FUN_004d8480(int *param_1)
{
  if ((*(int *)(DAT_007d6c5c + DAT_007d3ff8) == 0) &&
     (*(int *)(DAT_007d6c5c + 4 + DAT_007d3ff8) == (int)0x80000000)) {
    int hi = param_1[1];
    if (hi & 0x80000000) {
      *(int *)(DAT_007d6c5c + DAT_007d3ff8) = 0;
    }
    else {
      *(int *)(DAT_007d6c5c + DAT_007d3ff8) = *param_1;
    }
    *(int *)(DAT_007d6c5c + 4 + DAT_007d3ff8) = param_1[1];
  }
  return param_1;
}
