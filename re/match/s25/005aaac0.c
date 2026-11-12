// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
/* MATCH-S25 h3 0x005aaac0: by hand. Best 77.3% (regs 81.8%). Residual (a): ebx/edi pushes and the param loads are shrink-wrapped past the empty-list early exit in our output, the original pushes ebx/esi/edi first and loads edi/ebx after the je. Same class as the S19/S21/S24 shrink-wrap residual; while/for/do-while spellings tried, no change. */
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
MGLOBAL(undefined4 *, DAT_007dccf0, 0x007dccf0);
// MATCH: 0x005aaac0 FUN_005aaac0
void FUN_005aaac0(int (*param_1)(undefined4 *, undefined4),undefined4 param_2)
{
  undefined4 *p = DAT_007dccf0; if (p != (undefined4 *)&DAT_007dccf0) { do { if ((*param_1)(p - 1,param_2) == 0) return; p = (undefined4 *)*p; } while (p != (undefined4 *)&DAT_007dccf0); }
}
