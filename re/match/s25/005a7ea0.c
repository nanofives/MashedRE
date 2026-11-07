// FLAGS: /O2 /Ob1 /G5
/* MATCH-S25 0x005a7ea0 (h2): best 100.0% (regs 100.0%), class -. C++ fix: casts and variadic prototype. relocheck. */
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
typedef unsigned char undefined1;
typedef unsigned int undefined4;
typedef unsigned int uint;
typedef unsigned char byte;
MGLOBAL(undefined1, DAT_007dca84, 0x007dca84);
void FUN_004522d0(int p0, ...);
undefined4 FUN_005ae030(undefined4 p0);
void FUN_005ae920(uint * p0, uint p1);
// MATCH: 0x005a7ea0 FUN_005a7ea0
void FUN_005a7ea0(int param_1)

{
  FUN_005ae030(param_1);
  if ((*(byte *)(param_1 + 0x1b) & 1) != 0) {
    FUN_005ae920((uint *)&DAT_007dca84,param_1);
    return;
  }
  if ((*(byte *)(param_1 + 0x1b) & 4) == 0) {
    FUN_004522d0(param_1);
  }
  return;
}
