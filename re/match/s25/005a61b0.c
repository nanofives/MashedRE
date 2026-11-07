// FLAGS: /O2 /Ob1 /G5
/* MATCH-S25 0x005a61b0: hand-matched (h1). */
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

/* best 71.4% (regs 71.4%, ops 91.4%): residual = block order. Original places the "mov eax,4; ret" block right after the 0x400 compare;
   ours emits it last (after xor eax,eax). Tried case/return order permutations, split high/low if+switch. Class (b). */
// MATCH: 0x005a61b0 FUN_005a61b0
undefined4 FUN_005a61b0(int param_1)
{
  switch (param_1) {
  case 1:
  case 2:
    return 1;
  case 4:
  case 8:
    return 2;
  case 0x40:
  case 0x80:
    return 8;
  case 0x10:
  case 0x20:
  case 0x400:
  case 0x800:
  case 0x1000:
  case 0x2000:
    return 4;
  case 0x100:
  case 0x200:
    return 0x10;
  }
  return 0;
}
