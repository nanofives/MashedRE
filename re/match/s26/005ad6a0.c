// FLAGS: /O2 /Ob1 /G5
/* MATCH-S26 0x005ad6a0 (h3): 100.0% exact (regs 100.0%). flags were wrong: /O2 /Ob1 /G5 (not /GS) removes the lea esp pad; typed list-node struct for the name field. */
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
typedef unsigned char byte;
MGLOBAL(undefined4, DAT_007dd6a8, 0x007dd6a8);
int FUN_005adf30(byte * p0, byte * p1);
typedef struct Nd6a0 { byte *name; int pad[7]; struct Nd6a0 *next; } Nd6a0;
// MATCH: 0x005ad6a0 FUN_005ad6a0
Nd6a0 * FUN_005ad6a0(byte *param_1)
{
  Nd6a0 **p;
  Nd6a0 *n;
  if (param_1 != 0) {
    for (p = (Nd6a0 **)DAT_007dd6a8; p != (Nd6a0 **)&DAT_007dd6a8; p = (Nd6a0 **)*p) {
      n = (Nd6a0 *)(p - 8);
      if (FUN_005adf30(n->name, param_1) == 0) return n;
    }
  }
  return 0;
}
