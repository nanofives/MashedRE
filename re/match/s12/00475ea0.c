// FLAGS: /O2 /GS /Ob1
/* MATCH-S12 0x00475ea0: 100% (regs 100%), relocheck OK. Raw g2s body was 95.2%: its loop ends were the plain
   literals 0x691698 / 0x692498, so match.py left the original's `push 0x692498` unmasked. The globals are now
   0x80-byte arrays and the loop ends are `DAT_x + 0x80` (relocated), compared signed as in the original (`jl`).
   Residual: none. */
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
typedef unsigned char undefined;
typedef unsigned char undefined1;
MGLOBALA(undefined1, DAT_00691618, 0x80, 0x00691618);
MGLOBALA(undefined1, DAT_00692418, 0x80, 0x00692418);
MGLOBAL(undefined1, DAT_00692498, 0x00692498);
void FUN_004768c0(undefined *p0);
// MATCH: 0x00475ea0 FUN_00475ea0
void FUN_00475ea0(void)

{
  undefined *puVar1;
  
  puVar1 = DAT_00691618;
  do {
    FUN_004768c0(puVar1);
    puVar1 = puVar1 + 0x40;
  } while ((int)puVar1 < (int)(DAT_00691618 + 0x80));
  puVar1 = DAT_00692418;
  do {
    FUN_004768c0(puVar1);
    puVar1 = puVar1 + 0x40;
  } while ((int)puVar1 < (int)(DAT_00692418 + 0x80));
  FUN_004768c0(&DAT_00692498);
  return;
}
