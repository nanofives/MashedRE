// FLAGS: /O2 /Ob1 /G5
/* 100.0% via match_permute: the two zero stores DAT_007dc958/DAT_007dc95c are ordered 958 first. */
/* MATCH-S25 0x005a5f00: hand-matched (h1). */
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

MGLOBAL(undefined4, DAT_007dc960, 0x007dc960);
MGLOBAL(undefined4, DAT_007dc954, 0x007dc954);
MGLOBAL(undefined4, DAT_007dc94c, 0x007dc94c);
MGLOBAL(undefined4, DAT_007dc95c, 0x007dc95c);
MGLOBAL(undefined4, DAT_007dc958, 0x007dc958);
MGLOBAL(undefined *, PTR_LAB_00632ba8, 0x00632ba8);
uint FUN_005a6190(uint p0);
undefined4 FUN_005a5f60(undefined4 *p0, uint p1, void *p2, void *p3);
void FUN_005a5fd0(void);
void FUN_005a6010(void);
// MATCH: 0x005a5f00 FUN_005a5f00
undefined4 FUN_005a5f00(undefined4 param_1,undefined4 param_2)
{
  int iVar1;
  FUN_005a6190(0x10);
  DAT_007dc960 = param_2;
  DAT_007dc954 = param_1;
  iVar1 = FUN_005a5f60((undefined4 *)&PTR_LAB_00632ba8,0x10,(void *)FUN_005a5fd0,(void *)FUN_005a6010);
  if (iVar1 == 1) {
    DAT_007dc95c = 0;
    DAT_007dc94c = 1;
    DAT_007dc958 = 0;
    return 1;
  }
  return 0;
}
