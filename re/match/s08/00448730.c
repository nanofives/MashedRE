// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 0x00448730: 100% (regs 100%) by hand (h9). Raw 57.1%: the global was typed int; it is a float.
   0x005cc320 is the .rdata constant 1.0f (a literal, not a global: relocheck BAD until written as 1.0f). */
/* ---- MATCH-S08 preamble (same text in every re/match/s08 source; not part of any function body) ----
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
MGLOBAL(float, DAT_007f0f7c, 0x007f0f7c);
MGLOBAL(int, DAT_00896498, 0x00896498);
MGLOBAL(int, DAT_007f1a50, 0x007f1a50);
void FUN_00432ad0(void);
void FUN_004430c0(int p0);
void FUN_00444ff0(void);
// MATCH: 0x00448730 FUN_00448730
void FUN_00448730(void)
{
  FUN_00432ad0();
  DAT_007f0f7c = DAT_007f0f7c + 1.0f;
  FUN_004430c0(DAT_00896498);
  if (DAT_007f1a50 != 0) {
    FUN_00444ff0();
  }
}
