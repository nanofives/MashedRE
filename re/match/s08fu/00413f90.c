// FLAGS: /O2 /GS /Ob1
/* MATCH-S08-FU 0x00413f90 (TimerGetBasePtr): re/match/s03/raw_00413f90.c (raw converter, 100%) in the MATCH-S08 port form:
   the S08 preamble, and the global as an MGLOBAL so relocheck checks it and the port binds it to its original address.
   The function returns the address of DAT_005f2b10 (B8 10 2B 5F 00 / C3). */
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
MGLOBAL(undefined1, DAT_005f2b10, 0x005f2b10);
// MATCH: 0x00413f90 FUN_00413f90
undefined * FUN_00413f90(void)
{
  return &DAT_005f2b10;
}
