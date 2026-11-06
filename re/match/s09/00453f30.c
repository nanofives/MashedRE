// FLAGS: /O2 /GS /Ob1
/* MATCH-S09 0x00453f30 (h3): 100% (whole obj symbol byte-identical, 33 B). Ghidra's size (24 B) stops at the
   loop label, so the instruction score reads 55.6%; match.py --bytes reports BYTE-IDENTICAL. Raw g2s body. */
/* ---- MATCH-S09 preamble (same text in every re/match/s09 source; not part of any function body) ----
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
MGLOBAL(undefined4, DAT_006870b4, 0x006870b4);
// MATCH: 0x00453f30 FUN_00453f30
void FUN_00453f30(void)

{
  undefined4 *puVar1;
  
  puVar1 = &DAT_006870b4;
  do {
    *puVar1 = 0xffffffff;
    puVar1 = puVar1 + 9;
  } while ((int)puVar1 < 0x687ec4);
  return;
}
