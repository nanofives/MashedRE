// FLAGS: /O2 /GS /Ob1
/* MATCH-S15 h2 0x0048f740: 100% (whole obj symbol byte-identical, match.py --bytes). Ghidra's body size stops before the real end, so the per-instruction score reads 66.7%. Raw converter body unchanged. relocheck OK. Residual: none. */
/* ---- MATCH-S15 preamble (same text in every re/match/s15 source; not part of any function body) ----
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
MGLOBAL(undefined4, DAT_00766f40, 0x00766f40);
void FUN_0048f6b0(void);
void FUN_0048f6e0(void);
void FUN_0048f710(void);
// MATCH: 0x0048f740 FUN_0048f740
void FUN_0048f740(void)

{
  undefined4 *puVar1;
  
  FUN_0048f710();
  FUN_0048f6b0();
  FUN_0048f6e0();
  puVar1 = &DAT_00766f40;
  do {
    *puVar1 = 0;
    puVar1 = puVar1 + 0xc;
  } while ((int)puVar1 < 0x769f40);
  return;
}
