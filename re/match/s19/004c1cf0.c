// FLAGS: /O2 /GS /Ob1 /G5
/* MATCH-S19 0x004c1cf0 (h2 hand form, h6 flags): 100.0%, regs 100.0%, relocheck OK, cxx ok. Typed fn pointer through RwGlobals+0x11c. The 94.1% residual (push esi before the mov edx load) was the flag set: /G5 fixes it with the source unchanged. */
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
MGLOBAL(int, DAT_007d3ec0, 0x007d3ec0);
MGLOBALA(char, DAT_00617f98, 1, 0x00617f98);
typedef int (*fp2_t)(void *, int);
void FUN_004c0790(int p0);
int FUN_004d8060(char *p0, int p1);
// MATCH: 0x004c1cf0 FUN_004c1cf0
int FUN_004c1cf0(int param_1)
{
  FUN_004d8060(DAT_00617f98, param_1);
  FUN_004c0790(param_1);
  (*(fp2_t *)(DAT_007d3ff8 + 0x11c))(*(void **)(DAT_007d3ff8 + DAT_007d3ec0), param_1);
  return 1;
}
