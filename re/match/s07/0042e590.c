// FLAGS: /O2 /GS /Ob1
/* MATCH-S07 h4 0x0042e590: 100% (regs 100%), whole 29 B symbol BYTE-IDENTICAL. Raw converter had no argument and
   no table read (16.7%). By hand: one stack arg, a tail call to 0x0040bb70 with DAT_005f79d8[param_1 +
   DAT_0067f17c * 2] (26 consecutive .rdata string pointers start at 0x005f79d8). Residual: none. */
/* ---- MATCH-S07 preamble (same text in every re/match/s07 source; not part of any function body) ----
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
MGLOBAL(int, DAT_0067f17c, 0x0067f17c);
MGLOBALA(char *, DAT_005f79d8, 26, 0x005f79d8);
void *FUN_0040bb70(char *a1);

// MATCH: 0x0042e590 FUN_0042e590
void *FUN_0042e590(int param_1)
{
  return FUN_0040bb70(DAT_005f79d8[param_1 + DAT_0067f17c * 2]);
}
