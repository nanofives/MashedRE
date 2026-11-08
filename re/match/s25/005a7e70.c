// FLAGS: /O2 /Ob1 /G5
/* MATCH-S25 0x005a7e70 (h2): best 100.0% (regs 100.0%), class -. C++ fix: list-head casts. relocheck. */
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
MGLOBAL(undefined4, DAT_007dca7c, 0x007dca7c);
int * FUN_005ade10(int * p0, int p1);
// MATCH: 0x005a7e70 FUN_005a7e70
void FUN_005a7e70(undefined4 param_1)

{
  undefined4 *puVar1;
  
  puVar1 = (undefined4 *)DAT_007dca7c;
  if (puVar1 != (undefined4 *)&DAT_007dca7c) {
    do {
      FUN_005ade10((int *)(puVar1 + -4),param_1);
      puVar1 = (undefined4 *)*puVar1;
    } while (puVar1 != (undefined4 *)&DAT_007dca7c);
  }
  return;
}
