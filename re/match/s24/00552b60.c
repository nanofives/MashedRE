// FLAGS: /O2 /Ob1 /G5
/* MATCH-S24 0x00552b60: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs24_20261107/results.tsv. */
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
MGLOBAL(undefined4, DAT_00912bdc, 0x00912bdc);
undefined4 FUN_00552a60();
undefined4 FUN_00552c10(void);
undefined4 FUN_005540d0(void);
undefined4 FUN_00555360(void);
undefined4 FUN_00556d20(void);
undefined4 FUN_005571e0(void);
undefined4 FUN_00557250(void);
// MATCH: 0x00552b60 FUN_00552b60

void FUN_00552b60(void)

{
  DAT_00912bdc = 0;
  FUN_00557250();
  FUN_005540d0();
  FUN_00556d20();
  FUN_00555360();
  FUN_005571e0();
  FUN_00552c10();
  FUN_00552a60();
  return;
}
