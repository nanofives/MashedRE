// FLAGS: /O2 /Ob1 /G5
/* MATCH-S25 0x005a7560 (h2): best 100.0% (regs 100.0%), class -. C++ fix: callee prototypes int. relocheck. */
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
typedef unsigned int uint;
typedef unsigned char byte;
MGLOBAL(int, DAT_007dca40, 0x007dca40);
MGLOBAL(int, DAT_007dca3c, 0x007dca3c);
void FUN_005a7460(int p0);
void FUN_005a75b0(int p0);
// MATCH: 0x005a7560 FUN_005a7560
void FUN_005a7560(int param_1)

{
  if ((*(byte *)(param_1 + 0xc) & 1) != 0) {
    FUN_005a7460(param_1);
  }
  FUN_005a75b0(param_1);
  *(undefined4 *)(param_1 + 0x10) = 1;
  *(undefined4 *)(param_1 + 0x24) = 0;
  *(uint *)(param_1 + 0xc) = *(uint *)(param_1 + 0xc) | 2;
  DAT_007dca40 = DAT_007dca40 + 1;
  DAT_007dca3c = DAT_007dca3c + 1;
  return;
}
