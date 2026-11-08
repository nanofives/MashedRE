// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL06 0x00568c40: hand-matched (h2). 100.0% regs 100.0%. Four masked temporaries summed in order a(+0x14) b(+0xc) c(+8 *0x4c) d(+8 *4). */
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
// MATCH: 0x00568c40 FUN_00568c40
int FUN_00568c40(int param_1)
{
  int a = (*(int *)(param_1 + 0x14) * 0x10 + 0xf) & 0xfffffff0;
  int b = (*(int *)(param_1 + 0xc) * 4 + 0xf) & 0xfffffff0;
  int c = (*(int *)(param_1 + 8) * 0x4c + 0xf) & 0xfffffff0;
  int d = (*(int *)(param_1 + 8) * 4 + 0xf) & 0xfffffff0;
  return a + b + c + d;
}
