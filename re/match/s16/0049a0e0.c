// FLAGS: /O2 /GS /Ob1
/* MATCH-S16 0x0049a0e0 (h6): picks DAT_00773968 = 0/1/2 from DAT_00636ad4 and two 16-bit fields of *DAT_00771e58, tail-calls the matching init.
   By hand: 100% exact (regs 100%), relocheck OK. Residual: none. */
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
MGLOBAL(int, DAT_00636ad4, 0x00636ad4);
MGLOBAL(int, DAT_00773968, 0x00773968);
MGLOBAL(int, DAT_00771e58, 0x00771e58);
void FUN_0045b350(void);
void FUN_0049a730(void);
void FUN_0049aae0(void);
// MATCH: 0x0049a0e0 FUN_0049a0e0
void FUN_0049a0e0(void)
{
  if (DAT_00636ad4 != 0) {
    DAT_00773968 = 0;
    FUN_0045b350();
    return;
  }
  if ((*(uint *)(DAT_00771e58 + 0xc4) & 0xffff) >= 0x101 &&
      (*(uint *)(DAT_00771e58 + 0xcc) & 0xffff) >= 0x101) {
    DAT_00773968 = 1;
    FUN_0049aae0();
    return;
  }
  DAT_00773968 = 2;
  FUN_0049a730();
}
